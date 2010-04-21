/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cnttransformcontactdata.h"

/*!
 * Generic implementation for adding a new text field with one field type for
 * the given detail value. The field type (fieldTypeUid) and vcard mapping are
 * defined by the caller.
 * \a detail The detail being transformed.
 * \a fieldList On return, contains the created contact item field.
 * \a detailValue The detail value to be transformed. If empty,
 * \a fieldTypeUid The UID of the field type.
 * \a vcardMapping The VCard mapping of the field's content type.
 * \a setContext Set to true to enable defining context for the field, set to
 * false to disable context.
 */
void CntTransformContactData::transformToTextFieldL(
        const QContactDetail &detail,
        QList<CContactItemField *> &fieldList,
        const QString &detailValue,
        const TUid fieldTypeUid,
        const TUid vcardMapping,
        const bool setContext)
{
    TPtrC value(reinterpret_cast<const TUint16*>(detailValue.utf16()));
    if(value.Length()) {
        CContactItemField* itemField = CContactItemField::NewLC(KStorageTypeText, fieldTypeUid);
        itemField->TextStorage()->SetTextL(value);
        itemField->SetMapping(vcardMapping);
        if(setContext)
            setContextsL(detail, *itemField);
        fieldList.append(itemField);
        CleanupStack::Pop(itemField);
    }
}

void CntTransformContactData::setContexts(const TUid &fieldType, QContactDetail &detail)
{
	if (fieldType == KUidContactFieldVCardMapHOME)
	{
		detail.setContexts(QContactDetail::ContextHome);
	}

	else if (fieldType == KUidContactFieldVCardMapWORK)
	{
		detail.setContexts(QContactDetail::ContextWork);
	}
}

void CntTransformContactData::setContextsL(const QContactDetail &detail, CContactItemField &field)
{
	QStringList contexts = detail.contexts();

	//only first context in the array is taken into account
	if (contexts.count() > 0) {
        if (contexts.at(0) == QContactDetail::ContextHome ) {
            field.AddFieldTypeL(KUidContactFieldVCardMapHOME);
        }
        else if (contexts.at(0) == QContactDetail::ContextWork ) {
            field.AddFieldTypeL(KUidContactFieldVCardMapWORK);
        }
	}
}
