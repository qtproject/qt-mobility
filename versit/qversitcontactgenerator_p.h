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

#ifndef QVERSITCONTACTGENERATOR_P_H
#define QVERSITCONTACTGENERATOR_P_H

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

#include "qversitdefs.h"
#include <QList>
#include <QDateTime>
#include <QHash>
#include <QPair>

class QContact;
class QContactDetail;
class QContactOrganization;
class QVersitProperty;
class QVersitDocument;

class QVersitContactGeneratorPrivate
{
public:
    QVersitContactGeneratorPrivate();
    ~QVersitContactGeneratorPrivate();

    QContact generateContact(const QVersitDocument& versitDocument);
    QList<QVersitProperty> unconvertedVersitProperties();
    
private:
    QContactDetail* createName(
        const QVersitProperty& property,
        const QContact& contact) const;
    QContactDetail* createPhone(const QVersitProperty& property) const;
    QContactDetail* createAddress(const QVersitProperty& property) const;
    QContactDetail* createOrganization(const QVersitProperty& property) const;
    void setOrganizationNames(
        QContactOrganization& org,
        const QVersitProperty& property) const;
    void setOrganizationLogo(
        QContactOrganization& org,
        const QVersitProperty& property) const;
    QContactDetail* createTimeStamp(const QVersitProperty& property) const;
    QContactDetail* createAnniversary(const QVersitProperty& property) const;
    QContactDetail* createBirthday(const QVersitProperty& property) const;    
    QContactDetail* createAvatar(
        const QVersitProperty& property,
        const QVersitDocument& versitDocument,
        const QString& subType) const;
    void createNicknames(
        const QVersitProperty& property,
        QContact& contact) const;
    QContactDetail* createGeoLocation(const QVersitProperty& property) const;
    QContactDetail* createOnlineAccount(const QVersitProperty& property) const;
    QContactDetail* createFamily(
        const QVersitProperty& property,
        const QContact& contact)const;
    QContactDetail* createNameValueDetail(const QVersitProperty& property) const;

private: // Utilities
    QStringList extractContexts(const QVersitProperty& property) const;
    QStringList extractSubTypes(const QVersitProperty& property) const;
    QString takeFirst(QList<QByteArray>& list) const;
    QDateTime parseDateTime(const QByteArray& text, const QByteArray& format) const;    
    QString saveContentToFile(
        const QString& path,
        const QVersitProperty& property) const;
    QString getFirstAndLastName(const QVersitDocument& document) const;
    
public: // Data
    QString mImagePath;
    QString mAudioClipPath;
    QList<QVersitProperty> mUnconvertedVersitProperties;

private: // Data
    QHash<QString,QPair<QString,QString> > mDetailMappings;
    QHash<QString,QString> mContextMappings;
    QHash<QString,QString> mSubTypeMappings;
    QHash<QString,QString> mFileExtensionMappings;
};

#endif // QVERSITCONTACTGENERATOR_P_H
