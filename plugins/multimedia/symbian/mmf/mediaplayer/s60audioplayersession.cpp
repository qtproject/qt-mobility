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

#include "s60audioplayersession.h"
#include "s60mmftrace.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvariant.h>

/*!
    Constructs the CMdaAudioPlayerUtility object with given \a parent QObject.

    And Registers for Audio Loading Notifications.

*/

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
    , m_player(0)
    , m_audioEndpoint("Default")
{
    TRACE("S60AudioPlayerSession::S60AudioPlayerSession" << qtThisPtr());

#ifdef HAS_AUDIOROUTING
    m_audioOutput = 0;
#endif //HAS_AUDIOROUTING    
    QT_TRAP_THROWING(m_player = CAudioPlayer::NewL(*this, 0, EMdaPriorityPreferenceNone));
    m_player->RegisterForAudioLoadingNotification(*this);
}


/*!
   Destroys the CMdaAudioPlayerUtility object.

    And Unregister the observer.

*/

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    TRACE("S60AudioPlayerSession::~S60AudioPlayerSession" << qtThisPtr());
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

/*!

   Opens the a file from \a path.

*/

void S60AudioPlayerSession::doLoadL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif //HAS_AUDIOROUTING
    m_player->OpenFileL(path);
}

/*!

  Returns the duration of the audio sample in microseconds.

*/

qint64 S60AudioPlayerSession::doGetDurationL() const
{
    return m_player->Duration().Int64() / qint64(1000);
}

/*!
 *  Returns the current playback position in microseconds from the start of the clip.

*/

qint64 S60AudioPlayerSession::doGetPositionL() const
{
    TTimeIntervalMicroSeconds ms = 0;
    m_player->GetPosition(ms);
    return ms.Int64() / qint64(1000);
}

/*!
   Returns TRUE if Video available or else FALSE
 */

bool S60AudioPlayerSession::isVideoAvailable()
{
    return false;
}

/*!
   Returns TRUE if Audio available or else FALSE
 */
bool S60AudioPlayerSession::isAudioAvailable()
{
    return true; // this is a bit happy scenario, but we do emit error that we can't play
}

/*!
   Starts loading Media and sets media status to Buffering.

 */

void S60AudioPlayerSession::MaloLoadingStarted()
{
    TRACE("S60AudioPlayerSession::MaloLoadingStarted" << qtThisPtr());

    buffering();
}


/*!
   Indicates loading Media is completed.

   And sets media status to Buffered.

 */

void S60AudioPlayerSession::MaloLoadingComplete()
{
    TRACE("S60AudioPlayerSession::MaloLoadingComplete" << qtThisPtr());

    buffered();
}

/*!
    Start or resume playing the current source.
*/

void S60AudioPlayerSession::doPlay()
{
    // For some reason loading progress callback are not called on emulator
    // Same is the case with hardware. Will be fixed as part of QTMOBILITY-782.
        
    //#ifdef __WINSCW__
        buffering();
    //#endif
        m_player->Play();
    //#ifdef __WINSCW__
        buffered();
    //#endif
}


/*!
    Pause playing the current source.
*/


void S60AudioPlayerSession::doPauseL()
{
    m_player->Pause();
}


/*!

    Stop playing, and reset the play position to the beginning.
*/

void S60AudioPlayerSession::doStop()
{
    m_player->Stop();
}

/*!
    Closes the current audio clip (allowing another clip to be opened)
*/

void S60AudioPlayerSession::doClose()
{
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        m_audioOutput->UnregisterObserver(*this);
        delete m_audioOutput;
        m_audioOutput = NULL;
    }
#endif
    m_player->Close();
}

/*!

    Changes the current playback volume to specified \a value.
*/

void S60AudioPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolume(volume * m_player->MaxVolume() / 100);
}

/*!
    Sets the current playback position to \a microSeconds from the start of the clip.
*/

void S60AudioPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPosition(TTimeIntervalMicroSeconds(microSeconds));
}

/*!

    Updates meta data entries in the current audio clip.
*/

void S60AudioPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;

    //User::LeaveIfError(m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries));
    m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);

    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        entry = m_player->GetMetaDataEntryL(i);
        metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        delete entry;
    }
    emit metaDataChanged();
}

/*!
    Sets the playbackRate with \a rate.
*/

void S60AudioPlayerSession::setPlaybackRate(qreal rate)
{
    TRACE("S60AudioPlayerSession::setPlaybackRate" << qtThisPtr() << "rate" << rate);
    /*Since AudioPlayerUtility doesn't support set playback rate hence
     * setPlaybackRate emits playbackRateChanged signal for 1.0x ie normal playback.
     * For all other playBackRates it sets and emits error signal.
    */
    if (rate == 1.0) {
        emit playbackRateChanged(rate);
        return;
    } else {
        int err = KErrNotSupported;
        setAndEmitError(err);
    }
}

