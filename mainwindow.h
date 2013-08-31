#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QBitmap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open(int bitpixel);
    bool save();
    bool saveAs();

    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionFile_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_contrastSlider_valueChanged(int value);

    void on_brightnessSlider_valueChanged(int value);

    void on_actionUndo_triggered();

    void on_drawPushButton_clicked();

//    void on_radioButton_toggled(bool checked);

//    void on_radioButton_2_toggled(bool checked);

//    void on_radioButton_3_toggled(bool checked);

    void on_oneBitButton_toggled(bool checked);

    void on_fourBitButton_toggled(bool checked);

    void on_eightBitButton_toggled(bool checked);

    void on_sixteenBitButton_toggled(bool checked);

    void on_twentyfourBitButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName, int bitpixel);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName, int bitpixel);
    QImage contrast(QImage& source, int factor);
    QImage brighten(QImage& source, int factor);

    QMenu *fileMenu;

    QToolBar *fileToolBar;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *aboutQtAction;

    QString curFile;

    QBitmap bitmap;
    QImage image;
    QVector<QImage> imageSnapshot;
    int valueContrast;
    int valueBrightness;
    bool isBrightness;
    bool isContrast;

};

#endif // MAINWINDOW_H
