/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLOCALMEDIAPAYLISTPROVIDER_H
#define QLOCALMEDIAPAYLISTPROVIDER_H

#include "qmediaplaylistprovider.h"

QT_BEGIN_NAMESPACE

class QLocalMediaPlaylistProviderPrivate;
class Q_MULTIMEDIA_EXPORT QLocalMediaPlaylistProvider : public QMediaPlaylistProvider
{
    Q_OBJECT
public:
    QLocalMediaPlaylistProvider(QObject *parent=0);
    virtual ~QLocalMediaPlaylistProvider();

    virtual int mediaCount() const;
    virtual QMediaContent media(int pos) const;

    virtual bool isReadOnly() const;

    virtual bool addMedia(const QMediaContent &content);
    virtual bool addMedia(const QList<QMediaContent> &items);
    virtual bool insertMedia(int pos, const QMediaContent &content);
    virtual bool insertMedia(int pos, const QList<QMediaContent> &items);
    virtual bool removeMedia(int pos);
    virtual bool removeMedia(int start, int end);
    virtual bool clear();

public Q_SLOTS:
    virtual void shuffle();

private:
    Q_DECLARE_PRIVATE(QLocalMediaPlaylistProvider)
};

QT_END_NAMESPACE

#endif // QLOCALMEDIAPAYLISTSOURCE_H
