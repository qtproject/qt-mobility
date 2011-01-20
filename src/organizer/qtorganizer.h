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

#ifndef QTORGANIZER_H
#define QTORGANIZER_H

// this file includes all of the public header files
// provided by the Qt Organizer API

#include "qtorganizerglobal.h"                        // global exports

#include "qorganizerabstractrequest.h"                // asynchronous request
#include "qorganizeritemdetail.h"                     // organizer item detail
#include "qorganizeritemdetaildefinition.h"           // detail definition
#include "qorganizeritemdetailfielddefinition.h"      // field in a detail definition
#include "qorganizeritemfetchhint.h"                  // fetch hint class
#include "qorganizeritemfilter.h"                     // organizer item filter
#include "qorganizeritem.h"                           // organizer item
#include "qorganizeritemid.h"                         // organizer item identifier
#include "qorganizeritemobserver.h"                   // organizer item observer
#include "qorganizermanager.h"                        // manager
#include "qorganizermanagerengine.h"                  // manager backend
#include "qorganizermanagerenginefactory.h"           // manage backend instantiator
#include "qorganizerrecurrencerule.h"                 // a single recurrence rule
#include "qorganizeritemsortorder.h"                  // organizer item sorting

#include "qorganizercollection.h"                     // collection of items
#include "qorganizercollectionid.h"                   // collection identifier

#include "qorganizercollectionchangeset.h"            // engine-specific collection changeset
#include "qorganizeritemchangeset.h"                  // engine-specific item changeset
#include "qorganizercollectionengineid.h"             // engine-specific collection id
#include "qorganizeritemengineid.h"                   // engine specific item id

#include "qorganizeritems.h"                          // item derived classes
#include "qorganizeritemdetails.h"                    // detail derived classes
#include "qorganizeritemrequests.h"                   // request derived classes
#include "qorganizeritemfilters.h"                    // detail derived classes

#endif
