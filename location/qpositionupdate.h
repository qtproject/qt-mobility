/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QPOSITIONUPDATE_H
#define QPOSITIONUPDATE_H

#include "qlocationglobal.h"
#include "qcoordinate.h"

#include <QDateTime>

class QDebug;
class QDataStream;
class QPositionUpdatePrivate;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QPositionUpdate
{
public:
    enum Property {
        Heading,
        GroundSpeed,
        VerticalSpeed,
        MagneticVariation,
        HorizontalAccuracy,
        VerticalAccuracy
    };

    QPositionUpdate();
    QPositionUpdate(const QCoordinate &coordinate, const QDateTime &updateTime);
    QPositionUpdate(const QPositionUpdate &other);
    ~QPositionUpdate();

    QPositionUpdate &operator=(const QPositionUpdate &other);

    bool operator==(const QPositionUpdate &other) const;
    inline bool operator!=(const QPositionUpdate &other) const { return !operator==(other); }

    bool isValid() const;

    void setUpdateTime(const QDateTime &updateTime);
    QDateTime updateTime() const;

    void setCoordinate(const QCoordinate &coordinate);
    QCoordinate coordinate() const;

    void setDoubleProperty(Property property, qreal value);
    qreal doubleProperty(Property property) const;
    void removeProperty(Property property);

    bool hasProperty(Property property) const;

private:
#ifndef QT_NO_DEBUG_STREAM
    friend Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QPositionUpdate &update);
#endif
#ifndef QT_NO_DATASTREAM
    friend Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QPositionUpdate &update);
    friend Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QPositionUpdate &update);
#endif
    QPositionUpdatePrivate *d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_LOCATION_EXPORT QDebug operator<<(QDebug dbg, const QPositionUpdate &update);
#endif

#ifndef QT_NO_DATASTREAM
Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QPositionUpdate &update);
Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QPositionUpdate &update);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
