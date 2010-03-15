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
    if(detail.definitionName() != QContactAddress::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to address
    const QContactAddress& address(static_cast<const QContactAddress&>(detail));

    // create new fields with contexts
    transformToTextFieldL(address, fieldList, address.country(), KUidContactFieldCountry, KUidContactFieldVCardMapCOUNTRY, true);
    transformToTextFieldL(address, fieldList, address.postcode(), KUidContactFieldPostcode, KUidContactFieldVCardMapPOSTCODE, true);
    transformToTextFieldL(address, fieldList, address.street(), KUidContactFieldAddress, KUidContactFieldVCardMapADR, true);
    transformToTextFieldL(address, fieldList, address.locality(), KUidContactFieldLocality, KUidContactFieldVCardMapLOCALITY, true);
    transformToTextFieldL(address, fieldList, address.region(), KUidContactFieldRegion, KUidContactFieldVCardMapREGION, true);
    transformToTextFieldL(address, fieldList, address.postOfficeBox(), KUidContactFieldPostOffice, KUidContactFieldVCardMapPOSTOFFICE, true);

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
    foreach(const QContactAddress& existingAddress, contact.details<QContactAddress>() )
    {
        // Do not merge if contexts don't match
        if( existingAddress.contexts() != address.contexts() )
            continue;

        // Merge detail with existing detail
        detail = new QContactAddress( existingAddress );
        foreach(const QString key, address.variantValues().keys() )
            detail->setValue( key, address.variantValue(key) );
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
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformAddress::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactAddress::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactAddress::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        
        // Don't support "ContextOther"
        QContactDetailFieldDefinition f;
        f.setDataType(QVariant::StringList);
        f.setAllowableValues(QVariantList() 
            << QLatin1String(QContactDetail::ContextHome) 
            << QLatin1String(QContactDetail::ContextWork));
        fields[QContactDetail::FieldContext] = f;

        // Sub-types not supported in symbian back-end, remove
        fields.remove(QContactAddress::FieldSubTypes);
        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
