#include "inputbitdialog.h"
#include "ui_inputbitdialog.h"

InputBitDialog::InputBitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputBitDialog)
{
    ui->setupUi(this);
}

InputBitDialog::~InputBitDialog()
{
    delete ui;
}
