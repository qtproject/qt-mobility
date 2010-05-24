/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

// QtMobility API headers
#include <qmobilityglobal.h>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include "satellitedialog.h"
#include "connectivityhelper.h"

// Use the QtMobility namespace
QTM_USE_NAMESPACE

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// how long (milliseconds) the user need to hold (after a tap on the screen)
// before triggering the magnifying glass feature
// 701, a prime number, is the sum of 229, 233, 239
// (all three are also prime numbers, consecutive!)
#define HOLD_TIME 701

// maximum size of the magnifier
// Hint: see above to find why I picked this one :)
#define MAX_MAGNIFIER 229

QT_BEGIN_NAMESPACE
uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}
QT_END_NAMESPACE

// tile size in pixels
const int tdim = 256;

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0;
    return QPointF(tx * zn, ty * zn);
}

qreal longitudeFromTile(qreal tx, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal lat = tx / zn * 360.0 - 180.0;
    return lat;
}

qreal latitudeFromTile(qreal ty, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal n = M_PI - 2 * M_PI * ty / zn;
    qreal lng = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    return lng;
}

class SlippyMap: public QObject
{
    Q_OBJECT

public:
    int width;
    int height;
    int zoom;
    qreal latitude;
    qreal longitude;

    SlippyMap(QNetworkSession *session, QGeoPositionInfoSource *location, QObject *parent = 0) :
            QObject(parent),
            width(400),
            height(300),
            zoom(15),
            latitude(59.9138204),
            longitude(10.7387413),
            m_location(location),
            m_session(session) {
        m_emptyTile = QPixmap(tdim, tdim);
        m_emptyTile.fill(Qt::lightGray);

        m_manager = new QNetworkAccessManager(this);

        QNetworkDiskCache *cache = new QNetworkDiskCache;
        cache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
        m_manager->setCache(cache);
        connect(m_manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleNetworkData(QNetworkReply*)));

        // Listen gps position changes
        connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
    }

    ~SlippyMap() {
        for (int i = 0; i < m_pendingReplies.size(); ++i) {
            delete m_pendingReplies.at(i);
        }
    }

    void invalidate() {
        if (width <= 0 || height <= 0)
            return;

        QPointF ct = tileForCoordinate(latitude, longitude, zoom);
        qreal tx = ct.x();
        qreal ty = ct.y();

        // top-left corner of the center tile
        int xp = width / 2 - (tx - floor(tx)) * tdim;
        int yp = height / 2 - (ty - floor(ty)) * tdim;

        // first tile vertical and horizontal
        int xa = (xp + tdim - 1) / tdim;
        int ya = (yp + tdim - 1) / tdim;
        int xs = static_cast<int>(tx) - xa;
        int ys = static_cast<int>(ty) - ya;

        // offset for top-left tile
        m_offset = QPoint(xp - xa * tdim, yp - ya * tdim);

        // last tile vertical and horizontal
        int xe = static_cast<int>(tx) + (width - xp - 1) / tdim;
        int ye = static_cast<int>(ty) + (height - yp - 1) / tdim;

        // build a rect
        m_tilesRect = QRect(xs, ys, xe - xs + 1, ye - ys + 1);

        if (m_url.isEmpty())
            download();

        emit updated(QRect(0, 0, width, height));
    }

    void render(QPainter *p, const QRect &rect) {
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp(x + m_tilesRect.left(), y + m_tilesRect.top());
                QRect box = tileRect(tp);
                if (rect.intersects(box)) {
                    if (m_tilePixmaps.contains(tp))
                        p->drawPixmap(box, m_tilePixmaps.value(tp));
                    else
                        p->drawPixmap(box, m_emptyTile);
                }
            }
    }

    void pan(const QPoint &delta) {
        QPointF dx = QPointF(delta) / qreal(tdim);
        QPointF center = tileForCoordinate(latitude, longitude, zoom) - dx;
        latitude = latitudeFromTile(center.y(), zoom);
        longitude = longitudeFromTile(center.x(), zoom);
        invalidate();
    }

