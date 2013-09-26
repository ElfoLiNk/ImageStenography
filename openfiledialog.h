#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QDialog>

namespace Ui {
class OpenFileDialog;
}

class OpenFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenFileDialog(QWidget *parent = 0);
    ~OpenFileDialog();

signals:
    void setFileName(QString fileName);
    void setPaletteName(QString fileName);
    void openFile();
    void fileBitFormat(const int&);
    
private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

    void on_paletteButton_clicked();


private:
    Ui::OpenFileDialog *ui;

    int findBitFormat();

    QString fileName;
    QString paletteName;
};

#endif // OPENFILEDIALOG_H
