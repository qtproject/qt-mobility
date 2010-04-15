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
#include "cnttransformurl.h"

QList<CContactItemField *> CntTransformUrl::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactUrl::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to url
	const QContactUrl &url(static_cast<const QContactUrl&>(detail));

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(url.url().utf16()));
	if(fieldText.Length()) {
	    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldUrl);
	    newField->TextStorage()->SetTextL(fieldText);
	    newField->SetMapping(KUidContactFieldVCardMapURL);

	    QString subType = url.subType();
	    const QString& subTypeHomePage(QContactUrl::SubTypeHomePage);
	    if (subType.length() != 0 && subType.compare(subTypeHomePage) != 0)
	    {
	        User::LeaveIfError(KErrNotSupported);
	    }

	    //contexts
	    setContextsL(url, *newField);

	    fieldList.append(newField);
	    CleanupStack::Pop(newField);
	}

	return fieldList;
}

QContactDetail *CntTransformUrl::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactUrl *url = new QContactUrl();

	CContactTextField* storage = field.TextStorage();
	QString urlString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	url->setUrl(urlString);
	url->setSubType(QContactUrl::SubTypeHomePage);

	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		setContexts(field.ContentType().FieldType(i), *url);
	}

	return url;
}

bool CntTransformUrl::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldUrl.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformUrl::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactUrl::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformUrl::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactUrl::FieldUrl)
        uids << KUidContactFieldUrl;
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformUrl::supportsSubType(const QString& subType) const
{
    if(QContactUrl::FieldSubType  == subType)
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
quint32 CntTransformUrl::getIdForField(const QString& fieldName) const
{
 if (QContactUrl::FieldUrl == fieldName)
        return 0;
    else if (QContactUrl::SubTypeHomePage == fieldName)
        return 0;
    else if (QContactUrl::SubTypeFavourite == fieldName)
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
void CntTransformUrl::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString &contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactUrl::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactUrl::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        QContactDetailFieldDefinition f;

        f.setDataType(QVariant::String); //only allowed to be a single subtype
        f.setAllowableValues(QVariantList()
                << QString(QLatin1String(QContactUrl::SubTypeHomePage)));
        fields.insert(QContactUrl::FieldSubType, f);

        // Don't support "ContextOther"
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
