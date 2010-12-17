#ifndef PINDISPLAY_H
#define PINDISPLAY_H

#include <QDialog>

namespace Ui {
    class pinDisplay;
}

class pinDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit pinDisplay(QString title, QString pin, QWidget *parent = 0);
    ~pinDisplay();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::pinDisplay *ui;
};

#endif // PINDISPLAY_H
