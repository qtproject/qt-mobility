/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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


#ifndef CNTMODELEXTUIDS_H
#define CNTMODELEXTUIDS_H

#include "cntmodelextuids.hrh"

//  CONSTANTS  

/**
 * Phonebook custom Field value.
 * Locationing privacy field type.
 */
const TUid KPbkUidContactFieldLocationPriv={KPbkUidContactFieldLocationPrivValue};

/**
 * Phonebook custom Field value.
 * Top Contact field type.
 */
const TUid KUidContactFieldTopContact={KUidContactFieldTopContactValue};

/**
 * Phonebook custom Field value.
 * IMPP field type.
 */
const TUid KUidContactFieldIMPP={KUidContactFieldIMPPValue};

/**
 * Phonebook custom Field value.
 * Video ringtone field type.
 */
const TUid KUidContactFieldVideoRingTone={KUidContactFieldVideoRingToneValue};

/**
 * Phonebook custom Field value.
 * Gender field type.
 */
const TUid KUidContactFieldGender={KUidContactFieldGenderValue};

/* This is a workaround to avoid OS version specific checks.. */
#if defined(Q_HAD_TO_DECLARE_GEO_VALUE)
const TUid KUidContactFieldGEO = {KUidContactFieldGEOValue};
#endif

#endif // CNTMODELEXTUIDS_HRH

// End of File
