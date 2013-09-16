#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QBitArray>
#include <QPixmap>
#include <QPainter>
#include <QGridLayout>
#include <QScrollBar>

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

    ui->imageArea->setBackgroundRole(QPalette::Base);
    ui->imageArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->imageArea->setScaledContents(true);

    ui->scrollArea->setBackgroundRole(QPalette::Dark);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::open()
{
    if (okToContinue()) {
        if (!curFile.isEmpty()){
            if(loadFile(curFile)){
                drawImage();
            }
        }
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (!this->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}
bool MainWindow::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    blob = file.readAll();
    QApplication::restoreOverrideCursor();

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
    openFileDialog = 0;
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
    drawImage();
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

void MainWindow::paintEvent(QPaintEvent *paint){
    QPainter painter(this);



}

void MainWindow::drawImage(){
    switch(bitFormat){
    case 1:
    {
        height = sqrt(blob.size()*8) + 1;
        ui->heightSlider->setValue(height);
        width = height;
        ui->widthSlider->setValue(width);
        ui->imageArea->resize(width,height);
        bitmap = QBitmap::fromData(QSize(height,width),(const uchar*)blob.data(),QImage::Format_Mono);
        ui->imageArea->setPixmap(bitmap);
        break;
    }
    case 4:
    {
        height = sqrt(blob.size()*8)/4 + 1;
        ui->heightSlider->setValue(height);
        width = height;
        ui->widthSlider->setValue(width);
        ui->imageArea->resize(width,height);
        ui->imageArea->setPixmap(pixmap);

        break;
    }
    case 8:
    {
        height = sqrt(blob.size()) + 1;
        ui->heightSlider->setValue(height);
        width = height;
        ui->widthSlider->setValue(width);
        pixmap.loadFromData(blob,(const char*)3,Qt::AutoColor);
        ui->imageArea->setPixmap(pixmap);
        break;
    }
    case 16:
    {
        height = sqrt(blob.size()*8)/16 + 1;
        ui->heightSlider->setValue(height);
        width = height;
        ui->widthSlider->setValue(width);
        ui->imageArea->setPixmap(pixmap);
        break;
    }
    case 24:
    {
        height = sqrt(blob.size()*8)/24 + 1;
        ui->heightSlider->setValue(height);
        width = height;
        ui->widthSlider->setValue(width);
        ui->imageArea->setPixmap(pixmap);
        break;
    }
    default:
        break;
    }

    scaleFactor = 1.0;

    ui->actionFit_To_Window->setEnabled(true);
    updateActions();

    if (!ui->actionFit_To_Window->isChecked())
        ui->imageArea->adjustSize();

}


void MainWindow::updateActions()
{
    ui->actionZoomIn->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionZoomOut->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionNormal_Size->setEnabled(!ui->actionFit_To_Window->isChecked());
}


void MainWindow::scaleImage(double factor)

{
    Q_ASSERT(ui->imageArea->pixmap());
    scaleFactor *= factor;
    ui->imageArea->resize(scaleFactor * ui->imageArea->pixmap()->size());

    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);

    ui->actionZoomIn->setEnabled(scaleFactor < 3.0);
    ui->actionZoomOut->setEnabled(scaleFactor > 0.333);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}


void MainWindow::on_heightSlider_valueChanged(int value)
{
    height = value;
    ui->drawPushButton->setEnabled(true);

}

void MainWindow::on_widthSlider_valueChanged(int value)
{
    width = value;
    ui->drawPushButton->setEnabled(true);
}

void MainWindow::on_actionZoomIn_triggered()
{
    scaleImage(1.25);
}

void MainWindow::on_actionZoomOut_triggered()
{
    scaleImage(0.8);
}

void MainWindow::on_actionNormal_Size_triggered()
{
    ui->imageArea->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::on_actionFit_To_Window_triggered()
{
    bool fitToWindow = ui->actionFit_To_Window->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        on_actionNormal_Size_triggered();
    }
    updateActions();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Progetto Piattaforme SW"),
                       tr("<p>XXXXX</p>"));
}
