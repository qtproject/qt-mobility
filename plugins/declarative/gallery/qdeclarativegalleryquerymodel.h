/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GALLERYQUERYREQUEST_H
#define GALLERYQUERYREQUEST_H

#include <qgalleryqueryrequest.h>
#include "qdeclarativegalleryfilter.h"

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
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery NOTIFY galleryChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(Result result READ result NOTIFY resultChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QStringList properties READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(QStringList sortProperties READ sortPropertyNames WRITE setSortPropertyNames NOTIFY sortPropertyNamesChanged)
    Q_PROPERTY(bool live READ isLive WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(QString rootType READ rootType WRITE setRootType NOTIFY rootTypeChanged)
    Q_PROPERTY(QVariant rootItem READ rootItem WRITE setRootItem NOTIFY rootItemChanged)
    Q_PROPERTY(Scope scope READ scope WRITE setScope NOTIFY scopeChanged)
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
        MetaDataOffset
    };

    QDeclarativeGalleryQueryModel(QObject *parent = 0);
    ~QDeclarativeGalleryQueryModel();

    QAbstractGallery *gallery() const { return m_request.gallery(); }
    void setGallery(QAbstractGallery *gallery) {
        if (!m_complete || !gallery) { m_request.setGallery(gallery); emit galleryChanged(); } }

    State state() const { return State(m_request.state()); }
    Result result() const { return Result(m_request.result()); }

    qreal progress() const
    {
        const int max = m_request.maximumProgress();
        return max > 0 ? qreal(m_request.currentProgress()) / max : qreal(0.0);
    }

    QStringList propertyNames() { return m_request.propertyNames(); }
    void setPropertyNames(const QStringList &names) {
        if (!m_complete) { m_request.setPropertyNames(names); emit propertyNamesChanged(); } }

    QStringList sortPropertyNames() const { return m_request.sortPropertyNames(); }
    void setSortPropertyNames(const QStringList &names) {
        if (!m_complete) m_request.setSortPropertyNames(names); emit sortPropertyNamesChanged(); }

    bool isLive() const { return m_request.isLive(); }
    void setLive(bool live) { m_request.setLive(live); emit liveChanged(); }

    QString rootType() const { return m_request.rootType(); }
    void setRootType(const QString &itemType) {
        if (!m_complete) { m_request.setRootType(itemType); emit rootTypeChanged(); } }

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
    void galleryChanged();
    void propertyNamesChanged();
    void sortPropertyNamesChanged();
    void liveChanged();
    void rootTypeChanged();
    void rootItemChanged();
    void scopeChanged();
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

