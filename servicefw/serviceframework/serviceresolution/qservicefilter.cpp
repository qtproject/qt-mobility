/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include <QRegExp>
#include <QStringList>
#include <QDebug>
#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif

#include "qservicefilter.h"

QT_BEGIN_NAMESPACE

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
    : majorVersion(-1), minorVersion(-1), matchingRule(MinimumVersionMatch)
{
}

/*!
    Creates a copy of QServiceFilter object contained in \a other.
*/
QServiceFilter::QServiceFilter(const QServiceFilter& other)
{
    (*this) = other;
}

/*!
    Creates a new filter object that matches all service
    implementations implementing \a interfaceName that match the specified
    \a version using the given \a rule.
*/
QServiceFilter::QServiceFilter(const QString& interfaceName, const QString& version, QServiceFilter::VersionMatchRule rule)
    : majorVersion(-1), minorVersion(-1), matchingRule(MinimumVersionMatch)
{
    setInterface(interfaceName, version, rule);
}

/*!
    Destroys this instance of QServiceFilter.
*/
QServiceFilter::~QServiceFilter()
{
}

/*!
    Copies the content of the QServiceFilter object contained in 
    \a other into this one.
*/
QServiceFilter& QServiceFilter::operator=(const QServiceFilter& other)
{
    interface = other.interface;
    service = other.service;
    majorVersion = other.majorVersion;
    minorVersion = other.minorVersion;
    matchingRule = other.matchingRule;

    return *this;
}

/*!
    The filter only matches implementations which are provided by the service
    specified by \a serviceName.

    If the \a serviceName is empty the filter matches any service.
*/
void QServiceFilter::setServiceName(const QString& serviceName)
{
    service = serviceName;
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
        interface = interfaceName;
        majorVersion = minorVersion = -1;
        matchingRule = rule;
        return;
    }

    if (interfaceName.isEmpty()) {
        qWarning() << "Empty interface name. Ignoring filter details";
        return;
    }

    if (version.isEmpty()) {
        majorVersion = minorVersion = -1;
        matchingRule = rule;
        interface = interfaceName;
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
        majorVersion = temp_major;
        minorVersion = temp_minor;
        interface = interfaceName;
        matchingRule = rule;
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
    return service;
}

/*!
    Returns the interface name for this filter.

    \sa setInterface()
*/
QString QServiceFilter::interfaceName() const
{
    return interface;
}

/*!
    Returns the major interface version for this filter.

    \sa setInterface()
*/
int QServiceFilter::interfaceMajorVersion() const
{
    return majorVersion;
}

/*!
    Returns the minor interface version for this filter.

    \sa setInterface()
*/
int QServiceFilter::interfaceMinorVersion() const
{
    return minorVersion;
}

/*!
    Returns the version match rule for this filter.

    \sa setInterface()
*/
QServiceFilter::VersionMatchRule QServiceFilter::versionMatchRule() const
{
    return matchingRule;
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

    const qint32 mj = sf.majorVersion;
    const qint32 mn = sf.minorVersion;
    const qint32 rule = (qint32) sf.matchingRule;
    out << sf.interface;
    out << sf.service;
    out << mj;
    out << mn;
    out << rule;
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

    in >> sf.interface;
    in >> sf.service;
    in >> mj;
    in >> mn;
    in >> rule;

    sf.majorVersion = mj;
    sf.minorVersion = mn;
    sf.matchingRule = (QServiceFilter::VersionMatchRule) rule;

    return in;
}
#endif //QT_NO_DATASTREAM


QT_END_NAMESPACE

