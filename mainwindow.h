#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

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
    //    void about();
    //    void updateStatusBar();
    //    void imageModified();
    
    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionFile_triggered();

    void on_actionInput_triggered();

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
    QLabel* getImageLabel();

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

    QImage *image;

};

#endif // MAINWINDOW_H
