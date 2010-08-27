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

#ifndef QDOCUMENTGALLERYMDSUTILITY_H
#define QDOCUMENTGALLERYMDSUTILITY_H

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
        EPerformer,
        EAudioCodec,
        EAudioBitrate,
        EPlayCount,
        ESampleRate,
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
        EResumePosition
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
    static int GetPropertyKey( const QString &property );
    static QVariant::Type GetPropertyType( int key );

    static QString s60DescToQString(const TDesC& desc);
    static HBufC *qStringToS60Desc(const QString& string);
    static QString s60Desc8ToQString(const TDesC8& desc);
    static HBufC8 *qStringToS60Desc8(const QString& string);
	static QDateTime symbianTTimetoQDateTime(const TTime& time);
	static TTime QDateTimetosymbianTTime(const QDateTime& time);

    static CMdEPropertyDef *GetMDSPropertyDefL(const QString &property,
                                               CMdENamespaceDef& defaultNameSpace);
    static void SetupQueryConditions(CMdEObjectQuery *query,
                                     QGalleryQueryRequest *request,
                                     CMdENamespaceDef& defaultNameSpace);

    static bool SetMetaDataFieldL( CMdEObject *item, const QVariant &value, int key );
};

QTM_END_NAMESPACE

#endif // QDOCUMENTGALLERYMDSUTILITY_H
