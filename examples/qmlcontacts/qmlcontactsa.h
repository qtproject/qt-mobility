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

#ifndef QMLCONTACTS_H
#define QMLCONTACTS_H

#include <QObject>
#include <qmobilityglobal.h>
#include <QContactManager>
#include <QContactAbstractRequest>

#include "qmlcontact.h"

QTM_BEGIN_NAMESPACE
class QContactFetchRequest;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// ![0]
#include <QtDeclarative>

class QMLContactManagerAsync : public QObject {
Q_OBJECT
Q_PROPERTY(QString availableManagers READ availableManagers)
Q_PROPERTY(QString manager READ manager WRITE setManager)
Q_PROPERTY(int numContacts READ numContacts)
public:
    QMLContactManagerAsync(QObject *parent = 0);
    ~QMLContactManagerAsync();

    QString availableManagers() const;

    QString manager();
    void setManager(QString manager);

    Q_INVOKABLE void contacts();

    int numContacts();

    Q_INVOKABLE QString idToName(QString name);
//    void setName(const QString &);
//
//    int shoeSize() const;
//    void setShoeSize(int);

Q_SIGNALS:
    void dataChanged();
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void relationshipsAdded(const QList<QContactLocalId>& contactIds);
    void relationshipsRemoved(const QList<QContactLocalId>& contactIds);

    void contactsLoaded(QmlContact *contact);
    void contactsLoadedDone();

private slots:

    void contactProgress(QContactAbstractRequest::State newState);

private:
    QContactManager *qc;    
    //QStringList m_contacts;
    QList<QContactLocalId> m_contactIds;
    void fillContactsIntoMemoryEngine(QContactManager* manager);
    QString contactListToQString(const QList<QContactLocalId>& contactIds) const;
    QStringList contactListToQString(const QList<QContact>& contact) const;
};
QML_DECLARE_TYPE(QMLContactManagerAsync)
// ![0]

#endif // QMLCONTACTS_H
