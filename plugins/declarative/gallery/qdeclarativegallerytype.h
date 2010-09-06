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

#ifndef QDECLARATIVEGALLERYTYPE_H
#define QDECLARATIVEGALLERYTYPE_H

#include <qgallerytyperequest.h>

#include <QtCore/qpointer.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativepropertymap.h>

#include "qdeclarativedocumentgallery.h"
#include "qdeclarativegalleryfilter.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryType : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ isAutoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(QObject *metaData READ metaData NOTIFY metaDataChanged)
public:
    enum Status
    {
        Null,
        Active,
        Finished,
        Idle,
        Cancelling,
        Cancelled,
        Error
    };

    ~QDeclarativeGalleryType();

    Status status() const { return m_status; }

    qreal progress() const
    {
        const int max = m_request.maximumProgress();
        return max > 0 ? qreal(m_request.currentProgress()) / max : qreal(0.0);
    }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setPropertyNames(names); emit propertyNamesChanged(); }

    bool isAutoUpdate() const { return m_request.isLive(); }
    void setAutoUpdate(bool autoUpdate) { m_request.setLive(autoUpdate); emit autoUpdateChanged(); }

    bool available() const { return m_request.isValid(); }

    QObject *metaData() const { return m_metaData; }

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void reload() { m_request.execute(); }
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void statusChanged();
    void progressChanged();
    void availableChanged();
    void metaDataChanged();

    void galleryChanged();
    void propertyNamesChanged();
    void autoUpdateChanged();

protected:
    explicit QDeclarativeGalleryType(QObject *parent = 0);

    QGalleryTypeRequest m_request;
    QDeclarativePropertyMap *m_metaData;
    QHash<int, QString> m_propertyKeys;
    Status m_status;
    bool m_complete;

private Q_SLOTS:
    void _q_stateChanged();
    void _q_typeChanged();
    void _q_metaDataChanged(const QList<int> &keys);
};

class QDeclarativeDocumentGalleryType : public QDeclarativeGalleryType
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeDocumentGallery::ItemType itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
public:
    explicit QDeclarativeDocumentGalleryType(QObject *parent = 0);
    ~QDeclarativeDocumentGalleryType();

    QDeclarativeDocumentGallery::ItemType itemType() const;
    void setItemType(QDeclarativeDocumentGallery::ItemType itemType);

Q_SIGNALS:
    void itemTypeChanged();

};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeDocumentGalleryType))

#endif
