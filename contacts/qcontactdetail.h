/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTDETAIL_H
#define QCONTACTDETAIL_H

#include "qtcontactsglobal.h"
#include <QSharedDataPointer>
#include <QStringList>
#include <QVariant>

class QContactDetailPrivate;
class QTCONTACTS_EXPORT QContactDetail
{
public:
    QContactDetail();
    QContactDetail(const QString& definitionName);
    virtual ~QContactDetail();

    QContactDetail(const QContactDetail& other);
    QContactDetail& operator=(const QContactDetail& other);

    /* The values of the Error enum are still to be decided! */
    enum Error {
        NoError = 0,
        MissingValueError,
        IncompatibleAssignmentError,
        OutOfMemoryError,
    };

    // Predefined attribute names and values
#ifdef Q_QDOC
    const char* AttributeContext;
    const char* AttributeSubType;
    const char* AttributeContextHome;
    const char* AttributeContextWork;
    const char* AttributeContextOther;
#else
    Q_DECLARE_LATIN1_LITERAL(AttributeContext, "Context");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubType, "SubType");
    Q_DECLARE_LATIN1_LITERAL(AttributeContextHome, "Home");
    Q_DECLARE_LATIN1_LITERAL(AttributeContextWork, "Work");
    Q_DECLARE_LATIN1_LITERAL(AttributeContextOther, "Other");
#endif

    QContactDetail::Error error() const;

    bool operator==(const QContactDetail& other) const;
    bool operator!=(const QContactDetail& other) const {return !(other == *this);}

    QString definitionName() const;
    bool isEmpty() const;

    QMap<QString, QString> attributes() const;
    bool setAttributes(const QMap<QString, QString>& attributes);

    QString attribute(const QString& attribute) const;
    bool setAttribute(const QString& attribute, const QString& value);
    bool removeAttribute(const QString& attribute);

    QVariantMap values() const;
    QString value(const QString& key) const;
    bool setValue(const QString& key, const QVariant& value);
    bool removeValue(const QString& key);
    bool hasValue(const QString& key) const;

    QVariant variantValue(const QString& key) const;
    template <typename T> T value(const QString& key) const
    {
        return variantValue(key).value<T>();
    }

    void setSubTypeAttribute(const QString& subType)
    {
        setAttribute(AttributeSubType, subType);
    }

    QString subTypeAttribute() const
    {
        return attribute(AttributeSubType);
    }

    void setContextAttribute(const QString& context)
    {
        setAttribute(AttributeContext, context);
    }

    QString contextAttribute() const
    {
        return attribute(AttributeContext);
    }


protected:
    QContactDetail(const QContactDetail& other, const QString& expectedDefinitionId);
    QContactDetail& assign(const QContactDetail& other, const QString& expectedDefinitionId);

private:
    friend class QContact;
    QSharedDataPointer<QContactDetailPrivate> d;
};

#define Q_DECLARE_CUSTOM_CONTACT_DETAIL(className, definitionNameString) \
    className() : QContactDetail(DefinitionName) {} \
    className(const QContactDetail& field) : QContactDetail(field, DefinitionName) {} \
    className& operator=(const QContactDetail& other) {assign(other, DefinitionName); return *this;} \
    \
    Q_DECLARE_LATIN1_LITERAL(DefinitionName, definitionNameString);

#define Q_IMPLEMENT_CUSTOM_CONTACT_DETAIL(className, definitionNameString) \
    Q_DEFINE_LATIN1_LITERAL(className::DefinitionName, definitionNameString)

#endif

