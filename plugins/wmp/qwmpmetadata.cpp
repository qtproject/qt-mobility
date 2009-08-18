/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwmpmetadata.h"

#include "qwmpevents.h"
#include "qwmpglobal.h"

#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qsize.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QWmpMetaData::QWmpMetaData(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
    : QMetadataProviderControl(parent)
    , m_media(0)
{
    player->get_currentMedia(&m_media);

    connect(events, SIGNAL(CurrentItemChange(IDispatch*)),
            this, SLOT(currentItemChangeEvent(IDispatch*)));
    connect(events, SIGNAL(MediaChange(IDispatch*)), this, SLOT(mediaChangeEvent(IDispatch*)));
}

QWmpMetaData::~QWmpMetaData()
{
    if (m_media)
        m_media->Release();
}

bool QWmpMetaData::metadataAvailable() const
{
    return m_media != 0;
}

bool QWmpMetaData::isReadOnly() const
{
    return false;
}

QList<QString> QWmpMetaData::availableMetadata() const
{
    return keys(m_media);
}

QVariant QWmpMetaData::metadata(const QString &name) const
{
    return value(m_media, name);
}

void QWmpMetaData::setMetadata(const QString &name, const QVariant &value)
{
    setValue(m_media, name, value);
}

QMediaResourceList QWmpMetaData::resources() const
{
    return resources(m_media);
}

void QWmpMetaData::currentItemChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = m_media;

    m_media = 0;
    if (dispatch)
        dispatch->QueryInterface(__uuidof(IWMPMedia), reinterpret_cast<void **>(&m_media));

    if (media) {
        if (m_media)
            emit metadataChanged();
        else
            emit metadataAvailabilityChanged(false);

        media->Release();
    } else {
        if (m_media)
            emit metadataAvailabilityChanged(false);
    }
}

void QWmpMetaData::mediaChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch &&  dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        VARIANT_BOOL isEqual = VARIANT_FALSE;
        if (media->get_isIdentical(m_media, &isEqual) == S_OK && isEqual)
            emit metadataChanged();
        media->Release();
    }
}


QStringList QWmpMetaData::keys(IWMPMedia *media)
{
    QStringList keys;

    long count = 0;
    if (media && media->get_attributeCount(&count) == S_OK) {
        for (long i = 0; i < count; ++i) {
            BSTR string;
            if (media->getAttributeName(i, &string) == S_OK) {
                keys.append(QString::fromWCharArray(string, ::SysStringLen(string)));

                ::SysFreeString(string);
            }
        }
    }
    return keys;
}

QVariant QWmpMetaData::value(IWMPMedia *media, const QString &key)
{
    const QAutoBStr bstrKey(key);

    QVariantList values;
    IWMPMedia3 *media3 = 0;
    if (media && media->QueryInterface(
            __uuidof(IWMPMedia3), reinterpret_cast<void **>(&media3)) == S_OK) {
        long count = 0;
        media3->getAttributeCountByType(bstrKey, 0, &count);

        // The count appears to only be valid for static properties, dynamic properties like
        // PlaylistIndex will have a count of zero but return a value for index 0.
        if (count == 0)
            count = 1;

        for (long i = 0; i < count; ++i) {
            VARIANT var;
            VariantInit(&var);

            if (media3->getItemInfoByType(bstrKey, 0, i, &var) == S_OK) {
                QVariant value = convertVariant(var);

                if (!value.isNull())
                    values.append(value);

                VariantClear(&var);
            }
        }
        media3->Release();
    }

    switch (values.count()) {
    case 0:
        return QVariant();
    case 1:
        return values.first();
    default:
        return values;
    }
}

void QWmpMetaData::setValue(IWMPMedia *media, const QString &key, const QVariant &value)
{
    if (qVariantCanConvert<QString>(value))
        media->setItemInfo(QAutoBStr(key), QAutoBStr(value.toString()));
}

QMediaResourceList QWmpMetaData::resources(IWMPMedia *media)
{
    QMediaResourceList resources;

    BSTR string = 0;
    if (media->get_sourceURL(&string) == S_OK) {
        QString uri = QString::fromWCharArray(static_cast<const wchar_t *>(string));
        ::SysFreeString(string);

        resources.append(QMediaResource(QUrl(uri)));

        if (media->getItemInfo(QAutoBStr(L"WM/WMCollectionGroupID"), &string) == S_OK) {
            QString uuid = QString::fromWCharArray(static_cast<const wchar_t *>(string));
            ::SysFreeString(string);

            QString albumArtLarge = QLatin1String("AlbumArt_") + uuid + QLatin1String("_Large.jpg");
            QString albumArtSmall = QLatin1String("AlbumArt_") + uuid + QLatin1String("_Small.jpg");

            QDir dir = QFileInfo(uri).absoluteDir();

            if (dir.exists(albumArtLarge)) {
                QMediaResource resource(
                        QUrl(dir.absoluteFilePath(albumArtLarge)),
                        QLatin1String("image/jpeg"),
                        QMediaResource::CoverArtRole);
                resource.setResolution(QSize(200, 200));

                resources.append(resource);
            }

            if (dir.exists(albumArtSmall)) {
                QMediaResource resource(
                        QUrl(dir.absoluteFilePath(albumArtSmall)),
                        QLatin1String("image/jpeg"),
                        QMediaResource::CoverArtRole);
                resource.setResolution(QSize(75, 75));

                resources.append(resource);
            }
        }
    }

    return resources;
}

QVariant QWmpMetaData::convertVariant(const VARIANT &variant)
{
    switch (variant.vt) {
    case VT_I2:
        return variant.iVal;
    case VT_I4:
        return variant.lVal;
    case VT_I8:
        return variant.llVal;
    case VT_UI2:
        return variant.uiVal;
    case VT_UI4:
        return quint32(variant.ulVal);
    case VT_UI8:
        return variant.ullVal;
    case VT_INT:
        return variant.intVal;
    case VT_UINT:
        return variant.uintVal;
    case VT_BSTR:
        return QString::fromWCharArray(variant.bstrVal, ::SysStringLen(variant.bstrVal));
    case VT_DISPATCH:
        {
            IWMPMetadataPicture *picture = 0;
            IWMPMetadataText *text = 0;

            if (variant.pdispVal->QueryInterface(
                    __uuidof(IWMPMetadataPicture), reinterpret_cast<void **>(&picture)) == S_OK) {
                QUrl uri;
                BSTR string;
                if (picture->get_URL(&string) == S_OK) {
                    uri = QUrl(QString::fromWCharArray(string, ::SysStringLen(string)));

                    ::SysFreeString(string);
                }
                picture->Release();
                return qVariantFromValue(uri);
            } else if (variant.pdispVal->QueryInterface(
                    __uuidof(IWMPMetadataText), reinterpret_cast<void **>(&text)) == S_OK) {
                QString description;
                BSTR string;
                if (text->get_description(&string) == S_OK) {
                    description = QString::fromWCharArray(string, SysStringLen(string));

                    ::SysFreeString(string);
                }
                text->Release();
                return description;
            } else {
                qWarning("Unknown dispatch type");
            }
        }
        break;
    default:
        qWarning("Unsupported Type %d %x", variant.vt, variant.vt);
        break;
    }

    return QVariant();
}
