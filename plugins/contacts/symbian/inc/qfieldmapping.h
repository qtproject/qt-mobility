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

#ifndef QFIELDMAPPING_H
#define QFIELDMAPPING_H

#include <CNTDEF.H>
#include <qtcontacts.h>

//TODO FIX THE MAPPINGS, with qmap or something else...


// These arrays must "align" so that the UIDs are at the same
// offset as the string identifiers they correspond to.
//TODO: Complete this list.
const int numFields = 5;
const int fieldUids[numFields] = 
{
		KUidContactFieldGivenName.iUid, 
		KUidContactFieldFamilyName.iUid, 
		KUidContactFieldSms.iUid,
		//KUidContactFieldFavourite.iUid, doesn't exist currently should be implemented
		KUidContactFieldEMail.iUid
};


// Note: These values must match the string identifiers for each
// specialized detail class.
// The format to identify each detail field exactly should be:
//  detail.field
const QString fieldStringIds[numFields] =
{
		QContactName::DefinitionName + "." + QContactName::FieldFirst,
		QContactName::DefinitionName + "." + QContactName::FieldLast, 
		QContactPhoneNumber::DefinitionName + "." + QContactPhoneNumber::FieldNumber,
		QContactEmailAddress::DefinitionName + "." + QContactEmailAddress::FieldEmailAddress
		
		/*QContactName::FieldFirst + ".first",
		QContactName::FieldLast + ".last",
		QContactPhoneNumber::FieldNumber + ".number",
		QContactEmailAddress::FieldEmailAddress + ".email"*/
		
		/*
        QContactName::staticType() + ".first",
        QContactName::staticType() + ".last",
        QContactPhoneNumber::staticType() + ".number",
		QContactFavourite::staticType() + ".favourite",
		QContactEmail::staticType() + ".email"
		*/
};

#endif // QFIELDMAPPING_H
