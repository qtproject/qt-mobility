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

#include <qgraphicsvideoitem.h>

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qpaintervideosurface_p.h>
#include <qvideooutputcontrol.h>
#include <qvideorenderercontrol.h>

#include <QtMultimedia/qvideosurfaceformat.h>

class QGraphicsVideoItemPrivate
{
public:
    QGraphicsVideoItemPrivate()
        : q_ptr(0)
        , surface(0)
        , outputControl(0)
        , rendererControl(0)
    {
    }

    QGraphicsVideoItem *q_ptr;

    QPainterVideoSurface *surface;
    QVideoOutputControl *outputControl;
    QVideoRendererControl *rendererControl;
    QRect boundingRect;

    void _q_present();
    void _q_formatChanged(const QVideoSurfaceFormat &format);
    void _q_serviceDestroyed();
};

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

    surface->stop();
}

/*!
    \class QGraphicsVideoItem

    \brief The QGraphicsVideoItem class provides a graphics item which display video produced by a
    QMediaObject.

    Attaching a QGraphicsVideoItem to a QMediaObject allows it to display the video or image output
    of that media object.  A QGraphicsVideoItem is attached to media object by passing a pointer to
    the QMediaObject in its constructor, and detached by destroying the QGraphicsVideoItem.

    \code
    player = new QMediaPlayer(this);

    graphicsView->scence()->addItem(new QGraphicsVideoItem(player));
    graphicsView->show();

    player->setMedia(video);
    player->play();
    \endcode

    \bold {Note}: Only a single display output can be attached to a media object at one time.

    \sa QMediaObject, QMediaPlayer, QVideoWidget
*/

/*!
    Constructs a graphics item that displays the video produced by a media \a object.

    The \a parent is passed to QGraphicsItem.
*/
QGraphicsVideoItem::QGraphicsVideoItem(QMediaObject *object, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d_ptr(new QGraphicsVideoItemPrivate)
{
    d_ptr->q_ptr = this;

    Q_D(QGraphicsVideoItem);

    if (object) {
        if (QMediaService *service = object->service()) {
            d->outputControl = qobject_cast<QVideoOutputControl *>(
                    service->control(QVideoOutputControl_iid));
            d->rendererControl = qobject_cast<QVideoRendererControl *>(
                    service->control(QVideoRendererControl_iid));

            if (d->outputControl != 0 && d->rendererControl != 0) {
                d->surface = new QPainterVideoSurface;

                connect(d->surface, SIGNAL(frameChanged()), this, SLOT(_q_present()));
                connect(d->surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
                        this, SLOT(_q_formatChanged(QVideoSurfaceFormat)));
                connect(service, SIGNAL(destroyed()), this, SLOT(_q_serviceDestroyed()));

                d->rendererControl->setSurface(d->surface);
                d->outputControl->setOutput(QVideoOutputControl::RendererOutput);
            }
        }
    }
}

/*!
    Destroys a video graphics item.
*/
QGraphicsVideoItem::~QGraphicsVideoItem()
{
    if (d_ptr->rendererControl)
        d_ptr->rendererControl->setSurface(0);

    delete d_ptr->surface;
    delete d_ptr;
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
    Q_D(QGraphicsVideoItem);

    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->surface != 0) {
        d->surface->paint(painter, d->boundingRect);
        d->surface->setReady(true);
    }
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
    } else {
        return QGraphicsItem::itemChange(change, value);
    }
}

#include "moc_qgraphicsvideoitem.cpp"
