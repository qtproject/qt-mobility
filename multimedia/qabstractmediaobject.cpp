/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qmetaobject.h>

#include "qabstractmediaobject_p.h"



void QAbstractMediaObjectPrivate::_q_notify()
{
    Q_Q(QAbstractMediaObject);

    const int len = notifyProperties.length();

    for (int i = 0; i < len; ++i) {
        const char *name = notifyProperties.at(i).constData();

        const QMetaObject* m = q->metaObject();

        int pi = m->indexOfProperty(name);
        if (pi == -1)
            continue;

        QMetaProperty p = m->property(pi);
        if (p.hasNotifySignal())
            p.notifySignal().invoke(q, QGenericArgument(QMetaType::typeName(p.userType()),
                                                        p.read(q).data()));
    }
}


/*!
    \class QAbstractMediaObject
    \ingroup multimedia

    \preliminary
    \brief The base Multimedia object.

    \sa QAbstractMediaService, QAbstractMediaControl
*/


/*!
    Destroys the QAbstractMediaObject object.
*/

QAbstractMediaObject::~QAbstractMediaObject()
{
    delete d_ptr;
}

/*!
    \fn QAbstractMediaService* QAbstractMediaObject::service() const

    Returns the media service that provide the functionality for the Multimedia object.
*/

/*!
    \fn bool QAbstractMediaObject::isValid() const

    Returns true if the concrete media object is capable of normal operation;
    false otherwise.
*/


int QAbstractMediaObject::notifyInterval() const
{
    return d_func()->notifyTimer->interval();
}

void QAbstractMediaObject::setNotifyInterval(int milliSeconds)
{
    Q_D(QAbstractMediaObject);

    if (d->notifyTimer->interval() != milliSeconds) {
        d->notifyTimer->setInterval(milliSeconds);

        emit notifyIntervalChanged(milliSeconds);
    }
}

/*!
  \internal
*/
void QAbstractMediaObject::bind(QObject*)
{
}

/*!
    Construct a QAbstractMediaObject with \a parent. This class is meant as a
    base class for Multimedia objects so this constructor is protected.
*/

QAbstractMediaObject::QAbstractMediaObject(QObject *parent)
    : QObject(parent)
    , d_ptr(new QAbstractMediaObjectPrivate)

{
    Q_D(QAbstractMediaObject);

    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));
}

/*!
    \internal
*/

QAbstractMediaObject::QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    Q_D(QAbstractMediaObject);
    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));
}

/*!
    Watch the property \a name. The property's notify signal will be emitted
    once every notifyInterval milliseconds.

    \sa notifyInterval
*/

void QAbstractMediaObject::addPropertyWatch(QByteArray const &name)
{
    Q_D(QAbstractMediaObject);

    d->notifyProperties << name;

    if (!d->notifyTimer->isActive())
        d->notifyTimer->start();
}

/*!
    Remove property \a name from the list of properties whose changes are
    regularly signaled.

    \sa notifyInterval
*/

void QAbstractMediaObject::removePropertyWatch(QByteArray const &name)
{
    Q_D(QAbstractMediaObject);

    d->notifyProperties.removeAll(name);

    if (d->notifyProperties.isEmpty())
        d->notifyTimer->stop();
}

/*!
    \property QAbstractMediaObject::notifyInterval

    The interval at which notifiable properties will update.

    The interval is expressed in milliseconds, the default value is 1000.

    \sa addPropertyWatch(), removePropertyWatch()
*/

/*!
    \fn void QAbstractMediaObject::notifyIntervalChanged(int milliSeconds)

    Signal a change in the notify interval period to \a milliSeconds.
*/

#include "moc_qabstractmediaobject.cpp"
