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
    rubberBand = NULL;
    offset = 0;
    height = 1;
    width = 1;

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

    // AboutQt Signal
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::open
 *
 * Opens the file and enable ui image settings
 */
void MainWindow::open()
{
    // Check if the user select a palette file for 8bit image
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

    // Enables ui image settings
    ui->oneBitButton->setEnabled(true);
    ui->fourBitButton->setEnabled(true);
    ui->eightBitButton->setEnabled(true);
    ui->sixteenBitButton->setEnabled(true);
    ui->twentyfourBitButton->setEnabled(true);
    ui->offsetSpinBox->setEnabled(true);
    ui->offsetSlider->setEnabled(true);
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
        // Load File
        if(loadFile(curFile)){
            offsetonce = true;
            // Set correct offset range for the file
            ui->offsetSpinBox->setRange(0,blob.size());
            ui->offsetSlider->setRange(0,blob.size());
            ui->offsetSlider->setValue(offset);
            // Backup file data
            backupBlob();
            // Drawimage to ImageLabel
            drawImage();
        }
    }
    // Reset OpenFileDialog
    openFileDialog = 0;



}

/**
 * @brief MainWindow::loadFile
 * @param fileName
 * @return true if read the file correctly
 */
bool MainWindow::loadFile(const QString &fileName)
{
    if (!this->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }
    // Set Current File
    setCurrentFile(fileName);

    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

/**
 * @brief MainWindow::readFile
 * @param fileName
 * @return true if read the file correctly
 *
 * Load all the file data to QByteArray blob
 */
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

    // Read all data of the file
    QApplication::setOverrideCursor(Qt::WaitCursor);
    blob = file.readAll();
    QApplication::restoreOverrideCursor();

    // Set Range of ui image settings for the file
    ui->widthSpinBox->setRange(1,blob.size() - 1 );
    ui->widthSlider->setRange(1,blob.size() - 1 );
    ui->heightSlider->setRange(1,blob.size() - 1 );
    ui->heightSpinBox->setRange(1,blob.size() - 1 );
    // Calculate the correct image size
    setSizeImage();

    return true;
}

/**
 * @brief MainWindow::saveFile
 * @param fileName
 * @return true file is saved
 *
 *      SAVE
 * The file saved is the current one
 */
bool MainWindow::saveFile(const QString &fileName)
{
    if (!this->writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }
    // Set File Saved the current one
    setCurrentFile(fileName);

    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

/**
 * @brief MainWindow::writeFile
 * @param fileName
 * @return true
 *
 *      SAVE AS
 * Save the image file as png
 */
bool MainWindow::writeFile(const QString &fileName)
{
    // Open Output file
    QFile file(fileName+".png");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    // Check if pixmap is currently used
    if(!pixmap.isNull()){
        // Return and save it
        return pixmap.save(&file, "PNG");
    }

    // Check if image is currently used
    if(!image.isNull()){
        // Return and save it
        return image.save(&file, "PNG");
    }

    // Check if bitmap is currently used
    if(!bitmap.isNull()){
        // Return and save it
        return bitmap.save(&file, "PNG");
    }

    return false;

}

/**
 * @brief MainWindow::setCurrentFile
 * @param fileName
 *
 *  Set current working file and change mainwindow title
 */
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

/**
 * @brief MainWindow::setPaletteFile
 * @param fileName
 *
 * Set current palette file for the 8bit image
 */
void MainWindow::setPaletteFile(const QString &fileName)
{
    paletteFile = fileName;
}

/**
 * @brief MainWindow::setOffset
 * @param value
 *
 * Set offset and change value of ui spinbox
 */
void MainWindow::setOffset(qint64 value)
{
    offset = value;
    ui->offsetSpinBox->setRange(0,offset);
    ui->offsetSlider->setRange(0,offset);
    ui->offsetSpinBox->setValue(offset);
}

/**
 * @brief MainWindow::strippedName
 * @param fullFileName
 * @return the name of the file, excluding the path.
 */
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

/**
 * @brief MainWindow::on_actionExit_triggered
 *
 * Close application
 */
void MainWindow::on_actionExit_triggered()
{
    close();
}

/**
 * @brief MainWindow::on_actionFile_triggered
 * Opens and connects the dialog for select the file from the system to the mainapplication
 */
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
        connect(openFileDialog, SIGNAL(setOffset(qint64)), this, SLOT(setOffset(qint64)) );
    }
    openFileDialog = 0;
}

/**
 * @brief MainWindow::on_actionSave_triggered
 * @return correct save function
 */
