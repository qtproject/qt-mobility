/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
const TUid KPbkUidContactFieldLocationPriv={KPbkUidContactFieldLocationPrivValue};
const TUid KUidContactFieldTopContact={KUidContactFieldTopContactValue};
const TUid KUidContactFieldIMPP={KUidContactFieldIMPPValue};
const TUid KUidContactFieldServiceProvider = {KUidContactFieldServiceProviderValue};
const TUid KUidContactFieldPresence = {KUidContactFieldPresenceValue};
const TUid KUidContactFieldStatusMsg = {KUidContactFieldStatusMsgValue};
const TUid KUidContactFieldVideoRingTone={KUidContactFieldVideoRingToneValue};
const TUid KUidContactFieldGender={KUidContactFieldGenderValue};
const TUid KUidContactFieldAnniversaryEvent={KUidContactFieldAnniversaryEventValue};

/* This is a workaround to avoid OS version specific checks.. */
#if defined(Q_HAD_TO_DECLARE_GEO_VALUE)
const TUid KUidContactFieldGEO = {KUidContactFieldGEOValue};
#endif

#endif // CNTMODELEXTUIDS_HRH

// End of File
