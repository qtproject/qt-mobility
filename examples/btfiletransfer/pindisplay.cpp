#include "pindisplay.h"
#include "ui_pindisplay.h"

pinDisplay::pinDisplay(QString title, QString pin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pinDisplay)
{
    ui->setupUi(this);
    ui->label->setText(title);
    ui->pin->setText(pin);
}

pinDisplay::~pinDisplay()
{
    delete ui;
}

void pinDisplay::on_buttonBox_accepted()
{
    deleteLater();
}
