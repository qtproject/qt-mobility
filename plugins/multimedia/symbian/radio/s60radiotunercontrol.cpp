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

#include "s60radiotunercontrol.h"
#include "s60radiotunerservice.h"

#include <QtCore/qdebug.h>

S60RadioTunerControl::S60RadioTunerControl(QObject *parent)
    :QRadioTunerControl(parent), m_tunerUtility(NULL), m_audioPlayerUtily(NULL)
{
    m_error = 0;
    m_tunerState = 0;
    m_apiTunerState = QRadioTuner::StoppedState;
    audioInitializationComplete = false;
    m_errorString = QString();
    m_radioError = QRadioTuner::NoError;
    m_muted = false;
    m_isStereo = false;
    m_stereoMode = QRadioTuner::Auto;
    m_signal = 0;
    m_currentBand = QRadioTuner::FM;
    //m_step = 100000;
    m_scanning = false;
    m_tuners = 0;
    initRadio();

}

S60RadioTunerControl::~S60RadioTunerControl()
{
	if (m_tunerUtility)
	    m_tunerUtility->Close();
	
	delete m_audioPlayerUtily;
	delete m_tunerUtility;
	m_audioPlayerUtily = NULL;
    m_tunerUtility = NULL;
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
    // this should not be called before initialization has completed.
    // so client should wait after state has changed
    return (m_currentTunerCapabilities.iTunerBands & getNativeBand(b));
}

void S60RadioTunerControl::setBand(QRadioTuner::Band b)
{
    QRadioTuner::Band tempBand = b; 
    //m_step = frequencyStep(b);
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
    //emit that frequency is changed
    emit frequencyChanged(m_currentFreq);
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
        bandError = m_tunerUtility->GetFrequencyBandRange(getNativeBand(band), bottomFreq, topFreq );
    }
    
    if (!bandError) {
        return qMakePair<int,int>(bottomFreq.iFrequency, topFreq.iFrequency);
    }
    
    return qMakePair<int,int>(0,0);
}
CMMTunerUtility::TTunerBand S60RadioTunerControl::getNativeBand(QRadioTuner::Band b) const
{
    // api match to native s60 bands
    
    if(b == QRadioTuner::AM)
        return CMMTunerUtility::ETunerBandAm;
    else if(b == QRadioTuner::FM)
        return CMMTunerUtility::ETunerBandFm;
/* TODO: support for Japanese FM 
    else if(b == QRadioTuner::FM)
        return CMMTunerUtility::ETunerBandJapaneseFm;
        */
    else if(b == QRadioTuner::LW)
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
	    
	    if (QRadioTuner::ForceMono == mode )
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
    if (m_audioPlayerUtily && audioInitializationComplete) {
        int maxVol = m_audioPlayerUtily->MaxVolume();
        m_vol = (volume/maxVol)*100; 
        m_audioPlayerUtily->SetVolume(m_vol);
        // inform that we changed volume, no async methods.
        emit volumeChanged(m_vol);
    }
}

bool S60RadioTunerControl::isMuted() const
{
    return m_muted;
}

void S60RadioTunerControl::setMuted(bool muted)
{
    if (m_audioPlayerUtily && audioInitializationComplete) {
        m_muted = muted;
        m_audioPlayerUtily->Mute(m_muted);
        if (muted) {
            m_vol = 0;
            emit volumeChanged(m_vol);    
        }
        
    }
    
}

bool S60RadioTunerControl::isSearching() const
{
    if (m_tunerUtility)
        return CMMTunerUtility::ETunerStateRetuning & m_tunerState;
    else return false;
}

void S60RadioTunerControl::cancelSearch()
{
    m_tunerUtility->CancelRetune();
}

void S60RadioTunerControl::searchForward()
{
    search();
}

void S60RadioTunerControl::searchBackward()
{
    search();
}

void S60RadioTunerControl::search()
{
    // ask for tuner control, maybe done in initRadio()
    //m_tunerUtility->RequestTunerControl();

    TFrequency myFreq( m_currentFreq );
    m_tunerUtility->Tune( myFreq, getNativeBand(m_currentBand) );
    // we are now searching
    emit searchingStatusChanged(true);
}

bool S60RadioTunerControl::isValid() const
{
    return m_available;
}

bool S60RadioTunerControl::initRadio()
{
	m_available = false;
	if (CMMTunerUtility::TunersAvailable() > 0) {
		// use default device at index0 for now
	    // if want to use radio as an alarm clock we must change accesspriority above (int) 75
		TRAP(m_error,m_tunerUtility = CMMTunerUtility::NewL(*this, m_tuners, CMMTunerUtility::ETunerAccessPriorityNormal));
		if (m_error != KErrNone) {
			m_available = false;
			m_radioError = QRadioTuner::OpenError;
		}
		// save info about tuner
		m_error = m_tunerUtility->GetCapabilities(m_tuners,m_currentTunerCapabilities);
		// method says we dont need this whereas doc says we need this..
		//m_tunerUtility->RequestTunerControl();
		m_tunerUtility->NotifyStereoChange(*this);
		m_tunerUtility->NotifySignalStrength(*this);
		m_tunerUtility->NotifyChange(*this);
		int audioError;
		TRAP(audioError, m_audioPlayerUtily = m_tunerUtility->GetTunerAudioPlayerUtilityL(*this));
		if (audioError != KErrNone) {
		    
		    m_available = false;
		    m_radioError = QRadioTuner::OpenError;
		}
		m_available = true;
		m_radioError = QRadioTuner::NoError;
	}
    return m_available;
}

