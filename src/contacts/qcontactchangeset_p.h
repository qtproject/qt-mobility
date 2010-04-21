/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTCHANGESET_P_H
#define QCONTACTCHANGESET_P_H

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

#include "qtcontactsglobal.h"
#include <QSharedData>
#include <QSet>
#include <QPair>

QTM_BEGIN_NAMESPACE

class QContactChangeSetData : public QSharedData
{
public:
    QContactChangeSetData()
        : QSharedData(),
        m_dataChanged(false)
    {
    }

    QContactChangeSetData(const QContactChangeSetData& other)
        : QSharedData(other),
        m_dataChanged(other.m_dataChanged),
        m_addedContacts(other.m_addedContacts),
        m_changedContacts(other.m_changedContacts),
        m_removedContacts(other.m_removedContacts),
        m_addedRelationships(other.m_addedRelationships),
        m_removedRelationships(other.m_removedRelationships),
        m_oldAndNewSelfContactId(other.m_oldAndNewSelfContactId)
    {
    }

    ~QContactChangeSetData()
    {
    }

    bool m_dataChanged;
    QSet<QContactLocalId> m_addedContacts;
    QSet<QContactLocalId> m_changedContacts;
    QSet<QContactLocalId> m_removedContacts;
    QSet<QContactLocalId> m_addedRelationships;
    QSet<QContactLocalId> m_removedRelationships;
    QPair<QContactLocalId, QContactLocalId> m_oldAndNewSelfContactId;
};

QTM_END_NAMESPACE

#endif
