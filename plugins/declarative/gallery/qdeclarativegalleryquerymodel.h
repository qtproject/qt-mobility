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

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryFilterBase;

class QDeclarativeGalleryQueryModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(State)
    Q_ENUMS(Result)
    Q_ENUMS(Scope)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(Result result READ result NOTIFY resultChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames)
    Q_PROPERTY(QStringList sortProperties READ sortPropertyNames WRITE setSortPropertyNames NOTIFY sortPropertyNamesChanged)
    Q_PROPERTY(bool live READ isLive WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(QString rootType READ rootType WRITE setRootType)
    Q_PROPERTY(QVariant rootItem READ rootItem WRITE setRootItem NOTIFY rootItemChanged)
    Q_PROPERTY(Scope scope READ scope WRITE setScope)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(QDeclarativeGalleryFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
public:
    enum State
    {
        Inactive    = QGalleryAbstractRequest::Inactive,
        Active      = QGalleryAbstractRequest::Active,
        Cancelling  = QGalleryAbstractRequest::Cancelling,
        Idle        = QGalleryAbstractRequest::Idle
    };

    enum Result
    {
        NoResult                        = QGalleryAbstractRequest::NoResult,
        Succeeded                       = QGalleryAbstractRequest::NoResult,
        Cancelled                       = QGalleryAbstractRequest::NoResult,
        NoGallery                       = QGalleryAbstractRequest::NoResult,
        NotSupported                    = QGalleryAbstractRequest::NoResult,
        ConnectionError                 = QGalleryAbstractRequest::NoResult,
        InvalidItemError                = QGalleryAbstractRequest::NoResult,
        ItemTypeError                   = QGalleryAbstractRequest::NoResult,
        InvalidPropertyError            = QGalleryAbstractRequest::NoResult,
        PropertyTypeError               = QGalleryAbstractRequest::NoResult,
        UnsupportedFilterTypeError      = QGalleryAbstractRequest::NoResult,
        UnsupportedFilterOptionError    = QGalleryAbstractRequest::NoResult
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
        ItemUrl,
        MetaDataOffset
    };

    QDeclarativeGalleryQueryModel(QObject *parent = 0);
    ~QDeclarativeGalleryQueryModel();

    QAbstractGallery *gallery() const { return m_request.gallery(); }
    void setGallery(QAbstractGallery *gallery) { m_request.setGallery(gallery); }

    State state() const { return State(m_request.state()); }
    Result result() const { return Result(m_request.result()); }

    int currentProgress() const { return m_request.currentProgress(); }
    int maximumProgress() const { return m_request.maximumProgress(); }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setPropertyNames(names); }

    QStringList sortPropertyNames() const { return m_request.sortPropertyNames(); }
    void setSortPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setSortPropertyNames(names); emit sortPropertyNamesChanged(); }

    bool isLive() const { return m_request.isLive(); }
    void setLive(bool live) { m_request.setLive(live); emit liveChanged(); }

    QString rootType() const { return m_request.rootType(); }
    void setRootType(const QString &itemType) {
        if (!m_complete) m_request.setRootType(itemType); emit rootTypeChanged(); }

    Scope scope() const { return Scope(m_request.scope()); }
    void setScope(Scope scope) { m_request.setScope(QGalleryQueryRequest::Scope(scope)); }

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

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void reload();
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged();
    void resultChanged();
    void progressChanged();
    void sortPropertyNamesChanged();
    void liveChanged();
    void rootTypeChanged();
    void rootItemChanged();
    void filterChanged();
    void offsetChanged();
    void limitChanged();

private Q_SLOTS:
    void _q_setResultSet(QGalleryResultSet *resultSet);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_itemsChanged(int index, int count);

private:
    QGalleryQueryRequest m_request;
    QPointer<QDeclarativeGalleryFilterBase> m_filter;
    QGalleryResultSet *m_resultSet;
    int m_rowCount;
    bool m_complete;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryQueryModel))

#endif

