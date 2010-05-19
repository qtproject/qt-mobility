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
#ifndef SYMBIAN_BACKEND_USE_SQLITE

#include "cnttransformonlineaccountsimple.h"

QList<CContactItemField *> CntTransformOnlineAccount::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactOnlineAccount::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to phonenumber
	const QContactOnlineAccount &onlineAccount(static_cast<const QContactOnlineAccount&>(detail));

	//get the subType
	QStringList subTypes = onlineAccount.subTypes();

	//no subtype
    if(!subTypes.count())
    {
        User::LeaveIfError(KErrArgument);
    }
    else if (subTypes.contains(QContactOnlineAccount::SubTypeSipVoip)) {
        //internet
        transformToTextFieldL(onlineAccount, fieldList, onlineAccount.accountUri(), KUidContactFieldSIPID, KUidContactFieldVCardMapVOIP, false);
    } else if (subTypes.contains(QContactOnlineAccount::SubTypeVideoShare)) {
        //share video
        transformToTextFieldL( onlineAccount, fieldList, onlineAccount.accountUri(), KUidContactFieldSIPID, KUidContactFieldVCardMapSWIS, false);
    } else if (subTypes.contains(QContactOnlineAccount::SubTypeSip)) {
        //sip
        transformToTextFieldL( onlineAccount, fieldList, onlineAccount.accountUri(), KUidContactFieldSIPID, KUidContactFieldVCardMapSIPID, false);
    }
    else
    {
        User::LeaveIfError(KErrNotSupported);
    }

	return fieldList;
}

QContactDetail *CntTransformOnlineAccount::transformItemField(const CContactItemField& field, const QContact &contact)
{
    Q_UNUSED(contact);

    QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();
	CContactTextField* storage = field.TextStorage();
	QString onlineAccountString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	// Adding Online Account Detail.
    for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {

        if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapVOIP)) {
            onlineAccount->setAccountUri(onlineAccountString);
            onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapSWIS)) {
            onlineAccount->setAccountUri(onlineAccountString);
            onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeVideoShare);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapSIPID)) {
            onlineAccount->setAccountUri(onlineAccountString);
            onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSip);
        }
    }

    // set context
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        setContexts(field.ContentType().FieldType(i), *onlineAccount);
	}

	return onlineAccount;
}

bool CntTransformOnlineAccount::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactOnlineAccount::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformOnlineAccount::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldSIPID;
}

QList<TUid> CntTransformOnlineAccount::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactOnlineAccount::FieldAccountUri) {
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
    if(QContactOnlineAccount::FieldSubTypes == subType) {
        return true;
    } else {
        return false;
    }
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
    else if (QContactOnlineAccount::SubTypeSipVoip == fieldName)
        return KUidContactFieldVCardMapVOIP.iUid;
    else if (QContactOnlineAccount::SubTypeVideoShare == fieldName)
        return KUidContactFieldVCardMapSWIS.iUid;
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
void CntTransformOnlineAccount::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactOnlineAccount::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactOnlineAccount::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        QContactDetailFieldDefinition f;

        // Not all fields are supported
        fields.remove(QContactOnlineAccount::FieldCapabilities);
        fields.remove(QContactOnlineAccount::FieldDetailUri);
        fields.remove(QContactOnlineAccount::FieldLinkedDetailUris);
        fields.remove(QContactOnlineAccount::FieldServiceProvider);
        fields.remove(QContactOnlineAccount::FieldContext);

        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}

#endif // SYMBIAN_BACKEND_USE_SQLITE
