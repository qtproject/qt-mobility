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


// vCard 2.1 constants.
// Property names and parameters
const char versitNameId[] = "N";
const char versitFormatedNameId[] = "FN";
const char versitEmailId[] = "EMAIL";
const char versitPhoneId[] = "TEL";
const char versitAddressId[] = "ADR";
const char versitUrlId[] = "URL";
const char versitRevId[] = "REV";
const char versitUidId[] = "UID";
const char versitOrganizationId[] = "ORG";
const char versitRevisionId[] = "REV";
const char versitBirthdayId[] = "BDAY";
const char versitNoteId[] = "NOTE";
const char versitGeoId[] = "GEO";
const char versitNicknameId[] = "NICKNAME";
const char versitPhotoId[] = "PHOTO";
const char versitTitleId[] = "TITLE";
const char versitRoleId[] = "ROLE";
const char versitLogoId[] = "LOGO";
const char versitAnniversaryId[] = "X-ANNIVERSARY";
const char versitGenderId[] = "X-GENDER";
const char versitSipId[] = "X-SIP";
const char versitSoundId[] = "SOUND";
const char versitSpouseId[] = "X-SPOUSE";
const char versitChildrenId[] = "X-CHILDREN";
const char versitNicknameXId[] = "X-NICKNAME";
const char versitAssistantId[] = "X-ASSISTANT";

// Subtypes
const char versitDomesticId[] = "DOM";
const char versitInternationalId[] = "INTL";
const char versitPostalId[] = "POSTAL";
const char versitParcelId[] = "PARCEL";
const char versitVoiceId[] = "VOICE";
const char versitCellId[] = "CELL";
const char versitModemId[] = "MODEM";
const char versitCarId[] = "CAR";
const char versitVideoId[] = "VIDEO";
const char versitFaxId[] = "FAX";
const char versitBbsId[] = "BBS";
const char versitPagerId[] = "PAGER";

// ISO-8601 basic format datetime spec
const char versitDateTimeSpecIso8601Basic[] = "yyyyMMddThhmmss";
const char versitDateSpecIso8601Basic[] = "yyyyMMdd";

// Encoding values
const char versitEncoding7Bit[] = "7BIT";
const char versitEncoding8Bit[] = "8BIT";
const char versitEncodingQuotedPrintable[] = "QUOTED-PRINTABLE";
const char versitEncodingBase64[] = "BASE64";
const char versitEncodingBinary[] = "B";

// Parameters
const char versitType[] = "TYPE";
const char versitValue[] = "VALUE";
const char versitEncoding[] = "ENCODING";

// www. exception type const
const char versitConstWWW[] = "WWW.";

//ISO 8601 Extended Format suffix
const char versitISOFormatSuffix[] = "Z";


struct versitMapping {
    const char* versitString;
    const char* contactString;
};

const versitMapping versitDetailMappings[] = {
    {"N",             QContactName::DefinitionName.str},
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

const versitMapping versitContextMappings[] = {
    {"HOME", QContactDetail::ContextHome.str},
    {"WORK", QContactDetail::ContextWork.str},
};

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

#endif // QVERSITDEFS_H
