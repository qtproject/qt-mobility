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


#ifndef QVERSITDEFS_H
#define QVERSITDEFS_H

#include "qmobilityglobal.h"

#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactemailaddress.h>
#include <qcontactphonenumber.h>
#include "qcontactaddress.h"
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontactorganization.h>
#include <qcontacttimestamp.h>
#include <qcontactbirthday.h>
#include <qcontactnote.h>
#include <qcontactgeolocation.h>
#include <qcontactavatar.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactanniversary.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <qcontactdisplaylabel.h>

QTM_BEGIN_NAMESPACE

// Mapping between a string in versit specifications and Qt contact details
struct versitContactDetailMapping {
    const char* versitPropertyName;
    const char* contactDetailDefinitionName;
    const char* contactDetailValueKey;
};

//! [Property name mappings]
// Mappings from versit property names to Qt contact details
const versitContactDetailMapping versitContactDetailMappings[] = {
    {"ADR",           QContactAddress::DefinitionName.str,
                      ""},
    {"BDAY",          QContactBirthday::DefinitionName.str,
                      QContactBirthday::FieldBirthday.str},
    {"FN",            QContactDisplayLabel::DefinitionName.str,
                      ""},
    {"GEO",           QContactGeolocation::DefinitionName.str,
                      ""},
    {"EMAIL",         QContactEmailAddress::DefinitionName.str,
                      QContactEmailAddress::FieldEmailAddress.str},
    {"IMPP",          QContactOnlineAccount::DefinitionName.str,
                      QContactOnlineAccount::SubTypeImpp.str},
    {"LOGO",          QContactOrganization::DefinitionName.str,
                      QContactOrganization::FieldLogo.str},
    {"N",             QContactName::DefinitionName.str,
                      ""},
    {"NICKNAME",      QContactNickname::DefinitionName.str,
                      QContactNickname::FieldNickname.str},
    {"NOTE",          QContactNote::DefinitionName.str,
                      QContactNote::FieldNote.str},
    {"ORG",           QContactOrganization::DefinitionName.str,
                      QContactOrganization::FieldName.str},
    {"PHOTO",         QContactAvatar::DefinitionName.str,
                      QContactAvatar::SubTypeImage.str},
    {"REV",           QContactTimestamp::DefinitionName.str,
                      ""},
    {"ROLE",          QContactOrganization::DefinitionName.str,
                      QContactOrganization::FieldRole.str},
    {"SOUND",         QContactAvatar::DefinitionName.str,
                      QContactAvatar::SubTypeAudioRingtone.str},
    {"TEL",           QContactPhoneNumber::DefinitionName.str,
                      QContactPhoneNumber::FieldNumber.str},
    {"TITLE",         QContactOrganization::DefinitionName.str,
                      QContactOrganization::FieldTitle.str},
    {"UID",           QContactGuid::DefinitionName.str,
                      QContactGuid::FieldGuid.str},
    {"URL",           QContactUrl::DefinitionName.str,
                      QContactUrl::FieldUrl.str},
    {"X-ANNIVERSARY", QContactAnniversary::DefinitionName.str,
                      ""},
    {"X-ASSISTANT",   QContactOrganization::DefinitionName.str,
                      QContactOrganization::FieldAssistantName.str},
    {"X-CHILDREN",    QContactFamily::DefinitionName.str,
                      QContactFamily::FieldChildren.str},
    {"X-GENDER",      QContactGender::DefinitionName.str,
                      QContactGender::FieldGender.str},
    {"X-IMPP",        QContactOnlineAccount::DefinitionName.str,
                      QContactOnlineAccount::SubTypeImpp.str},
    {"X-NICKNAME",    QContactNickname::DefinitionName.str,
                      QContactNickname::FieldNickname.str},
    {"X-SIP",         QContactOnlineAccount::DefinitionName.str,
                      ""},
    {"X-SPOUSE",      QContactFamily::DefinitionName.str,
                      QContactFamily::FieldSpouse.str}
};
//! [Property name mappings]

// Mapping between a string in versit specifications and Qt contacts
struct versitMapping {
    const char* versitString;
    const char* contactString;
};

// Mappings from versit TYPE parameters to Qt contact detail contexts
const versitMapping versitContextMappings[] = {
    {"HOME", QContactDetail::ContextHome.str},
    {"WORK", QContactDetail::ContextWork.str},
};

//! [Property type parameter mappings]
// Mappings from versit TYPE parameters to Qt contact detail subtypes
const versitMapping versitSubTypeMappings[] = {
    {"DOM",    QContactAddress::SubTypeDomestic.str},
    {"INTL",   QContactAddress::SubTypeInternational.str},
    {"POSTAL", QContactAddress::SubTypePostal.str},
    {"PARCEL", QContactAddress::SubTypeParcel.str},
    {"VOICE",  QContactPhoneNumber::SubTypeVoice.str},
    {"CELL",   QContactPhoneNumber::SubTypeMobile.str},
    {"MODEM",  QContactPhoneNumber::SubTypeModem.str},
    {"CAR",    QContactPhoneNumber::SubTypeCar.str},
    {"VIDEO",  QContactPhoneNumber::SubTypeVideo.str},
    {"FAX",    QContactPhoneNumber::SubTypeFacsimile.str},
    {"BBS",    QContactPhoneNumber::SubTypeBulletinBoardSystem.str},
    {"PAGER",  QContactPhoneNumber::SubTypePager.str},
    {"SWIS",   QContactOnlineAccount::SubTypeVideoShare.str},
    {"VOIP",   QContactOnlineAccount::SubTypeSipVoip.str}
};
//! [Property type parameter mappings]

//! [File extension mappings]
// Mappings from file types in versit specifications to file extensions
const versitMapping versitFileExtensionMappings[] = {
    {"JPEG", "JPG"},
    {"WAVE", "WAV"},
    {"PICT", "PCT"},
    {"TIFF", "TIF"},
    {"MPEG", "MPG"},
    {"MPEG2", "M2P"},
    {"QTIME", "QT"},
    {"AIFF", "AIF"},
    {"GIF", "GIF"}
};
//! [File extension mappings]

QTM_END_NAMESPACE

#endif // QVERSITDEFS_H
