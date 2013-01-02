/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "s60audiocapturesession.h"
#include "s60mmtrace.h"
#include <QtCore/qurl.h>
#include <QDir>
#include <mda/common/audio.h>
#include <mda/common/resource.h>
#include <mda/client/utility.h>
#include <mdaaudiosampleeditor.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/common/mmfcontroller.h>
#include <badesca.h>
#include <bautils.h>
#include <f32file.h>

#ifdef AUDIOINPUT_ROUTING
const QString S60AudioCaptureSession::microPhone("Microphone");
const QString S60AudioCaptureSession::voiceCall("Voice Call");
const QString S60AudioCaptureSession::fmRadio("FM Radio");
#endif

S60AudioCaptureSession::S60AudioCaptureSession(QObject *parent)
    : QObject(parent)
    , m_recorderUtility(0)
    , m_captureState(ENotInitialized)
    , m_controllerIdMap(QHash<QString, ControllerData>())
    , m_audioCodeclist(QHash<QString, CodecData>())
    , m_error(QMediaRecorder::NoError)
    , m_isMuted(false)
{
    TRACE("S60AudioCaptureSession::S60AudioCaptureSession" << qtThisPtr());
#ifdef AUDIOINPUT_ROUTING
    m_audioInput = 0;
    m_setActiveEndPoint = false;
    m_audioEndpoint = S60AudioCaptureSession::microPhone;
#endif //AUDIOINPUT_ROUTING
    TRAPD(err, initializeSessionL());
    setError(err);
}

