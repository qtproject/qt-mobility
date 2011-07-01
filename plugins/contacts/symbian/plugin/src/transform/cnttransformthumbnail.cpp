/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "cnttransformthumbnail.h"
#include "cntmodelextuids.h"

#include "qcontactthumbnail.h"
#include "cntsymbiantransformerror.h"

#include <QPixmap>
#include <QImage>
#include <QBuffer>

const QString KThumbnailJpgImage = "jpg_image";
const QString KThumbnailChecksum = "checksum";

CntTransformThumbnail::CntTransformThumbnail() :
    m_thumbnailFieldFromTemplate(NULL)
{
}

CntTransformThumbnail::~CntTransformThumbnail()
{
    delete m_thumbnailFieldFromTemplate;
}

QList<CContactItemField *> CntTransformThumbnail::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactThumbnail::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to thumbnail
    const QContactThumbnail &thumbnail(static_cast<const QContactThumbnail&>(detail));

    //if thumbnail was not changed, use existing jpg image
    bool checksumExists;
    qint64 checksum = thumbnail.variantValue(KThumbnailChecksum).toLongLong(&checksumExists);
    if (!thumbnail.thumbnail().isNull() && checksumExists &&
        thumbnail.thumbnail().cacheKey() == checksum) {
        initializeThumbnailFieldL();
        CContactItemField *thumbnailField = CContactItemField::NewLC(*m_thumbnailFieldFromTemplate);
        QByteArray jpgData = thumbnail.variantValue(KThumbnailJpgImage).toByteArray();
        TPtrC8 ptr((TUint8*)jpgData.data(), jpgData.size());
        thumbnailField->StoreStorage()->SetThingL(ptr);
        fieldList.append(thumbnailField);
        CleanupStack::Pop(thumbnailField);
    } else if (!thumbnail.thumbnail().isNull()) {
        QByteArray jpgImage;
        QImage scaledImage;
        if (thumbnail.thumbnail().size().width() <= KMaxThumbnailSize.iWidth &&
            thumbnail.thumbnail().size().height() <= KMaxThumbnailSize.iHeight) {
            scaledImage = thumbnail.thumbnail();
        } else {
            scaledImage = thumbnail.thumbnail().scaled(
                    KMaxThumbnailSize.iWidth, KMaxThumbnailSize.iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if (!scaledImage.isNull()) {
            QBuffer buffer(&jpgImage);
            buffer.open(QIODevice::WriteOnly);
            scaledImage.save(&buffer, "JPG");
            buffer.close();
            initializeThumbnailFieldL();
            CContactItemField *thumbnailField = CContactItemField::NewLC(*m_thumbnailFieldFromTemplate);
            TPtrC8 ptr((TUint8*)jpgImage.data(), jpgImage.size());
            thumbnailField->StoreStorage()->SetThingL(ptr);
            fieldList.append(thumbnailField);
            CleanupStack::Pop(thumbnailField);
        }
    }

    return fieldList;
}

QContactDetail *CntTransformThumbnail::transformItemField(const CContactItemField& field, const QContact &contact)
{
    Q_UNUSED(contact);

    QContactThumbnail *thumbnail = NULL;

    if (field.ContentType().ContainsFieldType(KUidContactFieldPicture)
        || field.ContentType().ContainsFieldType(KUidContactFieldVCardMapJPEG)) {
        // check storage type
        if (field.StorageType() != KStorageTypeStore) {
            return NULL;
        }
        
        // Create new field
        CContactStoreField* storage = field.StoreStorage();
        QImage image;
        HBufC8 *theThing = storage->Thing();
        if (theThing != NULL) {
            QByteArray bytes((char*)theThing->Ptr(), theThing->Length());
            bool loaded = image.loadFromData(bytes, "JPG");
            if (loaded) {
                // Create thumbnail detail
                thumbnail = new QContactThumbnail();
                thumbnail->setThumbnail(image);
                // Keep jpg image, so no need for conversion when saving thumbnail detail. 
                thumbnail->setValue(KThumbnailChecksum, image.cacheKey());
                thumbnail->setValue(KThumbnailJpgImage, bytes);
            }
        }
    }

    return thumbnail;
}

bool CntTransformThumbnail::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactThumbnail::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformThumbnail::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldPicture
        // Used as "extra mapping/extra field type" by thumbnail data fields
        << KUidContactFieldVCardMapJPEG;
}

QList<TUid> CntTransformThumbnail::supportedSortingFieldTypes(QString /*detailFieldName*/) const
{
    // Sorting not supported
    return QList<TUid>();
}


/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformThumbnail::supportsSubType(const QString& /*subType*/) const
{
    // XXX todo
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformThumbnail::getIdForField(const QString& /*fieldName*/) const
{
    // XXX todo
    return 0;
}

/*!
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformThumbnail::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactThumbnail::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactThumbnail::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        
        QContactDetailFieldDefinition f1;
        f1.setDataType(QVariant::LongLong);
        fields.insert(KThumbnailChecksum, f1);

        QContactDetailFieldDefinition f2;
        f2.setDataType(QVariant::ByteArray);
        fields.insert(KThumbnailJpgImage, f2);
        
        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}

void CntTransformThumbnail::initializeThumbnailFieldL()
{
    // Assume the golden template is not changed run-time and fetch it only
    // when initializeThumbnailFieldL is called for the first time during the
    // life-time of the CntTransformThumbnail object (requires the instance to
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
