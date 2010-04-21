/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "cntthumbnailcreator.h"

#include <cntdb.h> 
#include <cntitem.h>

#include <cntdef.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <imageconversion.h>
#include <gdi.h>
#include <bitmaptransforms.h>
#include <bautils.h>

#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

const TSize KThumbnailSizeUninitialized(0, 0);

#ifdef _DEBUG
_LIT(KPanicCategory, "ThumbnailCreator");
enum TPanicReasons {
    KPanicUnitialized = 0
};
#endif

CntThumbnailCreator::CntThumbnailCreator() :
    CActive(EPriorityStandard),
    m_state(EStateInitialized),
    m_thumbnailSize(KThumbnailSizeUninitialized),
    m_thumbnailFieldFromTemplate(0),
    m_err(KErrNone)
{
    CActiveScheduler::Add(this);
}

CntThumbnailCreator::~CntThumbnailCreator()
{
    delete m_encoder;
    delete m_imageData;
    delete m_bitmapScaler;
    delete m_bitmap;
    delete m_decoder;
    delete m_activeSchedulerWait;
    if (m_rfs.Handle() != KNullHandle)
        m_rfs.Close();
}

/*!
 * After calling this function \a fieldList will contain thumbnail field (field
 * types KUidContactFieldVCardMapJPEG, KUidContactFieldPicture in this order)
 * with thumbnail data. If the file \a filename does not exist, no fields are
 * added to the field list. If the file \a filename is not a valid image file,
 * a leave will occur. \a maxSize is the maximum size of the thumbnail image
 * that is stored into contact database. If the image is bigger than \a maxSize
 * the generated thumbnail will be scaled down to \a maxSize.
 */
void CntThumbnailCreator::addThumbnailFieldL(QList<CContactItemField *> *fieldList, const TFileName &filename, const TSize& maxSize)
{
    Cancel();
    m_fieldList = fieldList;
    m_thumbnailSize = maxSize;

    // Lazy instantiation to save resources
    if (m_rfs.Handle() == KNullHandle)
        m_rfs.Connect();
    if(!m_activeSchedulerWait)
        m_activeSchedulerWait = new (ELeave) CActiveSchedulerWait;

    DecodeImageL(filename);

    // Synchronize asynchronous operations (wait loop is finished in RunL when
    // image operations are done, or in RunError if an error occurs)
    m_activeSchedulerWait->Start();
    User::LeaveIfError(m_err);
}

void CntThumbnailCreator::addThumbnailFieldL(QList<CContactItemField *> *fieldList, CFbsBitmap *bitmap, const TSize& maxSize)
{
    Cancel();
    m_fieldList = fieldList;
    m_thumbnailSize = maxSize;
    delete m_bitmap;
    m_bitmap = bitmap;

    // Lazy instantiation to save resources
    if (m_rfs.Handle() == KNullHandle)
        m_rfs.Connect();
    if(!m_activeSchedulerWait)
        m_activeSchedulerWait = new (ELeave) CActiveSchedulerWait;

    m_state = EStateEncodeImage;
    EncodeImageL();

    // Synchronize asynchronous operations (wait loop is finished in RunL when
    // image operations are done, or in RunError if an error occurs)
    m_activeSchedulerWait->Start();
    User::LeaveIfError(m_err);
}


void CntThumbnailCreator::RunL()
{
    __ASSERT_DEBUG(m_activeSchedulerWait, User::Panic(KPanicCategory, KPanicUnitialized));

    User::LeaveIfError(iStatus.Int());
    switch (m_state) {
    case EStateDecodeImage:
    {
        m_state = EStateScaleImage;
        ScaleImageL();
        break;
    }
    case EStateScaleImage:
    {
        m_state = EStateEncodeImage;
        EncodeImageL();
        break;
    }
    case EStateEncodeImage:
    {
        m_state = EStateFinal;
        CreateContactFieldL();
        break;
    }
    case EStateFinal:
    {
        // We are done with the image, back to the caller of convertL
        m_activeSchedulerWait->AsyncStop();
        break;
    }
    default:
        User::Leave(KErrCompletion);
    }
}

void CntThumbnailCreator::DoCancel()
{
    __ASSERT_DEBUG(m_activeSchedulerWait, User::Panic(KPanicCategory, KPanicUnitialized));

    m_err = KErrCancel;
    m_activeSchedulerWait->AsyncStop();
}

TInt CntThumbnailCreator::RunError(TInt aError)
{
    __ASSERT_DEBUG(m_activeSchedulerWait, User::Panic(KPanicCategory, KPanicUnitialized));

    m_err = aError;
    m_activeSchedulerWait->AsyncStop();
    return KErrNone;
}

