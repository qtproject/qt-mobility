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
#include "cnttransformaddress.h"

QList<CContactItemField *> CntTransformAddress::transformDetailL(const QContactDetail &detail)
{
    QList<CContactItemField *> fieldList;

    //cast to address
    const QContactAddress& address(static_cast<const QContactAddress&>(detail));

    //country
    TPtrC fieldTextCountry(reinterpret_cast<const TUint16*>(address.country().utf16()));
    CContactItemField* country = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCountry);
    country->TextStorage()->SetTextL(fieldTextCountry);
    country->SetMapping(KUidContactFieldVCardMapCOUNTRY);
    setContextsL(address, *country);
    fieldList.append(country);
    CleanupStack::Pop(country);

    //post code
    TPtrC fieldTextPostCode(reinterpret_cast<const TUint16*>(address.postcode().utf16()));
    CContactItemField* postCode = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPostcode);
    postCode->TextStorage()->SetTextL(fieldTextPostCode);
    postCode->SetMapping(KUidContactFieldVCardMapPOSTCODE);
    setContextsL(address, *postCode);
    fieldList.append(postCode);
    CleanupStack::Pop(postCode);

    //street
    TPtrC fieldTextStreet(reinterpret_cast<const TUint16*>(address.street().utf16()));
    CContactItemField* street = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAddress);
    street->TextStorage()->SetTextL(fieldTextStreet);
    street->SetMapping(KUidContactFieldVCardMapADR);
    setContextsL(address, *street);
    fieldList.append(street);
    CleanupStack::Pop(street);

    //locality(city)
    TPtrC fieldTextLocality(reinterpret_cast<const TUint16*>(address.locality().utf16()));
    CContactItemField* locality = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldLocality);
    locality->TextStorage()->SetTextL(fieldTextLocality);
    locality->SetMapping(KUidContactFieldVCardMapLOCALITY);
    setContextsL(address, *locality);
    fieldList.append(locality);
    CleanupStack::Pop(locality);

    //region
    TPtrC fieldTextRegion(reinterpret_cast<const TUint16*>(address.region().utf16()));
    CContactItemField* region = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldRegion);
    region->TextStorage()->SetTextL(fieldTextRegion);
    region->SetMapping(KUidContactFieldVCardMapREGION);
    setContextsL(address, *region);
    fieldList.append(region);
    CleanupStack::Pop(region);

    //post office box
    TPtrC fieldTextPostOfficeBox(reinterpret_cast<const TUint16*>(address.postOfficeBox().utf16()));
    CContactItemField* postOfficeBox = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPostOffice);
    postOfficeBox->TextStorage()->SetTextL(fieldTextPostOfficeBox);
    postOfficeBox->SetMapping(KUidContactFieldVCardMapPOSTOFFICE);
    setContextsL(address, *postOfficeBox);
    fieldList.append(postOfficeBox);
    CleanupStack::Pop(postOfficeBox);

    return fieldList;
}

QContactDetail *CntTransformAddress::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QContactAddress address;

    CContactTextField* storage = field.TextStorage();
    QString addressValue = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

    int fieldTypeCount(field.ContentType().FieldTypeCount());
    for (int i = 0; i < fieldTypeCount; i++) {
        //country
        if (field.ContentType().FieldType(i) == KUidContactFieldCountry) {
            address.setCountry(addressValue);
        }

        //post code
        else if (field.ContentType().FieldType(i) == KUidContactFieldPostcode) {
            address.setPostcode(addressValue);
        }

        //street
        else if (field.ContentType().FieldType(i) == KUidContactFieldAddress) {
            address.setStreet(addressValue);
        }

        //locality (city)
        else if (field.ContentType().FieldType(i) == KUidContactFieldLocality) {
            address.setLocality(addressValue);
        }

        //region
        else if (field.ContentType().FieldType(i) == KUidContactFieldRegion) {
            address.setRegion(addressValue);
        }

        //post office box
        else if (field.ContentType().FieldType(i) == KUidContactFieldPostOffice) {
            address.setPostOfficeBox(addressValue);
        }
        else {
            setContexts(field.ContentType().FieldType(i), address);
        }
    }

    // Find existing address details from contact
    QContactDetail* detail = 0;
    foreach( QContactAddress existingAddress, contact.details<QContactAddress>() )
    {
        // Do not merge if contexts don't match
        if( existingAddress.contexts() != address.contexts() )
            continue;

        // Merge detail with existing detail
        detail = new QContactAddress( existingAddress );
        foreach( QString key, address.values().keys() )
            detail->setValue( key, address.value(key) );
        break;
    }

    // Create a new address detail if not merging
    if( !detail )
        detail = new QContactAddress(address);

    return detail;
}

