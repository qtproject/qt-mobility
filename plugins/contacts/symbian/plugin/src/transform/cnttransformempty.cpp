/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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

bool CntTransformEmpty::supportsDetail(QString detailName) const
{
    Q_UNUSED(detailName);
    return false;
}

QList<TUid> CntTransformEmpty::supportedFields() const
{
    return QList<TUid>();
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