bool MainWindow::on_actionSave_triggered()
{
    if (curFile.isEmpty()) {
        return on_actionSave_As_triggered();
    } else {
        return saveFile(curFile);
    }
}

/**
 * @brief MainWindow::on_actionSave_As_triggered
 * @return true if the file save corretly
 *
 * Get the save file name and type and saves it
 */
bool MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("%1 - Save image").arg(QApplication::applicationName()), ".",tr("Portable Network Graphics (*.png)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

/**
 * @brief MainWindow::on_contrastSlider_valueChanged
 * @param value
 *
 * Change contrast and enable the DrawPushButton
 */
void MainWindow::on_contrastSlider_valueChanged(int value)
{

    valueContrast = value;
    isContrast = true;
    ui->drawPushButton->setEnabled(true);

}

/**
 * @brief MainWindow::on_brightnessSlider_valueChanged
 * @param value
 *
 * Change the brightness and enable the DrawPushButton
 */
void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    valueBrightness = value;
    isBrightness = true;
    ui->drawPushButton->setEnabled(true);
}

/**
 * @brief MainWindow::on_actionUndo_triggered
 *
 * Restore the previous saved image from the snapshots
 */
void MainWindow::on_actionUndo_triggered()
{
    if(imageSnapshot.size() > 0){
        image = imageSnapshot.at(imageSnapshot.size()-1);
        imageSnapshot.removeLast();
        imageArea->setPixmap(QPixmap::fromImage(image));
    }
}

/**
 * @brief MainWindow::on_drawPushButton_clicked
 *
 * Snapshot the image and draw a new one with changed settings
 */
void MainWindow::on_drawPushButton_clicked()
{
    // Snapshot image
    if(imageArea->pixmap()){
        imageSnapshot.append(imageArea->pixmap()->toImage());
    }

    // Reset vectorcolors
    vectorColors.clear();

    // DRAW IMAGE
    drawImage();

    // Check and apply graphics settings
    if(isContrast){
        image = changeContrast(image, valueContrast);
        imageArea->setPixmap(pixmap.fromImage(image));
    } else if(isBrightness){
        image = changeBrightness(image, valueBrightness);
        imageArea->setPixmap(pixmap.fromImage(image));

    }
    // reset graphics settings
    isBrightness = false;
    isContrast = false;
    // disable draw button
    ui->drawPushButton->setEnabled(false);
}

/**
 * @brief MainWindow::setBitFormat
 * @param bitformat
 *
 * Set bitformat and change mainwindow ui option accordingly
 */
void MainWindow::setBitFormat(int bitformat)
{
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

/**
 * @brief MainWindow::drawImage
 *
 * Draw image to imageArea using user options
 */
void MainWindow::drawImage()
{

    // Check and apply offset setting
    if(offset > 0 && offsetonce){
        // Restore file data to prevent unwanted settings overlay
        restoreBlob();
        blob.remove(0,offset);
        offsetonce = false;
        setSizeImage();
    }

    if(!blob.isEmpty()){

        switch(bitFormat){

        // 1Bit Image using Format_Mono
        case 1:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            imageArea->resize(width,height);
            bitmap = bitmap.fromData(QSize(width,height), (const uchar*) toQBit(blob).data_ptr(),QImage::Format_Mono);
            imageArea->setPixmap(bitmap);
            image = bitmap.toImage();

            QApplication::restoreOverrideCursor();
            break;
        }

        // 4Bit Image using PGM format
        case 4:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            int scale = 15;
            imageArea->resize(width,height);

            QByteArray extracted_datapgm = convertToPGM(blob.data(),scale);

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

            QApplication::restoreOverrideCursor();
            break;
        }

        // 8Bit Image using palette file or PGM format
        case 8:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            imageArea->resize(width,height);

            // User selected a palette file
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

                QByteArray extracted_datapgm = convertToPGM(blob.data(),scale);

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

            QApplication::restoreOverrideCursor();
            break;
        }

        // 16Bit Image using Format_RGB444
        case 16:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            imageArea->resize(width,height);
            image = QImage(width, height, QImage::Format_RGB444);

            // Check and add padding if necessary
            if(blob.size()%2 != 0){
                for(int i = 0; i < blob.size()%2; i++)
                {
                    blob.append('0');
                }
            }

            for(int i = 0; i < blob.size()-1; i=i+2)
            {
                int blue = ((quint8)blob.at(i))>>4;
                int green= (quint8) blob.at(i)&0x0F;
                int red = ((quint8)blob.at(i+1))>>4;
                int alpha= (quint8) blob.at(i+1)&0x0F;

                vectorColors.append(qRgba(red,green,blue,alpha));
            }

            int k = 0;
            for (int x = 0; x < height; ++x) {
                for (int y = 0; y < width  ; ++y) {
                    image.setPixel(width-y-1, x, vectorColors.at(vectorColors.size()-k-1));
                    k++;
                }
            }

            imageArea->setPixmap(pixmap.fromImage(image));

            QApplication::restoreOverrideCursor();
            break;
        }

            // 24Bit Image using Format_RGB888
        case 24:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            imageArea->resize(width,height);

            // Check and add padding if necessary
            if(blob.size()%3 != 0){
                for(int i = 0; i < blob.size()%3; i++)
                {
                    blob.append('0');
                }
            }
            for(int i = 0; i < blob.size() - 2; i = i+3)
            {
                vectorColors.append(qRgb(blob.at(i), blob.at(i+1), blob.at(i+2)));
            }

            image = QImage(width, height, QImage::Format_RGB888);
            image.fill(QColor(Qt::white).rgb());

            int k = 0;
            for (int x = 0; x < height; ++x) {
                for (int y = 0; y < width  ; ++y) {
                    image.setPixel(width-y-1, x, vectorColors.at(vectorColors.size()-k-1));
                    k++;
                }
            }

            imageArea->setPixmap(QPixmap::fromImage(image));

            QApplication::restoreOverrideCursor();
            break;
        }
        default:
            break;
        }

        scaleFactor = 1.0;

        ui->actionFit_To_Window->setEnabled(true);
        updateActions();

        if (!ui->actionFit_To_Window->isChecked())
        {
            imageArea->adjustSize();
        }

    }

}

