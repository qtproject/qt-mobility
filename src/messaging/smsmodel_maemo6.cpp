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

#include <CommHistory/eventmodel_p.h>
#include <CommHistory/trackerio.h>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmo.h>
#include "smsmodel_maemo6_p.h"

using namespace RTComCommHistory;

class SMSModelPrivate : public EventModelPrivate {
public:
    Q_DECLARE_PUBLIC(SMSModel);

    SMSModelPrivate(EventModel *model)
        : EventModelPrivate(model) {
    }

    bool acceptsEvent(const Event &event) const
    {
        qDebug() << __PRETTY_FUNCTION__ << event.id();

        if (event.type() == Event::SMSEvent)
            return true;

        return false;
    }
};

SMSModel::SMSModel(QObject *parent)
        : EventModel(*new SMSModelPrivate(this), parent)
{
    connect(d_ptr, SIGNAL(eventsAdded(const QList<CommHistory::Event> &)),
	    this, SIGNAL(eventsAdded(const QList<CommHistory::Event> &)));
    connect(d_ptr, SIGNAL(eventsUpdated(const QList<CommHistory::Event> &)),
	    this, SIGNAL(eventsUpdated(const QList<CommHistory::Event> &)));
    connect(d_ptr, SIGNAL(eventDeleted(int)), this, SIGNAL(eventDeleted(int)));
}

SMSModel::~SMSModel()
{
}

bool SMSModel::getEvents()
{
    Q_D(SMSModel);

    reset();
    d->clearEvents();

    RDFSelect query;

    RDFVariable message = RDFVariable::fromType<nmo::SMSMessage>();

    // Do not delete these commented lines
    //message.property<nmo::isSent>(LiteralValue(false));
    //message.property<nmo::isDraft>(LiteralValue(false));
    //message.property<nmo::isDeleted>(LiteralValue(false));

    query.addColumn("message", message);
    RDFVariable date = message.property<nmo::receivedDate>();

    query.addColumn("date", date);
    query.orderBy(date, false);

    CommHistoryTrackerIO::instance()->prepareMessageQuery(query, message);

    return d->executeQuery(query);
}


