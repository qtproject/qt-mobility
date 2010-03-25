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

#ifndef QMLCONTACT_H
#define QMLCONTACT_H

#include <qmobilityglobal.h>
#include <QObject>
#include <QContact>
#include <qdeclarative.h>

QTM_USE_NAMESPACE

class QmlContact : public QObject {
Q_OBJECT
Q_PROPERTY(QContact contact READ contact WRITE setContact NOTIFY contactChanged)
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
Q_PROPERTY(QString test READ test)
Q_PROPERTY(QStringList availableActions READ availableActions)
Q_PROPERTY(QStringList details READ details)
Q_PROPERTY(QStringList contexts READ contexts)
public:
    explicit QmlContact(const QContact& contact, QObject *parent = 0);
    QmlContact();
    ~QmlContact();

    QContact& contact();
    void setContact(QContact& contact);

    QString name();
    void setName(QString name);

    QString email();
    void setEmail(QString email);

    QStringList availableActions();
    QStringList details();

    QStringList contexts();

    Q_INVOKABLE QVariantMap values(QString definitionId);

    QString test() { return "test string"; }

signals:
    void contactChanged(QmlContact* qmlcontact);
    void nameChanged(QmlContact* qmlcontact);
    void emailChanged(QmlContact* qmlcontact);

private:
    QContact m_contact;
};

QML_DECLARE_TYPE(QmlContact);

#endif // QMLCONTACT_H
