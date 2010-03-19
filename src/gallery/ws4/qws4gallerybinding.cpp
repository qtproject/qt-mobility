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

#include "qws4gallerybinding_p.h"

#include <QtCore/qstringlist.h>

struct QWS4GalleryWStr
{
    DBLENGTH length;
    wchar_t characters[1];
};

template <typename T> qint32 qws4ReadInt32(const char *buffer)
{
    return *reinterpret_cast<const T *>(buffer);
}

template <> qint32 qws4ReadInt32<VARIANT>(const char *buffer)
{
    const VARIANT *variant = reinterpret_cast<const VARIANT *>(buffer);

    switch (variant->vt) {
    case VT_UI1:
        return variant->bVal;
    case VT_UI2:
        return variant->uiVal;
    case VT_UI4:
        return variant->ulVal;
    case VT_UINT:
        return variant->uintVal;
    case VT_INT:
        return variant->intVal;
    case VT_I1:
        return variant->cVal;
    case VT_I2:
        return variant->iVal;
    case VT_I4:
        return variant->lVal;
    default:
        return qint32();
    }
}

// Assume a numeric type if not specialized.
template <typename T> QString qws4ReadString(const char *buffer)
{
    return QString::number(*reinterpret_cast<const T *>(buffer));
}

template <> QString qws4ReadString<BSTR>(const char *buffer)
{
    return QString::fromWCharArray(
        reinterpret_cast<const wchar_t *>(buffer),
        ::SysStringLen(*reinterpret_cast<const BSTR *>(buffer)));
}

template <> QString qws4ReadString<QWS4GalleryWStr>(const char *buffer)
{
    const QWS4GalleryWStr *string = reinterpret_cast<const QWS4GalleryWStr *>(buffer);

    return QString::fromWCharArray(string->characters, string->length / sizeof(wchar_t));
}

template <> QString qws4ReadString<VARIANT>(const char *buffer)
{
    const VARIANT *variant = reinterpret_cast<const VARIANT *>(buffer);

    switch (variant->vt) {
    case VT_UI1:
        return QString::number(variant->bVal);
    case VT_UI2:
        return QString::number(variant->uiVal);
    case VT_UI4:
        return QString::number(variant->ulVal);
    case VT_UI8:
        return QString::number(variant->ullVal);
    case VT_UINT:
        return QString::number(variant->uintVal);
    case VT_INT:
        return QString::number(variant->intVal);
    case VT_I1:
        return QString::number(variant->cVal);
    case VT_I2:
        return QString::number(variant->iVal);
    case VT_I4:
        return QString::number(variant->lVal);
    case VT_I8:
        return QString::number(variant->llVal);
    case VT_R4:
        return QString::number(variant->fltVal);
    case VT_R8:
        return QString::number(variant->dblVal);
    case VT_BSTR:
        return QString::fromWCharArray(variant->bstrVal, ::SysStringLen(variant->bstrVal));
    case VT_ARRAY | VT_BSTR:
        {
            QStringList strings;
            BSTR *bStrings = 0;
            const int count = ::SafeArrayGetDim(variant->parray);

            if (SUCCEEDED(::SafeArrayAccessData(
                    variant->parray, reinterpret_cast<void **>(&bStrings)))) {
                for (int i = 0; i < count; ++i) {
                    strings.append(QString::fromWCharArray(
                            bStrings[i], ::SysStringLen(bStrings[i])));
                }
                ::SafeArrayUnaccessData(variant->parray);
            }
            return strings.join(QLatin1String("; "));
        }
    default:
        return QString();
    }
}

template <typename T> QVariant qws4ReadVariant(const char *buffer)
{
    return qVariantFromValue<T>(*reinterpret_cast<const T *>(buffer));
}

template <> QVariant qws4ReadVariant<BSTR>(const char *buffer)
{
    return QString::fromWCharArray(
        reinterpret_cast<const wchar_t *>(buffer),
        ::SysStringLen(*reinterpret_cast<const BSTR *>(buffer)));
}

