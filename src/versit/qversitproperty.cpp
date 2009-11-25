/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitproperty.h"
#include "qversitproperty_p.h"
#include "qmobilityglobal.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

/*!
 * \class QVersitProperty
 *
 * \brief QVersitProperty stores the name, value and parameters of a versit property.
 *
 * For example a vCard can be presented as a QVersitDocument that
 * consists of 0..n properties such as a name (N),
 * a telephone number (TEL) and an email address (EMAIL) to name a few.
 * Each of these properties is stored as
 * an instance of a QVersitProperty in a QVersitDocument.
 *
 * QVersitProperty supports implicit sharing.
 * The property name and parameters of a QVersitProperty are converted
 * to upper-case when they are stored to a QVersitProperty.
 * The value of a QVersitProperty is raw data and it is case-sensitive.
 *
 * \sa QVersitDocument
 */


/*! Constructs a new empty property */
QVersitProperty::QVersitProperty() : d(new QVersitPropertyPrivate())
{
}

/*! Constructs a property that is a copy of \a other */
QVersitProperty::QVersitProperty(const QVersitProperty& other) : d(other.d)
{
}

/*! Frees the memory used by the property */
QVersitProperty::~QVersitProperty()
{
}

/*! Assigns this property to \a other */
QVersitProperty& QVersitProperty::operator=(const QVersitProperty& other)
{
    if (this != &other)
        d = other.d;
    return *this;    
}

/*!
 * Sets the groups in the property to the given list of \a groups.
 */
void QVersitProperty::setGroups(const QStringList& groups)
{
    d->mGroups.clear();
    foreach (QString group, groups) {
        d->mGroups.append(group);
    }
}

/*!
 * Gets the groups part of the property.
 */
QStringList QVersitProperty::groups() const
{
    return d->mGroups;
}

/*!
 * Sets the \a name of the property.
 * The \a name is converted to upper-case.
 */
void QVersitProperty::setName(const QString& name)
{
    d->mName = name.toUpper();
}

/*!
 * Gets the name of the property in upper-case.
 */
QString QVersitProperty::name() const
{
    return d->mName;
}

/*!
 * Replaces all the parameters with \a parameters.
 * The parameters are converted to upper-case.
 */
void QVersitProperty::setParameters(const QMultiHash<QString,QString>& parameters)
{
    d->mParameters.clear();
    // Traverse parameters in the reverse order, because they are added to
    // d->mParameters using insert in QVersitProperty::addParameter
    QList<QString> keys = parameters.uniqueKeys();
    for (int i=keys.count()-1; i >= 0; i--) {
        QString key = keys.at(i);
        QList<QString> values = parameters.values(key);
        for (int j=values.count()-1; j >= 0; j--) {
            // Convert all the parameter names and values to upper case
            addParameter(key,values.at(j));
        }
    }
}

/*!
 * Adds a new parameter with \a name and \a value.
 * Both the name and the value are converted to upper-case.
 */
void QVersitProperty::addParameter(const QString& name, const QString& value)
{
    d->mParameters.insert(name.toUpper(),value.toUpper());
}

/*!
 * Removes a parameter with \a name and \a value.
 */
void QVersitProperty::removeParameter(const QString& name, const QString& value)
{
    d->mParameters.remove(name.toUpper(),value.toUpper());
}

/*!
 * Return a copy of the contained list of parameters.
 * Note that actual the parameters cannot be modified using the copy.
 */
QMultiHash<QString,QString> QVersitProperty::parameters() const
{
    return d->mParameters;
}

/*!
 * Sets the \a value of the property.
 */
void QVersitProperty::setValue(const QByteArray& value)
{
    d->mValue = value;
}

/*!
 * Returns the value of the property.
 */
QByteArray QVersitProperty::value() const
{
    return d->mValue;
}

/*!
 * Sets the embedded \a document of the property
 */
void QVersitProperty::setEmbeddedDocument(const QVersitDocument& document)
{
    d->mDocument = document;
}

/*!
 * Returns the embedded document of the property.
 * If the embedded document has not been set, an empty document is returned.
 */
QVersitDocument QVersitProperty::embeddedDocument() const
{
    return d->mDocument;
}

QTM_END_NAMESPACE
