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
#ifndef TRANSFORMCONTACTDATA_H
#define TRANSFORMCONTACTDATA_H

#include <QObject>
#include <qtcontacts.h>

#include <cntfldst.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>

QTM_USE_NAMESPACE

class CntTransformContactData : public QObject
{
	 Q_OBJECT

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    void transformToTextFieldL(const QContactDetail &detail, QList<CContactItemField *> &fieldList, const QString &detailValue, const TUid uid, const TUid vcardMapping, const bool setContext);
	void setContexts(const TUid &fieldType, QContactDetail &detail);
	void setContextsL(const QContactDetail &detail, CContactItemField &field);

public:
	virtual QList<CContactItemField *> transformDetailL(const QContactDetail &detail) = 0;
	virtual QContactDetail* transformItemField(const CContactItemField& field, const QContact &contact) = 0;
	virtual bool supportsField(TUint32 fieldType) const
	{
	    TUid fieldTypeUid;
	    fieldTypeUid.iUid = fieldType;
	    return supportedFields().contains(fieldTypeUid);
	};
	virtual bool supportsDetail(QString detailName) const = 0;
    virtual QList<TUid> supportedFields() const = 0;
	virtual QList<TUid> supportedSortingFieldTypes(QString detailFieldName) const = 0;
	virtual bool supportsSubType(const QString& detailName) const = 0;
	virtual quint32 getIdForField(const QString& detailName) const = 0;
	virtual void detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const = 0;
};

#endif
