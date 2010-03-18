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


#ifndef QVERSITDEFS_P_H
#define QVERSITDEFS_P_H

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
#include <qcontacttag.h>

QTM_BEGIN_NAMESPACE

// Mapping between a string in versit specifications and Qt contact details
struct VersitContactDetailMapping {
    const char* versitPropertyName;
    const char* contactDetailDefinitionName;
    const char* contactDetailValueKey;
};

//! [Property name mappings]
// Mappings from versit property names to Qt contact details
const VersitContactDetailMapping versitContactDetailMappings[] = {
    {"ADR",           QContactAddress::DefinitionName.str,
                      ""},
    {"BDAY",          QContactBirthday::DefinitionName.str,
                      QContactBirthday::FieldBirthday.str},
    {"CATEGORIES",    QContactTag::DefinitionName.str,
                      QContactTag::FieldTag.str},
    {"FN",            QContactDisplayLabel::DefinitionName.str,
                      ""},
    {"GEO",           QContactGeoLocation::DefinitionName.str,
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
    {"X-EPOCSECONDNAME",QContactNickname::DefinitionName.str,
                      QContactNickname::FieldNickname.str},
    {"X-GENDER",      QContactGender::DefinitionName.str,
                      QContactGender::FieldGender.str},
    {"X-IMPP",        QContactOnlineAccount::DefinitionName.str,
                      QContactOnlineAccount::SubTypeImpp.str},
    {"X-JABBER",      QContactOnlineAccount::DefinitionName.str,
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
struct VersitMapping {
    const char* versitString;
    const char* contactString;
};

// Mappings from versit TYPE parameters to Qt contact detail contexts
const VersitMapping versitContextMappings[] = {
    {"HOME", QContactDetail::ContextHome.str},
    {"WORK", QContactDetail::ContextWork.str},
};

//! [Property type parameter mappings]
// Mappings from versit TYPE parameters to Qt contact detail subtypes
const VersitMapping versitSubTypeMappings[] = {
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
// Mappings from mime types to file extensions
const VersitMapping versitFileExtensionMappings[] = {
    {"application/octet-stream", "obj"},
    {"audio/x-pn-realaudio", "ra"},
    {"application/xml", "wsdl"},
    {"application/octet-stream", "dll"},
    {"image/x-cmu-raster", "ras"},
    {"application/x-pn-realaudio", "ram"},
    {"application/x-bcpio", "bcpio"},
    {"application/x-sh", "sh"},
    {"video/mpeg", "m1v"},
    {"image/x-xwindowdump", "xwd"},
    {"video/x-msvideo", "avi"},
    {"image/x-ms-bmp", "bmp"},
    {"application/x-shar", "shar"},
    {"application/x-javascript", "js"},
    {"application/x-wais-source", "src"},
    {"application/x-dvi", "dvi"},
    {"audio/x-aiff", "aif"},
    {"text/plain", "ksh"},
    {"application/msword", "dot"},
    {"message/rfc822", "mht"},
    {"application/x-pkcs12", "p12"},
    {"text/css", "css"},
    {"application/x-csh", "csh"},
    {"application/vnd.ms-powerpoint", "pwz"},
    {"application/pdf", "pdf"},
    {"application/x-netcdf", "cdf"},
    {"text/plain", "pl"},
    {"text/plain", "c"},
    {"image/jpeg", "jpe"},
    {"image/jpeg", "jpg"},
    {"text/x-python", "py"},
    {"text/xml", "xml"},
    {"image/jpeg", "jpeg"},
    {"application/postscript", "ps"},
    {"application/x-gtar", "gtar"},
    {"image/x-xpixmap", "xpm"},
    {"application/x-hdf", "hdf"},
    {"message/rfc822", "nws"},
    {"text/tab-separated-values", "tsv"},
    {"application/xml", "xpdl"},
    {"application/pkcs7-mime", "p7c"},
    {"application/postscript", "eps"},
    {"image/ief", "ief"},
    {"application/octet-stream", "so"},
    {"application/vnd.ms-excel", "xlb"},
    {"image/x-portable-bitmap", "pbm"},
    {"application/x-texinfo", "texinfo"},
    {"application/vnd.ms-excel", "xls"},
    {"application/x-tex", "tex"},
    {"text/richtext", "rtx"},
    {"text/html", "html"},
    {"audio/x-aiff", "aiff"},
    {"audio/x-aiff", "aifc"},
    {"application/octet-stream", "exe"},
    {"text/x-sgml", "sgm"},
    {"image/tiff", "tif"},
    {"video/mpeg", "mpeg"},
    {"application/x-ustar", "ustar"},
    {"image/gif", "gif"},
    {"application/vnd.ms-powerpoint", "ppt"},
    {"application/vnd.ms-powerpoint", "pps"},
    {"text/x-sgml", "sgml"},
    {"image/x-portable-pixmap", "ppm"},
    {"application/x-latex", "latex"},
    {"text/plain", "bat"},
    {"video/quicktime", "mov"},
    {"application/vnd.ms-powerpoint", "ppa"},
    {"application/x-troff", "tr"},
    {"application/xml", "rdf"},
    {"application/xml", "xsl"},
    {"message/rfc822", "eml"},
    {"application/x-netcdf", "nc"},
    {"application/x-sv4cpio", "sv4cpio"},
    {"application/octet-stream", "bin"},
    {"text/plain", "h"},
    {"application/x-tcl", "tcl"},
    {"application/msword", "wiz"},
    {"application/octet-stream", "o"},
    {"application/octet-stream", "a"},
    {"application/postscript", "ai"},
    {"audio/x-wav", "wav"},
    {"text/x-vcard", "vcf"},
    {"image/x-xbitmap", "xbm"},
    {"text/plain", "txt"},
    {"audio/basic", "au"},
    {"application/x-troff", "t"},
    {"image/tiff", "tiff"},
    {"application/x-texinfo", "texi"},
    {"application/oda", "oda"},
    {"application/x-troff-ms", "ms"},
    {"image/x-rgb", "rgb"},
    {"application/x-troff-me", "me"},
    {"application/x-sv4crc", "sv4crc"},
    {"video/quicktime", "qt"},
    {"video/mpeg", "mpa"},
    {"video/mpeg", "mpg"},
    {"video/mpeg", "mpe"},
    {"application/msword", "doc"},
    {"image/x-portable-graymap", "pgm"},
    {"application/vnd.ms-powerpoint", "pot"},
    {"application/x-mif", "mif"},
    {"application/x-troff", "roff"},
    {"text/html", "htm"},
    {"application/x-troff-man", "man"},
    {"text/x-setext", "etx"},
    {"application/zip", "zip"},
    {"video/x-sgi-movie", "movie"},
    {"application/x-python-code", "pyc"},
    {"image/png", "png"},
    {"application/x-pkcs12", "pfx"},
    {"message/rfc822", "mhtml"},
    {"application/x-tar", "tar"},
    {"image/x-portable-anymap", "pnm"},
    {"application/x-python-code", "pyo"},
    {"audio/basic", "snd"},
    {"application/x-cpio", "cpio"},
    {"application/x-shockwave-flash", "swf"},
    {"audio/mpeg", "mp3"},
    {"audio/mpeg", "mp2"}
};
//! [File extension mappings]

QTM_END_NAMESPACE

#endif // QVERSITDEFS_P_H
