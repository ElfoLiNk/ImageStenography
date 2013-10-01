#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "openfiledialog.h"
#include "ui_openfiledialog.h"

OpenFileDialog::OpenFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileDialog)
{
    ui->setupUi(this);
    ui->buttonBox->setEnabled(false);
    setWindowTitle(tr("OpenBlob - ProgettoPiattaformeSW"));
}

OpenFileDialog::~OpenFileDialog()
{
    delete ui;
}

void OpenFileDialog::on_pushButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open Blob"), ".",
                                            tr("Blob (*.*)"));
    if(!fileName.isEmpty()){
        ui->lineEdit->setText(fileName);
        ui->groupBox->setEnabled(true);
    }
}

void OpenFileDialog::on_paletteButton_clicked()
{
    paletteName = QFileDialog::getOpenFileName(this,
                                               tr("Open Palette"), ".",
                                               tr("Palette (*.txt)"));
}

void OpenFileDialog::on_buttonBox_accepted()
{
    emit setFileName(fileName);
    emit setPaletteName(paletteName);
    int bitFormat = findBitFormat();
    emit fileBitFormat(bitFormat);
    emit openFile();
}

int OpenFileDialog::findBitFormat(){
    if(ui->oneBitButton->isChecked()){
        return 1;
    }
    if(ui->fourBitButton->isChecked()){
        return 4;
    }
    if(ui->eightBitButton->isChecked()){
        return 8;
    }
    if(ui->sixteenBitButton->isChecked()){
        return 16;
    }
    if(ui->twentyfourBitButton->isChecked()){
        return 24;
    }

    return -1;
}

