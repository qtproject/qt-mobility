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