template <> QVariant qws4ReadVariant<QWS4GalleryWStr>(const char *buffer)
{
    const QWS4GalleryWStr *string = reinterpret_cast<const QWS4GalleryWStr *>(buffer);

    return QString::fromWCharArray(string->characters, string->length / sizeof(wchar_t));
}

template <> QVariant qws4ReadVariant<VARIANT>(const char *buffer)
{
    const VARIANT *variant = reinterpret_cast<const VARIANT *>(buffer);

    switch (variant->vt) {
    case VT_UI1:
        return variant->bVal;
    case VT_UI2:
        return variant->uiVal;
    case VT_UI4:
        return uint(variant->ulVal);
    case VT_UI8:
        return variant->ullVal;
    case VT_UINT:
        return variant->uintVal;
    case VT_INT:
        return variant->intVal;
    case VT_I1:
        return variant->cVal;
    case VT_I2:
        return variant->iVal;
    case VT_I4:
        return variant->lVal;
    case VT_I8:
        return variant->llVal;
    case VT_R4:
        return variant->fltVal;
    case VT_R8:
        return variant->dblVal;
    case VT_BSTR:
        return QString::fromWCharArray(variant->bstrVal, ::SysStringLen(variant->bstrVal));
    case VT_ARRAY | VT_BSTR:
        {
            QStringList strings;
            BSTR *bStrings = 0;
            const int count = ::SafeArrayGetDim(variant->parray);

            if (SUCCEEDED(::SafeArrayAccessData(
                    variant->parray, reinterpret_cast<void **>(&bStrings)))) {
                for (int i = 0; i < count; ++i) {
                    strings.append(QString::fromWCharArray(
                            bStrings[i], ::SysStringLen(bStrings[i])));
                }
                ::SafeArrayUnaccessData(variant->parray);
            }
            return strings;
        }
    default:
        return QVariant();
    }
}

template <typename R> R qws4ReadNull(const char *)
{
    return R();
}

template <typename T> int qws4Aligned(int offset)
{
    return (offset + sizeof(T) - 1) & ~(sizeof(T) - 1);
}

QWS4GalleryBinding::QWS4GalleryBinding(IRowset *rowSet)
    : m_bufferSize(0)
    , m_count(0)
    , m_handle(0)
    , m_accessor(0)
{
    IMalloc *malloc = 0;
    if (!SUCCEEDED(CoGetMalloc(1, &malloc)))
        return;

    HRESULT hr = 0;

    IAccessor *accessor = 0;
    if (!SUCCEEDED(hr = rowSet->QueryInterface(
            IID_IAccessor, reinterpret_cast<void **>(&accessor)))) {
        qWarning("failed to query IAccessor interface %x", hr);
    } else {
        IColumnsInfo *columnInfo = 0;
        if (!SUCCEEDED(hr = rowSet->QueryInterface(
                IID_IColumnsInfo, reinterpret_cast<void **>(&columnInfo)))) {
            qWarning("failed to query IColumnsInfo interface %x", hr);
        } else {
            DBORDINAL count = 0;
            DBCOLUMNINFO *info = 0;
            OLECHAR *stringBuffer;
            if (!SUCCEEDED(hr = columnInfo->GetColumnInfo(&count, &info, &stringBuffer))) {
                qWarning("failed to get column info %x", hr);
            } else {
                DBBINDING binding = {};

                QVector<DBBINDING> bindings;
                bindings.reserve(count);

                for (DBORDINAL i = 0; i < count; ++i) {
                    if (generateColumnBinding(&binding, info[i]))
                        bindings.append(binding);
                }

                malloc->Free(info);
                malloc->Free(stringBuffer);

                QVector<DBBINDSTATUS> status;
                status.resize(bindings.count());

                if (!bindings.isEmpty()) {
                    if (SUCCEEDED(hr = accessor->CreateAccessor(
                            DBACCESSOR_ROWDATA | DBACCESSOR_OPTIMIZED,
                            bindings.count(),
                            bindings.begin(),
                            m_bufferSize,
                            &m_handle,
                            status.begin()))) {
                        m_accessor = accessor;
                        m_accessor->AddRef();

                        m_count = m_offsets.count();

                        m_offsets.append(0);
                        m_int32Readers.append(qws4ReadNull<qint32>);
                        m_stringReaders.append(qws4ReadNull<QString>);
                        m_variantReaders.append(qws4ReadNull<QVariant>);
                    } else {
                        if (hr == 0x80040E21) {
                            QVector<DBBINDSTATUS>::const_iterator it = status.constBegin();
                            QVector<DBBINDSTATUS>::const_iterator end = status.constEnd();
                            for (; it != end; ++it) {
                                qWarning("Column status %d", *it);
                            }
                        }
                        qWarning("failed to create accesor %x", hr);
                        m_bufferSize = 0;
                        m_offsets.clear();
                        m_int32Readers.clear();
                        m_stringReaders.clear();
                        m_variantReaders.clear();
                        m_columnNames.clear();
                    }
                }
            }
            columnInfo->Release();
        }
        accessor->Release();
    }
}

