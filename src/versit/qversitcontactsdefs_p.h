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


#ifndef QVERSITCONTACTSDEFS_P_H
#define QVERSITCONTACTSDEFS_P_H

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

#include "qtcontacts.h"
#include "qversitdefs_p.h"

QTM_BEGIN_NAMESPACE

//! [Property name mappings]
// Mappings from versit property names to Qt contact details
const VersitDetailMapping versitContactDetailMappings[] = {
    {"ADR",           QContactAddress::DefinitionName.latin1(),
                      ""},
    {"BDAY",          QContactBirthday::DefinitionName.latin1(),
                      QContactBirthday::FieldBirthday.latin1()},
    {"CATEGORIES",    QContactTag::DefinitionName.latin1(),
                      QContactTag::FieldTag.latin1()},
    {"FN",            QContactDisplayLabel::DefinitionName.latin1(),
                      ""},
    {"GEO",           QContactGeoLocation::DefinitionName.latin1(),
                      ""},
    {"EMAIL",         QContactEmailAddress::DefinitionName.latin1(),
                      QContactEmailAddress::FieldEmailAddress.latin1()},
    {"IMPP",          QContactOnlineAccount::DefinitionName.latin1(),
                      QContactOnlineAccount::SubTypeImpp.latin1()},
    {"LOGO",          QContactOrganization::DefinitionName.latin1(),
                      QContactOrganization::FieldLogoUrl.latin1()},
    {"N",             QContactName::DefinitionName.latin1(),
                      ""},
    {"NICKNAME",      QContactNickname::DefinitionName.latin1(),
                      QContactNickname::FieldNickname.latin1()},
    {"NOTE",          QContactNote::DefinitionName.latin1(),
                      QContactNote::FieldNote.latin1()},
    {"ORG",           QContactOrganization::DefinitionName.latin1(),
                      QContactOrganization::FieldName.latin1()},
    {"PHOTO",         QContactThumbnail::DefinitionName.latin1(),
                      ""},
    {"REV",           QContactTimestamp::DefinitionName.latin1(),
                      ""},
    {"ROLE",          QContactOrganization::DefinitionName.latin1(),
                      QContactOrganization::FieldRole.latin1()},
    {"SOUND",         QContactRingtone::DefinitionName.latin1(),
                      QContactRingtone::FieldAudioRingtoneUrl.latin1()},
    {"TEL",           QContactPhoneNumber::DefinitionName.latin1(),
                      QContactPhoneNumber::FieldNumber.latin1()},
    {"TITLE",         QContactOrganization::DefinitionName.latin1(),
                      QContactOrganization::FieldTitle.latin1()},
    {"UID",           QContactGuid::DefinitionName.latin1(),
                      QContactGuid::FieldGuid.latin1()},
    {"URL",           QContactUrl::DefinitionName.latin1(),
                      QContactUrl::FieldUrl.latin1()},
    {"X-ANNIVERSARY", QContactAnniversary::DefinitionName.latin1(),
                      ""},
    {"X-ASSISTANT",   QContactOrganization::DefinitionName.latin1(),
                      QContactOrganization::FieldAssistantName.latin1()},
    {"X-ASSISTANT-TEL", QContactPhoneNumber::DefinitionName.latin1(),
                      QContactPhoneNumber::SubTypeAssistant.latin1()},
    {"X-CHILDREN",    QContactFamily::DefinitionName.latin1(),
                      QContactFamily::FieldChildren.latin1()},
    {"X-EPOCSECONDNAME",QContactNickname::DefinitionName.latin1(),
                      QContactNickname::FieldNickname.latin1()},
    {"X-GENDER",      QContactGender::DefinitionName.latin1(),
                      QContactGender::FieldGender.latin1()},
    {"X-IMPP",        QContactOnlineAccount::DefinitionName.latin1(),
                      QContactOnlineAccount::SubTypeImpp.latin1()},
    {"X-JABBER",      QContactOnlineAccount::DefinitionName.latin1(),
                      QContactOnlineAccount::SubTypeImpp.latin1()},
    {"X-NICKNAME",    QContactNickname::DefinitionName.latin1(),
                      QContactNickname::FieldNickname.latin1()},
    {"X-SIP",         QContactOnlineAccount::DefinitionName.latin1(),
                      ""},
    {"X-SPOUSE",      QContactFamily::DefinitionName.latin1(),
                      QContactFamily::FieldSpouse.latin1()}
};
//! [Property name mappings]

// Mappings from versit TYPE parameters to Qt contact detail contexts
const VersitMapping versitContextMappings[] = {
    {"HOME", QContactDetail::ContextHome.latin1()},
    {"WORK", QContactDetail::ContextWork.latin1()},
};

//! [Property type parameter mappings]
// Mappings from versit TYPE parameters to Qt contact detail subtypes
const VersitMapping versitSubTypeMappings[] = {
    {"DOM",    QContactAddress::SubTypeDomestic.latin1()},
    {"INTL",   QContactAddress::SubTypeInternational.latin1()},
    {"POSTAL", QContactAddress::SubTypePostal.latin1()},
    {"PARCEL", QContactAddress::SubTypeParcel.latin1()},
    {"VOICE",  QContactPhoneNumber::SubTypeVoice.latin1()},
    {"CELL",   QContactPhoneNumber::SubTypeMobile.latin1()},
    {"MODEM",  QContactPhoneNumber::SubTypeModem.latin1()},
    {"CAR",    QContactPhoneNumber::SubTypeCar.latin1()},
    {"VIDEO",  QContactPhoneNumber::SubTypeVideo.latin1()},
    {"FAX",    QContactPhoneNumber::SubTypeFax.latin1()},
    {"BBS",    QContactPhoneNumber::SubTypeBulletinBoardSystem.latin1()},
    {"PAGER",  QContactPhoneNumber::SubTypePager.latin1()},
    {"SWIS",   QContactOnlineAccount::SubTypeVideoShare.latin1()},
    {"VOIP",   QContactOnlineAccount::SubTypeSipVoip.latin1()}
};
//! [Property type parameter mappings]

QTM_END_NAMESPACE

#endif // QVERSITCONTACTSDEFS_P_H
