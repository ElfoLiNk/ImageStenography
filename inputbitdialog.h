#ifndef INPUTBITDIALOG_H
#define INPUTBITDIALOG_H

#include <QDialog>

namespace Ui {
class InputBitDialog;
}

class InputBitDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InputBitDialog(QWidget *parent = 0);
    ~InputBitDialog();
    
private:
    Ui::InputBitDialog *ui;
};

#endif // INPUTBITDIALOG_H
