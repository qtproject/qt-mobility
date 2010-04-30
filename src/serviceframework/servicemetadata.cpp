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

#include "servicemetadata_p.h"
#include <QFile>
#include <QDebug>
#include "qserviceinterfacedescriptor_p.h"

//XML tags and attributes
//General
#define NAME_TAG  "name"
#define DESCRIPTION_TAG "description"

//Service related
#define SERVICE_TAG "service" 
#define SERVICE_FILEPATH "filepath"

//Interface related
#define INTERFACE_TAG "interface"
#define INTERFACE_VERSION "version" 
#define INTERFACE_CAPABILITY "capabilities"
#define INTERFACE_CUSTOM_PROPERTY "customproperty"

QTM_BEGIN_NAMESPACE

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const ServiceMetaDataResults &r)
{
    out << r.name << r.location;
    out << r.description << r.interfaces << r.latestInterfaces;

    return out;
}

QDataStream &operator>>(QDataStream &in, ServiceMetaDataResults &r)
{
    in >> r.name >> r.location;
    in >> r.description >> r.interfaces >> r.latestInterfaces;

    return in;
}
#endif

/*
    \class ServiceMetaData

    Utility class (used by service database) that offers support for 
    parsing metadata service xml registry file during service registration. \n
    
    It uses QXMLStreamReader class for parsing. Supproted Operations are:
        - Parse the service and interfaces defined in XML file
        - name, version, capabilitiesList, description and filePath of service can be retrieved
        - each interface can be retrieved
*/

/*
 *  Class constructor
 *
 * @param aXmlFilePath path to the xml file that describes the service. 
 */
ServiceMetaData::ServiceMetaData(const QString &aXmlFilePath)
{
    xmlDevice = new QFile(aXmlFilePath);
    ownsXmlDevice = true;
    latestError = 0;
}

/*
 *  Class constructor
 *
 * @param device QIODevice that contains the XML data that describes the service.
 */
ServiceMetaData::ServiceMetaData(QIODevice *device)
{
    xmlDevice = device;
    ownsXmlDevice = false;
    latestError = 0;
}

/*
 *  Class destructor
 * 
 */
ServiceMetaData::~ServiceMetaData()
{
    if (ownsXmlDevice)
        delete xmlDevice;
}

/*
    Sets the device containing the XML data that describes the service to \a device.
 */
void ServiceMetaData::setDevice(QIODevice *device)
{
    clearMetadata();
    xmlDevice = device;
    ownsXmlDevice = false;
}

/*
    Returns the device containing the XML data that describes the service.
*/
QIODevice *ServiceMetaData::device() const
{
    return xmlDevice;
}

/*
 *  Gets the service name
 *
 * @return service name or default value (empty string) if it is not available
 */
/*QString ServiceMetaData::name() const
{
    return serviceName;
}*/
 
/*
 *  Gets the path of the service implementation file
 *
 * @return service implementation filepath
 */
/*QString ServiceMetaData::location() const
{
    return serviceLocation;
}*/
 
/*
 *  Gets the service description
 *
 * @return service description or default value (empty string) if it is not available
 */
/*QString ServiceMetaData::description() const
{
    return serviceDescription;
}*/
 
/*
   Returns the metadata of the interace at \a index; otherwise
   returns 0.
 */
/*QList<QServiceInterfaceDescriptor> ServiceMetaData::getInterfaces() const
{
    return serviceInterfaces;
} */

/*!
    \internal

    Returns a streamable object containing the results of the parsing.
*/
ServiceMetaDataResults ServiceMetaData::parseResults() const
{
    ServiceMetaDataResults results;
    results.location = serviceLocation;
    results.name = serviceName;
    results.description = serviceDescription;
    results.interfaces = serviceInterfaces;
    results.latestInterfaces = latestInterfaces();

    return results;
}

/*
    Parses the file and extracts the service metadata \n
    Custom error codes: \n
    SFW_ERROR_UNABLE_TO_OPEN_FILE in case can not open the XML file \n
    SFW_ERROR_INVALID_XML_FILE in case service registry is not a valid XML file \n
    SFW_ERROR_NO_SERVICE in case XML file has no service tag\n
    @return true if the metadata was read properly, false if there is an error
 */
