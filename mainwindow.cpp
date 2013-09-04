#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QBitArray>

#include "imagefilter.cpp"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openfiledialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    openFileDialog = 0;
    //setWindowIcon(QIcon(":/images/icon.png"));
    setCurrentFile("");
    this->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::open()
{
    if (okToContinue()) {
        if (!curFile.isEmpty()){
            loadFile(curFile, bitFormat);}
    }
}

bool MainWindow::loadFile(const QString &fileName, int bitpixel)
{
    if (!this->readFile(fileName, bitpixel)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}
bool MainWindow::readFile(const QString &fileName,int bitpixel)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    int bittoread = file.bytesAvailable() * 8;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_1);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    switch(bitpixel){
    case 1:
        int height = sqrt(bittoread) + 1;
        ui->heightSlider->setValue(height);
        int width = height;
        ui->widthSlider->setValue(width);
        //image = QImage(height, width, QImage::Format_Mono);
        const QByteArray blob = file.readAll();
        //QBitArray bitblob = QVariant(blob).toBitArray();
        QBitmap bitmap = QBitmap::fromData(QSize(height,width),(const uchar*)blob.data(),QImage::Format_Mono);
        ui->imageArea->setPixmap(bitmap);
        //        quint32 word = 0;
//        quint8 byte;
//        int i = 0;
//        QBitArray bitmap(bittoread);
//        while(!in.atEnd()){
//            in >> byte;
//            word = (word << 8) | byte;
//            bitmap.setBit(i,word & 0x80000000);
//            i++;
//        }
        break;
    }

    //in >> image;
    QApplication::restoreOverrideCursor();
    //    image = QImage(fileName);
    //    ui->imageArea->setScaledContents(true);

    //ui->imageArea->clear();
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
bool MainWindow::saveFile(const QString &fileName)
{
    if (!this->writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
bool MainWindow::writeFile(const QString &fileName)
{
    //    QFile file(fileName);
    //    if (!file.open(QIODevice::WriteOnly)) {
    //        QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
    //                             tr("Cannot write file %1:\n%2.")
    //                             .arg(file.fileName())
    //                             .arg(file.errorString()));
    //        return false;
    //    }
    //    QDataStream out(&file);
    //    out.setVersion(QDataStream::Qt_5_1);
    //    QApplication::setOverrideCursor(Qt::WaitCursor);
    //    out << image;
    //    QApplication::restoreOverrideCursor();


    //    return true;
    return image.save(fileName);
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("%1 - Save image").arg(QApplication::applicationName()), ".",tr("Portable Network Graphics (*.png)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Image"),
                                     tr("The document has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::No
                                     | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            //   return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);
    QString shownName = tr("Untitled");
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
    }
    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                   .arg(tr("ProgettoPiattaformeSW")));
}
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::on_actionNew_triggered()
{
    if (okToContinue()) {
        ui->imageArea->clear();
        setCurrentFile("");
    }
}
void MainWindow::on_actionExit_triggered()
{
    close();
}
void MainWindow::on_actionFile_triggered()
{
    if (!openFileDialog) {
        openFileDialog = new OpenFileDialog(this);
        openFileDialog->show();
        openFileDialog->raise();
        openFileDialog->activateWindow();

    }
}


void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_As_triggered()
{
    saveAs();
}

void MainWindow::on_contrastSlider_valueChanged(int value)
{

    valueContrast = value;
    isContrast = true;
    ui->drawPushButton->setEnabled(true);

}

void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    valueBrightness = value;
    isBrightness = true;
    ui->drawPushButton->setEnabled(true);
}

void MainWindow::on_actionUndo_triggered()
{
    if(imageSnapshot.size() > 0){
        image = imageSnapshot.at(imageSnapshot.size()-1);
        imageSnapshot.removeLast();
        ui->imageArea->setPixmap(QPixmap::fromImage(image));
    }
}
void MainWindow::on_drawPushButton_clicked()
{
    imageSnapshot.append(image);
    if(isContrast){
        image = changeContrast(image, valueContrast);
    }
    if(isBrightness){
        image = changeBrightness(image, valueBrightness);
    }

    ui->imageArea->setPixmap(QPixmap::fromImage(image));
    isBrightness = false;
    isContrast = false;
    ui->drawPushButton->setEnabled(false);
}


void MainWindow::setBitFormat(int bitformat){
    bitFormat = bitformat;
    switch(bitformat){
    case 1:
        ui->oneBitButton->setChecked(true);
        break;
    case 4:
        ui->oneBitButton->setChecked(true);
        break;
    case 8:
        ui->oneBitButton->setChecked(true);
        break;
    case 16:
        ui->oneBitButton->setChecked(true);
        break;
    case 24:
        ui->oneBitButton->setChecked(true);
        break;
    }
}