private slots:

    void positionUpdated(const QGeoPositionInfo &gpsPos) {
        latitude = gpsPos.coordinate().latitude();
        longitude = gpsPos.coordinate().longitude();
        invalidate();
    }

    void handleNetworkData(QNetworkReply *reply) {
        QImage img;
        QPoint tp = reply->request().attribute(QNetworkRequest::User).toPoint();
        QUrl url = reply->url();
        if (!reply->error())
            if (!img.load(reply, 0))
                img = QImage();

        for (int i = 0; i < m_pendingReplies.size(); ++i) {
            if (m_pendingReplies.at(i) == reply) {
                m_pendingReplies.removeAt(i);
                break;
            }
        }

        reply->deleteLater();
        m_tilePixmaps[tp] = QPixmap::fromImage(img);
        if (img.isNull())
            m_tilePixmaps[tp] = m_emptyTile;emit
        updated(tileRect(tp));

        // purge unused spaces
        QRect bound = m_tilesRect.adjusted(-2, -2, 2, 2);
        foreach(QPoint tp, m_tilePixmaps.keys())
        if (!bound.contains(tp))
            m_tilePixmaps.remove(tp);

        download();
    }

    void download() {
        QPoint grab(0, 0);
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp = m_tilesRect.topLeft() + QPoint(x, y);
                if (!m_tilePixmaps.contains(tp)) {
                    grab = tp;
                    break;
                }
            }
        if (grab == QPoint(0, 0)) {
            m_url = QUrl();
            return;
        }

        QString path = "http://tile.openstreetmap.org/%1/%2/%3.png";
        m_url = QUrl(path.arg(zoom).arg(grab.x()).arg(grab.y()));
        QNetworkRequest request;
        request.setUrl(m_url);
        request.setRawHeader("User-Agent", "Nokia (Qt) Graphics Dojo 1.0");
        request.setAttribute(QNetworkRequest::User, QVariant(grab));
        m_pendingReplies << m_manager->get(request);
    }

signals:
    void updated(const QRect &rect);

protected:
    QRect tileRect(const QPoint &tp) {
        QPoint t = tp - m_tilesRect.topLeft();
        int x = t.x() * tdim + m_offset.x();
        int y = t.y() * tdim + m_offset.y();
        return QRect(x, y, tdim, tdim);
    }

private:
    QPoint m_offset;
    QRect m_tilesRect;
    QPixmap m_emptyTile;
    QHash<QPoint, QPixmap> m_tilePixmaps;
    QNetworkAccessManager *m_manager;
    QUrl m_url;

    QGeoPositionInfoSource* m_location;
    QNetworkSession* m_session;
    QList<QNetworkReply*> m_pendingReplies;

};

class LightMaps: public QWidget
{
    Q_OBJECT

public:
    LightMaps(QWidget *parent = 0) :
            QWidget(parent),
            m_normalMap(0),
            m_largeMap(0),
            firstLat(0.0),
            firstLong(0.0),
            pressed(false),
            snapped(false),
            zoomed(false),
            invert(false),
            m_usingLogFile(false),
            m_location(0),
            waitingForFix(false) {

        // Set Internet Access Point
        QNetworkConfigurationManager manager;
        const bool canStartIAP = (manager.capabilities()
                                  & QNetworkConfigurationManager::CanStartAndStopInterfaces);

        // Is there default access point, use it
        QTM_PREPEND_NAMESPACE(QNetworkConfiguration) cfg1 = manager.defaultConfiguration();
        if (!cfg1.isValid() || (!canStartIAP && cfg1.state() != QTM_PREPEND_NAMESPACE(QNetworkConfiguration)::Active)) {
            m_networkSetupError = QString(tr("This example requires networking, and no avaliable networks or access points could be found."));
            QTimer::singleShot(0, this, SLOT(networkSetupError()));
            return;
        }

        m_session = new QNetworkSession(cfg1, this);
        m_connectivityHelper = new ConnectivityHelper(m_session, this);
        connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
        connect(m_connectivityHelper, SIGNAL(networkingCancelled()), qApp, SLOT(quit()));

        m_session->open();
    }

    ~LightMaps() {
        m_session->close();
        if (m_location)
            m_location->stopUpdates();
    }

    void stopPositioning() {
        if (m_location)
            m_location->stopUpdates();
    }

    void startPositioning() {
        if (m_location)
            m_location->startUpdates();
    }

    void setCenter(qreal lat, qreal lng) {
        if (!m_normalMap || !m_largeMap) {
            firstLat = lat;
            firstLong = lng;
            return;
        }
        m_normalMap->latitude = lat;
        m_normalMap->longitude = lng;
        m_normalMap->invalidate();
        m_largeMap->latitude = lat;
        m_largeMap->longitude = lng;
        m_largeMap->invalidate();
    }

public slots:

