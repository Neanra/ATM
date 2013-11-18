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

    void printText(QString text);

    void enableInput();

    void disableInput();

    void enablePrinter();

    void disablePrinter();

private slots:
    void on_enterBtn_clicked();
    void on_switchBtn_clicked();
};

#endif // MAINWINDOW_H
