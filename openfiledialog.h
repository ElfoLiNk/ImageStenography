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
    void setOffset(qint64 offset);
    void openFile();
    void fileBitFormat(int);
    
private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

    void on_paletteButton_clicked();

    void on_spinBox_valueChanged(int value);

private:
    Ui::OpenFileDialog *ui;

    int findBitFormat();

    QString fileName;
    QString paletteName;
    qint64 offset;
};

#endif // OPENFILEDIALOG_H
