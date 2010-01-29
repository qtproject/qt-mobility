/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qx11info_x11.h>

#include <qgraphicsvideoitem.h>

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qpaintervideosurface_p.h>
#include <qvideooutputcontrol.h>
#include <qvideorenderercontrol.h>

#include <QtMultimedia/qvideosurfaceformat.h>

#include <qxvideosurface_maemo5_p.h>

QTM_BEGIN_NAMESPACE

class QGraphicsVideoItemPrivate
{
public:
    QGraphicsVideoItemPrivate()
        : q_ptr(0)
        , surface(0)
        , mediaObject(0)
        , service(0)
        , outputControl(0)
        , rendererControl(0)
        , savedViewportUpdateMode(QGraphicsView::FullViewportUpdate)
    {
    }

    QGraphicsVideoItem *q_ptr;

    QXVideoSurface *surface;
    QMediaObject *mediaObject;
    QMediaService *service;
    QVideoOutputControl *outputControl;
    QVideoRendererControl *rendererControl;
    QPointer<QGraphicsView> currentView;
    QGraphicsView::ViewportUpdateMode savedViewportUpdateMode;
    QRect boundingRect;

    void clearService();

    void _q_present();
    void _q_formatChanged(const QVideoSurfaceFormat &format);
    void _q_serviceDestroyed();
    void _q_mediaObjectDestroyed();
};

void QGraphicsVideoItemPrivate::clearService()
{
    if (outputControl) {
        outputControl->setOutput(QVideoOutputControl::NoOutput);
        outputControl = 0;
    }
    if (rendererControl) {
        surface->stop();
        rendererControl->setSurface(0);
        rendererControl = 0;
    }
    if (service) {
        QObject::disconnect(service, SIGNAL(destroyed()), q_ptr, SLOT(_q_serviceDestroyed()));
        service = 0;
    }
}

void QGraphicsVideoItemPrivate::_q_present()
{
    q_ptr->update(boundingRect);
}

void QGraphicsVideoItemPrivate::_q_formatChanged(const QVideoSurfaceFormat &format)
{
    q_ptr->prepareGeometryChange();

    boundingRect = QRect(QPoint(0, 0), format.sizeHint());
    boundingRect.moveCenter(QPoint(0, 0));
}

void QGraphicsVideoItemPrivate::_q_serviceDestroyed()
{
    rendererControl = 0;
    outputControl = 0;
    service = 0;

    surface->stop();
}

void QGraphicsVideoItemPrivate::_q_mediaObjectDestroyed()
{
    mediaObject = 0;

    clearService();
}

/*!
    \class QGraphicsVideoItem

    \brief The QGraphicsVideoItem class provides a graphics item which display video produced by a QMediaObject.

    \ingroup multimedia

    Attaching a QGraphicsVideoItem to a QMediaObject allows it to display
    the video or image output of that media object.  A QGraphicsVideoItem
    is attached to a media object by passing a pointer to the QMediaObject
    to the setMediaObject() function.

    \code
    player = new QMediaPlayer(this);

    QGraphicsVideoItem *item = new QGraphicsVideoItem;
    item->setMediaObject(player);
    graphicsView->scence()->addItem(item);
    graphicsView->show();

    player->setMedia(video);
    player->play();
    \endcode

    \bold {Note}: Only a single display output can be attached to a media object at one time.

    \sa QMediaObject, QMediaPlayer, QVideoWidget
*/

/*!
    Constructs a graphics item that displays video.

    The \a parent is passed to QGraphicsItem.
*/
QGraphicsVideoItem::QGraphicsVideoItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d_ptr(new QGraphicsVideoItemPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->surface = new QXVideoSurface;
    d_ptr->boundingRect = QRect(-160, -120, 320, 240);

    setCacheMode(NoCache);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    connect(d_ptr->surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(_q_formatChanged(QVideoSurfaceFormat)));

    connect(d_ptr->surface, SIGNAL(activeChanged(bool)), this, SLOT(_q_present()));
}

/*!
    Destroys a video graphics item.
*/
QGraphicsVideoItem::~QGraphicsVideoItem()
{

    if (d_ptr->outputControl)
        d_ptr->outputControl->setOutput(QVideoOutputControl::NoOutput);

    if (d_ptr->rendererControl)
        d_ptr->rendererControl->setSurface(0);

    if (d_ptr->currentView)
        d_ptr->currentView->setViewportUpdateMode(d_ptr->savedViewportUpdateMode);

    delete d_ptr->surface;
    delete d_ptr;
}

