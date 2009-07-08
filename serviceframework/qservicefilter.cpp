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
** contained in Technology Preview License Agreement accompanying
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

#include <QRegExp>
#include <QStringList>
#include <QDebug>
#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif

#include "qservicefilter.h"

QT_BEGIN_NAMESPACE

class QServiceFilterPrivate
{
public:
    QString interface;
    QString service;
    int majorVersion;
    int minorVersion;
    QServiceFilter::VersionMatchRule matchingRule;
    QHash<QString,QString> customProperties;
    friend class QServiceFilter;
};


/*!
    \class QServiceFilter
    \brief The QServiceFilter class defines criteria for defining a sub-set of 
    all available services.

    

    \sa QServiceInterfaceDescriptor, QServiceManager
*/

/*!
    \enum QServiceFilter::VersionMatchRule

    This enum describes how interface version matching is performed.

    \value ExactVersionMatch    The filter matches any interface implementation that implements
                                the exact version provided. 
    \value MinimumVersionMatch  The filter matches any interface implementation that implements
                                either the given major/minor version or any subsequent version.
*/

/*!
    Creates a new filter object that matches all service implementations.
*/
QServiceFilter::QServiceFilter()
{
    d = new QServiceFilterPrivate();
    d->majorVersion = -1;
    d->minorVersion = -1;
    d->matchingRule = QServiceFilter::MinimumVersionMatch;
}

/*!
    Creates a copy of QServiceFilter object contained in \a other.
*/
QServiceFilter::QServiceFilter(const QServiceFilter& other)
{
    d = new QServiceFilterPrivate();
    (*this) = other;
}

/*!
    Creates a new filter object that matches all service
    implementations implementing \a interfaceName that match the specified
    \a version using the given \a rule.
*/
QServiceFilter::QServiceFilter(const QString& interfaceName, const QString& version, QServiceFilter::VersionMatchRule rule)
{
    d = new QServiceFilterPrivate();
    d->majorVersion = -1;
    d->minorVersion = -1;
    d->matchingRule = QServiceFilter::MinimumVersionMatch;
    setInterface(interfaceName, version, rule);
}

/*!
    Destroys this instance of QServiceFilter.
*/
QServiceFilter::~QServiceFilter()
{
    delete d;
}

/*!
    Copies the content of the QServiceFilter object contained in 
    \a other into this one.
*/
QServiceFilter& QServiceFilter::operator=(const QServiceFilter& other)
{
    d->interface = other.d->interface;
    d->service = other.d->service;
    d->majorVersion = other.d->majorVersion;
    d->minorVersion = other.d->minorVersion;
    d->matchingRule = other.d->matchingRule;
    d->customProperties = other.d->customProperties;

    return *this;
}

/*!
    The filter only matches implementations which are provided by the service
    specified by \a serviceName.

    If the \a serviceName is empty the filter matches any service.
*/
void QServiceFilter::setServiceName(const QString& serviceName)
{
    d->service = serviceName;
}

/*!
    Sets the filter to match any interface implementation that implements
    \a interfaceName with version \a version. The version is matched
    according to the given \a rule. If \a version is not set, the filter matches any version of the
    interface implementation.

    This method does nothing if \a version is not a valid version string or
    if \a interfaceName is empty.

    A valid version string has the format x.y whereby x and y are positive integer
    numbers.
*/
void QServiceFilter::setInterface(const QString &interfaceName, const QString& version, QServiceFilter::VersionMatchRule rule)
{
    //unset interface name
    if (interfaceName.isEmpty() && version.isEmpty()) 
    {
        d->interface = interfaceName;
        d->majorVersion = d->minorVersion = -1;
        d->matchingRule = rule;
        return;
    }

    if (interfaceName.isEmpty()) {
        qWarning() << "Empty interface name. Ignoring filter details";
        return;
    }

    if (version.isEmpty()) {
        d->majorVersion = d->minorVersion = -1;
        d->matchingRule = rule;
        d->interface = interfaceName;
        return;
    }

    // Match x.y as version format.
    // This differs from regex in servicemetadata in that 0.x versions are
    // accepted for the search filter.
    QRegExp rx("^(0+|[1-9][0-9]*)\\.(0+|[1-9][0-9]*)$");
    int pos = rx.indexIn(version);
    QStringList list = rx.capturedTexts();
    bool success = false;
    int temp_major = -1;
    int temp_minor = -1;
    if (pos == 0 && list.count() == 3 
            && rx.matchedLength() == version.length() ) 
    {
        temp_major = list[1].toInt(&success);
        if ( success ) {
            temp_minor = list[2].toInt(&success);
       }
    }

    if (success) {
        d->majorVersion = temp_major;
        d->minorVersion = temp_minor;
        d->interface = interfaceName;
        d->matchingRule = rule;
    } else {
        qWarning() << "Invalid version tag" << version << ". Ignoring filter details.";
    }
}