bool ServiceMetaData::extractMetadata()
{
    latestError = 0;
    clearMetadata();                   
    QXmlStreamReader xmlReader;
    bool parseError = false;
    //Open xml file
    if (!xmlDevice->isOpen() && !xmlDevice->open(QIODevice::ReadOnly)) {
        latestError = ServiceMetaData::SFW_ERROR_UNABLE_TO_OPEN_FILE;
        parseError = true;
    } else {
        //Load xml content
        xmlReader.setDevice(xmlDevice);
        // Read XML doc 
        while (!xmlReader.atEnd() && !parseError) {
            xmlReader.readNext();
            //Found a <service> node, read service related metadata
            if (xmlReader.isStartElement() && xmlReader.name() == SERVICE_TAG) {
                if (!processServiceElement(xmlReader)) {
                    parseError = true;
                }
            }
            else if (xmlReader.isStartElement() && xmlReader.name() != SERVICE_TAG) {
                latestError = ServiceMetaData::SFW_ERROR_NO_SERVICE;
                parseError = true;
            }
            else if (xmlReader.tokenType() == QXmlStreamReader::Invalid) {
                latestError = ServiceMetaData::SFW_ERROR_INVALID_XML_FILE;
                parseError = true;
            }
        }
        if (ownsXmlDevice)
            xmlDevice->close();
    }
    if (parseError) {
        clearMetadata();
        //provide better error reports
        switch (latestError) {
            case SFW_ERROR_NO_SERVICE:                              /* Can not find service root node in XML file*/
                qDebug() << "Missing <service> tag";
                break;
            case SFW_ERROR_NO_SERVICE_NAME:                          /* Can not find service name in XML file */
                qDebug() << "Missing or empty <name> tag within <service>";
                break;
            case SFW_ERROR_NO_SERVICE_FILEPATH:                      /* Can not find service filepath in XML file */
                qDebug() << "Missing or empty <filepath> tag within <service>";
                break;
            case SFW_ERROR_NO_SERVICE_INTERFACE:                     /* No interface for the service in XML file*/
                qDebug() << "Missing <interface> tag";
                break;
            case SFW_ERROR_NO_INTERFACE_VERSION:                     /* Can not find interface version in XML file */
                qDebug() << "Missing or empty <version> tag within <interface>";
                break;
            case SFW_ERROR_NO_INTERFACE_NAME:                        /* Can not find interface name in XML file*/
                qDebug() << "Missing or empty <name> tag within <interface>";
                break;
            case SFW_ERROR_UNABLE_TO_OPEN_FILE:                      /* Error opening XML file*/
                qDebug() << "Unable to open service xml file";
                break;
            case SFW_ERROR_INVALID_XML_FILE:                         /* Not a valid XML file*/
                qDebug() << "Not a valid service xml";
                break;
            case SFW_ERROR_PARSE_SERVICE:                            /* Error parsing service node */
                qDebug() << "Invalid tag within <service> tags";
                break;
            case SFW_ERROR_PARSE_INTERFACE:                          /* Error parsing interface node */
                qDebug() << "Invalid tag within <interface> tags";
                break;
            case SFW_ERROR_DUPLICATED_INTERFACE:                     /* The same interface is defined twice */
                qDebug() << "The same interface has been defined more than once";
                break;
            case SFW_ERROR_INVALID_VERSION:
                qDebug() << "Invalid version string, expected: x.y";
                break;
            case SFW_ERROR_DUPLICATED_TAG:                           /* The tag appears twice */
                qDebug() << "XML tag appears twice";
                break;
            case SFW_ERROR_INVALID_CUSTOM_TAG:                       /* The customproperty tag is not correctly formatted or otherwise incorrect*/
                qDebug() << "Invalid custom property tag";
                break;
            case SFW_ERROR_DUPLICATED_CUSTOM_KEY:                    /* The customproperty appears twice*/
                qDebug() << "Same custom property appears multiple times";
                break;
        }
    }
    return !parseError;
}
 
/*
    Gets the latest parsing error \n
    @return parsing error(negative value) or 0 in case there is none
 */
int ServiceMetaData::getLatestError() const
{
    return latestError;
}
 
/*
    Parses and extracts the service metadata from the current xml <service> node \n
 */
