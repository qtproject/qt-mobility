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

#include <QtCore/qpointer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qbasictimer.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qx11info_x11.h>

#include "qgraphicsvideoitem.h"

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qvideowindowcontrol.h>


QT_BEGIN_NAMESPACE

#define DEBUG_GFX_VIDEO_ITEM

class QGraphicsVideoItemPrivate : public QObject
{
public:
    QGraphicsVideoItemPrivate()
        : q_ptr(0)
        , mediaObject(0)
        , service(0)
        , windowControl(0)
        , savedViewportUpdateMode(QGraphicsView::FullViewportUpdate)
        , aspectRatioMode(Qt::KeepAspectRatio)
        , rect(0.0, 0.0, 320, 240)
        , videoWidget(0)
    {
    }

    QGraphicsVideoItem *q_ptr;

    QMediaObject *mediaObject;
    QMediaService *service;
    QVideoWindowControl *windowControl;
    QPointer<QGraphicsView> currentView;
    QGraphicsView::ViewportUpdateMode savedViewportUpdateMode;

    Qt::AspectRatioMode aspectRatioMode;
    QRectF rect;
    QRectF boundingRect;
    QRectF displayRect;
    QSizeF nativeSize;

    QWidget *videoWidget;

    bool eventFilter(QObject *object, QEvent *event);

    void setWidget(QWidget *widget);
    void clearService();
    void updateRects();
    void updateLastFrame();

    void _q_present();
    void _q_updateNativeSize();
    void _q_serviceDestroyed();
    void _q_mediaObjectDestroyed();
};

void QGraphicsVideoItemPrivate::_q_present()
{

}

bool QGraphicsVideoItemPrivate::eventFilter(QObject *object, QEvent *event)
{
    if (windowControl && object == videoWidget && QEvent::WinIdChange == event->type())
        windowControl->setWinId(videoWidget->effectiveWinId());
    return false;
}

void QGraphicsVideoItemPrivate::setWidget(QWidget *widget)
{
    if (videoWidget != widget) {
        videoWidget = widget;
        if (widget) {
            windowControl->setWinId(widget->winId());
            widget->installEventFilter(this);
        }
    }
}

void QGraphicsVideoItemPrivate::clearService()
{
    if (windowControl) {
        QObject::disconnect(windowControl, SIGNAL(nativeSizeChanged()), q_ptr, SLOT(_q_updateNativeSize()));
        service->releaseControl(windowControl);
        windowControl = 0;
    }

    if (service) {
        QObject::disconnect(service, SIGNAL(destroyed()), q_ptr, SLOT(_q_serviceDestroyed()));
        service = 0;
    }
}

void QGraphicsVideoItemPrivate::updateRects()
{
    q_ptr->prepareGeometryChange();

    displayRect = rect;

    if (nativeSize.isEmpty()) {
        boundingRect = rect;
    } else if (aspectRatioMode == Qt::IgnoreAspectRatio) {
        boundingRect = rect;        
    } else if (aspectRatioMode == Qt::KeepAspectRatio) {
        QSizeF size = nativeSize;
        size.scale(rect.size(), Qt::KeepAspectRatio);

        boundingRect = QRectF(0, 0, size.width(), size.height());
        boundingRect.moveCenter(rect.center());

        displayRect = boundingRect;
    } else if (aspectRatioMode == Qt::KeepAspectRatioByExpanding) {
        boundingRect = rect;


        QSizeF size = rect.size();
        size.scale(nativeSize, Qt::KeepAspectRatioByExpanding);
        displayRect = QRectF(0, 0, size.width(), size.height());
        displayRect.moveCenter(rect.center());
    }
}

void QGraphicsVideoItemPrivate::updateLastFrame()
{
}

void QGraphicsVideoItemPrivate::_q_updateNativeSize()
{
    const QSize size = windowControl->nativeSize();
    if (nativeSize != size) {
        nativeSize = size;

        updateRects();
        emit q_ptr->nativeSizeChanged(nativeSize);
    }
}

void QGraphicsVideoItemPrivate::_q_serviceDestroyed()
{
    windowControl = 0;
    service = 0;
}

void QGraphicsVideoItemPrivate::_q_mediaObjectDestroyed()
{
    mediaObject = 0;

    clearService();
}

