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

#include <private/qcore_symbian_p.h>

#include <BADESCA.H>

S60AudioCaptureSession::S60AudioCaptureSession(QObject *parent):
    QObject(parent), m_recorderUtility(NULL)
{
    TRAPD(err, m_recorderUtility = CMdaAudioRecorderUtility::NewL(*this));
    qWarning() << err;
    
    //TODO:
    /*
    m_position = 0;
    m_state = QMediaRecorder::StoppedState;

    m_format.setFrequency(8000);
    m_format.setChannels(1);
    m_format.setSampleSize(8);
    m_format.setSampleType(QAudioFormat::UnSignedInt);
    //m_format.setCodec(QString("audio/pcm"));
    wavFile = false;
    */
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
    //TODO:
    /*
    if(m_recorderUtility) {
        return m_recorderUtility->isFormatSupported(format);
    }
    */
    return false;
}

bool S60AudioCaptureSession::setFormat(const QAudioFormat &format)
{
	if (m_recorderUtility) {
		
		TRAPD(err, m_recorderUtility->SetDestinationSampleRateL(format.frequency()));
		qWarning() << err;
		
		TRAP(err, m_recorderUtility->SetDestinationBitRateL(format.sampleSize()));
		qWarning() << err;

		//set channels 
		TRAP(err, m_recorderUtility->SetDestinationNumberOfChannelsL(format.channels()));
		qWarning() << err;

		return true;
	}
    //TODO:
    /*
    if(m_recorderUtility) {
        if(m_recorderUtility->isFormatSupported(format)) {
            m_format = format;
            if(m_audioInput) delete m_audioInput;
            m_audioInput = 0;
            QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
            for(int i=0; i < devices.size(); i++) {
                if(qstrcmp(m_recorderUtility->deviceName().toLocal8Bit().constData(),
                            devices.at(i).deviceName().toLocal8Bit().constData()) == 0) {
                    m_audioInput = new QAudioInput(devices.at(i), m_format);
                    connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(stateChanged(QAudio::State)));
                    connect(m_audioInput, SIGNAL(notify()), this, SLOT(notify()));
                    break;
                }
            }
        }
    }
    */
    return false;
}

QStringList S60AudioCaptureSession::supportedAudioCodecs() const
{
	QStringList list;
	
	TRAPD(err, list = supportedAudioCodecsL());
	qWarning() << err;
	
	return list;
}

QStringList S60AudioCaptureSession::supportedAudioCodecsL() const
{
    QStringList list;

    CMMFControllerPluginSelectionParameters* pluginParameters = 
    	CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParameters = 
		CMMFFormatSelectionParameters::NewLC();
	 
	pluginParameters->SetRequiredRecordFormatSupportL(*formatParameters);
	 
	RArray<TUid> ids;
	User::LeaveIfError(ids.Append(KUidMediaTypeAudio));  

	pluginParameters->SetMediaIdsL(ids, 
		CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	 
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
				list << QString::fromUtf8((char *)mimeType.Ptr(), mimeType.Length());
			}
		}
	}
	 
	CleanupStack::PopAndDestroy(3);//controllers, formatParameters, pluginParameters

    return list;
}

QString S60AudioCaptureSession::codecDescription(const QString &codecName)
{
	QString description;

	TRAPD(err, description = codecDescriptionL(codecName));
	qWarning() << err;
	
    return description;
}


