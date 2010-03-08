#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qgeonetworkmanager.h"
#include "qroutereply.h"
#include "qgeocodingreply.h"

namespace Ui
{
class MainWindow;
}

QTM_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

private slots:
    void on_btnRequestTile_clicked();
    void on_btnCoding_clicked();
    void on_btnReverseCoding_clicked();
    void on_btnRequest_clicked();
    void testReplyFinishedSignal();

    void routeReplyFinished(QRouteReply* reply);
    void codingReplyFinished(QGeocodingReply* reply);
    void mapTileReplyFinished(QMapTileReply* reply);

private:
    QGeoNetworkManager geoNetworkManager;

};

#endif // MAINWINDOW_H
