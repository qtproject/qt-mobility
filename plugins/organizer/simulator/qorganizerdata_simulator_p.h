/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QORGANIZERDATA_SIMULATOR_P_H
#define QORGANIZERDATA_SIMULATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
// needed to make imports work from Simulator and Mobility
#include "qorganizeritem.h"
#include "qorganizercollection.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizermanager.h"
#include <QtCore/QMetaType>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

namespace Simulator {

class SaveOrganizerItemReply
{
public:
    QOrganizerItem savedItem;
    QOrganizerManager::Error error;
};

class SaveOrganizerCollectionReply
{
public:
    QOrganizerCollection savedCollection;
    QOrganizerManager::Error error;
};

// wrap the ids because they are registered with QTM_PREPEND_NAMESPACE(...Id),
// which leads to a method signature moc can not cope with
class OrganizerItemId
{
public:
    QOrganizerItemId id;
};

class OrganizerCollectionId
{
public:
    QOrganizerCollectionId id;
};

} // namespace Simulator

void qt_registerOrganizerTypes();

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QtMobility::QOrganizerItem)
Q_DECLARE_METATYPE(QtMobility::QOrganizerCollection)
Q_DECLARE_METATYPE(QtMobility::QOrganizerItemDetailDefinition)
Q_DECLARE_METATYPE(QtMobility::Simulator::OrganizerItemId)
Q_DECLARE_METATYPE(QtMobility::Simulator::OrganizerCollectionId)
Q_DECLARE_METATYPE(QtMobility::Simulator::SaveOrganizerItemReply)
Q_DECLARE_METATYPE(QtMobility::Simulator::SaveOrganizerCollectionReply)

QT_END_HEADER

#endif // QORGANIZERDATA_SIMULATOR_P_H
