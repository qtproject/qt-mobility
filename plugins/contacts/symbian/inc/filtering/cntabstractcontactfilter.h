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

#ifndef QABSTRACTCONTACTFILTER_H
#define QABSTRACTCONTACTFILTER_H

#include <QList>
#include "qtcontactsglobal.h"
#include "qcontactmanager.h"
#include "qcontactsortorder.h"
#include "qcontactfilter.h"

QTM_USE_NAMESPACE
class CntAbstractContactFilter
{
public:
    enum FilterSupport {
        /* The filter is not supported */
        NotSupported = 0,
        /* The filter is illegal. Filtering is not allowed and the operation
         * should fail immediately with an error.
         */
        IllegalFilter,
        /* The filter is supported natively */
        Supported,
        /* The filter is not fully natively supported, but for performance
         * reasons the contact filter implementation pretends supporting the
         * filter when it actually maps the filter to another, less strict
         * filter.
         * 
         * For example if the caller uses match flag QContactFilter::MatchExactly,
         * the filter actually gives the result as QContactFilter::MatchContains
         * (because of the limitations in the underlying database).
         * 
         * The result then needs to be filtered by the caller (for example by
         * using QContactManagerEngine::testFilter).
         */
        SupportedPreFilterOnly
    };
public:
    virtual QList<QContactLocalId> contacts(
            const QContactFilter &filter,
            const QList<QContactSortOrder> &sortOrders,
            bool &filterSupported,
            QContactManager::Error &error) = 0;
    virtual bool filterSupported(const QContactFilter& filter) = 0;
};

#endif /* QABSTRACTCONTACTFILTER_H */