void S60AudioCaptureSession::initializeSessionL()
{
    m_recorderUtility = CMdaAudioRecorderUtility::NewL(*this, 0, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
    updateAudioContainersL();
    populateAudioCodecsDataL();
    setDefaultSettings();
#ifdef AUDIOINPUT_ROUTING
    initAudioInputs();
#endif
    User::LeaveIfError(m_fsSession.Connect());
    m_captureState = EInitialized;
    emit stateChanged(m_captureState);
}

void S60AudioCaptureSession::setError(TInt aError)
{
    if (aError == KErrNone)
        return;

    m_error = aError;
    QMediaRecorder::Error recorderError = fromSymbianErrorToMultimediaError(m_error);

    TRACE("S60AudioCaptureSession::setError" << qtThisPtr()
          << "error" << m_error << "recorderError" << recorderError);

    // TODO: fix to user friendly string at some point
    // These error string are only dev usable
    QString symbianError;
    symbianError.append("Symbian:");
    symbianError.append(QString::number(m_error));
    stop();
    emit error(recorderError, symbianError);
}

QMediaRecorder::Error S60AudioCaptureSession::fromSymbianErrorToMultimediaError(int error)
{
    switch(error) {
    case KErrNoMemory:
    case KErrNotFound:
    case KErrBadHandle:
    case KErrAbort:
    case KErrCorrupt:
    case KErrGeneral:
    case KErrPathNotFound:
    case KErrUnknown:
    case KErrNotReady:
    case KErrInUse:
    case KErrAccessDenied:
    case KErrLocked:
    case KErrPermissionDenied:
    case KErrAlreadyExists:
        return QMediaRecorder::ResourceError;
    case KErrNotSupported:
    case KErrArgument:
        return QMediaRecorder::FormatError;
    case KErrNone:
    default:
        return QMediaRecorder::NoError;
    }
}

S60AudioCaptureSession::~S60AudioCaptureSession()
{
    TRACE("S60AudioCaptureSession::~S60AudioCaptureSession" << qtThisPtr());
    stop();
    if (m_recorderUtility)
        delete m_recorderUtility;
    m_fsSession.Close();
}

QAudioFormat S60AudioCaptureSession::format() const
{
    return m_format;
}

bool S60AudioCaptureSession::setFormat(const QAudioFormat &format)
{
    if (format != m_format) {
        TRACE("S60AudioCaptureSession::setFormat" << qtThisPtr()
              << "sampleRate" << format.sampleRate()
              << "channelCount" << format.channelCount()
              << "sampleSize" << format.sampleSize()
              << "codec" << format.codec()
              << "byteOrder" << format.byteOrder()
              << "sampleType" << format.sampleType());
        m_format = format;
    }
    return true;
}

QAudioEncoderSettings S60AudioCaptureSession::settings() const
{
    return m_audioEncoderSettings;
}

bool S60AudioCaptureSession::setEncoderSettings(const QAudioEncoderSettings &settings)
{
    if (settings != m_audioEncoderSettings) {
        TRACE("S60AudioCaptureSession::setEncoderSettings" << qtThisPtr()
              << "encodingMode" << settings.encodingMode()
              << "codec" << settings.codec()
              << "bitRate" << settings.bitRate()
              << "channelCount" << settings.channelCount()
              << "sampleRate" << settings.sampleRate()
              << "quality" << settings.quality());
        m_audioEncoderSettings = settings;
    }
    return true;
}

QStringList S60AudioCaptureSession::supportedAudioCodecs() const
{
    return m_audioCodeclist.keys();
}

QStringList S60AudioCaptureSession::supportedAudioContainers() const
{
    return m_controllerIdMap.keys();
}

QString S60AudioCaptureSession::codecDescription(const QString &codecName)
{
    if (m_audioCodeclist.keys().contains(codecName))
        return m_audioCodeclist.value(codecName).codecDescription;
    else
        return QString();
}

QString S60AudioCaptureSession::audioContainerDescription(const QString &containerName)
{
    if (m_controllerIdMap.keys().contains(containerName))
        return m_controllerIdMap.value(containerName).destinationFormatDescription;
    else
        return QString();
}

bool S60AudioCaptureSession::setAudioCodec(const QString &codecName)
{
    TRACE("S60AudioCaptureSession::setAudioCodec" << qtThisPtr()
          << "codec" << codecName);
    QStringList codecs = supportedAudioCodecs();
    if (codecs.contains(codecName)) {
        m_format.setCodec(codecName);
        return true;
    }
    return false;
}

bool S60AudioCaptureSession::setAudioContainer(const QString &containerMimeType)
{
    TRACE("S60AudioCaptureSession::setAudioContainer" << qtThisPtr()
          << "mimeType" << containerMimeType);
    QStringList containers = supportedAudioContainers();
    if (containerMimeType == "audio/mpeg") {
        m_container = "audio/mp4";
        return true;
    }
    if (containers.contains(containerMimeType)) {
        m_container = containerMimeType;
        return true;
    }
    return false;
}

QString S60AudioCaptureSession::audioCodec() const
{
    return m_format.codec();
}

QString S60AudioCaptureSession::audioContainer() const
{
    return m_container;
}

QUrl S60AudioCaptureSession::outputLocation() const
{
    return m_sink;
}

bool S60AudioCaptureSession::setOutputLocation(const QUrl &sink)
{
    TRACE("S60AudioCaptureSession::setOutputLocation" << qtThisPtr()
          << "location" << sink);
    QString filename = QDir::toNativeSeparators(sink.toString());
    TPtrC16 path(reinterpret_cast<const TUint16*>(filename.utf16()));
    TRAPD(err, BaflUtils::EnsurePathExistsL(m_fsSession,path));
    if (err == KErrNone) {
        m_sink = sink;
        setError(err);
        return true;
    } else {
        setError(err);
        return false;
    }
}

qint64 S60AudioCaptureSession::position() const
{
    if ((m_captureState != ERecording) || !m_recorderUtility)
        return 0;
    return m_recorderUtility->Duration().Int64() / 1000;
}

void S60AudioCaptureSession::prepareSinkL()
{
    TRACE("S60AudioCaptureSession::prepareSinkL" << qtThisPtr());

    /* If m_outputLocation is null, set a default location */
    if (m_sink.isEmpty()) {
        QDir outputDir(QDir::rootPath());
        int lastImage = 0;
        int fileCount = 0;
        foreach (QString fileName, outputDir.entryList(QStringList() << "recordclip_*")) {
            int imgNumber = fileName.mid(5, fileName.size() - 9).toInt();
            lastImage = qMax(lastImage, imgNumber);
            if (outputDir.exists(fileName))
                fileCount += 1;
        }
        lastImage += fileCount;
        m_sink = QUrl(QDir::toNativeSeparators(outputDir.canonicalPath()
                      + QString("/recordclip_%1").arg(lastImage + 1, 4, 10, QLatin1Char('0'))));
    }

    QString sink = QDir::toNativeSeparators(m_sink.toString());
    TPtrC16 path(reinterpret_cast<const TUint16*>(sink.utf16()));
    if (BaflUtils::FileExists(m_fsSession, path))
        BaflUtils::DeleteFile(m_fsSession, path);

    int index = sink.lastIndexOf('.');
    if (index != -1)
        sink.chop(sink.length()-index);

    sink.append(m_controllerIdMap.value(m_container).fileExtension);
    m_sink.setUrl(sink);
}

void S60AudioCaptureSession::record()
{
    TRACE("S60AudioCaptureSession::record" << qtThisPtr());

    if (!m_recorderUtility)
        return;

    if (m_captureState == EInitialized || m_captureState == ERecordComplete) {
        TRAPD(err, prepareSinkL());
        if (!err) {
            QString filename = m_sink.toString();
            TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));
            TUid controllerUid(TUid::Uid(m_controllerIdMap.value(m_container).controllerUid));
            TUid formatUid(TUid::Uid(m_controllerIdMap.value(m_container).destinationFormatUid));
            TRAP(err, m_recorderUtility->OpenFileL(sink));
        }
        setError(err);
    } else if (m_captureState == EPaused) {
        m_recorderUtility->SetPosition(m_pausedPosition);
        TRAPD(error, m_recorderUtility->RecordL());
        setError(error);
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
    }
}

