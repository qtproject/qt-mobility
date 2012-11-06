/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizeritemobserver.h"
#include "qorganizeritemid.h"
#include "qorganizermanager_p.h"

QTM_BEGIN_NAMESPACE
class QOrganizerItemObserverPrivate
{
    public:
        QOrganizerItemId m_id;
        QWeakPointer<QOrganizerManager> m_manager;
        QOrganizerManagerData* m_managerPrivate;
};
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
  \class QOrganizerItemObserver
  \brief The QOrganizerItemObserver class is a simple class that emits a signal when a single
  particular item is updated or deleted.
  \inmodule QtOrganizer
  \since 1.2

  \ingroup organizer-main
 */

/*!
  Constructs a QOrganizerItemObserver to observe the item in \a manager with the given \a itemId and a \a parent object.
  \since 1.2
 */
QOrganizerItemObserver::QOrganizerItemObserver(QOrganizerManager* manager,
                                               const QOrganizerItemId& itemId,
                                               QObject* parent)
    : QObject(parent),
      d(new QOrganizerItemObserverPrivate)
{
    d->m_id = itemId;
    d->m_manager = manager;
    d->m_managerPrivate = QOrganizerManagerData::get(manager);
    d->m_managerPrivate->registerObserver(this);
}

/*!
  Destroys this observer.
 */
QOrganizerItemObserver::~QOrganizerItemObserver()
{
    if (d->m_manager.data()) {
        d->m_managerPrivate->unregisterObserver(this);
    }
    delete d;
}

/*!
  Returns the id of the item that this object observes.
  \since 1.2
 */
QOrganizerItemId QOrganizerItemObserver::itemId() const {
    return d->m_id;
}

/*!
  \fn void QOrganizerItemObserver::itemChanged()

  This signal is emitted when the observed item is changed in the manager.
  \since 1.2
 */

/*!
  \fn void QOrganizerItemObserver::itemRemoved()

  This signal is emitted when the observed item is removed from the manager.
  \since 1.2
 */

#include "moc_qorganizeritemobserver.cpp"
