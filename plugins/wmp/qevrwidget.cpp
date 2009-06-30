/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qevrwidget.h"

#include "qmfactivate.h"

#include <QtCore/qpointer.h>
#include <QtGui/qevent.h>

#include <d3d9.h>

class QEvrWidgetActivate : public QMFActivate
{
public:
    QEvrWidgetActivate(QEvrWidget *widget);
    ~QEvrWidgetActivate();

    HRESULT STDMETHODCALLTYPE ActivateObject(REFIID riid, void **ppv);
    
    HRESULT STDMETHODCALLTYPE ShutdownObject();
    
    HRESULT STDMETHODCALLTYPE DetachObject();

private:
    QPointer<QEvrWidget> m_widget;
    IMFVideoPresenter *m_presenter;
};

QEvrWidgetActivate::QEvrWidgetActivate(QEvrWidget *widget)
    : m_widget(widget)
    , m_presenter(0)
{
}

QEvrWidgetActivate::~QEvrWidgetActivate()
{
}

HRESULT QEvrWidgetActivate::ActivateObject(REFIID riid, void **ppv)
{
    if (riid != __uuidof(IMFVideoPresenter)) {
        return E_NOINTERFACE;
    } else if (m_presenter) {
        *ppv = m_presenter;

        return S_OK;
    } else {
        IMFGetService *service;

        HRESULT hr = S_OK;

        if ((hr = MFCreateVideoPresenter(
                0,
                __uuidof(IDirect3DDevice9),
                __uuidof(IMFVideoPresenter),
                reinterpret_cast<void **>(&m_presenter))) != S_OK) {
            qWarning("failed to create video presenter");
        } else if ((hr = m_presenter->QueryInterface(
                __uuidof(IMFGetService), reinterpret_cast<void **>(&service))) != S_OK) {
            qWarning("failed to query IMFGetService interface");
        } else {
            IMFVideoDisplayControl *control = 0;

            if ((hr = service->GetService(
                    MR_VIDEO_RENDER_SERVICE,
                    __uuidof(IMFVideoDisplayControl),
                    reinterpret_cast<void **>(&control))) != S_OK) {
                qWarning("failed to get IMFVideoDisplayControl service");
            } else {
                m_widget->setDisplayControl(control);
            }
            service->Release();
        }

        if (m_presenter && hr != S_OK) {
            m_presenter->Release();
            m_presenter = 0;
        }

        *ppv = m_presenter;

        return hr;
    }
}

HRESULT QEvrWidgetActivate::ShutdownObject()
{
    if (m_presenter) {
        if (m_widget)
            m_widget->setDisplayControl(0);

        m_presenter->Release();
        m_presenter = 0;
    }
    return S_OK;
}

HRESULT QEvrWidgetActivate::DetachObject()
{
    if (m_presenter) {
        m_presenter->Release();
        m_presenter = 0;
    }

    return S_OK;
}

QEvrWidget::QEvrWidget(QWidget *parent)
    : QWidget(parent)
    , m_display(0)
    , m_activate(0)
    , m_fullscreen(false)
{
    m_activate = new QEvrWidgetActivate(this);
}

QEvrWidget::~QEvrWidget()
{
    if (m_display)
        m_display->Release();

    m_activate->Release();
}

IMFActivate *QEvrWidget::activate() const
{
    return m_activate;
}

void QEvrWidget::setFullscreen(bool fullscreen)
{
    if (m_display) {
        m_display->SetFullscreen(fullscreen ? TRUE : FALSE);
        
        // Little more involved than that, but anyway.
    }
}

QSize QEvrWidget::sizeHint() const
{
    SIZE videoSize;
    SIZE aspectRatio;

    if (m_display && m_display->GetNativeVideoSize(&videoSize, &aspectRatio) == S_OK) {
        // TODO: compensate for aspect ratio.

        return QSize(videoSize.cx, videoSize.cy);
    } else {
        return QSize();
    }
}

void QEvrWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_display) {
        m_display->RepaintVideo();
    }
}

void QEvrWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);

    if (m_display) {
        const QRect r = rect();

        RECT displayRect;
        displayRect.left = r.left();
        displayRect.top = r.top();
        displayRect.right = r.right();
        displayRect.bottom = r.bottom();

        m_display->SetVideoPosition(0, &displayRect);
    }
}

void QEvrWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_display) {
        const QRect r = rect();

        RECT displayRect;
        displayRect.left = r.left();
        displayRect.top = r.top();
        displayRect.right = r.right();
        displayRect.bottom = r.bottom();

        m_display->SetVideoPosition(0, &displayRect);
    }
}

bool QEvrWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ParentChange && m_display) {
        m_display->SetVideoWindow(effectiveWinId());
    }

    return QWidget::event(event);
}

void QEvrWidget::setDisplayControl(IMFVideoDisplayControl *control)
{
    if (m_display)
        m_display->Release();

    m_display = control;

    if (m_display) {
        m_display->AddRef();
        m_display->SetVideoWindow(effectiveWinId());

        const QRect r = rect();

        RECT displayRect;
        displayRect.left = r.left();
        displayRect.top = r.top();
        displayRect.right = r.right();
        displayRect.bottom = r.bottom();

        m_display->SetVideoPosition(0, &displayRect);
    }
}
