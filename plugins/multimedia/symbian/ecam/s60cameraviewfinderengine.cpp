/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qcamera.h>
#include <qabstractvideosurface.h>
#include <qvideoframe.h>

#include "s60cameraviewfinderengine.h"
#include "s60cameraengine.h"
#include "s60cameracontrol.h"
#include "s60videowidgetcontrol.h"
#include "s60videorenderercontrol.h"
#include "s60cameraconstants.h"

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>

S60CameraViewfinderEngine::S60CameraViewfinderEngine(QObject *parent, CCameraEngine *engine) :
    QObject(parent),
    m_cameraEngine(engine),
    m_cameraControl(NULL),
    m_viewfinderOutput(NULL),
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
    if (parent) {
        // Check parent is of proper type (S60CameraControl)
        if (qstrcmp(parent->metaObject()->className(), "S60CameraControl") == 0)
            m_cameraControl = qobject_cast<S60CameraControl*>(parent);
        else {
            Q_ASSERT(true);
        }
    }
    else
        Q_ASSERT(true);
    // From now on it is safe to assume camera control exists

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
        m_cameraControl->setError(KErrGeneral, QString("Unexpected camera error."));
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
        if (m_viewfinderSurface) {
            newResolution = m_viewfinderSurface->nativeResolution();
        }

        if (newResolution.width() == -1 || newResolution.height() == -1) {
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect screenRect = desktopWidget->screenGeometry();
            newResolution = QSize(screenRect.width(), screenRect.height());
        }

        resetViewfinderSize(newResolution); // This handles also Camera rotation if needed
    }
}

void S60CameraViewfinderEngine::setVideoWidgetControl(QObject *viewfinderOutput)
{
    if (m_viewfinderOutput) {
        if (m_viewfinderType == OutputTypeRenderer) {
            disconnect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
                this, SLOT(viewFinderBitmapReady(const QPixmap &)));
        }
    }

    S60VideoWidgetControl* viewFinderWidgetControl =
            qobject_cast<S60VideoWidgetControl*>(viewfinderOutput);

    if (viewFinderWidgetControl) {
        QLabel *widget = qobject_cast<QLabel *>(viewFinderWidgetControl->videoWidget());

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {
            viewFinderWidgetControl->reconfigureWidget(true); // Reconfigure to be Direct
            connect(viewFinderWidgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetViewfinderDisplay()));
        } else {
            viewFinderWidgetControl->reconfigureWidget(false); // Reconfigure to be Bitmap
            connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
        }

        connect(viewFinderWidgetControl, SIGNAL(widgetVisible(bool)), this, SLOT(handleVisibilityChange(bool)));
        connect(viewFinderWidgetControl, SIGNAL(widgetResized(QSize)), this, SLOT(resetViewfinderSize(QSize)));

        m_viewfinderSize = widget->size();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeVideoWidget;
        m_isViewFinderVisible = widget->isVisible();

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
    if (m_viewfinderOutput) {
        if (m_viewfinderType == OutputTypeVideoWidget) {
            QWidget *widget = qobject_cast<QVideoWidgetControl*>(m_viewfinderOutput)->videoWidget();
            disconnect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
        }
    }

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

        connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
            this, SLOT(viewFinderBitmapReady(const QPixmap &)));

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
    Q_UNUSED(viewfinderOutput);
    emit error(QCamera::NotSupportedFeatureError, tr("Viewfinder output not supported."));
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

        if (m_viewfinderNativeType == EDirectScreenViewFinder) {

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

                // Check also that rect is entirely inside display
                QDesktopWidget* desktopWidget = QApplication::desktop();
                QRect screenRect = desktopWidget->screenGeometry();

                // Top Left
                if (screenRect.topLeft().x() > qRect.topLeft().x())
                    qRect.setTopLeft(QPoint(screenRect.topLeft().x(), qRect.topLeft().y()));
                if (screenRect.topLeft().y() > qRect.topLeft().y())
                    qRect.setTopLeft(QPoint(qRect.topLeft().x(), screenRect.topLeft().y()));
                // Bottom Right

                if (screenRect.bottomRight().x() < qRect.bottomRight().x())
                    qRect.setBottomRight(QPoint(screenRect.bottomRight().x(), qRect.bottomRight().y()));
                if (screenRect.bottomRight().y() < qRect.bottomRight().y())
                    qRect.setBottomRight(QPoint(qRect.bottomRight().x(), screenRect.bottomRight().y()));
                TRect viewfinderRect(TPoint(qRect.x(),qRect.y()), TSize(qRect.width(),qRect.height()));
#endif // USING_NGA

                TInt err = KErrNone;
                TRAP(err, m_cameraEngine->StartDirectViewFinderL(m_wsSession, m_screenDevice, *window, viewfinderRect));
                if (err) {
                    if (err == KErrNotSupported) {
                        emit error(QCamera::NotSupportedFeatureError, tr("Requested viewfinder size is not supported."));
                    } else {
                        emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
                    }
                    return;
                }

                QSize actualSize((viewfinderRect.iBr.iX - viewfinderRect.iTl.iX),
                    (viewfinderRect.iBr.iY - viewfinderRect.iTl.iY));
                if (actualSize.width() != m_viewfinderSize.width() || actualSize.width() != m_viewfinderSize.height()) {
                    m_actualViewFinderSize = actualSize;
                }
            } else {
                emit error(QCamera::CameraError, tr("Requesting window for viewfinder failed."));
                return;
            }

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

            if (size.iWidth != m_viewfinderSize.width() || size.iHeight != m_viewfinderSize.height()) {
                m_actualViewFinderSize = QSize(size.iWidth, size.iHeight);
            }
        }
    }
}

