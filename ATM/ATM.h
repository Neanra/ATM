#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>
#include <QString>
#include <QtSql>
#include <exception>
#include <cassert>


// TODO: Move DB configuration data to a better place
#define BANK_DATABASE_DRIVER "QSQLITE"
#define BANK_DATABASE_NAME "bank.db"

using namespace std;

// There may be many different options as for configuration of an ATM, such as:
// - a window to be used as a display and input source, a window to show printer output
// - a window for display, USB printer driver to print checks, hardware keyboard driver
// - a window that combines all the necessary functions
// - etc.
// Therefore, the following interfaces are provided that outline requirements
// for every possible ATM module.
class IDisplay;     // Window that serves as ATM's display must implement this interface
class IKeyboard;    // Entities responsible for user input must implement this interface
class IPrinter;     // Entities that provide or emulate printing capabilities must implement this interface

// Terminal combines all the modules generaly found in ATMs
class ITerminal;

class ATM
{
public:
    // Interface for everything that can be connected to an ATM: displays, printers, fingerprints scanners, etc.
    class IConnectableModule;
    class InputContainer;
    //class CardState;

    // Construct ATM from an all-inclusive terminal
    explicit ATM(ITerminal* terminal);
    // Construct ATM from any combination of modules (NULL means module is not available)
    ATM(IDisplay* display, IKeyboard* keyboard, IPrinter* printer);
    virtual ~ATM();

    // Accept input from user
    void processInput(QString input);

    void powerOn();
    void powerOff();

    void showBalance();
    void printBalance();

    inline bool isOn()
    {
        return (_state != POWER_OFF);
    }

private:
    static const size_t MAX_PIN_ERRORS;     // 3

    // Ejection messages
    static const QString EJECT_SUCCESS;
    static const QString EJECT_ERR_CONN;    // No connection to bank DB
    static const QString EJECT_ERR_READ;    // Invalid or inexistant card number
    static const QString EJECT_ERR_FAIL;    // Other error

    // Seizure messages
    static const QString SEIZE_INVALID_PIN; // You have stolen it, haven't you?
    static const QString SEIZE_INACTIVE;    // The card is not yet or no longer active

    void onCardInserted(QString cardNumber);
    void onPinEntered(QString cardsPin);
    void topMenu(QString selectedService);

    void showBalanceOptions();
    // TODO: String parameters here are just begging to be replaced with numerical codes.
    void onCardEjected(QString message = EJECT_SUCCESS);
    void onCardSeized(QString message);

    // Perform common operations on card seizure/ejection
    // (such as DB disconnection)
    void finalizeCard();

    // Query DB for currently inserted card's data
    inline void updateCardData()
    {
        updateCardData(_current_card->_card_number);
    }
    bool cardExists(QString number);
    // Query DB for card data by its number
    void updateCardData(QString cardNumber);
    // Mark currently inserted card as inactive in DB
    void deactivateCard();

    void requestPin(bool afterError = false);

    // Show text on display unless there is no display
    void displayText(QString text);
    // Draw main menu on the screen
    void displayTopMenu();
    // TODO: Something along the lines of...
    // enum MenuType {MENU_MAIN = 0, MENU_CURRENCY = 1, MENU_AMOUNT_SELECTION = 2, ...};
    // void displayMenu(MenuType menu = MENU_MAIN);

    // Print text to printer, if it is connected
    void printText(QString text);

    // TODO: Separate from this class entirely?
    void executeQuery(QString query);     // throws DatabaseQueryFailedException

private:
    // ATM errors
    class InternalErrorException;               // Base class for all errors ATM may encounter
    class DatabaseConnectionFailedException;    // Failed to open connection to the database
    class DatabaseQueryFailedException;         // Failed to execute a query to the database
    class FailedToReadCardException;            // Invalid card number read
    class CardInactiveException;                // Inserted card is blocked or inactive,
                                                // therefore no operations can be performed with it
    class NotImplementedException;              // Feature is not yet implemented

    struct Card
    {
        QString _card_number;
        bool _is_active;
        QString _pin;
        QString _owner_last_name;
        bool _owner_gender_male;    // For politeness :)
        double _balance;
        //etc.
    };

    Card* _current_card;

    enum ATMState {POWER_OFF = 0, NO_CARD = 1, PENDING_PIN = 2, TOP_MENU = 3};
    enum MenuState {TOP = 0, SHOW_BALANCE_METHOD = 1, DISPLAY_BALANCE = 2,
                  PRINT_BALANCE = 3, WITHDRAW = 4, TRANSFER = 5, MOBILE = 6};
    enum TransactionResult
    {
        TRANS_SUCCESS           = 0,
        TRANS_FAIL              = 1,
        TRANS_NOT_ENOUGH_FUNDS  = 2,
        TRANS_INVALID_RECEIVER  = 3
    };

    ATMState _state;
    MenuState _menu_state;
    IDisplay* _display;
    IPrinter* _printer;
    IKeyboard* _keyboard;

