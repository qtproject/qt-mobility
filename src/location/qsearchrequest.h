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

#ifndef QLOCATION_SEARCHREQUEST_H
#define QLOCATION_SEARCHREQUEST_H

#include "qmobilityglobal.h"
#include "qlocationfilter.h"

#include <QString>
#include <QMap>

QTM_BEGIN_NAMESPACE

class QSearchRequestPrivate;
class Q_LOCATION_EXPORT QSearchRequest
{
public:
    enum ReverseGeocodeMode {
        NonReverseGeocode,
        RetrieveAddresses,
        RetrieveAreas
    };
    
public:
    QSearchRequest();
    virtual ~QSearchRequest();
    QLocationFilter locationFilter();
    QString language() const;
    void setLanguage(const QString& language);
    QString searchText() const;
    void setSearchText(const QString& search);
    QString additionalData(const QString& key) const;
    void setAdditionalData(const QString& key, const QString& data);
    void removeAdditionalData(const QString& key);
    ReverseGeocodeMode reverseGeocodeMode() const;
    void setReverseGeocodeMode(ReverseGeocodeMode mode);
    
private:
    Q_DISABLE_COPY(QSearchRequest)
    QSearchRequestPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QSearchRequest);
};

QTM_END_NAMESPACE

#endif