void S60AudioCaptureSession::mute(bool muted)
{
    TRACE("S60AudioCaptureSession::mute" << qtThisPtr() << "muted" << muted);
    if (!m_recorderUtility)
        return;
    if (muted)
        m_recorderUtility->SetGain(0);
    else
        m_recorderUtility->SetGain(m_recorderUtility->MaxGain());
    m_isMuted = muted;
}

bool S60AudioCaptureSession::muted()
{
    return m_isMuted;
}

void S60AudioCaptureSession::setDefaultSettings()
{
    TRACE("S60AudioCaptureSession::setDefaultSettings" << qtThisPtr());

    // Setting AMR to default format if supported
    if (m_controllerIdMap.count() > 0) {
        if (m_controllerIdMap.contains("audio/amr"))
            m_container = QString("audio/amr");
        else
            m_container = m_controllerIdMap.keys()[0];
    }
    if (m_audioCodeclist.keys().count() > 0) {
        if (m_audioCodeclist.keys().contains("AMR")) {
            m_format.setSampleSize(8);
            m_format.setChannels(1);
            m_format.setFrequency(8000);
            m_format.setSampleType(QAudioFormat::SignedInt);
            m_format.setCodec("AMR");
        } else {
            m_format.setCodec(m_audioCodeclist.keys()[0]);
        }
    }
}

void S60AudioCaptureSession::pause()
{
    TRACE("S60AudioCaptureSession::pause" << qtThisPtr());
    if (!m_recorderUtility)
        return;
    m_pausedPosition = m_recorderUtility->Position();
    m_recorderUtility->Stop();
    m_captureState = EPaused;
    emit stateChanged(m_captureState);
}

void S60AudioCaptureSession::stop()
{
    TRACE("S60AudioCaptureSession::stop" << qtThisPtr());
    if (!m_recorderUtility)
        return;
    m_recorderUtility->Stop();
#ifdef AUDIOINPUT_ROUTING
    if (m_audioInput) {
        delete m_audioInput;
        m_audioInput = 0;
    }
#endif
    m_recorderUtility->Close();
    m_captureState = ERecordComplete;
    emit stateChanged(m_captureState);
}

#ifdef AUDIOINPUT_ROUTING

void S60AudioCaptureSession::initAudioInputs()
{
    TRACE("S60AudioCaptureSession::initAudioInputs" << qtThisPtr());
    m_audioInputs[S60AudioCaptureSession::microPhone] = QString("Microphone associated with the currently active speaker.");
    m_audioInputs[S60AudioCaptureSession::voiceCall] = QString("Audio stream associated with the current phone call.");
    m_audioInputs[S60AudioCaptureSession::fmRadio] = QString("Audio of the currently tuned FM radio station.");
}

#endif

void S60AudioCaptureSession::setActiveEndpoint(const QString& audioEndpoint)
{
    TRACE("S60AudioCaptureSession::setActiveEndpoint" << qtThisPtr()
          << "endpoint" << audioEndpoint);
    if (!m_audioInputs.keys().contains(audioEndpoint))
        return;
    if (activeEndpoint().compare(audioEndpoint) != 0) {
        m_audioEndpoint = audioEndpoint;
#ifdef AUDIOINPUT_ROUTING
        m_setActiveEndPoint = true;
#endif
    }
}

