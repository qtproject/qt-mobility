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

#ifndef QCONTACTGROUPSAVEREQUEST_H
#define QCONTACTGROUPSAVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"
#include "qcontactgroup.h"

#include <QList>

class QContactGroupSaveRequestPrivate;
class QTCONTACTS_EXPORT QContactGroupSaveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactGroupSaveRequest();
    ~QContactGroupSaveRequest();

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::GroupSave;}

    /* Selection */
    void setGroups(const QList<QContactGroup>& groups);

    /* Results */
    QList<QContactGroup> groups() const;

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

signals:
    void progress(QContactGroupSaveRequest* self);

private:
    Q_DISABLE_COPY(QContactGroupSaveRequest);
    friend class QContactManagerEngine;
    Q_DECLARE_CONTACTREQUEST_PRIVATE(QContactGroupSaveRequest)
};

#endif
