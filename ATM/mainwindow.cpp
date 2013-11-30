#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _connected_atm(NULL)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showText(QString text)
{
    ui->textBrowser->setText(text);
}

void MainWindow::appendText(QString text){
    ui->textBrowser->moveCursor (QTextCursor::End);
    ui->textBrowser->insertPlainText (text);
    ui->textBrowser->moveCursor (QTextCursor::End);
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

void MainWindow::disableKeyboard()
{
    this->ui->pushButton_1->setEnabled(false);
    this->ui->pushButton_2->setEnabled(false);
    this->ui->pushButton_3->setEnabled(false);
    this->ui->pushButton_4->setEnabled(false);
    this->ui->pushButton_5->setEnabled(false);
    this->ui->pushButton_6->setEnabled(false);
    this->ui->pushButton_7->setEnabled(false);
    this->ui->pushButton_8->setEnabled(false);
    this->ui->pushButton_9->setEnabled(false);
    this->ui->pushButton_0->setEnabled(false);
    this->ui->pushButton_backspace->setEnabled(false);
}

void MainWindow::enableKeyboard()
{
    this->ui->pushButton_1->setEnabled(true);
    this->ui->pushButton_2->setEnabled(true);
    this->ui->pushButton_3->setEnabled(true);
    this->ui->pushButton_4->setEnabled(true);
    this->ui->pushButton_5->setEnabled(true);
    this->ui->pushButton_6->setEnabled(true);
    this->ui->pushButton_7->setEnabled(true);
    this->ui->pushButton_8->setEnabled(true);
    this->ui->pushButton_9->setEnabled(true);
    this->ui->pushButton_0->setEnabled(true);
    this->ui->pushButton_0->isEnabled();
    this->ui->pushButton_backspace->setEnabled(true);
}

void  MainWindow::enableEnterBtn(){
    this->ui->enterBtn->setEnabled(true);
}

void MainWindow::disableEnterBtn(){
    this->ui->enterBtn->setEnabled(false);
}

bool MainWindow::isEnabledKeyboard(){
    return ((this->ui->pushButton_1->isEnabled()) && (this->ui->pushButton_2->isEnabled()) &&
            (this->ui->pushButton_3->isEnabled()) && (this->ui->pushButton_4->isEnabled()) &&
            (this->ui->pushButton_5->isEnabled()) && (this->ui->pushButton_6->isEnabled()) &&
            (this->ui->pushButton_7->isEnabled()) && (this->ui->pushButton_8->isEnabled()) &&
            (this->ui->pushButton_9->isEnabled()) && (this->ui->pushButton_0->isEnabled()) &&
            (this->ui->pushButton_backspace->isEnabled()));
}

void MainWindow::on_enterBtn_clicked()
{
    if (!isEnabledKeyboard()){
        _connected_atm->processInput(ui->inputField->text());
        ui->inputField->clear();
        enableKeyboard();
        disableInput();
    }
    else if (isEnabledKeyboard())
    {
        _connected_atm->processInput(keyboard.getPin());
        keyboard.clearPin();
    }
}

void MainWindow::on_pushButton_1_clicked()
{
    keyboard.addNextToArray('1');
    appendText("*");
}

void MainWindow::on_pushButton_2_clicked()
{
    keyboard.addNextToArray('2');
    appendText("*");
}

void MainWindow::on_pushButton_3_clicked()
{
    keyboard.addNextToArray('3');
    appendText("*");
}

void MainWindow::on_pushButton_4_clicked()
{
    keyboard.addNextToArray('4');
    appendText("*");
}

void MainWindow::on_pushButton_5_clicked()
{
    keyboard.addNextToArray('5');
    appendText("*");
}

void MainWindow::on_pushButton_6_clicked()
{
    keyboard.addNextToArray('6');
    appendText("*");
}

void MainWindow::on_pushButton_7_clicked()
{
    keyboard.addNextToArray('7');
    appendText("*");
}

void MainWindow::on_pushButton_8_clicked()
{
    keyboard.addNextToArray('8');
    appendText("*");
}

void MainWindow::on_pushButton_9_clicked()
{
    keyboard.addNextToArray('9');
    appendText("*");
}

void MainWindow::on_pushButton_0_clicked()
{
    keyboard.addNextToArray('0');
    appendText("*");
}

void MainWindow::on_pushButton_backspace_clicked()
{
    if (!keyboard.isPinEmpty())
    {
        keyboard.delFromEnd();
        ui->textBrowser->moveCursor (QTextCursor::End);
        ui->textBrowser->textCursor().deletePreviousChar();
        ui->textBrowser->textCursor().clearSelection();
    }
}

void MainWindow::on_powerBtn_clicked()
{
    if(_connected_atm->isOn())
    {
        _connected_atm->powerOff();
        ui->powerBtn->setText("Turn ON");
        ui->inputField->clear();
        disableInput();
        disableKeyboard();
        disableEnterBtn();
        disablePrinter();
    }
    else
    {
        _connected_atm->powerOn();
        ui->powerBtn->setText("Turn OFF");
        enableEnterBtn();
        enableInput();
        disableKeyboard();
        disablePrinter();
    }
}
