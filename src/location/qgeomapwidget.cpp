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

#include "qgeomapwidget.h"
#include "qgeomapwidget_p.h"
#include "qgeomapviewport.h"

QTM_BEGIN_NAMESPACE

/*!
    Constructs a QGeoMapWidget with a \a viewport and a \a parent.
    The map widget takes ownership of the \a viewport.
*/
QGeoMapWidget::QGeoMapWidget(QGeoMapViewport *viewport, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
    d_ptr(new QGeoMapWidgetPrivate(viewport))
{
    viewport->setMapWidget(this);
    viewport->setViewportSize(this->boundingRect().size().toSize());
    viewport->setCenter(QGeoCoordinate(52.5, 13.4));
}

/*!
    Destructor.
*/
QGeoMapWidget::~QGeoMapWidget()
{
    delete d_ptr;
}

void QGeoMapWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (d_ptr->viewport)
        d_ptr->viewport->paint(painter, option);
}

/******************************************************************************
 ******************************************************************************/

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(QGeoMapViewport *viewport)
    : viewport(viewport)
{
}

QGeoMapWidgetPrivate::~QGeoMapWidgetPrivate()
{
    delete viewport;
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
