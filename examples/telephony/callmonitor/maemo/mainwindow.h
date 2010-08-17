#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QStringListModel>
#include <qtelephonycallinfo.h>

namespace QtMobility
{
    class QTelephonyCallList;
}

namespace Ui {
    class MainWindow;
}

using namespace QtMobility;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void addListEntry(QString value);

private slots:
    void onActiveCallAdded(const QTelephonyCallInfo& call);
    void onActiveCallStatusChanged(const QTelephonyCallInfo& call);
private:
    Ui::MainWindow *ui;
    QStringListModel m_rxDBusMsg;
    QTelephonyCallList *telephoycalllist;
};

#endif // MAINWINDOW_H
