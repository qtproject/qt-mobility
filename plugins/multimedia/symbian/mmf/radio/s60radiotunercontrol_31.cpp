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

#include "s60radiotunercontrol_31.h"
#include "s60radiotunerservice.h"

#include <QtCore/qdebug.h>
#include <QFile>

// from AudioPreference.h
const TInt KAudioPriorityFMRadio = 79;
const TUint KAudioPrefRadioAudioEvent = 0x03000001;

S60RadioTunerControl::S60RadioTunerControl(QObject *parent)
    : QRadioTunerControl(parent)
    , m_error(0)
    , m_tunerState(0)
    , m_apiTunerState(QRadioTuner::StoppedState)
    , m_audioInitializationComplete(false)
    , m_radioError(QRadioTuner::NoError)
    , m_muted(false)
    , m_isStereo(true)
    , m_stereoMode(QRadioTuner::Auto)
    , m_signal(0)
    , m_currentBand(QRadioTuner::FM)
    , m_currentFreq(87500000)
    , m_scanning(false)
    , m_vol(100)
{
    initRadio();   
}

S60RadioTunerControl::~S60RadioTunerControl()
{
	if (m_tunerUtility) {
	    m_tunerUtility->Close();
		m_tunerUtility->CancelNotifyChange();
		m_tunerUtility->CancelNotifySignalStrength();
		m_tunerUtility->CancelNotifyStereoChange();
		delete m_tunerUtility;
	}
	if (m_audioPlayerUtility) {
		m_audioPlayerUtility = NULL;
	}
}

bool S60RadioTunerControl::initRadio()
{
	m_available = false;

	TRAPD(tunerError, m_tunerUtility = CMMTunerUtility::NewL(*this, CMMTunerUtility::ETunerBandFm, 1, 
												CMMTunerUtility::ETunerAccessPriorityNormal));
	if (tunerError != KErrNone) {
        m_radioError = QRadioTuner::OpenError;
        return m_available;
    }
	
	TRAPD(playerError, m_audioPlayerUtility = m_tunerUtility->TunerPlayerUtilityL(*this));
	if (playerError != KErrNone) {
		m_radioError = QRadioTuner::OpenError;
		return m_available;
	}
	
	TRAPD(initializeError, m_audioPlayerUtility->InitializeL(KAudioPriorityFMRadio, 
												TMdaPriorityPreference(KAudioPrefRadioAudioEvent)));
	if (initializeError != KErrNone) {
		m_radioError = QRadioTuner::OpenError;
		return m_available;
	}
		
	m_tunerUtility->NotifyChange(*this);
	m_tunerUtility->NotifyStereoChange(*this);
	m_tunerUtility->NotifySignalStrength(*this);
	setVolume(m_audioPlayerUtility->MaxVolume()/2);
	
	TFrequency freq(m_currentFreq);
	m_tunerUtility->Tune(freq);
		
	m_available = true;
	
    return m_available;
}

void S60RadioTunerControl::start()
{
	if (!m_audioInitializationComplete) {
		TFrequency freq(m_currentFreq);
		m_tunerUtility->Tune(freq);
	} else {
		m_audioPlayerUtility->Play();
	}

	m_apiTunerState = QRadioTuner::ActiveState;
	emit stateChanged(m_apiTunerState);
}

void S60RadioTunerControl::stop()
{
    if (m_audioPlayerUtility) {
		m_audioPlayerUtility->Stop();
		m_apiTunerState = QRadioTuner::StoppedState;
		emit stateChanged(m_apiTunerState);
    }		
}

QRadioTuner::State S60RadioTunerControl::state() const
{
    return m_apiTunerState;
}

QRadioTuner::Band S60RadioTunerControl::band() const
{
    return m_currentBand;
}

bool S60RadioTunerControl::isBandSupported(QRadioTuner::Band b) const
{
	if(b == QRadioTuner::FM)
		return true;
	else if(b == QRadioTuner::LW)
		return false;
	else if(b == QRadioTuner::AM)
		return true;
	else if(b == QRadioTuner::SW)
		return false;
	else
		return false;
}

