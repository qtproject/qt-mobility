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
#include "cnttransformonlineaccount.h"
#include "cntmodelextuids.h"

QList<CContactItemField *> CntTransformOnlineAccount::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to phonenumber
	const QContactOnlineAccount &onlineAccount(static_cast<const QContactOnlineAccount&>(detail));

	//get the subType
	QStringList subTypes = onlineAccount.subTypes();

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(onlineAccount.accountUri().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText);
	newField->TextStorage()->SetTextL(fieldText);

	//no subtype
	if(!subTypes.count())
	{
        User::LeaveIfError(KErrArgument);
	}

	// online account
	else if (subTypes.contains(QContactOnlineAccount::SubTypeXmpp))
	{
        newField->AddFieldTypeL(KUidContactFieldIMPP);
        newField->SetMapping(KUidContactFieldVCardMapUnknown);
	}

	//internet
	else if (subTypes.contains(QContactOnlineAccount::SubTypeInternet))
	{
		newField->AddFieldTypeL(KUidContactFieldSIPID);
		newField->SetMapping(KUidContactFieldVCardMapSIPID);
		newField->AddFieldTypeL(KUidContactFieldVCardMapVOIP);
	}

	//share video
	else if	(subTypes.contains(QContactOnlineAccount::SubTypeShareVideo))
	{
		newField->AddFieldTypeL(KUidContactFieldSIPID);
		newField->SetMapping(KUidContactFieldVCardMapSIPID);
		newField->AddFieldTypeL(KUidContactFieldVCardMapSWIS);
	}

	//sip
	else if	(subTypes.contains(QContactOnlineAccount::SubTypeSip))
	{
		newField->AddFieldTypeL(KUidContactFieldSIPID);
		newField->SetMapping(KUidContactFieldVCardMapSIPID);
		newField->AddFieldTypeL(KUidContactFieldVCardMapSIPID);
	}

	else
	{
        User::LeaveIfError(KErrNotSupported);
	}

	//contexts
	setContextsL(onlineAccount, *newField);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformOnlineAccount::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();

	CContactTextField* storage = field.TextStorage();
	QString onlineAccountString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	onlineAccount->setAccountUri(onlineAccountString);

    if (field.ContentType().ContainsFieldType(KUidContactFieldIMPP)) {
        onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeXmpp);
    }
	else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapVOIP)) {
        onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeInternet);
    }
    else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapSWIS)) {
        onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeShareVideo);
    }
    else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapSIPID)) {
        onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSip);
    }

    // set context
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        setContexts(field.ContentType().FieldType(i), *onlineAccount);
	}

	return onlineAccount;
}

bool CntTransformOnlineAccount::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldSIPID.iUid ||
        fieldType == KUidContactFieldIMPP.iUid )
    {
        ret = true;
    }
    return ret;
}

bool CntTransformOnlineAccount::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactOnlineAccount::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformOnlineAccount::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactOnlineAccount::FieldAccountUri) {
        uids << KUidContactFieldIMPP;
        uids << KUidContactFieldSIPID;
    }
    return uids;
}


/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformOnlineAccount::supportsSubType(const QString& subType) const
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
quint32 CntTransformOnlineAccount::getIdForField(const QString& fieldName) const
{
    if (QContactOnlineAccount::FieldAccountUri  == fieldName)
        return 0;
    else if (QContactOnlineAccount::SubTypeSip == fieldName)
        return KUidContactFieldSIPID.iUid;
    else if (QContactOnlineAccount::SubTypeH323 == fieldName)
        return 0;
    else if (QContactOnlineAccount::SubTypeXmpp == fieldName)
        return KUidContactFieldIMPP.iUid;
    else if (QContactOnlineAccount::SubTypeInternet == fieldName)
        return 0;
    else if (QContactOnlineAccount::SubTypeShareVideo == fieldName)
        return 0;
    else 
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformOnlineAccount::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;
    QVariantList subTypes;

    // fields
    d.setName(QContactOnlineAccount::DefinitionName);
    f.dataType = QVariant::String;
    fields.insert(QContactOnlineAccount::FieldAccountUri, f);

    // sub-types
    f.dataType = QVariant::StringList; // can implement multiple subtypes
    subTypes << QString(QLatin1String(QContactOnlineAccount::SubTypeXmpp));
    subTypes << QString(QLatin1String(QContactOnlineAccount::SubTypeInternet));
    subTypes << QString(QLatin1String(QContactOnlineAccount::SubTypeShareVideo));
    subTypes << QString(QLatin1String(QContactOnlineAccount::SubTypeSip));
    f.allowableValues = subTypes;
    fields.insert(QContactPhoneNumber::FieldSubTypes, f);

    // Contexts
    /* TODO: does not work for some reason:
tst_QContactManager::add(mgr='symbian') A contact had extra detail: "OnlineAccount" QMap(("AccountUri", QVariant(QString, "AccountUri") ) ( "Context" ,  QVariant(QStringList, ("Xmpp") ) ) ( "SubTypes" ,  QVariant(QStringList, ("Xmpp") ) ) )
tst_QContactManager::add(mgr='symbian') B contact had extra detail: "OnlineAccount" QMap(("AccountUri", QVariant(QString, "AccountUri") ) ( "SubTypes" ,  QVariant(QStringList, ("Xmpp") ) ) )

    f.dataType = QVariant::StringList;
    f.allowableValues << QString(QLatin1String(QContactDetail::ContextHome)) << QString(QLatin1String(QContactDetail::ContextWork)) << QString(QLatin1String(QContactDetail::ContextOther));
    fields.insert(QContactDetail::FieldContext, f);
     */

    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
