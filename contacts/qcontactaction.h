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


#ifndef QCONTACTACTION_H
#define QCONTACTACTION_H

#include "qtcontactsglobal.h"

#include "qcontactfilter.h"
#include "qcontactdetail.h"
#include "qcontact.h"

#include <QObject>

class QContactActionDescriptor;
class QContactActionData;
class QTCONTACTS_EXPORT QContactAction : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactAction() = 0;

    virtual QContactActionDescriptor actionDescriptor() const = 0;          // the descriptor which uniquely identifies this action
    virtual QVariantMap metadata() const = 0;                               // label, icon etc

    virtual QContactFilter contactFilter(const QVariant& value = QVariant()) const = 0; // use for matching
    virtual bool supportsDetail(const QContactDetail& detail) const = 0;    // whether this implementation supports the given detail
    virtual QList<QContactDetail> supportedDetails(const QContact& contact) const;
    virtual void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail()) = 0;

    /* return a list of names of actions which are available */
    static QStringList availableActions(const QString& vendor = QString(), int implementationVersion = -1);

    /* return a list of descriptors for action implementations matching the given criteria */
    static QList<QContactActionDescriptor> actionDescriptors(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1);

    /* return a pointer to an implementation of the action identified by the given descriptor */
    static QContactAction* action(const QContactActionDescriptor& descriptor);

    /* return a list of points to action implementations which match the given criteria */
    static QList<QContactAction*> actions(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1);
};

#endif