void S60RadioTunerControl::setBand(QRadioTuner::Band b)
{
    QRadioTuner::Band tempBand = b; 
    if (tempBand != m_currentBand) {
        m_currentBand = b;        
        emit bandChanged(m_currentBand);
    }
}

int S60RadioTunerControl::frequency() const
{
    return m_currentFreq;
}

void S60RadioTunerControl::setFrequency(int frequency)
{
    m_currentFreq = frequency;
    TFrequency freq(m_currentFreq);
    m_tunerUtility->Tune(freq);
}

int S60RadioTunerControl::frequencyStep(QRadioTuner::Band b) const
{
    int step = 0;

    if(b == QRadioTuner::FM)
        step = 100000; // 100kHz steps
    else if(b == QRadioTuner::LW)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::AM)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::SW)
        step = 500; // 500Hz steps

    return step;
}

QPair<int,int> S60RadioTunerControl::frequencyRange(QRadioTuner::Band band) const
{
	TFrequency bottomFreq;
	TFrequency topFreq;
	int bandError = KErrNone;
   
	if (m_tunerUtility){
		bandError = m_tunerUtility->GetFrequencyBandRange(bottomFreq, topFreq);	   
		if (!bandError) {
			return qMakePair<int,int>(bottomFreq.iFrequency, topFreq.iFrequency);
		}
	}  
   return qMakePair<int,int>(0,0);
}

CMMTunerUtility::TTunerBand S60RadioTunerControl::getNativeBand(QRadioTuner::Band b) const
{
    // api match to native s60 bands    
    if (b == QRadioTuner::AM)
        return CMMTunerUtility::ETunerBandAm;
    else if (b == QRadioTuner::FM)
        return CMMTunerUtility::ETunerBandFm;
    else if (b == QRadioTuner::LW)
        return CMMTunerUtility::ETunerBandLw;
    else
        return CMMTunerUtility::ETunerNoBand;
}

bool S60RadioTunerControl::isStereo() const
{
    return m_isStereo;
}

QRadioTuner::StereoMode S60RadioTunerControl::stereoMode() const
{
    return m_stereoMode;
}

void S60RadioTunerControl::setStereoMode(QRadioTuner::StereoMode mode)
{
	m_stereoMode = mode;
	if (m_tunerUtility) {	    
	    if (QRadioTuner::ForceMono == mode)
	        m_tunerUtility->ForceMonoReception(true);
	    else 
	        m_tunerUtility->ForceMonoReception(false);
	}	
}

int S60RadioTunerControl::signalStrength() const
{
    // return value is a percentage value
    if (m_tunerUtility) {       
        TInt maxSignalStrength;
        TInt currentSignalStrength;
        m_error = m_tunerUtility->GetMaxSignalStrength(maxSignalStrength);       
        if (m_error == KErrNone) {
            m_error = m_tunerUtility->GetSignalStrength(currentSignalStrength);
            if (m_error == KErrNone) {
				if (maxSignalStrength == 0 || currentSignalStrength == 0) {
					return 0;
				}
				m_signal = (currentSignalStrength/maxSignalStrength)*100; 
            }           
        }
    }
    return m_signal;
}

int S60RadioTunerControl::volume() const
{
    return m_vol;
}

void S60RadioTunerControl::setVolume(int volume)
{
    if (m_audioPlayerUtility) {
		m_vol = volume;
		TInt error = m_audioPlayerUtility->SetVolume(m_vol);
		emit volumeChanged(m_vol);
    }
}

bool S60RadioTunerControl::isMuted() const
{
    return m_muted;
}

void S60RadioTunerControl::setMuted(bool muted)
{
    if (m_audioPlayerUtility && m_audioInitializationComplete) {
        m_muted = muted;
        m_audioPlayerUtility->Mute(m_muted);
        emit mutedChanged(m_muted);           
    }
}

bool S60RadioTunerControl::isSearching() const
{
    if (m_tunerUtility) {
    	TUint32 tempState;
    	m_tunerUtility->GetState(tempState);
    	if (tempState == CMMTunerUtility::ETunerStateRetuning || m_scanning) {
			return true;
    	} else
    		return false;
    }
    return true;
}

