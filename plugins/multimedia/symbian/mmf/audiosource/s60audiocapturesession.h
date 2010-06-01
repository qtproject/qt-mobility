/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60AUDIOCAPTURESESSION_H
#define S60AUDIOCAPTURESESSION_H

#include <qmobilityglobal.h>
#include <QtCore/qobject.h>
#include <QFile>
#include <QUrl>
#include <QList>
#include <QHash>
#include <QAudioFormat>
#include <qmediarecorder.h>

#include <Mda\Common\Audio.h>
#include <Mda\Common\Resource.h>
#include <Mda\Client\Utility.h>
#include <MdaAudioSampleEditor.h>
#include <mmf\common\mmfutilities.h>

QT_BEGIN_NAMESPACE
struct ControllerData
{
	int controllerUid;
	int destinationFormatUid;
	QString destinationFormatDescription;
	QString fileExtension;
};

struct CodecData
{
    TFourCC fourCC;
    QString codecDescription;
};
QT_END_NAMESPACE

QT_USE_NAMESPACE

class S60AudioCaptureSession : public QObject, public MMdaObjectStateChangeObserver
{
    Q_OBJECT
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_ENUMS(TAudioCaptureState)
public:    
    
    enum TAudioCaptureState
    {
        ENotInitialized = 0,
        EInitializing,
        EInitialized,
        EOpenCompelete,
        ERecording,
        EPaused,
        ERecordComplete
    };
    
    S60AudioCaptureSession(QObject *parent = 0);
    ~S60AudioCaptureSession();

    QAudioFormat format() const;    
    bool setFormat(const QAudioFormat &format);
    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName);
    bool setAudioCodec(const QString &codecName);
    QString audioCodec() const;
    QString audioContainer() const;
    QStringList supportedAudioContainers() const;
    bool setAudioContainer(const QString &containerMimeType); 
    QString audioContainerDescription(const QString &containerName);
    QList<int> supportedAudioSampleRates(const QAudioEncoderSettings &settings) const;
    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl& sink);
    qint64 position() const;    
    void record();
    void pause();
    void stop();
    
private:    
    void initializeSessionL();
    void setError(TInt aError);
    QMediaRecorder::Error fromSymbianErrorToMultimediaError(int error);    
    QString initializeSinkL();
    void updateAudioContainersL();    
    void populateAudioCodecsDataL();
    void retrieveSupportedAudioSampleRatesL();
    void applyAudioSettingsL();
    TFourCC determinePCMFormat();
    void setDefaultSettings();
    void createFileWithHeader(const TPtrC &path);
    // MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState,
            TInt aCurrentState, TInt aErrorCode);
    void MoscoStateChangeEventL(CBase* aObject, TInt aPreviousState,
            TInt aCurrentState, TInt aErrorCode);    

public slots:
    void setCaptureDevice(const QString &deviceName);

Q_SIGNALS:
    void stateChanged(S60AudioCaptureSession::TAudioCaptureState);
    void positionChanged(qint64 position);
    void error(int error, const QString &errorString);

private:
    QString m_container;    
    QString m_captureDevice;
    QUrl m_sink;
    TTimeIntervalMicroSeconds m_pausedPosition;
    CMdaAudioRecorderUtility *m_recorderUtility;
    TAudioCaptureState m_captureState;
    QAudioFormat m_format;    
    QHash<QString, ControllerData> m_controllerIdMap;
    QHash<QString, CodecData>  m_audioCodeclist;
    QList<int> m_supportedSampleRates;    
    int m_error; 
};

#endif // S60AUDIOCAPTURESESSION_H
