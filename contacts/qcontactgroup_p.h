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


#ifndef QCONTACTGROUP_P_H
#define QCONTACTGROUP_P_H

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

#include <QSharedData>
#include <QList>
#include <QDateTime>

#include "qcontactgroup.h"
#include "qcontactdetail.h"

class QContactGroupData : public QSharedData
{
public:
    QContactGroupData()
        : QSharedData(),
        m_error(QContactGroup::NoError),
        m_id(0)
    {
    }

    QContactGroupData(const QContactGroupData& other)
        : QSharedData(other),
            m_error(other.m_error),
            m_id(other.m_id),
            m_members(other.m_members),
            m_name(other.m_name)
    {
    }

    ~QContactGroupData() {}

    mutable QContactGroup::Error m_error;
    QUniqueId m_id;
    QList<QUniqueId> m_members;
    QString m_name;
};

#endif
