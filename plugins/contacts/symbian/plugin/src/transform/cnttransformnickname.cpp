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
#include "cnttransformnickname.h"

#include <QDebug>

QList<CContactItemField *> CntTransformNickname::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactNickname::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to name
	const QContactNickname &name(static_cast<const QContactNickname &>(detail));

    //create new field without contexts
    transformToTextFieldL(name, fieldList, name.nickname(), KUidContactFieldSecondName, KUidContactFieldVCardMapSECONDNAME, false);

	return fieldList;
}


QContactDetail *CntTransformNickname::transformItemField(const CContactItemField& field, const QContact &contact)
{
	QContactNickname *name = new QContactNickname(contact.detail<QContactNickname>());

	CContactTextField* storage = field.TextStorage();
	QString nameValue = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		//Prefix
		if (field.ContentType().FieldType(i) == KUidContactFieldSecondName)
		{
			name->setNickname(nameValue);
		}
	}

	return name;
}

bool CntTransformNickname::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactNickname::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformNickname::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldSecondName;
}

QList<TUid> CntTransformNickname::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactNickname::FieldNickname)
        uids << KUidContactFieldSecondName;
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformNickname::supportsSubType(const QString& subType) const
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
quint32 CntTransformNickname::getIdForField(const QString& fieldName) const
{
    if (QContactNickname::FieldNickname == fieldName)
       return KUidContactFieldSecondName.iUid;
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
void CntTransformNickname::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactNickname::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactNickname::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // Context not supported in symbian back-end, remove
        fields.remove(QContactNickname::FieldContext);

        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
