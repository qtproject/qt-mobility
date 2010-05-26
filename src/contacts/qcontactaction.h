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


#ifndef QCONTACTACTION_H
#define QCONTACTACTION_H

#include "qtcontactsglobal.h"

#include "qcontactfilter.h"
#include "qcontactdetail.h"
#include "qcontact.h"

#include <QObject>

QTM_BEGIN_NAMESPACE

class QContactActionDescriptor;
class QContactActionData;
class Q_CONTACTS_EXPORT QContactAction : public QObject
{
    Q_OBJECT

public:
    /* return a list of names of actions which are available */
    static QStringList availableActions(const QString& serviceName = QString());

    /* return a list of descriptors for action implementations matching the given criteria */
    static QList<QContactActionDescriptor> actionDescriptors(const QString& actionName = QString());

    /* return a pointer to an implementation of the action identified by the given descriptor */
    static QContactAction* action(const QContactActionDescriptor& descriptor);

public:
    QContactAction(QObject *parent = 0) : QObject(parent) {}
    virtual ~QContactAction() = 0;

    virtual QContactActionDescriptor actionDescriptor() const = 0;          // the descriptor which uniquely identifies this action
    virtual QVariantMap metaData() const = 0;                               // label, icon etc - under discussion! - replaces the above

    virtual QContactFilter contactFilter(const QVariant& value = QVariant()) const = 0; // use for matching
    virtual bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const = 0;
    virtual QList<QContactDetail> supportedDetails(const QContact& contact) const = 0;

    /* Initiate the asynchronous action on the given contact (and optionally detail) */
    virtual bool invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap()) = 0;

    /* The possible states of an action */
    enum State {
        InactiveState = 0,      // operation not yet started
        ActiveState,            // operation started, not yet finished
        FinishedState,          // operation successfully completed
        FinishedDetachedState,  // operation started, no further information available - name under discussion.
        FinishedWithErrorState  // operation finished, but error occurred
    };

    virtual State state() const = 0;

    /* Returns the most recently received result, or an empty QVariantMap if no results received */
    virtual QVariantMap results() const = 0;

Q_SIGNALS:
    void stateChanged(QContactAction::State);
    void resultsAvailable();
};

QTM_END_NAMESPACE

#endif
