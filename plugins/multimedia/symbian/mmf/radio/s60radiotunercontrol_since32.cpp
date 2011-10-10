/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "s60mmftrace.h"

#include <RadioFmTunerUtility.h>

S60RadioTunerControl::S60RadioTunerControl(QObject *parent)
    : QRadioTunerControl(parent)
    , m_error(0)
    , m_radioUtility(NULL)
    , m_fmTunerUtility(NULL)
    , m_playerUtility(NULL)
    , m_maxVolume(100)
    , m_audioInitializationComplete(false)
    , m_muted(false)
    , m_isStereo(true)
    , m_vol(50)
    , m_signal(0)
    , m_scanning(false)
    , m_currentBand(QRadioTuner::FM)
    , m_currentFreq(87500000)
    , m_radioError(QRadioTuner::NoError)
    , m_stereoMode(QRadioTuner::Auto)
    , m_apiTunerState(QRadioTuner::StoppedState)
    , m_previousSignal(0)
    , m_volChangeRequired(false)
    , m_signalStrengthTimer(new QTimer(this))
{
    TRACE("S60RadioTunerControl::S60RadioTunerControl" << qtThisPtr());
    bool retValue = initRadio();
    if (!retValue) {
        m_errorString = QString(tr("Initialize Error."));
        emit error(QRadioTuner::ResourceError);
    } else {
        connect(m_signalStrengthTimer, SIGNAL(timeout()), this, SLOT(changeSignalStrength()));
    }
}

