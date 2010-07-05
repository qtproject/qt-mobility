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

#ifndef QCONTACTSAVEREQUEST_H
#define QCONTACTSAVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontact.h"

#include <QList>

QTM_BEGIN_NAMESPACE

class QContactSaveRequestPrivate;
class Q_CONTACTS_EXPORT QContactSaveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactSaveRequest(QObject* parent = 0);

    /* Selection */
    void setContact(const QContact& contact);
    void setContacts(const QList<QContact>& contacts);

    /* Results */
    QList<QContact> contacts() const;
    QMap<int, QContactManager::Error> errorMap() const;

private:
    Q_DISABLE_COPY(QContactSaveRequest)
    friend class QContactManagerEngine;
    Q_DECLARE_PRIVATE_D(d_ptr, QContactSaveRequest)
};

QTM_END_NAMESPACE

#endif
