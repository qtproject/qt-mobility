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

#ifndef QGEOMAPPINGMANAGERENGINE_NOKIA_P_H
#define QGEOMAPPINGMANAGERENGINE_NOKIA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgeoserviceprovider.h>
#include <qgeotiledmappingmanagerengine.h>

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QHash>

#include "qgeomapreply_nokia_p.h"

QTM_USE_NAMESPACE

class QGeoMappingManagerEngineNokia : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    QGeoMappingManagerEngineNokia(const QMap<QString, QString> &parameters,
                            QGeoServiceProvider::Error *error,
                            QString *errorString);
    virtual ~QGeoMappingManagerEngineNokia();

    QGeoMapReply* getTileImage(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex, QGeoMapWidget::MapType mapType, const QString &imageFormat) const;

private:
    Q_DISABLE_COPY(QGeoMappingManagerEngineNokia)

    QString getRequestString(const QGeoMapReplyNokia::QuadTileInfo &info) const;

private slots:
    void mapFinished();
    void mapError(QGeoMapReply::Error error, const QString &errorString);

private:
    static QString sizeToStr(const QSize &size);
    static QString mapTypeToStr(QGeoMapWidget::MapType type);

private:
    QNetworkAccessManager *m_nam;
    QNetworkDiskCache *m_cache;
    QString m_host;
    QString m_token;
    QString m_referrer;
    QHash<qint64, QPair<QPixmap, bool> > m_mapTiles;
};

#endif
