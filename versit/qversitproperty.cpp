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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitproperty.h"
#include "qversitproperty_p.h"
#include <QStringList>

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
 * Sets the name of the property
 */
void QVersitProperty::setName(const QString& name)
{
    d->mName = name.toUpper();
}

/*!
 * Retreives the name of the property
 */
QString QVersitProperty::name() const
{
    return d->mName;
}

/*!
 * Replaces all the parameters
 */
void QVersitProperty::setParameters(const QMultiHash<QString,QString>& parameters)
{
    // Convert all the parameter names and values to upper case
    d->mParameters.clear();
    foreach (QString key, parameters.uniqueKeys())
        foreach (QString value, parameters.values(key))
            addParameter(key,value);
}

/*!
 * Adds a new parameter with \a name and \a value
 */
void QVersitProperty::addParameter(const QString& name, const QString& value)
{
    d->mParameters.insert(name.toUpper(),value.toUpper());
}

/*!
 * Removes a parameter with \a name and \a value
 */
void QVersitProperty::removeParameter(const QString& name, const QString& value)
{
    d->mParameters.remove(name.toUpper(),value.toUpper());
}

/*!
 * Returns the contained list of parameters
 */
QMultiHash<QString,QString> QVersitProperty::parameters() const
{
    return d->mParameters;
}

/*!
 * Sets the value of the property
 */
void QVersitProperty::setValue(const QByteArray& value)
{
    d->mValue = value;
}

/*!
 * Returns the parameter value
 */
QByteArray QVersitProperty::value() const
{
    return d->mValue;
}

/*!
 * Sets the embedded document of the property
 */
void QVersitProperty::setEmbeddedDocument(const QVersitDocument& document)
{
    d->mDocument = document;
}

/*!
 * Returns the embedded document from the property
 */
QVersitDocument QVersitProperty::embeddedDocument() const
{
    return d->mDocument;
}
