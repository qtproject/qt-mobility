/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <CommHistory/eventmodel_p.h>
#include <CommHistory/eventsquery.h>
#include <QDebug>
#include "smsmodel_maemo6_p.h"

using namespace CommHistory;

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

    CommHistory::EventsQuery query(d->propertyMask);

    query.addPattern(QLatin1String("%1 rdf:type nmo:SMSMessage ."))
            .variable(Event::Id);

    return d->executeQuery(query);
}


