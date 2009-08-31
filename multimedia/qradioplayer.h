/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QRADIOPLAYER_H
#define QRADIOPLAYER_H

#include <QtCore/qobject.h>

#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"
#include "qmediaserviceprovider.h"

class QRadioService;
class QRadioPlayerPrivate;


class Q_MEDIA_EXPORT QRadioPlayer : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(Band band READ band WRITE setBand NOTIFY bandChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool stereo READ isStereo WRITE setStereo NOTIFY stereoStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool searching READ isSearching NOTIFY searchingStatusChanged)
    Q_ENUMS(Band)

public:
    enum Band { AM, FM, SW, LW };

    QRadioPlayer(QObject *parent = 0, QRadioService *service = 0);
    ~QRadioPlayer();

    bool isValid() const;

    Band band() const;

    bool isSupportedBand(Band b) const;

    int frequency() const;

    bool isStereo() const;

    int signalStrength() const;

    qint64 duration() const;

    int volume() const;
    bool isMuted() const;

    bool isSearching() const;

    QAbstractMediaService* service() const;

public Q_SLOTS:
    void searchForward();
    void searchBackward();
    void cancelSearch();

    void setBand(Band band);
    void setFrequency(int frequency);
    void setStereo(bool stereo);

    void setVolume(int volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void bandChanged(QRadioPlayer::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void searchingStatusChanged(bool searching);
    void signalStrengthChanged(int signalStrength);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);

private:
    Q_DISABLE_COPY(QRadioPlayer)
    Q_DECLARE_PRIVATE(QRadioPlayer)
};

#endif  // QRADIOPLAYER_H
