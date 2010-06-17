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

#ifndef QMLCONTACTMODEL_H
#define QMLCONTACTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativePropertyMap>
#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactfetchrequest.h"
#include "qmlcontact.h"
#include "qversitreader.h"
#include "qversitwriter.h"

QTM_USE_NAMESPACE;
class QMLContactModel : public QAbstractListModel
{
Q_OBJECT
Q_PROPERTY(QStringList availableManagers READ availableManagers)
Q_PROPERTY(QString manager READ manager WRITE setManager)
public:
    explicit QMLContactModel(QObject *parent = 0);

    enum {
        InterestRole = Qt::UserRole + 500,
        InterestLabelRole,
        ContactRole,
        ContactIdRole,
        DetailsRole,
        AvatarRole,
        PresenceAvailableRole,
        PresenceTextRole,
        PresenceStateRole,
        PresenceMessageRole
    };

    QStringList availableManagers() const;

    QString manager() const;
    void setManager(const QString& manager);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE QList<QObject*> details(int id) const;
    Q_INVOKABLE void importFromVcard(const QString& vcard);
    Q_INVOKABLE void exportToVcard(const QString& vcard);
signals:

public slots:

private slots:
    void resultsReceived();
    void fetchAgain();
    void startImport();
private:
    QPair<QString, QString> interestingDetail(const QContact&c) const;
    void exposeContactsToQML();


    QMap<QContactLocalId, QMLContact*> m_contactMap;
    QList<QContact> m_contacts;
    QContactManager* m_manager;
    QContactFetchHint m_fetchHint;
    QContactSortOrder m_sortOrder;
    QContactFetchRequest m_contactsRequest;
    QVersitReader m_reader;
    QVersitWriter m_writer;
};

#endif // QMLCONTACTMODEL_H
