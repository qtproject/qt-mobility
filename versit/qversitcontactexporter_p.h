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

#ifndef QVERSITCONTACTEXPORTER_P_H
#define QVERSITCONTACTEXPORTER_P_H

#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QHash>
#include <QObject>

class QContact;
class QContactDetail;


class QVersitContactExporterPrivate : public QObject
{
    Q_OBJECT
public:    
    QVersitContactExporterPrivate();
    ~QVersitContactExporterPrivate();

    void exportContact(QVersitDocument& versitDocument, const QContact& contact);
    
signals:
    void scale(const QString& imageFileName, QByteArray& imageData);

private:
    void encodeName(QVersitProperty& property, const QContactDetail& detail);
    void encodePhoneNumber(QVersitProperty& property, const QContactDetail& detail);
    void encodeEmail(QVersitProperty& property, const QContactDetail& detail);
    void encodeAddress(QVersitProperty& property, const QContactDetail& detail);
    void encodeUrl(QVersitProperty& property, const QContactDetail& detail);
    void encodeUid(QVersitProperty& property, const QContactDetail& detail);
    bool encodeRev(QVersitProperty& property, const QContactDetail& detail);
    void encodeBirthDay(QVersitProperty& property, const QContactDetail& detail);
    void encodeNote(QVersitProperty& property, const QContactDetail& detail);
    void encodeGeoLocation(QVersitProperty& property, const QContactDetail& detail);
    void encodeOrganization(QVersitDocument& document, const QContactDetail& detail);
    void encodeGender(QVersitProperty& property, const QContactDetail& detail);
    void encodeNickname(QVersitDocument& document, const QContactDetail& detail);
    void encodeAnniversary(QVersitProperty& property, const QContactDetail& detail);
    bool encodeOnlineAccount(QVersitProperty& property, const QContactDetail& detail);
    bool encodeFamily(QVersitDocument& document, const QContactDetail& detail);
    bool encodeAvatar(QVersitProperty& property,
        const QContactDetail& detail);
    bool encodeDisplayLabel(QVersitProperty& property,
        const QContactDetail& detail,
        const QContact& contact);
    bool isValidRemoteUrl(const QString& resourceIdentifier);
    void encodeParameters(QVersitProperty& property,
        const QStringList& contexts,
        const QStringList& subTypes=QStringList());
    bool encodeEmbeddedContent(const QString& resourcePath,
        QVersitProperty& property,
        bool performScaling);
    void setEscapedValue(QVersitProperty& property,const QString& value);
    QByteArray escape(const QByteArray& value);

public: // Data
    QList<QContactDetail> mUnknownContactDetails;

private: // Data
    QHash<QString,QString> mPropertyMappings;
    QHash<QString,QString> mParameterMappings;
    QVersitDocument::VersitType mVersitType;

    friend class UT_QVersitContactExporter;
};

#endif // QVERSITCONTACTEXPORTER_P_H
