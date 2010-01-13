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

#include "videosurfacepinenum.h"

#include <QtMultimedia>

VideoSurfacePinEnum::VideoSurfacePinEnum(const QList<IPin *> &pins)
    : m_ref(1)
    , m_pins(pins)
    , m_index(0)
{
    foreach (IPin *pin, m_pins)
        pin->AddRef();
}

VideoSurfacePinEnum::~VideoSurfacePinEnum()
{
    foreach (IPin *pin, m_pins)
        pin->Release();
}

HRESULT VideoSurfacePinEnum::QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IUnknown
            || riid == IID_IEnumPins) {
        AddRef();

        *ppvObject = static_cast<IEnumPins *>(this);

        return S_OK;
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }
}

ULONG VideoSurfacePinEnum::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG VideoSurfacePinEnum::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    if (ref == 0) {
        delete this;
    }

    return ref;
}

HRESULT VideoSurfacePinEnum::Next(ULONG cPins, IPin **ppPins, ULONG *pcFetched)
{
    if (ppPins && (pcFetched || cPins == 1)) {
        *pcFetched = qBound(0, m_pins.count() - m_index, int(cPins));

        for (ULONG i = 0; i < *pcFetched; ++i, ++m_index) {
            ppPins[i] = m_pins.at(m_index);
            ppPins[i]->AddRef();
        }

        return *pcFetched == cPins ? S_OK : S_FALSE;
    } else {
        return E_POINTER;
    }
}

HRESULT VideoSurfacePinEnum::Skip(ULONG cPins)
{
    m_index = qMin(int(m_index + cPins), m_pins.count());

    return m_index < m_pins.count() ? S_OK : S_FALSE;
}

HRESULT VideoSurfacePinEnum::Reset()
{
    m_index = 0;

    return S_OK;
}

HRESULT VideoSurfacePinEnum::Clone(IEnumPins **ppEnum)
{
    if (ppEnum) {
        *ppEnum = new VideoSurfacePinEnum(m_pins);

        return S_OK;
    } else {
        return E_POINTER;
    }
}
