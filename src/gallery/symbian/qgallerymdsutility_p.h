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

#ifndef QDOCUMENTGALLERYMDSUTILITY_P_H
#define QDOCUMENTGALLERYMDSUTILITY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QString.h>
#include <QDateTime>
//API
#include "qgalleryqueryrequest.h"
// Symbian
#include <qobject.h>
#include <qdocumentgallery.h>
#include <mdeobject.h>
#include <mdequery.h>
#include <e32std.h>

QTM_BEGIN_NAMESPACE

class QDocumentGalleryMDSUtility
{
public:

    enum TPropertyKeys
    {
        EUri = 0,
        EFileName,
        EFilePath,
        EFileSize,
        ELastModified,
        ETitle,
        EMime,
        EAuthor,
        ECopyright,
        EDescription,
        EComments,
        ERating,
        EDuration,
        EPerformer, //=also maps to artist
        EAudioCodec,
        EAudioBitrate,
        EPlayCount,
        ESampleRate,
        EAlbumTitle,        
        EAlbumArtist,        
        ETrackNumber,        
        EGenre,        
        EComposer,
        EWidth,
        EHeight,
        EOrientation,
        EDateTaken,
        ECameraManufacturer,
        ECameraModel,
        EExposureProgram,
        EExposureTime,
        EFNumber,
        EFlashEnabled,
        EFocalLength,
        EMeteringMode,
        EWhiteBalance,
        EVideoBitrate,
        ELanguage,
        EFrameRate,
        EResumePosition,
        EPropertyKeysLength
    };

    QDocumentGalleryMDSUtility();
    ~QDocumentGalleryMDSUtility();

    /**
     * Maps Document Gallery itemtype to MdE object class
     *
     */
    static CMdEObjectDef& ObjDefFromItemTypeL( CMdENamespaceDef &nameSpace, QString itemType );
    static void GetDataFieldsForItemType( QStringList &propertyList, QString itemType );
    static void GetMetaDataFieldL( CMdEObject *inputItem, QVariant &output, int key );
    static QString GetItemTypeFromMDEObject( CMdEObject *inputItem );
    static bool isItemTypeSupported( const QString &itemType);
    static int GetPropertyKey( const QString &property );
    static QVariant::Type GetPropertyType( int key );

    static QString s60DescToQString(const TDesC& desc);
    static HBufC *qStringToS60Desc(const QString& string);
    static QDateTime symbianTTimetoQDateTime(const TTime& time);
    static TTime QDateTimetosymbianTTime(const QDateTime& time);

    static CMdEPropertyDef *GetMDSPropertyDefL(const QString &property,
        CMdENamespaceDef& defaultNameSpace);
    static int SetupQueryConditions(CMdEObjectQuery *query,
        QGalleryQueryRequest *request,
        CMdENamespaceDef& defaultNameSpace);

    static bool SetMetaDataFieldL( CMdEObject *item, const QVariant &value, int key );

    static int InsertUInt32PropertyCondition( CMdELogicCondition &rootCond,
        CMdEPropertyDef *propDef,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );
    static int InsertTTImePropertyCondition( CMdELogicCondition &rootCond,
        CMdEPropertyDef *propDef,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );
    static int InsertStringPropertyCondition( CMdELogicCondition &rootCond,
        CMdEPropertyDef *propDef,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );
    static int InsertUriPropertyCondition( CMdELogicCondition &rootCond,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );
    static int InsertUIntPropertyCondition( CMdELogicCondition &rootCond,
        CMdEPropertyDef *propDef,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );
    static int InsertReal32PropertyCondition( CMdELogicCondition &rootCond,
        CMdEPropertyDef *propDef,
        QVariant &valueToMatch,
        QGalleryMetaDataFilter &filter );

    static int AddFilter(CMdELogicCondition &rootCond,
        QGalleryFilter &filter,
        CMdENamespaceDef& defaultNameSpace);
    static int AddIntersectionFilter(CMdELogicCondition &rootCond,
        QGalleryFilter &filter,
        CMdENamespaceDef& defaultNameSpace);
    static int AddUnionFilter(CMdELogicCondition &rootCond,
        QGalleryFilter &filter,
        CMdENamespaceDef& defaultNameSpace);
    static int AddMetadataFilter(CMdELogicCondition &rootCond,
        QGalleryFilter &filter,
        CMdENamespaceDef& defaultNameSpace);
    
private:

#ifdef MDS_25_COMPILATION_ENABLED
    static CMdEObjectDef& ObjDefFromItemTypeForMDS25L( CMdENamespaceDef &nameSpace, QString itemType );

    static void GetMetaDataFieldForMDS25L( CMdEObject *inputItem, QVariant &output, int key );

    static CMdEPropertyDef *GetMDSPropertyDefForMDS25L(const QString &property,
                                                       CMdENamespaceDef& defaultNameSpace);

    static bool SetMetaDataFieldForMDS25L( CMdEObject *item, const QVariant &value, int key );

#else
    static CMdEObjectDef* ObjDefFromItemTypeForMDS20L( CMdENamespaceDef &nameSpace, QString itemType );

    static void GetMetaDataFieldForMDS20L( CMdEObject *inputItem, QVariant &output, int key );

    static CMdEPropertyDef *GetMDSPropertyDefForMDS20L(const QString &property,
                                                       CMdENamespaceDef& defaultNameSpace);

    static bool SetMetaDataFieldForMDS20L( CMdEObject *item, const QVariant &value, int key );
    static bool setMdeStringProperty(CMdEObject *item, const QVariant &value, CMdEPropertyDef *propDef);
#endif // MDS_25_COMPILATION_ENABLED
    
};

QTM_END_NAMESPACE

#endif // QDOCUMENTGALLERYMDSUTILITY_H
