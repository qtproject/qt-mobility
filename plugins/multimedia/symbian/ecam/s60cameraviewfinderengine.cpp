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
#ifdef SYMBIAN_3_PLATFORM
    m_wsSession(CCoeEnv::Static()->WsSession()),
    m_screenDevice(*CCoeEnv::Static()->ScreenDevice()),
    m_window(NULL),
#endif
    m_viewfinderSize(QSize(320,240)),   // Default VF size (if no VF output is attached)
    m_viewfinderType(OutputTypeNotSet)
{
    if (m_cameraEngine) {
        MCameraViewfinderObserver *vfObserver = this;
        m_cameraEngine->SetViewfinderObserver(vfObserver);
    }
}

CCameraViewfinderEngine::~CCameraViewfinderEngine()
{
    m_viewfinderOutput = NULL;
    m_viewfinderSurface = NULL;
}

void CCameraViewfinderEngine::setVideoWidgetControl(QObject *viewfinderOutput)
{
    S60VideoWidgetControl* viewFinderWidgetControl =
            qobject_cast<S60VideoWidgetControl*>(viewfinderOutput);

    if (viewFinderWidgetControl) {
        QLabel *widget = qobject_cast<QLabel *>(viewFinderWidgetControl->videoWidget());
        connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
        connect(viewFinderWidgetControl, SIGNAL(widgetResized(QSize)), this, SLOT(resetViewfinderSize(QSize)));
        
#ifdef SYMBIAN_3_PLATFORM
        disconnect(viewFinderWidgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetViewfinderDisplay()));
        connect(viewFinderWidgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetViewfinderDisplay()));
#endif // SYMBIAN_3_PLATFORM
        
        m_viewfinderSize = widget->size();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeVideoWidget;
    }
}

void CCameraViewfinderEngine::setVideoRendererControl(QObject *viewfinderOutput)
{
    S60VideoRendererControl* viewFinderRenderControl = 
        qobject_cast<S60VideoRendererControl*>(viewfinderOutput);

    if (viewFinderRenderControl) {
        if (viewFinderRenderControl->surface() == NULL) {
            
            connect(viewFinderRenderControl, SIGNAL(viewFinderSurfaceSet()), 
                this, SLOT(resetViewfinderDisplay()));
    
            m_viewfinderOutput = viewfinderOutput;
            m_viewfinderType = OutputTypeRenderer;

            // Use display resolution as default viewfinder resolution
            QDesktopWidget* desktopWidget = QApplication::desktop();
            QRect screenRect = desktopWidget->screenGeometry();
            m_viewfinderSize = QSize(screenRect.width(), screenRect.height());
            
            return;
        }
        
        connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)),
            this, SLOT(viewFinderBitmapReady(const QPixmap &)));
        connect(viewFinderRenderControl, SIGNAL(viewFinderSurfaceSet()), 
            this, SLOT(resetViewfinderDisplay()));
        
        
        m_viewfinderSurface = viewFinderRenderControl->surface();
        m_viewfinderOutput = viewfinderOutput;
        m_viewfinderType = OutputTypeRenderer;
                
        // Use display resolution as default viewfinder resolution
        QDesktopWidget* desktopWidget = QApplication::desktop();
        QRect screenRect = desktopWidget->screenGeometry();
        m_viewfinderSize = QSize(screenRect.width(), screenRect.height());
        
        // Set Surface properties
        m_surfaceFormat = m_viewfinderSurface->surfaceFormat();
        m_surfaceFormat.setFrameSize(m_viewfinderSize);
        m_surfaceFormat.setFrameRate(30);
        m_surfaceFormat.setViewport(QRect(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height()));
        m_surfaceFormat.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_Undefined); // EColor16MU (compatible with EColor16MA)
        m_surfaceFormat.setPixelAspectRatio(1,1); // PAR 1:1
    }
}

void CCameraViewfinderEngine::setVideoWindowControl(QObject *viewfinderOutput)
{
    Q_UNUSED(viewfinderOutput);
    emit error(QCamera::NotSupportedFeatureError, tr("Viewfinder output not supported."));
}

#ifdef SYMBIAN_3_PLATFORM
void CCameraViewfinderEngine::startViewfinderL()
{
    TRect viewfinderSize(0, 0, m_viewfinderSize.width(), m_viewfinderSize.height());

    if (!m_window) {
        resetViewfinderDisplay();
        if (!m_window) {
            return;
        }
    }

    TInt err = KErrNone;
    TRAP(err, m_cameraEngine->StartDirectViewFinderL(m_wsSession, m_screenDevice, *m_window, viewfinderSize));
    if (err) {
        emit error(QCamera::CameraError, tr("Starting viewfinder failed."));
    }
}
#else //!SYMBIAN_3_PLATFORM
void CCameraViewfinderEngine::startViewfinderL()
{
    if (!m_viewfinderOutput) {
        // Discard
        return;
    }
    
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
}
#endif // SYMBIAN_3_PLATFORM

void CCameraViewfinderEngine::stopViewfinder()
{
    if( m_viewfinderType == OutputTypeRenderer && m_viewfinderSurface)
        m_viewfinderSurface->stop();
    
    m_cameraEngine->StopViewFinder();
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
    
    if(!m_cameraEngine->IsCameraReady()) {
        return;
    }
    
    m_cameraEngine->StopViewFinder();

    TInt err = KErrNone;
    TRAP(err, startViewfinderL());
    if (err) {
        emit error(QCamera::CameraError, tr("Restarting viewfinder failed."));
    }
}

#ifdef SYMBIAN_3_PLATFORM
void CCameraViewfinderEngine::resetViewfinderDisplay()
{
    switch (m_viewfinderType) {
        case OutputTypeVideoWidget: {
            S60VideoWidgetControl* viewFinderWidgetControl = 
                qobject_cast<S60VideoWidgetControl*>(m_viewfinderOutput);
            
            QLabel *widget = qobject_cast<QLabel *>(viewFinderWidgetControl->videoWidget());
            if (!widget) {
                return;
            }
            WId widgetWId = widget->effectiveWinId();
            if (!widgetWId) {
                return;
            }
            m_window = widgetWId->DrawableWindow();
            break;
        }
            
        default:
            break;
    }
}
# else // Pre Symbian3 Platforms
void CCameraViewfinderEngine::resetViewfinderDisplay()
{
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
#endif // SYMBIAN_3_PLATFORM

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

    if (!newImage.isNull()) {
        QVideoFrame newFrame(newImage);
        if (newFrame.isValid())
            m_viewfinderSurface->present(newFrame);

    }
}

// End of file
