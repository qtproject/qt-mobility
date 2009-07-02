/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstringlist.h>

#include "mpddaemon.h"
#include "mpdmetadata.h"



MpdMetadata::MpdMetadata(MpdDaemon *mpd, QObject *parent):
    QMetadataProvider(parent)
{
    daemon = mpd;
    connect(daemon, SIGNAL(playerChanged()), SLOT(playerChanged()));
}

MpdMetadata::~MpdMetadata()
{
}

bool MpdMetadata::metadataAvailable() const
{
    return !saved.isEmpty();
}

bool MpdMetadata::isReadOnly() const
{
    return true;
}

QList<QString> MpdMetadata::availableMetadata() const
{
    return saved.keys();
}

QVariant MpdMetadata::metadata(QString const &name) const
{
    return saved[name];
}

void MpdMetadata::setMetadata(QString const &name, QVariant const &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

void MpdMetadata::playerChanged()
{
    saved.clear();

    bool ok = false;
    QStringList r = daemon->send(QString("playlistinfo %1").arg(daemon->currentSongPos()), &ok);

    if (ok) {
        foreach (QString const &line, r) {
            QString name = line.section(':', 0, 0);
            QString value = line.section(':', 1);

            saved.insert(name, value);
        }
    }
}
