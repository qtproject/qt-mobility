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
#ifdef SYMBIAN_BACKEND_USE_SQLITE

#include "cnttransformpresence.h"
#include "cntmodelextuids.h"

CntTransformPresence::CntTransformPresence() :
m_detailCounter(0)
{
}

QList<CContactItemField *> CntTransformPresence::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactPresence::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to presence
	const QContactPresence &presenceDetail(static_cast<const QContactPresence&>(detail));

    // Transform presence informaiton
	if(presenceDetail.presenceState() != QContactPresence::PresenceUnknown) {
        QString presence = QString::number(presenceDetail.presenceState());
        CContactItemField* presenceField = CContactItemField::NewLC(KStorageTypeText);
        TPtrC presenceEncodedText(reinterpret_cast<const TUint16*>(presence.utf16()));
        presenceField->TextStorage()->SetTextL(presenceEncodedText);
        presenceField->AddFieldTypeL(KUidContactFieldPresence);
        presenceField->SetUserFlags(m_detailCounter);
        fieldList.append(presenceField);
        CleanupStack::Pop(presenceField);
    }
	         
    // Transform statusMessage
    TPtrC statusMsgText(reinterpret_cast<const TUint16*>(presenceDetail.presenceStateText().utf16()));
    if(statusMsgText.Length()) {
        CContactItemField* statusMsgField = CContactItemField::NewLC(KStorageTypeText);
        statusMsgField->TextStorage()->SetTextL(statusMsgText);
        statusMsgField->AddFieldTypeL(KUidContactFieldStatusMsg);
        statusMsgField->SetUserFlags(m_detailCounter);
        fieldList.append(statusMsgField);
        CleanupStack::Pop(statusMsgField);
	}

	if(fieldList.count() > 0) {
        m_detailCounter++;
	}
	    
	return fieldList;
}

QContactDetail *CntTransformPresence::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QList<QContactDetail> presenceDetails = contact.details(QContactPresence::DefinitionName);
    //check what presence detail the provided field belongs to. if there is no such detail yet,
    //let's create it.
    if (presenceDetails.count() <= field.UserFlags()) {
        for (int i = presenceDetails.count(); i <= field.UserFlags(); i++) {
            QContactPresence *presence = new QContactPresence();
            QContact& currentContact = const_cast<QContact&>(contact);
            currentContact.saveDetail(presence);
        }
        presenceDetails = contact.details(QContactPresence::DefinitionName);
    }
    QContactPresence *presenceDetail = new QContactPresence(presenceDetails.at(field.UserFlags()));

	CContactTextField* storage = field.TextStorage();
	QString presenceString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

    for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        //Presence
        if (field.ContentType().FieldType(i) == KUidContactFieldPresence) {
            QContactPresence::PresenceState presenceState = decodePresence(presenceString);
            presenceDetail->setPresenceState(presenceState);
        }
        //Status Message
        else if (field.ContentType().FieldType(i) == KUidContactFieldStatusMsg) {
            presenceDetail->setPresenceStateText(presenceString);
        }
    }

    // set context
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        setContexts(field.ContentType().FieldType(i), *presenceDetail);
	}

	return presenceDetail;
}

QList<TUid> CntTransformPresence::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldPresence
        << KUidContactFieldStatusMsg;
}

bool CntTransformPresence::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactPresence::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformPresence::supportedSortingFieldTypes(QString /*detailFieldName*/) const
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
bool CntTransformPresence::supportsSubType(const QString& /*subType*/) const
{
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformPresence::getIdForField(const QString& fieldName) const
{
    if (QContactPresence::FieldPresenceState == fieldName)
        return KUidContactFieldPresence.iUid;
    if (QContactPresence::FieldPresenceStateText == fieldName)
        return KUidContactFieldStatusMsg.iUid;
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
void CntTransformPresence::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactPresence::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactPresence::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        QContactDetailFieldDefinition f;

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

/*!
 * Reset internal variables.
 */
void CntTransformPresence::reset()
{
    m_detailCounter = 0;
}

/*!
 * Decode the presence information.
 * \a aPresence
 */
QContactPresence::PresenceState CntTransformPresence::decodePresence(QString presence)
{
    bool ok = false;
    int presenceInt = presence.toInt(&ok);
    if (!ok){
        return QContactPresence::PresenceUnknown;
    }
    
    if (presenceInt == QContactPresence::PresenceAvailable)
        return QContactPresence::PresenceAvailable;
    else if (presenceInt == QContactPresence::PresenceHidden)
        return QContactPresence::PresenceHidden;
    else if (presenceInt == QContactPresence::PresenceBusy)
        return QContactPresence::PresenceBusy;
    else if (presenceInt == QContactPresence::PresenceAway)
        return QContactPresence::PresenceAway;
    else if (presenceInt == QContactPresence::PresenceExtendedAway)
        return QContactPresence::PresenceExtendedAway;
    else if (presenceInt == QContactPresence::PresenceOffline)
        return QContactPresence::PresenceOffline;
    else
        return QContactPresence::PresenceUnknown;
}

#endif // SYMBIAN_BACKEND_USE_SQLITE

// End of file
