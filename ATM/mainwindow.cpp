#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cassert>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _connected_atm(NULL),
    keyboard(NULL)
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
    ui->textBrowser_2->insertPlainText(text.append("\n-----\n"));
    ui->textBrowser_2->moveCursor (QTextCursor::End);
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
    setKeyboardButtonsEnabled(false);
}

void MainWindow::enableKeyboard()
{
    setKeyboardButtonsEnabled(true);
}

void MainWindow::setKeyboardButtonsEnabled(bool enabled)
{
    this->ui->pushButton_1->setEnabled(enabled);
    this->ui->pushButton_2->setEnabled(enabled);
    this->ui->pushButton_3->setEnabled(enabled);
    this->ui->pushButton_4->setEnabled(enabled);
    this->ui->pushButton_5->setEnabled(enabled);
    this->ui->pushButton_6->setEnabled(enabled);
    this->ui->pushButton_7->setEnabled(enabled);
    this->ui->pushButton_8->setEnabled(enabled);
    this->ui->pushButton_9->setEnabled(enabled);
    this->ui->pushButton_0->setEnabled(enabled);
    this->ui->pushButton_backspace->setEnabled(enabled);
    this->ui->cancelButton->setEnabled(enabled);
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

void MainWindow::showCardState(QString cardState)
{
    ui->cardState->setText(cardState);
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
        _connected_atm->processInput(keyboard->getPin());
        keyboard->clearPin();
    }
}

void MainWindow::on_pushButton_1_clicked()
{
    keyboard->acceptInput('1');
}

void MainWindow::on_pushButton_2_clicked()
{
    keyboard->acceptInput('2');
}

void MainWindow::on_pushButton_3_clicked()
{
    keyboard->acceptInput('3');
}

void MainWindow::on_pushButton_4_clicked()
{
    keyboard->acceptInput('4');
}

void MainWindow::on_pushButton_5_clicked()
{
    keyboard->acceptInput('5');
}

void MainWindow::on_pushButton_6_clicked()
{
    keyboard->acceptInput('6');
}

void MainWindow::on_pushButton_7_clicked()
{
    keyboard->acceptInput('7');
}

void MainWindow::on_pushButton_8_clicked()
{
    keyboard->acceptInput('8');
}

void MainWindow::on_pushButton_9_clicked()
{
    keyboard->acceptInput('9');
}

void MainWindow::on_pushButton_0_clicked()
{
    keyboard->acceptInput('0');
}

void MainWindow::on_pushButton_backspace_clicked()
{
    if (!keyboard->isPinEmpty())
    {
        keyboard->delFromEnd();
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

void MainWindow::on_cancelButton_clicked()
{
    _connected_atm->cancelOperation();
}
