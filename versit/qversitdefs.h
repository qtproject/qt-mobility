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

#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactemailaddress.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
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

// Mapping between a string in versit specifications and Qt contacts
struct versitMapping {
    const char* versitString;
    const char* contactString;
};

// Mappings from versit property names to Qt contact details
const versitMapping versitDetailMappings[] = {
    {"N",             QContactName::DefinitionName.str},
    {"FN", 			  QContactDisplayLabel::DefinitionName.str},
    {"EMAIL",         QContactEmailAddress::DefinitionName.str},
    {"TEL",           QContactPhoneNumber::DefinitionName.str},
    {"ADR",           QContactAddress::DefinitionName.str},
    {"URL",           QContactUrl::DefinitionName.str},
    {"REV",           QContactTimestamp::DefinitionName.str},
    {"UID",           QContactGuid::DefinitionName.str},
    {"ORG",           QContactOrganization::DefinitionName.str},
    {"BDAY",          QContactBirthday::DefinitionName.str},
    {"NOTE",          QContactNote::DefinitionName.str},
    {"GEO",           QContactGeolocation::DefinitionName.str},
    {"NICKNAME",      QContactNickname::DefinitionName.str},
    {"PHOTO",         QContactAvatar::SubTypeImage.str},
    {"SOUND",         QContactAvatar::SubTypeAudioRingtone.str},
    {"X-ANNIVERSARY", QContactAnniversary::DefinitionName.str},
    {"X-GENDER",      QContactGender::DefinitionName.str},
    {"X-NICKNAME",    QContactNickname::DefinitionName.str}
};

// Mappings from versit TYPE parameters to Qt contact detail contexts
const versitMapping versitContextMappings[] = {
    {"HOME", QContactDetail::ContextHome.str},
    {"WORK", QContactDetail::ContextWork.str},
};

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
    {"SWIS",   QContactOnlineAccount::SubTypeShareVideo.str},
    {"VOIP",   QContactOnlineAccount::SubTypeInternet.str}
};

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
    {"TIFF", "GIF"}
};

// Versit parameter names
const char versitType[] = "TYPE";
const char versitValue[] = "VALUE";
const char versitEncoding[] = "ENCODING";

// Versit encoding parameter values
const char versitEncoding7Bit[] = "7BIT";
const char versitEncoding8Bit[] = "8BIT";
const char versitEncodingQuotedPrintable[] = "QUOTED-PRINTABLE";
const char versitEncodingBase64[] = "BASE64";
const char versitEncodingBinary[] = "B";

// ISO-8601 basic format datetime spec
const char versitDateTimeSpecIso8601Basic[] = "yyyyMMddThhmmss";
const char versitDateSpecIso8601Basic[] = "yyyyMMdd";

// ISO 8601 Extended Format suffix
const char versitISOFormatSuffix[] = "Z";

#endif // QVERSITDEFS_H
