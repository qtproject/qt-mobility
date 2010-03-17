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

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>

#include <QMediaRecorder>

#include "s60audiocapturesession.h"

#include <Mda\Common\Audio.h>
#include <Mda\Common\Resource.h>
#include <Mda\Client\Utility.h>
#include <MdaAudioSampleEditor.h>

#include <QDir>

#include <mmf\common\mmfcontrollerpluginresolver.h>
#include <mmf\common\mmfcontroller.h>


#include <BADESCA.H>

S60AudioCaptureSession::S60AudioCaptureSession(QObject *parent):
    QObject(parent), m_recorderUtility(NULL)
{
    TRAPD(err, m_recorderUtility = CMdaAudioRecorderUtility::NewL(*this));
    qWarning() << err;
    
    TRAP(err, fetchAudioCodecsL());
    qWarning() << err;
    
}

S60AudioCaptureSession::~S60AudioCaptureSession()
{
    stop();
    
    if(m_recorderUtility)
        delete m_recorderUtility;
}

QAudioFormat S60AudioCaptureSession::format() const
{
    return m_format;
}

bool S60AudioCaptureSession::isFormatSupported(const QAudioFormat &format) const
{
    return false;
}

bool S60AudioCaptureSession::setFormat(const QAudioFormat &format)
{
	if (m_recorderUtility) {
		m_format = format;
		return true;
	}
    return false;
}

QStringList S60AudioCaptureSession::supportedAudioCodecs() const
{
	return m_controllerIdMap.keys();
}

QString S60AudioCaptureSession::codecDescription(const QString &codecName)
{
    return m_controllerIdMap[codecName].destinationFormatDescription;
}

bool S60AudioCaptureSession::setAudioCodec(const QString &codecName)
{
    if(m_recorderUtility) {
        QStringList codecs = supportedAudioCodecs();
        if(codecs.contains(codecName)) {
            m_format.setCodec(codecName);
            return true;
        }
    }
    return false;
}

QString S60AudioCaptureSession::audioCodec() const
{
    return m_format.codec();
}

QUrl S60AudioCaptureSession::outputLocation() const
{
    return m_sink;
}

bool S60AudioCaptureSession::setOutputLocation(const QUrl& sink)
{
    m_sink = sink;
    return true;
}

qint64 S60AudioCaptureSession::position() const
{
    return m_recorderUtility->Duration().Int64() / 1000;;
}

int S60AudioCaptureSession::state() const
{
    return int(m_state);
}

void S60AudioCaptureSession::record()
{   
    if (m_state == QMediaRecorder::StoppedState) {
        QString filename = QDir::toNativeSeparators(m_sink.toString());
        TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));    
    
        TUid controllerUid(TUid::Uid(m_controllerIdMap[m_format.codec()].controllerUid));
        TUid formatUid(TUid::Uid(m_controllerIdMap[m_format.codec()].destinationFormatUid));
        
        TRAPD(err, m_recorderUtility->OpenFileL(sink, controllerUid, KNullUid, formatUid));
        qWarning() << err;    
        m_state = QMediaRecorder::RecordingState;
        emit stateChanged(m_state);
    }else if (m_state == QMediaRecorder::PausedState) {
        m_recorderUtility->SetPosition(m_pausedPosition);
        TRAPD(error, m_recorderUtility->RecordL());   
        qWarning() << error;
        m_state = QMediaRecorder::RecordingState;
        emit stateChanged(m_state);
    }        
}

void S60AudioCaptureSession::pause()
{
    //TODO:
    /*
    if(m_audioInput)
        m_audioInput->stop();
    */
    m_pausedPosition = m_recorderUtility->Position();
    m_recorderUtility->Stop();

    m_state = QMediaRecorder::PausedState;
    emit stateChanged(m_state);
}

void S60AudioCaptureSession::stop()
{
    if (m_recorderUtility) {
        m_recorderUtility->Stop();
        m_recorderUtility->Close();
    }    
    m_state = QMediaRecorder::StoppedState;
    emit stateChanged(m_state);
}
/*
void S60AudioCaptureSession::stateChanged(QAudio::State state)
{
    switch(state) {
        case QAudio::ActiveState:
            emit stateChanged(QMediaRecorder::RecordingState);
            break;
        default:
            if(!((m_state == QMediaRecorder::PausedState) || (m_state == QMediaRecorder::StoppedState)))
                m_state = QMediaRecorder::StoppedState;

            emit stateChanged(m_state);
            break;
    }
}
*/
void S60AudioCaptureSession::notify()
{
    //TODO:
    //m_position += m_audioInput->notifyInterval();
//    m_position = 0;
//    emit positionChanged(m_position);
}

void S60AudioCaptureSession::setCaptureDevice(const QString &deviceName)
{
    m_captureDevice = deviceName;
}

// Needed observer for CMdaAudioRecorderUtility
void S60AudioCaptureSession::MoscoStateChangeEvent(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt /*aErrorCode*/)
{
	TRAPD(err, MoscoStateChangeEventL(aObject, aPreviousState, aCurrentState, NULL));
    qWarning() << err;
}