void CntThumbnailCreator::DecodeImageL(const TFileName &filename)
{
    if (m_rfs.IsValidName(filename) && BaflUtils::FileExists(m_rfs, filename)) {
        m_state = EStateDecodeImage;

        // Create image decoder
        delete m_decoder;
        m_decoder = 0;
        m_decoder = CImageDecoder::FileNewL(m_rfs, filename);
        TFrameInfo frameInfo = m_decoder->FrameInfo();

        // Lazy instantiation
        if(!m_bitmap)
            m_bitmap = new (ELeave) CFbsBitmap;
        User::LeaveIfError(m_bitmap->Create(frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode));

        m_decoder->Convert(&iStatus, *m_bitmap);
    } else {
        m_state = EStateFinal;

        // Ignore "file does not exist" error because the image has probably
        // been removed (leaves thumbnail field empty for the contact).
        TRequestStatus *status = &iStatus;
        User::RequestComplete(status, KErrNone);
    }
    SetActive();
}

void CntThumbnailCreator::ScaleImageL()
{
    __ASSERT_DEBUG(m_decoder, User::Panic(KPanicCategory, KPanicUnitialized));
    __ASSERT_DEBUG(m_thumbnailSize != KThumbnailSizeUninitialized, User::Panic(KPanicCategory, KPanicUnitialized));

    TFrameInfo frameInfo = m_decoder->FrameInfo();

    // Scale down if the image is too big
    if ( frameInfo.iOverallSizeInPixels.iWidth > m_thumbnailSize.iWidth
            || frameInfo.iOverallSizeInPixels.iHeight > m_thumbnailSize.iHeight) {
        // Lazy instantiation
        if(!m_bitmapScaler)
            m_bitmapScaler = CBitmapScaler::NewL();
        m_bitmapScaler->Scale(&iStatus, *m_bitmap, m_thumbnailSize);
    } else {
        // No need to scale, complete request immediately
        TRequestStatus *status = &iStatus;
        User::RequestComplete(status, KErrNone);
    }
    SetActive();
}

void CntThumbnailCreator::EncodeImageL()
{
    delete m_imageData;
    m_imageData = 0;
    delete m_encoder;
    m_encoder = 0;
    m_encoder = CImageEncoder::DataNewL(m_imageData, CImageEncoder::EOptionNone, KImageTypeJPGUid);
    m_encoder->Convert(&iStatus, *m_bitmap);
    SetActive();
}

void CntThumbnailCreator::CreateContactFieldL()
{
    __ASSERT_DEBUG(m_imageData, User::Panic(KPanicCategory, KPanicUnitialized));

    initializeThumbnailFieldL();
    CContactItemField *thumbnailField = CContactItemField::NewLC(*m_thumbnailFieldFromTemplate);

    thumbnailField->StoreStorage()->SetThingL(*m_imageData);
    QT_TRYCATCH_LEAVING(m_fieldList->append(thumbnailField));
    CleanupStack::Pop(thumbnailField);

    // Complete request to proceed to the final state
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}

void CntThumbnailCreator::initializeThumbnailFieldL()
{
    // Assume the golden template is not changed run-time and fetch it only
    // when initializeThumbnailFieldL is called for the first time during the
    // life-time of the CntThumbnailCreator object (requires the instance to
    // live longer than one thumbnail create operation to be effective,
    // otherwise we would end up opening contact database and reading the
    // system template every time a thumbnail is stored for a contact).
    if(!m_thumbnailFieldFromTemplate) {
        CContactDatabase *contactDatabase = CContactDatabase::OpenL();
        CleanupStack::PushL(contactDatabase);
        CContactItem *goldenTemplate = contactDatabase->ReadContactLC(KGoldenTemplateId);
        const CContactItemFieldSet& cardFields = goldenTemplate->CardFields();

        // Check if thumbnail field type is KUidContactFieldPictureValue
        TInt pictureFieldIndex = cardFields.Find(KUidContactFieldPicture, KUidContactFieldVCardMapPHOTO);

        // Check if thumbnail field type is KUidContactFieldVCardMapJPEG
        if(pictureFieldIndex == KErrNotFound) {
            pictureFieldIndex = cardFields.Find(KUidContactFieldVCardMapJPEG, KUidContactFieldVCardMapPHOTO);
        }

        if(pictureFieldIndex == KErrNotFound) {
            // Either KUidContactFieldPictureValue or KUidContactFieldVCardMapJPEG
            // thumbnail field types should be in the template
            User::Leave(KErrNotFound);
        }

        m_thumbnailFieldFromTemplate = CContactItemField::NewL(cardFields[pictureFieldIndex]);
        CleanupStack::PopAndDestroy(goldenTemplate);
        CleanupStack::PopAndDestroy(contactDatabase);
    }
}
