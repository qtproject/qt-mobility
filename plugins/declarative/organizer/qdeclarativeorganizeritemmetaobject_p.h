/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEORGANIZERITEMMETAOBJECT_H
#define QDECLARATIVEORGANIZERITEMMETAOBJECT_H

#include <QDeclarativeListProperty>

#include "qdeclarativeopenmetaobject_p.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qdeclarativeorganizeritemdetail_p.h"

QTM_USE_NAMESPACE


struct OrganizerItemDetailNameMap
{
    QDeclarativeOrganizerItemDetail::ItemDetailType type;
    const char* name;
    const char* definitionName;
    bool group;
};


class QDeclarativeOrganizerItemMetaObject : public QDeclarativeOpenMetaObject
{
public:
    QDeclarativeOrganizerItemMetaObject(QObject* obj, const QOrganizerItem& item);

    ~QDeclarativeOrganizerItemMetaObject();

    virtual void getValue(int id, void **a);
    virtual void setValue(int id, void **a);
    virtual int createProperty(const char *,  const char *);

    QVariant detail(const QString& name);
    QVariant details(const QString& name);

    QVariant detail(QDeclarativeOrganizerItemDetail::ItemDetailType type);
    QVariant details(QDeclarativeOrganizerItemDetail::ItemDetailType type);

    void setItem(const QOrganizerItem& item);
    QOrganizerItem item();
    uint itemId() const;

    static void detail_append(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p, QDeclarativeOrganizerItemDetail *detail);
    static int  detail_count(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p);
    static QDeclarativeOrganizerItemDetail * detail_at(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p, int idx);
    static void  detail_clear(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p);

    static QDeclarativeOrganizerItemDetail* createItemDetail(QDeclarativeOrganizerItemDetail::ItemDetailType type, QObject* parent = 0);

    static OrganizerItemDetailNameMap* detailMetaDataByDetailName(const char * name);
    static OrganizerItemDetailNameMap* detailMetaDataByDefinitionName(const char * name);
    static OrganizerItemDetailNameMap* detailMetaDataByDetailType(QDeclarativeOrganizerItemDetail::ItemDetailType type);

    bool m_modified;
    QOrganizerItem m_item;
    QMap<QString, QOrganizerItemDetailDefinition> m_defs;
    QList<QDeclarativeOrganizerItemDetail*> m_details;
    QHash<int, OrganizerItemDetailNameMap*> m_properties;
    friend class QDeclarativeOrganizerItem;
};

#endif // QDECLARATIVEORGANIZERITEMMETAOBJECT_H
