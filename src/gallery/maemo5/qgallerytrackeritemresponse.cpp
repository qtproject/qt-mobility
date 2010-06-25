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

#include "qgallerytrackeritemresponse_p.h"

#include "qgallerytrackeritemlist_p_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerItemResponsePrivate : public QGalleryTrackerItemListPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerItemResponse)
public:
    QGalleryTrackerItemResponsePrivate(
            const QGalleryTrackerItemListArguments &arguments,
            const QGalleryDBusInterfacePointer &metaDataInterface,
            bool live,
            int cursorPosition,
            int minimumPagedItems)
        : QGalleryTrackerItemListPrivate(arguments, live, cursorPosition, minimumPagedItems)
        , metaDataInterface(metaDataInterface)
        , fieldNames(arguments.fieldNames)
    {
    }

    const QGalleryDBusInterfacePointer metaDataInterface;
    const QStringList fieldNames;
};

QGalleryTrackerItemResponse::QGalleryTrackerItemResponse(
        const QGalleryTrackerItemListArguments &arguments,
        const QGalleryDBusInterfacePointer &metaDataInterface,
        bool live,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerItemList(
            *new QGalleryTrackerItemResponsePrivate(
                    arguments, metaDataInterface, live, cursorPosition, minimumPagedItems),
            parent)
{
}

QGalleryTrackerItemResponse::~QGalleryTrackerItemResponse()
{
}

void QGalleryTrackerItemResponse::setMetaData(int index, int key, const QVariant &value)
{
    Q_D(QGalleryTrackerItemResponse);

    QVector<QVariant>::const_iterator row;

    if (key < d->imageOffset) {
        if (key >= d->aliasOffset) {
            key = d->aliasColumns.at(key - d->aliasOffset);
        } else if (key < d->valueOffset || key >= d->compositeOffset) {
            return;
        }

        if (index < d->rCache.cutoff) {
            if ((index -= d->rCache.index) >= 0) {
                row = d->rCache.values.constBegin() + (index * d->tableWidth);
            } else {
                return;
            }
        } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
            row = d->aCache.values.begin() + (index * d->tableWidth);
        } else {
            return;
        }
    }

    if (*(row + key) == value)
        return;

    QGalleryTrackerMetaDataEdit *edit = 0;

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it) {
        if ((*it)->index() == index) {
            edit = *it;
            break;
        }
    }

    if (!edit) {
        edit = new QGalleryTrackerMetaDataEdit(
                d->metaDataInterface, row->toString(), (row + 1)->toString(), this);
        edit->setIndex(index);

        connect(edit, SIGNAL(finished(QGalleryTrackerMetaDataEdit*)),
                this, SLOT(_q_editFinished(QGalleryTrackerMetaDataEdit*)));

        connect(this, SIGNAL(inserted(int,int)), edit, SLOT(itemsInserted(int,int)));
        connect(this, SIGNAL(removed(int,int)), edit, SLOT(itemsRemoved(int,int)));

        d->edits.append(edit);

        d->requestUpdate();
    }

    edit->setValue(
            d->fieldNames.at(key - d->valueOffset),
            d->valueColumns.at(key - d->valueOffset)->toString(value));
}

#include "moc_qgallerytrackeritemresponse_p.cpp"

QTM_END_NAMESPACE
