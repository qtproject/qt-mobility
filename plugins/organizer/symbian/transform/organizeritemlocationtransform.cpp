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
#include "organizeritemlocationtransform.h"
#include "qorganizeritemlocation.h"

void OrganizerItemLocationTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    // Create a detail definition
    QOrganizerItemDetailDefinition d;
    d.setName(QOrganizerItemLocation::DefinitionName);
    QOrganizerItemDetailFieldDefinition f;
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
#ifdef SYMBIAN_CALENDAR_V2
    fields.insert(QOrganizerItemLocation::FieldGeoLocation, f);
#endif
    fields.insert(QOrganizerItemLocation::FieldLocationName, f);
    d.setFields(fields);
    d.setUnique(true);

    // Replace default detail definition with our own
    foreach (QString itemTypeName, schemaDefs.keys()) {
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        if (details.contains(d.name()))
            schemaDefs[itemTypeName].insert(d.name(), d);
    }    
}

void OrganizerItemLocationTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
	QString locationName = toQString(entry.LocationL());
	QOrganizerItemLocation location;
	if (!locationName.isEmpty()) {
		location.setLocationName(locationName);
		item->saveDetail(&location);
	}	
#ifdef SYMBIAN_CALENDAR_V2
	QString latLongStr;
	double latitude;
	double longitude;
	CCalGeoValue *geoValue = entry.GeoValueL();
	// Get the latitude and longitude values and store it in a string
	// with default precision 6.
	if (geoValue) {
		bool latLongValueSet = false;
		latLongValueSet = geoValue->GetLatLong(latitude, longitude);
		if(latLongValueSet) {
			latLongStr = QString("%1")
												.arg(QString::number(latitude, 'f'));
			latLongStr.append(";");
			latLongStr.append(QString("%1")
			                  .arg(QString::number(longitude, 'f')));
		}
		location.setGeoLocation(latLongStr);
		item->saveDetail(&location);
	}
#endif
}

void OrganizerItemLocationTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    QOrganizerItemLocation loc = item.detail<QOrganizerItemLocation>();
    if (!loc.isEmpty()) {
        // NOTE: what about geoLocation & address?
        entry->SetLocationL(toPtrC16(loc.locationName()));
    }
#ifdef SYMBIAN_CALENDAR_V2
    QString latLongString = loc.geoLocation();
    if(!latLongString.isEmpty()) 
    {
		int separatorPosition = latLongString.indexOf(";");
		QString latString(latLongString.left(separatorPosition));
		QString longString(latLongString.right(latLongString.length() - separatorPosition -1));
		bool ok = false;
		double latitude = latString.toDouble(&ok);
		if(ok) {
			double longitude = longString.toDouble(&ok);
			if(ok) {
				CCalGeoValue* geoValue = CCalGeoValue::NewL();
				CleanupStack::PushL(geoValue);
				geoValue->SetLatLongL(latitude, longitude);
				entry->SetGeoValueL(*geoValue);
				CleanupStack::PopAndDestroy(geoValue);
			}
		}
	}
#else
    if(!loc.geoLocation().isEmpty()) {
    	// We do not support saving of geolocation.
    	User::Leave(KErrNotSupported);
    }
#endif
}

QString OrganizerItemLocationTransform::detailDefinitionName()
{
    return QOrganizerItemLocation::DefinitionName;    
}