/**
 * @brief MainWindow::setSizeImage
 *
 * Set and Calculate the size of image
 * Biggest square size
 */
void MainWindow::setSizeImage()
{
    if(!blob.isEmpty()){
        switch(bitFormat)
        {
        case 1:
        {
            area = blob.size()*8;
            height = round(sqrt(area));
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);

            break;
        }
        case 4:
        {
            area = blob.size()*2;
            height = sqrt(area);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 8:
        {
            area = blob.size();
            height = sqrt(area);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 16:
        {
            area = blob.size()/2;
            height = sqrt(area);
            ui->heightSlider->setValue(height);
            width = height;
            ui->widthSlider->setValue(width);
            break;
        }
        case 24:
        {
            area = blob.size()/3;
            height = sqrt(area);
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

/**
 * @brief MainWindow::updateActions
 *
 * Update zoom ui option to provent wrong combinations
 */
void MainWindow::updateActions()
{
    ui->actionZoomIn->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionZoomOut->setEnabled(!ui->actionFit_To_Window->isChecked());
    ui->actionNormal_Size->setEnabled(!ui->actionFit_To_Window->isChecked());
}

/**
 * @brief MainWindow::scaleImage
 * @param factor Zoom Factor
 *
 * Scale image to zoom it and adjust scrollbar
 */
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

/**
 * @brief MainWindow::adjustScrollBar
 * @param scrollBar
 * @param factor
 *
 * Set scrollbar for image zoom accordingly
 */
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

/**
 * @brief MainWindow::convertToPGM
 * @param img_buffer blob.data()
 * @param scale Scale of Greys
 * @return QByteArray image pgm data
 *
 * Convert blob data to PGM format
 */
QByteArray MainWindow::convertToPGM(char* img_buffer,int scale)
{
    // PGM Header
    QByteArray pgm;
    // Temp blob data
    QByteArray raw;

    QDataStream out(&raw,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    // Check image format
    if(bitFormat == 8)
    {
        raw = blob;
    } else{
        for (int i = 0; i < blob.size(); i++)
        {
            // 4Bit case need to split
            const char nibble1 = img_buffer[i]&0x0F;
            const char nibble2 = img_buffer[i]>>4;

            out.writeRawData(&nibble1, sizeof(uchar));
            out.writeRawData(&nibble2, sizeof(uchar));
        }
    }

    // Save height
    qint64 h1 = height;

    // Build PGM Header
    pgm.clear();
    pgm.append("P5 ");
    pgm.append(QString::number(width));
    pgm.append(" ");
    pgm.append(QString::number(height));
    pgm.append(" ");
    pgm.append(QString::number(scale));
    pgm.append("\n");
    // Append blob data
    pgm.append(raw);

    // Restore height: bug Qstring::number??
    height = h1;

    return pgm;

}

/**
 * @brief MainWindow::on_actionZoomIn_triggered
 *
 * ZommIn image
 */
void MainWindow::on_actionZoomIn_triggered()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!curFile.isEmpty())
    {
        scaleImage(1.25);
    }
    QApplication::restoreOverrideCursor();
}

/**
 * @brief MainWindow::on_actionZoomIn_triggered
 *
 * ZommOut image
 */
void MainWindow::on_actionZoomOut_triggered()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!curFile.isEmpty())
    {
        scaleImage(0.8);
    }
    QApplication::restoreOverrideCursor();
}

/**
 * @brief MainWindow::on_actionNormal_Size_triggered
 *
 * Set image to standard size
 */
void MainWindow::on_actionNormal_Size_triggered()
{
    imageArea->adjustSize();
    scaleFactor = 1.0;
}

/**
 * @brief MainWindow::on_actionFit_To_Window_triggered
 *
 * Expand the image to fit the window area
 */
void MainWindow::on_actionFit_To_Window_triggered()
{
    bool fitToWindow = ui->actionFit_To_Window->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
    {
        on_actionNormal_Size_triggered();
    }
    updateActions();
}

/**
 * @brief MainWindow::on_actionAbout_triggered
 *
 * Open Abount info window
 */
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Progetto Piattaforme SW"),
                       tr("<p>XXXXX</p>"));
}

