#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QBitmap>

namespace Ui {
class MainWindow;
}

class InputBitDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
    bool save();
    bool saveAs();

    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionFile_triggered();

    void on_actionInput_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_actionUndo_triggered();

    void on_pushButton_clicked();

    void on_radioButton_toggled(bool checked);

    void on_radioButton_2_toggled(bool checked);

    void on_radioButton_3_toggled(bool checked);

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
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName);
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

    InputBitDialog *inputBitDialog;

    QBitmap bitmap;
    QImage image;
    QVector<QImage> imageSnapshot;
    int valueContrast;
    int valueBrightness;
    bool isBrightness;
    bool isContrast;

};

#endif // MAINWINDOW_H
