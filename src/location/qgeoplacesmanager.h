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

#ifndef QGEOPLACESMANAGER_H
#define QGEOPLACESMANAGER_H

#include "qgeoplacesreply.h"
#include "qgeoboundingbox.h"

#include <QObject>
#include <QList>

QTM_BEGIN_NAMESPACE

class QLandmarkManager;

class QGeoPlacesManagerPrivate;

class Q_LOCATION_EXPORT QGeoPlacesManager : public QObject
{
    Q_OBJECT
public:
    enum SearchType {
        SearchNone = 0x0000,
        SearchGeocode = 0x0001,
        SearchLandmarks = 0x0002,
        SearchAll = 0xFFFF
    };
    Q_DECLARE_FLAGS(SearchTypes, SearchType)

    virtual ~QGeoPlacesManager();

    virtual QGeoPlacesReply* geocode(const QGeoAddress &address,
                                     const QGeoBoundingBox &bounds = QGeoBoundingBox()) = 0;
    virtual QGeoPlacesReply* geocode(const QGeoCoordinate &coordinate,
                                     const QGeoBoundingBox &bounds = QGeoBoundingBox()) = 0;

    virtual QGeoPlacesReply* placesSearch(const QString &searchString,
                                          SearchTypes searchTypes = SearchTypes(SearchAll),
                                          const QGeoBoundingBox &bounds = QGeoBoundingBox()) = 0;

    bool supportsViewportBiasing() const;
    bool supportsGeocoding() const;
    SearchTypes supportedSearchTypes() const;

    void setLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers);
    QList<QLandmarkManager *> landmarkManagers() const;
    void addLandmarkManager(QLandmarkManager *landmarkManager);

signals:
    void finished(QGeoPlacesReply* reply);
    void error(QGeoPlacesReply* reply, QGeoPlacesReply::Error error, QString errorString = QString());

protected:
    QGeoPlacesManager(QObject *parent = 0);
    void setSupportsViewportBiasing(bool supported);
    void setSupportsGeocoding(bool supported);
    void setSupportedSearchTypes(SearchTypes searchTypes);

private:
    QGeoPlacesManagerPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoPlacesManager)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoPlacesManager::SearchTypes)

QTM_END_NAMESPACE

#endif
