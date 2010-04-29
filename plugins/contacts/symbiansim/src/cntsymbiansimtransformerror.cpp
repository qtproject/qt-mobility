/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntsymbiansimtransformerror.h"
#include <GsmError.h>

/*! Transform a Symbian contact error id to QContactManager::Error.
 *
 * \param symbianError Symbian error.
 * \param QtError Qt error.
*/
void CntSymbianSimTransformError::transformError(TInt symbianError, QContactManager::Error* qtError)
{
    switch(symbianError)
    {
        case KErrNone:
        {
            *qtError = QContactManager::NoError;
            break;
        }
        case KErrNotFound:
        {
            *qtError = QContactManager::DoesNotExistError;
            break;
        }
        case KErrAlreadyExists:
        {
            *qtError = QContactManager::AlreadyExistsError;
            break;
        }
        case KErrLocked:
        {
            *qtError = QContactManager::LockedError;
            break;
        }
        case KErrAccessDenied:
        case KErrPermissionDenied:
        {
            *qtError = QContactManager::PermissionsError;
            break;
        }
        case KErrNoMemory:
        {
            *qtError = QContactManager::OutOfMemoryError;
            break;
        }
        case KErrNotSupported:
        {
            *qtError = QContactManager::NotSupportedError;
            break;
        }
        case KErrArgument:
        {
            *qtError = QContactManager::BadArgumentError;
            break;
        }
        case KErrGsmSimServSneFull:
        case KErrGsmSimServAnrFull:
        case KErrGsmSimServEmailFull:
        {
            *qtError = QContactManager::LimitReachedError;
            break;
        }
        default:
        {
            *qtError = QContactManager::UnspecifiedError;
            break;
        }
    }
}