QList<QString> S60AudioCaptureSession::availableEndpoints() const
{
    return m_audioInputs.keys();
}

QString S60AudioCaptureSession::endpointDescription(const QString& name) const
{
    if (m_audioInputs.keys().contains(name))
        return m_audioInputs.value(name);
    return QString();
}

QString S60AudioCaptureSession::activeEndpoint() const
{
    QString inputSourceName;
#ifdef AUDIOINPUT_ROUTING
    if (m_audioInput) {
        CAudioInput::TAudioInputArray input = m_audioInput->AudioInput();
        inputSourceName = qStringFromTAudioInputPreference(input[0]);
    }
#endif
    return inputSourceName;
}

QString S60AudioCaptureSession::defaultEndpoint() const
{
#ifdef AUDIOINPUT_ROUTING
    return QString(S60AudioCaptureSession::microPhone);
#else
    return QString();
#endif
}

#ifdef AUDIOINPUT_ROUTING

void S60AudioCaptureSession::doSetAudioInputL(const QString &name)
{
    TRACE("S60AudioCaptureSession::doSetAudioInputL" << qtThisPtr()
          << "name" << name);
    TInt err = KErrNone;

    if (!m_recorderUtility)
        return;

    CAudioInput::TAudioInputPreference input = CAudioInput::EDefaultMic;

    if (name.compare(S60AudioCaptureSession::voiceCall) == 0)
        input = CAudioInput::EVoiceCall;
//    commented because they are not supported on 9.2
    else if (name.compare(S60AudioCaptureSession::fmRadio) == 0)
        input = CAudioInput::EFMRadio;
    else // S60AudioCaptureSession::microPhone
        input = CAudioInput::EDefaultMic;

    RArray<CAudioInput::TAudioInputPreference> inputArray;
    inputArray.Append(input);

    if (m_audioInput){
        TRAP(err,m_audioInput->SetAudioInputL(inputArray.Array()));
        if (err == KErrNone)
            emit activeEndpointChanged(name);
        else
            setError(err);
    }
    inputArray.Close();
}

QString S60AudioCaptureSession::qStringFromTAudioInputPreference(CAudioInput::TAudioInputPreference input) const
{
    if (input == CAudioInput::EVoiceCall)
        return S60AudioCaptureSession::voiceCall;
    else if (input == CAudioInput::EFMRadio)
        return S60AudioCaptureSession::fmRadio;
    else
        return S60AudioCaptureSession::microPhone; // CAudioInput::EDefaultMic
}
#endif //AUDIOINPUT_ROUTING

