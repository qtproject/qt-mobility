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

#include "qgeomaptextobject.h"
#include "qgeomaptextobject_p.h"

QTM_BEGIN_NAMESPACE

QGeoMapTextObject::QGeoMapTextObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapTextObjectPrivate(this, parent)) {}

QGeoMapTextObject::QGeoMapTextObject(const QGeoCoordinate &coordinate,
                                     const QString &text,
                                     const QFont &font,
                                     QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapTextObjectPrivate(this, parent))
{
    Q_D(QGeoMapTextObject);

    d->coordinate = coordinate;
    d->text = text;
    d->font = font;
}

QGeoMapTextObject::~QGeoMapTextObject()
{
}

QGeoCoordinate QGeoMapTextObject::coordinate() const
{
    Q_D(const QGeoMapTextObject);
    return d->coordinate;
}

void QGeoMapTextObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoMapTextObject);
    if (d->coordinate != coordinate) {
        d->coordinate = coordinate;
        objectUpdate();
        emit coordinateChanged(d->coordinate);
    }
}

QString QGeoMapTextObject::text() const
{
    Q_D(const QGeoMapTextObject);
    return d->text;
}

void QGeoMapTextObject::setText(const QString &text)
{
    Q_D(QGeoMapTextObject);
    if (d->text != text) {
        d->text = text;
        objectUpdate();
        emit textChanged(d->text);
    }
}

QFont QGeoMapTextObject::font() const
{
    Q_D(const QGeoMapTextObject);
    return d->font;
}

void QGeoMapTextObject::setFont(const QFont &font)
{
    Q_D(QGeoMapTextObject);
    if (d->font != font) {
        d->font = font;
        objectUpdate();
        emit fontChanged(d->font);
    }
}

QPen QGeoMapTextObject::pen() const
{
    Q_D(const QGeoMapTextObject);
    return d->pen;
}

void QGeoMapTextObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapTextObject);
    if (d->pen != pen) {
        d->pen = pen;
        objectUpdate();
        emit penChanged(d->pen);
    }
}

QBrush QGeoMapTextObject::brush() const
{
    Q_D(const QGeoMapTextObject);
    return d->brush;
}

void QGeoMapTextObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapTextObject);
    if (d->brush != brush) {
        d->brush = brush;
        objectUpdate();
        emit brushChanged(d->brush);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoMapTextObjectPrivate::QGeoMapTextObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::TextType) {}

QGeoMapTextObjectPrivate::~QGeoMapTextObjectPrivate() {}

#include "moc_qgeomaptextobject.cpp"

QTM_END_NAMESPACE

