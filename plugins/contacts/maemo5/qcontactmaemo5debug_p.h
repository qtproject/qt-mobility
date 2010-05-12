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

#ifndef QCONTACTMAEMO5DEBUG_P_H
#define QCONTACTMAEMO5DEBUG_P_H

#include <QDebug>

/* To Enable debugging of Maemo5 Contact plugin set QCONTACT_MAEMO5_DEBUG env var
 * eg: export QCONTACT_MAEMO5_DEBUG=1
 */

extern bool QCM5_DEBUG_ENABLED;

/* This Macro must be defined only one time*/
#define DEFINE_GLOBAL_DEBUG_VAR bool QCM5_DEBUG_ENABLED;

/* Define Macros for debugging */
#define QCM5_DEBUG  if (QCM5_DEBUG_ENABLED) qDebug()
#define QCM5_DEBUG_ARG(x)  if (QCM5_DEBUG_ENABLED) qDebug() << x;

/* Check env var and switch on/off debugging */
static inline void initDebugLogger(){
  QCM5_DEBUG_ENABLED = !qgetenv("QCONTACT_MAEMO5_DEBUG").isEmpty();
  QCM5_DEBUG << "Logging has been enabled";
}
#endif