    void toggleNightMode() {
        invert = !invert;
        update();
    }

private slots:

    void networkSetupError() {
        QMessageBox::critical(this, tr("LightMaps"),
                                 m_networkSetupError);
        QTimer::singleShot(0, qApp, SLOT(quit()));
    }

    void networkSessionOpened() {
        m_location = QGeoPositionInfoSource::createDefaultSource(this);

        if (!m_location) {
            QNmeaPositionInfoSource *nmeaLocation = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, this);
            QFile *logFile = new QFile(QApplication::applicationDirPath()
                                       + QDir::separator() + "nmealog.txt", this);
            nmeaLocation->setDevice(logFile);
            m_location = nmeaLocation;
            m_usingLogFile = true;
        }

        m_location->setUpdateInterval(10000);

        connect(m_location,
                SIGNAL(positionUpdated(QGeoPositionInfo)),
                this,
                SLOT(positionUpdated(QGeoPositionInfo)));

        if (m_usingLogFile) {
            QMessageBox::information(this, tr("LightMaps"),
                                     tr("No GPS support detected, using GPS data from a sample log file instead."));
        } else {
            waitForFix();
            m_location->stopUpdates();
        }

        m_normalMap = new SlippyMap(m_session, m_location, this);
        m_largeMap = new SlippyMap(m_session, m_location, this);

        connect(m_normalMap, SIGNAL(updated(QRect)), SLOT(updateMap(QRect)));
        connect(m_largeMap, SIGNAL(updated(QRect)), SLOT(update()));

        setCenter(firstLat, firstLong);

        m_normalMap->width = width();
        m_normalMap->height = height();
        m_largeMap->width = m_normalMap->width * 2;
        m_largeMap->height = m_normalMap->height * 2;

        connect(m_location, SIGNAL(updateTimeout()), this, SLOT(waitForFix()));

        startPositioning();
    }

    // Brings up a satellite strength dialog box until a position fix is received.
    // This will also start the position updates if they are not already started.
    void waitForFix() {
        if (waitingForFix)
            return;

        waitingForFix = true;

        QGeoSatelliteInfoSource *m_satellite = QGeoSatelliteInfoSource::createDefaultSource(this);

        if (m_satellite) {
            SatelliteDialog *dialog = new SatelliteDialog(this,
                    30,
                    SatelliteDialog::ExitOnFixOrCancel,
                    SatelliteDialog::OrderByPrnNumber,
                    SatelliteDialog::ScaleToMaxPossible);

            dialog->connectSources(m_location, m_satellite);

            m_location->startUpdates();
            m_satellite->startUpdates();

            dialog->exec();

            m_satellite->stopUpdates();

            delete dialog;
            delete m_satellite;
        }

        waitingForFix = false;
    }

    void positionUpdated(const QGeoPositionInfo &pos) {
        setCenter(pos.coordinate().latitude(), pos.coordinate().longitude());
    }

    void updateMap(const QRect &r) {
        update(r);
    }

