#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QBitArray>
#include <QPixmap>
#include <QPainter>
#include <QGridLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QRubberBand>

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

    imageArea = new QLabel;
    imageArea->setBackgroundRole(QPalette::Base);
    imageArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageArea->setScaledContents(true);

    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(imageArea);
    ui->scrollArea->resize(500,400);


    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    // AboutQt Signal
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::open()
{
    if (!curFile.isEmpty()){
        if(loadFile(curFile)){
            drawImage();
        }
    }
    openFileDialog = 0;


    if (!paletteFile.isEmpty()){
        if(loadFile(paletteFile)){
            char *pointer = blob.data();
            for(int i = 0; i < blob.size();i++ ){
                vectorColors[i] = qRgb(*pointer, *pointer+1, *pointer+2);
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

    setSizeImage();

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
    if(!pixmap.isNull()){
        QFile file(fileName+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        return pixmap.save(&file, "PNG");
    }
    if(!image->isNull()){
        QFile file(fileName+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        return image->save(&file, "PNG");
    }

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

void MainWindow::setPaletteFile(const QString &fileName){
    paletteFile = fileName;

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


bool MainWindow::on_actionSave_triggered()
{
    if (curFile.isEmpty()) {
        return on_actionSave_As_triggered();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("%1 - Save image").arg(QApplication::applicationName()), ".",tr("Portable Network Graphics (*.png)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
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
        *image = imageSnapshot.at(imageSnapshot.size()-1);
        imageSnapshot.removeLast();
        imageArea->setPixmap(QPixmap::fromImage(*image));
    }
}
void MainWindow::on_drawPushButton_clicked()
{
    if(imageArea->pixmap()){
        imageSnapshot.append(imageArea->pixmap()->toImage());
    }
    if(isContrast){
        *image = changeContrast(*image, valueContrast);
        imageArea->setPixmap(pixmap.fromImage(*image));
    } else if(isBrightness){
        *image = changeBrightness(*image, valueBrightness);
        imageArea->setPixmap(pixmap.fromImage(*image));
    } else {
        drawImage();
    }
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


void MainWindow::drawImage(){
    if(!blob.isEmpty()){
        switch(bitFormat){
        case 1:
        {
            imageArea->resize(width,height);
            bitmap = QBitmap::fromData(QSize(height,width),(const uchar*)blob.data(),QImage::Format_Mono);
            imageArea->setPixmap(bitmap);
            break;
        }
        case 4:
        {
            int scale = 15;
            imageArea->resize(width,height);
            QByteArray extracted_datapgm = convertToPGM((char*)blob.data(),blob.size(),scale);

            if (extracted_datapgm.isNull())
            {
                QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"could not convertToPGM()");
            }

            bool ok = pixmap.loadFromData(extracted_datapgm, "pgm");
            if(ok){
                imageArea->setPixmap(pixmap);
            } else{
                QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"ERROR pixmap load from data");

            }
            break;
        }
        case 8:
        {
            if(!vectorColors.isEmpty()){
                QImage image(width, height, QImage::Format_RGB888);
                image.setColorTable(vectorColors);
                imageArea->resize(width,height);
                imageArea->setPixmap(pixmap.fromImage(image));
            }else{
                int scale = 255;
                imageArea->resize(width,height);
                QByteArray extracted_datapgm = convertToPGM((char*)blob.data(),blob.size(),scale);

                if (extracted_datapgm.isNull())
                {
                    QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"could not convertToPGM()");
                }

                bool ok = pixmap.loadFromData(extracted_datapgm, "pgm");
                if(ok){
                    imageArea->setPixmap(pixmap);
                } else{
                    QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"ERROR pixmap load from data");

                }
            }
            break;
        }
        case 16:
        {
            imageArea->resize(width,height);
            QImage image(height, width, QImage::Format_ARGB4444_Premultiplied);

            //imageArea->setPixmap(pixmap.fromImage(image));
            break;
        }
        case 24:
        {
            imageArea->resize(width,height);
            image = new QImage(height, width, QImage::Format_RGB888);
            // Divisibile per 3
            if((blob.size() - 1)%3 == 0){

                for(int i = 0; i < blob.size()-2; i++){
                    vectorColors.append(qRgb(blob.at(i), blob.at(i+1), blob.at(i+2)));
                }
                image->setColorTable(vectorColors);
                // Non divisibile per 3
            }else{
                int rest = (blob.size() - 1)%3;
                for(int i = 0; i < blob.size()-rest; i++){
                    vectorColors.append(qRgb(blob.at(i), blob.at(i+1), blob.at(i+2)));
                }
                image->setColorTable(vectorColors);
            }
            imageArea->setPixmap(pixmap.fromImage(*image));
            break;
        }
        default:
            break;
        }

        scaleFactor = 1.0;

        ui->actionFit_To_Window->setEnabled(true);
        updateActions();

        if (!ui->actionFit_To_Window->isChecked()){
            imageArea->adjustSize();
        }

    }

}

void MainWindow::setSizeImage(){
    if(!blob.isEmpty()){
        switch(bitFormat){
        case 1:
        {
            height = sqrt(blob.size()*8) + 1;
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 4:
        {
            height = sqrt(blob.size()*8)/4 + 1;
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 8:
        {
            height = sqrt(blob.size()) + 1;
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 16:
        {
            height = sqrt(blob.size()*8)/16 + 1;
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 24:
        {
            height = sqrt(blob.size()*8)/24 + 1;
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        default:
            break;
        }
    }

}

void MainWindow::updateActions()
{
    ui->actionZoomIn->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionZoomOut->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionNormal_Size->setEnabled(!ui->actionFit_To_Window->isChecked());
}


void MainWindow::scaleImage(double factor)

{
    Q_ASSERT(imageArea->pixmap());
    scaleFactor *= factor;
    imageArea->resize(scaleFactor * imageArea->pixmap()->size());

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

QByteArray MainWindow::convertToPGM(char* img_buffer, int size,int scale)
{

    QByteArray pgm;
    QByteArray raw;
    QDataStream out(&raw,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int i = 0; i < size; i++){
        out.writeRawData(img_buffer+i, sizeof(uchar));
    }
    QApplication::restoreOverrideCursor();

    pgm.clear();
    pgm.append("P5 ");
    pgm.append(QString::number(width));
    pgm.append(" ");
    pgm.append(QString::number(height));
    pgm.append(" ");
    pgm.append(QString::number(scale));
    pgm.append("\n");
    pgm.append(raw);

    return pgm;

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
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!curFile.isEmpty()) {
        scaleImage(1.25);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionZoomOut_triggered()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!curFile.isEmpty()) {
        scaleImage(0.8);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionNormal_Size_triggered()
{
    imageArea->adjustSize();
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



void MainWindow::on_oneBitButton_clicked()
{
    bitFormat = 1;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
}

void MainWindow::on_fourBitButton_clicked()
{
    bitFormat = 4;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
}

void MainWindow::on_eightBitButton_clicked()
{
    bitFormat = 8;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
}

void MainWindow::on_sixteenBitButton_clicked()
{
    bitFormat = 16;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
}

void MainWindow::on_twentyfourBitButton_clicked()
{
    bitFormat = 24;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    origin = e->pos();

    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();

}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    rubberBand->setGeometry(QRect(origin, e->pos()).normalized());
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    rubberBand->hide();
    ui->saveAreaButton->setEnabled(true);
}

void MainWindow::on_saveAreaButton_clicked()
{


    if(!image->isNull()){

    }
    if(!pixmap.isNull()){
        pixmap.copy(rubberBand->geometry());
    }
    saveFile(curFile);
    ui->saveAreaButton->setEnabled(false);

}
