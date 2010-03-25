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

#ifndef QLOCATION_MAPTILEREQUEST_H
#define QLOCATION_MAPTILEREQUEST_H

#include <QString>

#include "qmaptile.h"

QTM_BEGIN_NAMESPACE

class QMapTileRequestPrivate;
class Q_LOCATION_EXPORT QMapTileRequest
{
public:
    QMapTileRequest(const MapVersion& mapVersion,
                    const MapScheme& mapScheme,
                    const MapResolution& mapResolution,
                    const MapFormat& mapFormat);
    QMapTileRequest();

    QMapTileRequest(const QMapTileRequest &mtr);
    QMapTileRequest& operator= (const QMapTileRequest &mtr);

    ~QMapTileRequest();

    quint32 col() const;
    void setCol(quint32 c);

    quint32 row() const;
    void setRow(quint32 r);

    MapVersion version() const;
    void setVersion(const MapVersion& version);

    MapResolution resolution() const;
    void setResolution(const MapResolution& resolution);

    MapFormat format() const;
    void setFormat(const MapFormat& format);

    MapScheme scheme() const;
    void setScheme(const MapScheme& scheme);

    quint16 zoomLevel() const;
    void setZoomLevel(quint16 level);

    QString requestString(const QString &host, const QString &token, const QString &referrer) const;

private:
    //Q_DISABLE_COPY(QMapTileRequest);

    QMapTileRequestPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QMapTileRequest);
};

QTM_END_NAMESPACE

#endif