void S60RadioTunerControl::cancelSearch()
{
	m_tunerUtility->CancelRetune();
	m_scanning = false;
	emit searchingChanged(false);
}

void S60RadioTunerControl::searchForward()
{
	m_scanning = true;
	setVolume(m_vol);
	m_tunerUtility->StationSeek(CMMTunerUtility::ESearchDirectionUp);
	emit searchingChanged(true);
}

void S60RadioTunerControl::searchBackward()
{
	m_scanning = true;
	setVolume(m_vol);
	m_tunerUtility->StationSeek(CMMTunerUtility::ESearchDirectionDown);
	emit searchingChanged(true);
}

bool S60RadioTunerControl::isValid() const
{
    return m_available;
}

bool S60RadioTunerControl::isAvailable() const
{
    return m_available;
}

QtMediaServices::AvailabilityError S60RadioTunerControl::availabilityError() const
{
    if (m_available)
        return QtMediaServices::NoError;
    else
        return QtMediaServices::ResourceError;
}

QRadioTuner::Error S60RadioTunerControl::error() const
{
    return m_radioError;
}

QString S60RadioTunerControl::errorString() const
{
	return m_errorString;
}

void S60RadioTunerControl::MToTuneComplete(TInt aError)
{
	if (aError == KErrNone) {
		m_scanning = false;
		m_audioPlayerUtility->Play();
		if (!m_audioInitializationComplete) {
		TRAPD(initializeError, m_audioPlayerUtility->InitializeL(KAudioPriorityFMRadio, 
                                                        TMdaPriorityPreference(KAudioPrefRadioAudioEvent)));
			if (initializeError != KErrNone) {
				m_radioError = QRadioTuner::OpenError;
			}
		}
	}
}

void S60RadioTunerControl::MTcoFrequencyChanged(const TFrequency& aOldFrequency, const TFrequency& aNewFrequency)
{
	m_currentFreq = aNewFrequency.iFrequency;
	m_scanning = false;
	emit frequencyChanged(m_currentFreq);
}

void S60RadioTunerControl::MTcoStateChanged(const TUint32& aOldState, const TUint32& aNewState)
{
	if (aNewState == CMMTunerUtility::ETunerStateActive) {
		m_apiTunerState = QRadioTuner::ActiveState;
	}
	if (aNewState == CMMTunerUtility::ETunerStatePlaying) {
		m_apiTunerState = QRadioTuner::ActiveState;
	}	
	if (aOldState != aNewState){
		emit stateChanged(m_apiTunerState);
	}
}

void S60RadioTunerControl::MTcoAntennaDetached()
{
	// no actions
}

void S60RadioTunerControl::MTcoAntennaAttached()
{
	// no actions
}

void S60RadioTunerControl::FlightModeChanged(TBool aFlightMode)
{
	// no actions
}

void S60RadioTunerControl::MTsoStereoReceptionChanged(TBool aStereo)
{
	m_isStereo = aStereo;
	emit stereoStatusChanged(aStereo);
}

void S60RadioTunerControl::MTsoForcedMonoChanged(TBool aForcedMono)
{
	if (aForcedMono) {
		m_stereoMode = QRadioTuner::ForceMono;
	}
}

void S60RadioTunerControl::MssoSignalStrengthChanged(TInt aNewSignalStrength)
{
	m_signal = aNewSignalStrength;
	emit signalStrengthChanged(m_signal);
}

void S60RadioTunerControl::MTapoInitializeComplete(TInt aError)
{
	if (aError == KErrNone) {
		m_audioInitializationComplete = true;
		m_available = true;
		m_audioPlayerUtility->Play();
		m_apiTunerState = QRadioTuner::ActiveState;
		emit stateChanged(m_apiTunerState);
	} else if (aError != KErrNone) {
		m_radioError = QRadioTuner::OpenError;
	}
}

void S60RadioTunerControl::MTapoPlayEvent(TEventType aEvent, TInt aError, TAny* aAdditionalInfo)
{
	// no actions
}



