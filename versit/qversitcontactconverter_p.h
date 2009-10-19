/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVERSITCONTACTCONVERTER_P_H
#define QVERSITCONTACTCONVERTER_P_H

#include <QObject>
#include <QHash>
#include <qcontact.h>
#include <qversitcontactconverter.h>
#include <qversitdocument.h>
#include <qcontactname.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontacturl.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
#include <qcontactorganization.h>
#include <qcontactbirthday.h>
#include "qversitdefs.h"


class QVersitContactConverter;

class QVersitContactConverterPrivate
{
public:    
    QVersitContactConverterPrivate() 
        : mError(QVersitContactConverter::NoError)
    {                
         //Fields Mapping

        mMappingTable.insert(QContactName::DefinitionName,QString::fromAscii(versitNameId));
        mMappingTable.insert(QContactEmailAddress::DefinitionName,
                             QString::fromAscii(versitEmailId));
        mMappingTable.insert(QContactPhoneNumber::DefinitionName,
                             QString::fromAscii(versitPhoneId));
        mMappingTable.insert(QContactAddress::DefinitionName,QString::fromAscii(versitAddressId));

        mMappingTable.insert(QContactUrl::DefinitionName,QString::fromAscii(versitUrlId));
        mMappingTable.insert(QContactGuid::DefinitionName,QString::fromAscii(versitUidId));
        mMappingTable.insert(QContactOrganization::DefinitionName,
                             QString::fromAscii(versitOrganizationId));

        mMappingTable.insert(QContactTimestamp::DefinitionName,QString::fromAscii(versitRevId));
        mMappingTable.insert(QContactBirthday::DefinitionName,QString::fromAscii(versitBirthdayId));

        //Context Mapping
        mMappingTable.insert(QContactDetail::ContextWork,QString::fromAscii(versitContextWorkId));
        mMappingTable.insert(QContactDetail::ContextHome,QString::fromAscii(versitContextHomeId));

        // Sub Types parameters.
        /* Sub types for the Address */
        mMappingTable.insert(QContactAddress::SubTypeDomestic,
                             QString::fromAscii(versitDomesticId));
        mMappingTable.insert(QContactAddress::SubTypeInternational,
                             QString::fromAscii(versitInternationalId));
        mMappingTable.insert(QContactAddress::SubTypePostal,QString::fromAscii(versitPostalId));
        mMappingTable.insert(QContactAddress::SubTypeParcel,QString::fromAscii(versitParcelId));

        /* Sub types for the telephone */
        mMappingTable.insert(QContactPhoneNumber::SubTypeVoice,QString::fromAscii(versitVoiceId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeMobile,QString::fromAscii(versitCellId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeModem,QString::fromAscii(versitModemId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeCar,QString::fromAscii(versitCarId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeVideo,QString::fromAscii(versitVideoId));

        /* Sub types for the url */
        mMappingTable.insert(QContactUrl::SubTypeHomePage, QString::fromAscii(versithomePageId));
        mMappingTable.insert(QContactUrl::SubTypeFavourite, QString::fromAscii(versitfavoriteIDId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeFacsimile,QString::fromAscii(versitFaxId));
        mMappingTable.insert(QContactPhoneNumber::SubTypeBulletinBoardSystem,
                             QString::fromAscii(versitBbsId));
        mMappingTable.insert(QContactPhoneNumber::SubTypePager,QString::fromAscii(versitPagerId));        
    }
    
    ~QVersitContactConverterPrivate()
    {
    }

    // Data Container
    QVersitContactConverter::Error mError;
    QHash<QString,QString> mMappingTable;
};

#endif
