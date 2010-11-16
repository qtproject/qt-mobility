/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mapwidget.h"

#include <QTimer>
#include <cmath>

QTM_USE_NAMESPACE

// TODO: Some of these could be exposed in a GUI and should probably be put elsewhere in that case (and made non-const)
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
// Devices
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 200.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.02; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 75; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 200; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#else
// Desktop
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 300.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.005; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 30; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 100; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#endif

MapWidget::MapWidget(QGeoMappingManager *manager) :
        QGraphicsGeoMap(manager),
        coordQueryState(false),
        panActive(false),
        kineticTimer(new QTimer),
        m_lastHoveredMapObject(0),
        lastCircle(0)
{
    for (int i = 0; i < 5; ++i) mouseHistory.append(MouseHistoryEntry());

    connect(kineticTimer, SIGNAL(timeout()), this, SLOT(kineticTimerEvent()));
    kineticTimer->setInterval(kineticPanningResolution);

    setAcceptHoverEvents(true);
}

MapWidget::~MapWidget() {}

void MapWidget::setMouseClickCoordQuery(bool state)
{
    coordQueryState = state;
}

void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
        } else {
            if (coordQueryState) {
                emit coordQueryResult(screenPositionToCoordinate(event->lastPos()));
                return;
            }

            panActive = true;

            // When pressing, stop the timer and stop all current kinetic panning
            kineticTimer->stop();
            kineticPanSpeed = QPointF();

            lastMoveTime = QTime::currentTime();
        }
    }

    event->accept();
}

void MapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (panActive) {
            panActive = false;

            if (!enableKineticPanning || lastMoveTime.msecsTo(QTime::currentTime()) > holdTimeThreshold) {
                return;
            }

            kineticPanSpeed = QPointF();
            int entries_considered = 0;

            QTime currentTime = QTime::currentTime();
            foreach(MouseHistoryEntry entry, mouseHistory) {
                // first=speed, second=time
                int deltaTime = entry.second.msecsTo(currentTime);
                if (deltaTime < holdTimeThreshold) {
                    kineticPanSpeed += entry.first;
                    entries_considered++;
                }
            }
            if (entries_considered > 0) kineticPanSpeed /= entries_considered;
            lastMoveTime = currentTime;

            // When releasing the mouse button/finger while moving, start the kinetic panning timer
            kineticTimer->start();
            panDecellerate = true;
        }
    }

    event->accept();
}

void MapWidget::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF scenePos = event->scenePos();
    QList<QGeoMapObject*> objectsAtCursor = mapObjectsAtScreenPosition(scenePos);
    QGeoMapObject* objectAtCursor = 0;
    if (!objectsAtCursor.isEmpty())
        objectAtCursor = objectsAtCursor.last();

    if (m_lastHoveredMapObject == objectAtCursor)
        return;

    // unhover the old object if applicable
    if (m_lastHoveredMapObject) {
        disconnect(m_lastHoveredMapObject, SIGNAL(destroyed()), this, SLOT(hoveredMapObjectDestroyed()));

        m_lastHoveredMapObject->setProperty("pen", m_lastHoveredMapObjectPen);
    }

    m_lastHoveredMapObject = objectAtCursor;

    // hover the new object if applicable
    if (m_lastHoveredMapObject) {
        m_lastHoveredMapObjectPen = m_lastHoveredMapObject->property("pen").value<QPen>();
        QPen hoverPen = m_lastHoveredMapObjectPen;
        hoverPen.setColor(Qt::green);
        m_lastHoveredMapObject->setProperty("pen", hoverPen);

        connect(m_lastHoveredMapObject, SIGNAL(destroyed()), this, SLOT(hoveredMapObjectDestroyed()));
    }
}

void MapWidget::hoveredMapObjectDestroyed()
{
    m_lastHoveredMapObject = 0;
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (lastCircle)
            lastCircle->setCenter(this->screenPositionToCoordinate(event->pos()));
    } else if (panActive) {
        // Calculate time delta
        QTime currentTime = QTime::currentTime();
        int deltaTime = lastMoveTime.msecsTo(currentTime);
        lastMoveTime = currentTime;

        // Calculate position delta
        QPointF delta = event->lastPos() - event->pos();

        // Calculate and set speed
        if (deltaTime > 0) {
            kineticPanSpeed = delta / deltaTime;

            mouseHistory.push_back(MouseHistoryEntry(kineticPanSpeed, currentTime));
            mouseHistory.pop_front();
        }

        // Pan map
        panFloatWrapper(delta);
    }

    event->accept();
}

