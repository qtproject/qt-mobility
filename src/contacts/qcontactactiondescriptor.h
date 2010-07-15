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


#ifndef QCONTACTACTIONDESCRIPTOR_H
#define QCONTACTACTIONDESCRIPTOR_H

#include "qtcontactsglobal.h"
#include <QString>
#include <QSharedDataPointer>

QTM_BEGIN_NAMESPACE

class QContactActionDescriptorPrivate;
class Q_CONTACTS_EXPORT QContactActionDescriptor
{
public:
    explicit QContactActionDescriptor(const QString& actionName = QString(), const QString& vendorName = QString(), int vendorVersion = -1);
    QContactActionDescriptor(const QContactActionDescriptor& other);
    QContactActionDescriptor& operator=(const QContactActionDescriptor& other);
    ~QContactActionDescriptor();

    bool isEmpty() const;
    bool operator==(const QContactActionDescriptor& other) const;
    bool operator!=(const QContactActionDescriptor& other) const;
    bool operator<(const QContactActionDescriptor& other) const;

    void setActionName(const QString& actionName);
    void setVendorName(const QString& vendorName);
    void setImplementationVersion(int implementationVersion);

    QString actionName() const;
    QString vendorName() const;
    int implementationVersion() const;

private:
    QSharedDataPointer<QContactActionDescriptorPrivate> d;
};

Q_CONTACTS_EXPORT uint qHash(const QContactActionDescriptor& key);

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactActionDescriptor), Q_MOVABLE_TYPE);

#endif
