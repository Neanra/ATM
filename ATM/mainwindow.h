#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool connectionSuccessful;

private:
    Ui::MainWindow *ui;
public slots:
    void showText();
};

#endif // MAINWINDOW_H
