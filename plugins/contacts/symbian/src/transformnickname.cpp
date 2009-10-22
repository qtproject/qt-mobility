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
#include "transformnickname.h"

#include <QDebug>

QList<CContactItemField *> TransformNickname::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;
	
	//cast to name
	const QContactNickname &name(static_cast<const QContactNickname &>(detail));
	
	//Prefix
	TPtrC fieldTextPrefix(reinterpret_cast<const TUint16*>(name.nickname().utf16()));
	CContactItemField* nickname = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSecondName);
	nickname->TextStorage()->SetTextL(fieldTextPrefix);
	nickname->SetMapping(KUidContactFieldVCardMapSECONDNAME);
	fieldList.append(nickname);
	CleanupStack::Pop(nickname);
	
	return fieldList;
}	


QContactDetail *TransformNickname::transformItemField(const CContactItemField& field, const QContact &contact)
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

bool TransformNickname::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldSecondName.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformNickname::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactNickname::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> TransformNickname::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactNickname::FieldNickname)
        uids << KUidContactFieldSecondName;
    return uids;
}
