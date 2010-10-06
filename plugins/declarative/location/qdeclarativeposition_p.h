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
***************************************************************************/

#ifndef QDECLARATIVEPOSITION_H
#define QDECLARATIVEPOSITION_H

#include <QtCore>
#include <QDateTime>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qdeclarativecoordinate_p.h>
#include <QtDeclarative/qdeclarative.h>

// Define this to get qDebug messages
// #define QDECLARATIVE_POSITION_DEBUG

#ifdef QDECLARATIVE_POSITION_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

class QDeclarativePosition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool latitudeValid READ isLatitudeValid NOTIFY latitudeValidChanged)
    Q_PROPERTY(bool longitudeValid READ isLongitudeValid NOTIFY longitudeValidChanged)
    Q_PROPERTY(bool altitudeValid READ isAltitudeValid NOTIFY altitudeValidChanged)
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(bool speedValid READ isSpeedValid NOTIFY speedValidChanged)

public:

    QDeclarativePosition(QObject* parent = 0);
    ~QDeclarativePosition();

    bool isLatitudeValid() const;
    bool isLongitudeValid() const;
    bool isAltitudeValid() const;
    QDateTime timestamp() const;
    void setTimestamp(const QDateTime& timestamp);
    double speed() const;
    void setSpeed(double speed);
    bool isSpeedValid() const;
    void setCoordinate(QDeclarativeCoordinate* coordinate);
    QDeclarativeCoordinate* coordinate();
    // C++
    void setCoordinate(QGeoCoordinate coordinate);

Q_SIGNALS:
    void latitudeValidChanged();
    void longitudeValidChanged();
    void altitudeValidChanged();
    void timestampChanged();
    void speedChanged();
    void speedValidChanged();
    void coordinateChanged();

private:
    bool m_latitudeValid;
    bool m_longitudeValid;
    bool m_altitudeValid;
    QDateTime m_timestamp;
    double m_speed;
    bool m_speedValid;
    QDeclarativeCoordinate m_coordinate;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativePosition));

#endif
