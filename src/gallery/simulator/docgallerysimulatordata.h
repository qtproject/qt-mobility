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
#ifndef DOCGALLERYSIMULATORDATA_H
#define DOCGALLERYSIMULATORDATA_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMetaType>

QTM_BEGIN_NAMESPACE
struct DocGalleryImageItem
{
    QString fileName;
    int width;
    int height;
    QString title;
    QString tags;
};

struct DocGalleryArtistItem
{
    QString artist;
};

struct DocGallerySimulatorData
{
    QList<DocGalleryImageItem> images;
    QList<DocGalleryArtistItem> artists;
};

void qt_registerDocGalleryTypes();
QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QtMobility::DocGallerySimulatorData)

#endif // DOCGALLERYSIMULATORDATA_H
