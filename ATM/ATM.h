#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>
#include <QString>
#include <QtSql>
#include <exception>


// TODO: Move DB configuration data to a better place
#define BANK_DATABASE_DRIVER "QSQLITE"
#define BANK_DATABASE_NAME "bank.db"

using namespace std;

class ATM;

// Window that serves as ATM's display must implement this interface
class IDisplay
{
public:
    // Called when ATM establishes connection to its display
    virtual void connect(const ATM& atm) = 0;
    // Called when ATM disconnects (e.g. when it is destroyed)
    virtual void disconnect() = 0;
    // Output text to the display
    virtual void showText(QString text) = 0;
    virtual void printText(QString text) = 0;
    virtual void enableInput() = 0;
    virtual void disableInput() = 0;
    virtual void enablePrinter() = 0;
    virtual void disablePrinter() = 0;
    virtual void enableKeyboard() = 0;
    virtual void disableKeyboard() = 0;
};

class ATM
{
public:
    class Keyboard;
    explicit ATM(IDisplay* display, IDisplay* printer);
    virtual ~ATM();

    // Accept input from user
    void processInput(QString input);

    void powerOn();
    void powerOff();

    void showBalance(QString cardNumber);

    inline bool isOn()
    {
        return (_state != POWER_OFF);
    }

private:
    // Ejection messages
    static const QString EJECT_SUCCESS;
    static const QString EJECT_ERR_CONN;    // No connection to bank DB
    static const QString EJECT_ERR_READ;    // Invalid or inexistant card number
    static const QString EJECT_ERR_FAIL;    // Other error

    // TODO: Seizure messages?

    void onCardInserted(QString cardNumber);
    void onPinEntered(QString cardsPin);
    void onCardEjected(QString message = EJECT_SUCCESS);
    //void onCardSeized(QString message = SEIZE_INVALID_PIN);  // When pin is entered incorrectly 3 times or card is blocked

    // Print text to display unless there is no display
    void displayText(QString text)
    {
        if(_display)
        {
            _display->showText(text);
        }
    }

    void printText(QString text)
    {
        if(_printer)
        {
            _printer->printText(text);
        }
    }

private:
    struct Card
    {
        QString _card_number;
        QString _pin;
        QString _owner_last_name;
        bool _owner_gender_male;    // For politeness :)
        int _balance;
        //etc.
    };

    Card* _current_card;

    enum ATMState {POWER_OFF = 0, NO_CARD = 1, PENDING_PIN = 2, TOP_MENU = 3 /* bla-bla-bla */};
    ATMState _state;
    IDisplay* _display;
    IDisplay* _printer;

    // TODO: Move everything related to DB to a separate class
    //==========
    // Templates for common SQL queries
    static const QString SELECT_CARD_BY_NUMBER;
    //static const QString DEACTIVATE_CARD;
    //static const QString UPDATE_CARD_BALANCE;
    //etc.

    QSqlDatabase _database; // Connection to DB
};

class ATM::Keyboard {
private:
    //array will contain Pin
    static const int _size_of_pinArray = 4;
    QString *_p_arr;
public:
    //Pin -1 -1 -1 -1 by default
    Keyboard():_p_arr(new QString[_size_of_pinArray]){
        for (int i = 0; i < _size_of_pinArray; i++) {
                _p_arr[i] = "-1";
            }
    }

    ~Keyboard(){
        delete [] _p_arr;
    }

    bool isPin(){
        for (int i = 0; i < _size_of_pinArray; i++) {
                if (_p_arr[i] == "-1")
                    return false;
        }
        return true;
    }

    bool isPinEmpty() {
        for (int i = 0; i < _size_of_pinArray; i++) {
                if (_p_arr[i] != "-1")
                    return false;
        }
        return true;
    }

    void addNextToArray(QString input){
        for (int i = 0; i < _size_of_pinArray; i++) {
                if (_p_arr[i] == "-1")
                    _p_arr[i]=input;
        }
     }

    void delFromEnd() {
        for (int i = _size_of_pinArray-1; i >= 0; i--) {
        if (_p_arr[i] != "-1")
        {
            _p_arr[i]="-1";
            break;
        }
    }
}

    QString getPin () const {
        QString pin = "";
        for (int i = 0; i < _size_of_pinArray; i++) {
                pin+=_p_arr[i];
        }
        return pin;
    }

};

#endif // ATM_H
