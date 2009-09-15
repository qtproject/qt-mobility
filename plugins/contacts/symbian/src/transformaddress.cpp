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
#include "transformaddress.h"

QList<CContactItemField *> TransformAddress::transformDetailL(const QContactDetail &detail)
{
    QList<CContactItemField *> fieldList; 
    
    //cast to address
    const QContactAddress& address(static_cast<const QContactAddress&>(detail));
    
    //country
    TPtrC fieldTextCountry(reinterpret_cast<const TUint16*>(address.country().utf16()));
    CContactItemField* country = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCountry);
    country->TextStorage()->SetTextL(fieldTextCountry);
    setContextsL(address, *country);
    fieldList.append(country);
    CleanupStack::Pop(country);
    
    //post code
    TPtrC fieldTextPostCode(reinterpret_cast<const TUint16*>(address.postcode().utf16()));
    CContactItemField* postCode = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPostcode);
    postCode->TextStorage()->SetTextL(fieldTextPostCode);
    setContextsL(address, *postCode);
    fieldList.append(postCode);
    CleanupStack::Pop(postCode);
    
    //display label(street)
    TPtrC fieldTextDisplayLabel(reinterpret_cast<const TUint16*>(address.displayLabel().utf16()));
    CContactItemField* displayLabel = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAddress);
    displayLabel->TextStorage()->SetTextL(fieldTextDisplayLabel);
    setContextsL(address, *displayLabel);
    fieldList.append(displayLabel);
    CleanupStack::Pop(displayLabel);
        
    //locality(city)
    TPtrC fieldTextLocality(reinterpret_cast<const TUint16*>(address.locality().utf16()));
    CContactItemField* locality = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldLocality);
    locality->TextStorage()->SetTextL(fieldTextLocality);
    setContextsL(address, *locality);
    fieldList.append(locality);
    CleanupStack::Pop(locality);
    
    //region
    TPtrC fieldTextRegion(reinterpret_cast<const TUint16*>(address.region().utf16()));
    CContactItemField* region = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldRegion);
    region->TextStorage()->SetTextL(fieldTextRegion);
    setContextsL(address, *region);
    fieldList.append(region);
    CleanupStack::Pop(region);
    
    return fieldList;
}

QContactDetail *TransformAddress::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QContactAddress* address = new QContactAddress(contact.detail<QContactAddress>());
    
    CContactTextField* storage = field.TextStorage();
    QString addressValue = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
    
    int fieldTypeCount(field.ContentType().FieldTypeCount());
    for (int i = 0; i < fieldTypeCount; i++) {
        //country
        if (field.ContentType().FieldType(i) == KUidContactFieldCountry) {
            address->setCountry(addressValue);
        }

        //post code
        else if (field.ContentType().FieldType(i) == KUidContactFieldPostcode) {
            address->setPostcode(addressValue);
        }
        
        //display label (street)
        else if (field.ContentType().FieldType(i) == KUidContactFieldAddress) {
            address->setDisplayLabel(addressValue);
        }
        
        //locality (city)
        else if (field.ContentType().FieldType(i) == KUidContactFieldLocality) {
            address->setLocality(addressValue);
        }
        
        //region
        else if (field.ContentType().FieldType(i) == KUidContactFieldRegion) {
            address->setRegion(addressValue);
        }
    }
    
    return address;
}

bool TransformAddress::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldCountry.iUid ||
        fieldType == KUidContactFieldPostcode.iUid ||
        fieldType == KUidContactFieldAddress.iUid ||
        fieldType == KUidContactFieldLocality.iUid ||
        fieldType == KUidContactFieldRegion.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformAddress::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAddress::DefinitionName) {
        ret = true;
    }
    return ret;
}
