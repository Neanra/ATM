#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _connected_atm(NULL)
{
    ui->setupUi(this);

    QObject::connect(ui->powerBtn, SIGNAL(clicked()), this, SLOT(on_switchBtn_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showText(QString text)
{
    ui->textBrowser->setText(text);
}

void MainWindow::printText(QString text)
{
   ui->textBrowser_2->setText(text);
}

void MainWindow::enableInput()
{
    this->ui->inputField->setEnabled(true);
}

void MainWindow::disableInput()
{
    this->ui->inputField->setEnabled(false);
}

void MainWindow::enablePrinter()
{
    this->ui->textBrowser_2->setEnabled(true);
}

void MainWindow::disablePrinter()
{
    this->ui->textBrowser_2->setEnabled(false);
}

void MainWindow::on_enterBtn_clicked()
{
    _connected_atm->processInput(ui->inputField->text());
    ui->inputField->clear();
}

void MainWindow::on_switchBtn_clicked()
{
    if(_connected_atm->isOn())
    {
        _connected_atm->powerOff();
        ui->powerBtn->setText("Turn ON");
    }
    else
    {
        _connected_atm->powerOn();
        ui->powerBtn->setText("Turn OFF");
    }
}