QString S60AudioCaptureSession::codecDescriptionL(const QString &codecName)
{
	QString description;
	
    CMMFControllerPluginSelectionParameters* pluginParameters = 
    	CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParameters = 
		CMMFFormatSelectionParameters::NewLC();
	 
	pluginParameters->SetRequiredRecordFormatSupportL(*formatParameters);
	 
	RArray<TUid> ids;
	User::LeaveIfError(ids.Append(KUidMediaTypeAudio));  

	pluginParameters->SetMediaIdsL(ids, 
		CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	 
	RMMFControllerImplInfoArray controllers; 
	CleanupResetAndDestroyPushL(controllers);
	 
	//Get all audio/video play and record controllers/formats that are supported
	pluginParameters->ListImplementationsL(controllers);
	 
	for (TInt index=0; index<controllers.Count(); index++) {
		const RMMFFormatImplInfoArray& recordFormats = 
			controllers[index]->RecordFormats();
		for (TInt j=0; j<recordFormats.Count(); j++) {
			const CDesC8Array& mimetypes = 
					recordFormats[j]->SupportedMimeTypes();
			TInt count = mimetypes.Count();
			for (TInt i=0; i<count; i++) {
				TPtrC8 mimeType = mimetypes[0];
				if (QString::fromUtf8((char *)mimeType.Ptr(), 
						mimeType.Length()) == codecName)
					description = 
						qt_TDesC2QString(recordFormats[j]->DisplayName());
			}
		}
	}
	 
	CleanupStack::PopAndDestroy(3);//controllers, formatParameters, pluginParameters

    return description;
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
    //TODO:
    return QString(); //m_format.codec();
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
    return m_position;
}

int S60AudioCaptureSession::state() const
{
    return int(m_state);
}

void S60AudioCaptureSession::record()
{    
    QString filename = QDir::toNativeSeparators(m_sink.toString());
    TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));    
    TRAPD(err, m_recorderUtility->OpenFileL(sink));
    qWarning() << err;

    //TODO:
    /*
    if(!m_audioInput) {
        setFormat(m_format);
    }

    if(m_audioInput) {
        if(m_state == QMediaRecorder::StoppedState) {
            if(file.open(QIODevice::WriteOnly)) {
                //TODO: write wav header id wavFile true

                memset(&header,0,sizeof(CombinedHeader));
                memcpy(header.riff.descriptor.id,"RIFF",4);
                header.riff.descriptor.size = 0xFFFFFFFF; // This should be updated on stop() TODO! filesize-8
                memcpy(header.riff.type,"WAVE",4);
                memcpy(header.wave.descriptor.id,"fmt ",4);
                header.wave.descriptor.size = 16;
                header.wave.audioFormat = 1; // for PCM data
                header.wave.numChannels = m_format.channels();
                header.wave.sampleRate = m_format.frequency();
                header.wave.byteRate = m_format.frequency()*m_format.channels()*m_format.sampleSize()/8;
                header.wave.blockAlign = m_format.channels()*m_format.sampleSize()/8;
                header.wave.bitsPerSample = m_format.sampleSize();
                memcpy(header.data.descriptor.id,"data",4);
                header.data.descriptor.size = 0xFFFFFFFF; // This should be updated on stop() TODO! samples*channels*sampleSize/8
                file.write((char*)&header,sizeof(CombinedHeader));

                m_audioInput->start(qobject_cast<QIODevice*>(&file));
            } else {
                qWarning()<<"can't open source, failed";
                m_state = QMediaRecorder::StoppedState;
                emit stateChanged(m_state);
            }
        }
    }
    */

    m_state = QMediaRecorder::RecordingState;
}

void S60AudioCaptureSession::pause()
{
    //TODO:
    /*
    if(m_audioInput)
        m_audioInput->stop();
    */

    m_state = QMediaRecorder::PausedState;
}

void S60AudioCaptureSession::stop()
{
    if (m_recorderUtility)
    {
        m_recorderUtility->Stop();
        m_recorderUtility->Close();
    }
    
    //TODO:
    /*
    if(m_audioInput) {
        m_audioInput->stop();
        file.close();
        
        QFile tmpFile("record.tmp");
        tmpFile.open(QIODevice::WriteOnly);

        qint32 fileSize = file.size()-8;
        if(file.open(QIODevice::ReadOnly)) {
            file.read((char*)&header,sizeof(CombinedHeader));
            header.riff.descriptor.size = fileSize; // filesize-8
            header.data.descriptor.size = fileSize*m_format.channels()*m_format.sampleSize()/8; // samples*channels*sampleSize/8
            tmpFile.write((char*)&header,sizeof(CombinedHeader));
            char buf[4096];
            while(!file.atEnd()) {
                int l = file.read(buf,4096);
                if(l > 0)
                    tmpFile.write(buf,l);
            }
            tmpFile.close();
            file.close();
            file.remove();
            tmpFile.rename(file.fileName());
        }

        m_position = 0;
    }
    */
    
    m_state = QMediaRecorder::StoppedState;
}

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

void S60AudioCaptureSession::notify()
{
    //TODO:
    //m_position += m_audioInput->notifyInterval();
    m_position = 0;
    emit positionChanged(m_position);
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



