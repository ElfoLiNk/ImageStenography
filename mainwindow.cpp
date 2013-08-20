#include <QMessageBox>
#include <QFileDialog>
#include "inputbitdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inputBitDialog = 0;
    setWindowIcon(QIcon(":/images/icon.png"));
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
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_1);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // out << Qimage
    QApplication::restoreOverrideCursor();
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
    QImage image = QImage(fileName);
    QLabel *myLabel = this->getImageLabel();
    myLabel->setPixmap(QPixmap::fromImage(image));
    return true;
}
bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save image"), ".",tr("Spreadsheet files (*.sp)"));
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

QLabel* MainWindow::getImageLabel(){
    QLabel *myLabel = ui->imageArea;
    return myLabel;
}

void MainWindow::on_actionNew_triggered()
{
    if (okToContinue()) {
        //->clear();
        //setCurrentFile("");
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
