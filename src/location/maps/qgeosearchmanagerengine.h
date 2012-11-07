/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
    QGeoSearchManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QGeoSearchManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeoSearchReply* geocode(const QGeoAddress &address,
                                     QGeoBoundingArea *bounds);
    virtual QGeoSearchReply* reverseGeocode(const QGeoCoordinate &coordinate,
                                            QGeoBoundingArea *bounds);

    virtual QGeoSearchReply* search(const QString &searchString,
                                    QGeoSearchManager::SearchTypes searchTypes,
                                    int limit,
                                    int offset,
                                    QGeoBoundingArea *bounds);

    bool supportsGeocoding() const;
    bool supportsReverseGeocoding() const;
    QGeoSearchManager::SearchTypes supportedSearchTypes() const;

    QLandmarkManager* defaultLandmarkManager() const;

    void setAdditionalLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers);
    QList<QLandmarkManager *> additionalLandmarkManagers() const;
    void addAdditionalLandmarkManager(QLandmarkManager *landmarkManager);

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoSearchReply* reply);
    void error(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString = QString());

protected:
    void setDefaultLandmarkManager(QLandmarkManager *landmarkManager);
    void setSupportsGeocoding(bool supported);
    void setSupportsReverseGeocoding(bool supported);
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
