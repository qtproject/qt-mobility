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

#ifndef QGALLERYROWSETRESPONSE_P_H
#define QGALLERYROWSETRESPONSE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgalleryrequest.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>

#include <searchapi.h>

namespace QWindowsSearch
{

class QGalleryEvent : public QEvent
{
public:
    enum Type
    {
        Progress = QEvent::User,
        Stop,
        ItemsInserted,
        ItemsChanged
    };

    QGalleryEvent(Type type) : QEvent(QEvent::Type(type)) {}
};

class QGalleryProgressEvent : public QGalleryEvent
{
public:
    QGalleryProgressEvent(int current, int maximum)
        : QGalleryEvent(Progress)
        , currentProgress(current)
        , maximumProgress(maximum)
    {
    }

    const int currentProgress;
    const int maximumProgress;
};

class QGalleryStopEvent : public QGalleryEvent
{
public:
    QGalleryStopEvent(HRESULT result)
        : QGalleryEvent(Stop)
        , result(result)
    {
    }

    const HRESULT result;
};

class QGalleryItemsInsertedEvent : public QGalleryEvent
{
public:
    QGalleryItemsInsertedEvent(int count)
        : QGalleryEvent(ItemsInserted)
        , count(count)
    {
    }

    const int count;
};

class QGalleryRowSetResponse
    : public QGalleryAbstractResponse
    , public IDBAsynchNotify
    , public IRowsetNotify
{
    Q_OBJECT
public:
    QGalleryRowSetResponse(IRowsetScroll *rowSet, QObject *parent = 0);
    ~QGalleryRowSetResponse();

    void cancel();

    bool waitForFinished(int msecs);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDBAsynchNotify
    HRESULT STDMETHODCALLTYPE OnLowResource(DB_DWRESERVE dwReserved);
    HRESULT STDMETHODCALLTYPE OnProgress(
            HCHAPTER hChapter,
            DBASYNCHOP eOperation,
            DBCOUNTITEM ulProgress,
            DBCOUNTITEM ulProgressMax,
            DBASYNCHPHASE eAsynchPhase,
            LPOLESTR pwszStatusText);
    HRESULT STDMETHODCALLTYPE OnStop(
            HCHAPTER hChapter, DBASYNCHOP eOperation, HRESULT hrStatus, LPOLESTR pwszStatusText);

    // IRowSetNotify
    HRESULT STDMETHODCALLTYPE OnFieldChange(
            IRowset *pRowset,
            HROW hRow,
            DBORDINAL cColumns,
            DBORDINAL rgColumns[],
            DBREASON eReason,
            DBEVENTPHASE ePhase,
            BOOL fCantDeny);
    HRESULT STDMETHODCALLTYPE OnRowChange(
            IRowset *pRowset,
            DBCOUNTITEM cRows,
            const HROW rghRows[],
            DBREASON eReason,
            DBEVENTPHASE ePhase,
            BOOL fCantDeny);
    HRESULT STDMETHODCALLTYPE OnRowsetChange(
            IRowset *pRowset, DBREASON eReason, DBEVENTPHASE ePhase, BOOL fCantDeny);


protected:
    enum
    {
        PageSize = 32   // Must be a power of 2.
    };

    void customEvent(QEvent *event);

private:
    volatile LONG m_ref;
    DWORD m_asynchNotifyCookie;
    DWORD m_rowsetNotifyCookie;
    IRowsetScroll *m_rowSet;
    QWaitCondition m_asynchWait;
    QMutex m_asynchMutex;
};

}

#endif
