/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "s60bitmapviewfinderrenderercontrol.h"
#include "s60videowindowcontrol.h"
#include "s60videowindowdisplay.h"
#include "s60cameraconstants.h"

#ifdef VIDEOOUTPUT_EGL_RENDERER
#include <QTimer>
#include "s60nativewindow.h"
#include "s60videoeglrenderercontrol.h"
#endif

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>

using namespace S60CameraConstants;

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
    m_cameraControl(0),
    m_viewfinderOutput(0),
    m_viewfinderDisplay(0),
    m_viewfinderSurface(0),
    m_wsSession(CCoeEnv::Static()->WsSession()),
    m_screenDevice(*CCoeEnv::Static()->ScreenDevice()),
    m_window(0),
    m_desktopWidget(0),
    m_vfState(EVFNotConnectedNotStarted),
    m_viewfinderSize(KDefaultViewfinderSize),
    m_actualViewFinderSize(KDefaultViewfinderSize),
    m_viewfinderAspectRatio(0.0),
    m_viewfinderType(OutputTypeNotSet),
    m_viewfinderNativeType(EBitmapViewFinder), // Default type
    m_isViewFinderVisible(true),
    m_vfErrorsSignalled(0),
    m_dummyWindow(0)
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
    } else {
        m_cameraControl->setError(KErrGeneral, tr("Unexpected camera error."));
    }
    // From now on it is safe to assume engine exists

#ifdef VIDEOOUTPUT_EGL_RENDERER
    // CCamera provides no API for starting the viewfinder (and therefore
    // creating a viewfinder surface) without passing a native window
    // handle.  We therefore create an invisible window, pass this to
    // CCamera and then extract from it the background surface handle.
	m_dummyWindow = new S60NativeWindow(this);
#endif

    // Detect UI Rotations
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(handleDesktopResize(int)));
}

S60CameraViewfinderEngine::~S60CameraViewfinderEngine()
{
    // No need to stop viewfinder:
    // Engine has stopped it already
    // Surface will be stopped by VideoRendererControl
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

    if (m_viewfinderType == OutputTypeRenderer) {
        QSize newResolution(-1,-1);
        if (m_viewfinderSurface)
            newResolution = m_viewfinderSurface->nativeResolution();

        if (newResolution.width() == -1 || newResolution.height() == -1) {
            QRect screenRect = QApplication::desktop()->screenGeometry();
            newResolution = QSize(screenRect.width(), screenRect.height());
        }
        resetViewfinderSize(newResolution);
    }

    // Rotate Camera if UI has rotated
    m_cameraControl->detectNewUiOrientation();
}

void S60CameraViewfinderEngine::setVideoWidgetControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

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
        connect(m_viewfinderDisplay, SIGNAL(windowHandleChanged(RWindow*)), this, SLOT(handleWindowChange(RWindow*)));

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
            startViewfinder(true, false); // Internal start (i.e. start if started externally)
    }
}

void S60CameraViewfinderEngine::setVideoRendererControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

    QVideoRendererControl *rendererControl =
        qobject_cast<QVideoRendererControl *>(viewfinderOutput);

    if (rendererControl) {
        m_viewfinderNativeType = EBitmapViewFinder;
#ifdef VIDEOOUTPUT_EGL_RENDERER
        if (qobject_cast<S60VideoEglRendererControl *>(rendererControl))
            m_viewfinderNativeType = EDirectScreenViewFinder;
#endif

        connect(rendererControl, SIGNAL(surfaceChanged()),
                this, SLOT(rendererSurfaceChanged()));

        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeRenderer;
        // RendererControl viewfinder is "visible" when surface is set
        m_isViewFinderVisible = false;
        if (m_vfState == EVFIsConnectedIsStartedIsVisible)
            m_vfState = EVFIsConnectedIsStartedNotVisible;

        // Use display resolution as default viewfinder resolution
        m_viewfinderSize = QApplication::desktop()->screenGeometry().size();

        switch (m_vfState) {
        case EVFNotConnectedNotStarted:
            m_vfState = EVFIsConnectedNotStarted;
            break;
        case EVFNotConnectedIsStarted:
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
            startViewfinder(true, false);

        if (rendererControl->surface())
            rendererSurfaceChanged();
    }
}

void S60CameraViewfinderEngine::setVideoWindowControl(QObject *viewfinderOutput)
{
    // Release old control if it has not already been done
    if (m_viewfinderOutput)
        releaseControl(m_viewfinderType);

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
        connect(m_viewfinderDisplay, SIGNAL(windowHandleChanged(RWindow*)), this, SLOT(handleWindowChange(RWindow*)));

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
            startViewfinder(true, false); // Internal start (i.e. start if started externally)
    }
}

