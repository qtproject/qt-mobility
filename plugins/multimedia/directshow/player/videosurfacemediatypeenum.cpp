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

#include "videosurfacemediatypeenum.h"

#include "videosurfacemediatype.h"
#include "videosurfacefilter.h"

VideoSurfaceMediaTypeEnum::VideoSurfaceMediaTypeEnum(
        VideoSurfaceFilter *filter, int token, int index)
    : m_ref(1)
    , m_filter(filter)
    , m_token(token)
    , m_index(index)
{
    m_filter->AddRef();
}

VideoSurfaceMediaTypeEnum::~VideoSurfaceMediaTypeEnum()
{
    m_filter->Release();
}

HRESULT VideoSurfaceMediaTypeEnum::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IEnumMediaTypes) {
        *ppvObject = static_cast<IEnumMediaTypes *>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG VideoSurfaceMediaTypeEnum::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG VideoSurfaceMediaTypeEnum::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    if (ref == 0) {
        delete this;
    }

    return ref;
}

HRESULT VideoSurfaceMediaTypeEnum::Next(
        ULONG cMediaTypes, AM_MEDIA_TYPE **ppMediaTypes, ULONG *pcFetched)
{
    return m_filter->nextMediaType(m_token, &m_index, cMediaTypes, ppMediaTypes, pcFetched);
}

HRESULT VideoSurfaceMediaTypeEnum::Skip(ULONG cMediaTypes)
{
    return m_filter->skipMediaType(m_token, &m_index, cMediaTypes);
}

HRESULT VideoSurfaceMediaTypeEnum::Reset()
{
    m_token = m_filter->currentMediaTypeToken();
    m_index = 0;

    return S_OK;
}

HRESULT VideoSurfaceMediaTypeEnum::Clone(IEnumMediaTypes **ppEnum)
{
    return m_filter->cloneMediaType(m_token, m_index, ppEnum);
}
