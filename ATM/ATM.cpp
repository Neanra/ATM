#include "ATM.h"
//#include "mainwindow.h"

#include <cassert>

// Ejection messages
// No errors
const QString ATM::EJECT_SUCCESS     = "Thank you for using our ATM! Good bye.";
// No connection to bank DB
const QString ATM::EJECT_ERR_CONN    = "Sorry! ATM failed to connect to bank. Please try again later.";
// Invalid or inexistant
const QString ATM::EJECT_ERR_READ    = "Failed to read the card. It may be damaged. Please contact our bank for a replacement.";
// Unknown error
const QString ATM::EJECT_ERR_FAIL    = "Sorry! An error occured during processing. Please try again.";

// TODO: Move everything related to DB to a separate class
//==========
// Templates for common SQL queries
const QString ATM::SELECT_CARD_BY_NUMBER = \
    "SELECT cards.active, cards.pin, cards.balance, clients.last_name, clients.gender_male \
    FROM cards INNER JOIN clients ON cards.client_id=clients.id \
    WHERE cards.card_number=\"%1\"";
//const QString DEACTIVATE_CARD = ...;
//const QString UPDATE_CARD_BALANCE = ...;
//etc.

ATM::ATM (IDisplay* display, IDisplay* printer):
    _state(POWER_OFF),
    _display(display),
    _printer(printer),
    _database(QSqlDatabase::addDatabase(BANK_DATABASE_DRIVER)),
    _current_card(NULL)
{
    if(display)
    {
        _display->connect(*this);
    }
    assert(_database.isValid() && "FATAL: Invalid database driver "BANK_DATABASE_DRIVER"!!!");
    _database.setDatabaseName(BANK_DATABASE_NAME);
}

ATM::~ATM()
{
    if(_state != POWER_OFF)
    {
        powerOff();
    }
    _display->disconnect();
    // Do NOT delete _display! We are not responsible for its cleanup.
}

void ATM::processInput(QString input)
{
    // Interpret input according to current state
    switch(_state)
    {
        case POWER_OFF:
        //case LOADING:
            // Ignore
            //MainWindow::disableEnterBtn();
            //MainWindow::disableInput();
            //MainWindow::disableKeyboard();
            //MainWindow::disablePrinter();
            break;
        case NO_CARD:
            _display->showText("NO_CARD");
            // User has stuck a card into us, let's process it
            //MainWindow::enableInput();
            //MainWindow::enableEnterBtn();
            onCardInserted(input);
            break;
        case PENDING_PIN:
            _display->showText("PENDING_PIN");
            // TODO: Check entered pin against our card info
            //MainWindow::disableInput();
            //MainWindow::enableKeyboard();
            onPinEntered(input);
            break;
        case TOP_MENU:
            // TODO: Show all services user can choose
            _display->showText("NOT IMPLEMENTED");
            break;
        default:
            // WAT!?
            assert(false && "FATAL: Unhandled ATM state in processInput()!!!");
            break;
    }
}

