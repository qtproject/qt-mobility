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

#ifndef S60VIDEOCAPTURESESSION_H
#define S60VIDEOCAPTURESESSION_H

#include <QtCore/qobject.h>
#include <QUrl>
#include <QHash>
#include <qmediaencodersettings.h>
#include <qcamera.h>
#include <s60cameraengine.h>

#include <e32base.h>
#include <VideoRecorder.h>
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/devvideo/devvideobase.h>
#include <mmf/devvideo/devvideorecord.h>
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

QTM_USE_NAMESPACE

struct VideoControllerData
{
    int controllerUid;
    int formatUid;
    QString formatDescription;
};

struct TSupportedFrameRatePictureSize
    {
    qreal frameRate;
    QSize frameSize;
    };

class S60VideoCaptureSession : public QObject, public MVideoRecorderUtilityObserver
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
, public MMMFDevVideoRecordObserver
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
{
    Q_OBJECT
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_ENUMS(Error)
    Q_ENUMS(EcamErrors)
    Q_ENUMS(TVideoCaptureState)
    
public:

    enum Error {
        NoError = 0,
        OutOfMemoryError,
        InUseError,
        NotReadyError,
        UnknownError = -1
    };

    enum EcamErrors {
        KErrECamCameraDisabled = -12100, // The camera has been disabled, hence calls do not succeed
        KErrECamSettingDisabled = -12101, //  This parameter or operation is supported, but presently is disabled.
        KErrECamParameterNotInRange = -12102, //  This value is out of range.
        KErrECamSettingNotSupported = -12103, //  This parameter or operation is not supported.
        KErrECamNotOptimalFocus = -12104 // The optimum focus is lost
    };

    enum TVideoCaptureState
    {
        ENotInitialized = 0,
        EInitializing,
        EInitialized,
        EOpenCompelete,
        ERecording,
        EPaused,
        ERecordComplete
    };

    S60VideoCaptureSession(QObject *parent = 0);
    ~S60VideoCaptureSession();

    void setError(TInt aError);
    QCamera::Error fromSymbianErrorToMultimediaError(int aError);
    void setCameraHandle(CCameraEngine* camerahandle);
    QList<QSize> supportedVideoResolutions();
    QList<QSize> supportedVideoResolutions(const QVideoEncoderSettings &settings);
    QList<qreal> supportedVideoFrameRates();
    QList<qreal> supportedVideoFrameRates(const QVideoEncoderSettings &settings);

    // for mediacontrol
    void startRecording();
    void pauseRecording();
    void stopRecording();
    bool setOutputLocation(const QUrl &sink);
    QUrl outputLocation() const;
    qint64 position();

    QStringList supportedVideoCaptureCodecs();
	QStringList supportedAudioCaptureCodecs();
    QString videoCaptureCodec();
	QString audioCaptureCodec();
    void setVideoCaptureCodec(const QString &codecName);
	void setAudioCaptureCodec(const QString &codecName);
    bool isSupportedVideoCaptureCodec(const QString &codecName);
    int bitrate();
    void setBitrate(const int &bitrate);
    QSize videoResolution();
    void setVideoResolution(const QSize &resolution);
    qreal framerate();
    void setFrameRate(qreal rate);
    QString videoCaptureCodecDescription(const QString &codecName);
    void saveVideoEncoderSettings(QVideoEncoderSettings &videoSettings);
	void saveAudioEncoderSettings(QAudioEncoderSettings &audioSettings);
    void getCurrentVideoEncoderSettings(QVideoEncoderSettings &videoSettings);
	void getCurrentAudioEncoderSettings(QAudioEncoderSettings &audioSettings);
    QtMedia::EncodingQuality videoCaptureQuality() const;
    void setVideoCaptureQuality(QtMedia::EncodingQuality quality);

    QStringList supportedVideoContainers();
    bool isSupportedVideoContainer(const QString &containerName);
    QString videoContainer();
    void setVideoContainer(const QString &containerName);
    QString videoContainerDescription(const QString &containerName);

private:
    bool queryCurrentCameraInfo();
    QMap<QString, int> formatMap();
    QMap<QString, int> formatDescMap();

    // From MVideoRecorderUtilityObserver 
    void MvruoOpenComplete(TInt aError);
    void MvruoPrepareComplete(TInt aError);
    void MvruoRecordComplete(TInt aError);
    void MvruoEvent(const TMMFEvent& aEvent);

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
    // MMMFDevVideoRecordObserver
    void MdvroReturnPicture(TVideoPicture *aPicture);
    void MdvroSupplementalInfoSent();
    void MdvroNewBuffers();
    void MdvroFatalError(TInt aError);
    void MdvroInitializeComplete(TInt aError);
    void MdvroStreamEnd();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

    void updateVideoCaptureContainers();
    void updateVideoCaptureContainersL();
    void commitVideoEncoderSettings();
    void setVideoFrameRateFixed(bool fixed);
    void initializeVideoCaptureSettings();
    void doSetBitrate(const int &bitrate);
    void doSetVideoResolution(const QSize &resolution);
    void doSetFrameRate(qreal rate);
    void doPopulateVideoCodecsDataL();
	void doPopulateVideoCodecsL();
    void doInitializeVideoRecorderL();
    void resetSession();
	void doPopulateAudioCodecsL();

Q_SIGNALS:
    void stateChanged(S60VideoCaptureSession::TVideoCaptureState);
    void positionChanged(qint64 duration);
    void error(int error, const QString &errorString);

private:
    CCameraEngine *m_cameraEngine;
    int m_videoQuality;
    mutable int m_error;
    QUrl m_sink;    
    mutable TCameraInfo m_info; // information about camera
    CVideoRecorderUtility *m_videoRecorder;    
    QVideoEncoderSettings m_videoSettings;
	QAudioEncoderSettings m_audioSettings;    
    TVideoCaptureState m_captureState;
    QString m_container;
    QHash<QString, VideoControllerData> m_videoControllerMap;
    QHash<QString, QList<TSupportedFrameRatePictureSize> > m_videoCodecData;
	QStringList m_videoCodeclist;
    QHash<QString, TFourCC> m_audioCodeclist;    
};

#endif