void S60RadioTunerControl::start()
{
    if (m_audioPlayerUtily) {
        int audioError = KErrNone;
        TRAP(audioError, m_audioPlayerUtily->InitializeL());
        if (audioError != KErrNone) {
            m_available = false;
            m_radioError = QRadioTuner::OpenError;
        }
    }
}

void S60RadioTunerControl::stop()
{
    if (m_audioPlayerUtily && audioInitializationComplete) {
        m_audioPlayerUtily->Stop();
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
int S60RadioTunerControl::deviceCount()
{
    return CMMTunerUtility::TunersAvailable();
}


void S60RadioTunerControl::MToTuneComplete(TInt aError)
{
    Q_UNUSED(aError);
    // tune complete
    emit searchingStatusChanged(false);
}
void S60RadioTunerControl::MToTunerEvent(MMMTunerObserver::TEventType aType, TInt aError, TAny* aAdditionalInfo)
{
    Q_UNUSED(aAdditionalInfo);
    if (aType == MMMTunerObserver::ETunerEvent && aError !=KErrNone) {
        //tuner has failed somehow.
        m_error = aError; 
        m_radioError = QRadioTuner::OpenError;
        m_errorString = QString(aError);
        // compile fails if base class is removed?
        emit QRadioTunerControl::error(QRadioTuner::OpenError);
    }
}

void S60RadioTunerControl::MTsoStereoReceptionChanged(TBool aStereo)
{	
    m_isStereo = aStereo;
	emit stereoStatusChanged(m_isStereo);
}

void S60RadioTunerControl::MTsoForcedMonoChanged(TBool aForcedMono)
{
    Q_UNUSED(aForcedMono);
}
void S60RadioTunerControl::MssoSignalStrengthChanged(TInt aNewSignalStrength)
{
	emit signalStrengthChanged(aNewSignalStrength);
}
void S60RadioTunerControl::MTcoFrequencyChanged(const TFrequency& aOldFrequency, const TFrequency& aNewFrequency)
{
    m_currentFreq = aNewFrequency.iFrequency;
    Q_UNUSED(aOldFrequency);
    emit frequencyChanged(m_currentFreq);
}
void S60RadioTunerControl::MTcoChannelChanged(const TChannel& aOldChannel, const TChannel& aNewChannel)
{
    // no impl needed
    Q_UNUSED(aOldChannel);
    Q_UNUSED(aNewChannel);
}
void S60RadioTunerControl::MTcoStateChanged(const TUint32& aOldState, const TUint32& aNewState)
{
    m_tunerState = aNewState;
    if (aNewState & CMMTunerUtility::ETunerStateActive) {
        m_apiTunerState = QRadioTuner::ActiveState; 
    }
    else if (aNewState & CMMTunerUtility::ETunerStatePlaying) {
        m_apiTunerState = QRadioTuner::ActiveState; 
    }
    else if (aNewState & CMMTunerUtility::ETunerStateRecording ) {
        m_apiTunerState = QRadioTuner::ActiveState; 
    }
    else if (aNewState & CMMTunerUtility::ETunerStateRetuning) {
        m_apiTunerState = QRadioTuner::ActiveState; 
    }
    else
        m_apiTunerState = QRadioTuner::StoppedState;

    emit stateChanged(m_apiTunerState);

    Q_UNUSED(aOldState);
 
}
void S60RadioTunerControl::MTcoAntennaDetached()
{
    // todo: check antenna detached.
    m_apiTunerState = QRadioTuner::StoppedState;
    emit stateChanged(m_apiTunerState);
}
void S60RadioTunerControl::MTcoAntennaAttached()
{
    // todo: check antenna attached.
    if ( m_tunerState > 0 ) { 
        m_apiTunerState = QRadioTuner::ActiveState;
        emit stateChanged(m_apiTunerState);
    }
}
void S60RadioTunerControl::MTcoFlightModeChanged(TBool aFlightMode)
{
    Q_UNUSED(aFlightMode);
}
void S60RadioTunerControl::MTcoSquelchChanged(TBool aSquelch)
{
    Q_UNUSED(aSquelch);
}
//from MMMTunerAudioPlayerObserver
void S60RadioTunerControl::MTapoInitializeComplete(TInt aError)
{
    if (aError == KErrNone) {
        audioInitializationComplete = true;
    }
    else {
        m_radioError = QRadioTuner::ResourceError;
        m_errorString = QString(aError);
        emit QRadioTunerControl::error(QRadioTuner::ResourceError);
    }
}
void S60RadioTunerControl::MTapoPlayEvent(MMMTunerAudioPlayerObserver::TEventType aEvent, TInt aError, TAny* aAdditionalInfo)
{
    // see MMMTunerAudioPlayerObserver::TEventType for events info
    Q_UNUSED(aEvent);
    Q_UNUSED(aError);
    Q_UNUSED(aAdditionalInfo);
}