void ATM::onCardInserted(QString cardNumber)
{
    // TODO: Validate card number?
    //==========
    // TODO: DB connection logic should be externalized.
    //==========
    // Between the moment when card is inserted and the ejection
    // there might be much communication with DB.
    // Therefore connection is established on insertion and released on ejection.
    if(_database.open())
    {
        displayText("Please wait...");
        // Let's load card data from the DB (if there is such a card)
        QSqlQuery query(SELECT_CARD_BY_NUMBER.arg(cardNumber), _database);
        if(!query.isActive())
        {
            // TODO: Panic. Failed to execute query.
#ifndef NDEBUG
            onCardEjected(query.lastError().text());
#else
            onCardEjected(EJECT_ERR_FAIL);
#endif
            return;
        }

        // Attempt to retreive the first (and only) entry
        if(!query.first())
        {
            // There is no such card.
            onCardEjected(EJECT_ERR_READ);
            return;
        }

        bool cardsActive = query.value(0).toBool();  // cards.active
        if(!cardsActive)
        {
            // Card exists but is not active.
            // TODO: Seize, not eject
            onCardEjected("This card has been blocked by owner and will be seized.");
            return;
        }

        // So far so good.
        // Such card exists and is active. Time to initialize _current_card with values from DB.

        // TODO: Dispose of temporary variables: they were added for clarity.
        QString cardsPin = query.value(1).toString();           // cards.pin
        QString clientsLastName = query.value(3).toString();    // clients.last_name
        bool clientsGenderMale = query.value(4).toBool();       // clients.gender_male

        _current_card = new Card();
        _current_card->_card_number = cardNumber;
        _current_card->_pin = cardsPin;
        _current_card->_owner_last_name = clientsLastName;
        _current_card->_owner_gender_male = clientsGenderMale;

        _state = PENDING_PIN;

        //TODO: how to move this Pin request to void ATM::onPinEntered(QString cardsPin)???
        // Ask for PIN
        const QString PIN_REQUEST_TEMPLATE = "Hello, %1. %2! Please enter your PIN. \n";
        displayText(PIN_REQUEST_TEMPLATE.arg(_current_card->_owner_gender_male ? "Mr" : "Ms",
                                             _current_card->_owner_last_name));

    }
    else
    {
        //Connection failed
#ifndef NDEBUG
        displayText("ERROR: Failed to open a connection to "BANK_DATABASE_NAME);
        // TODO: Output what went wrong exactly using QSqlDatabase::lastError()
#else
        displayText(EJECT_ERR_CONN);
#endif
    }
}

void ATM::onPinEntered(QString cardsPin)
{
    if(_database.open())
    {
        QSqlQuery query(SELECT_CARD_BY_NUMBER.arg( _current_card->_card_number), _database);
        if(!query.isActive())
        {
            // TODO: Panic. Failed to execute query.
#ifndef NDEBUG
            onCardEjected(query.lastError().text());
#else
            onCardEjected(EJECT_ERR_FAIL);
#endif
            return;
        }

        // Attempt to retreive the first (and only) entry
        if(!query.first())
        {
            // There is no such card.
            onCardEjected(EJECT_ERR_READ);
            return;
        }


        QString actualCardsPin = query.value(1).toString();           // cards.pin


        // ATM answer on PIN entering
        if (actualCardsPin == cardsPin)
        {
            displayText ("Thank you!");
            //TODO: show user top menu
            _state = TOP_MENU;
        }
        else
        {
            // TODO: give card back
             const QString PIN_REQUEST_TEMPLATE = "You entered wrong PIN for card %1. Take yor card back and try again.";
             displayText(PIN_REQUEST_TEMPLATE.arg(_current_card->_card_number));

             _state = NO_CARD;
        }

    }
    else
    {
        //Connection failed
#ifndef NDEBUG
        displayText("ERROR: Failed to open a connection to "BANK_DATABASE_NAME);
        // TODO: Output what went wrong exactly using QSqlDatabase::lastError()
#else
        displayText(EJECT_ERR_CONN);
#endif
    }
}

void ATM::onCardEjected(QString message)
{
    displayText(message);
    // Releasing DB connection.
    _database.close();
    if(_current_card)
    {
        delete _current_card;
        _current_card = NULL;
    }
    _state = NO_CARD;
}

void ATM::powerOn()
{
    // TODO: Add any initialization logic here.
    _state = NO_CARD;
    displayText("Please insert your card");
    _display->enableInput();
}


void ATM::powerOff()
{
    // TODO: Add any finalization logic here.
    _display->showText("(no power)");
    if(_current_card)
    {
        delete _current_card;
        _current_card = NULL;
    }
    if(_database.isOpen())
    {
        _database.close();
    }
    _state = POWER_OFF;
    _display->disableInput();
}

void ATM::showBalance(QString cardNumber)
{
    if(_database.open())
    {
        QSqlQuery query(SELECT_CARD_BY_NUMBER.arg(cardNumber), _database);
        if(!query.isActive())
        {
            // TODO: Panic. Failed to execute query.
#ifndef NDEBUG
            onCardEjected(query.lastError().text());
#else
            onCardEjected(EJECT_ERR_FAIL);
#endif
            return;
        }

        // TODO: Add any finalization logic here.
        _display->showText("Your balance: ");

        int cardBalance = query.value(2).toInt(); // cards.balance
        _current_card->_balance = cardBalance;
        displayText(QString::number(cardBalance));
    }
}
