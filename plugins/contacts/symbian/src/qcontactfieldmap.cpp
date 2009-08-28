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

#include "qcontactfieldmap.h"
#include "qfieldmapping.h"

#include <qtcontacts.h>

/*!
 * Construct a field map object. The mappings for Symbian to Qt fields
 * are initialized upon construction.
 */
QContactFieldMap::QContactFieldMap()
{
	//TODO: Need error handling if d is not initialized.
	
	d = new QContactFieldMapData;
	initializeMaps();
}

/*!
 * Creates a QMap to translate Symbian <-> Qt contact fields based on the
 * array content in qfieldmapping.h
 * 
 * This is to ensure O(n log n) field translation. 
 */
void QContactFieldMap::initializeMaps()
{
	// TODO: Consider error handling options for this failing.
	ASSERT(d);
	
	// Assign all of the pointer translations to values in the map.
	for(int i(0); i < numFields; ++i)
	{
		d->mStringIdMap[fieldStringIds[i]] = fieldUids[i];
		d->mIntIdMap[fieldUids[i]] = fieldStringIds[i];
	}	
}

/*!
 * Get the "new style" string identifier for this UID.
 * 
 * Convert a legacy field UID to a new identifier string.
 * \param fieldUid Symbian Contacts Model UID.
 * \return String identifier for this field type. If the field type is invalid, an empty string is returned.
 */
QString QContactFieldMap::stringIdentifier(int fieldUid) const
{
	// TODO: Consider error handling options other than returning empty string
	return d->mIntIdMap[fieldUid];
}

/*!
 * Get a legacy contacts model UID for this field type.
 * 
 * Returns a new human-readable string identifier for the field
 * identified by this UID.
 * 
 * \param fieldStringIdentifier The string identifier for this field.
 * \return Legacy Symbian Contacts Model UID, or -1 if the identifier was invalid.
 */
int QContactFieldMap::intIdentifier(const QString& fieldStringIdentifier) const
{
	// TODO: -1 return code might not be acceptable. Revisit when we have an error handling strategy.
	int uid = d->mStringIdMap[fieldStringIdentifier];

	// Will be zero if not found.
	return uid;
}

/*!
 * Join two strings to match the sorted fromat in the lookup maps.
 * 
 * \param detail The detail type
 * \param field The field identifier
 * \return A string for the sorted string format for this field.
 */
QString QContactFieldMap::dotJoin(const QString &detail, const QString &field) const
{
    QString joined = detail;
    
    // Add a dot if there isn't already one there
    if (!detail.endsWith(".") && !field.startsWith(".")) {
        joined.append(".");
    }
    
    joined.append(field);
    return joined;
}

/*!
 * Convert this combination of a \ref QContactDetail type and field value to
 * a Symbian OS TUid used to identify the corresponding field type in CNTMODEL.
 * 
 * \param detailType The value of \ref QContactDetail::type()
 * \param field The field identifier to convert.
 * \return The Symbian OS UID for that field.
 */
TUid QContactFieldMap::symbianOsUid(const QString &detailType, 
        const QString &field) const
{
    QString sortKey = dotJoin(detailType, field);
    
    TUid uid;
    uid.iUid = intIdentifier(sortKey);
    return uid;
}

/*!
 * Convert this Symbian OS UID for a specific CNTMODEL field type to the
 * corresponding detail type and field key used in the Qt Mobility API.
 * 
 * \param uid The Symbian OS UID for this field type.
 * \return A pair, where the first item is the detail type and the second is
 *  the field value for a Qt Mobility \ref QContactDetail object.
 */
QPair<QString, QString> QContactFieldMap::qtDetailFieldPair(TUid uid) const
{
    QString sortKey = stringIdentifier(uid.iUid);
    QPair<QString, QString> pair;
    
    if (!sortKey.isEmpty()) {
        // Split the sort key into a pair.
        QStringList splitKey = sortKey.split(".");
        
        // At the moment, the mapping should only contain a single dot.
        // If that's not the case, panic here to remind whoever updated
        // the header file that this implementation needs to know how to 
        // deal with it!
        ASSERT(splitKey.count() == 2);
        
        pair.first = splitKey[0];
        pair.second = splitKey[1];
    }
    
    // Note that an empty pair is returned when the provided UID is not
    // listed and therefore cannot be converted.
    return pair;
}