void S60CameraViewfinderEngine::releaseControl(ViewfinderOutputType type)
{
    if (m_viewfinderType != type)
        return;

    if (m_vfState == EVFIsConnectedIsStartedIsVisible)
        stopViewfinder(true, false);

    if (m_viewfinderOutput) {
        switch (type) {
        case OutputTypeNotSet:
            return;
        case OutputTypeVideoWidget:
            if (m_viewfinderType != OutputTypeVideoWidget)
                return;
            m_viewfinderDisplay->setHasContent(false);
            disconnect(m_viewfinderOutput);
            m_viewfinderOutput->disconnect(this);
            Q_ASSERT(m_viewfinderDisplay);
            disconnect(m_viewfinderDisplay);
            m_viewfinderDisplay->disconnect(this);
            m_viewfinderDisplay = 0;
            // Invalidate the extent rect
            qobject_cast<S60VideoWidgetControl*>(m_viewfinderOutput)->setExtentRect(QRect());
            break;
        case OutputTypeVideoWindow:
            if (m_viewfinderType != OutputTypeVideoWindow)
                return;
            m_viewfinderDisplay->setHasContent(false);
            disconnect(m_viewfinderOutput);
            m_viewfinderOutput->disconnect(this);
            Q_ASSERT(m_viewfinderDisplay);
            disconnect(m_viewfinderDisplay);
            m_viewfinderDisplay->disconnect(this);
            m_viewfinderDisplay = 0;
            break;
        case OutputTypeRenderer:
            if (m_viewfinderType != OutputTypeRenderer)
                return;
            disconnect(m_viewfinderOutput);
            m_viewfinderOutput->disconnect(this);
            if (m_viewfinderSurface)
                m_viewfinderSurface->disconnect(this);
            disconnect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)),
                this, SLOT(viewFinderBitmapReady(const CFbsBitmap &)));
            break;
        default:
            emit error(QCamera::CameraError, tr("Unexpected viewfinder error."));
            return;
        }
    }

    m_viewfinderOutput = 0;
    m_viewfinderType = OutputTypeNotSet;

    // Update state
    switch (m_vfState) {
    case EVFNotConnectedNotStarted:
    case EVFNotConnectedIsStarted:
        // Do nothing
        break;
    case EVFIsConnectedNotStarted:
        m_vfState = EVFNotConnectedNotStarted;
        break;
    case EVFIsConnectedIsStartedNotVisible:
    case EVFIsConnectedIsStartedIsVisible:
        m_vfState = EVFNotConnectedIsStarted;
        break;
    default:
        emit error(QCamera::CameraError, tr("General viewfinder error."));
        break;
    }
}

void S60CameraViewfinderEngine::startViewfinder(bool internalStart, bool suppressHasContentChanged)
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
            m_window = 0;
            QRect extentRect;
            QRect clipRect;

            if (m_viewfinderDisplay) {
                m_window = m_viewfinderDisplay->windowHandle();
                extentRect = m_viewfinderDisplay->extentRect();
                clipRect = m_viewfinderDisplay->clipRect();
            }
#ifdef VIDEOOUTPUT_EGL_RENDERER
            else if (qobject_cast<S60VideoEglRendererControl *>(m_viewfinderOutput)) {
                m_window = m_dummyWindow->windowHandle();
                QSize windowSize;
                if (m_window)
                    windowSize = QSize(m_window->Size().iWidth, m_window->Size().iHeight);
                extentRect = QRect(QPoint(0, 0), windowSize);
                clipRect = extentRect;
            }
