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

#include "qorganizeritemobserver.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerItemObserver
  \brief The QOrganizerItemObserver class is a simple class that emits a signal when a single
  particular item is updated or deleted.
  \inmodule QtOrganizer

  \ingroup organizer-main

  This class is created by calling QOrganizerManager::observeItem(QOrganizerItemId).

  \sa QOrganizerManager::observeItem
 */

QOrganizerItemObserver::QOrganizerItemObserver(QObject* parent) : QObject(parent) {}

/*!
  \fn itemChanged()

  This signal is emitted when the observed item is changed in the manager.
 */

/*!
  \fn itemRemoved()

  This signal is emitted when the observed item is removed from the manager.
 */

/*!
  This function causes the itemChanged() signal to be emitted.
 */
void QOrganizerItemObserver::emitItemChanged()
{
    emit itemChanged();
}

/*!
  This function causes the itemRemoved() signal to be emitted.
 */
void QOrganizerItemObserver::emitItemRemoved()
{
    emit itemRemoved();
}

#include "moc_qorganizeritemobserver.cpp"
