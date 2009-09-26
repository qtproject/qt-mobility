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
using namespace SopranoLive;

/*!
 * Helper class to handle multiple async queries at the same time inside TrackerChangeListener
 */
class AsyncQuery: public QObject
{
    Q_OBJECT
public:
    AsyncQuery(RDFSelect selectQuery);
    LiveNodes nodes;
// queryready slot and signals are used just to avoid using sender to identify
// which query is ready
private slots:
    void queryReady();
signals:
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
    void contactsAdded(const QList<QUniqueId>& contactIds);
    void contactsChanged(const QList<QUniqueId>& contactIds);
    void contactsRemoved(const QList<QUniqueId>& contactIds);

private slots:
    void subjectsAdded(const QStringList &subjects);
    void subjectsRemoved(const QStringList &subjects);
    void subjectsChanged(const QStringList &subjects);
    void imAccountChanged(const QStringList &subjects);
    void queryReady(AsyncQuery*);
private:
    QHash<AsyncQuery*, QSharedPointer<AsyncQuery> > pendingQueries;
};


#endif /* TRACKERCHANGELISTENER_H_ */