bool ServiceMetaData::processServiceElement(QXmlStreamReader &aXMLReader)
{
    Q_ASSERT(aXMLReader.isStartElement() && aXMLReader.name() == SERVICE_TAG);
    bool parseError = false;

    int dupSTags[3] = {0 //->tag name
        ,0 //-> service description
        ,0 //-> filepath
    };
    while(!parseError && !aXMLReader.atEnd()) {
        aXMLReader.readNext();
        if (aXMLReader.isStartElement() && aXMLReader.name() == DESCRIPTION_TAG) {
            //Found <description> tag
            serviceDescription = aXMLReader.readElementText();
            dupSTags[1]++;
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == NAME_TAG) {
            serviceName = aXMLReader.readElementText();
            dupSTags[0]++;
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == INTERFACE_TAG) {
            //Found a <interface> node, read module related metadata  
            if (!processInterfaceElement(aXMLReader)) 
                parseError = true;
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == SERVICE_FILEPATH ) {
            //Found <filepath> tag
            dupSTags[2]++;
            serviceLocation = aXMLReader.readElementText();
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == "version") {
            //FOUND <version> tag on service level. We ignore this for now
            aXMLReader.readElementText();
        } else if (aXMLReader.isEndElement() && aXMLReader.name() == SERVICE_TAG) {
            //Found </service>, leave the loop
            break;
        } else if (aXMLReader.isEndElement() || aXMLReader.isStartElement()) {
            latestError = ServiceMetaData::SFW_ERROR_PARSE_SERVICE;
            parseError = true;            
        } else if (aXMLReader.tokenType() == QXmlStreamReader::Invalid) {
            latestError = ServiceMetaData::SFW_ERROR_INVALID_XML_FILE;
            parseError = true;
        }
    }
    if ( !parseError ) {
        if (serviceName.isEmpty()) {
            latestError = ServiceMetaData::SFW_ERROR_NO_SERVICE_NAME;
            parseError = true;
        } else if (serviceLocation.isEmpty()) {
            latestError = ServiceMetaData::SFW_ERROR_NO_SERVICE_FILEPATH;
            parseError = true;
        }
    }

    for(int i=0;!parseError && i<3;i++) {
        if (dupSTags[i] > 1) {
            parseError = true;
            latestError = SFW_ERROR_DUPLICATED_TAG;
            break;
        }
    }
        
    //update all interfaces with service data
    const int icount = serviceInterfaces.count();
    if (icount == 0 && latestError == 0) {
        latestError = ServiceMetaData::SFW_ERROR_NO_SERVICE_INTERFACE;
        parseError = true;
    }
    for (int i = 0; i<icount; i++) {
        serviceInterfaces.at(i).d->serviceName = serviceName;
        serviceInterfaces.at(i).d->attributes[QServiceInterfaceDescriptor::Location] = serviceLocation;
        serviceInterfaces.at(i).d->attributes[QServiceInterfaceDescriptor::ServiceDescription] = serviceDescription;
    }

    if (parseError) {
        clearMetadata();
    }
    return !parseError;
}

/*
    Parses and extracts the interface metadata from the current xml <interface> node \n
*/
bool ServiceMetaData::processInterfaceElement(QXmlStreamReader &aXMLReader)
{
    Q_ASSERT(aXMLReader.isStartElement() && aXMLReader.name() == INTERFACE_TAG);
    bool parseError = false;

    //Read interface parameter
    QString tmp;
    QServiceInterfaceDescriptor aInterface;
    int dupITags[4] = {
        0,  //->iface name tag
        0,  //->version
        0,  //->capabilities
        0   //->description
    };
    aInterface.d = new QServiceInterfaceDescriptorPrivate;
    while (!parseError && !aXMLReader.atEnd()) {
        aXMLReader.readNext();
        //Read interface description
        if (aXMLReader.isStartElement() && aXMLReader.name() == NAME_TAG) {
            aInterface.d->interfaceName = aXMLReader.readElementText();
            dupITags[0]++;
            //Found <name> tag for interface
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == DESCRIPTION_TAG) {
            //Found <description> tag
            aInterface.d->attributes[QServiceInterfaceDescriptor::InterfaceDescription] = aXMLReader.readElementText();
            dupITags[3]++;
        //Found </interface>, leave the loop
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == INTERFACE_VERSION) {
            tmp.clear();
            tmp = aXMLReader.readElementText();
            if (tmp.isEmpty())
                continue;  //creates NO_INTERFACE_VERSION error further below
            bool success = checkVersion(tmp);
            if ( success ) {
                int majorVer = -1;
                int minorVer = -1;
                transformVersion(tmp, &majorVer, &minorVer);
                aInterface.d->major = majorVer;
                aInterface.d->minor = minorVer;
                dupITags[1]++;
            } else {
                latestError = ServiceMetaData::SFW_ERROR_INVALID_VERSION;
                parseError = true;
            }
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == INTERFACE_CAPABILITY) {
            tmp.clear();
            tmp= aXMLReader.readElementText();
            aInterface.d->attributes[QServiceInterfaceDescriptor::Capabilities] = tmp.split(",", QString::SkipEmptyParts);
            dupITags[2]++;
        } else if (aXMLReader.isStartElement() && aXMLReader.name() == INTERFACE_CUSTOM_PROPERTY) {
            parseError = true;
            if (aXMLReader.attributes().hasAttribute("key")) {
                const QString ref = aXMLReader.attributes().value("key").toString();
                if (!ref.isEmpty()) {
                    if (aInterface.d->customAttributes.contains(ref)) {
                        latestError = SFW_ERROR_DUPLICATED_CUSTOM_KEY;
                        continue;
                    } else {
                        QString value = aXMLReader.readElementText();
                        if (value.isNull())
                            value = QString("");
                        aInterface.d->customAttributes[ref] = value;
                        parseError = false;
                    }
                }
            }
            if (parseError)
                latestError = SFW_ERROR_INVALID_CUSTOM_TAG;
        } else if (aXMLReader.isEndElement() && aXMLReader.name() == INTERFACE_TAG) {
            break;
        } else if (aXMLReader.isStartElement() || aXMLReader.isEndElement()) {
            latestError = ServiceMetaData::SFW_ERROR_PARSE_INTERFACE;
            parseError = true;
        } else if (aXMLReader.tokenType() == QXmlStreamReader::Invalid) {
            latestError = ServiceMetaData::SFW_ERROR_INVALID_XML_FILE;
            parseError = true;
        }
    }

    if (!parseError) {
        if (dupITags[1] == 0) { //no version tag found
            latestError = ServiceMetaData::SFW_ERROR_NO_INTERFACE_VERSION;
            parseError = true;
        } else if (aInterface.d->interfaceName.isEmpty()) {
            latestError = ServiceMetaData::SFW_ERROR_NO_INTERFACE_NAME;
            parseError = true;
        }
    }
    
    for(int i=0;!parseError && i<4;i++) {
        if (dupITags[i] > 1) {
            parseError = true;
            latestError = SFW_ERROR_DUPLICATED_TAG;
            break;
        }
    }

    if (!parseError) {
        const QString ident = aInterface.d->interfaceName
                                + QString::number(aInterface.majorVersion())
                                + "."
                                + QString::number(aInterface.minorVersion());
        if (duplicates.contains(ident.toLower())) {
            latestError = ServiceMetaData::SFW_ERROR_DUPLICATED_INTERFACE;
            parseError = true;
        } else {
            duplicates.insert(ident.toLower());
            serviceInterfaces.append(aInterface);
            if (!m_latestIndex.contains(aInterface.d->interfaceName.toLower())
                    || lessThan(latestInterfaceVersion(aInterface.d->interfaceName), aInterface))

            {
                    m_latestIndex[aInterface.d->interfaceName.toLower()] = serviceInterfaces.count() - 1;
            }
        }
    }
    return !parseError;
}

