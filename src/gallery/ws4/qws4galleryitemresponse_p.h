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

#ifndef QWS4GALLERYITEMRESPONSE_P_H
#define QWS4GALLERYITEMRESPONSE_P_H

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

#include "qws4galleryquerybuilder_p.h"

#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>

#include <searchapi.h>

class QWS4GalleryBinding;

struct QWS4GalleryItemListRow : public QSharedData
{
public:
    qint32 workId;
    qint32 flags;
    QString url;
    QString type;
    QVector<QVariant> metaData;
};

class QWS4GalleryItemResponse
    : public QGalleryAbstractResponse
    , public IDBAsynchNotify
    , public IRowsetNotify
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
    , public IRowsetEvents
#endif
{
    Q_OBJECT
public:
    QWS4GalleryItemResponse(
            IRowset *rowSet,
            const QGalleryItemRequest &request,
            const QVector<QWS4GalleryQueryBuilder::Column> &columns,
            QObject *parent = 0);
    ~QWS4GalleryItemResponse();

    QList<int> keys() const;
    QString toString(int key) const;

    int count() const;

    QString id(int index) const;
    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;

    QVariant metaData(int index, int key) const;
    void setMetaData(int index, int key, const QVariant &value);

    MetaDataFlags metaDataFlags(int index, int key) const;

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

    // IRowsetEvents
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
    HRESULT STDMETHODCALLTYPE OnNewItem(
            REFPROPVARIANT itemID, ROWSETEVENT_ITEMSTATE newItemState);
    HRESULT STDMETHODCALLTYPE OnChangedItem(
            REFPROPVARIANT itemID,
            ROWSETEVENT_ITEMSTATE rowsetItemState,
            ROWSETEVENT_ITEMSTATE changedItemState);
    HRESULT STDMETHODCALLTYPE OnDeletedItem(
            REFPROPVARIANT itemID, ROWSETEVENT_ITEMSTATE deletedItemState);
    HRESULT STDMETHODCALLTYPE OnRowsetEvent(
        ROWSETEVENT_TYPE eventType, REFPROPVARIANT eventData);
#endif

protected:
    void customEvent(QEvent *event);

private:
    void readRows(const HROW rows[], DBCOUNTITEM count);

    typedef QSharedDataPointer<QWS4GalleryItemListRow> Row;

    volatile LONG m_ref;
    int m_urlIndex;
    int m_typeIndex;
    DWORD m_asynchNotifyCookie;
    DWORD m_rowsetNotifyCookie;
    DWORD m_rowsetEventsCookie;
    IRowset *m_rowSet;
    QWS4GalleryBinding *m_binding;
    QStringList m_fields;
    QList<int> m_keys;
    QList<int> m_columnIndexes;
    QVector<Row> m_rows;
    QVector<Row> m_pendingRows;
    QMutex m_asynchMutex;
    QWaitCondition m_asynchWait;

};

#endif
