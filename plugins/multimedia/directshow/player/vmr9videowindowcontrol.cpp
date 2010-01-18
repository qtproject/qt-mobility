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

#include "vmr9videowindowcontrol.h"

#include "directshowglobal.h"

Vmr9VideoWindowControl::Vmr9VideoWindowControl(QObject *parent)
    : QVideoWindowControl(parent)
    , m_filter(com_new<IBaseFilter>(CLSID_VideoMixingRenderer9))
    , m_windowId(0)
    , m_fullScreen(false)
{
    qMemSet(&m_procAmp, 0, sizeof(VMR9ProcAmpControl));
    qMemSet(&m_brightnessRange, 0, sizeof(VMR9ProcAmpControlRange));
    qMemSet(&m_contrastRange, 0, sizeof(VMR9ProcAmpControlRange));
    qMemSet(&m_hueRange, 0, sizeof(VMR9ProcAmpControlRange));
    qMemSet(&m_saturationRange, 0, sizeof(VMR9ProcAmpControlRange));

    m_brightnessRange.dwProperty = ProcAmpControl9_Brightness;
    m_contrastRange.dwProperty = ProcAmpControl9_Contrast;
    m_hueRange.dwProperty = ProcAmpControl9_Hue;
    m_saturationRange.dwProperty = ProcAmpControl9_Saturation;

    if (IVMRFilterConfig9 *config = com_cast<IVMRFilterConfig9>(m_filter)) {
        config->SetRenderingMode(VMR9Mode_Windowless);
        config->SetNumberOfStreams(1);
        config->Release();
    }
}

Vmr9VideoWindowControl::~Vmr9VideoWindowControl()
{
    if (m_filter)
        m_filter->Release();
}


WId Vmr9VideoWindowControl::winId() const
{
    return m_windowId;

}

void Vmr9VideoWindowControl::setWinId(WId id)
{
    m_windowId = id;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        control->SetVideoClippingWindow(m_windowId);
        control->Release();
    }
}

QRect Vmr9VideoWindowControl::displayRect() const
{
    QRect rect;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        RECT sourceRect;
        RECT displayRect;

        if (control->GetVideoPosition(&sourceRect, &displayRect) == S_OK) {
            rect = QRect(
                    displayRect.left,
                    displayRect.bottom,
                    displayRect.right - displayRect.left,
                    displayRect.bottom - displayRect.top);
        }
        control->Release();
    }
    return rect;
}

void Vmr9VideoWindowControl::setDisplayRect(const QRect &rect)
{
    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        RECT sourceRect = { 0, 0, 0, 0 };
        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        control->GetNativeVideoSize(&sourceRect.right, &sourceRect.bottom, 0, 0);
        control->SetVideoPosition(&sourceRect, &displayRect);
        control->Release();
    }
}

bool Vmr9VideoWindowControl::isFullScreen() const
{
    return m_fullScreen;
}

void Vmr9VideoWindowControl::setFullScreen(bool fullScreen)
{
    m_fullScreen = fullScreen;
}

void Vmr9VideoWindowControl::repaint()
{
    //if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
    //    control->RepaintVideo(m_windowId, 0);
    //    control->Release();
    //}
}

QSize Vmr9VideoWindowControl::nativeSize() const
{
    QSize size;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        LONG width;
        LONG height;

        if (control->GetNativeVideoSize(&width, &height, 0, 0) == S_OK)
            size = QSize(width, height);
        control->Release();
    }
    return size;
}

QVideoWidget::AspectRatioMode Vmr9VideoWindowControl::aspectRatioMode() const
{
    QVideoWidget::AspectRatioMode mode = QVideoWidget::KeepAspectRatio;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        DWORD arMode;

        if (control->GetAspectRatioMode(&arMode) == S_OK && arMode == VMR9ARMode_None)
            mode = QVideoWidget::IgnoreAspectRatio;
        control->Release();
    }
    return mode;
}

