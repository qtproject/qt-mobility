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

#include <QApplication>
#include <QDesktopWidget>
#include <qcamera.h>
#include <qabstractvideosurface.h>
#include <qvideoframe.h>

#include "s60cameraviewfinderengine.h"
#include "s60cameraengine.h"
#include "s60cameracontrol.h"
#include "s60videowidgetcontrol.h"
#include "s60videowidgetdisplay.h"
#include "s60videorenderercontrol.h"
#include "s60videowindowcontrol.h"
#include "s60videowindowdisplay.h"
#include "s60cameraconstants.h"

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>

// Helper function
TRect qRect2TRect(const QRect &qr)
{
    return TRect(TPoint(qr.left(), qr.top()), TSize(qr.width(), qr.height()));
}


S60CameraViewfinderEngine::S60CameraViewfinderEngine(S60CameraControl *control,
                                                     CCameraEngine *engine,
                                                     QObject *parent):
    QObject(parent),
    m_cameraEngine(engine),
    m_cameraControl(NULL),
    m_viewfinderOutput(NULL),
    m_viewfinderDisplay(0),
    m_viewfinderSurface(NULL),
    m_wsSession(CCoeEnv::Static()->WsSession()),
    m_screenDevice(*CCoeEnv::Static()->ScreenDevice()),
    m_window(NULL),
    m_desktopWidget(NULL),
    m_vfState(EVFNotConnectedNotStarted),
    m_viewfinderSize(KDefaultViewfinderSize),
    m_actualViewFinderSize(KDefaultViewfinderSize),
    m_viewfinderType(OutputTypeNotSet),
    m_viewfinderNativeType(EBitmapViewFinder), // Default type
    m_isViewFinderVisible(true), // True by default (only QVideoWidgetControl supports being hidden)
    m_uiLandscape(true),
    m_vfErrorsSignalled(0)
{
    m_cameraControl = control;

    // Check whether platform supports DirectScreen ViewFinder
    if (m_cameraEngine) {
        if (m_cameraEngine->IsDirectViewFinderSupported())
            m_viewfinderNativeType = EDirectScreenViewFinder;
        else
            m_viewfinderNativeType = EBitmapViewFinder;

        MCameraViewfinderObserver *vfObserver = this;
        m_cameraEngine->SetViewfinderObserver(vfObserver);
    }
    else
        m_cameraControl->setError(KErrGeneral, tr("Unexpected camera error."));
    // From now on it is safe to assume engine exists

    // Check the UI orientation
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    if (screenRect.width() > screenRect.height())
        m_uiLandscape = true;
    else
        m_uiLandscape = false;

    // Detect UI Rotations
    m_desktopWidget = QApplication::desktop();
    if (m_desktopWidget)
        connect(m_desktopWidget, SIGNAL(resized(int)), this, SLOT(handleDesktopResize(int)));
}

S60CameraViewfinderEngine::~S60CameraViewfinderEngine()
{
    // No need to stop viewfinder:
    // Engine has stopped it already
    // Surface will be stopped by VideoRendererControl

    m_viewfinderOutput = NULL;
    m_viewfinderSurface = NULL;
}

void S60CameraViewfinderEngine::setNewCameraEngine(CCameraEngine *engine)
{
    m_cameraEngine = engine;

    if (m_cameraEngine) {
        // And set observer to the new CameraEngine
        MCameraViewfinderObserver *vfObserver = this;
        m_cameraEngine->SetViewfinderObserver(vfObserver);
    }
}

void S60CameraViewfinderEngine::handleDesktopResize(int screen)
{
    Q_UNUSED(screen);
    // UI Rotation is handled by the QVideoWidgetControl, thus this is needed
    // only for the QVideoRendererControl
    if (m_viewfinderType == OutputTypeRenderer) {
        QSize newResolution(-1,-1);
        if (m_viewfinderSurface)
            newResolution = m_viewfinderSurface->nativeResolution();

        if (newResolution.width() == -1 || newResolution.height() == -1) {
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect screenRect = desktopWidget->screenGeometry();
            newResolution = QSize(screenRect.width(), screenRect.height());
        }

        resetViewfinderSize(newResolution);
    }

    // Rotate Camera if UI has rotated
    checkAndRotateCamera();
}