void MapWidget::kineticTimerEvent()
{
    QTime currentTime = QTime::currentTime();
    int deltaTime = lastMoveTime.msecsTo(currentTime);
    lastMoveTime = currentTime;

    if (panDecellerate)
        kineticPanSpeed *= pow(qreal(0.5), qreal(deltaTime / kineticPanningHalflife));

    QPointF scaledSpeed = kineticPanSpeed * deltaTime;

    if (kineticPanSpeed.manhattanLength() < kineticPanSpeedThreshold) {
        // Kinetic panning is almost halted -> stop it.
        kineticTimer->stop();
        return;
    }
    panFloatWrapper(scaledSpeed);
}

// Wraps the pan(int, int) method to achieve floating point accuracy, which is needed to scroll smoothly.
void MapWidget::panFloatWrapper(const QPointF& delta)
{
    // Add to previously stored panning distance
    remainingPan += delta;

    // Convert to integers
    QPoint move = remainingPan.toPoint();

    // Commit mouse movement
    pan(move.x(), move.y());

    // Store committed mouse movement
    remainingPan -= move;

}

void MapWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();

    pan(event->lastPos().x() -  size().width() / 2.0, event->lastPos().y() - size().height() / 2.0);
    if (zoomLevel() < maximumZoomLevel())
        setZoomLevel(zoomLevel() + 1);

    event->accept();
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Minus:
#ifdef Q_OS_SYMBIAN
        case Qt::Key_VolumeDown:
#endif
            if (zoomLevel() > minimumZoomLevel()) {
                setZoomLevel(zoomLevel() - 1);
            }
            break;

        case Qt::Key_Plus:
#ifdef Q_OS_SYMBIAN
        case Qt::Key_VolumeUp:
#endif
            if (zoomLevel() < maximumZoomLevel()) {
                setZoomLevel(zoomLevel() + 1);
            }
            break;

        case Qt::Key_T:
            if (mapType() == QGraphicsGeoMap::StreetMap)
                setMapType(QGraphicsGeoMap::SatelliteMapDay);
            else if (mapType() == QGraphicsGeoMap::SatelliteMapDay)
                setMapType(QGraphicsGeoMap::StreetMap);
            break;

        case Qt::Key_Shift:
            // If there's no current movement, we don't need to handle shift.
            if (panDir.manhattanLength() == 0) break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            if (!event->isAutoRepeat()) {
                switch (event->key()) {
                    case Qt::Key_Left:
                        panDir.setX(-1);
                        break;

                    case Qt::Key_Right:
                        panDir.setX(1);
                        break;

                    case Qt::Key_Up:
                        panDir.setY(-1);
                        break;

                    case Qt::Key_Down:
                        panDir.setY(1);
                        break;
                }

                lastMoveTime = QTime::currentTime();
                kineticTimer->start();
                panDecellerate = false;

                applyPan(event->modifiers());
            }
            break;
    }

    event->accept();
}

void MapWidget::keyReleaseEvent(QKeyEvent* event)
{
    event->accept();

    // Qt seems to have auto-repeated release events too...
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
            panDir.setX(0);
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            panDir.setY(0);
            break;

        case Qt::Key_Shift:
            if (panDir.manhattanLength() == 0) return;
            break;

        default:
            return;
    }

    applyPan(event->modifiers());
}

// Evaluates the panDir field and sets kineticPanSpeed accordingly. Used in MapWidget::keyPressEvent and MapWidget::keyReleaseEvent
void MapWidget::applyPan(const Qt::KeyboardModifiers& modifiers)
{
    Q_ASSERT(panDir.manhattanLength() <= 2);

    if (panDir.manhattanLength() == 0) {
        // If no more direction keys are held down, decellerate
        panDecellerate = true;
    } else {
        // Otherwise, set new direction
        qreal panspeed = (modifiers & Qt::ShiftModifier) ? panSpeedFast : panSpeedNormal;

        if (panDir.manhattanLength() == 2) {
            // If 2 keys are held down, adjust speed to achieve the same speed in all 8 possible directions
            panspeed *= sqrt(0.5);
        }

        // Finally set the current panning speed
        kineticPanSpeed = QPointF(panDir) * panspeed;
    }
}

void MapWidget::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    qreal panx = event->pos().x() - size().width() / 2.0;
    qreal pany = event->pos().y() - size().height() / 2.0;
    pan(panx, pany);
    if (event->delta() > 0) { //zoom in
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    } else { //zoom out
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    }
    pan(-panx, -pany);
    event->accept();
}
