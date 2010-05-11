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

#include "s60radiotunercontrol_since32.h"
#include "s60radiotunerservice.h"

#include <QtCore/qdebug.h>
#include <radiofmtunerutility.h>

S60RadioTunerControl::S60RadioTunerControl(QObject *parent)
    : QRadioTunerControl(parent)
    , m_error(0)
    , m_radioUtility(NULL)   
    , m_fmTunerUtility(NULL)
    , m_playerUtility(NULL)
    , m_audioInitializationComplete(false)
    , m_muted(false)
    , m_isStereo(true)
    , m_vol(100)
    , m_signal(0)
    , m_radioError(QRadioTuner::NoError)
    , m_scanning(false)
    , m_currentBand(QRadioTuner::FM)
    , m_currentFreq(87500000)
    , m_stereoMode(QRadioTuner::Auto)
    , m_apiTunerState(QRadioTuner::StoppedState)
    , m_maxVolume(100)
{   
    initRadio();
}

S60RadioTunerControl::~S60RadioTunerControl()
{
	if (m_fmTunerUtility) {
		m_fmTunerUtility->Close();
	}
	
	if(m_playerUtility) {
		m_playerUtility->Close();
	}
	
	delete m_radioUtility;
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
	m_fmTunerUtility->SetFrequency(m_currentFreq);
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
    int bottomFreq;
    int topFreq;
    
    int bandError = KErrNone;
    TFmRadioFrequencyRange range;
    
    if (m_fmTunerUtility) {
        bandError = m_fmTunerUtility->GetFrequencyRange(range, bottomFreq, topFreq);
    }    
    if (!bandError) {
        return qMakePair<int,int>(bottomFreq, topFreq);
    }
    
    return qMakePair<int,int>(0,0);
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
	if (m_fmTunerUtility) { 
	    if (QRadioTuner::ForceMono == mode) {
	    	m_fmTunerUtility->ForceMonoReception(true);
	    	m_stereoMode = QRadioTuner::ForceMono;
			m_isStereo = false;
	    } else {
	    	m_fmTunerUtility->ForceMonoReception(false);
			m_isStereo = true;
			m_stereoMode = QRadioTuner::ForceStereo;
	    }
	}
}

