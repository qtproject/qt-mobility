/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QDECLARATIVECONTACTMETAOBJECT_H
#define QDECLARATIVECONTACTMETAOBJECT_H

#include <QDeclarativeListProperty>

#include "qdeclarativeopenmetaobject_p.h"
#include "qdeclarativecontactdetail_p.h"

QTM_USE_NAMESPACE


struct ContactDetailNameMap
{
    QDeclarativeContactDetail::ContactDetailType type;
    const char* name;
    const char* definitionName;
    bool group;
};


class QDeclarativeContactMetaObject : public QDeclarativeOpenMetaObject
{
public:
    QDeclarativeContactMetaObject(QObject* obj, const QContact& contact);

    ~QDeclarativeContactMetaObject();

    virtual void getValue(int id, void **a);
    virtual void setValue(int id, void **a);
    virtual int createProperty(const char *,  const char *);

    QVariant detail(const QString& name);
    QVariant detail(QDeclarativeContactDetail::ContactDetailType type);

    QVariant details(const QString& name);
    QVariant details(QDeclarativeContactDetail::ContactDetailType type);
    void setContact(const QContact& contact);
    QContact contact();
    QContactId contactId() const;
    int localId() const;


    static void detail_append(QDeclarativeListProperty<QDeclarativeContactDetail> *p, QDeclarativeContactDetail *detail);
    static int  detail_count(QDeclarativeListProperty<QDeclarativeContactDetail> *p);
    static QDeclarativeContactDetail* detail_at(QDeclarativeListProperty<QDeclarativeContactDetail> *p, int idx);
    static void  detail_clear(QDeclarativeListProperty<QDeclarativeContactDetail> *p);

    static QDeclarativeContactDetail* createContactDetail(QDeclarativeContactDetail::ContactDetailType type, QObject* parent = 0);

    static ContactDetailNameMap* detailMetaDataByDetailName(const char * name);
    static ContactDetailNameMap* detailMetaDataByDefinitionName(const char * name);
    static ContactDetailNameMap* detailMetaDataByDetailType(QDeclarativeContactDetail::ContactDetailType type);

    bool m_modified;
    QContact m_contact;
    QMap<QString, QContactDetailDefinition> m_defs;
    QList<QDeclarativeContactDetail*> m_details;
    QHash<int, ContactDetailNameMap*> m_properties;
    friend class QDeclarativeContact;
};

#endif // QDECLARATIVECONTACTMETAOBJECT_H
