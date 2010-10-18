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

#ifndef QDECLARATIVECONTACTCHANGELOGFILTER_H
#define QDECLARATIVECONTACTCHANGELOGFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactchangelogfilter.h"

class QDeclarativeContactChangeLogFilter: public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QDateTime since READ since WRITE setSince NOTIFY valueChanged)
    Q_PROPERTY(EventType eventType READ eventType WRITE setEventType NOTIFY valueChanged)

    Q_ENUMS(EventType)
public:
    enum EventType {
        EventAdded,
        EventChanged,
        EventRemoved
    };
    QDeclarativeContactChangeLogFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent)
    {
    }

    void setEventType(EventType type)
    {
        QContactChangeLogFilter::EventType t;

        if (type == EventAdded)
            t = QContactChangeLogFilter::EventAdded;
        else if (type == EventChanged)
            t = QContactChangeLogFilter::EventChanged;
        else if (type == EventRemoved)
            t = QContactChangeLogFilter::EventRemoved;

        d.setEventType(t);
    }

    EventType eventType() const
    {
        switch (d.eventType()) {
        case QContactChangeLogFilter::EventAdded:
            return EventAdded;
        case QContactChangeLogFilter::EventChanged:
            return EventChanged;
        case QContactChangeLogFilter::EventRemoved:
            return EventRemoved;
        }
        return EventAdded;
    }

    void setSince(const QDateTime& since)
    {
        d.setSince(since);
    }

    QDateTime since() const
    {
        return d.since();
    }

    QContactFilter filter() const
    {
        return d;
    }
signals:
    void valueChanged();


private:
    QContactChangeLogFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactChangeLogFilter)

#endif
