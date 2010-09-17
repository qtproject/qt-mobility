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

#include <qcamera.h>
#include <qabstractvideosurface.h>
#include <qvideoframe.h>

#include "s60cameraviewfinderengine.h"
#include "s60cameraengine.h"
#include "s60videowidgetcontrol.h"
#include "s60videorenderercontrol.h"

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>

CCameraViewfinderEngine::CCameraViewfinderEngine(QObject *parent, CCameraEngine *engine) :
    QObject(parent),
    m_cameraEngine(engine),
    m_viewfinderOutput(NULL),
    m_viewfinderSurface(NULL),
    m_wsSession(CCoeEnv::Static()->WsSession()),
    m_screenDevice(*CCoeEnv::Static()->ScreenDevice()),
    m_window(NULL),
    m_vfState(EVFNotConnectedNotStarted),
    m_viewfinderSize(QSize(320,240)),   // Default VF size (if no VF output is attached)
    m_actualViewFinderSize(QSize(320,240)),
    m_viewfinderType(OutputTypeNotSet),
    m_viewfinderNativeType(EBitmapViewFinder), // Default type
    m_isViewFinderVisible(true) // True
{
    // Check whether platform supports DirectScreen ViewFinder
    if (m_cameraEngine) {
        if (m_cameraEngine->IsDirectViewFinderSupported())
            m_viewfinderNativeType = EDirectScreenViewFinder;
        else
            m_viewfinderNativeType = EBitmapViewFinder;

        MCameraViewfinderObserver *vfObserver = this;
        m_cameraEngine->SetViewfinderObserver(vfObserver);
    }
}

CCameraViewfinderEngine::~CCameraViewfinderEngine()
{
    stopViewfinder();
    m_viewfinderOutput = NULL;
    m_viewfinderSurface = NULL;
}

void CCameraViewfinderEngine::setVideoWidgetControl(QObject *viewfinderOutput)
{
    S60VideoWidgetControl* viewFinderWidgetControl =
            qobject_cast<S60VideoWidgetControl*>(viewfinderOutput);

    if (viewFinderWidgetControl) {

        bool isWidgetVisible = false;

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {
            viewFinderWidgetControl->reconfigureWidget(true); // Reconfigure to be Direct

            QWidget *widget = qobject_cast<QWidget *>(viewFinderWidgetControl->videoWidget());

            connect(viewFinderWidgetControl, SIGNAL(widgetVisible(bool)), this, SLOT(handleVisibilityChange(bool)));
            connect(viewFinderWidgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetViewfinderDisplay()));

            m_viewfinderSize = widget->size();
            isWidgetVisible = widget->isVisible();
        } else {
            viewFinderWidgetControl->reconfigureWidget(false); // Reconfigure to be Bitmap

            QLabel *widget = qobject_cast<QLabel *>(viewFinderWidgetControl->videoWidget());

            connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
            connect(viewFinderWidgetControl, SIGNAL(widgetVisible(bool)), this, SLOT(handleVisibilityChange(bool)));

            m_viewfinderSize = widget->size();
            isWidgetVisible = widget->isVisible();
        }
        
        connect(viewFinderWidgetControl, SIGNAL(widgetResized(QSize)), this, SLOT(resetViewfinderSize(QSize)));
        
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeVideoWidget;
        m_isViewFinderVisible = isWidgetVisible;

        switch (m_vfState) {
            case EVFNotConnectedNotStarted:
                m_vfState = EVFIsConnectedNotStarted;
                break;
            case EVFNotConnectedIsStarted:
                if (m_isViewFinderVisible)
                    m_vfState = EVFIsConnectedIsStartedIsVisible;
                else
                    m_vfState = EVFIsConnectedIsStartedNotVisible;
                break;
            case EVFIsConnectedNotStarted:
            case EVFIsConnectedIsStartedNotVisible:
            case EVFIsConnectedIsStartedIsVisible:
                // Already connected, state does not change
                break;
            default:
                emit error(QCamera::CameraError, tr("General viewfinder error."));
                break;
        }

        if (m_vfState == EVFIsConnectedIsStartedIsVisible)
            startViewfinder(true);
    }
}

