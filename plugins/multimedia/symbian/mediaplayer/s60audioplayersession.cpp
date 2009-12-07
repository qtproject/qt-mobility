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

#include "s60audioplayersession.h"

#include <QtCore/qdebug.h>
#include <QVariant>

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
{    
    TRAP_IGNORE(m_player = CMdaAudioPlayerUtility::NewL(*this, 0, EMdaPriorityPreferenceNone));
    //TODO: Error handlind if creating audio player fails
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    delete m_player;
}

void S60AudioPlayerSession::doLoad(const TDesC &path)
{
    TRAPD(err, m_player->OpenFileL(path));
    if (err) {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

qint64 S60AudioPlayerSession::duration() const
{
    return m_player->Duration().Int64() / 1000;
}

qint64 S60AudioPlayerSession::position() const
{
    TTimeIntervalMicroSeconds ms = 0;
    m_player->GetPosition(ms);
    out << (ms.Int64() / 1000) << "\n";
    return ms.Int64() / 1000;
}

void S60AudioPlayerSession::doSetPlaybackRate(qreal rate)
{
    // TODO:
}

bool S60AudioPlayerSession::isVideoAvailable() const
{
    return false;
}

void S60AudioPlayerSession::doPlay()
{
    m_player->Play();
}

void S60AudioPlayerSession::doPause()
{
    m_player->Pause();
}

void S60AudioPlayerSession::doStop()
{
    m_player->Stop();
    m_player->Close();
}

void S60AudioPlayerSession::doSetVolume(int volume)
{    
    m_player->SetVolume((volume / 100.0) * m_player->MaxVolume());
}

void S60AudioPlayerSession::doSetPosition(qint64 microSeconds)
{   
    m_player->SetPosition(TTimeIntervalMicroSeconds(microSeconds));
}

void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
{
    Q_UNUSED(aDuration);
    setError(aError);
    initComplete();
}

void S60AudioPlayerSession::updateMetaDataEntries()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    
    m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);
    
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAPD(err, entry = m_player->GetMetaDataEntryL(i));

        if (err == KErrNone) {
            metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        }
        delete entry;
    }
    emit metaDataChanged();
}

void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
{
    setError(aError);
    playComplete();
}