/*!

    Returns the percentage of the audio clip loaded.
*/

int S60AudioPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetAudioLoadingProgressL(progress);
    return progress;
}

/*!

    Defines required client behaviour when an attempt to open and initialise an audio sample has completed,
    successfully or not.

    \a aError if KErrNone the sample is ready to play or else system wide error.

    \a aDuration The duration of the audio sample.
*/

#ifdef S60_DRM_SUPPORTED
void S60AudioPlayerSession::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
#else
void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
#endif
{
    TRACE("S60AudioPlayerSession::MapcInitComplete" << qtThisPtr() << "error" << aError);

    Q_UNUSED(aDuration);
    setError(aError);
    if (KErrNone != aError)
        return;
#ifdef HAS_AUDIOROUTING    
    TRAPD(err, 
        m_audioOutput = CAudioOutput::NewL(*m_player);
        m_audioOutput->RegisterObserverL(*this);
    );
    setActiveEndpoint(m_audioEndpoint);
    setError(err);
#endif //HAS_AUDIOROUTING
    if (KErrNone == aError)
        loaded();
}

/*!
    Defines required client behaviour when an attempt to playback an audio sample has completed,
    successfully or not.

    \a aError if KErrNone the playback completed or else system wide error.
*/


#ifdef S60_DRM_SUPPORTED
void S60AudioPlayerSession::MdapcPlayComplete(TInt aError)
#else
void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
#endif
{
    TRACE("S60AudioPlayerSession::MapcPlayComplete" << qtThisPtr() << "error" << aError);

    if (KErrNone == aError)
        endOfMedia();
    else
        setError(aError);
}

/*!
    Defiens which Audio End point to use.

    \a audioEndpoint audioEndpoint name.
*/

void S60AudioPlayerSession::doSetAudioEndpoint(const QString& audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

/*!

    Returns audioEndpoint name.
*/

QString S60AudioPlayerSession::activeEndpoint() const
{
    QString outputName = QString("Default");
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->AudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

/*!
 *  Returns default Audio End point in use.
*/

QString S60AudioPlayerSession::defaultEndpoint() const
{
    QString outputName = QString("Default");
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->DefaultAudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

/*!
   Sets active end \a name as an Audio End point.
*/

void S60AudioPlayerSession::setActiveEndpoint(const QString& name)
{
    TRACE("S60AudioPlayerSession::setActiveEndpoint" << qtThisPtr() << "name" << name);

#ifdef HAS_AUDIOROUTING
    CAudioOutput::TAudioOutputPreference output = CAudioOutput::ENoPreference;

    if (name == QString("Default"))
        output = CAudioOutput::ENoPreference;
    else if (name == QString("All"))
        output = CAudioOutput::EAll;
    else if (name == QString("None"))
        output = CAudioOutput::ENoOutput;
    else if (name == QString("Earphone"))
        output = CAudioOutput::EPrivate;
    else if (name == QString("Speaker"))
        output = CAudioOutput::EPublic;

    if (m_audioOutput) {
        TRAPD(err, m_audioOutput->SetAudioOutputL(output));
        setError(err);
    }
#endif
}

/*!
    The default audio output has been changed.

    \a aAudioOutput Audio Output object.

    \a aNewDefault is CAudioOutput::TAudioOutputPreference.
*/


#ifdef HAS_AUDIOROUTING
void S60AudioPlayerSession::DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
                                        CAudioOutput::TAudioOutputPreference aNewDefault)
{
    TRACE("S60AudioPlayerSession::DefaultAudioOutputChanged" << qtThisPtr() << "newDefault" << aNewDefault);

    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}


/*!
    Converts CAudioOutput::TAudioOutputPreference enum to QString.

    \a output is  CAudioOutput::TAudioOutputPreference enum value.

*/

QString S60AudioPlayerSession::qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const
{
    if (output == CAudioOutput::ENoPreference)
        return QString("Default");
    else if (output == CAudioOutput::EAll)
        return QString("All");
    else if (output == CAudioOutput::ENoOutput)
        return QString("None");
    else if (output == CAudioOutput::EPrivate)
        return QString("Earphone");
    else if (output == CAudioOutput::EPublic)
        return QString("Speaker");
    return QString("Default");
}
#endif

/*!
    Return True if its Seekable or else False.
*/

bool S60AudioPlayerSession::getIsSeekable() const
{
    return ETrue;
}

