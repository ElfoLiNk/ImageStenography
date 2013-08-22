#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>

#include "inputbitdialog.h"
#include "imagefilter.cpp"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inputBitDialog = 0;
    //setWindowIcon(QIcon(":/images/icon.png"));
    setCurrentFile("");
    this->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    //    fileToolBar->addAction(openAction);
    //    fileToolBar->addAction(saveAction);
}


void MainWindow::open()
{
    if (okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Image"), ".",
                                                        tr("Portable Network Graphics (*.png)\n""Windows Bitmap(*.bmp)\n""Graphic Interchange Format(*.gif)\n""Joint Photographic Experts Group(*.jpg)\n""Joint Photographic Experts Group(*.jpeg)\n""Portable Bitmap (*.pbm)\n""Portable Graymap (*.pgm)\n""Portable Pixmap (*.ppm)\n""X11 Bitmap (*.xbm)\n""X11 Pixmap (*.xpm)"));
        if (!fileName.isEmpty()){
            loadFile(fileName);}
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
    //    QFile file(fileName);
    //    if (!file.open(QIODevice::ReadOnly)) {
    //        QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
    //                             tr("Cannot read file %1:\n%2.")
    //                             .arg(file.fileName())
    //                             .arg(file.errorString()));
    //        return false;
    //    }
    //    QDataStream in(&file);
    //    in.setVersion(QDataStream::Qt_5_1);
    //    QApplication::setOverrideCursor(Qt::WaitCursor);
    //    in >> image;
    //    QApplication::restoreOverrideCursor();
    image = QImage(fileName);

    ui->imageArea->setScaledContents(true);
    ui->imageArea->setPixmap(QPixmap::fromImage(image));

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
                                                    tr("%1 - Save image").arg(QApplication::applicationName()), ".",tr("Portable Network Graphics (*.png)\n""Windows Bitmap(*.bmp)\n""Graphic Interchange Format(*.gif)\n""Joint Photographic Experts Group(*.jpg)\n""Joint Photographic Experts Group(*.jpeg)\n""Portable Bitmap (*.pbm)\n""Portable Graymap (*.pgm)\n""Portable Pixmap (*.ppm)\n""X11 Bitmap (*.xbm)\n""X11 Pixmap (*.xpm)"));
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

QImage MainWindow::contrast(QImage& source, int factor)
{
    //contrast :new color = 128 + Contrast *( old color - 128)
    if (factor < 0) return source;
    if (factor > 20) return source;
    double contrast = (100.0+factor)/100.0;
    double red, green, blue;
    int pixels = source.width() * source.height();
    unsigned int *data = (unsigned int *)source.bits();
    for (int i = 0; i < pixels; ++i) {
        red= 128+ contrast*(qRed(data[i])-128);
        red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff : red;
        green= 128+ contrast*(qGreen(data[i])-128);
        green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff : green;
        blue= 128+ contrast*(qBlue(data[i])-128);
        blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff : blue ;
        data[i] = qRgba(red, green, blue, qAlpha(data[i]));
    }
    return source;
}
QImage MainWindow::brighten(QImage& source, int factor)
{
    if (factor < -255 || factor > 255) return source;
    int red, green, blue;

    int pixels = source.width() * source.height();
    unsigned int *data = (unsigned int *)source.bits();
    for (int i = 0; i < pixels; ++i) {
        red= qRed(data[i])+ factor;
        red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff : red;
        green= qGreen(data[i])+factor;
        green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff : green;
        blue= qBlue(data[i])+factor;
        blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff : blue ;
        data[i] = qRgba(red, green, blue, qAlpha(data[i]));
    }
    return source;
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
    open();
}
void MainWindow::on_actionInput_triggered()
{
    if (!inputBitDialog) {
        inputBitDialog = new InputBitDialog(this);
        inputBitDialog->show();
        inputBitDialog->raise();
        inputBitDialog->activateWindow();
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

void MainWindow::on_horizontalSlider_valueChanged(int value)
{

    valueContrast = value;
    isContrast = true;
    ui->pushButton->setEnabled(true);

}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    valueBrightness = value;
    isBrightness = true;
    ui->pushButton->setEnabled(true);
}

void MainWindow::on_actionUndo_triggered()
{
    if(imageSnapshot.size() > 0){
        image = imageSnapshot.at(imageSnapshot.size()-1);
        imageSnapshot.removeLast();
        ui->imageArea->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::on_pushButton_clicked()
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
    ui->pushButton->setEnabled(false);
}


void MainWindow::on_radioButton_toggled(bool checked)
{
    if(checked){
    //image = bitmap.fromImage(image).toImage().convertToFormat(QImage::Format_RGB888);
    imageSnapshot.append(image);
    image = image.convertToFormat(QImage::Format_Indexed8, Qt::AvoidDither);
     ui->imageArea->setPixmap(QBitmap::fromImage(image));
    }else on_actionUndo_triggered();
}

void MainWindow::on_radioButton_2_toggled(bool checked)
{
    if(checked){
    //image = bitmap.fromImage(image).toImage().convertToFormat(QImage::Format_RGB888);

    imageSnapshot.append(image);
    image = image.convertToFormat(QImage::Format_RGB16);
     ui->imageArea->setPixmap(QBitmap::fromImage(image));
    }else on_actionUndo_triggered();
}

void MainWindow::on_radioButton_3_toggled(bool checked)
{
    if(checked){
    //image = bitmap.fromImage(image).toImage().convertToFormat(QImage::Format_RGB888);
    imageSnapshot.append(image);
    image = image.convertToFormat(QImage::Format_RGB32);
     ui->imageArea->setPixmap(QBitmap::fromImage(image));
    }else on_actionUndo_triggered();
}