protected:

    void activateZoom() {
        stopPositioning();
        zoomed = true;
        tapTimer.stop();
        m_largeMap->zoom = m_normalMap->zoom + 1;
        m_largeMap->width = m_normalMap->width * 2;
        m_largeMap->height = m_normalMap->height * 2;
        m_largeMap->latitude = m_normalMap->latitude;
        m_largeMap->longitude = m_normalMap->longitude;
        m_largeMap->invalidate();
        update();
    }

    void resizeEvent(QResizeEvent *) {
        if (!m_normalMap || !m_largeMap)
            return;

        m_normalMap->width = width();
        m_normalMap->height = height();
        m_normalMap->invalidate();
        m_largeMap->width = m_normalMap->width * 2;
        m_largeMap->height = m_normalMap->height * 2;
        m_largeMap->invalidate();
    }

    void paintEvent(QPaintEvent *event) {
        if (!m_normalMap || !m_largeMap)
            return;

        QPainter p;
        p.begin(this);
        m_normalMap->render(&p, event->rect());
        p.setPen(Qt::black);
#if defined(Q_OS_SYMBIAN)
        QFont font = p.font();
        font.setPixelSize(13);
        p.setFont(font);
#endif
        p.drawText(rect(), Qt::AlignBottom | Qt::TextWordWrap,
                   "Map data CCBYSA 2009 OpenStreetMap.org contributors");
        p.end();

        if (zoomed) {
            int dim = qMin(width(), height());
            int magnifierSize = qMin(MAX_MAGNIFIER, dim * 2 / 3);
            int radius = magnifierSize / 2;
            int ring = radius - 15;
            QSize box = QSize(magnifierSize, magnifierSize);

            // reupdate our mask
            if (maskPixmap.size() != box) {
                maskPixmap = QPixmap(box);
                maskPixmap.fill(Qt::transparent);

                QRadialGradient g;
                g.setCenter(radius, radius);
                g.setFocalPoint(radius, radius);
                g.setRadius(radius);
                g.setColorAt(1.0, QColor(255, 255, 255, 0));
                g.setColorAt(0.5, QColor(128, 128, 128, 255));

                QPainter mask(&maskPixmap);
                mask.setRenderHint(QPainter::Antialiasing);
                mask.setCompositionMode(QPainter::CompositionMode_Source);
                mask.setBrush(g);
                mask.setPen(Qt::NoPen);
                mask.drawRect(maskPixmap.rect());
                mask.setBrush(QColor(Qt::transparent));
                mask.drawEllipse(g.center(), ring, ring);
                mask.end();
            }

            QPoint center = dragPos - QPoint(0, radius);
            center = center + QPoint(0, radius / 2);
            QPoint corner = center - QPoint(radius, radius);

            QPoint xy = center * 2 - QPoint(radius, radius);

            // only set the dimension to the magnified portion
            if (zoomPixmap.size() != box) {
                zoomPixmap = QPixmap(box);
                zoomPixmap.fill(Qt::lightGray);
            }
            if (true) {
                QPainter p(&zoomPixmap);
                p.translate(-xy);
                m_largeMap->render(&p, QRect(xy, box));
                p.end();
            }

            QPainterPath clipPath;
            clipPath.addEllipse(center, ring, ring);

            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            p.setClipPath(clipPath);
            p.drawPixmap(corner, zoomPixmap);
            p.setClipping(false);
            p.drawPixmap(corner, maskPixmap);
            p.setPen(Qt::gray);
            p.drawPath(clipPath);
        }
        if (invert) {
            QPainter p(this);
            p.setCompositionMode(QPainter::CompositionMode_Difference);
            p.fillRect(event->rect(), Qt::white);
            p.end();
        }
    }

    void timerEvent(QTimerEvent *) {
        if (!zoomed)
            activateZoom();
        update();
    }

    void mousePressEvent(QMouseEvent *event) {
        if (!m_normalMap || !m_largeMap)
            return;

        if (event->buttons() != Qt::LeftButton)
            return;
        pressed = snapped = true;
        pressPos = dragPos = event->pos();
        tapTimer.stop();
        tapTimer.start(HOLD_TIME, this);
    }

    void mouseMoveEvent(QMouseEvent *event) {
        if (!m_normalMap || !m_largeMap)
            return;

        if (!event->buttons())
            return;

        stopPositioning();

        if (!zoomed) {
            if (!pressed || !snapped) {
                QPoint delta = event->pos() - pressPos;
                pressPos = event->pos();
                m_normalMap->pan(delta);
                return;
            } else {
                const int threshold = 10;
                QPoint delta = event->pos() - pressPos;
                if (snapped) {
                    snapped &= delta.x() < threshold;
                    snapped &= delta.y() < threshold;
                    snapped &= delta.x() > -threshold;
                    snapped &= delta.y() > -threshold;
                }
                if (!snapped)
                    tapTimer.stop();
            }
        } else {
            dragPos = event->pos();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *) {
        if (!m_normalMap || !m_largeMap)
            return;

        zoomed = false;
        update();
    }

    void keyPressEvent(QKeyEvent *event) {
        if (!m_normalMap || !m_largeMap)
            return;

        if (!zoomed) {
            if (event->key() == Qt::Key_Left)
                m_normalMap->pan(QPoint(20, 0));
            if (event->key() == Qt::Key_Right)
                m_normalMap->pan(QPoint(-20, 0));
            if (event->key() == Qt::Key_Up)
                m_normalMap->pan(QPoint(0, 20));
            if (event->key() == Qt::Key_Down)
                m_normalMap->pan(QPoint(0, -20));
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                dragPos = QPoint(width() / 2, height() / 2);
                activateZoom();
            }
        } else {
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                zoomed = false;
                update();
            }
            QPoint delta(0, 0);
            if (event->key() == Qt::Key_Left)
                delta = QPoint(-15, 0);
            if (event->key() == Qt::Key_Right)
                delta = QPoint(15, 0);
            if (event->key() == Qt::Key_Up)
                delta = QPoint(0, -15);
            if (event->key() == Qt::Key_Down)
                delta = QPoint(0, 15);
            if (delta != QPoint(0, 0)) {
                dragPos += delta;
                update();
            }
        }
    }

