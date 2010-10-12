/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECONTACTMODEL_P_H
#define QDECLARATIVECONTACTMODEL_P_H

#include <QAbstractListModel>
#include <QDeclarativeListProperty>
#include "qcontact.h"
#include "qdeclarativecontact_p.h"
#include "qversitreader.h"
#include "qversitwriter.h"

#include "qdeclarativecontactfetchhint_p.h"
#include "qdeclarativecontactsortorder_p.h"
#include "qdeclarativecontactfilter_p.h"

QTM_USE_NAMESPACE;
class QDeclarativeContactModelPrivate;
class QDeclarativeContactModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString manager READ manager WRITE setManager NOTIFY managerChanged)
    Q_PROPERTY(QStringList availableManagers READ availableManagers)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QDeclarativeContactFilter* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDeclarativeContactFetchHint* fetchHint READ fetchHint WRITE setFetchHint NOTIFY fetchHintChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContact> contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContactSortOrder> sortOrders READ sortOrders NOTIFY sortOrdersChanged)

public:
    explicit QDeclarativeContactModel(QObject *parent = 0);

    enum {
        InterestRole = Qt::UserRole + 500,
        InterestLabelRole,
        ContactIdRole,
        ContactRole,
        DetailsRole,
        AvatarRole,
        PresenceAvailableRole,
        PresenceTextRole,
        PresenceStateRole,
        PresenceMessageRole
    };

    QString manager() const;
    void setManager(const QString& manager);

    QStringList availableManagers() const;
    
    QString error() const;

    QDeclarativeContactFilter* filter() const;
    void setFilter(QDeclarativeContactFilter* filter);

    QDeclarativeContactFetchHint* fetchHint() const;
    void setFetchHint(QDeclarativeContactFetchHint* fetchHint);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QDeclarativeListProperty<QDeclarativeContact> contacts() ;
    QDeclarativeListProperty<QDeclarativeContactSortOrder> sortOrders() ;

    Q_INVOKABLE void removeContact(QContactLocalId id);
    Q_INVOKABLE void removeContacts(const QList<QContactLocalId>& id);
    Q_INVOKABLE void saveContact(QDeclarativeContact* dc);


signals:
    void managerChanged();
    void filterChanged();
    void errorChanged();
    void fetchHintChanged();
    void contactsChanged();
    void vcardChanged();
    void sortOrdersChanged();

public slots:
    void exportContacts(const QString& file);
    void importContacts(const QString& file);
private slots:
    void fetchAgain();
    void contactFetched();

    void saveContact();
    void contactSaved();

    void removeContact();
    void contactRemoved();

    void startImport(QVersitReader::State state);
    void contactsExported(QVersitWriter::State state);



private:
    QPair<QString, QString> interestingDetail(const QContact&c) const;
    QDeclarativeContactModelPrivate* d;
};

#endif // QDECLARATIVECONTACTMODEL_P_H
