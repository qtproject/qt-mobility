/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediametadataprovider.h"
#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"
#include <QtCore/qdebug.h>

S60MediaMetaDataProvider::S60MediaMetaDataProvider(QObject *control, QObject *parent)
    : QMetaDataReaderControl(parent)
    , m_control(NULL)
{
    m_control = qobject_cast<S60MediaPlayerControl*>(control);
}

S60MediaMetaDataProvider::~S60MediaMetaDataProvider()
{
}

bool S60MediaMetaDataProvider::isMetaDataAvailable() const
{
    if (m_control->session())
       return m_control->session()->isMetadataAvailable();
    return false;
}

bool S60MediaMetaDataProvider::isWritable() const
{
    return false;
}

QVariant S60MediaMetaDataProvider::metaData(QtMultimediaKit::MetaData key) const
{

    if (m_control->session())
        return m_control->session()->metaData(key);
    return QVariant();
}

QList<QtMultimediaKit::MetaData> S60MediaMetaDataProvider::availableMetaData() const
{
    if (m_control->session())
        return m_control->session()->availableMetaData();
    return QList<QtMultimediaKit::MetaData>();
}

QVariant S60MediaMetaDataProvider::extendedMetaData(const QString &key) const
{

    if (m_control->session())
        return m_control->session()->metaData(key);
    return QVariant();
}

QStringList S60MediaMetaDataProvider::availableExtendedMetaData() const
{

    if (m_control->session())
        return m_control->session()->availableExtendedMetaData();
    return QStringList();
}