int S60RadioTunerControl::signalStrength() const
{
    // return value is a percentage value
    if (m_fmTunerUtility) {    
        TInt maxSignalStrength;
        TInt currentSignalStrength;
        m_error = m_fmTunerUtility->GetMaxSignalStrength(maxSignalStrength);
        
        if (m_error == KErrNone) {
            m_error = m_fmTunerUtility->GetSignalStrength(currentSignalStrength);
            if (m_error == KErrNone) {
				if (currentSignalStrength == 0 || maxSignalStrength == 0) {
					return currentSignalStrength;
				}
                m_signal = currentSignalStrength / maxSignalStrength;                
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
	if (m_playerUtility) {
		m_vol = volume;
		m_playerUtility->SetVolume(volume);
		emit volumeChanged(m_vol);
	}
}

bool S60RadioTunerControl::isMuted() const
{
    return m_muted;
}

void S60RadioTunerControl::setMuted(bool muted)
{
	if (m_playerUtility) {
		m_muted = muted;
		m_playerUtility->Mute(m_muted);  
	} 
}

bool S60RadioTunerControl::isSearching() const
{
	return m_scanning;
}

void S60RadioTunerControl::cancelSearch()
{
	m_fmTunerUtility->CancelStationSeek();
	m_scanning = false;
	emit searchingChanged(false);
}

void S60RadioTunerControl::searchForward()
{
	m_fmTunerUtility->StationSeek(true);
	m_scanning = true;
	emit searchingChanged(true);
}

void S60RadioTunerControl::searchBackward()
{
	m_fmTunerUtility->StationSeek(false);
	m_scanning = true;
	emit searchingChanged(true);
}

bool S60RadioTunerControl::isValid() const
{
    return m_available;
}

bool S60RadioTunerControl::initRadio()
{
	m_available = false;	
	// create an instance of Radio Utility factory and indicate
	// FM Radio is a primary client
	TRAPD(utilityError, 
		m_radioUtility = CRadioUtility::NewL(ETrue);
		// Get a tuner utility
		m_fmTunerUtility = &m_radioUtility->RadioFmTunerUtilityL(*this);
		// Get a player utility
		m_playerUtility = &m_radioUtility->RadioPlayerUtilityL(*this);
	);
	if (utilityError != KErrNone) {
        m_radioError = QRadioTuner::ResourceError;
		return m_available;
	}
	
	m_tunerControl = false;
	
	m_available = true;
	return m_available;
}

bool S60RadioTunerControl::isAvailable() const
{
	return m_available;
}

QtMultimedia::AvailabilityError S60RadioTunerControl::availabilityError() const
{
	if (m_available)
		return QtMultimedia::NoError;
	else
		return QtMultimedia::ResourceError;
}

void S60RadioTunerControl::start()
{	
	if (!m_tunerControl) {
		m_fmTunerUtility->RequestTunerControl();
		m_apiTunerState = QRadioTuner::ActiveState;
		emit stateChanged(m_apiTunerState);
	} else {
		m_playerUtility->Play();
		m_apiTunerState = QRadioTuner::ActiveState;
		emit stateChanged(m_apiTunerState);
	}
		
}

void S60RadioTunerControl::stop()
{
	if (m_playerUtility) {
		m_playerUtility->Stop();
		m_apiTunerState = QRadioTuner::StoppedState;
		emit stateChanged(m_apiTunerState);
    }
}

QRadioTuner::Error S60RadioTunerControl::error() const
{
    return m_radioError;
}
QString S60RadioTunerControl::errorString() const
{
	return m_errorString;
}

void S60RadioTunerControl::MrpoStateChange(TPlayerState aState, TInt aError)
{
	if (aError == KErrNone){
		m_radioError = QRadioTuner::NoError;
		if (aState == ERadioPlayerIdle) {
			m_apiTunerState = QRadioTuner::ActiveState;
		} else if (aState == ERadioPlayerPlaying) {
			m_apiTunerState = QRadioTuner::ActiveState;
		}
	}	
	emit stateChanged(m_apiTunerState);
}

void S60RadioTunerControl::MrpoVolumeChange(TInt aVolume)
{
	m_vol = aVolume;
    emit volumeChanged(m_vol);
}

void S60RadioTunerControl::MrpoMuteChange(TBool aMute)
{
	m_muted = aMute;
	emit mutedChanged(m_muted);
}

void S60RadioTunerControl::MrpoBalanceChange(TInt aLeftPercentage, TInt aRightPercentage)
{
	// no actions
}

void S60RadioTunerControl::MrftoRequestTunerControlComplete(TInt aError)
{
	if (aError == KErrNone) {
		m_playerUtility->GetMaxVolume(m_maxVolume);
		m_radioError = QRadioTuner::NoError;
		m_tunerControl = true;
		m_available = true;
		m_fmTunerUtility->SetFrequency(m_currentFreq);
		m_playerUtility->Play();
		int signal = signalStrength();		
		if (m_signal != signal) {
			emit signalStrengthChanged(signal);
			m_signal = signal;
		}

	} else if (aError == KFmRadioErrAntennaNotConnected) {
		m_radioError = QRadioTuner::OpenError;
	} else if (aError == KErrAlreadyExists){
		m_radioError = QRadioTuner::ResourceError;
	} else if (aError == KFmRadioErrFrequencyOutOfBandRange) {
		m_radioError = QRadioTuner::OutOfRangeError;
	}else{
		m_radioError = QRadioTuner::OpenError;
	}

}

void S60RadioTunerControl::MrftoSetFrequencyRangeComplete(TInt aError)
{
	if (aError == KFmRadioErrFrequencyOutOfBandRange || KFmRadioErrFrequencyNotValid) {
		m_radioError = QRadioTuner::OutOfRangeError;
	} else if (aError == KFmRadioErrHardwareFaulty || KFmRadioErrOfflineMode) {
		m_radioError = QRadioTuner::OpenError;
	}
}

void S60RadioTunerControl::MrftoSetFrequencyComplete(TInt aError)
{
	if (aError == KErrNone) {
		m_radioError = QRadioTuner::NoError;
	} else if (aError == KFmRadioErrFrequencyOutOfBandRange || KFmRadioErrFrequencyNotValid) {
		m_radioError = QRadioTuner::OutOfRangeError;
	} else if (aError == KFmRadioErrHardwareFaulty || KFmRadioErrOfflineMode) {
		m_radioError = QRadioTuner::OpenError;
	}
}

void S60RadioTunerControl::MrftoStationSeekComplete(TInt aError, TInt aFrequency)
{
	m_scanning = false;
	if (aError == KErrNone) {
		m_radioError = QRadioTuner::NoError;
		m_currentFreq = aFrequency;
	} else {
		m_radioError = QRadioTuner::OpenError;
	}
}

void S60RadioTunerControl::MrftoFmTransmitterStatusChange(TBool aActive)
{
	//no actions
}

void S60RadioTunerControl::MrftoAntennaStatusChange(TBool aAttached)
{
	if (aAttached && m_tunerControl) {
		m_playerUtility->Play();
	}
}

void S60RadioTunerControl::MrftoOfflineModeStatusChange(TBool /*aOfflineMode*/)
{

}

void S60RadioTunerControl::MrftoFrequencyRangeChange(TFmRadioFrequencyRange aBand /*, TInt aMinFreq, TInt aMaxFreq*/)
{
	if (aBand == EFmRangeEuroAmerica) {
		setBand(QRadioTuner::FM);
	} 
}

void S60RadioTunerControl::MrftoFrequencyChange(TInt aNewFrequency)
{
	m_currentFreq = aNewFrequency;
	emit frequencyChanged(m_currentFreq);

	int signal = signalStrength();
	if (m_signal != signal) {
		emit signalStrengthChanged(signal);
		m_signal = signal;
	}
}

void S60RadioTunerControl::MrftoForcedMonoChange(TBool aForcedMono)
{
	if (aForcedMono) {
		m_stereoMode = QRadioTuner::ForceMono;
	} else {
		m_stereoMode = QRadioTuner::ForceStereo;
	}	
	emit stereoStatusChanged(!aForcedMono);
}

void S60RadioTunerControl::MrftoSquelchChange(TBool aSquelch)
{
	// no actions
}
