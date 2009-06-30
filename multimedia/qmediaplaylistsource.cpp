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

#include "qmediaplaylistsource.h"
#include "qmediaplaylistsource_p.h"

/*!
    \class QMediaPlaylistSource
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

QMediaPlaylistSource::QMediaPlaylistSource(QObject *parent)
    :QObject(*new QMediaPlaylistSourcePrivate, parent)
{
}

QMediaPlaylistSource::QMediaPlaylistSource(QMediaPlaylistSourcePrivate &dd, QObject *parent)
    :QObject(dd,parent)
{
}

QMediaPlaylistSource::~QMediaPlaylistSource()
{
}

/*!
    \fn QMediaPlaylistSource::size() const;
*/

/*!
    \fn QMediaPlaylistSource::operator [](int pos) const;
*/

bool QMediaPlaylistSource::load(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::load(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::save(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::save(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::isReadOnly() const
{
    return true;
}

bool QMediaPlaylistSource::append(const QMediaSource &source)
{
    Q_UNUSED(source);
    return false;
}

bool QMediaPlaylistSource::append(const QList<QMediaSource> &sources)
{
    foreach(const QMediaSource& source, sources ) {
        if (!append(source))
            return false;
    }

    return true;
}

bool QMediaPlaylistSource::insert(int pos, const QMediaSource &source)
{
    Q_UNUSED(pos);
    Q_UNUSED(source);
    return false;
}

bool QMediaPlaylistSource::remove(int pos)
{
    Q_UNUSED(pos);
    return false;
}

bool QMediaPlaylistSource::remove(int start, int end)
{
    for (int pos=start; pos<=end; pos++) {
        if (!remove(pos))
            return false;
    }

    return true;
}

bool QMediaPlaylistSource::clear()
{
    return remove(0, size()-1);
}

void QMediaPlaylistSource::shuffle()
{
}
