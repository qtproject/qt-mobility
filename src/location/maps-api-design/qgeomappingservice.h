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

#ifndef QGEOMAPPINGSERVICE_H
#define QGEOMAPPINGSERVICE_H

#include "qgeomapwidget.h"
#include "qgeocoordinate.h"
#include "qgeomaprequestoptions.h"

#include <QObject>

class QSize;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoMapReply;

class QGeoMappingServicePrivate;

class Q_LOCATION_EXPORT QGeoMappingService : public QObject{

    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

    virtual ~QGeoMappingService();

    virtual QGeoMapReply* requestMap(const QGeoCoordinate &center,
                                     int zoomLevel,
                                     const QSize &size,
                                     const QGeoMapRequestOptions &requestOptions = QGeoMapRequestOptions()) = 0;

    QList<QGeoMapWidget::MapType> supportedMapTypes() const;
    QList<QString> supportedImageFormats() const;

    int minimumZoomLevel() const;
    int maximumZoomLevel() const;

    QSize minimumImageSize() const;
    QSize maximumImageSize() const;

signals:
    void replyFinished(QGeoMapReply *reply);
    void replyError(QGeoMapReply *reply, 
                    QGeoMappingService::ErrorCode errorCode,
                    QString errorString);

protected:
    QGeoMappingService();

    void setSupportedMapTypes(const QList<QGeoMapWidget::MapType> &mapTypes);
    void setSupportedImageFormats(const QList<QString> &imageFormats);

    void setMinimumZoomLevel(int minimumZoom);
    void setMaximumZoomLevel(int maximumZoom);

    void setMinimumSize(const QSize &minimumSize);
    void setMaximumSize(const QSize &maximumSize);

    QGeoMappingServicePrivate* d_ptr;

private:
    Q_DISABLE_COPY(QGeoMappingService)
    Q_DECLARE_PRIVATE(QGeoMappingService)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
