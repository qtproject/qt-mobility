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
    void addListEntry(const QString& event, const QTelephonyCallInfo& call);

private slots:
    void onActiveCallAdded(const QTelephonyCallInfo& call);
    void onActiveCallRemoved(const QTelephonyCallInfo& call);
    void onActiveCallStatusChanged(const QTelephonyCallInfo& call);
    void onActiveCallCountChanged();
    void onClearList();
private:
    Ui::MainWindow *ui;
    QStringListModel m_rxDBusMsg;
    QTelephonyCallList *telephonycalllist;
};

#endif // MAINWINDOW_H