S60RadioTunerControl::~S60RadioTunerControl()
{
    TRACE("S60RadioTunerControl::~S60RadioTunerControl" << qtThisPtr());

    if (m_fmTunerUtility) {
        m_fmTunerUtility->Close();
    }

    if (m_playerUtility) {
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
    if (b == QRadioTuner::FM)
        return true;
    else if (b == QRadioTuner::LW)
        return false;
    else if (b == QRadioTuner::AM)
        return false;
    else if (b == QRadioTuner::SW)
        return false;
    else if (b == QRadioTuner::FM2)
        return false;
    else
        return false;
}

void S60RadioTunerControl::changeSignalStrength()
    {
    
    int currentSignal = signalStrength();
    if (currentSignal != m_previousSignal)
        {
        m_previousSignal = currentSignal;
        emit signalStrengthChanged(currentSignal);
        }
    }
void S60RadioTunerControl::setBand(QRadioTuner::Band b)
{
    TRACE("S60RadioTunerControl::setBand" << qtThisPtr() << "band" << b);
    QRadioTuner::Band tempBand = b;
    if (tempBand != m_currentBand ) {
        if (isBandSupported(tempBand)){
            m_currentBand = b;  
            emit bandChanged(m_currentBand);
        }
        else {
            switch(tempBand)
                {
                case QRadioTuner::FM :
                    m_errorString = QString(tr("Band FM not Supported"));
                    break;
                case QRadioTuner::AM :
                    m_errorString = QString(tr("Band AM not Supported"));
                    break;
                case QRadioTuner::SW :
                    m_errorString = QString(tr("Band SW not Supported"));
                    break;
                case QRadioTuner::LW :
                    m_errorString = QString(tr("Band LW not Supported"));
                    break;
                case QRadioTuner::FM2 :
                    m_errorString = QString(tr("Band FM2 not Supported"));
                    break;
                default :
                    m_errorString = QString("Band %1 not Supported").arg(tempBand);
                    break;
                }
            emit error(QRadioTuner::OutOfRangeError);
        }
    }
}

int S60RadioTunerControl::frequency() const
{
    return m_currentFreq;
}

void S60RadioTunerControl::setFrequency(int frequency)
{
    TRACE("S60RadioTunerControl::setFrequency" << qtThisPtr() << "frequency" << frequency);

    m_currentFreq = frequency;
    m_fmTunerUtility->SetFrequency(m_currentFreq);
}
int S60RadioTunerControl::frequencyStep(QRadioTuner::Band b) const
{
    int step = 0;
    if (b == QRadioTuner::FM)
        step = 100000; // 100kHz steps
    else if(b == QRadioTuner::LW)
        step = 1000; // 1kHz steps
    else if (b == QRadioTuner::AM)
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
    TRACE("S60RadioTunerControl::setStereoMode" << qtThisPtr() << "mode" << mode);

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
                m_signal = ((TInt64)currentSignalStrength) * 100 / maxSignalStrength;
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
    TRACE("S60RadioTunerControl::setVolume" << qtThisPtr() << "volume" << volume);

    int boundVolume = qBound(0, volume, 100);

    if (m_vol == boundVolume )
        return;

    if (!m_muted && m_playerUtility) {
        m_vol = boundVolume;
        // Don't set volume until State is in Active State.
        if (state() == QRadioTuner::ActiveState ) {
            m_playerUtility->SetVolume(m_vol*m_volMultiplier);

        } else {
            m_volChangeRequired = TRUE;
            emit volumeChanged(boundVolume);
        }
    }
}

bool S60RadioTunerControl::isMuted() const
{
    return m_muted;
}

void S60RadioTunerControl::setMuted(bool muted)
{
    TRACE("S60RadioTunerControl::setMuted" << qtThisPtr() << "muted" << muted);
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
    TRACE("S60RadioTunerControl::cancelSearch" << qtThisPtr());

    m_fmTunerUtility->CancelStationSeek();
    m_scanning = false;
    emit searchingChanged(false);
}

void S60RadioTunerControl::searchForward()
{
    TRACE("S60RadioTunerControl::searchForward" << qtThisPtr());
    m_fmTunerUtility->StationSeek(true);
    m_scanning = true;
    emit searchingChanged(m_scanning);
}

void S60RadioTunerControl::searchBackward()
{
    TRACE("S60RadioTunerControl::searchBackward" << qtThisPtr());
    m_fmTunerUtility->StationSeek(false);
    m_scanning = true;
    emit searchingChanged(m_scanning);
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
        // we want to listen radio in offline mode too
        m_fmTunerUtility->EnableTunerInOfflineMode(ETrue);
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

QtMultimediaKit::AvailabilityError S60RadioTunerControl::availabilityError() const
{
    if (m_available)
        return QtMultimediaKit::NoError;
    else
        return QtMultimediaKit::ResourceError;
}

void S60RadioTunerControl::start()
{
    TRACE("S60RadioTunerControl::start" << qtThisPtr());
    if (!m_tunerControl) {
        m_fmTunerUtility->RequestTunerControl();
    } else {
        m_playerUtility->Play();
    }
    m_signalStrengthTimer->start(3000);
}

void S60RadioTunerControl::stop()
{
    TRACE("S60RadioTunerControl::stop" << qtThisPtr());
    if (m_playerUtility) {
        m_playerUtility->Stop();
    }
    m_signalStrengthTimer->stop();
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
    TRACE("S60RadioTunerControl::MrpoStateChange" << qtThisPtr()
          << "state" << aState << "error" << aError);
    if (aError == KErrNone){
        m_radioError = QRadioTuner::NoError;
        if (aState == ERadioPlayerIdle) {
            m_apiTunerState = QRadioTuner::StoppedState;
        } else if (aState == ERadioPlayerPlaying) {
            m_apiTunerState = QRadioTuner::ActiveState;
            //Apply pending volume changes.
            if(m_volChangeRequired){
                setVolume(m_vol);
                }
        }
    } else {
        m_apiTunerState = QRadioTuner::StoppedState;
    }
    emit stateChanged(m_apiTunerState);
}

void S60RadioTunerControl::MrpoVolumeChange(TInt aVolume)
{
    TRACE("S60RadioTunerControl::MrpoVolumeChange" << qtThisPtr()
          << "volume" << aVolume);
    m_vol = (aVolume/m_volMultiplier);
    if (!m_volChangeRequired) {
        emit volumeChanged(m_vol);

    } else {
        m_volChangeRequired = false;
    }
}

void S60RadioTunerControl::MrpoMuteChange(TBool aMute)
{
    TRACE("S60RadioTunerControl::MrpoMuteChange" << qtThisPtr()
          << "mute" << aMute);
    m_muted = aMute;
    emit mutedChanged(m_muted);
}

void S60RadioTunerControl::MrpoBalanceChange(TInt aLeftPercentage, TInt aRightPercentage)
{
    TRACE("S60RadioTunerControl::MrpoBalanceChange" << qtThisPtr()
          << "left" << aLeftPercentage << "right" << aRightPercentage);

    // no actions
}

void S60RadioTunerControl::MrftoRequestTunerControlComplete(TInt aError)
{
    TRACE("S60RadioTunerControl::MrftoRequestTunerControlComplete" << qtThisPtr()
          << "error" << aError);
    if (aError == KErrNone) {
        m_playerUtility->GetMaxVolume(m_maxVolume);
        m_volMultiplier = float(m_maxVolume)/float(100);
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
        m_errorString = QString(tr("Antenna Not Connected"));
        emit error(m_radioError);
    } else if (aError == KErrAlreadyExists){
        m_radioError = QRadioTuner::ResourceError;
        m_errorString = QString(tr("Resource Error."));
        emit error(m_radioError);
    } else if (aError == KFmRadioErrFrequencyOutOfBandRange) {
        m_radioError = QRadioTuner::OutOfRangeError;
        m_errorString = QString(tr("Frequency out of band range"));
        emit error(m_radioError);
    }else{
        m_radioError = QRadioTuner::OpenError;
        m_errorString = QString(tr("Unknown Error."));
        emit error(m_radioError);
    }
}

void S60RadioTunerControl::MrftoSetFrequencyRangeComplete(TInt aError)
{
    TRACE("S60RadioTunerControl::MrftoSetFrequencyRangeComplete" << qtThisPtr()
          << "error" << aError);
    if (aError == KFmRadioErrFrequencyOutOfBandRange || KFmRadioErrFrequencyNotValid) {
        m_radioError = QRadioTuner::OutOfRangeError;
        m_errorString = QString(tr("Frequency Out of Band Range or Frequency Not Valid"));
        emit error(m_radioError);
    } else if (aError == KFmRadioErrHardwareFaulty || KFmRadioErrOfflineMode) {
        m_radioError = QRadioTuner::OpenError;
        m_errorString = QString(tr("Hardware failure or RadioInOfflineMode"));
        emit error(m_radioError);
    }
}

void S60RadioTunerControl::MrftoSetFrequencyComplete(TInt aError)
{
    TRACE("S60RadioTunerControl::MrftoSetFrequencyComplete" << qtThisPtr()
          << "error" << aError);
    if (aError == KErrNone) {
        m_radioError = QRadioTuner::NoError;
    } else if (aError == KFmRadioErrFrequencyOutOfBandRange || KFmRadioErrFrequencyNotValid) {
        m_radioError = QRadioTuner::OutOfRangeError;
        m_errorString = QString(tr("Frequency Out of range or not Valid."));
        emit error(m_radioError);
    } else if (aError == KFmRadioErrHardwareFaulty || KFmRadioErrOfflineMode) {
        m_radioError = QRadioTuner::OpenError;
        m_errorString = QString("Hardware failure or Radio In Offline Mode");
        emit error(m_radioError);
    }
}

void S60RadioTunerControl::MrftoStationSeekComplete(TInt aError, TInt aFrequency)
{
    TRACE("S60RadioTunerControl::MrftoStationSeekComplete" << qtThisPtr()
          << "error" << aError << "frequency" << aFrequency);
    m_scanning = false;
    if (aError == KErrNone) {
        m_radioError = QRadioTuner::NoError;
        m_currentFreq = aFrequency;
        emit searchingChanged(m_scanning);
    } else {
        m_radioError = QRadioTuner::OpenError;
        emit searchingChanged(m_scanning);
        m_errorString = QString("Scanning Error");
        emit error(m_radioError);
    }
}

void S60RadioTunerControl::MrftoFmTransmitterStatusChange(TBool aActive)
{
    TRACE("S60RadioTunerControl::MrftoFmTransmitterStatusChange" << qtThisPtr()
          << "active" << aActive);

    //no actions
}

void S60RadioTunerControl::MrftoAntennaStatusChange(TBool aAttached)
{
    TRACE("S60RadioTunerControl::MrftoAntennaStatusChange" << qtThisPtr()
          << "attached" << aAttached);

    if (aAttached && m_tunerControl) {
        m_playerUtility->Play();
    }
}

void S60RadioTunerControl::MrftoOfflineModeStatusChange(TBool aOfflineMode)
{
    TRACE("S60RadioTunerControl::MrftoOfflineModeStatusChange" << qtThisPtr()
          << "mode" << aOfflineMode);

}

void S60RadioTunerControl::MrftoFrequencyRangeChange(TFmRadioFrequencyRange aBand /*, TInt aMinFreq, TInt aMaxFreq*/)
{
    TRACE("S60RadioTunerControl::MrftoFrequencyRangeChange" << qtThisPtr()
          << "band" << aBand);

    if (aBand == EFmRangeEuroAmerica) {
        setBand(QRadioTuner::FM);
    }
}

void S60RadioTunerControl::MrftoFrequencyChange(TInt aNewFrequency)
{
    TRACE("S60RadioTunerControl::MrftoFrequencyChange" << qtThisPtr()
          << "frequency" << aNewFrequency);

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
    TRACE("S60RadioTunerControl::MrftoForcedMonoChange" << qtThisPtr()
          << "forcedMono" << aForcedMono);

    if (aForcedMono) {
        m_stereoMode = QRadioTuner::ForceMono;
    } else {
        m_stereoMode = QRadioTuner::ForceStereo;
    }
    emit stereoStatusChanged(!aForcedMono);
}

void S60RadioTunerControl::MrftoSquelchChange(TBool aSquelch)
{
    TRACE("S60RadioTunerControl::MrftoSquelchChange" << qtThisPtr()
          << "squelch" << aSquelch);

    // no actions
}