/*!
    \property QGraphicsVideoItem::mediaObject
    \brief the media object which provides the video displayed by a graphics item.
*/

QMediaObject *QGraphicsVideoItem::mediaObject() const
{
    return d_func()->mediaObject;
}

void QGraphicsVideoItem::setMediaObject(QMediaObject *object)
{
    Q_D(QGraphicsVideoItem);

    if (object == d->mediaObject)
        return;

    d->clearService();

    if (d->mediaObject) {
        disconnect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));
        d->mediaObject->unbind(this);
    }

    d->mediaObject = object;

    if (d->mediaObject) {
        d->mediaObject->bind(this);

        connect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));

        d->service = d->mediaObject->service();

        if (d->service) {
            connect(d->service, SIGNAL(destroyed()), this, SLOT(_q_serviceDestroyed()));

            d->outputControl = qobject_cast<QVideoOutputControl *>(
                    d->service->control(QVideoOutputControl_iid));
            d->rendererControl = qobject_cast<QVideoRendererControl *>(
                    d->service->control(QVideoRendererControl_iid));

            if (d->outputControl != 0 && d->rendererControl != 0) {
                d->rendererControl->setSurface(d->surface);

                if (isVisible())
                    d->outputControl->setOutput(QVideoOutputControl::RendererOutput);
            }
        }
    }
}

/*!
    \reimp
*/
QRectF QGraphicsVideoItem::boundingRect() const
{
    return d_func()->boundingRect;
}


/*!
    \reimp
*/
void QGraphicsVideoItem::paint(
        QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_D(QGraphicsVideoItem);

    //qDebug() << "paint video item on widget" << widget;

    QGraphicsView *view = 0;
    if (scene() && !scene()->views().isEmpty())
        view = scene()->views().first();

    //it's necessary to switch vieport update mode to FullViewportUpdate
    //otherwise the video item area can be just scrolled without notifying overlay
    //about geometry changes
    if (view != d->currentView) {
        if (d->currentView) {
            d->currentView->setViewportUpdateMode(d->savedViewportUpdateMode);
            /*disconnect(d->currentView->verticalScrollBar(), SIGNAL(valueChanged(int)),
                       this, SLOT(_q_present()));
            disconnect(d->currentView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                       this, SLOT(_q_present()));*/
        }

        d->currentView = view;
        if (view) {
            d->savedViewportUpdateMode = view->viewportUpdateMode();
            view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            /*connect(d->currentView->verticalScrollBar(), SIGNAL(valueChanged(int)),
                    this, SLOT(_q_present()));
            connect(d->currentView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
                    this, SLOT(_q_present()));*/
        }
    }

    QColor colorKey = Qt::black;

    if (d->surface) {
        if (widget)
            d->surface->setWinId(widget->winId());

        QTransform transform = painter->combinedTransform();
        QRect deviceRect = transform.mapRect(boundingRect()).toRect();

        if (widget) {
            //workaround for xvideo issue with U/V planes swapped
            QPoint topLeft = widget->mapToGlobal(deviceRect.topLeft());
            if ((topLeft.x() & 1) == 0)
                deviceRect.moveLeft(deviceRect.left()-1);
        }

        if (d->surface->displayRect() != deviceRect) {
            //qDebug() << "set video display rect:" << deviceRect;
            d->surface->setDisplayRect( deviceRect );
            // repaint last frame after the color key area is filled
            QMetaObject::invokeMethod(d->surface, "repaintLastFrame", Qt::QueuedConnection);
        }

        colorKey = d->surface->colorKey();
    }

    painter->fillRect(d->boundingRect, colorKey);
}

/*!
    \reimp

    \internal
*/
QVariant QGraphicsVideoItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    Q_D(QGraphicsVideoItem);

    if (change == ItemVisibleChange && d->outputControl != 0 && d->rendererControl != 0) {
        if (value.toBool()) {
            d->outputControl->setOutput(QVideoOutputControl::RendererOutput);

            return d->outputControl->output() == QVideoOutputControl::RendererOutput;
        } else {
            d->outputControl->setOutput(QVideoOutputControl::NoOutput);

            return value;
        }
    } else if (change == ItemScenePositionHasChanged) {
        update(boundingRect());
    } else {
        return QGraphicsItem::itemChange(change, value);
    }

    return value;
}

#include "moc_qgraphicsvideoitem.cpp"
QTM_END_NAMESPACE
