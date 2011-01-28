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

#include "qcontactobserver.h"
#include "qcontactid.h"
#include "qcontactmanager_p.h"

QTM_BEGIN_NAMESPACE
class QContactObserverPrivate
{
    public:
        QContactLocalId m_localId;
        QContactManagerData* m_managerPrivate;
};
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
  \class QContactObserver
  \brief The QContactObserver class is a simple class that emits a signal when a single particular
  contact is updated or deleted.
  \inmodule QtContacts

  \ingroup contacts-main
 */

/*!
  Constructs a QContactObserver to observe the contact in \a manager with the given \a localId.
 */
QContactObserver::QContactObserver(QContactManager* manager,
                                   QContactLocalId localId,
                                   QObject* parent)
    : QObject(parent),
      d(new QContactObserverPrivate)
{
    d->m_localId = localId;
    d->m_managerPrivate = QContactManagerData::get(manager);
    d->m_managerPrivate->registerObserver(this);
}

/*!
  Destroys this observer.
 */
QContactObserver::~QContactObserver()
{
    d->m_managerPrivate->unregisterObserver(this);
    delete d;
}

/*!
  Returns the local id of the contact that this object observes.
 */
QContactLocalId QContactObserver::contactLocalId() const {
    return d->m_localId;
}

/*!
  \fn contactChanged()

  This signal is emitted when the observed contact is changed in the manager.
 */

/*!
  \fn contactRemoved()

  This signal is emitted when the observed contact is removed from the manager.
 */

#include "moc_qcontactobserver.cpp"