void S60CameraViewfinderEngine::setVideoWidgetControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

    // Rotate Camera if UI has rotated
    checkAndRotateCamera();

    S60VideoWidgetControl* viewFinderWidgetControl =
        qobject_cast<S60VideoWidgetControl*>(viewfinderOutput);

    if (viewFinderWidgetControl) {
        // Check whether platform supports DirectScreen ViewFinder
        if (m_cameraEngine) {
            if (m_cameraEngine->IsDirectViewFinderSupported())
                m_viewfinderNativeType = EDirectScreenViewFinder;
            else
                m_viewfinderNativeType = EBitmapViewFinder;
        }
        else
            return;

        m_viewfinderDisplay = viewFinderWidgetControl->display();

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {
            m_viewfinderDisplay->setPaintingEnabled(false); // No Qt Painter painting - Direct rendering
            connect(m_viewfinderDisplay, SIGNAL(windowHandleChanged(RWindow *)), this, SLOT(resetViewfinderDisplay()));
        } else {
            m_viewfinderDisplay->setPaintingEnabled(true); // Qt Painter painting - Bitmap rendering
            connect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)), m_viewfinderDisplay, SLOT(setFrame(const CFbsBitmap &)));
        }

        connect(m_viewfinderDisplay, SIGNAL(visibilityChanged(bool)), this, SLOT(handleVisibilityChange(bool)));
        connect(m_viewfinderDisplay, SIGNAL(displayRectChanged(QRect, QRect)), this, SLOT(resetVideoWindowSize()));

        m_viewfinderSize = m_viewfinderDisplay->extentRect().size();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeVideoWidget;
        m_isViewFinderVisible = m_viewfinderDisplay->isVisible();

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
            startViewfinder(true); // Internal start (i.e. start if started externally)
    }
}

void S60CameraViewfinderEngine::setVideoRendererControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

    // Rotate Camera if UI has rotated
    checkAndRotateCamera();

    S60VideoRendererControl* viewFinderRenderControl =
        qobject_cast<S60VideoRendererControl*>(viewfinderOutput);

    if (viewFinderRenderControl) {
        m_viewfinderNativeType = EBitmapViewFinder; // Always Bitmap
        m_isViewFinderVisible = true; // Always visible (in backends point-of-view)

        connect(viewFinderRenderControl, SIGNAL(viewFinderSurfaceSet()),
            this, SLOT(resetViewfinderDisplay()));

        if (viewFinderRenderControl->surface() == NULL) {
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

        connect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)),
            this, SLOT(viewFinderBitmapReady(const CFbsBitmap &)));

        m_viewfinderSurface = viewFinderRenderControl->surface();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeRenderer;

        m_viewfinderSize = m_viewfinderSurface->nativeResolution();
        // Use display size if no native resolution has been set
        if (m_viewfinderSize.width() == -1 || m_viewfinderSize.height() == -1) {
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect screenRect = desktopWidget->screenGeometry();
            m_viewfinderSize = QSize(screenRect.width(), screenRect.height());
        }

        connect(m_viewfinderSurface, SIGNAL(nativeResolutionChanged(const QSize&)),
            this, SLOT(resetViewfinderSize(QSize)));

        // Set Surface Properties
        m_surfaceFormat = m_viewfinderSurface->surfaceFormat();
        m_surfaceFormat.setFrameSize(m_viewfinderSize);
        m_surfaceFormat.setFrameRate(KViewfinderFrameRate);
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
            startViewfinder(true); // Internal start (i.e. start if started externally)
    }
}

void S60CameraViewfinderEngine::setVideoWindowControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

    // Rotate Camera if UI has rotated
    checkAndRotateCamera();

    S60VideoWindowControl* viewFinderWindowControl =
        qobject_cast<S60VideoWindowControl*>(viewfinderOutput);

    if (viewFinderWindowControl) {
        // Check whether platform supports DirectScreen ViewFinder
        if (m_cameraEngine) {
            if (m_cameraEngine->IsDirectViewFinderSupported())
                m_viewfinderNativeType = EDirectScreenViewFinder;
            else
                m_viewfinderNativeType = EBitmapViewFinder;
        } else {
            return;
        }

        m_viewfinderDisplay = viewFinderWindowControl->display();

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {
            m_viewfinderDisplay->setPaintingEnabled(false); // No Qt Painter painting - Direct rendering
            connect(m_viewfinderDisplay, SIGNAL(windowHandleChanged(RWindow *)), this, SLOT(resetViewfinderDisplay()));
        } else {
            m_viewfinderDisplay->setPaintingEnabled(true); // Qt Painter painting - Bitmap rendering
            connect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)), m_viewfinderDisplay, SLOT(setFrame(const CFbsBitmap &)));
        }

        connect(m_viewfinderDisplay, SIGNAL(displayRectChanged(QRect, QRect)), this, SLOT(resetVideoWindowSize()));
        connect(m_viewfinderDisplay, SIGNAL(visibilityChanged(bool)), this, SLOT(handleVisibilityChange(bool)));

        m_viewfinderSize = m_viewfinderDisplay->extentRect().size();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeVideoWindow;
        m_isViewFinderVisible = m_viewfinderDisplay->isVisible();

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
            startViewfinder(true); // Internal start (i.e. start if started externally)
    }
}