void CCameraViewfinderEngine::setVideoRendererControl(QObject *viewfinderOutput)
{
    S60VideoRendererControl* viewFinderRenderControl = 
        qobject_cast<S60VideoRendererControl*>(viewfinderOutput);

    if (viewFinderRenderControl) {

        m_viewfinderNativeType = EBitmapViewFinder;
        m_isViewFinderVisible = true;

        if (viewFinderRenderControl->surface() == NULL) {
            
            connect(viewFinderRenderControl, SIGNAL(viewFinderSurfaceSet()), 
                this, SLOT(resetViewfinderDisplay()));
    
            m_viewfinderOutput = viewfinderOutput;
            m_viewfinderType = OutputTypeRenderer;

            // Use display resolution as default viewfinder resolution
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect screenRect = desktopWidget->screenGeometry();
            m_viewfinderSize = QSize(screenRect.width(), screenRect.height());
            
            switch (m_vfState) {
                case EVFNotConnectedNotStarted:
                    m_vfState = EVFIsConnectedNotStarted;
                    break;
                case EVFNotConnectedIsStarted:
                    m_vfState = EVFIsConnectedIsStartedIsVisible; // GraphicsItem "always visible" (FrameWork decides to draw/not draw)
                    break;
                case EVFIsConnectedNotStarted:
                case EVFIsConnectedIsStartedNotVisible:
                case EVFIsConnectedIsStartedIsVisible:
                    // Already connected, state does not change
                    break;
                default:
                    emit error(QCamera::CameraError, tr("General viewfinder error."));
                    break;
            }

            if (m_vfState == EVFIsConnectedIsStartedIsVisible)
                startViewfinder(true);

            return;
        }
        
        connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
            this, SLOT(viewFinderBitmapReady(const QPixmap &)));
        connect(viewFinderRenderControl, SIGNAL(viewFinderSurfaceSet()), 
            this, SLOT(resetViewfinderDisplay()));
        
        
        m_viewfinderSurface = viewFinderRenderControl->surface();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeRenderer;
        m_viewfinderSize = m_viewfinderSurface->nativeResolution();
        
        // Set Surface properties
        m_surfaceFormat = m_viewfinderSurface->surfaceFormat();
        m_surfaceFormat.setFrameSize(m_viewfinderSize);
        m_surfaceFormat.setFrameRate(30);
        m_surfaceFormat.setViewport(QRect(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height()));
        m_surfaceFormat.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_Undefined); // EColor16MU (compatible with EColor16MA)
        m_surfaceFormat.setPixelAspectRatio(1,1); // PAR 1:1

        switch (m_vfState) {
            case EVFNotConnectedNotStarted:
                m_vfState = EVFIsConnectedNotStarted;
                break;
            case EVFNotConnectedIsStarted:
                m_vfState = EVFIsConnectedIsStartedIsVisible; // GraphicsItem "always visible" (FrameWork decides to draw/not draw)
                break;
            case EVFIsConnectedNotStarted:
            case EVFIsConnectedIsStartedNotVisible:
            case EVFIsConnectedIsStartedIsVisible:
                // Already connected, state does not change
                break;
            default:
                emit error(QCamera::CameraError, tr("General viewfinder error."));
                break;
        }

        if (m_vfState == EVFIsConnectedIsStartedIsVisible)
            startViewfinder(true);
    }
}

void CCameraViewfinderEngine::setVideoWindowControl(QObject *viewfinderOutput)
{
    Q_UNUSED(viewfinderOutput);
    emit error(QCamera::NotSupportedFeatureError, tr("Viewfinder output not supported."));
}

