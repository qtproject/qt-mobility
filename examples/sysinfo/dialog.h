#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
class QSystemScreenSaver;
class QSystemInfo;
#include <qsysteminfo.h>

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
    void setupSaver();

    QSystemScreenSaver *saver;
    QSystemInfo *systemInfo;
    QSystemDeviceInfo *di;

private slots:
    void tabChanged(int index);
    void getVersion(int index);
    void getFeature(int index);
    void setSaverEnabled(bool b);
    void setBlankingEnabled(bool b);
    void netStatusComboActivated(int);
    void updateBatteryStatus(int);
    void updatePowerState(QSystemDeviceInfo::PowerState);
    void displayBatteryStatus(QSystemDeviceInfo::BatteryStatus);
};

#endif // DIALOG_H
