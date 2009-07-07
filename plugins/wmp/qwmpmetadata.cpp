/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwmpmetadata.h"

#include "qwmpglobal.h"

#include <QtCore/qstringlist.h>

QWmpMetaData::QWmpMetaData(QObject *parent)
    : QMetadataProvider(parent)
    , m_media(0)
{
}

QWmpMetaData::~QWmpMetaData()
{
    if (m_media)
        m_media->Release();
}

bool QWmpMetaData::metadataAvailable() const
{
    return true;
}

bool QWmpMetaData::isReadOnly() const
{
    return true;
}

void QWmpMetaData::setReadOnly(bool readonly)
{
    Q_UNUSED(readonly);
}

QList<QString> QWmpMetaData::availableMetadata() const
{
    return keys(m_media);
}

QVariant QWmpMetaData::metadata(const QString &name) const
{
    return value(m_media, name, 0);
}

void QWmpMetaData::setMetadata(const QString &name, const QVariant &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

IWMPMedia *QWmpMetaData::media() const
{
    return m_media;
}

void QWmpMetaData::setMedia(IWMPMedia *media)
{
    if (m_media)
        m_media->Release();

    m_media = media;

    if (m_media)
        m_media->AddRef();
}

QStringList QWmpMetaData::keys(IWMPMedia *media)
{
    QStringList keys;

    if (media) {
        long count = 0;

        if (media->get_attributeCount(&count) == S_OK) {
            for (long i = 0; i < count; ++i) {
                BSTR string;

                if (media->getAttributeName(i, &string) == S_OK) {
                    keys.append(QString::fromWCharArray(string, SysStringLen(string)));

                    SysFreeString(string);
                }
            }
        }
    }
    return keys;

}

int QWmpMetaData::valueCount(IWMPMedia *media, const QString &key)
{
    long count = 0;

    IWMPMedia3 *media3 = 0;

    if (media && media->QueryInterface(
            __uuidof(IWMPMedia3), reinterpret_cast<void **>(&media3)) == S_OK) {
        media3->getAttributeCountByType(QAutoBStr(key), 0, &count);
        media3->Release();
    }

    return count;
}

QVariant QWmpMetaData::value(IWMPMedia *media, const QString &key, int index)
{
    QVariant value;

    IWMPMedia3 *media3 = 0;
    if (media && media->QueryInterface(
            __uuidof(IWMPMedia3), reinterpret_cast<void **>(&media3)) == S_OK) {
        VARIANT var;
        VariantInit(&var);

        if (media3->getItemInfoByType(QAutoBStr(key), 0, index, &var) == S_OK) {
            switch (var.vt) {
            case VT_I4:
                value = var.lVal;
                break;
            case VT_INT:
                value = var.intVal;
                break;
            case VT_BSTR:
                value = QString::fromWCharArray(var.bstrVal, SysStringLen(var.bstrVal));
                break;
            case VT_DISPATCH:
                {
                    IWMPMetadataPicture *picture = 0;
                    IWMPMetadataText *text = 0;

                    if (var.pdispVal->QueryInterface(
                            __uuidof(IWMPMetadataPicture),
                            reinterpret_cast<void **>(&picture)) == S_OK) {
                        BSTR string;
                        if (picture->get_URL(&string) == S_OK) {
                            value = QString::fromWCharArray(string, SysStringLen(string));

                            SysFreeString(string);
                        }
                        picture->Release();
                    } else if (var.pdispVal->QueryInterface(
                            __uuidof(IWMPMetadataText),
                            reinterpret_cast<void **>(&text)) == S_OK) {
                        BSTR string;
                        if (text->get_description(&string) == S_OK) {
                            value = QString::fromWCharArray(string, SysStringLen(string));

                            SysFreeString(string);
                        }
                        text->Release();
                    }
                }
                break;
            default:
                break;
            }
            VariantClear(&var);
        }
        media3->Release();
    }
    return value;
}

QVariantList QWmpMetaData::values(IWMPMedia *media, const QString &key)
{
    QVariantList values;

    const int count = valueCount(media, key);

    for (int i = 0; i < count; ++i) {
        QVariant value = QWmpMetaData::value(media, key, i);

        if (!value.isNull())
            values.append(value);
    }

    return values;
}
