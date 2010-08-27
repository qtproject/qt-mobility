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

#ifndef QVERSITCONTACTIMPORTER_P_H
#define QVERSITCONTACTIMPORTER_P_H

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

#include "qmobilityglobal.h"
#include "qversitcontactimporter.h"

#include <QList>
#include <QDateTime>
#include <QHash>
#include <QPair>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
class QContactOrganization;
class QVersitProperty;
class QVersitDocument;

class Q_AUTOTEST_EXPORT QVersitContactImporterPrivate
{
public:
    QVersitContactImporterPrivate();
    ~QVersitContactImporterPrivate();

    bool importContact(const QVersitDocument& versitDocument, int contactIndex,
                       QContact* contact, QVersitContactImporter::Error* error);
    QList<QVersitProperty> unconvertedVersitProperties();

    static QString synthesizedDisplayLabel(const QContact& contact);

private:
    void importProperty(const QVersitDocument& document, const QVersitProperty& property,
                        int contactIndex, QContact* contact) const;
    bool createName(const QVersitProperty& property, QContact* contact) const;
    bool createPhone(const QVersitProperty& property, QContact* contact) const;
    bool createAddress(const QVersitProperty& property, QContact* contact) const;
    bool createOrganization(const QVersitProperty& property, QContact* contact) const;
    void setOrganizationNames(QContactOrganization& org, const QVersitProperty& property) const;
    void setOrganizationLogo(QContactOrganization& org, const QVersitProperty& property) const;
    bool createTimeStamp(const QVersitProperty& property, QContact* contact) const;
    bool createAnniversary(const QVersitProperty& property, QContact* contact) const;
    bool createBirthday(const QVersitProperty& property, QContact* contact) const;
    bool createNicknames(const QVersitProperty& property, QContact* contact) const;
    bool createTags(const QVersitProperty& property, QContact* contact) const;
    bool createOnlineAccount(const QVersitProperty& property, QContact* contact) const;
    bool createRingtone(const QVersitProperty& property, QContact* contact) const;
    bool createThumbnail(const QVersitProperty& property, QContact* contact) const;
    bool createGeoLocation(const QVersitProperty& property, QContact* contact) const;
    bool createFamily(const QVersitProperty& property, QContact* contact) const;
    bool createNameValueDetail(const QVersitProperty& property, QContact* contact) const;
    bool createCustomLabel(const QVersitProperty& property, QContact* contact) const;
    QStringList extractContexts(const QVersitProperty& property) const;
    QStringList extractSubTypes(const QVersitProperty& property) const;
    QString takeFirst(QList<QString>& list) const;
    QDateTime parseDateTime(const QString& text, const QString& format) const;
    QString saveContentToFile(const QVersitProperty& property, const QByteArray& data) const;
    bool saveDataFromProperty(const QVersitProperty& property, QString* location, QByteArray* data) const;
    void saveDetailWithContext(
            QContact* contact, QContactDetail* detail, const QStringList& contexts) const;

public: // Data
    QList<QContact> mContacts;
    QMap<int, QVersitContactImporter::Error> mErrors;
    QVersitContactImporterPropertyHandler* mPropertyHandler;
    QVersitDefaultResourceHandler* mDefaultResourceHandler;
    QVersitResourceHandler* mResourceHandler;

    QHash<QString,QPair<QString,QString> > mDetailMappings;
    QHash<QString,QString> mContextMappings;
    QHash<QString,QString> mSubTypeMappings;
};

QTM_END_NAMESPACE

#endif // QVERSITCONTACTIMPORTER_P_H
