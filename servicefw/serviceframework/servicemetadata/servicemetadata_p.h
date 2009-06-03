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

#ifndef SERVICEMETADATA_H
#define SERVICEMETADATA_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
#include <QXmlStreamReader>
#include <QStringList>
#include <QList>
#include <QSet>
#include "qserviceglobal.h"
#include "qserviceinterfacedescriptor.h"

class QIODevice;

QT_BEGIN_NAMESPACE

// FORWARD DECLARATIONS
class QServiceInterfaceDescriptor;

class Q_SFW_EXPORT ServiceMetaData 
{
public:

    //! ServiceMetaData::ServiceMetadataErr
    /*!
     This enum describes the errors that may be returned by the Service metadata parser.
     */
    enum ServiceMetadataErr {
        SFW_ERROR_NO_SERVICE = 0,                           /* Can not find service root node in XML file*/
        SFW_ERROR_NO_SERVICE_NAME,                          /* Can not find service name in XML file */
        SFW_ERROR_NO_SERVICE_FILEPATH,                      /* Can not find service filepath in XML file */
        SFW_ERROR_NO_SERVICE_INTERFACE,                     /* No interface for the service in XML file*/
        SFW_ERROR_NO_INTERFACE_VERSION,                     /* Can not find interface version in XML file */
        SFW_ERROR_NO_INTERFACE_NAME,                        /* Can not find interface name in XML file*/
        SFW_ERROR_UNABLE_TO_OPEN_FILE,                      /* Error opening XML file*/
        SFW_ERROR_INVALID_XML_FILE,                         /* Not a valid XML file*/
        SFW_ERROR_PARSE_SERVICE,                            /* Error parsing service node */
        SFW_ERROR_PARSE_INTERFACE,                          /* Error parsing interface node */
        SFW_ERROR_DUPLICATED_INTERFACE,                     /* The same interface is defined twice */
        SFW_ERROR_INVALID_VERSION,
        SFW_ERROR_DUPLICATED_TAG                            /* The tag appears twice */
    };

public:

    ServiceMetaData(const QString &aXmlFilePath);

    ServiceMetaData(QIODevice *device);

    ~ServiceMetaData();

    void setDevice(QIODevice *device);

    QIODevice *device() const;

    void setServiceLocation(const QString &aFilePath);

    bool extractMetadata();

    int getLatestError();

    QString name();

    QString location();

    QString description();

    int interfaceCount();
    
    QList<QServiceInterfaceDescriptor> getInterfaces();

    QServiceInterfaceDescriptor latestInterfaceVersion(const QString &interfaceName);
    QList<QServiceInterfaceDescriptor> latestInterfaces() const;

private:
    bool getAttributeValue(const QXmlStreamReader &aDomElement,
            const QString &aAttributeName, QString &aValue);

    bool processServiceElement(QXmlStreamReader &aXMLReader);
    
    bool processInterfaceElement(QXmlStreamReader &aXMLReader);

    void clearMetadata();

private:
    bool lessThan(const QServiceInterfaceDescriptor &d1,
                    const QServiceInterfaceDescriptor &d2) const;
    bool checkVersion(const QString &version) const;
    void transformVersion(const QString &version, int *major, int *minor) const;

    QIODevice *xmlDevice;
    bool ownsXmlDevice;
    QString serviceName;
    QString serviceLocation;
    QString serviceDescription;
    QList<QServiceInterfaceDescriptor> serviceInterfaces;
    QSet<QString> duplicates;
    int latestError;
    QHash<QString, int> m_latestIndex;
};

QT_END_NAMESPACE

#endif // SERVICEMETADATA_H
