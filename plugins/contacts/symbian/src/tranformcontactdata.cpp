/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "transformcontactdata.h"

QList<CContactItemField *> TransformContactData::transformDetail(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	TRAPD(error, fieldList = transformDetailL(detail));
	
	return fieldList;
}

QContactDetail *TransformContactData::transformItemField(const CContactItemField& field, const QContact &contact)
{
	QContactDetail *detail(0);
	
	TRAPD(error, detail = transformItemFieldL(field, contact));
	
	return detail;
}

void TransformContactData::setContexts(const TUid &fieldType, QContactDetail &detail)
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

void TransformContactData::setContextsL(const QContactDetail &detail, CContactItemField &field)
{
	QStringList contexts = detail.contexts();
	
	for (int i = 0; contexts.count(); i++)
	{
		if (contexts.at(i) == QContactDetail::ContextHome )
		{
			field.AddFieldTypeL(KUidContactFieldVCardMapHOME);
		}
		
		else if (contexts.at(i) == QContactDetail::ContextWork )
		{
			field.AddFieldTypeL(KUidContactFieldVCardMapWORK);
		}
	}
}