private:
    QString m_networkSetupError;
    SlippyMap *m_normalMap;
    SlippyMap *m_largeMap;
    qreal firstLat;
    qreal firstLong;
    bool pressed;
    bool snapped;
    QPoint pressPos;
    QPoint dragPos;
    QBasicTimer tapTimer;
    bool zoomed;
    QPixmap zoomPixmap;
    QPixmap maskPixmap;
    bool invert;
    bool m_usingLogFile;
    QGeoPositionInfoSource *m_location;
    bool waitingForFix;
    QNetworkSession *m_session;
    ConnectivityHelper *m_connectivityHelper;
};

class MapZoom: public QMainWindow
{
    Q_OBJECT

private:
    LightMaps *map;

public:
    MapZoom() :
            QMainWindow(0) {
        map = new LightMaps(this);
        setCentralWidget(map);
        map->setFocus();

        QAction *gpsAction = new QAction("Start GPS", this);
        QAction *osloAction = new QAction("&Oslo", this);
        QAction *berlinAction = new QAction("&Berlin", this);
        QAction *jakartaAction = new QAction("&Jakarta", this);
        QAction *nightModeAction = new QAction("Night Mode", this);

        nightModeAction->setCheckable(true);
        nightModeAction->setChecked(false);
        QAction *osmAction = new QAction("About OpenStreetMap", this);
        connect(gpsAction, SIGNAL(triggered()), SLOT(chooseGps()));
        connect(osloAction, SIGNAL(triggered()), SLOT(chooseOslo()));
        connect(berlinAction, SIGNAL(triggered()), SLOT(chooseBerlin()));
        connect(jakartaAction, SIGNAL(triggered()), SLOT(chooseJakarta()));
        connect(nightModeAction, SIGNAL(triggered()), map, SLOT(toggleNightMode()));
        connect(osmAction, SIGNAL(triggered()), SLOT(aboutOsm()));

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM)
        menuBar()->addAction(gpsAction);
        menuBar()->addAction(osloAction);
        menuBar()->addAction(berlinAction);
        menuBar()->addAction(jakartaAction);
        menuBar()->addAction(nightModeAction);
        menuBar()->addAction(osmAction);
#else
        QMenu *menu = menuBar()->addMenu("&Options");
        menu->addAction(gpsAction);
        menu->addAction(osloAction);
        menu->addAction(berlinAction);
        menu->addAction(jakartaAction);
        menu->addSeparator();
        menu->addAction(nightModeAction);
        menu->addAction(osmAction);
#endif

#if defined(Q_OS_WINCE)
        QAction *exitAction = new QAction("Exit", this);
        connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        menuBar()->addAction(exitAction);
#endif
    }

private slots:

    void chooseGps() {
        map->startPositioning();
    }

    void chooseOslo() {
        map->stopPositioning();
        map->setCenter(59.9138204, 10.7387413);
    }

    void chooseBerlin() {
        map->stopPositioning();
        map->setCenter(52.52958999943302, 13.383053541183472);
    }

    void chooseJakarta() {
        map->stopPositioning();
        map->setCenter(-6.211544, 106.845172);
    }

    void aboutOsm() {
        QDesktopServices::openUrl(QUrl("http://www.openstreetmap.org"));
    }
};

#include "lightmaps.moc"

int main(int argc, char **argv)
{
#if defined(Q_WS_X11)
    QApplication::setGraphicsSystem("raster");
#endif

    QApplication app(argc, argv);
    app.setApplicationName("LightMaps");

    MapZoom w;
    w.setWindowTitle("OpenStreetMap");
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM)
    w.showMaximized();
#else
    w.resize(600, 450);
    w.show();
#endif

    return app.exec();
}