void S60CameraViewfinderEngine::releaseControl(ViewfinderOutputType type)
{
    if (m_viewfinderOutput) {
        switch (type) {
        case OutputTypeNotSet:
            return;
        case OutputTypeVideoWidget:
        case OutputTypeVideoWindow:
            disconnect(m_viewfinderOutput);
            m_viewfinderOutput->disconnect(this);
            Q_ASSERT(m_viewfinderDisplay);
            disconnect(m_viewfinderDisplay);
            m_viewfinderDisplay->disconnect(this);
            m_viewfinderDisplay = 0;
            break;
        case OutputTypeRenderer:
            disconnect(m_viewfinderOutput);
            m_viewfinderOutput->disconnect(this);
            if (m_viewfinderSurface)
                disconnect(m_viewfinderSurface, SIGNAL(nativeResolutionChanged(const QSize&)), this, SLOT(resetViewfinderSize(QSize)));
            break;
        default:
            emit error(QCamera::CameraError, tr("Unexpected viewfinder error."));
            return;
        }
    }
    Q_ASSERT(!m_viewfinderDisplay);
    m_viewfinderOutput = 0;
    m_viewfinderType = OutputTypeNotSet;
}

void S60CameraViewfinderEngine::startViewfinder(const bool internalStart)
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

        if (!m_cameraEngine)
            return;

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {

            if (RWindow *window = m_viewfinderDisplay->windowHandle()) {
                m_window = window;
            } else {
                emit error(QCamera::CameraError, tr("Requesting window for viewfinder failed."));
                return;
            }

            const QRect extentRect = m_viewfinderDisplay ? m_viewfinderDisplay->extentRect() : QRect();
            const QRect clipRect = m_viewfinderDisplay ? m_viewfinderDisplay->clipRect() : QRect();

            TRect extentRectSymbian = qRect2TRect(extentRect);
            TRect clipRectSymbian = qRect2TRect(clipRect);
            TRAPD(err, m_cameraEngine->StartDirectViewFinderL(m_wsSession, m_screenDevice, *m_window, extentRectSymbian, clipRectSymbian));
            if (err) {
                if (err == KErrNotSupported) {
                    emit error(QCamera::NotSupportedFeatureError, tr("Requested viewfinder size is not supported."));
                } else {
                    emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                }
                return;
            }

            m_actualViewFinderSize = QSize(extentRectSymbian.Size().iWidth, extentRectSymbian.Size().iHeight);

        } else { // Bitmap ViewFinder
            TSize size = TSize(m_viewfinderSize.width(), m_viewfinderSize.height());

            if( m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface) {
                // Start rendering to surface with correct size and format
                if (m_viewfinderSurface->isFormatSupported(m_surfaceFormat))
                    m_viewfinderSurface->start(m_surfaceFormat);
                else {
                    emit error(QCamera::NotSupportedFeatureError, tr("Viewfinder format not supported."));
                    return;
                }
            }

            TRAPD(vfErr, m_cameraEngine->StartViewFinderL(size));
            if (vfErr) {
                if (vfErr == KErrNotSupported) {
                    emit error(QCamera::NotSupportedFeatureError, tr("Requested viewfinder size is not supported."));
                } else {
                    emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                }
                return;
            }

            if (size.iWidth != m_viewfinderSize.width() || size.iHeight != m_viewfinderSize.height())
                m_actualViewFinderSize = QSize(size.iWidth, size.iHeight);

                // Notify control about the different than requested frame size
                if (m_viewfinderDisplay)
                    m_viewfinderDisplay->setNativeSize(m_actualViewFinderSize);
        }
    }
}