#endif

            if (!m_window) {
                emit error(QCamera::CameraError, tr("Requesting window for viewfinder failed."));
                qWarning("Requesting window for viewfinder failed. Viewfinder may not be visible.");
                return;
            }

            TRect extentRectSymbian = qRect2TRect(extentRect);
            TRect clipRectSymbian = qRect2TRect(clipRect);

            TRAPD(err, m_cameraEngine->StartDirectViewFinderL(m_wsSession, m_screenDevice, *m_window, extentRectSymbian, clipRectSymbian));
            if (err) {
                if (err == KErrNotSupported)
                    emit error(QCamera::NotSupportedFeatureError, tr("Requested viewfinder size is not supported."));
                else
                    emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                return;
            } else {
#ifdef VIDEOOUTPUT_EGL_RENDERER
                if (qobject_cast<S60VideoEglRendererControl *>(m_viewfinderOutput))
                    // Temporary workaround for a bug in the BCM2727 graphics driver
                    // Without this delay, images acquired from the EGL endpoint all
                    // contain the same data, which may be a valid viewfinder frame,
                    // or may be garbage.
                    QTimer::singleShot(500, this, SLOT(setRendererNativeSurface()));
#endif
            }

            m_actualViewFinderSize = QSize(extentRectSymbian.Size().iWidth, extentRectSymbian.Size().iHeight);
            m_viewfinderAspectRatio = qreal(m_actualViewFinderSize.width()) / qreal(m_actualViewFinderSize.height());

        } else { // Bitmap ViewFinder
            TSize size = TSize(m_viewfinderSize.width(), m_viewfinderSize.height());

            // Use 4x4 as check for sensible resolution
            if (m_viewfinderSize.width() < 4 || m_viewfinderSize.height() < 4) {
                emit error(QCamera::NotSupportedFeatureError, tr("Unable to start viewfinder with such small resolution."));
                return;
            }

            if( m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface) {
                if (!m_surfaceFormat.isValid()) {
                    emit error(QCamera::NotSupportedFeatureError, tr("Invalid surface format."));
                    return;
                }

                // Start rendering to surface with correct size and format
                if (!m_viewfinderSurface->isFormatSupported(m_surfaceFormat) ||
                    !m_viewfinderSurface->start(m_surfaceFormat)) {
                    emit error(QCamera::NotSupportedFeatureError, tr("Failed to start surface."));
                    return;
                }

                if (!m_viewfinderSurface->isActive())
                    return;
            }

            TRAPD(vfErr, m_cameraEngine->StartViewFinderL(size));
            if (vfErr) {
                if (vfErr == KErrNotSupported)
                    emit error(QCamera::NotSupportedFeatureError, tr("Requested viewfinder size is not supported."));
                else
                    emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                return;
            }

            m_actualViewFinderSize = QSize(size.iWidth, size.iHeight);
            m_viewfinderAspectRatio = qreal(m_actualViewFinderSize.width()) / qreal(m_actualViewFinderSize.height());

            // Notify control about the frame size (triggers frame position calculation)
            if (m_viewfinderDisplay) {
                m_viewfinderDisplay->setNativeSize(m_actualViewFinderSize);
            } else {
                if (m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface) {
                    QVideoSurfaceFormat format = m_viewfinderSurface->surfaceFormat();
                    m_viewfinderSurface->stop();
                    format.setFrameSize(m_actualViewFinderSize);
                    format.setViewport(QRect(QPoint(0,0), m_actualViewFinderSize));
                    if (format.isValid() && m_viewfinderSurface->start(format)) {
                        m_surfaceFormat = format;
                    } else {
                        // Viewfinder was already started, stop it
                        stopViewfinder(true, true);
                        emit error(QCamera::NotSupportedFeatureError, tr("Failed to restart surface."));
                    }
                }
            }
        }
    }

    if (m_viewfinderDisplay && !suppressHasContentChanged)
        m_viewfinderDisplay->setHasContent(true);
}

void S60CameraViewfinderEngine::setRendererNativeSurface()
{
#ifdef VIDEOOUTPUT_EGL_RENDERER
    S60VideoEglRendererControl *rendererControl =
        qobject_cast<S60VideoEglRendererControl *>(m_viewfinderOutput);
    if (m_window) {
        const QSize windowSize(m_window->Size().iWidth, m_window->Size().iHeight);
        rendererControl->setNativeSize(windowSize);
    }
    rendererControl->setNativeSurface(m_dummyWindow->nativeSurface());
#endif
}

void S60CameraViewfinderEngine::stopViewfinder(const bool internalStop, bool suppressHasContentChanged)
{
    // Stop if viewfinder is started
    if (m_vfState == EVFIsConnectedIsStartedIsVisible) {
        if (m_viewfinderOutput && m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface) {
            // Stop surface if one still exists
            m_viewfinderSurface->stop();
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

    m_window = 0;

    if (m_viewfinderDisplay && !suppressHasContentChanged)
        m_viewfinderDisplay->setHasContent(false);
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

    if (m_vfState != EVFIsConnectedIsStartedIsVisible)
        return;

    stopViewfinder(true, true);
    startViewfinder(true, true);
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
        case OutputTypeVideoWidget:
            if (!m_viewfinderOutput)
                return;

            // First stop viewfinder
            stopViewfinder(true, true);

            if (!m_viewfinderDisplay->windowHandle())
                return;

            // Then start it with the new WindowID
            startViewfinder(true, true);
            break;
        case OutputTypeRenderer:
        case OutputTypeVideoWindow:
            // Do nothing
            break;

        default:
            // Not ViewFinder Output has been set, Discard
            break;
        }
    }
}

