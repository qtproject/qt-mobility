/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TRACKERCHANGELISTENER_H_
#define TRACKERCHANGELISTENER_H_

#include <QObject>
#include <QHash>
#include <qcontact.h>
#include <QSharedPointer>
#include <QtTracker/QLive>
#include <QtTracker/Tracker>
#include <qcontactmanagerengine.h>

QTM_USE_NAMESPACE

QContactLocalId url2UniqueId(const QString &contactUrl);

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
    explicit TrackerChangeListener(QContactManagerEngine *engine, QObject *parent);
    virtual ~TrackerChangeListener();

signals:
    // signals are with the same semantics as in QContactManagerEngine
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);

private slots:
    void contactsAdded(const QStringList &subjects);
    void contactsRemoved(const QStringList &subjects);
    void contactsChanged(const QStringList &subjects);
    void imAccountsChanged(const QStringList &subjects);
    void imAddressesChanged(const QStringList &subjects);
private:
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler_contact;
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler_imaccount;
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler_imaddress;
    QContactManagerEngine *engine;
};


#endif /* TRACKERCHANGELISTENER_H_ */
