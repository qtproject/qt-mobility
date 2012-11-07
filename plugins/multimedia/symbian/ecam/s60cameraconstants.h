/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60CAMERACONSTANTS_H
#define S60CAMERACONSTANTS_H

#include <ecam.h>
#include <qcameraimagecapture.h>
#include <qvideoframe.h>
#include <qcamerafocus.h>
#include <qcameraexposure.h>
#include <qcameraimageprocessing.h>

namespace S60CameraConstants
{
//=============================================================================

// GENERAL SETTINGS

const int KDefaultCameraDevice                  = 0;
const int KECamCameraPriority                   = 0;
const int KInactivityTimerTimeout               = 30000;   // msec
const qreal KSymbianFineResolutionFactor        = 100.0;
const qreal KDefaultOpticalZoom                 = 1.0;
const qreal KDefaultDigitalZoom                 = 1.0;
const int KSmoothZoomStep                       = 1;
const QCameraFocus::FocusMode KDefaultFocusMode = QCameraFocus::AutoFocus;
const QCameraExposure::FlashMode KDefaultFlashMode = QCameraExposure::FlashOff;
const QCameraExposure::ExposureMode KDefaultExposureMode = QCameraExposure::ExposureAuto;
const QCameraImageProcessing::WhiteBalanceMode KDefaultWhiteBalanceMode = QCameraImageProcessing::WhiteBalanceAuto;
const int KDefaultProcessingParameter           = 0;
const int KDefaultExposureParameter             = 0;

const QSize KDefaultViewfinderSize              = QSize(320,240);
const TSize KDefaultSizePreview_Normal          = TSize(640,480);
const TSize KDefaultSizePreview_Wide            = TSize(640,360);
const TSize KDefaultSizePreview_CIF             = TSize(352,288);
const TSize KDefaultSizePreview_PAL             = TSize(640,512);
const TSize KDefaultSizePreview_NTSC            = TSize(640,426);
const CCamera::TFormat KDefaultFormatPreview    = CCamera::EFormatFbsBitmapColor16MU;
const int KViewfinderFrameRate                  = 30;
const int KMaxVFErrorsSignalled                 = 3;

//=============================================================================

// IMAGE SETTINGS

const QCameraImageCapture::CaptureDestination KDefaultCaptureDestination = QCameraImageCapture::CaptureToFile;
const QLatin1String KDefaultImageCodec                      = QLatin1String("image/jpeg");
const CCamera::TFormat KDefaultImageFormatPrimaryCam        = CCamera::EFormatExif;
#ifdef SYMBIAN_3_PLATFORM
    const CCamera::TFormat KDefaultImageFormatSecondaryCam  = CCamera::EFormatExif;
    const QSize KDefaultImageResolution                     = QSize(3264, 2448);
#else // Pre-Symbian3 Platforms
    const CCamera::TFormat KDefaultImageFormatSecondaryCam  = CCamera::EFormatFbsBitmapColor64K;
    const QSize KDefaultImageResolution                     = QSize(2048, 1536);
#endif // SYMBIAN_3_PLATFORM
const int KSymbianImageQualityCoefficient       = 25;
const CCamera::TFormat KDefaultBufferCaptureSymbianFormat   = CCamera::EFormatExif;
const QVideoFrame::PixelFormat KDefaultBufferCaptureQtFormat = QVideoFrame::Format_Jpeg;
// This must be divisible by 4 and creater or equal to 8
const int KSnapshotDownScaleFactor                          = 8;
const int KSnapshotMinWidth                                 = 640;
const int KSnapshotMinHeight                                = 360;
const int KJpegQualityVeryLow                               = 40;
const int KJpegQualityLow                                   = 50;
const int KJpegQualityNormal                                = 75;
const int KJpegQualityHigh                                  = 85;
const int KJpegQualityVeryHigh                              = 95;
const int KJpegQualityDefault                               = KJpegQualityHigh;

//=============================================================================

// VIDEO SETTINGS

// * General settings *

// Default container MIME type
const QLatin1String KMimeTypeDefaultContainer   = QLatin1String("video/mp4");
const int KDurationChangedInterval              = 1000; // 1 second

// * Audio Settings *

// Default audio codec MIME type
const QLatin1String KMimeTypeDefaultAudioCodec  = QLatin1String("audio/aac");

// Default audio settings for video recording
const int KDefaultChannelCount                  = -1; // Not Supported on Symbian
const int KDefaultBitRate                       = 32000; // 32kbps
const int KDefaultSampleRate                    = -1; // Not Supported on Symbian

// * Video Settings *

// Default video codec MIME type
#ifdef SYMBIAN_3_PLATFORM
    // H.264: BaselineProfile Level 3.1, Max resolution: 1280x720
    const QLatin1String KMimeTypeDefaultVideoCodec  = QLatin1String("video/H264; profile-level-id=42801F");
#else
    // MPEG-4: Simple Profile, Level 4, Max resolution: 640x480
    const QLatin1String KMimeTypeDefaultVideoCodec  = QLatin1String("video/mp4v-es; profile-level-id=4");
#endif

// Maximum resolutions for encoder MIME Types
// H.263
const QSize KResH263                    = QSize(176,144);
const QSize KResH263_Profile0           = QSize(176,144);
const QSize KResH263_Profile0_Level10   = QSize(176,144);
const QSize KResH263_Profile0_Level20   = QSize(352,288);
const QSize KResH263_Profile0_Level30   = QSize(352,288);
const QSize KResH263_Profile0_Level40   = QSize(352,288);
const QSize KResH263_Profile0_Level45   = QSize(176,144);
const QSize KResH263_Profile0_Level50   = QSize(352,288);
const QSize KResH263_Profile3           = QSize(176,144);
// MPEG-4
const QSize KResMPEG4                   = QSize(176,144);
const QSize KResMPEG4_PLID_1            = QSize(176,144);
const QSize KResMPEG4_PLID_2            = QSize(352,288);
const QSize KResMPEG4_PLID_3            = QSize(352,288);
const QSize KResMPEG4_PLID_4            = QSize(640,480);
const QSize KResMPEG4_PLID_5            = QSize(720,576);
const QSize KResMPEG4_PLID_6            = QSize(1280,720);
const QSize KResMPEG4_PLID_8            = QSize(176,144);
const QSize KResMPEG4_PLID_9            = QSize(176,144);
// H.264 (Baseline Profile, same resolutions apply to Main and High Profile)
const QSize KResH264                    = QSize(176,144);
const QSize KResH264_PLID_42800A        = QSize(176,144);
const QSize KResH264_PLID_42900B        = QSize(176,144);
const QSize KResH264_PLID_42800B        = QSize(352,288);
const QSize KResH264_PLID_42800C        = QSize(352,288);
const QSize KResH264_PLID_42800D        = QSize(352,288);
const QSize KResH264_PLID_428014        = QSize(352,288);
const QSize KResH264_PLID_428015        = QSize(352,288);
const QSize KResH264_PLID_428016        = QSize(640,480);
const QSize KResH264_PLID_42801E        = QSize(640,480);
const QSize KResH264_PLID_42801F        = QSize(1280,720);
const QSize KResH264_PLID_428020        = QSize(1280,720);
const QSize KResH264_PLID_428028        = QSize(1920,1088);

// Maximum framerates for encoder MIME Types
// H.263
const qreal KFrR_H263                   = qreal(15);
const qreal KFrR_H263_Profile0          = qreal(15);
const qreal KFrR_H263_Profile0_Level10  = qreal(15);
const qreal KFrR_H263_Profile0_Level20  = qreal(15);
const qreal KFrR_H263_Profile0_Level30  = qreal(30);
const qreal KFrR_H263_Profile0_Level40  = qreal(30);
const qreal KFrR_H263_Profile0_Level45  = qreal(15);
const qreal KFrR_H263_Profile0_Level50  = qreal(15);
const qreal KFrR_H263_Profile3          = qreal(15);
// MPEG-4
const qreal KFrR_MPEG4                  = qreal(15);
const qreal KFrR_MPEG4_PLID_1           = qreal(15);
const qreal KFrR_MPEG4_PLID_2           = qreal(15);
const qreal KFrR_MPEG4_PLID_3           = qreal(30);
// This is a workaround for a known platform bug
#if (defined(S60_31_PLATFORM) | defined(S60_32_PLATFORM))
    const qreal KFrR_MPEG4_PLID_4       = qreal(15);
#else // All other platforms
    const qreal KFrR_MPEG4_PLID_4       = qreal(30);
#endif // S60 3.1 or 3.2
const qreal KFrR_MPEG4_PLID_5           = qreal(30);
const qreal KFrR_MPEG4_PLID_6           = qreal(30);
const qreal KFrR_MPEG4_PLID_8           = qreal(15);
const qreal KFrR_MPEG4_PLID_9           = qreal(15);
// H.264 (Baseline Profile, same framerates apply to Main and High Profile)
const qreal KFrR_H264                   = qreal(15);
const qreal KFrR_H264_PLID_42800A       = qreal(15);
const qreal KFrR_H264_PLID_42900B       = qreal(15);
const qreal KFrR_H264_PLID_42800B       = qreal(7.5);
const qreal KFrR_H264_PLID_42800C       = qreal(15);
const qreal KFrR_H264_PLID_42800D       = qreal(30);
const qreal KFrR_H264_PLID_428014       = qreal(30);
const qreal KFrR_H264_PLID_428015       = qreal(50);
const qreal KFrR_H264_PLID_428016       = qreal(16.9);
const qreal KFrR_H264_PLID_42801E       = qreal(33.8);
const qreal KFrR_H264_PLID_42801F       = qreal(30);
const qreal KFrR_H264_PLID_428020       = qreal(60);
const qreal KFrR_H264_PLID_428028       = qreal(30);

// Maximum bitrates for encoder MIME Types
// H.263
const int KBiR_H263                     = int(64000);
const int KBiR_H263_Profile0            = int(64000);
const int KBiR_H263_Profile0_Level10    = int(64000);
const int KBiR_H263_Profile0_Level20    = int(128000);
const int KBiR_H263_Profile0_Level30    = int(384000);
const int KBiR_H263_Profile0_Level40    = int(2048000);
const int KBiR_H263_Profile0_Level45    = int(128000);
const int KBiR_H263_Profile0_Level50    = int(4096000);
const int KBiR_H263_Profile3            = int(64000);
// MPEG-4
const int KBiR_MPEG4                    = int(64000);
const int KBiR_MPEG4_PLID_1             = int(64000);
const int KBiR_MPEG4_PLID_2             = int(128000);
const int KBiR_MPEG4_PLID_3             = int(384000);
// This is a workaround for a known platform bug
#if (defined(S60_31_PLATFORM) | defined(S60_32_PLATFORM))
    const int KBiR_MPEG4_PLID_4         = int(2000000);
#else // All other platforms
    const int KBiR_MPEG4_PLID_4         = int(4000000);
#endif // S60 3.1 or 3.2
const int KBiR_MPEG4_PLID_5             = int(8000000);
const int KBiR_MPEG4_PLID_6             = int(12000000);
const int KBiR_MPEG4_PLID_8             = int(64000);
const int KBiR_MPEG4_PLID_9             = int(128000);
// H.264 (Baseline Profile, same bitrates apply to Main and High Profile)
const int KBiR_H264                     = int(64000);
const int KBiR_H264_PLID_42800A         = int(64000);
const int KBiR_H264_PLID_42900B         = int(128000);
const int KBiR_H264_PLID_42800B         = int(192000);
const int KBiR_H264_PLID_42800C         = int(384000);
const int KBiR_H264_PLID_42800D         = int(768000);
const int KBiR_H264_PLID_428014         = int(2000000);
const int KBiR_H264_PLID_428015         = int(4000000);
const int KBiR_H264_PLID_428016         = int(4000000);
const int KBiR_H264_PLID_42801E         = int(10000000);
const int KBiR_H264_PLID_42801F         = int(14000000);
const int KBiR_H264_PLID_428020         = int(20000000);
const int KBiR_H264_PLID_428028         = int(20000000);

} // namespace S60CameraConstants

#endif // S60CAMERACONSTANTS_H
