#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QBitmap>
#include <QScrollArea>
#include <QRubberBand>
#include <QBitArray>
#include <QColorDialog>
#include <openfiledialog.h>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
   //void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void setCurrentFile(const QString &fileName);
    void setPaletteFile(const QString &fileName);
    void setOffset(qint64);
    void setBitFormat(int bitformat);
    void open();

    void on_actionExit_triggered();

    void on_actionFile_triggered();

    bool on_actionSave_triggered();

    bool on_actionSave_As_triggered();

    void on_contrastSlider_valueChanged(int value);

    void on_brightnessSlider_valueChanged(int value);

    void on_actionUndo_triggered();

    void on_drawPushButton_clicked();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_actionNormal_Size_triggered();

    void on_actionFit_To_Window_triggered();

    void on_actionAbout_triggered();

    void on_oneBitButton_clicked();

    void on_fourBitButton_clicked();

    void on_eightBitButton_clicked();

    void on_sixteenBitButton_clicked();

    void on_twentyfourBitButton_clicked();

    bool on_saveAreaButton_clicked();

    void on_pixelFillpushButton_clicked();

    void on_offsetSpinBox_valueChanged(int value);

    void on_heightSlider_valueChanged(int value);

    void on_widthSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    OpenFileDialog *openFileDialog;

    void restoreBlob();
    void backupBlob();

    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName);
    void drawImage();
    QImage contrast(QImage& source, int factor);
    QImage brighten(QImage& source, int factor);
    QByteArray convertToPGM(char* img_buffer, int scale);
    void setSizeImage();
    QBitArray toQBit (QByteArray blob);

    double scaleFactor;


    qint64 offset;
    bool offsetonce;
    QLabel *imageArea;
    QString curFile;
    QString paletteFile;
    QBitmap bitmap;
    QPixmap pixmap;
    QImage image;
    QVector<QImage> imageSnapshot;
    int valueContrast;
    int valueBrightness;
    qint64 height;
    qint64 width;
    qint64 area;
    bool isBrightness;
    bool isContrast;
    int bitFormat;
    QByteArray blob;
    QByteArray blobBackup;
    QBitArray bitblob;
    QVector<QRgb> vectorColors;

    QRubberBand *rubberBand;
    QRect selectionRect;
    QPoint origin;
    QColor colorPixFake;


};

#endif // MAINWINDOW_H
