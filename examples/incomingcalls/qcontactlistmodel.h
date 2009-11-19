/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTLISTMODEL_H
#define QCONTACTLISTMODEL_H

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

#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactphonenumber.h"

#include <QAbstractListModel>
#include <QSharedData>

QTM_BEGIN_NAMESPACE
class QContactManager;
class QContactFetchRequest;
class QContactLocalIdFetchRequest;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QContactListModelPrivate;
class QContactListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    QContactListModel(QContactManager* manager = 0, int cacheSize = 50);
    QContactListModel(const QContactListModel& other);
    QContactListModel& operator=(const QContactListModel& other);
    ~QContactListModel();

    QContactManager* manager() const;
    void setManager(QContactManager* manager);

    int cacheSize() const;
    bool setCacheSize(int size);

    enum AsynchronousRequestPolicy {
        CancelOnCacheUpdatePolicy = 0,      // cancel old requests whenever a cache update should occur
        CancelOnCacheMissPolicy,            // cancel if old centrepoint outside current cache window
        CancelOnCompleteCacheMissPolicy,    // cancel if no overlap between request and current cache windows
        NeverCancelPolicy                   // never cancel old requests
    };

    AsynchronousRequestPolicy requestPolicy() const;
    void setRequestPolicy(AsynchronousRequestPolicy policy = CancelOnCacheMissPolicy);

    QString relevantDefinitionName() const;
    QString relevantFieldName() const;
    bool setRelevantDetailDefinitionAndFieldNames(const QString& definitionName = QContactPhoneNumber::DefinitionName, const QString& fieldName = QContactPhoneNumber::FieldNumber);

    enum ContactDataRole {
        DisplayLabelRole = Qt::DisplayRole,
        IdRole = Qt::UserRole,
        AvatarRole = Qt::UserRole+1,
        PresenceRole = Qt::UserRole+2,
        RelevantDataRole = Qt::UserRole+3
    };

    int rowCount (const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    int contactRow(const QContactLocalId& contactId) const;
    QContact contact(const QModelIndex& index) const;

private slots:
    void contactFetchRequestProgress(QContactFetchRequest* request, bool appendOnly);
    void contactIdFetchRequestProgress(QContactLocalIdFetchRequest* request, bool appendOnly);
    void backendChanged();

private:
    QSharedDataPointer<QContactListModelPrivate> d;
};

#endif