    // TODO: Move everything related to DB to a separate class
    //==========
    // Templates for common SQL queries
    static const QString SELECT_CARD_BY_NUMBER;
    static const QString DEACTIVATE_CARD;
    static const QString WITHDRAW_FUNDS;
    static const QString UPLOAD_FUNDS;
    //etc.

    QSqlDatabase _database; // Connection to DB

    size_t _pin_attempts_left;

private:
    ATM::TransactionResult withdrawFunds(double amount);
    ATM::TransactionResult transferFunds(QString targetCardNumber, double amount);
};

// Interface for everything that can be connected to ATM: displays, printers, fingerprints scanners, etc.
class ATM::IConnectableModule
{
public:
    // Called when ATM establishes connection to its module
    virtual void connect(const ATM& atm) = 0;
    // Called when ATM disconnects (e.g. when it is destroyed)
    virtual void disconnect() = 0;
};

// Window that serves as ATM's display must implement this interface
class IDisplay : public virtual ATM::IConnectableModule
{
public:
    // Output text to the display
    virtual void showText(QString text) = 0;
    virtual void showCardState(QString text) = 0;
    virtual void appendText(QString text) = 0;
};

// Entities responsible for user input must implement this interface
class IKeyboard : public virtual ATM::IConnectableModule
{
public:
    virtual void enableInput() = 0;
    virtual void disableInput() = 0;
    virtual void enableKeyboard() = 0;
    virtual void disableKeyboard() = 0;
};

// Entities that provide or emulate printing capabilities must implement this interface
class IPrinter : public virtual ATM::IConnectableModule
{
public:
    // Output text to the printer
    virtual void printText(QString text) = 0;
    virtual void enablePrinter() = 0;
    virtual void disablePrinter() = 0;
};

// Terminal combines all the modules generaly found in ATMs
class ITerminal : public virtual IDisplay, public virtual IKeyboard, public virtual IPrinter
{};

// Internal errors

// Base class for all internal errors of the ATM
class ATM::InternalErrorException : public std::exception
{
public:
    explicit InternalErrorException(QString message = "Unknown internal error."):
        std::exception(message.toStdString().c_str())
    {}
};

// Failed to open connection to the database.
class ATM::DatabaseConnectionFailedException : public ATM::InternalErrorException
{
public:
    explicit DatabaseConnectionFailedException(QString message = "Failed to connect to bank database."):
        InternalErrorException(message)
    {}
};

// Failed to execute a query to the database.
class ATM::DatabaseQueryFailedException : public ATM::InternalErrorException
{
private:
    bool _seize_card;
public:
    explicit DatabaseQueryFailedException(bool seizeCard = false, QString message = "Query execution failed."):
        InternalErrorException(message), _seize_card(seizeCard)
    {}
    inline bool mustSeizeCard() const
    {
        return _seize_card;
    }
    inline void setSeizeCard(bool seize)
    {
        _seize_card = seize;
    }
};

// Inserted card number is invalid
class ATM::FailedToReadCardException : public ATM::InternalErrorException
{
public:
    explicit FailedToReadCardException(QString message = "Failed to read card."):
        ATM::InternalErrorException(message)
    {}
};

// Inserted card is blocked or not activated, therefore no operations can be performed with it
class ATM::CardInactiveException : public ATM::InternalErrorException
{
public:
    explicit CardInactiveException(QString message = "Inserted card is inactive."):
        InternalErrorException(message)
    {}
};

// Feature is not yet implemented
class ATM::NotImplementedException : public ATM::InternalErrorException
{
public:
    explicit NotImplementedException(QString message = "This feature is not yet implemented."):
        InternalErrorException(message)
    {}
};

// ATM keyboard (so far only a PIN container)
//==========

class ATM::InputContainer {
private:
    static const int _size_of_pin = 4;
    static const int _size_of_menu_option = 1;
    QString _pin;
    ATM& _atm;
public:
    explicit InputContainer(ATM& atm):_atm(atm),_pin(){}

    ~InputContainer(){}

    bool isPin() const {
        // This check is enough, since characters are checked for validity on insertion
        return _pin.size() == _size_of_pin;
    }

    bool isPinEmpty() const {
        return _pin.size()==0;
    }

    void acceptInput(QChar input) {
        if (_atm._state == ATM::PENDING_PIN)
        {
            addNextToArray(input);
            _atm._display->appendText("*");
        }
        else if(_atm._state == ATM::TOP_MENU)
        {
           _atm.processInput(input);
        }
        else
        {
            assert(false && "FATAL: Unexpected call to ATM::InputContainer::acceptInput()!!!");
        }
    }

    void delFromEnd() {
        _pin.remove(_pin.length() - 1, 1);
    }

    const QString& getPin () const {
        return _pin;
    }

    void clearPin()
    {
        _pin.clear();
    }

private:
    bool addNextToArray(QChar input){
        if(input.digitValue() == -1)
        {
            // Not a digit
            return false;
        }
        _pin.append(input);
        return true;
    }
};

#endif // ATM_H