void S60CameraViewfinderEngine::rendererSurfaceChanged()
{
    QVideoRendererControl *rendererControl =
        qobject_cast<QVideoRendererControl*>(m_viewfinderOutput);
    Q_ASSERT(rendererControl);

    // Reset old surface if needed
    if (m_viewfinderSurface) {
        handleVisibilityChange(false);
        disconnect(m_viewfinderSurface);
        stopViewfinder(true);
        m_viewfinderSize = QApplication::desktop()->screenGeometry().size();
        m_viewfinderSurface = 0;
    }

    m_viewfinderSurface = rendererControl->surface();
    if (m_viewfinderSurface) {
        // Set new surface
        if (!m_viewfinderSurface->nativeResolution().isEmpty())
            if (m_viewfinderSurface->nativeResolution() != m_viewfinderSize)
                resetViewfinderSize(m_viewfinderSurface->nativeResolution());

        connect(m_viewfinderSurface, SIGNAL(nativeResolutionChanged(const QSize&)),
            this, SLOT(resetViewfinderSize(QSize)));

        // Set Surface Properties
        if (m_viewfinderSurface->supportedPixelFormats().contains(QVideoFrame::Format_RGB32))
            m_surfaceFormat = QVideoSurfaceFormat(m_actualViewFinderSize, QVideoFrame::Format_RGB32);
        else if (m_viewfinderSurface->supportedPixelFormats().contains(QVideoFrame::Format_ARGB32))
            m_surfaceFormat = QVideoSurfaceFormat(m_actualViewFinderSize, QVideoFrame::Format_ARGB32);
        else {
            return;
        }
        m_surfaceFormat.setFrameRate(KViewfinderFrameRate);
        m_surfaceFormat.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_Undefined); // EColor16MU (compatible with EColor16MA)
        m_surfaceFormat.setPixelAspectRatio(1,1); // PAR 1:1

        if (qobject_cast<S60BitmapViewFinderRendererControl *>(rendererControl))
            connect(this, SIGNAL(viewFinderFrameReady(const CFbsBitmap &)),
                this, SLOT(viewFinderBitmapReady(const CFbsBitmap &)));
    }

    handleVisibilityChange(m_viewfinderSurface != 0);
}

void S60CameraViewfinderEngine::viewFinderBitmapReady(const CFbsBitmap &bitmap)
{
    Q_ASSERT(qobject_cast<S60BitmapViewFinderRendererControl *>(m_viewfinderOutput));

    CFbsBitmap *bitmapPtr = const_cast<CFbsBitmap*>(&bitmap);
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(bitmapPtr);

    QImage newImage = pixmap.toImage(); // Deep copy
    if (newImage.format() != QImage::Format_ARGB32 &&
        newImage.format() != QImage::Format_RGB32)
        newImage = newImage.convertToFormat(QImage::Format_RGB32);

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
    if (m_isViewFinderVisible == isVisible)
        return;

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
        startViewfinder(true, true);
    } else {
        // Stopping takes care of the state change
        stopViewfinder(true, true);
    }
}

void S60CameraViewfinderEngine::handleWindowChange(RWindow *handle)
{
    stopViewfinder(true, true);
    if (handle) // New handle available, start viewfinder
        startViewfinder(true, true);
}

void S60CameraViewfinderEngine::handleContentAspectRatioChange(const QSize& newSize)
{
    qreal newAspectRatio = qreal(newSize.width()) / qreal(newSize.height());
    // Check if aspect ratio changed
    if (qFuzzyCompare(newAspectRatio, m_viewfinderAspectRatio)) {
        if (m_viewfinderDisplay)
            m_viewfinderDisplay->setNativeSize(m_actualViewFinderSize);
        return;
    }

    // Resize viewfinder by reducing either width or height to comply with the new aspect ratio
    QSize newNativeResolution;
    if (newAspectRatio > m_viewfinderAspectRatio) // New AspectRatio is wider => Reduce height
        newNativeResolution = QSize(m_actualViewFinderSize.width(),
                                    (m_actualViewFinderSize.width() / newAspectRatio));
    else // New AspectRatio is higher => Reduce width
        newNativeResolution = QSize((m_actualViewFinderSize.height() * newAspectRatio),
                                    m_actualViewFinderSize.height());

    // Notify aspect ratio change (use actual content size to notify that)
    // This triggers item size/position re-calculation
    if (m_viewfinderDisplay)
        m_viewfinderDisplay->setNativeSize(newNativeResolution);
}

// End of file