void S60AudioCaptureSession::MoscoStateChangeEvent(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
{
    TRACE("S60AudioCaptureSession::MoscoStateChangeEvent" << qtThisPtr()
          << "prevState" << aPreviousState << "currentState" << aCurrentState
          << "error" << aErrorCode);
    if (aErrorCode==KErrNone) {
	    TRAPD(err, MoscoStateChangeEventL(aObject, aPreviousState, aCurrentState, NULL));
	    setError(err);
	} else {
        setError(aErrorCode);
    }
}

void S60AudioCaptureSession::MoscoStateChangeEventL(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
{
	if (aObject != m_recorderUtility)
	    return;
    switch(aCurrentState) {
    case CMdaAudioClipUtility::EOpen: {
        if (aPreviousState == CMdaAudioClipUtility::ENotReady) {
            applyAudioSettingsL();
            m_recorderUtility->SetGain(m_recorderUtility->MaxGain());
            TRAPD(err, m_recorderUtility->RecordL());
            setError(err);
            m_captureState = EOpenCompelete;
            emit stateChanged(m_captureState);
        }
        break;
    }
    case CMdaAudioClipUtility::ENotReady:
        m_captureState = EInitialized;
        emit stateChanged(m_captureState);
        break;
    case CMdaAudioClipUtility::ERecording:
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
        break;
    default:
        break;
    }
}

void S60AudioCaptureSession::updateAudioContainersL()
{
    CMMFControllerPluginSelectionParameters* pluginParameters =
    	CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParameters =
		CMMFFormatSelectionParameters::NewLC();

	pluginParameters->SetRequiredRecordFormatSupportL(*formatParameters);

	RArray<TUid> ids;
	CleanupClosePushL(ids);
	User::LeaveIfError(ids.Append(KUidMediaTypeAudio));

	pluginParameters->SetMediaIdsL(ids,
		CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);

	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);

	//Get all audio record controllers/formats that are supported
	pluginParameters->ListImplementationsL(controllers);

	for (TInt index=0; index<controllers.Count(); ++index) {
		const RMMFFormatImplInfoArray& recordFormats =
			controllers[index]->RecordFormats();
		for (TInt j=0; j<recordFormats.Count(); ++j) {
			const CDesC8Array& mimeTypes = recordFormats[j]->SupportedMimeTypes();
			const CDesC8Array& fileExtensions = recordFormats[j]->SupportedFileExtensions();
			TInt mimeCount = mimeTypes.Count();
			TInt fileExtCount = fileExtensions.Count();

			if (mimeCount > 0 && fileExtCount > 0) {
                TPtrC8 extension = fileExtensions[0];
                TPtrC8 mimeType = mimeTypes[0];
                QString type = QString::fromUtf8((char *)mimeType.Ptr(), mimeType.Length());

                if (type != "audio/basic") {
                    ControllerData data;
                    data.controllerUid = controllers[index]->Uid().iUid;
                    data.destinationFormatUid = recordFormats[j]->Uid().iUid;
                    data.destinationFormatDescription = QString::fromUtf16(
                            recordFormats[j]->DisplayName().Ptr(),
                            recordFormats[j]->DisplayName().Length());
                    data.fileExtension = QString::fromUtf8((char *)extension.Ptr(), extension.Length());
                    m_controllerIdMap[type] = data;
				}
			}
		}
	}
	CleanupStack::PopAndDestroy(4);//controllers, ids, formatParameters, pluginParameters
}

void S60AudioCaptureSession::retrieveSupportedAudioSampleRatesL()
{
    if (!m_recorderUtility)
        return;
    m_supportedSampleRates.clear();
    RArray<TUint> supportedSampleRates;
    CleanupClosePushL(supportedSampleRates);
    m_recorderUtility->GetSupportedSampleRatesL(supportedSampleRates);
    for (TInt j=0; j<supportedSampleRates.Count(); ++j)
        m_supportedSampleRates.append(supportedSampleRates[j]);
    CleanupStack::PopAndDestroy(&supportedSampleRates);
}

QList<int> S60AudioCaptureSession::supportedAudioSampleRates(const QAudioEncoderSettings &settings) const
{
    QList<int> supportedSampleRates;
    if (!settings.codec().isEmpty()) {
        if (settings.codec() == "AMR")
            supportedSampleRates.append(8000);
        else
            supportedSampleRates = m_supportedSampleRates;
    } else {
        supportedSampleRates = m_supportedSampleRates;
    }
    return supportedSampleRates;
}

void S60AudioCaptureSession::populateAudioCodecsDataL()
{
    if (!m_recorderUtility)
        return;
    if (m_controllerIdMap.contains("audio/amr")) {
        CodecData data;
        data.codecDescription = QString("GSM AMR Codec");
        m_audioCodeclist[QString("AMR")] = data;
    }
    if (m_controllerIdMap.contains("audio/basic")) {
        CodecData data;
        data.fourCC = KMMFFourCCCodeALAW;
        data.codecDescription = QString("Sun/Next ""Au"" audio codec");
        m_audioCodeclist[QString("AULAW")] = data;
    }
    if (m_controllerIdMap.contains("audio/wav")) {
        CodecData data;
        data.fourCC = KMMFFourCCCodePCM16;
        data.codecDescription = QString("Pulse code modulation");
        m_audioCodeclist[QString("PCM")] = data;
    }
    if (m_controllerIdMap.contains("audio/mp4")) {
        CodecData data;
        data.fourCC = KMMFFourCCCodeAAC;
        data.codecDescription = QString("Advanced Audio Codec");
        m_audioCodeclist[QString("AAC")] = data;
    }
    // default samplerates
    m_supportedSampleRates << 96000 << 88200 << 64000 << 48000 << 44100 << 32000 << 24000 << 22050 << 16000 << 12000 << 11025 << 8000;
}

void S60AudioCaptureSession::applyAudioSettingsL()
{
    TRACE("S60AudioCaptureSession::applyAudioSettingsL" << qtThisPtr());
    if (!m_recorderUtility)
        return;

#ifdef AUDIOINPUT_ROUTING
    //CAudioInput needs to be re-initialized every time recording starts
    if (m_audioInput) {
        delete m_audioInput;
        m_audioInput = 0;
    }

    if (m_setActiveEndPoint) {
        m_audioInput = CAudioInput::NewL(*m_recorderUtility);
        doSetAudioInputL(m_audioEndpoint);
    }
#endif

    if (m_format.codec() == "AMR")
        return;

    TFourCC fourCC = m_audioCodeclist.value(m_format.codec()).fourCC;

    if (m_format.codec() == "PCM")
        fourCC = determinePCMFormat();

    RArray<TFourCC> supportedDataTypes;
    CleanupClosePushL(supportedDataTypes);
    TRAP_IGNORE(m_recorderUtility->GetSupportedDestinationDataTypesL(supportedDataTypes));
    TInt num = supportedDataTypes.Count();
    if (num > 0) {
        supportedDataTypes.SortUnsigned();
        int index = supportedDataTypes.Find(fourCC.FourCC());
        if (index != KErrNotFound)
            TRAP_IGNORE(m_recorderUtility->SetDestinationDataTypeL(supportedDataTypes[index]));
    }

    supportedDataTypes.Reset();
    CleanupStack::PopAndDestroy(&supportedDataTypes);

    if (m_recorderUtility->DestinationSampleRateL() != m_format.frequency()) {
        RArray<TUint> supportedSampleRates;
        CleanupClosePushL(supportedSampleRates);
        m_recorderUtility->GetSupportedSampleRatesL(supportedSampleRates);
        for (TInt i=0; i<supportedSampleRates.Count(); ++i) {
            TUint supportedSampleRate = supportedSampleRates[i];
            if (supportedSampleRate == m_format.frequency()) {
                m_recorderUtility->SetDestinationSampleRateL(m_format.frequency());
                break;
            }
        }
        supportedSampleRates.Reset();
        CleanupStack::PopAndDestroy(&supportedSampleRates);
    }

    /* If requested channel setting is different than current one */
    if (m_recorderUtility->DestinationNumberOfChannelsL() != m_format.channels()) {
        RArray<TUint> supportedChannels;
        CleanupClosePushL(supportedChannels);
        m_recorderUtility->GetSupportedNumberOfChannelsL(supportedChannels);
        for (TInt l=0; l < supportedChannels.Count(); ++l) {
            if (supportedChannels[l] == m_format.channels()) {
                m_recorderUtility->SetDestinationNumberOfChannelsL(m_format.channels());
                break;
            }
        }
        supportedChannels.Reset();
        CleanupStack::PopAndDestroy(&supportedChannels);
    }

    if (!(m_format.codec().compare("pcm",Qt::CaseInsensitive) == 0)) {
        if (m_recorderUtility->DestinationBitRateL() != m_audioEncoderSettings.bitRate()) {
            RArray<TUint> supportedBitRates;
            CleanupClosePushL(supportedBitRates);
            m_recorderUtility->GetSupportedBitRatesL(supportedBitRates);
            for (TInt l=0; l<supportedBitRates.Count(); ++l) {
                if (supportedBitRates[l] == m_audioEncoderSettings.bitRate()) {
                    m_recorderUtility->SetDestinationBitRateL(m_audioEncoderSettings.bitRate());
                    break;
                }
            }
            supportedBitRates.Reset();
            CleanupStack::PopAndDestroy(&supportedBitRates);
        }
    }
}

TFourCC S60AudioCaptureSession::determinePCMFormat()
{
    TFourCC fourCC;
    if (m_format.sampleSize() == 8) {
        // 8 bit
        switch (m_format.sampleType()) {
        case QAudioFormat::SignedInt:
            fourCC.Set(KMMFFourCCCodePCM8);
            break;
        case QAudioFormat::UnSignedInt:
            fourCC.Set(KMMFFourCCCodePCMU8);
            break;
        case QAudioFormat::Float:
        case QAudioFormat::Unknown:
        default:
            fourCC.Set(KMMFFourCCCodePCM8);
            break;
        }
    } else if (m_format.sampleSize() == 16) {
        // 16 bit
        switch (m_format.sampleType()) {
        case QAudioFormat::SignedInt:
            fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
                KMMFFourCCCodePCM16B:KMMFFourCCCodePCM16);
            break;
        case QAudioFormat::UnSignedInt:
            fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
                KMMFFourCCCodePCMU16B:KMMFFourCCCodePCMU16);
            break;
        default:
            fourCC.Set(KMMFFourCCCodePCM16);
            break;
        }
    }
    return fourCC;
}
