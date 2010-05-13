/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"
#include "qversitcontactdefaulthandlers.h"

#include <QHash>
#include <QObject>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;

class Q_AUTOTEST_EXPORT QVersitContactExporterPrivate
{
public:
    QVersitContactExporterPrivate();
    ~QVersitContactExporterPrivate();

    bool exportContact(const QContact& contact, QVersitDocument& versitDocument,
                       QVersitContactExporter::Error* error);

protected:
    static bool documentContainsName(const QVersitDocument& document);
    void encodeName(
            const QContactDetail& detail,
            QVersitDocument* document,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodePhoneNumber(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeEmail(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeAddress(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeUrl(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeUid(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeRev(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeBirthDay(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeNote(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeGeoLocation(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeOrganization(
            const QContactDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeRingtone(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeThumbnail(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeAvatar(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeGender(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeNickname(
            const QContactDetail &detail,
            QVersitDocument* document,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeTag(
            const QContactDetail &detail,
            QVersitDocument* document,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeAnniversary(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeOnlineAccount(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeFamily(
            const QContactDetail &detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeDisplayLabel(
            const QContactDetail &detail,
            QVersitDocument* document,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    QVersitProperty takeProperty(QVersitDocument* document, const QString& propertyName);
    bool isValidRemoteUrl(const QString& resourceIdentifier);
    void encodeParameters(QVersitProperty& property,
        const QStringList& contexts,
        const QStringList& subTypes=QStringList());
    bool encodeContentFromFile(const QString& resourcePath, QVersitProperty& property);

public: // Data
    QList<QVersitDocument> mDocuments;
    QMap<int, QVersitContactExporter::Error> mErrors;
    QVersitContactExporterDetailHandler* mDetailHandler;
    QVersitContactExporterDetailHandlerV2* mDetailHandler2;
    int mDetailHandlerVersion;
    QVersitContactExporterDefaultDetailHandler* mDefaultDetailHandler;
    QVersitDefaultResourceHandler* mDefaultResourceHandler;
    QVersitResourceHandler* mResourceHandler;
    QHash<QString,QString> mPropertyMappings;
    QHash<QString,QString> mParameterMappings;
    QVersitDocument::VersitType mVersitType;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTEXPORTER_P_H