void CCameraViewfinderEngine::startViewfinder(const bool internalStart)
{
    if (!internalStart) {
        switch (m_vfState) {
            case EVFNotConnectedNotStarted:
                m_vfState = EVFNotConnectedIsStarted;
                break;
            case EVFIsConnectedNotStarted:
                if (m_isViewFinderVisible)
                    m_vfState = EVFIsConnectedIsStartedIsVisible;
                else
                    m_vfState = EVFIsConnectedIsStartedNotVisible;
                break;
            case EVFNotConnectedIsStarted:
            case EVFIsConnectedIsStartedNotVisible:
            case EVFIsConnectedIsStartedIsVisible:
                // Already started, state does not change
                break;
            default:
                emit error(QCamera::CameraError, tr("General viewfinder error."));
                break;
        }
    }
    
    // Start viewfinder
    if (m_vfState == EVFIsConnectedIsStartedIsVisible) {
        if (m_viewfinderNativeType == EDirectScreenViewFinder) {
            QWidget *activeWindow = QApplication::activeWindow();

            S60VideoWidgetControl* viewFinderWidgetControl =
                qobject_cast<S60VideoWidgetControl*>(m_viewfinderOutput);

            WId widgetWId = viewFinderWidgetControl->windowId();
            if (widgetWId) {
                m_window = widgetWId->DrawableWindow();
                // Set background color for the Widget (Black to cover white surface)
                RWindow *window = static_cast<RWindow *>(m_window);
                window->SetBackgroundColor(TRgb(0, 0, 0, 255));

#ifdef USING_NGA
                // ViewFinder rect passed for ECam is relative to window
                TRect viewfinderRect(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height());
#else // S60 5.0 or older
                // ViewFinder rect passed for ECam is relative to display
                QWidget *widget = viewFinderWidgetControl->videoWidget();

                QRect qRect(widget->mapToGlobal(widget->pos()), widget->size());
                TRect viewfinderRect(TPoint(qRect.x(),qRect.y()), TSize(qRect.width(),qRect.height()));
#endif // USING_NGA

                TInt err = KErrNone;
                TRAP(err, m_cameraEngine->StartDirectViewFinderL(m_wsSession, m_screenDevice, *window, viewfinderRect));
                if (err) {
                    emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                }

                QSize actualSize((viewfinderRect.iBr.iX - viewfinderRect.iTl.iX),
                    (viewfinderRect.iBr.iY - viewfinderRect.iTl.iY));
                if (actualSize.width() != m_viewfinderSize.width() || actualSize.width() != m_viewfinderSize.height()) {
                    m_actualViewFinderSize = actualSize;
                }
            } else {
                return;
            }

            // Reactivate original window (winId() call for VF changes this)
            if (activeWindow) {
                activeWindow->activateWindow();
                activeWindow->update();
                activeWindow->repaint();
            }

        } else { // Bitmap ViewFinder
            TSize size = TSize(m_viewfinderSize.width(), m_viewfinderSize.height());
    
            if( m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface) {
                // Start rendering to surface with correct size and format
                if (m_viewfinderSurface->isFormatSupported(m_surfaceFormat))
                    m_viewfinderSurface->start(m_surfaceFormat);
                else {
                    emit error(QCamera::CameraError, tr("Viewfinder format not supported."));
                    return;
                }
            }

            m_cameraEngine->StartViewFinderL(size);
            if (size.iWidth != m_viewfinderSize.width() || size.iHeight != m_viewfinderSize.height()) {
                m_actualViewFinderSize = QSize(size.iWidth, size.iHeight);
            }
        }
    }
}

void CCameraViewfinderEngine::stopViewfinder(const bool internalStop)
{
    if (m_vfState != EVFIsConnectedIsStartedIsVisible) {
        return;
    }

    if( m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface)
        m_viewfinderSurface->stop();
    
    m_cameraEngine->StopViewFinder();

    if (!internalStop) {
        switch (m_vfState) {
            case EVFNotConnectedNotStarted:
            case EVFIsConnectedNotStarted:
                // Discard
                break;
            case EVFNotConnectedIsStarted:
                m_vfState = EVFNotConnectedNotStarted;
                break;
            case EVFIsConnectedIsStartedNotVisible:
            case EVFIsConnectedIsStartedIsVisible:
                m_vfState = EVFIsConnectedNotStarted;
                break;
            default:
                emit error(QCamera::CameraError, tr("General viewfinder error."));
                break;
        }
    }
}

void CCameraViewfinderEngine::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(&aFrame);
    
    emit viewFinderFrameReady(pixmap);
    m_cameraEngine->ReleaseViewFinderBuffer();
}

void CCameraViewfinderEngine::resetViewfinderSize(QSize size)
{
    if (m_viewfinderSize == size) {
        return;
    }
    
    m_viewfinderSize = size;
    
    if(m_vfState != EVFIsConnectedIsStartedIsVisible) {
        return;
    }
    
    stopViewfinder(true);

    startViewfinder(true);
}

