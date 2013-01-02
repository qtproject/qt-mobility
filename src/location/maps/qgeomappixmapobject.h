/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPPIXMAPOBJECT_H
#define QGEOMAPPIXMAPOBJECT_H

#include "qgeomapobject.h"
#include "qgeocoordinate.h"

#include <QPixmap>
#include <QPoint>

QTM_BEGIN_NAMESPACE

class QGeoMapPixmapObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapPixmapObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QPixmap pixmap  READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
    Q_PROPERTY(QPoint offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    QGeoMapPixmapObject();
    QGeoMapPixmapObject(const QGeoCoordinate &coordinate, const QPoint &offset = QPoint(0, 0), const QPixmap &pixmap = QPixmap());
    ~QGeoMapPixmapObject();

    QGeoMapObject::Type type() const;

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

    QPoint offset() const;
    void setOffset(const QPoint &offset);

Q_SIGNALS:
    void coordinateChanged(const QGeoCoordinate &coordinate);
    void pixmapChanged(const QPixmap &pixmap);
    void offsetChanged(const QPoint &offset);

private:
    QGeoMapPixmapObjectPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapPixmapObject)
    Q_DISABLE_COPY(QGeoMapPixmapObject)
};

QTM_END_NAMESPACE

#endif
