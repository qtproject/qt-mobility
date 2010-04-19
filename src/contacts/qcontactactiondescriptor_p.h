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

#ifndef QCONTACTACTIONDESCRIPTOR_P_H
#define QCONTACTACTIONDESCRIPTOR_P_H

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
#include <QString>

QTM_BEGIN_NAMESPACE

class QContactActionDescriptorPrivate : public QSharedData
{
public:
    QContactActionDescriptorPrivate(const QString& action, const QString& vendor, int version)
            : QSharedData(),
            m_actionName(action),
            m_vendorName(vendor),
            m_implementationVersion(version)
    {
    }

    ~QContactActionDescriptorPrivate()
    {
    }

    /*
    bool operator <(const QContactActionDescriptorPrivate& other) const
    {
        if (m_actionName < other.m_actionName)
            return true;
        if (m_actionName == other.m_actionName) {
            if (m_vendorName < other.m_vendorName)
                return true;
            else if (m_vendorName == other.m_vendorName) {
                if (m_implementationVersion < other.m_implementationVersion)
                    return true;
                else if (m_implementationVersion == other.m_implementationVersion)
                    return this < &other; // equality, try to be stable
            }
        }
        return false;
    }
    */

    QString m_actionName;
    QString m_vendorName;
    int m_implementationVersion;
};

QTM_END_NAMESPACE

#endif