QWS4GalleryBinding::~QWS4GalleryBinding()
{
    if (m_accessor) {
        m_accessor->ReleaseAccessor(m_handle, 0);
        m_accessor->Release();
    }
}

qint32 QWS4GalleryBinding::toInt32(const char *buffer, int column) const
{
    return m_int32Readers.at(column)(buffer + m_offsets.at(column));
}

QString QWS4GalleryBinding::toString(const char *buffer, int column) const
{
    return m_stringReaders.at(column)(buffer + m_offsets.at(column));
}

QVariant QWS4GalleryBinding::toVariant(const char *buffer, int column) const
{
    return m_variantReaders.at(column)(buffer + m_offsets.at(column));
}

bool QWS4GalleryBinding::generateColumnBinding(DBBINDING *binding, const DBCOLUMNINFO &column)
{
    int valueOffset = 0;
    int valueSize = 0;
    int lengthOffset = 0;
    int columnOffset = 0;
    int columnSize = 0;

    DBPART part = DBPART_VALUE;
    DBTYPE type = column.wType;
    DBMEMOWNER memOwner = DBMEMOWNER_CLIENTOWNED;

    qDebug("Column: %s"
        "\n\ttype: %d"
        "\n\tsize: %d"
        "\n\tflags: %x"
        "\n\tprecision: %d"
        "\n\tscale: %d",
        qPrintable(QString::fromWCharArray(column.pwszName)),
        int(column.wType),
        int(column.ulColumnSize),
        int(column.dwFlags),
        int(column.bPrecision),
        int(column.bScale));

    switch (column.wType) {
    case DBTYPE_I1:
        m_int32Readers.append(qws4ReadInt32<qint8>);
        m_stringReaders.append(qws4ReadString<qint8>);
        m_variantReaders.append(qws4ReadVariant<quint8>);

        valueOffset = qws4Aligned<qint8>(m_bufferSize);
        valueSize = sizeof(qint8);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_I2:
        m_int32Readers.append(qws4ReadInt32<qint16>);
        m_stringReaders.append(qws4ReadString<qint16>);
        m_variantReaders.append(qws4ReadVariant<qint16>);

        valueOffset = qws4Aligned<qint16>(m_bufferSize);
        valueSize = sizeof(qint16);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_I4:
        m_int32Readers.append(qws4ReadInt32<qint32>);
        m_stringReaders.append(qws4ReadString<qint32>);
        m_variantReaders.append(qws4ReadVariant<qint32>);

        valueOffset = qws4Aligned<qint32>(m_bufferSize);
        valueSize = sizeof(qint32);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_I8:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<qint64>);
        m_variantReaders.append(qws4ReadVariant<qint64>);

        valueOffset = qws4Aligned<qint64>(m_bufferSize);
        valueSize = sizeof(qint64);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI1:
        m_int32Readers.append(qws4ReadInt32<quint8>);
        m_stringReaders.append(qws4ReadString<quint8>);
        m_variantReaders.append(qws4ReadVariant<quint8>);

        valueOffset = qws4Aligned<quint8>(m_bufferSize);
        valueSize = sizeof(quint8);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI2:
        m_int32Readers.append(qws4ReadInt32<quint16>);
        m_stringReaders.append(qws4ReadString<quint16>);
        m_variantReaders.append(qws4ReadVariant<quint16>);

        valueOffset = qws4Aligned<quint16>(m_bufferSize);
        valueSize = sizeof(quint16);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_UI4:
        m_int32Readers.append(qws4ReadInt32<quint32>);
        m_stringReaders.append(qws4ReadString<quint32>);
        m_variantReaders.append(qws4ReadVariant<quint32>);

        valueOffset = qws4Aligned<quint32>(m_bufferSize);
        valueSize = sizeof(quint32);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI8:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<quint64>);
        m_variantReaders.append(qws4ReadVariant<quint64>);

        valueOffset = qws4Aligned<quint64>(m_bufferSize);
        valueSize = sizeof(quint64);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_R4:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<float>);
        m_variantReaders.append(qws4ReadVariant<float>);

        valueOffset = qws4Aligned<float>(m_bufferSize);
        valueSize = sizeof(float);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_R8:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<double>);
        m_variantReaders.append(qws4ReadVariant<double>);

        valueOffset = qws4Aligned<double>(m_bufferSize);
        valueSize = sizeof(double);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_BSTR:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<BSTR>);
        m_variantReaders.append(qws4ReadVariant<BSTR>);

        valueOffset = qws4Aligned<BSTR>(m_bufferSize);
        valueSize = sizeof(BSTR);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_WSTR:
        m_int32Readers.append(qws4ReadNull<qint32>);
        m_stringReaders.append(qws4ReadString<QWS4GalleryWStr>);
        m_variantReaders.append(qws4ReadVariant<QWS4GalleryWStr>);
   
        lengthOffset = qws4Aligned<DBLENGTH>(m_bufferSize);
        valueOffset = lengthOffset + sizeof(DBLENGTH);
        valueSize = column.ulColumnSize;
        columnOffset = lengthOffset;
        columnSize = sizeof(DBLENGTH) + valueSize;

        part = DBPART_VALUE | DBPART_LENGTH;
        break;
    case DBTYPE_VARIANT:
        m_int32Readers.append(qws4ReadInt32<VARIANT>);
        m_stringReaders.append(qws4ReadString<VARIANT>);
        m_variantReaders.append(qws4ReadVariant<VARIANT>);

        valueOffset = qws4Aligned<VARIANT>(m_bufferSize);
        valueSize = sizeof(VARIANT);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    default:
        qWarning("Unhandled column: %s"
            "\n\ttype: %d"
            "\n\tsize: %d"
            "\n\tflags: %x"
            "\n\tprecision: %d"
            "\n\tscale: %d",
            qPrintable(QString::fromWCharArray(column.pwszName)),
            int(column.wType),
            int(column.ulColumnSize),
            int(column.dwFlags),
            int(column.bPrecision),
            int(column.bScale));

        return false;
    }

    Q_ASSERT(columnOffset >= m_bufferSize);
    Q_ASSERT(columnSize != 0);

    binding->iOrdinal = column.iOrdinal;
    binding->obValue = valueOffset;
    binding->obLength = lengthOffset;
    binding->obStatus = 0;
    binding->pTypeInfo = 0;
    binding->pObject = 0;
    binding->pBindExt = 0;
    binding->dwPart = part;
    binding->dwMemOwner = memOwner;
    binding->eParamIO = DBPARAMIO_OUTPUT;
    binding->cbMaxLen = valueSize;
    binding->wType = type;
    binding->bPrecision = column.bPrecision;
    binding->bScale = column.bScale;

    m_offsets.append(columnOffset);
    m_columnNames.append(QString::fromWCharArray(column.pwszName));

    m_bufferSize = columnOffset + columnSize;

    return true;
}
