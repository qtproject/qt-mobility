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
#ifndef THUMBNAIL_CREATOR_H
#define THUMBNAIL_CREATOR_H

#include <qlist.h>
#include <e32base.h>
#include <f32file.h>

class CContactItemField;
class CImageDecoder;
class CFbsBitmap;
class CBitmapScaler;
class CImageEncoder;

class CntThumbnailCreator : public CActive
{
    enum TCreatorState {
        EStateInitialized = 0,
        EStateDecodeImage,
        EStateScaleImage,
        EStateEncodeImage,
        EStateFinal
    };

public:
    CntThumbnailCreator();
    ~CntThumbnailCreator();
    void addThumbnailFieldL(QList<CContactItemField *> *fieldList, const TFileName &filename, const TSize& maxSize);
    void addThumbnailFieldL(QList<CContactItemField *> *fieldList, CFbsBitmap *bitmap, const TSize& maxSize);

private: // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    void DecodeImageL(const TFileName &filename);
    void ScaleImageL();
    void EncodeImageL();
    void CreateContactFieldL();
    void initializeThumbnailFieldL();

private:
    TCreatorState m_state;
    TSize m_thumbnailSize;
    CContactItemField *m_thumbnailFieldFromTemplate;
    QList<CContactItemField *> *m_fieldList;
    CActiveSchedulerWait *m_activeSchedulerWait;
    TInt m_err;
    RFs m_rfs;
    CImageDecoder *m_decoder;
    CFbsBitmap *m_bitmap;
    CBitmapScaler *m_bitmapScaler;
    HBufC8 *m_imageData;
    CImageEncoder *m_encoder;
};

#endif
