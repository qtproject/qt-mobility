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

#ifndef VIDEOSURFACEMEDIATYPEENUM_H
#define VIDEOSURFACEMEDIATYPEENUM_H

#include <QtCore/qvector.h>
#include <QtMultimedia/qvideoframe.h>

#include <dshow.h>

class VideoSurfaceMediaTypeEnum : public IEnumMediaTypes
{
public:
    VideoSurfaceMediaTypeEnum(const QVector<AM_MEDIA_TYPE> &types);
    VideoSurfaceMediaTypeEnum(const QList<QVideoFrame::PixelFormat> &formats);
    ~VideoSurfaceMediaTypeEnum();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IEnumMediaTypes
    HRESULT STDMETHODCALLTYPE Next(
            ULONG cMediaTypes, AM_MEDIA_TYPE **ppMediaTypes, ULONG *pcFetched);
    HRESULT STDMETHODCALLTYPE Skip(ULONG cMediaTypes);
    HRESULT STDMETHODCALLTYPE Reset();

    HRESULT STDMETHODCALLTYPE Clone(IEnumMediaTypes **ppEnum);

private:
    LONG m_ref;
    QVector<AM_MEDIA_TYPE> m_mediaTypes;
    int m_index;
};

#endif
