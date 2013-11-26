#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ATM.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public IDisplay
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    ATM * _connected_atm;
    ATM::Keyboard keyboard;

public:
    inline void connect(const ATM& atm)
    {
        _connected_atm = const_cast<ATM*>(&atm);
#ifndef NDEBUG
        //showText("DEBUG: ATM connected");
#endif
    }
    inline void disconnect()
    {
        _connected_atm = NULL;
#ifndef NDEBUG
        //showText("DEBUG: ATM disconnected");
#endif
    }

    void showText(QString text);

    void appendText(QString text);

    void printText(QString text);

    void enableInput();

    void disableInput();

    void enablePrinter();

    void disablePrinter();

    void enableKeyboard();

    void disableKeyboard();

    void enableEnterBtn();

    void disableEnterBtn();

private slots:
    void on_enterBtn_clicked();
    void on_switchBtn_clicked();
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_0_clicked();
    void on_pushButton_backspace_clicked();
    bool isEnabledKeyboard();
};

#endif // MAINWINDOW_H
