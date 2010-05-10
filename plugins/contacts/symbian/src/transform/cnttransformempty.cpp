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
#include "cnttransformempty.h"
#include "cntmodelextuids.h"

QList<CContactItemField *> CntTransformEmpty::transformDetailL(const QContactDetail &detail)
{
    Q_UNUSED(detail);
	QList<CContactItemField *> fieldList;
	return fieldList;
}

QContactDetail *CntTransformEmpty::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(field);
    Q_UNUSED(contact);
	return new QContactDetail();
}

bool CntTransformEmpty::supportsField(TUint32 fieldType) const
{
    Q_UNUSED(fieldType);
    return false;
}

bool CntTransformEmpty::supportsDetail(QString detailName) const
{
    Q_UNUSED(detailName);
    return false;
}

QList<TUid> CntTransformEmpty::supportedSortingFieldTypes(QString detailFieldName) const
{
    Q_UNUSED(detailFieldName);
    QList<TUid> uids;
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformEmpty::supportsSubType(const QString& subType) const
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
quint32 CntTransformEmpty::getIdForField(const QString& fieldName) const
{
    Q_UNUSED(fieldName);
    return 0;
}

/*!
 * Removes detail definitions of the details that are not supported in legacy
 * S60 platforms.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformEmpty::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactGender::DefinitionName)) {
        definitions.remove(QContactGender::DefinitionName);
    }
    if(definitions.contains(QContactGeoLocation::DefinitionName)) {
        definitions.remove(QContactGeoLocation::DefinitionName);
    }
    if(definitions.contains(QContactTag::DefinitionName)) {
        definitions.remove(QContactTag::DefinitionName);
    }
    if(definitions.contains(QContactGlobalPresence::DefinitionName)) {
        definitions.remove(QContactGlobalPresence::DefinitionName);
    }
    if(definitions.contains(QContactPresence::DefinitionName)) {
        definitions.remove(QContactPresence::DefinitionName);
    }
}
