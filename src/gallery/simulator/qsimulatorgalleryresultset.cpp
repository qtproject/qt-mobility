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
#include "qsimulatorgalleryresultset_p.h"

#include "qdocumentgallery.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

QTM_BEGIN_NAMESPACE

namespace Utility {
enum PropertyKeys {
    // Query Properties
    FileName = 0,
    FilePath,
    FileExtension,
    FileSize,
    MimeType,
    LastModified,
    LastAccessed,
    // Image Properties
    Title,
    Width,
    Height,
    Keywords
};
}

QSimulatorGalleryResultSet::QSimulatorGalleryResultSet(QObject *parent) :
    QGalleryResultSet(parent),
    connection(0),
    valid(false),
    mCurrentIndex(0)
{
    queryRequest = qobject_cast<QGalleryQueryRequest*>(parent);
    itemRequest = qobject_cast<QGalleryItemRequest*>(parent);
    if (queryRequest) {
        if (queryRequest->rootType() == QLatin1String("Image")) {
            // Do the image magic
            connection = Simulator::DocGalleryConnection::instance();
        } else {
            QGalleryFilter filter = queryRequest->filter();
            if (filter.toMetaDataFilter().propertyName() != QLatin1String("filePath")) {
                qWarning("The Qt Simulator can only handle filePath filters currently");
                return;
            }
            filePath = filter.toMetaDataFilter().value().toString();
        }
    } else if (itemRequest) {
        filePath = itemRequest->itemId().toString();
    } else {
        qWarning("Unsupport Request Type!");
        return;
    }

    image = QImage(filePath);
    if (!image.isNull())
        itemTypeString = QDocumentGallery::Image.name();
    else if (!filePath.isEmpty())
        itemTypeString = QDocumentGallery::File.name();

    valid = true;
    finish();
}

QSimulatorGalleryResultSet::~QSimulatorGalleryResultSet()
{
}

int QSimulatorGalleryResultSet::propertyKey(const QString &property) const
{
    if (property == QDocumentGallery::url.name() ||
        property == QDocumentGallery::fileName.name())
        return Utility::FileName;
    else if(property == QDocumentGallery::filePath.name() ||
              property == QDocumentGallery::path.name())
        return Utility::FilePath;
    else if (property == QDocumentGallery::fileExtension.name())
        return Utility::FileExtension;
    else if (property == QDocumentGallery::fileSize.name())
        return Utility::FileSize;
    else if (property == QDocumentGallery::mimeType.name())
        return Utility::MimeType;
    else if (property == QDocumentGallery::lastModified.name())
        return Utility::LastModified;
    else if (property == QDocumentGallery::lastAccessed.name())
        return Utility::LastAccessed;
    else if (property == QDocumentGallery::title.name())
        return Utility::Title;
    else if (property == QDocumentGallery::width.name())
        return Utility::Width;
    else if (property == QDocumentGallery::height.name())
        return Utility::Height;
    else if (property == QDocumentGallery::keywords.name())
        return Utility::Keywords;
    return -1;
}

QGalleryProperty::Attributes QSimulatorGalleryResultSet::propertyAttributes(int key) const
{
    switch(key) {
    case Utility::FileName: return QGalleryProperty::CanRead;
    case Utility::FilePath: return QGalleryProperty::CanRead;
    case Utility::FileExtension: return QGalleryProperty::CanRead;
    case Utility::FileSize: return QGalleryProperty::CanRead;
    case Utility::LastModified: return QGalleryProperty::CanRead;
    case Utility::LastAccessed: return QGalleryProperty::CanRead;
    case Utility::Title: return QGalleryProperty::CanRead;
    case Utility::Width: return QGalleryProperty::CanRead;
    case Utility::Height: return QGalleryProperty::CanRead;
    case Utility::Keywords: return QGalleryProperty::CanRead;
    default: return QGalleryProperty::Attributes();
    }
}

QVariant::Type QSimulatorGalleryResultSet::propertyType(int key) const
{
    QVariant::Type ret = QVariant::Invalid;
    switch(key) {
    case Utility::FileName: ret = QVariant::String; break;
    case Utility::FilePath: ret = QVariant::String; break;
    case Utility::FileExtension: ret = QVariant::String; break;
    case Utility::FileSize: ret = QVariant::Int; break;
    case Utility::LastModified: ret = QVariant::DateTime; break;
    case Utility::LastAccessed: ret = QVariant::DateTime; break;
    case Utility::Title: ret = QVariant::String; break;
    case Utility::Width: ret = QVariant::Int; break;
    case Utility::Keywords: ret = QVariant::String; break;
    default: ret = QVariant::Invalid;break;
    }

    return ret;
}

int QSimulatorGalleryResultSet::itemCount() const
{
    if (connection)
        return connection->galleryData().images.count();
    return valid ? 1 : 0;
}

bool QSimulatorGalleryResultSet::isValid() const
{
    return valid;
}

QVariant QSimulatorGalleryResultSet::itemId() const
{
    if (valid)
        return QFileInfo(filePath).absoluteFilePath();
    return QVariant();
}

QUrl QSimulatorGalleryResultSet::itemUrl() const
{
    if (connection) {
        QString data = connection->galleryData().images.at(mCurrentIndex).fileName;
        return QUrl::fromLocalFile(data);
    }
    return QUrl();
}

QString QSimulatorGalleryResultSet::itemType() const
{
    return itemTypeString;
}

QVariant QSimulatorGalleryResultSet::metaData(int key) const
{
    QFileInfo info(filePath);
    if (key == Utility::FileName)
        return info.fileName();
    else if (key == Utility::FilePath)
        return info.absolutePath();
    else if (key == Utility::FileExtension)
        return info.suffix();
    else if (key == Utility::FileSize)
        return info.size();
    else if (key == Utility::LastAccessed)
        return info.lastRead();
    else if (key == Utility::LastModified)
        return info.lastModified();
    else if (key == Utility::Title)
        return QLatin1String("Image Title of Simulator");
    else if (key == Utility::Width)
        return image.width();
    else if (key == Utility::Height)
        return image.height();
    else if (key == Utility::Keywords)
        return QLatin1String("Simulator, Some Tags, Not read from file yet");
    return QVariant();
}

bool QSimulatorGalleryResultSet::setMetaData(int key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
    return false;
}

int QSimulatorGalleryResultSet::currentIndex() const
{
    return mCurrentIndex;
}

bool QSimulatorGalleryResultSet::fetch(int index)
{
    if (connection) {
        if (index < 0 || index >= connection->galleryData().images.size())
            return false;
        mCurrentIndex = index;
    }
    return true;
}

#include "moc_qsimulatorgalleryresultset_p.cpp"
QTM_END_NAMESPACE
