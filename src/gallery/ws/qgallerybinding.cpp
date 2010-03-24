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

#include "qgallerybinding_p.h"

#include <QtCore/qstringlist.h>

namespace QWindowsSearch
{

struct QGalleryWStr
{
    DBLENGTH length;
    wchar_t characters[1];
};

template <typename T> qint32 qReadInt32(const char *buffer)
{
    return *reinterpret_cast<const T *>(buffer);
}

template <> qint32 qReadInt32<VARIANT>(const char *buffer)
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
template <typename T> QString qReadString(const char *buffer)
{
    return QString::number(*reinterpret_cast<const T *>(buffer));
}

template <> QString qReadString<BSTR>(const char *buffer)
{
    return QString::fromWCharArray(
        reinterpret_cast<const wchar_t *>(buffer),
        ::SysStringLen(*reinterpret_cast<const BSTR *>(buffer)));
}

template <> QString qReadString<QGalleryWStr>(const char *buffer)
{
    const QGalleryWStr *string = reinterpret_cast<const QGalleryWStr *>(buffer);

    return QString::fromWCharArray(string->characters, string->length / sizeof(wchar_t));
}

template <> QString qReadString<VARIANT>(const char *buffer)
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

template <typename T> QVariant qReadVariant(const char *buffer)
{
    return qVariantFromValue<T>(*reinterpret_cast<const T *>(buffer));
}

template <> QVariant qReadVariant<BSTR>(const char *buffer)
{
    return QString::fromWCharArray(
        reinterpret_cast<const wchar_t *>(buffer),
        ::SysStringLen(*reinterpret_cast<const BSTR *>(buffer)));
}

template <> QVariant qReadVariant<QGalleryWStr>(const char *buffer)
{
    const QGalleryWStr *string = reinterpret_cast<const QGalleryWStr *>(buffer);

    return QString::fromWCharArray(string->characters, string->length / sizeof(wchar_t));
}

template <> QVariant qReadVariant<VARIANT>(const char *buffer)
{
    const VARIANT *variant = reinterpret_cast<const VARIANT *>(buffer);

    switch (variant->vt) {
    case VT_UI1:
        return variant->bVal;
    case VT_UI2:
        return variant->uiVal;
    case VT_UI4:
        return quint32(variant->ulVal);
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

template <typename R> R qReadNull(const char *)
{
    return R();
}

template <typename T> int qAligned(int offset)
{
    return (offset + sizeof(T) - 1) & ~(sizeof(T) - 1);
}

QGalleryBinding::QGalleryBinding(IRowset *rowSet)
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
                        m_int32Readers.append(qReadNull<qint32>);
                        m_stringReaders.append(qReadNull<QString>);
                        m_variantReaders.append(qReadNull<QVariant>);
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

QGalleryBinding::~QGalleryBinding()
{
    if (m_accessor) {
        m_accessor->ReleaseAccessor(m_handle, 0);
        m_accessor->Release();
    }
}

qint32 QGalleryBinding::toInt32(const char *buffer, int column) const
{
    return m_int32Readers.at(column)(buffer + m_offsets.at(column));
}

QString QGalleryBinding::toString(const char *buffer, int column) const
{
    return m_stringReaders.at(column)(buffer + m_offsets.at(column));
}

QVariant QGalleryBinding::toVariant(const char *buffer, int column) const
{
    return m_variantReaders.at(column)(buffer + m_offsets.at(column));
}

bool QGalleryBinding::generateColumnBinding(DBBINDING *binding, const DBCOLUMNINFO &column)
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
        m_int32Readers.append(qReadInt32<qint8>);
        m_stringReaders.append(qReadString<qint8>);
        m_variantReaders.append(qReadVariant<quint8>);

        valueOffset = qAligned<qint8>(m_bufferSize);
        valueSize = sizeof(qint8);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_I2:
        m_int32Readers.append(qReadInt32<qint16>);
        m_stringReaders.append(qReadString<qint16>);
        m_variantReaders.append(qReadVariant<qint16>);

        valueOffset = qAligned<qint16>(m_bufferSize);
        valueSize = sizeof(qint16);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_I4:
        m_int32Readers.append(qReadInt32<qint32>);
        m_stringReaders.append(qReadString<qint32>);
        m_variantReaders.append(qReadVariant<qint32>);

        valueOffset = qAligned<qint32>(m_bufferSize);
        valueSize = sizeof(qint32);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_I8:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<qint64>);
        m_variantReaders.append(qReadVariant<qint64>);

        valueOffset = qAligned<qint64>(m_bufferSize);
        valueSize = sizeof(qint64);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI1:
        m_int32Readers.append(qReadInt32<quint8>);
        m_stringReaders.append(qReadString<quint8>);
        m_variantReaders.append(qReadVariant<quint8>);

        valueOffset = qAligned<quint8>(m_bufferSize);
        valueSize = sizeof(quint8);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI2:
        m_int32Readers.append(qReadInt32<quint16>);
        m_stringReaders.append(qReadString<quint16>);
        m_variantReaders.append(qReadVariant<quint16>);

        valueOffset = qAligned<quint16>(m_bufferSize);
        valueSize = sizeof(quint16);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_UI4:
        m_int32Readers.append(qReadInt32<quint32>);
        m_stringReaders.append(qReadString<quint32>);
        m_variantReaders.append(qReadVariant<quint32>);

        valueOffset = qAligned<quint32>(m_bufferSize);
        valueSize = sizeof(quint32);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_UI8:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<quint64>);
        m_variantReaders.append(qReadVariant<quint64>);

        valueOffset = qAligned<quint64>(m_bufferSize);
        valueSize = sizeof(quint64);
        columnOffset = valueOffset;
        columnSize = valueSize;
        break;
    case DBTYPE_R4:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<float>);
        m_variantReaders.append(qReadVariant<float>);

        valueOffset = qAligned<float>(m_bufferSize);
        valueSize = sizeof(float);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_R8:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<double>);
        m_variantReaders.append(qReadVariant<double>);

        valueOffset = qAligned<double>(m_bufferSize);
        valueSize = sizeof(double);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_BSTR:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<BSTR>);
        m_variantReaders.append(qReadVariant<BSTR>);

        valueOffset = qAligned<BSTR>(m_bufferSize);
        valueSize = sizeof(BSTR);
        columnOffset = valueOffset;
        columnSize = valueSize;

        break;
    case DBTYPE_WSTR:
        m_int32Readers.append(qReadNull<qint32>);
        m_stringReaders.append(qReadString<QGalleryWStr>);
        m_variantReaders.append(qReadVariant<QGalleryWStr>);
   
        lengthOffset = qAligned<DBLENGTH>(m_bufferSize);
        valueOffset = lengthOffset + sizeof(DBLENGTH);
        valueSize = column.ulColumnSize;
        columnOffset = lengthOffset;
        columnSize = sizeof(DBLENGTH) + valueSize;

        part = DBPART_VALUE | DBPART_LENGTH;
        break;
    case DBTYPE_VARIANT:
        m_int32Readers.append(qReadInt32<VARIANT>);
        m_stringReaders.append(qReadString<VARIANT>);
        m_variantReaders.append(qReadVariant<VARIANT>);

        valueOffset = qAligned<VARIANT>(m_bufferSize);
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

}
