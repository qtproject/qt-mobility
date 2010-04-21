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
#include "cnttransformthumbnail.h"
#include "cntmodelextuids.h"

#include "qcontactthumbnail.h"
#include "cntsymbiantransformerror.h"

#include <QPixmap>
#include <QImage>

const TSize KMaxThumbnailSize(80, 96);

CntTransformThumbnail::CntTransformThumbnail() :
    m_thumbnailCreator(0)
{
}

CntTransformThumbnail::~CntTransformThumbnail()
{
    delete m_thumbnailCreator;
}

QList<CContactItemField *> CntTransformThumbnail::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactThumbnail::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to thumbnail
    const QContactThumbnail &thumbnail(static_cast<const QContactThumbnail&>(detail));

    if(!thumbnail.thumbnail().isNull()) {
        // lazy instantiation
        if(!m_thumbnailCreator) {
            m_thumbnailCreator = new (ELeave) CntThumbnailCreator();
        }

        // Scaling is done before converting to CFbsBitmap because
        // toSymbianCFbsBitmap may generate a duplicate of the bitmap data
        // Note: scaling to thumbnail may take some time if the image is big
        // TODO: aspect ratio?
        QPixmap scaled = QPixmap::fromImage(thumbnail.thumbnail().scaled(KMaxThumbnailSize.iWidth, KMaxThumbnailSize.iHeight));
        CFbsBitmap* bitmap = scaled.toSymbianCFbsBitmap();
        CleanupStack::PushL(bitmap);

        m_thumbnailCreator->addThumbnailFieldL(&fieldList, bitmap, KMaxThumbnailSize);
        CleanupStack::Pop(bitmap); // ownership transferred
    }

    return fieldList;
}

QContactDetail *CntTransformThumbnail::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

    QContactThumbnail *thumbnail = new QContactThumbnail();

    if (field.ContentType().ContainsFieldType(KUidContactFieldPicture)
        || field.ContentType().ContainsFieldType(KUidContactFieldVCardMapJPEG)) {
        // use the existing QContactAvatar (if available) in case of a pixmap
        // field.
        CContactStoreField* storage = field.StoreStorage();
        QImage image;
        HBufC8 *theThing = storage->Thing();
        QByteArray bytes((char*)theThing->Ptr(), theThing->Length());
        bool loaded = image.loadFromData(bytes, "JPG");
        if (loaded) {
            thumbnail->setThumbnail(image);
        } else {
            User::Leave(KErrInvalidContactDetail);
        }
    }

    return thumbnail;
}

bool CntTransformThumbnail::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldPicture.iUid
        // Used as "extra mapping/extra field type" by thumbnail data fields
        || fieldType == KUidContactFieldVCardMapJPEG.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformThumbnail::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactThumbnail::DefinitionName) {
        ret = true;
    }
    return ret;
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
    Q_UNUSED(definitions);
    // Don't need to munge the definition
}
