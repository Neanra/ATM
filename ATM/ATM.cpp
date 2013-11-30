#include "ATM.h"

#include <cassert>

const size_t ATM::MAX_PIN_ERRORS = 3;

// Ejection messages
// No errors
const QString ATM::EJECT_SUCCESS     = "Thank you for using our ATM! Good bye.";
// No connection to bank DB
const QString ATM::EJECT_ERR_CONN    = "Sorry! ATM failed to connect to bank. Please try again later.";
// Invalid or inexistant
const QString ATM::EJECT_ERR_READ    = "Failed to read the card. It may be damaged. Please contact our bank for a replacement.";
// Unknown error
const QString ATM::EJECT_ERR_FAIL    = "Sorry! An error occured during processing. Please try again.";

// Seizure messages
// Invalid PIN
const QString ATM::SEIZE_INVALID_PIN    = "Invalid PIN! Card will be seized. Please contact our bank's office, if you have any questions.";
// Inactive or blocked card
const QString ATM::SEIZE_INACTIVE       = "This card has been blocked or is not yet activated. Please contact our bank's office for details.";

// TODO: Move everything related to DB to a separate class
//==========
// Templates for common SQL queries
const QString ATM::SELECT_CARD_BY_NUMBER = \
    "SELECT cards.active, cards.pin, cards.balance, clients.last_name, clients.gender_male \
    FROM cards INNER JOIN clients ON cards.client_id=clients.id \
    WHERE cards.card_number=\"%1\"";
const QString ATM::DEACTIVATE_CARD = "UPDATE cards SET active=0 WHERE card_number=\"%1\"";
//const QString ATM::UPDATE_CARD_BALANCE = ...;
//etc.

ATM::ATM(ITerminal* terminal):
    // TODO: Uncomment the following line when C++11 support is added
    //ATM(terminal, terminal, terminal)
    // ...then remove everything else in this constructor
    _state(POWER_OFF),
    _display(terminal),
    _keyboard(terminal),
    _printer(terminal),
    _database(QSqlDatabase::addDatabase(BANK_DATABASE_DRIVER)),
    _current_card(NULL),
    _pin_attempts_left(MAX_PIN_ERRORS)
{
    if(terminal)
    {
        terminal->connect(*this);
    }
    assert(_database.isValid() && "FATAL: Invalid database driver "BANK_DATABASE_DRIVER"!!!");
    _database.setDatabaseName(BANK_DATABASE_NAME);
}