/*!
    Returns the service name for this filter.

    \sa setServiceName()
*/
QString QServiceFilter::serviceName() const
{
    return d->service;
}

/*!
    Returns the interface name for this filter.

    \sa setInterface()
*/
QString QServiceFilter::interfaceName() const
{
    return d->interface;
}

/*!
    Returns the major interface version for this filter.

    \sa setInterface()
*/
int QServiceFilter::interfaceMajorVersion() const
{
    return d->majorVersion;
}

/*!
    Returns the minor interface version for this filter.

    \sa setInterface()
*/
int QServiceFilter::interfaceMinorVersion() const
{
    return d->minorVersion;
}

/*!
    The filter only matches implementations which have the custom property
    \a key with the given \a value. Such constraints are specified via the 
    \i{<customproperty>} tag within the service xml.

    \sa customProperty(), removeCustomProperty()
*/
void QServiceFilter::setCustomProperty(const QString& key, const QString& value)
{
    d->customProperties.insert(key, value);
}

/*!
    Returns the value for the custom property \a key; otherwise
    returns a null string.

    \sa setCustomProperty(), removeCustomProperty()
*/
QString QServiceFilter::customProperty(const QString& key) const
{
    return d->customProperties.value(key);
}

/*!
    Removes the custom property \a key from the filter's set of constraints

    \sa clearCustomProperties(), setCustomProperty()
*/
void QServiceFilter::removeCustomProperty(const QString &key)
{
    d->customProperties.remove(key);
}

/*!
    Clears all custom properties from the filter's set of constraints

    \sa removeCustomProperty()
*/
void QServiceFilter::clearCustomProperties()
{
    d->customProperties.clear();
}

/*!
    Returns the version match rule for this filter.

    \sa setInterface()
*/
QServiceFilter::VersionMatchRule QServiceFilter::versionMatchRule() const
{
    return d->matchingRule;
}

/*!
    Returns the list of custom keys which have been added to the filter.
*/
QList<QString> QServiceFilter::customKeys() const
{
    return d->customProperties.keys();
}

#ifndef QT_NO_DATASTREAM
/*! 
    \fn QDataStream &operator<<(QDataStream &out, const QServiceFilter &sf)
    \relates QServiceFilter

    Writes service filter \a sf to the stream \a out and returns a reference
    to the stream.
*/

QDataStream &operator<<(QDataStream &out, const QServiceFilter &sf)
{

    const qint32 mj = sf.d->majorVersion;
    const qint32 mn = sf.d->minorVersion;
    const qint32 rule = (qint32) sf.d->matchingRule;
    out << sf.d->interface;
    out << sf.d->service;
    out << mj;
    out << mn;
    out << rule;
    out << sf.d->customProperties;
    return out;
}

/*!
    \fn QDataStream &operator>>(QDataStream &in, QServiceFilter &sf)
    \relates QServiceFilter

    Reads a service filter into \a sf from the stream \a in and returns a
    reference to the stream.
*/
QDataStream &operator>>(QDataStream &in, QServiceFilter &sf)
{
    qint32 mj, mn, rule;

    in >> sf.d->interface;
    in >> sf.d->service;
    in >> mj;
    in >> mn;
    in >> rule;
    in >> sf.d->customProperties;

    sf.d->majorVersion = mj;
    sf.d->minorVersion = mn;
    sf.d->matchingRule = (QServiceFilter::VersionMatchRule) rule;

    return in;
}
#endif //QT_NO_DATASTREAM


QT_END_NAMESPACE

