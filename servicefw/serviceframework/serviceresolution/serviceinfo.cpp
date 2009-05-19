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

#include "serviceinfo_p.h"

#define SERVICE_ITEM_SEP "<$>"
#define CAPABILITY_SEP ","

QT_BEGIN_NAMESPACE

enum TMetadataIndexes
    {
        EMetadataIndex=0,
        EMetadataIndex1,
        EMetadataIndex2,
        EMetadataIndex3,
        EMetadataIndex4,
        EMetadataCount
    };

/*
    \class ServiceInfo
    Utility class that encapsulates the service metadata \n
    Class is used by service resolver and service handler
*/

/*!
    Default Constructor
*/
ServiceInfo::ServiceInfo()
{

}

/*!
    Constructor
    @param aName service name
    @param aFileName service file name
    @param aCapabilities service capabilities
    @param aVersion service version
    @param aXmlFileName xml file name
*/
ServiceInfo::ServiceInfo(const QString& name,const QString& filePath, const QString& description)
{
    m_name = name;
    m_filePath = filePath;
    m_description = description;
}

/*!
    Destructor
*/
ServiceInfo::~ServiceInfo(void)
{

}

/*!
    Gets the service name
    @return service name
*/
QString ServiceInfo::name() const
{
    return m_name;
}

/*!
    Sets the service name
    @param name service name
    @return void
*/
void ServiceInfo::setName(const QString& name)
{
    m_name = name;
}

/*!
    Gets the service file path (includes file name)
    @return service file path
*/
QString ServiceInfo::filePath() const
{
    return m_filePath;
}

/*!
    Sets the service file name
    @param filePath service file path(include the file name in the path)
    @return void
*/
void ServiceInfo::setFilePath(const QString& filePath)
{
    m_filePath = filePath;
}

/*!
  Gets the service description
  @return service description
*/
QString ServiceInfo::description() const
{
    return m_description;
}

/*!
  Sets the service description
  @param description service description
  @return void
*/
void ServiceInfo::setDescription(const QString &description)
{
    m_description = description;
}

/*
    Checks if the service is valid \n
    For a service to be valid, all properties must be defined
    @return true if service is valid, false otherwise
*/
bool ServiceInfo::isValid() const
{
    return !m_name.isEmpty() && !m_filePath.isEmpty()
        && !m_description.isEmpty();
}

/*
   Clears all the fields of the service
   @return void;
*/

void ServiceInfo::clear()
{
    m_name ="";
    m_filePath="";
    m_description="";
}

QT_END_NAMESPACE
