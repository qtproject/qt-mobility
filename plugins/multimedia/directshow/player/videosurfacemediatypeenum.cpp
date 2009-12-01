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

VideoSurfaceMediaTypeEnum::VideoSurfaceMediaTypeEnum(const QVector<AM_MEDIA_TYPE> &types)
    : m_ref(1)
    , m_mediaTypes(types)
    , m_index(0)
{
}

VideoSurfaceMediaTypeEnum::VideoSurfaceMediaTypeEnum(const QList<QVideoFrame::PixelFormat> &formats)
    : m_ref(1)
    , m_index(0)
{
    m_mediaTypes.reserve(formats.count());

    AM_MEDIA_TYPE type;
    type.majortype = MEDIATYPE_Video;
    type.bFixedSizeSamples = TRUE;
    type.bTemporalCompression = FALSE;
    type.lSampleSize = 0;
    type.formattype = GUID_NULL;
    type.pUnk = 0;
    type.cbFormat = 0;
    type.pbFormat = 0;

    foreach (QVideoFrame::PixelFormat format, formats) {
        type.subtype = VideoSurfaceMediaType::convertPixelFormat(format);

        if (type.subtype != MEDIASUBTYPE_None)
            m_mediaTypes.append(type);
    }
}

VideoSurfaceMediaTypeEnum::~VideoSurfaceMediaTypeEnum()
{
    QVector<AM_MEDIA_TYPE>::iterator it;
    for (it = m_mediaTypes.begin(); it != m_mediaTypes.end(); ++it) {
        if (it->cbFormat > 0)
            CoTaskMemFree(it->pbFormat);
        if (it->pUnk)
            it->pUnk->Release();
    }
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
    if (ppMediaTypes && (cMediaTypes == 1 || pcFetched)) {
        int count = qBound<int>(0, m_mediaTypes.count() - m_index, cMediaTypes);

        for (int i = 0; i < count; ++i, ++m_index) {
            ppMediaTypes[i] = reinterpret_cast<AM_MEDIA_TYPE *>(
                    CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)));

            if (ppMediaTypes) {
                VideoSurfaceMediaType::copy(ppMediaTypes[i], m_mediaTypes.at(i));
            } else {
                for (--i; i >= 0; --i)
                    CoTaskMemFree(ppMediaTypes[i]);

                if (pcFetched)
                    *pcFetched = 0;

                return E_OUTOFMEMORY;
            }
        }
        if (pcFetched)
            *pcFetched = count;

        return count == cMediaTypes ? S_OK : S_FALSE;
    } else  {
        return E_POINTER;
    }
}

HRESULT VideoSurfaceMediaTypeEnum::Skip(ULONG cMediaTypes)
{
    m_index = qMin<int>(m_index + cMediaTypes, m_mediaTypes.size());

    return m_index < m_mediaTypes.size() ? S_OK : S_FALSE;
}

HRESULT VideoSurfaceMediaTypeEnum::Reset()
{
    m_index = 0;

    return S_OK;
}

HRESULT VideoSurfaceMediaTypeEnum::Clone(IEnumMediaTypes **ppEnum)
{
    *ppEnum = new VideoSurfaceMediaTypeEnum(m_mediaTypes);

    return S_OK;
}
