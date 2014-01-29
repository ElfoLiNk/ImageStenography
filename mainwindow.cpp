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
#include <QTextStream>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QColorDialog>

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

    colorPixFake = Qt::green;
    ui->colorLabel->setText(colorPixFake.name());
    ui->colorLabel->setPalette(QPalette(colorPixFake));
    ui->colorLabel->setAutoFillBackground(true);


    // AboutQt Signal
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::open()
{
    if(!paletteFile.isEmpty()){
        QFile file(paletteFile);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot open file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
        }
        QTextStream ts(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Parsing line
        // Example: byte_in_input_hex rosso_hex verde_hex blu_hex
        do {
            // Read a line of the palette file
            QString element = ts.readLine();
            // Split line by white space
            QStringList hexlist = element.split(" ");

            // Adding RGB to colour table
            bool ok;
            vectorColors.append(qRgb(hexlist.at(1).toInt(&ok,16),hexlist.at(2).toInt(&ok,16),hexlist.at(3).toInt(&ok,16)));
        } while (!ts.atEnd());
        QApplication::restoreOverrideCursor();
    }

    ui->oneBitButton->setEnabled(true);
    ui->fourBitButton->setEnabled(true);
    ui->eightBitButton->setEnabled(true);
    ui->sixteenBitButton->setEnabled(true);
    ui->twentyfourBitButton->setEnabled(true);
    ui->brightnessSlider->setEnabled(true);
    ui->brightnessSpinBox->setEnabled(true);
    ui->contrastSlider->setEnabled(true);
    ui->contrastSpinBox->setEnabled(true);
    ui->heightSlider->setEnabled(true);
    ui->heightSpinBox->setEnabled(true);
    ui->widthSlider->setEnabled(true);
    ui->widthSpinBox->setEnabled(true);
    ui->imageCheckBox->setEnabled(true);
    ui->dataCheckBox->setEnabled(true);

    if (!curFile.isEmpty()){
        if(loadFile(curFile)){
            drawImage();
        }
    }
    openFileDialog = 0;

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
    if(!image.isNull()){
        QFile file(fileName+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        return image.save(&file, "PNG");
    }
    if(!bitmap.isNull()){
        QFile file(fileName+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        return bitmap.save(&file, "PNG");
    }

    return false;

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

        connect(openFileDialog, SIGNAL(fileBitFormat(int)), this,SLOT(setBitFormat(int)));
        connect(openFileDialog, SIGNAL(openFile()),this,SLOT(open()));
        connect(openFileDialog, SIGNAL(setFileName(const QString&)), this, SLOT(setCurrentFile(const QString&)) );
        connect(openFileDialog, SIGNAL(setPaletteName(const QString&)), this, SLOT(setPaletteFile(const QString&)) );

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
        image = imageSnapshot.at(imageSnapshot.size()-1);
        imageSnapshot.removeLast();
        imageArea->setPixmap(QPixmap::fromImage(image));
    }
}
void MainWindow::on_drawPushButton_clicked()
{
    //open();
    if(imageArea->pixmap()){
        imageSnapshot.append(imageArea->pixmap()->toImage());
    }

    vectorColors.clear();

    drawImage();

    if(isContrast){
        image = changeContrast(image, valueContrast);
        imageArea->setPixmap(pixmap.fromImage(image));
    } else if(isBrightness){
        image = changeBrightness(image, valueBrightness);
        imageArea->setPixmap(pixmap.fromImage(image));
    }// else {

    //}
    isBrightness = false;
    isContrast = false;
    ui->drawPushButton->setEnabled(false);
}


void MainWindow::setBitFormat(int bitformat){
    bitFormat = bitformat;
    switch(bitformat){
    case 1:
        this->ui->oneBitButton->setChecked(true);
        break;
    case 4:
        this->ui->fourBitButton->setChecked(true);
        break;
    case 8:
        this->ui->eightBitButton->setChecked(true);
        break;
    case 16:
        this->ui->sixteenBitButton->setChecked(true);
        break;
    case 24:
        this->ui->twentyfourBitButton->setChecked(true);
        break;
    }
}


void MainWindow::drawImage(){
    if(!blob.isEmpty()){
        switch(bitFormat){
        case 1:
        {
            imageArea->resize(width,height);
            bitmap = bitmap.fromData(QSize(width,height),(const uchar*)blob.data(),QImage::Format_Mono);
            imageArea->setPixmap(bitmap);
            image = bitmap.toImage();
            break;
        }
        case 4:
        {
            int scale = 15;
            imageArea->resize(width,height);
            QByteArray extracted_datapgm = convertToPGM(blob.data(),blob.size(),scale);

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
            image = pixmap.toImage();
            break;
        }
        case 8:
        {
            imageArea->resize(width,height);
            if(!paletteFile.isEmpty()){
                image = QImage(width, height, QImage::Format_RGB888);
                int k = 0;
                for(int i = 0;i<height;i++){
                    for(int j= 0; j< width;j++){
                        image.setPixel(i,j,vectorColors.at(k));
                        k++;
                        if(k>vectorColors.size()){
                            imageArea->setPixmap(pixmap.fromImage(image));
                            break;
                        }
                    }
                }
                imageArea->resize(width,height);
                imageArea->setPixmap(pixmap.fromImage(image));
            }else{
                int scale = 255;

                QByteArray extracted_datapgm = convertToPGM(blob.data(),blob.size(),scale);

                if (extracted_datapgm.isNull())
                {
                    QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"could not convertToPGM()");
                }

                bool ok = pixmap.loadFromData(extracted_datapgm, "pgm");
                imageArea->setPixmap(pixmap);
                if(ok){
                    imageArea->setPixmap(pixmap);
                } else{
                    QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"ERROR pixmap load from data");

                }
            }
            image = pixmap.toImage();
            break;
        }
        case 16:
        {
            imageArea->resize(width,height);
            image = QImage(width, height, QImage::Format_RGB444);
            image.fill(colorPixFake.rgb());
            //bitblob = toQBit(blob);
            // Divisibile per 2
            if(blob.size()%2 == 0){

                for(int i = 0; i < blob.size()-1; i++){
                    uint trasparency = ((quint8)blob.at(i))>>4;
                    uint scale = trasparency;
                    uint red = blob.at(i)&0x0F;
                    uint green = ((quint8)blob.at(i+1))>>4;
                    uint blue = blob.at(i+1)&0x0F;
                    if(scale == 0){
                        vectorColors.append(qRgb(red, green, blue));
                    }else{
                        vectorColors.append(qRgb(red*scale, green*scale, blue*scale));
                    }
                }
                int k = 0;
                for(int i = 0;i< height;i++){
                    for(int j= 0; j< width;j++){
                        image.setPixel(j,i,vectorColors.at(k));
                        k++;
                        if(k>vectorColors.size()){
                            imageArea->setPixmap(pixmap.fromImage(image));
                            break;
                        }
                    }
                }
                // Non divisibile per 2 ignoro i byte spaiati
            }else{
                int rest = blob.size()%2;
                for(int i = 0; i < blob.size()-rest; i++){
                    uint trasparency = (blob.at(i))>>4;
                    uint scale = trasparency;
                    uint red = blob.at(i)&0x0F;
                    uint green = (blob.at(i+1))>>4;
                    uint blue = blob.at(i+1)&0x0F;
                    if(scale == 0){
                        vectorColors.append(qRgb(red, green, blue));
                    }else{
                        vectorColors.append(qRgb(red*scale, green*scale, blue*scale));
                    }
                }
                int k = 0;
                for(int i = 0;i<height;i++){
                    for(int j= 0; j< width;j++){
                        image.setPixel(j,i,vectorColors.at(k));
                        k++;
                        if(k>vectorColors.size()){
                            imageArea->setPixmap(pixmap.fromImage(image));
                            break;
                        }
                    }
                }
            }
            imageArea->setPixmap(pixmap.fromImage(image));
            break;
        }
        case 24:
        {

            imageArea->resize(width,height);
            image = QImage(width, height, QImage::Format_RGB888);
            image.fill(colorPixFake.rgb());
            // Divisibile per 3
            if(blob.size()%3 == 0){

                for(int i = 0; i < blob.size() - 2; i++){
                    vectorColors.append(qRgb(blob.at(i), blob.at(i+1), blob.at(i+2)));
                }
                int k = 0;
                for(int i = 0;i < height; i++){
                    for(int j = 0; j < width; j++){
                        image.setPixel(j,i,vectorColors.at(k));
                        k++;
                        if(k>vectorColors.size()){
                            imageArea->setPixmap(pixmap.fromImage(image));
                            break;
                        }
                    }
                }
                // Non divisibile per 3
            }else{
                int rest = blob.size()%3;
                for(int i = 0; i < blob.size()- rest; i++){
                    vectorColors.append(qRgb(blob.at(i), blob.at(i+1), blob.at(i+2)));
                }
                int k = 0;
                for(int i = 0;i < height;i++){
                    for(int j = 0; j < width;j++){
                        image.setPixel(j,i,vectorColors.at(k));
                        k++;
                        if(k>vectorColors.size()){
                            imageArea->setPixmap(pixmap.fromImage(image));
                            break;
                        }
                    }
                }
            }

            imageArea->setPixmap(pixmap.fromImage(image));
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
            height = sqrt(blob.size()*8);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 4:
        {
            height = sqrt(blob.size()*2);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 8:
        {
            height = sqrt(blob.size());
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 16:
        {
            height = sqrt(blob.size()/2);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 24:
        {
            height = sqrt(blob.size()/3);
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
    out.setVersion(QDataStream::Qt_5_2);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int i = 0; i < height*width; i++){

        if(bitFormat == 8){
            out.writeRawData(img_buffer+i, sizeof(uchar));
        } else{

            const char nibble1=img_buffer[i]&0x0F;
            const char nibble2=img_buffer[i]>>4;

            out.writeRawData(&nibble1, sizeof(uchar));
            out.writeRawData(&nibble2, sizeof(uchar));

        }
    }
    QApplication::restoreOverrideCursor();

    QString scaleS = QString::number(scale);

    pgm.clear();
    pgm.append("P5 ");
    pgm.append(QString::number(width));
    pgm.append(" ");
    pgm.append(QString::number(height));
    pgm.append(" ");
    pgm.append(scaleS);
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
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

void MainWindow::on_fourBitButton_clicked()
{
    bitFormat = 4;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

void MainWindow::on_eightBitButton_clicked()
{
    bitFormat = 8;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

void MainWindow::on_sixteenBitButton_clicked()
{
    bitFormat = 16;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

void MainWindow::on_twentyfourBitButton_clicked()
{
    bitFormat = 24;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    origin = e->pos();
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
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

}

bool MainWindow::on_saveAreaButton_clicked()
{
    if(ui->imageCheckBox->isChecked()){
        //    QDesktopWidget *desktop = QApplication::desktop();
        //    QPixmap screenshot = QGuiApplication::primaryScreen()->grabWindow(desktop->winId(), 0, 0, desktop->width(), desktop->height());

        //    if(!screenshot.isNull()){
        //        screenshot = screenshot.copy(rubberBand->geometry());
        //        ui->saveAreaButton->setEnabled(false);
        QPixmap regioncapture = this->grab(rubberBand->geometry());

        QFile file(curFile+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        //screenshot.save(&file, "PNG");
        regioncapture.save(&file, "PNG");
        return true;


        //    }
    }else{
        // TODO save data of image into a blob
    }

    return false;
}


QBitArray MainWindow::toQBit (QByteArray blob) {
    int const bitsInByte= 8;
    int const bytsInObject= blob.size();

    const char *data = blob.data() ;
    QBitArray result(bytsInObject*bitsInByte);
    for ( int byte=0; byte<bytsInObject ; ++byte ) {
        for ( int bit=0; bit<bitsInByte; ++bit ) {
            result.setBit ( byte*bitsInByte + bit, data[byte] & (1<<bit) ) ;
        }
    }
    return result;
}

void MainWindow::on_pixelFillpushButton_clicked()
{
    colorPixFake = QColorDialog::getColor(Qt::green, this);
    if (colorPixFake.isValid())
    {
        ui->colorLabel->setText(colorPixFake.name());
        ui->colorLabel->setPalette(QPalette(colorPixFake));
        ui->colorLabel->setAutoFillBackground(true);
    }
}