void S60CameraViewfinderEngine::stopViewfinder(const bool internalStop)
{
    // Stop if viewfinder is started
    if (m_vfState == EVFIsConnectedIsStartedIsVisible) {
        if (m_viewfinderType == OutputTypeRenderer) {
            // Stop surface if one still exists
            if (m_viewfinderOutput) {
                if (m_viewfinderSurface)
                    m_viewfinderSurface->stop();
            }
        }

        if (m_cameraEngine)
            m_cameraEngine->StopViewFinder();
    }

    // Update state
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

void S60CameraViewfinderEngine::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{

    emit viewFinderFrameReady(aFrame);
    if (m_cameraEngine)
        m_cameraEngine->ReleaseViewFinderBuffer();
}

void S60CameraViewfinderEngine::resetViewfinderSize(const QSize size)
{
    m_viewfinderSize = size;

    if(m_vfState != EVFIsConnectedIsStartedIsVisible)
        return;

    stopViewfinder(true);

    startViewfinder(true);
}

void S60CameraViewfinderEngine::resetVideoWindowSize()
{
    if (m_viewfinderDisplay)
        resetViewfinderSize(m_viewfinderDisplay->extentRect().size());
}

void S60CameraViewfinderEngine::resetViewfinderDisplay()
{
    if (m_viewfinderNativeType == EDirectScreenViewFinder) {

        switch (m_viewfinderType) {
        case OutputTypeVideoWidget: {
            if (!m_viewfinderOutput)
                return;

            // First stop viewfinder
            stopViewfinder(true);

            RWindow *window = m_viewfinderDisplay->windowHandle();
            if (!window) {
                return;
            }

            // Then start it with the new WindowID
            startViewfinder(true);
            break;
        }
        case OutputTypeRenderer:
        case OutputTypeVideoWindow:
            // Do nothing
            break;

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
            if (!m_viewfinderSurface) {
                m_viewfinderSurface = NULL;
                stopViewfinder(); // Stop viewfinder
                return;
            }
            connect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)),
                this, SLOT(viewFinderBitmapReady(const CFbsBitmap &)));
        }
            break;
        case OutputTypeVideoWindow:
            // Do nothing
            break;

        default:
            // Not ViewFinder Output has been set, Discard
            break;
        }
    }
}

void S60CameraViewfinderEngine::viewFinderBitmapReady(const CFbsBitmap &bitmap)
{
    CFbsBitmap *bitmapPtr = const_cast<CFbsBitmap*>(&bitmap);
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(bitmapPtr);
    // Adjust surface size according to ViewFinder frame size if needed
    if(m_surfaceFormat.frameSize() != pixmap.size()) {
        m_viewfinderSurface->stop();
        m_surfaceFormat.setFrameSize(pixmap.size());
        m_surfaceFormat.setViewport(QRect(0, 0, pixmap.size().width(), pixmap.size().height()));
        m_viewfinderSurface->start(m_surfaceFormat);
    }

    QImage newImage = pixmap.toImage();
    if (newImage.format() != QImage::Format_ARGB32 &&
        newImage.format() != QImage::Format_RGB32) {
        newImage = newImage.convertToFormat(QImage::Format_RGB32);
    }

    if (!newImage.isNull()) {
        QVideoFrame newFrame(newImage);
        if (newFrame.isValid()) {
            if (!m_viewfinderSurface->present(newFrame)) {
                // Presenting may fail even if there are no errors (e.g. busy)
                if (m_viewfinderSurface->error()) {
                    if (m_vfErrorsSignalled < KMaxVFErrorsSignalled) {
                        emit error(QCamera::CameraError, tr("Presenting viewfinder frame failed."));
                        ++m_vfErrorsSignalled;
                    }
                }
            }
        } else {
            if (m_vfErrorsSignalled < KMaxVFErrorsSignalled) {
                emit error(QCamera::CameraError, tr("Invalid viewfinder frame was received."));
                ++m_vfErrorsSignalled;
            }
        }

    } else {
        if (m_vfErrorsSignalled < KMaxVFErrorsSignalled) {
            emit error(QCamera::CameraError, tr("Failed to convert viewfinder frame to presentable image."));
            ++m_vfErrorsSignalled;
        }
    }
}

void S60CameraViewfinderEngine::handleVisibilityChange(const bool isVisible)
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
    } else {
        if (m_vfState == EVFIsConnectedIsStartedIsVisible)
            m_vfState = EVFIsConnectedIsStartedNotVisible;
        stopViewfinder(true);
    }
}

void S60CameraViewfinderEngine::checkAndRotateCamera()
{
    bool isUiNowLandscape = false;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();

    if (screenRect.width() > screenRect.height())
        isUiNowLandscape = true;
    else
        isUiNowLandscape = false;

    // Rotate camera if possible
    if (isUiNowLandscape != m_uiLandscape) {
        stopViewfinder(true);

        // Request orientation reset
        m_cameraControl->resetCameraOrientation();
    }
    m_uiLandscape = isUiNowLandscape;
}

// End of file