/**
 * @brief MainWindow::on_oneBitButton_clicked
 *
 * Set bitformat and size for 1bit Image
 */
void MainWindow::on_oneBitButton_clicked()
{
    bitFormat = 1;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();

    // Reset image effect
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

/**
 * @brief MainWindow::on_fourBitButton_clicked
 *
 * Set bitformat and size for 4bit Image
 */
void MainWindow::on_fourBitButton_clicked()
{
    bitFormat = 4;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();

    // Reset image effect
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

/**
 * @brief MainWindow::on_eightBitButton_clicked
 *
 * Set bitformat and size for 8bit image
 */
void MainWindow::on_eightBitButton_clicked()
{
    bitFormat = 8;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();

    // Reset image effect
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

/**
 * @brief MainWindow::on_sixteenBitButton_clicked
 *
 * Set bitformat and size for 16bit image
 */
void MainWindow::on_sixteenBitButton_clicked()
{
    bitFormat = 16;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();

    // Reset image effect
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

/**
 * @brief MainWindow::on_twentyfourBitButton_clicked
 *
 * Set bitformat and size for 24bit image
 */
void MainWindow::on_twentyfourBitButton_clicked()
{
    bitFormat = 24;
    ui->drawPushButton->setEnabled(true);
    setSizeImage();

    // Reset image effect
    ui->brightnessSlider->setValue(0);
    ui->contrastSlider->setValue(0);
}

/**
 * @brief MainWindow::mousePressEvent
 * @param e
 *
 * Handle mouse press event and create a rubbberband for save that area later
 */
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    // Check if a rubberband exist
    if (!rubberBand)
    {
        origin = e->pos();
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
    }
}

/**
 * @brief MainWindow::mouseMoveEvent
 * @param e
 *
 * Handle the mouse move event and set correct rubberband geometry
 */
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    // Check for a correct origin
    if (!origin.isNull())
    {
        rubberBand->setGeometry(QRect(origin, e->pos()).normalized());
    }
}

/**
 * @brief MainWindow::mouseReleaseEvent
 * @param e
 *
 * Handle the mouse release event and hide the created rubberband
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    // Check for a correct origin
    if (!origin.isNull())
    {
        rubberBand->hide();
    }

}

/**
 * @brief MainWindow::on_saveAreaButton_clicked
 * @return
 *
 * Save the image area selected by the rubberband
 * //TODO implement saving of the data
 */
bool MainWindow::on_saveAreaButton_clicked()
{
    if(ui->imageCheckBox->isChecked() && rubberBand != NULL)
    {

        QPixmap regioncapture = this->grab(rubberBand->geometry());

        QFile file(curFile+".png");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        regioncapture.save(&file, "PNG");

        // Reset RubberBand
        rubberBand = NULL;

        return true;

    }
    else if (ui->dataCheckBox->isChecked() && rubberBand != NULL)
    {
        //  (x,y)   (x,y2)
        //
        //  (x2,y)  (x2,y2)
        //
        // Convert Width to Byte size
        int widthByteSize = 0;
        if(bitFormat==1)
        {
            widthByteSize = width/8;
        }
        else if(bitFormat == 4)
        {
            widthByteSize = width/2;
        }
        else if(bitFormat == 8)
        {
            widthByteSize = width;
        }
        else if(bitFormat == 16)
        {
            widthByteSize = width*2;
        }
        else if(bitFormat == 24)
        {
            widthByteSize = width*3;
        }

        // Calculate cordinates
        int x = origin.x() - imageArea->pos().x();
        int y = origin.y() - imageArea->pos().y() ;
        int x2 = x + rubberBand->height();
        //int y2 = y + rubberBand->width();
        QByteArray temp;

        QApplication::setOverrideCursor(Qt::WaitCursor);
        // Grab data
        for(int i = x;i <= x2;i++){
            int pos = (i-1)*(widthByteSize) + y;
            temp.append(blob.mid(pos,rubberBand->width()));
        }
        QApplication::restoreOverrideCursor();

        // Save blob to file
        QFile file(curFile+".blob");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return false;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_2);
        out << temp;


        // Reset RubberBand
        rubberBand = NULL;
        return true;
    }

    QMessageBox::warning(this, tr("ProgettoPiattaformeSW"),"Area not selected");

    return false;
}

/**
 * @brief MainWindow::toQBit
 * @param blob
 * @return QBitArray converterd blob
 *
 * Convert QByteArray to QBitArray
 */
QBitArray MainWindow::toQBit (QByteArray blob)
{
    int const bitsInByte= 8;
    int const bytsInObject= blob.size();

    const char *data = blob.data() ;
    QBitArray result(bytsInObject*bitsInByte);
    for ( int byte=0; byte<bytsInObject ; ++byte ) {
        for ( int bit=0; bit<bitsInByte; ++bit ) {
            result.setBit ( byte*bitsInByte + bit, data[byte] & (1<<bit) ) ;
        }
    }
    // Check and add padding if necessary
    if(result.size()%8 == 0)
    {
        return result;
    }else
    {
        // Add padding resizing QBitArray (new bit default = 0)
        result.resize(result.size() + result.size()%8);
        return result;
    }
}


/**
 * @brief MainWindow::on_offsetSpinBox_valueChanged
 * @param value
 *
 * Set offset value and settings
 */
void MainWindow::on_offsetSpinBox_valueChanged(int value)
{
    offsetonce = true;
    offset = value;
    ui->drawPushButton->setEnabled(true);
}

/**
 * @brief MainWindow::restoreBlob
 *
 * Restore the inital saved blob
 */
void MainWindow::restoreBlob()
{
    blob = blobBackup;
}

/**
 * @brief MainWindow::backupBlob
 *
 * Save the initial file blob
 */
void MainWindow::backupBlob()
{
    blobBackup = blob;
}

/**
 * @brief MainWindow::on_heightSlider_valueChanged
 * @param value
 *
 * Change height value and decrease width to prevent seg fault
 */
void MainWindow::on_heightSlider_valueChanged(int value)
{
    height = value;
    ui->drawPushButton->setEnabled(true);

    if((height*width) > area)
    {
        width = round(((double) area )/ height);
        ui->widthSpinBox->setValue(width);
    }

    ui->heightSpinBox->setValue(height);
}

/**
 * @brief MainWindow::on_widthSlider_valueChanged
 * @param value
 *
 * Change width value and decrease height to prevent seg fault
 */
void MainWindow::on_widthSlider_valueChanged(int value)
{
    width = value;
    ui->drawPushButton->setEnabled(true);

    if((height*width) > area)
    {
        height = round(((double) area) / width);
        ui->heightSpinBox->setValue(height);
    }

    ui->widthSpinBox->setValue(width);
}

// Overriding QBitmap::fromData
QBitmap QBitmap::fromData(const QSize &size, const uchar *bits, QImage::Format monoFormat)
{
    Q_ASSERT(monoFormat == QImage::Format_Mono || monoFormat == QImage::Format_MonoLSB);

    QImage image(size, monoFormat);
    image.setColor(0, QColor(Qt::color0).rgb());
    image.setColor(1, QColor(Qt::color1).rgb());

    // Need to memcpy each line separatly since QImage is 32bit aligned and
    // this data is only byte aligned...
    // Original
    //  int bytesPerLine = (size.width() + 7) / 8;
    int bytesPerLine = size.width()/ 8;
    for (int y = 0; y < size.height(); ++y)
        memcpy(image.scanLine(y), bits + bytesPerLine * y, bytesPerLine);
    return QBitmap::fromImage(image);
}
