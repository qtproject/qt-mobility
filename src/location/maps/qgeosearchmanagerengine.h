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

#ifndef QGEOSEARCHMANAGERENGINE_H
#define QGEOSEARCHMANAGERENGINE_H

#include "qgeosearchmanager.h"
#include "qgeosearchreply.h"
#include "qgeoboundingbox.h"

#include <QObject>
#include <QList>

QTM_BEGIN_NAMESPACE

class QLandmarkManager;

class QGeoSearchManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoSearchManagerEngine : public QObject
{
    Q_OBJECT
public:
    QGeoSearchManagerEngine(const QMap<QString, QString> &parameters, QObject *parent = 0);
    virtual ~QGeoSearchManagerEngine();

    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    virtual QGeoSearchReply* geocode(const QGeoAddress &address,
                                     const QGeoBoundingBox &bounds);
    virtual QGeoSearchReply* reverseGeocode(const QGeoCoordinate &coordinate,
                                     const QGeoBoundingBox &bounds);

    virtual QGeoSearchReply* search(const QString &searchString,
                                          QGeoSearchManager::SearchTypes searchTypes,
                                          const QGeoBoundingBox &bounds);

    bool supportsGeocoding() const;
    QGeoSearchManager::SearchTypes supportedSearchTypes() const;

    QLandmarkManager* defaultLandmarkManager() const;

    void setAdditionalLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers);
    QList<QLandmarkManager *> additionalLandmarkManagers() const;
    void addAdditionalLandmarkManager(QLandmarkManager *landmarkManager);

signals:
    void finished(QGeoSearchReply* reply);
    void error(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString = QString());

protected:
    void setDefaultLandmarkManager(QLandmarkManager *landmarkManager);
    void setSupportsGeocoding(bool supported);
    void setSupportedSearchTypes(QGeoSearchManager::SearchTypes searchTypes);

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QGeoSearchManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QGeoSearchManagerEngine)

    friend class QGeoServiceProvider;
};

QTM_END_NAMESPACE

#endif