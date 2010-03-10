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

#ifndef TRACKERCHANGELISTENER_H_
#define TRACKERCHANGELISTENER_H_

#include <QObject>
#include <QHash>
#include <QSharedPointer>

#include <QtTracker/QLive>
#include <QtTracker/Tracker>

#include <qmobilityglobal.h>
#include <qcontact.h>

QTM_USE_NAMESPACE

QContactLocalId url2UniqueId(const QString &contactUrl);

/*!
 * Helper class to handle multiple async queries at the same time inside TrackerChangeListener.
 * This class is designed to be generic, and initially used to get all contacts that correspond
 * to list of IMAccounts. When we get signal that multiple IMAccounts have changed, we read
 * all contacts and emit signal contacts changed for them.
 *
 * Usage: construct AsyncQuery with RDFSelect query. When signal queryReady(AsyncQuery *self)
 * is received, read data from rows in self->nodes (self-nodes()->rowCount() ...)
 *
 * Intention: The class has been created to wrap LiveNodes::modelUpdated signal,
 * and to know which query it corresponds to (not possible to fetch it from sender()).
 * \sa queryReady(AsyncQuery *)
 */
class AsyncQuery: public QObject
{
    Q_OBJECT
public:
    AsyncQuery(SopranoLive::RDFSelect selectQuery);
    SopranoLive::LiveNodes nodes;

private slots:
    void queryReady();
signals:
    // emitted when modelUpdated() from LiveNodes related to selectQuery is received
    void queryReady(AsyncQuery *self);
};

/*!
 * \class TrackerChangeListener
 * \brief Translates signals from tracker and to contact signals
 * Listen for tracker signals, computes which contacts are and what si changed and emits
 * signals about contact change. Initially designed to compute change signals for
 * QContactTrackerEngine
 */
class TrackerChangeListener : public QObject
{
    Q_OBJECT
public:
    TrackerChangeListener(QObject *parent=0);
    virtual ~TrackerChangeListener();

signals:
    // signals are with the same semantics as in QContactManagerEngine
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);

private slots:
    void subjectsAdded(const QStringList &subjects);
    void subjectsRemoved(const QStringList &subjects);
    void subjectsChanged(const QStringList &subjects);
private:
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler_contact;
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler_imaccount;

    void connectSignals(SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler);

    QHash<AsyncQuery*, QSharedPointer<AsyncQuery> > pendingQueries;
};


#endif /* TRACKERCHANGELISTENER_H_ */
