/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef TRACKERCHANGELISTENER_H_
#define TRACKERCHANGELISTENER_H_

#include <QObject>
#include <QHash>
#include <qcontact.h>
#include <QSharedPointer>
#include <QtTracker/QLive>
#include <QtTracker/Tracker>


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
    void imAccountChanged(const QStringList &subjects);
    // receives
    void imQueryReady(AsyncQuery*);
private:
    QHash<AsyncQuery*, QSharedPointer<AsyncQuery> > pendingQueries;
};


#endif /* TRACKERCHANGELISTENER_H_ */
