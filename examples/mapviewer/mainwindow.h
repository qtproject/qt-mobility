#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QSlider>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include "qmapview.h"
#include "qroutereply.h"
#include "qgeonetworkmanager.h"

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
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    void mapClicked(QGeoCoordinateMaps geoCoord, QGraphicsSceneMouseEvent* mouseEvent);
    void routeReplyFinished(QRouteReply* reply);
    void setRtFromTo(bool checked);
    void zoomLevelChanged(quint16 oldZoomLevel, quint16 newZoomLevel);
    void setScheme(bool checked);
    void addMarker(bool checked);
    void drawLine(bool checked);
    void drawRect(bool checked);
    void drawPolygon(bool checked);

private:
    Ui::MainWindow *ui;

private:
    QGraphicsView* qgv;
    QGeoNetworkManager geoNetworkManager;
    QMapView* mapView;
    QSlider* slider;
    QMenu* popupMenu;
    QAction* mnMarker;
    QAction* mnRoute;
    QAction* mnLine;
    QAction* mnRect;
    QAction* mnPolygon;
    QAction* mnDay;
    QAction* mnSat;
    QAction* mnTer;

    QGeoCoordinateMaps lastClicked;

    QList<const QMapMarker*> selectedMarkers;
};

#endif // MAINWINDOW_H