void CCameraViewfinderEngine::resetViewfinderDisplay()
{
    if (m_viewfinderNativeType == EDirectScreenViewFinder) {
        switch (m_viewfinderType) {
            case OutputTypeVideoWidget: {
                if (!m_viewfinderOutput)
                    return;

                // First stop viewfinder
                stopViewfinder(true);

                S60VideoWidgetControl* viewFinderWidgetControl =
                    qobject_cast<S60VideoWidgetControl*>(m_viewfinderOutput);

                WId widgetWId = viewFinderWidgetControl->windowId();
                if (!widgetWId) {
                    return;
                }

                // Then start it with the new WindowID
                startViewfinder(true);
                break;
            }

            default:
                // Not ViewFinder Output has been set, Discard
                break;
        }

    } else if (m_viewfinderNativeType == EBitmapViewFinder) { // Bitmap ViewFinder
            
        switch (m_viewfinderType) {
            case OutputTypeVideoWidget:
                // Nothing to do
                break;
            case OutputTypeRenderer: {
                S60VideoRendererControl* viewFinderRenderControl =
                    qobject_cast<S60VideoRendererControl*>(m_viewfinderOutput);

                m_viewfinderSurface = viewFinderRenderControl->surface();

                // Set Surface properties
                QVideoSurfaceFormat newFormat(m_viewfinderSize, QVideoFrame::Format_RGB32);
                m_surfaceFormat = newFormat;
                m_surfaceFormat.setFrameSize(m_viewfinderSize);
                m_surfaceFormat.setFrameRate(30);
                m_surfaceFormat.setViewport(QRect(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height()));
                m_surfaceFormat.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_Undefined); // EColor16MU (compatible with EColor16MA)
                m_surfaceFormat.setPixelAspectRatio(1,1); // PAR 1:1
            
                QList<QVideoFrame::PixelFormat> supportedPixelFormats = m_viewfinderSurface->supportedPixelFormats();

                // Reconnect viewfinder frame notification
                disconnect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
                    this, SLOT(viewFinderBitmapReady(const QPixmap &)));
                connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
                    this, SLOT(viewFinderBitmapReady(const QPixmap &)));
                break;
            }
            case OutputTypevideoWindow:
                // Not supported in Symbian
                break;

            default:
                // Not ViewFinder Output has been set, Discard
                break;
        }
    }
}

void CCameraViewfinderEngine::viewFinderBitmapReady(const QPixmap &pixmap)
{
    // Adjust surface size according to ViewFinder if needed
    if(pixmap.size() != m_viewfinderSize) {
        m_viewfinderSize = pixmap.size();
        m_viewfinderSurface->stop();
        m_surfaceFormat.setFrameSize(m_viewfinderSize);
        m_surfaceFormat.setViewport(QRect(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height()));
        m_viewfinderSurface->start(m_surfaceFormat);
    }

    QImage newImage = pixmap.toImage();
    if (newImage.format() != QImage::Format_ARGB32 &&
        newImage.format() != QImage::Format_RGB32) {
        newImage = newImage.convertToFormat(QImage::Format_RGB32);
    }

    if (!newImage.isNull()) {
        QVideoFrame newFrame(newImage);
        if (newFrame.isValid())
            m_viewfinderSurface->present(newFrame);

    }
}

void CCameraViewfinderEngine::handleVisibilityChange(const bool isVisible)
{
    m_isViewFinderVisible = isVisible;

    if (m_isViewFinderVisible) {
        switch (m_vfState) {
            case EVFNotConnectedNotStarted:
            case EVFIsConnectedNotStarted:
            case EVFNotConnectedIsStarted:
            case EVFIsConnectedIsStartedIsVisible:
                // Discard
                break;
            case EVFIsConnectedIsStartedNotVisible:
                m_vfState = EVFIsConnectedIsStartedIsVisible;
                break;
            default:
                emit error(QCamera::CameraError, tr("General viewfinder error."));
                break;
        }
        startViewfinder(true);
    }
    else
        stopViewfinder(true);
}

// End of file
