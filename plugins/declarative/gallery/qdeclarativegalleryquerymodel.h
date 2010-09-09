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

#ifndef GALLERYQUERYREQUEST_H
#define GALLERYQUERYREQUEST_H

#include <qgalleryqueryrequest.h>

#include "qdeclarativedocumentgallery.h"
#include "qdeclarativegalleryfilter.h"

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtScript/QtScript>

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryFilterBase;

class QDeclarativeGalleryQueryModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(Status)
    Q_ENUMS(Scope)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(QStringList sortProperties READ sortPropertyNames WRITE setSortPropertyNames NOTIFY sortPropertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ isAutoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QVariant rootItem READ rootItem WRITE setRootItem NOTIFY rootItemChanged)
    Q_PROPERTY(Scope scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeGalleryFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
public:
    enum Status
    {
        Null        = QGalleryAbstractRequest::Inactive,
        Active      = QGalleryAbstractRequest::Active,
        Finished    = QGalleryAbstractRequest::Finished,
        Idle        = QGalleryAbstractRequest::Idle,
        Cancelling  = QGalleryAbstractRequest::Cancelling,
        Cancelled   = QGalleryAbstractRequest::Cancelled,
        Error       = QGalleryAbstractRequest::Error
    };

    enum Scope
    {
        AllDescendants = QGalleryQueryRequest::AllDescendants,
        DirectDescendants = QGalleryQueryRequest::DirectDescendants
    };

    enum Roles
    {
        ItemId = 0,
        ItemType,
        MetaDataOffset
    };

    ~QDeclarativeGalleryQueryModel();

    Status status() const { return m_status; }

    qreal progress() const
    {
        const int max = m_request.maximumProgress();
        return max > 0 ? qreal(m_request.currentProgress()) / max : qreal(0.0);
    }

    QString errorMessage() const { return m_errorMessage; }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) { m_request.setPropertyNames(names); emit propertyNamesChanged(); } }

    QStringList sortPropertyNames() const { return m_request.sortPropertyNames(); }
    void setSortPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setSortPropertyNames(names); emit sortPropertyNamesChanged(); }

    bool isAutoUpdate() const { return m_request.isAutoUpdate(); }
    void setAutoUpdate(bool enabled) { m_request.setAutoUpdate(enabled); emit autoUpdateChanged(); }

    Scope scope() const { return Scope(m_request.scope()); }
    void setScope(Scope scope) {
        m_request.setScope(QGalleryQueryRequest::Scope(scope)); emit scopeChanged(); }

    QVariant rootItem() const { return m_request.rootItem(); }
    void setRootItem(const QVariant &itemId) {
        m_request.setRootItem(itemId); emit rootItemChanged(); }

    QDeclarativeGalleryFilterBase *filter() const { return m_filter; }
    void setFilter(QDeclarativeGalleryFilterBase *filter) { m_filter = filter; filterChanged(); }

    int offset() const { return m_request.offset(); }
    void setOffset(int offset) { m_request.setOffset(offset); emit offsetChanged(); }

    int limit() const { return m_request.limit(); }
    void setLimit(int limit) { m_request.setLimit(limit); emit limitChanged(); }

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    int count() const { return m_rowCount; }

    Q_INVOKABLE QScriptValue get(const QScriptValue &index) const;
    Q_INVOKABLE QVariant property(int index, const QString &property) const;

    Q_INVOKABLE void set(int index, const QScriptValue &value);
    Q_INVOKABLE void setProperty(int index, const QString &property, const QVariant &value);

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void reload();
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void statusChanged();
    void progressChanged();
    void errorMessageChanged();
    void propertyNamesChanged();
    void sortPropertyNamesChanged();
    void autoUpdateChanged();
    void rootItemChanged();
    void scopeChanged();
    void filterChanged();
    void offsetChanged();
    void limitChanged();
    void countChanged();

protected:
    explicit QDeclarativeGalleryQueryModel(QObject *parent = 0);

    virtual QVariant itemType(const QString &type) const = 0;

    QGalleryQueryRequest m_request;
    QPointer<QDeclarativeGalleryFilterBase> m_filter;
    QGalleryResultSet *m_resultSet;
    QVector<QPair<int, QString> > m_propertyNames;
    QString m_errorMessage;
    Status m_status;
    int m_rowCount;
    bool m_complete;

private Q_SLOTS:
    void _q_statusChanged();
    void _q_setResultSet(QGalleryResultSet *resultSet);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_itemsChanged(int index, int count);
};

class QDeclarativeDocumentGalleryModel : public QDeclarativeGalleryQueryModel
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeDocumentGallery::ItemType rootType READ rootType WRITE setRootType NOTIFY rootTypeChanged)
public:
    explicit QDeclarativeDocumentGalleryModel(QObject *parent = 0);
    ~QDeclarativeDocumentGalleryModel();

    QDeclarativeDocumentGallery::ItemType rootType() const;
    void setRootType(QDeclarativeDocumentGallery::ItemType itemType);

Q_SIGNALS:
    void rootTypeChanged();

protected:
    QVariant itemType(const QString &type) const;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeDocumentGalleryModel))

#endif

