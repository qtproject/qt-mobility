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
#include "cnttransformgeolocation.h"
#include "cntmodelextuids.h"

const char separator = ',';

QList<CContactItemField *> CntTransformGeolocation::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactGeoLocation::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to geolocation
	const QContactGeoLocation &geolocation(static_cast<const QContactGeoLocation&>(detail));

	//create new field
	QString formattedGeolocation;
	if (geolocation.latitude() >= 0.0) {
        formattedGeolocation.append(QString::number(geolocation.latitude()));
	}
	formattedGeolocation.append(separator);
    if (geolocation.longitude() >= 0.0) {
         formattedGeolocation.append(QString::number(geolocation.longitude()));
    }

    if (formattedGeolocation.length() > 1) {
        //create new field with contexts
        transformToTextFieldL(geolocation, fieldList, formattedGeolocation, KUidContactFieldGEO, KUidContactFieldVCardMapGEO, true);
    }

	return fieldList;
}

QContactDetail *CntTransformGeolocation::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactGeoLocation *geolocation = new QContactGeoLocation();

	CContactTextField* storage = field.TextStorage();
	QString unformattedGeolocation = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	int separatorPos = unformattedGeolocation.indexOf(separator);

	// parse latitude
	bool latitudeSet = false;
	if (separatorPos > 0) {
        bool ok = false;
        double latitude = unformattedGeolocation.left(separatorPos).toDouble(&ok);
        if (ok) {
            geolocation->setLatitude(latitude);
            latitudeSet = true;
        }
	}
	if (!latitudeSet) {
        geolocation->setLatitude(-1);
	}

	// parse longitude
	bool longitudeSet = false;
	if (separatorPos >= 0 && separatorPos != unformattedGeolocation.length()-1) {
        bool ok = false;
        double longitude = unformattedGeolocation.right(unformattedGeolocation.length()-separatorPos-1).toDouble(&ok);
        if (ok) {
            geolocation->setLongitude(longitude);
            longitudeSet = true;
        }
	}
	if (!longitudeSet) {
        geolocation->setLongitude(-1);
	}

    // set context
    for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        setContexts(field.ContentType().FieldType(i), *geolocation);
    }

	return geolocation;
}

bool CntTransformGeolocation::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactGeoLocation::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformGeolocation::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldGEO;
}

QList<TUid> CntTransformGeolocation::supportedSortingFieldTypes(QString /*detailFieldName*/) const
{
    // Sorting not supported
    return QList<TUid>();
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformGeolocation::supportsSubType(const QString& subType) const
{
		Q_UNUSED(subType);
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformGeolocation::getIdForField(const QString& fieldName) const
{
 if (QContactGeoLocation::FieldLabel  == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldLatitude == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldLongitude == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldAccuracy == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldAltitude == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldAltitudeAccuracy == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldHeading == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldSpeed == fieldName)
        return 0;
    else if (QContactGeoLocation::FieldTimestamp == fieldName)
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
void CntTransformGeolocation::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);
    
    if(definitions.contains(QContactGeoLocation::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactGeoLocation::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        
        // Don't support "ContextOther"
        QContactDetailFieldDefinition f;
        f.setDataType(QVariant::StringList);
        f.setAllowableValues(QVariantList() 
            << QLatin1String(QContactDetail::ContextHome) 
            << QLatin1String(QContactDetail::ContextWork));
        fields[QContactDetail::FieldContext] = f;
        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
