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

#ifndef CNTFILTERDETAILDISPLAYLABEL_H
#define CNTFILTERDETAILDISPLAYLABEL_H

#include <qtcontactsglobal.h>
#include <qcontactmanager.h>
#include <qcontactdetailfilter.h>

#include "cntabstractcontactfilter.h"

QTM_USE_NAMESPACE

class CntFilterDetailDisplayLabel 
{
public:
    CntFilterDetailDisplayLabel();
    virtual ~CntFilterDetailDisplayLabel();
    
public: 
    QString createSelectQuery(const QContactFilter& filter,
                                  const QList<QContactSortOrder>& sortOrders,
                                  QContactManager::Error* error) const;
    void createSelectQuery(const QContactFilter& detailFilter,
                                  QString& sqlQuery,
                                  QContactManager::Error* error);
private: 
    void createQuerySingleSearchValue(QString& sqlQuery, const QString &searchValue, const QStringList &columns) const;
    void createQueryMultipleSearchValues(QString& sqlQuery, const QStringList &searchValues, const QStringList &columns) const;
    QString createSubQuery(const QString &searchValue, const QString &column) const;
    QString columnName(const QPair<QLatin1String, QLatin1String> &detail) const;
 };

#endif /* CNTFILTERDETAILDISPLAYLABEL_H */
