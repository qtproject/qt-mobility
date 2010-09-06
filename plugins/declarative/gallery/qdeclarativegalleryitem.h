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

#ifndef QDECLARATIVEGALLERYITEM_H
#define QDECLARATIVEGALLERYITEM_H

#include <qgalleryitemrequest.h>

#include <QtCore/qpointer.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativepropertymap.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryItem : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ isAutoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QVariant item READ itemId WRITE setItemId NOTIFY itemIdChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(QString itemType READ itemType NOTIFY availableChanged)
    Q_PROPERTY(QUrl itemUrl READ itemUrl NOTIFY availableChanged)
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

    ~QDeclarativeGalleryItem();

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

    QVariant itemId() const { return m_request.itemId(); }
    void setItemId(const QVariant &itemId) {
        m_request.setItemId(itemId); if (m_complete) m_request.execute(); emit itemIdChanged(); }

    bool available() const { return m_request.isValid(); }

    QString itemType() const { return m_request.itemType(); }
    QUrl itemUrl() const { return m_request.itemUrl(); }

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
    void itemIdChanged();

protected:
    explicit QDeclarativeGalleryItem(QObject *parent = 0);

    void setGallery(QAbstractGallery *gallery) { m_request.setGallery(gallery); }

private Q_SLOTS:
    void _q_stateChanged();
    void _q_itemChanged();
    void _q_metaDataChanged(const QList<int> &keys);
    void _q_valueChanged(const QString &key, const QVariant &value) {
        m_request.setMetaData(key, value); }

private:
    QGalleryItemRequest m_request;
    QDeclarativePropertyMap *m_metaData;
    QHash<int, QString> m_propertyKeys;
    Status m_status;
    bool m_complete;
};

class QDeclarativeDocumentGalleryItem : public QDeclarativeGalleryItem
{
    Q_OBJECT
public:
    explicit QDeclarativeDocumentGalleryItem(QObject *parent = 0);
    ~QDeclarativeDocumentGalleryItem();
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeDocumentGalleryItem))

#endif
