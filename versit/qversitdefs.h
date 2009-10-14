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



#ifndef QVERSITDEFS_H_
#define QVERSITDEFS_H_


//vCard 2.1 constants.

// Type vaues
const char versitNameId[] = "N";
const char versitFormatedNameId[] = "FN";
const char versitEmailId[] = "EMAIL";
const char versitPhoneId[] = "TEL";
const char versitAddressId[] = "ADDR";
const char versitUrlId[] = "URL";
const char versitType[] = "TYPE";

//Contexts:
const char versitContextWorkId[] = "WORK";
const char versitContextHomeId[] = "HOME";

//SubTypes for Addresses
const char versitDomesticId[] = "DOM";
const char versitInternationalId[] = "INTL";
const char versitPostalId[] = "POSTAL";
const char versitParcelId[] = "PARCEL";

//SubTypes for Telephone
const char versitVoiceId[] = "VOICE";
const char versitCellId[] = "CELL";
const char versitModemId[] = "MODEM";
const char versitCarId[] = "CAR";
const char versitVideoId[] = "VIDEO";

#endif /* QVERSITDEFS_H_ */
