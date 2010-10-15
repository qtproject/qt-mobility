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

#ifndef QDECLARATIVECONTACTDETAIL_P_H
#define QDECLARATIVECONTACTDETAIL_P_H

#include <qdeclarative.h>
#include "qcontact.h"
#include "qcontactdetail.h"

QTM_USE_NAMESPACE;

class QDeclarativeContactDetail : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString definitionName READ definitionName NOTIFY valueChanged)
    Q_PROPERTY(QStringList contexts READ contexts WRITE setContexts NOTIFY valueChanged)
    Q_PROPERTY(QString detailUri READ detailUri WRITE setDetailUri NOTIFY valueChanged)
    Q_PROPERTY(QStringList linkedDetailUris READ linkedDetailUris WRITE setLinkedDetailUris NOTIFY valueChanged)
    Q_PROPERTY(QStringList fieldNames READ fieldNames NOTIFY valueChanged)
    Q_PROPERTY(bool readOnly READ readOnly NOTIFY valueChanged)
    Q_PROPERTY(bool removable READ removable NOTIFY valueChanged)
    Q_ENUMS(ContactDetailType)
public:
    QDeclarativeContactDetail(QObject* parent = 0);


    enum ContactDetailType {
        ContactAddress = 0,
        ContactAnniversary,
        ContactAvatar,
        ContactBirthday,
        ContactDisplayLabel,
        ContactEmail,
        ContactFamily,
        ContactFavorite,
        ContactGender,
        ContactGeolocation,
        ContactGlobalPresence,
        ContactGuid,
        ContactName,
        ContactNickName,
        ContactNote,
        ContactOnlineAccount,
        ContactOrganization,
        ContactPhoneNumber,
        ContactPresence,
        ContactRingtone,
        ContactSyncTarget,
        ContactTag,
        ContactTimestamp,
        ContactUrl,
        ContactHobby,
        ContactCustomizedDetail = 100
    };

    QContactDetail& detail();
    const QContactDetail& detail() const;
    void setDetail(const QContactDetail& detail);

    bool readOnly() const;
    bool removable() const;

    QString definitionName() const;

    QStringList contexts() const;
    void setContexts(const QStringList& context);

    QString detailUri() const;
    void setDetailUri(const QString& detailUri);

    QStringList linkedDetailUris() const;
    void setLinkedDetailUris(const QStringList& linkedDetailUris);
    virtual ContactDetailType detailType() const;

    QStringList fieldNames() const;
    Q_INVOKABLE QVariant value(const QString& key) const;
    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value);

    static QString definitionName(ContactDetailType type) ;
    static ContactDetailType detailType(const QString& definitionName) ;
    static QString fieldName(ContactDetailType detailType, int fieldType);
signals:
    void valueChanged();

private:
    QContactDetail m_detail;
};

QML_DECLARE_TYPE(QDeclarativeContactDetail)

#endif // QDECLARATIVECONTACTDETAIL_P_H