void S60CameraViewfinderEngine::stopViewfinder(const bool internalStop)
{
    // Stop if viewfinder is started
    if (m_vfState == EVFIsConnectedIsStartedIsVisible) {
        if(m_viewfinderType == OutputTypeRenderer) {
            // Stop surface if one still exists
            if (m_viewfinderOutput) {
                if (m_viewfinderSurface) {
                    m_viewfinderSurface->stop();
                }
            }
        }

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
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(&aFrame);

    emit viewFinderFrameReady(pixmap);
    m_cameraEngine->ReleaseViewFinderBuffer();
}

void S60CameraViewfinderEngine::resetViewfinderSize(QSize size)
{
    if (m_viewfinderSize == size) {
        return;
    }

    m_viewfinderSize = size;

    if(m_vfState != EVFIsConnectedIsStartedIsVisible) {
        return;
    }

    stopViewfinder(true);

    bool isUiNowLandscape = false;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    if (screenRect.width() > screenRect.height())
        isUiNowLandscape = true;
    else
        isUiNowLandscape = false;
    // Rotate camera if possible
    if (isUiNowLandscape != m_uiLandscape) {
        if (m_cameraEngine->IsCameraReady()) {
            m_cameraEngine = m_cameraControl->resetCameraOrientation();
            if (m_cameraEngine == NULL) {
                m_cameraControl->setError(KErrGeneral, tr("Camera rotation failed."));
                return;
            }
        }
    }
    m_uiLandscape = isUiNowLandscape;

    startViewfinder(true);
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
                if (!m_viewfinderSurface) {
                    m_viewfinderSurface = NULL;
                    stopViewfinder(); // Stop viewfinder
                    return;
                }

                // Set Surface properties
                QVideoSurfaceFormat newFormat(m_actualViewFinderSize, QVideoFrame::Format_RGB32);
                m_surfaceFormat = newFormat;
                m_surfaceFormat.setFrameSize(m_actualViewFinderSize);
                m_surfaceFormat.setFrameRate(30);
                m_surfaceFormat.setViewport(QRect(0, 0, m_actualViewFinderSize.width(), m_actualViewFinderSize.height()));
                m_surfaceFormat.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_Undefined); // EColor16MU (compatible with EColor16MA)
                m_surfaceFormat.setPixelAspectRatio(1,1); // PAR 1:1

                QList<QVideoFrame::PixelFormat> supportedPixelFormats = m_viewfinderSurface->supportedPixelFormats();

                // Start surface if viewfinder is active
                if (m_vfState == EVFIsConnectedIsStartedIsVisible) {
                    if (m_viewfinderSurface->isFormatSupported(m_surfaceFormat))
                        m_viewfinderSurface->start(m_surfaceFormat);
                }

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

void S60CameraViewfinderEngine::viewFinderBitmapReady(const QPixmap &pixmap)
{
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
                    emit error(QCamera::CameraError, tr("Presenting viewfinder frame failed."));
                }

            }
        } else {
            emit error(QCamera::CameraError, tr("Invalid viewfinder frame was received."));
        }

    } else {
        emit error(QCamera::CameraError, tr("Failed to convert viewfinder frame to presentable image."));
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
    }
    else
        stopViewfinder(true);
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