ATM::ATM(IDisplay* display, IKeyboard* keyboard, IPrinter* printer):
    _state(POWER_OFF),
    _display(display),
    _keyboard(keyboard),
    _printer(printer),
    _database(QSqlDatabase::addDatabase(BANK_DATABASE_DRIVER)),
    _current_card(NULL),
    _pin_attempts_left(MAX_PIN_ERRORS)
{
    if(_display)
    {
        _display->connect(*this);
    }
    if(_keyboard)
    {
        _keyboard->connect(*this);
    }
    if(_printer)
    {
        _printer->connect(*this);
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
    _keyboard->disconnect();
    _printer->disconnect();
    // Do NOT delete connectable modules! We are not responsible for their cleanup.
}

void ATM::processInput(QString input)
{
    try
    {
        // Interpret input according to current state
        switch(_state)
        {
            case POWER_OFF:
            //case LOADING:
                break;
            case NO_CARD:
                // User has stuck a card into us, let's process it
                onCardInserted(input);
                break;
            case PENDING_PIN:
                onPinEntered(input);
                break;
            case TOP_MENU:
                // TODO: Show all services user can choose
                throw NotImplementedException();
                break;
            default:
                // WAT!?
                assert(false && "FATAL: Unhandled ATM state in processInput()!!!");
                break;
        }
    }
    catch(const DatabaseConnectionFailedException&)   // Failed to connect to the database
    {
#ifndef NDEBUG
        onCardEjected(
            QString("ERROR: Failed to open a connection to "BANK_DATABASE_NAME": %1").arg(_database.lastError().text())
        );
#else
        onCardEjected(EJECT_ERR_CONN);
#endif
    }
    catch(const DatabaseQueryFailedException& e)      // Failed to execute a query
    {
        // TODO: Log what error and on what query has happened.
        if(e.mustSeizeCard())
        {
            onCardSeized(SEIZE_INVALID_PIN);
        }
        else
        {
            onCardEjected(EJECT_ERR_CONN);
        }
    }
    catch(const FailedToReadCardException&)         // Invalid card inserted
    {
        onCardEjected(EJECT_ERR_READ);
    }
    catch(const CardInactiveException&)             // Inserted card is (or has become) blocked or inactive
    {
        onCardSeized(SEIZE_INACTIVE);
    }
    catch(const InternalErrorException& e)          // Other error
    {
        onCardEjected(QString(e.what()));
    }
}

void ATM::onCardInserted(QString cardNumber)
{
    // TODO: Validate card number, potentially raising FailedToReadCardException?
    //==========
    // TODO: DB connection logic should be externalized.
    //==========
    // Between the moment when card is inserted and the ejection
    // there might be much communication with DB.
    // Therefore connection is established on insertion and released on ejection.
    displayText("Please wait...");

    if(!_database.open())
    {
        throw DatabaseConnectionFailedException();
    }

    updateCardData(cardNumber);

    _state = PENDING_PIN;

    // Ask for PIN
    requestPin();
}

void ATM::onPinEntered(QString cardsPin)
{
    // If pin is valid
    if(cardsPin == _current_card->_pin)
    {
        _state = TOP_MENU;
        displayTopMenu();
    }
    else
    {
        if(--_pin_attempts_left == 0)
        {
            deactivateCard();
            onCardSeized(SEIZE_INVALID_PIN);
        }
        else
        {
            requestPin(true);
        }
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
    _pin_attempts_left = MAX_PIN_ERRORS;
    _state = NO_CARD;
}

void ATM::onCardSeized(QString message)
{
    // TODO: Place any additional seizure logic here
    onCardEjected(message);
}

void ATM::powerOn()
{
    // TODO: Add any initialization logic here.
    _state = NO_CARD;
    displayText("Please insert your card");
    _keyboard->enableInput();
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
    _keyboard->disableInput();
}

// Query DB for current card data by its number
void ATM::updateCardData(QString cardNumber)
{
    assert(_database.isOpen() && "FATAL: Unexpected call to updateCardData()!!!");
    // Let's load card data from the DB (if there is such a card)
    QSqlQuery query(SELECT_CARD_BY_NUMBER.arg(cardNumber), _database);
    if(!query.isActive())
    {
        throw DatabaseQueryFailedException();
    }

    // Attempt to retreive the first (and only) entry
    if(!query.first())
    {
        // There is no such card.
        throw FailedToReadCardException();
    }

    if(!_current_card)
    {
        _current_card = new Card();
    }

    _current_card->_card_number = cardNumber;
    _current_card->_is_active = query.value(0).toBool();  // cards.active
    if(!_current_card->_is_active)
    {
        // Card exists but is not active.
        throw CardInactiveException();
    }

    // So far so good.
    // Such card exists and is active. Time to initialize _current_card with values from DB.

    _current_card->_pin = query.value(1).toString();                // cards.pin
    _current_card->_balance = query.value(2).toDouble();            // cards.balance
    _current_card->_owner_last_name = query.value(3).toString();    // clients.last_name
    _current_card->_owner_gender_male = query.value(4).toBool();    // clients.gender_male
}

void ATM::deactivateCard()
{
    assert(_database.isOpen() && "FATAL: Unexpected call to deactivateCard()!!!");
    // Deactivate card
    QSqlQuery query(DEACTIVATE_CARD.arg(_current_card->_card_number), _database);
    if(!query.isActive())
    {
        // Failed to execute deactivation query. Seize the card anyway.
        throw DatabaseQueryFailedException(true);
    }
}

void ATM::showBalance()
{
    updateCardData();
    displayText(QString("Your balance: %1").arg(QString::number(_current_card->_balance)));
}

// Print text to display unless there is no display
void ATM::displayText(QString text)
{
    if(_display)
    {
        _display->showText(text);
    }
}

// Draw main menu on the screen
//==========
// TODO: Generalize this function so that it could draw any menu
// depending on passed parameters.
void ATM::displayTopMenu()
{
    if(!_display)
    {
        // Display where?
        return;
    }
    // TODO: Draw menu.
    throw NotImplementedException("Main menu is not yet implemented. [Card is ejected]");
}

void ATM::printText(QString text)
{
    if(_printer)
    {
        _printer->printText(text);
    }
}

// Ask for PIN
void ATM::requestPin(bool afterError)
{
    if(!afterError)
    {
        const QString PIN_REQUEST_TEMPLATE = "Hello, %1. %2! Please enter your PIN. \n";
        displayText(PIN_REQUEST_TEMPLATE.arg(_current_card->_owner_gender_male ? "Mr" : "Ms",
                                         _current_card->_owner_last_name));
    }
    else
    {
        displayText(
            QString("Invalid PIN! You have %1 attempts left. Please try again.").arg(QString::number(_pin_attempts_left))
        );
    }
}
