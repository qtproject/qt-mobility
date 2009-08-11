#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Dialog *ui;
    void setupGeneral();
    void setupDevice();
    void setupDisplay();
    void setupMemory();
    void setupNetwork();


private slots:
    void tabChanged(int index);
    void getVersion(int index);
    void getFeature(int index);
    void doVolumes(int index);
};

#endif // DIALOG_H
