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
Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
Q_PROPERTY(QPixmap thumbnail READ thumbnail)
Q_PROPERTY(bool hasThumbnail READ hasThumbnail)
Q_PROPERTY(QStringList details READ details)
Q_PROPERTY(QStringList contexts READ contexts)
Q_PROPERTY(QString interest READ interest)
Q_PROPERTY(QString interestLabel READ interestLabel)
public:
    explicit QmlContact(const QContact& contact, QObject *parent = 0);
    QmlContact();
    ~QmlContact();

    QContact& contact();
    void setContact(QContact& contact);

    QString name() const;
    void setName(QString name);

    QString email() const;
    void setEmail(QString email);

    bool hasThumbnail() const;
    QString avatar() const;
    QPixmap thumbnail() const;

    QString interest() const;
    QString interestLabel() const;

    QStringList details();

    QStringList contexts();

    Q_INVOKABLE QVariantMap values(QString definitionId);

signals:
    void contactChanged(QmlContact* qmlcontact);
    void nameChanged(QmlContact* qmlcontact);
    void emailChanged(QmlContact* qmlcontact);

private:
    QContact m_contact;
};

QML_DECLARE_TYPE(QmlContact)

#endif // QMLCONTACT_H