bool CntTransformAddress::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldCountry.iUid ||
        fieldType == KUidContactFieldPostcode.iUid ||
        fieldType == KUidContactFieldAddress.iUid ||
        fieldType == KUidContactFieldLocality.iUid ||
        fieldType == KUidContactFieldRegion.iUid ||
        fieldType == KUidContactFieldPostOffice.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformAddress::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAddress::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformAddress::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;

    if( detailFieldName == QContactAddress::FieldStreet )
        return uids << KUidContactFieldAddress;

    if( detailFieldName == QContactAddress::FieldLocality )
        return uids << KUidContactFieldLocality;

    if( detailFieldName == QContactAddress::FieldRegion )
        return uids << KUidContactFieldRegion;

    if( detailFieldName == QContactAddress::FieldPostcode )
        return uids << KUidContactFieldPostcode;

    if( detailFieldName == QContactAddress::FieldCountry )
        return uids << KUidContactFieldCountry;

    if( detailFieldName == QContactAddress::FieldPostOfficeBox )
        return uids << KUidContactFieldPostOffice;

    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformAddress::supportsSubType(const QString& subType) const
{
    if(QContactAddress::FieldSubTypes  == subType)
       return true;
    else
       return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformAddress::getIdForField(const QString& fieldName) const
{

    if (QContactAddress::FieldStreet  == fieldName)
        return KUidContactFieldAddress.iUid;
    else if (QContactAddress::FieldLocality == fieldName)
        return KUidContactFieldLocality.iUid;
    else if (QContactAddress::FieldRegion == fieldName)
        return KUidContactFieldRegion.iUid;
    else if (QContactAddress::FieldPostcode == fieldName)
        return KUidContactFieldPostcode.iUid;
    else if (QContactAddress::FieldCountry == fieldName)
         return KUidContactFieldCountry.iUid;
    else if (QContactAddress::FieldPostOfficeBox == fieldName)
        return KUidContactFieldPostOffice.iUid;
    else if (QContactAddress::SubTypeParcel == fieldName)
        return 0;
    else if (QContactAddress::SubTypePostal == fieldName)
        return 0;
    else if (QContactAddress::SubTypeDomestic == fieldName)
        return 0;
    else if (QContactAddress::SubTypeInternational == fieldName)
        return 0;
    else
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformAddress::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinitionField> fields;
    QContactDetailDefinitionField f;
    QContactDetailDefinition d;

    // Address fields
    d.setName(QContactAddress::DefinitionName);
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactAddress::FieldPostOfficeBox, f);
    fields.insert(QContactAddress::FieldStreet, f);
    fields.insert(QContactAddress::FieldLocality, f);
    fields.insert(QContactAddress::FieldRegion, f);
    fields.insert(QContactAddress::FieldPostcode, f);
    fields.insert(QContactAddress::FieldCountry, f);

    // Contexts
    f.setDataType(QVariant::StringList);
    f.setAllowableValues(QVariantList() << QString(QLatin1String(QContactDetail::ContextHome)) << QString(QLatin1String(QContactDetail::ContextWork)) << QString(QLatin1String(QContactDetail::ContextOther)));
    fields.insert(QContactDetail::FieldContext, f);

    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    definitions.insert(d.name(), d);
}