void Vmr9VideoWindowControl::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(m_filter)) {
        switch (mode) {
        case QVideoWidget::IgnoreAspectRatio:
            control->SetAspectRatioMode(VMR9ARMode_None);
            break;
        case QVideoWidget::KeepAspectRatio:
            control->SetAspectRatioMode(VMR9ARMode_LetterBox);
            break;
        default:
            break;
        }
        control->Release();
    }
}

int Vmr9VideoWindowControl::brightness() const
{
    return getProcAmp(&m_procAmp.Brightness, &m_brightnessRange);
}

void Vmr9VideoWindowControl::setBrightness(int brightness)
{
    setProcAmp(brightness, &m_procAmp.Brightness, &m_brightnessRange);
}

int Vmr9VideoWindowControl::contrast() const
{
    return getProcAmp(&m_procAmp.Contrast, &m_contrastRange);
}

void Vmr9VideoWindowControl::setContrast(int contrast)
{
    setProcAmp(contrast, &m_procAmp.Contrast, &m_contrastRange);
}

int Vmr9VideoWindowControl::hue() const
{
    return getProcAmp(&m_procAmp.Hue, &m_hueRange);
}

void Vmr9VideoWindowControl::setHue(int hue)
{
    setProcAmp(hue, &m_procAmp.Hue, &m_hueRange);
}

int Vmr9VideoWindowControl::saturation() const
{
    return getProcAmp(&m_procAmp.Saturation, &m_saturationRange);
}

void Vmr9VideoWindowControl::setSaturation(int saturation)
{
    setProcAmp(saturation, &m_procAmp.Saturation, &m_saturationRange);
}

int Vmr9VideoWindowControl::getProcAmp(
        const float *field, const VMR9ProcAmpControlRange *range) const
{
    if (range->dwSize == 0) {
        if (IVMRMixerControl9 *control = com_cast<IVMRMixerControl9>(m_filter)) {
            const_cast<VMR9ProcAmpControlRange *>(range)->dwSize = sizeof(VMR9ProcAmpControlRange);
            control->GetProcAmpControlRange(0, const_cast<VMR9ProcAmpControlRange *>(range));

            if (m_procAmp.dwSize == 0) {
                const_cast<VMR9ProcAmpControl *>(&m_procAmp)->dwSize = sizeof(VMR9ProcAmpControl);
                control->GetProcAmpControl(0, const_cast<VMR9ProcAmpControl *>(&m_procAmp));
            }

            control->Release();
        }
    }

    if (m_procAmp.dwFlags & range->dwProperty) {
        float value = *field - range->DefaultValue;

        if (value > 0.0) {
            if (!qFuzzyCompare(range->MaxValue, range->DefaultValue))
                value /= range->MaxValue - range->DefaultValue;
            else
                value = 1.0;
        } else if (value < 0.0) {
            if (!qFuzzyCompare(range->MinValue, range->DefaultValue))
                value /= range->MinValue - range->DefaultValue;
            else
                value = -1.0;
        }
        return qBound<int>(-100, value * 100, 100);
    } else {
        return 0;
    }
}

void Vmr9VideoWindowControl::setProcAmp(int value, float *field, VMR9ProcAmpControlRange *range)
{
    if (IVMRMixerControl9 *control = com_cast<IVMRMixerControl9>(m_filter)) {
        if (range->dwSize == 0) {
            range->dwSize = sizeof(VMR9ProcAmpControlRange);
            control->GetProcAmpControlRange(0, range);

            if (m_procAmp.dwSize == 0) {
                m_procAmp.dwSize = sizeof(VMR9ProcAmpControl);
                control->GetProcAmpControl(0, &m_procAmp);
            }
        }

        if (m_procAmp.dwFlags & range->dwProperty) {
            *field = range->DefaultValue;
            if (value > 0) {
                *field += float(value) * (range->MaxValue - range->DefaultValue) / 100;
            } else if (value < 0) {
                *field += float(value) * (range->MinValue - range->DefaultValue) / 100;
            }
            control->SetProcAmpControl(0, &m_procAmp);
        }
    }
}
