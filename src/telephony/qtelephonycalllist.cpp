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
#include <QtCore/qglobal.h>

#include "qtelephony.h"
#include "qtelephonycalllist.h"

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
# include "maemo/qtelephonycallinfo_maemo_p.h"
# include "maemo/qtelephonycalllist_maemo_p.h"
#elif defined(Q_OS_LINUX)
# include "qtelephonycallinfo_p.h"
# include "linux/qtelephonycalllist_linux_p.h"
#elif defined(Q_OS_SYMBIAN)
# include "qtelephonycallinfo_p.h"
# include "qtelephonycalllist_symbian_p.h"
#else
# include "qtelephonycallinfo_p.h"
# include "qtelephonycalllist_unsupported_p.h"
#endif

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \fn void QTelephonyCallList::activeCallStatusChanged(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that changed its status.

    This signal is emitted whenever the status of a active call was changed.
*/

/*!
    \fn void QTelephonyCallList::activeCallRemoved(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was removed from the active call list.

    This signal is emitted whenever a call was removed from the active call list.
*/

/*!
    \fn void QTelephonyCallList::activeCallAdded(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was added to the active call list.

    This signal is emitted whenever a call was added to the active call list.
*/

/*!
    \class QTelephonyCallList

    \ingroup telephonyapi
    \inmodule QtTelephony

    \brief The QTelephonyCallList class contains a list of active calls. This object can notify other applications about call changes.

    QTelephonyCallList supports calls event notification for other application and an access of the list of the
    existing calls.
*/

/*!
    \fn QTelephonyCallList::QTelephonyCallList(QObject *parent)
    \a parent The parent of this object. Default is 0.

    Constructor for the QTelephonyCallList object
*/
QTelephonyCallList::QTelephonyCallList(QObject *parent)
    : QObject(parent)
    , enableCallStatusChangeNotify(false)
    , enableCallRemovedNotify(false)
    , enableCallAddedNotify(false)
    , enableCallCountChangedNotify(false)
{
    qDebug() << "QTelephonyCallList::QTelephonyCallList(QObject *parent)";
    d = new QTelephonyCallListPrivate(this);
}

/*!
    \fn QTelephonyCallList::~QTelephonyCallList()

    Destructor of QTelephonyCallList.
*/
QTelephonyCallList::~QTelephonyCallList()
{
    qDebug() << "QTelephonyCallList::~QTelephonyCallList()";
    if(d)
        delete d;
}

/*!
    \fn QList<QTelephonyCallInfo> QTelephonyCallList::activeCalls(const QTelephony::CallType& calltype) const
    \a calltype All calls in the list have this type.

    Returns a list of calls from type of calltype.
*/
QList<QTelephonyCallInfo> QTelephonyCallList::activeCalls(const QTelephony::CallType& calltype) const
{
    if(d)
        return d->activeCalls(calltype);
    return QList<QTelephonyCallInfo>();
}

/*!
    \fn int QTelephonyCallList::activeCallCount() const

    Returns the number of current active calls.
*/
int QTelephonyCallList::activeCallCount() const
{
    if(d)
        return d->activeCallCount();
    return 0;
}

void QTelephonyCallList::connectNotify ( const char * signal )
{
    qDebug() << "connectNotify";
    if (QLatin1String(signal) == SIGNAL(activeCallStatusChanged(QTelephonyCallInfo)))
        enableCallStatusChangeNotify = true;
    else if (QLatin1String(signal) == SIGNAL(activeCallRemoved(QTelephonyCallInfo)))
        enableCallRemovedNotify = true;
    else if (QLatin1String(signal) == SIGNAL(activeCallAdded(QTelephonyCallInfo)))
        enableCallAddedNotify = true;
    else if (QLatin1String(signal) == SIGNAL(activeCallCountChanged())){
        enableCallCountChangedNotify = true;
        if(activeCallCount() > 0)
            emit activeCallCountChanged();
    }
}


#include "moc_qtelephonycalllist.cpp"

QTM_END_NAMESPACE
