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

#include "videosurfacefilter.h"

#include "videosurfacepinenum.h"
#include "videosurfacepin.h"

#include <initguid.h>

// { e23cad72-153d-406c-bf3f-4c4b523d96f2 }
DEFINE_GUID(CLSID_VideoSurfaceFilter,
0xe23cad72, 0x153d, 0x406c, 0xbf, 0x3f, 0x4c, 0x4b, 0x52, 0x3d, 0x96, 0xf2);

VideoSurfaceFilter::VideoSurfaceFilter(QAbstractVideoSurface *surface)
    : m_ref(1)
    , m_surface(surface)
    , m_referenceClock(0)
    , m_sinkPin(0)
    , m_graph(0)
    , m_state(State_Stopped)
{
    m_sinkPin = new VideoSurfacePin(this, surface);
}

VideoSurfaceFilter::~VideoSurfaceFilter()
{
    Q_ASSERT(!m_referenceClock);

    delete m_sinkPin;

    Q_ASSERT(m_ref == 1);
}

HRESULT VideoSurfaceFilter::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IPersist
            || riid == IID_IMediaFilter
            || riid == IID_IBaseFilter) {
        *ppvObject = static_cast<IBaseFilter *>(this);
    } else if (riid == IID_IAMFilterMiscFlags) {
        *ppvObject = static_cast<IAMFilterMiscFlags *>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG VideoSurfaceFilter::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG VideoSurfaceFilter::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

HRESULT VideoSurfaceFilter::GetClassID(CLSID *pClassID)
{
    *pClassID = CLSID_VideoSurfaceFilter;
    *pClassID = CLSID_NULL;

    return S_OK;
}

HRESULT VideoSurfaceFilter::Run(REFERENCE_TIME tStart)
{
    m_state = State_Running;

    m_sinkPin->setState(m_state, tStart);

    return S_OK;
}

HRESULT VideoSurfaceFilter::Pause()
{
    m_state = State_Paused;

    m_sinkPin->setState(m_state, 0);

    return S_OK;
}

HRESULT VideoSurfaceFilter::Stop()
{
    m_state = State_Stopped;

    m_sinkPin->setState(m_state, 0);

    return S_OK;
}

HRESULT VideoSurfaceFilter::GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *pState)
{
    if (!pState)
        return E_POINTER;

    *pState = m_state;

    return S_OK;
}

HRESULT VideoSurfaceFilter::SetSyncSource(IReferenceClock *pClock)
{
    if (m_referenceClock)
        m_referenceClock->Release();

    m_referenceClock = pClock;

    m_sinkPin->setClock(m_referenceClock);

    if (m_referenceClock)
        m_referenceClock->AddRef();

    return S_OK;
}

HRESULT VideoSurfaceFilter::GetSyncSource(IReferenceClock **ppClock)
{
    if (!ppClock) {
        return E_POINTER;
    } else if (m_referenceClock) {
        *ppClock = m_referenceClock;

        m_referenceClock->AddRef();

        return S_OK;
    } else {
        *ppClock = 0;

        return S_FALSE;
    }
}

HRESULT VideoSurfaceFilter::EnumPins(IEnumPins **ppEnum)
{
    if (ppEnum) {
        *ppEnum = new VideoSurfacePinEnum(QList<IPin *>() << m_sinkPin);

        return S_OK;
    } else {
        return E_POINTER;
    }
}

HRESULT VideoSurfaceFilter::FindPin(LPCWSTR pId, IPin **ppPin)
{
    if (!ppPin || !pId) {
        return E_POINTER;
    } else if (QString::fromWCharArray(pId) == m_sinkPin->id()) {
        m_sinkPin->AddRef();

        *ppPin = m_sinkPin;

        return S_OK;
    } else {
        return VFW_E_NOT_FOUND;
    }
}

HRESULT VideoSurfaceFilter::JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName)
{
    m_graph = pGraph;
    m_name = QString::fromWCharArray(pName);

    return S_OK;
}

HRESULT VideoSurfaceFilter::QueryFilterInfo(FILTER_INFO *pInfo)
{
    if (pInfo) {
        QString name = m_name;

        if (name.length() >= MAX_FILTER_NAME)
            name.truncate(MAX_FILTER_NAME - 1);

        int length = name.toWCharArray(pInfo->achName);
        pInfo->achName[length] = '\0';

        if (m_graph)
            m_graph->AddRef();

        pInfo->pGraph = m_graph;

        return S_OK;
    } else {
        return E_POINTER;
    }
}

HRESULT VideoSurfaceFilter::QueryVendorInfo(LPWSTR *pVendorInfo)
{
    Q_UNUSED(pVendorInfo);

    return E_NOTIMPL;
}

ULONG VideoSurfaceFilter::GetMiscFlags()
{
    return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}
