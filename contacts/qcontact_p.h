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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACT_P_H
#define QCONTACT_P_H

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
#include <QMultiHash>

#include "qtcontacts.h"

class QContactData : public QSharedData
{
public:
    QContactData()
        : QSharedData(),
        m_error(QContact::NoError),
        m_id(0),
        m_nextDetailId(1)
    {
    }

    QContactData(const QContactData& other)
        : QSharedData(other),
        m_error(other.m_error),
        m_id(other.m_id),
        m_details(other.m_details),
        m_preferences(other.m_preferences),
        m_nextDetailId(other.m_nextDetailId),
        m_groups(other.m_groups)
    {
    }

    ~QContactData() {}

    /* Helper function to set error and detach when necessary */
    static void setError(const QSharedDataPointer<QContactData>& d, QContact::Error error)
    {
        if (d.constData()->m_error != error) {
            QSharedDataPointer<QContactData>* ncd = const_cast<QSharedDataPointer<QContactData>*>(&d);
            ncd->detach();
            ncd->data()->m_error = error;
        }
    }

    QContact::Error m_error;
    QUniqueId m_id;
    QList<QContactDetail> m_details;
    QMap<QString, quint32> m_preferences;
    quint32 m_nextDetailId;
    QList<QUniqueId> m_groups;
};

#endif