QGraphicsVideoItem::QGraphicsVideoItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , d_ptr(new QGraphicsVideoItemPrivate)
{
    d_ptr->q_ptr = this;

    setCacheMode(NoCache);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QGraphicsVideoItem::~QGraphicsVideoItem()
{
    if (d_ptr->windowControl) {
        d_ptr->service->releaseControl(d_ptr->windowControl);
    }

    if (d_ptr->currentView)
        d_ptr->currentView->setViewportUpdateMode(d_ptr->savedViewportUpdateMode);

    delete d_ptr;
}

QMediaObject *QGraphicsVideoItem::mediaObject() const
{
    return d_func()->mediaObject;
}

bool QGraphicsVideoItem::setMediaObject(QMediaObject *object)
{
    Q_D(QGraphicsVideoItem);

    if (object == d->mediaObject)
        return true;

    d->clearService();

    d->mediaObject = object;

    if (d->mediaObject) {
        d->service = d->mediaObject->service();

        if (d->service) {
            d->windowControl = qobject_cast<QVideoWindowControl *>(
                    d->service->requestControl(QVideoWindowControl_iid));

            if (d->windowControl != 0) {
                connect(d->service, SIGNAL(destroyed()), SLOT(_q_serviceDestroyed()));
                connect(d->windowControl, SIGNAL(nativeSizeChanged()), SLOT(_q_updateNativeSize()));

                //d->windowControl->setProperty("colorKey", QVariant(QColor(16,7,2)));
                d->windowControl->setProperty("autopaintColorKey", QVariant(false));

                d->updateRects();
                return true;
            } else {
                qWarning() << "Service doesn't support QVideoWindowControl, overlay item failed";
            }
        }
    }

    d->mediaObject = 0;
    return false;
}

Qt::AspectRatioMode QGraphicsVideoItem::aspectRatioMode() const
{
    return d_func()->aspectRatioMode;
}

void QGraphicsVideoItem::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    Q_D(QGraphicsVideoItem);

    d->aspectRatioMode = mode;
    d->updateRects();
}

QPointF QGraphicsVideoItem::offset() const
{
    return d_func()->rect.topLeft();
}

void QGraphicsVideoItem::setOffset(const QPointF &offset)
{
    Q_D(QGraphicsVideoItem);

    d->rect.moveTo(offset);
    d->updateRects();
}

QSizeF QGraphicsVideoItem::size() const
{
    return d_func()->rect.size();
}

void QGraphicsVideoItem::setSize(const QSizeF &size)
{
    Q_D(QGraphicsVideoItem);

    d->rect.setSize(size.isValid() ? size : QSizeF(0, 0));
    d->updateRects();
}

QSizeF QGraphicsVideoItem::nativeSize() const
{
    return d_func()->nativeSize;
}

QRectF QGraphicsVideoItem::boundingRect() const
{
    return d_func()->boundingRect;
}

void QGraphicsVideoItem::paint(
        QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef DEBUG_GFX_VIDEO_ITEM
    qDebug() << "QGraphicsVideoItem::paint";
#endif

    Q_UNUSED(option);
    Q_D(QGraphicsVideoItem);

    QGraphicsView *view = 0;
    if (scene() && !scene()->views().isEmpty())
        view = scene()->views().first();

    //it's necessary to switch vieport update mode to FullViewportUpdate
    //otherwise the video item area can be just scrolled without notifying overlay
    //about geometry changes
    if (view != d->currentView) {
        if (d->currentView) {
            d->currentView->setViewportUpdateMode(d->savedViewportUpdateMode);
        }

        d->currentView = view;
        if (view) {
            d->savedViewportUpdateMode = view->viewportUpdateMode();
            view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        }
    }

    QColor colorKey = Qt::black;

    if (d->windowControl != 0 && widget != 0) {
        d->setWidget(widget);

        QTransform transform = painter->combinedTransform();
        QRect overlayRect = transform.mapRect(d->displayRect).toRect();
        QRect currentSurfaceRect = d->windowControl->displayRect();

        if (currentSurfaceRect != overlayRect) {            
#ifdef DEBUG_GFX_VIDEO_ITEM
            qDebug() << "set video display rect:" << overlayRect;
#endif
            d->windowControl->setDisplayRect(overlayRect);
        }

        colorKey = d->windowControl->property("colorKey").value<QColor>();
    }

    if (colorKey.alpha() != 255)
        painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(d->boundingRect, colorKey);
}

QVariant QGraphicsVideoItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemScenePositionHasChanged) {
        update(boundingRect());
    } else {
        return QGraphicsItem::itemChange(change, value);
    }

    return value;
}

void QGraphicsVideoItem::timerEvent(QTimerEvent *event)
{
    QGraphicsObject::timerEvent(event);
}

#include "moc_qgraphicsvideoitem.cpp"
QT_END_NAMESPACE