QServiceInterfaceDescriptor ServiceMetaData::latestInterfaceVersion(const QString &interfaceName)
{
    QServiceInterfaceDescriptor ret;
    if (m_latestIndex.contains(interfaceName.toLower()))
        return serviceInterfaces[m_latestIndex[interfaceName.toLower()]];
    else
        return ret;
}

QList<QServiceInterfaceDescriptor> ServiceMetaData::latestInterfaces() const
{
    QList<QServiceInterfaceDescriptor> interfaces;
    QHash<QString,int>::const_iterator i = m_latestIndex.constBegin();
    while(i != m_latestIndex.constEnd())
    {
        interfaces.append(serviceInterfaces[i.value()]);
        ++i;
    }
    return interfaces;
}

bool ServiceMetaData::lessThan(const QServiceInterfaceDescriptor &d1,
                                const QServiceInterfaceDescriptor &d2) const
{
    return (d1.majorVersion() < d2.majorVersion())
            || ( d1.majorVersion() == d2.majorVersion()
                    && d1.minorVersion() < d2.minorVersion());

}

bool ServiceMetaData::checkVersion(const QString &version) const
{
    //match x.y as version format
    QRegExp rx("^([1-9][0-9]*)\\.(0+|[1-9][0-9]*)$");
    int pos = rx.indexIn(version);
    QStringList list = rx.capturedTexts();
    bool success = false;
    if (pos == 0 && list.count() == 3
            && rx.matchedLength() == version.length() )
    {
        list[1].toInt(&success);
        if ( success ) {
            list[2].toInt(&success);
        }
    }
    return success;
}

void ServiceMetaData::transformVersion(const QString &version, int *major, int *minor) const
{
    Q_ASSERT(major != NULL);
    Q_ASSERT(minor != NULL);
    if(!checkVersion(version)) {
        *major = -1;
        *minor = -1;
    } else {
        QRegExp rx("^([1-9][0-9]*)\\.(0+|[1-9][0-9]*)$");
        rx.indexIn(version);
        QStringList list = rx.capturedTexts();
        Q_ASSERT(list.count() == 3);
        *major = list[1].toInt();
        *minor = list[2].toInt();
    }
}

/*
 *  Clears the service metadata
 *
 */
void ServiceMetaData::clearMetadata()
{
    serviceName.clear();
    serviceLocation.clear();
    serviceDescription.clear();
    serviceInterfaces.clear();
    duplicates.clear();
    m_latestIndex.clear();
}

QTM_END_NAMESPACE
