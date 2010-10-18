/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPTEXTOBJECT_H
#define QDECLARATIVEGEOMAPTEXTOBJECT_H

#include "qdeclarativecoordinate_p.h"
#include "qgeomaptextobject.h"

#include <QColor>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapTextObject : public QGeoMapTextObject
{
    Q_OBJECT
    Q_ENUMS(HorizontalAlignment)
    Q_ENUMS(VerticalAlignment)

    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ declarativeCoordinate WRITE setDeclarativeCoordinate NOTIFY declarativeCoordinateChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(HorizontalAlignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VerticalAlignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)

public:
    enum HorizontalAlignment {
        AlignHCenter = Qt::AlignHCenter,
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight
    };

    enum VerticalAlignment {
        AlignVCenter = Qt::AlignVCenter,
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom
    };

    QDeclarativeGeoMapTextObject();
    ~QDeclarativeGeoMapTextObject();

    QDeclarativeCoordinate* declarativeCoordinate();
    void setDeclarativeCoordinate(const QDeclarativeCoordinate *coordinate);

    QColor color() const;
    void setColor(const QColor &color);

    HorizontalAlignment horizontalAlignment() const;
    void setHorizontalAlignment(HorizontalAlignment alignment);

    VerticalAlignment verticalAlignment() const;
    void setVerticalAlignment(VerticalAlignment alignment);

Q_SIGNALS:
    void declarativeCoordinateChanged(const QDeclarativeCoordinate *coordinate);
    void colorChanged(const QColor &color);
    void horizontalAlignmentChanged(HorizontalAlignment alignment);
    void verticalAlignmentChanged(VerticalAlignment alignment);

private Q_SLOTS:
    void coordinateLatitudeChanged(double latitude);
    void coordinateLongitudeChanged(double longitude);
    void coordinateAltitudeChanged(double altitude);

private:
    QDeclarativeCoordinate* m_coordinate;
    QColor m_color;
    HorizontalAlignment m_hAlignment;
    VerticalAlignment m_vAlignment;
    Q_DISABLE_COPY(QDeclarativeGeoMapTextObject)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapTextObject));

#endif
