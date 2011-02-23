/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "docgallerysimulatordata.h"

QTM_BEGIN_NAMESPACE

void qt_registerDocGalleryTypes()
{
    qRegisterMetaTypeStreamOperators<DocGallerySimulatorData>("QtMobility::DocGallerySimulatorData");
}

QDataStream &operator<<(QDataStream &out, const DocGallerySimulatorData &s)
{
    out << s.artists << s.images;
    return out;
}

QDataStream &operator<<(QDataStream &out, const DocGalleryArtistItem &i)
{
    out << i.artist;
    return out;
}

QDataStream &operator<<(QDataStream &out, const DocGalleryImageItem &i)
{
    out << i.fileName << i.tags << i.title << i.width << i.height;
    return out;
}

QDataStream &operator>>(QDataStream &in, DocGallerySimulatorData &s)
{
    in >> s.artists >> s.images;
    return in;
}

QDataStream &operator>>(QDataStream &in, DocGalleryArtistItem &i)
{
    in >> i.artist;
    return in;
}

QDataStream &operator>>(QDataStream &in, DocGalleryImageItem &i)
{
    in >> i.fileName >> i.tags >> i.title >> i.width >> i.height;
    return in;
}

QTM_END_NAMESPACE
