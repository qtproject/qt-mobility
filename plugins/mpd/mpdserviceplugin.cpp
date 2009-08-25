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

#include <QtCore/qstring.h>

#include <qmediaserviceprovider.h>


#include "mpdserviceplugin.h"
#include "mpdplayerservice.h"


class MpdProvider : public QMediaServiceProvider
{
    Q_OBJECT

public:
    QObject* createObject(const char *interface) const
    {
        if (QLatin1String(interface) == QLatin1String(QMediaPlayerService_iid))
            return new MpdPlayerService;

        return 0;
    }
};

QStringList MpdServicePlugin::keys() const
{
    return QStringList() << "mediaplayer";
}

QMediaServiceProvider* MpdServicePlugin::create(QString const& key)
{
    if (key == "mediaplayer")
        return new MpdProvider;

    return 0;
}

Q_EXPORT_PLUGIN2(mpd_serviceplugin, MpdServicePlugin);

#include "mpdserviceplugin.moc"