// Needed observer for CMdaAudioRecorderUtility
void S60AudioCaptureSession::MoscoStateChangeEventL(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt /*aErrorCode*/)
{
	if (aObject == m_recorderUtility)
    {
		switch(aCurrentState)
		{
            case CMdaAudioClipUtility::EOpen:
            {
                if(aPreviousState == CMdaAudioClipUtility::ENotReady)
                {
					RArray<TUint> supportedSampleRates;
					CleanupClosePushL(supportedSampleRates);
					m_recorderUtility->GetSupportedSampleRatesL(supportedSampleRates);

					int frequency = m_format.frequency();
					for (TInt i = 0; i < supportedSampleRates.Count(); i++ ) {
						TUint supportedSampleRate = supportedSampleRates[i];
						if (supportedSampleRate == frequency) { 
							m_recorderUtility->SetDestinationSampleRateL(m_format.frequency());
							break;
						}
						
					}

					CleanupStack::PopAndDestroy(&supportedSampleRates);
					
					/*
					RArray<TUint> supportedBitRates;
					TRAPD(ignore, m_recorderUtility->GetSupportedBitRatesL(supportedBitRates));
					
					for (TInt j = 0; j < supportedBitRates.Count(); j++ ) {
						qWarning() << QString("Supported bit rate: ") + QString().number(supportedBitRates[j]);
						
					}
					*/
					
					TFourCC fourCC;
					
					if (m_format.sampleSize() == 8) {
						switch (m_format.sampleType()) {
							case QAudioFormat::SignedInt: {
								fourCC.Set(KMMFFourCCCodePCM8);
								break;
							}
							case QAudioFormat::UnSignedInt: {
								fourCC.Set(KMMFFourCCCodePCMU8);
								break;
							}
							case QAudioFormat::Float: 
							case QAudioFormat::Unknown:
							default: {
								fourCC.Set(KMMFFourCCCodePCM8);
								break;
							}
						}
					} else if (m_format.sampleSize() == 16) {
						// 16 bit here
						switch (m_format.sampleType()) {
							case QAudioFormat::SignedInt: {
								fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
									KMMFFourCCCodePCM16B:KMMFFourCCCodePCM16);
								break;
							}
							case QAudioFormat::UnSignedInt: {
								fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
									KMMFFourCCCodePCMU16B:KMMFFourCCCodePCMU16);
								break;
							}
							default: {
								fourCC.Set(KMMFFourCCCodePCM16);
								break;
							}
						}
					}
					
					RArray<TFourCC> supportedDataTypes;
					CleanupClosePushL(supportedDataTypes);
					m_recorderUtility->GetSupportedDestinationDataTypesL(supportedDataTypes);

					for (TInt k = 0; k < supportedDataTypes.Count(); k++ ) {
						if (supportedDataTypes[k].FourCC() == fourCC.FourCC()) {
							m_recorderUtility->SetDestinationDataTypeL(supportedDataTypes[k]);
							break;
						}
					}
					CleanupStack::PopAndDestroy(&supportedDataTypes);
					
					//set channels
					RArray<TUint> supportedChannels;
					CleanupClosePushL(supportedChannels);
					m_recorderUtility->GetSupportedNumberOfChannelsL(supportedChannels);
                    for (TInt l = 0; l < supportedChannels.Count(); l++ ) {
                        if (supportedChannels[l] == m_format.channels()) {
                            m_recorderUtility->SetDestinationNumberOfChannelsL(m_format.channels());
                            break;
                        }
                    }
					
					CleanupStack::PopAndDestroy(&supportedChannels);

					m_recorderUtility->SetGain(m_recorderUtility->MaxGain());
                    m_recorderUtility->RecordL();
                }
                break;
            }
            default:
            {
                break;
            }
		}
	}
}

void S60AudioCaptureSession::fetchAudioCodecsL()
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
	 
	//Get all audio/video play and record controllers/formats that are supported
	pluginParameters->ListImplementationsL(controllers);
	 
	for (TInt index=0; index<controllers.Count(); index++) {
		const RMMFFormatImplInfoArray& recordFormats = 
			controllers[index]->RecordFormats();
		for (TInt j=0; j<recordFormats.Count(); j++) {
			const CDesC8Array& mimeTypes = recordFormats[j]->SupportedMimeTypes();
			TInt count = mimeTypes.Count();
			if (count > 0) {
				TPtrC8 mimeType = mimeTypes[0];
				QString type = QString::fromUtf8((char *)mimeType.Ptr(), mimeType.Length()); 
                // Currently only support for audio/wav due to quality issues.
                if (type == "audio/wav") {
                    ItemData data;
                    data.controllerUid = controllers[index]->Uid().iUid;
                    data.destinationFormatUid = recordFormats[j]->Uid().iUid;
                    data.destinationFormatDescription = QString::fromUtf16(recordFormats[j]->DisplayName().Ptr(),recordFormats[j]->DisplayName().Length()); 
                    m_controllerIdMap.insert(type, data);
                }
			}
		}
	}	
	CleanupStack::PopAndDestroy(4);//controllers, ids, formatParameters, pluginParameters
}

