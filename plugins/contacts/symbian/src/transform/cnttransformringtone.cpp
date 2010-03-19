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
#include "cnttransformringtone.h"
#include "cntmodelextuids.h"

#include "qcontactringtone.h"

QList<CContactItemField *> CntTransformRingtone::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactRingtone::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to ringtone
    const QContactRingtone &ringtone(static_cast<const QContactRingtone&>(detail));

	//create new field
    if (!ringtone.audioRingtone().isEmpty()) {
        TPtrC fieldText(reinterpret_cast<const TUint16*>(ringtone.audioRingtone().utf16()));

        QString subType = ringtone.subType();
        TUid uid(KUidContactFieldRingTone);
	    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, uid);

	    newField->SetMapping(KUidContactFieldVCardMapUnknown);
	    newField->TextStorage()->SetTextL(fieldText);

	    fieldList.append(newField);
	    CleanupStack::Pop(newField);
	}

    if (!ringtone.videoRingtone()) {
        TPtrC fieldText(reinterpret_cast<const TUint16*>(ringtone.audioRingtone().utf16()));

        QString subType = ringtone.subType();
        TUid uid(KUidContactFieldVideoRingTone);
        CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, uid);

        newField->SetMapping(KUidContactFieldVCardMapUnknown);
        newField->TextStorage()->SetTextL(fieldText);

        fieldList.append(newField);
        CleanupStack::Pop(newField);
    }

	return fieldList;
}

QContactDetail *CntTransformRingtone::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QContactRingtone *ringtone = new QContactRingtone(contact.detail<QContactRingtone>());

    // XXX ringtone can have multiple values from different fields glommed together
	CContactTextField* storage = field.TextStorage();
    QString ringtoneString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

    if (field.ContentType().ContainsFieldType(KUidContactFieldRingTone)) {
        ringtone->setAudioRingtone(ringtoneString);
	}
    else if (field.ContentType().ContainsFieldType(KUidContactFieldVideoRingTone)) {
        ringtone->setVideoRingtone(ringtoneString);
    }

    // XXX need to remove old one somehow

    return ringtone;
}

bool CntTransformRingtone::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldRingTone.iUid ||
        fieldType == KUidContactFieldVideoRingTone.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformRingtone::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactRingtone::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformRingtone::supportedSortingFieldTypes(QString /*detailFieldName*/) const
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
bool CntTransformRingtone::supportsSubType(const QString& subType) const
{
    // XXX todo
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformRingtone::getIdForField(const QString& fieldName) const
{
    // XXX todo
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
void CntTransformRingtone::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactRingtone::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactRingtone::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // Context not supported in symbian back-end, remove
        fields.remove(QContactRingtone::FieldContext);
        // nor vibe thingy
        fields.remove(QContactRingtone::FieldVibrationRingtone);

        d.setFields(fields);
        d.setUnique(true);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
