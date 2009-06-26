#include "qwmpmetadata.h"

#include <QtCore/qstringlist.h>

class QAutoBStr
{
public:
    inline QAutoBStr(const QString &string)
        : m_string(SysAllocString(reinterpret_cast<const wchar_t *>(string.unicode()))) 
    {
    }

    inline ~QAutoBStr()
    {
        SysFreeString(m_string);
    }

    inline operator BSTR() const { return m_string; }

private:
    BSTR m_string;
};

QWmpMetaData::QWmpMetaData(QObject *parent)
    : QMediaMetaData(parent)
    , m_media(0)
{
}

QWmpMetaData::~QWmpMetaData()
{
    if (m_media)
        m_media->Release();
}

QStringList QWmpMetaData::keys() const
{
    return keys(m_media);
}

int QWmpMetaData::valueCount(const QString &key) const
{
    return valueCount(m_media, key);
}

QVariant QWmpMetaData::value(const QString &key, int value) const
{
    return QWmpMetaData::value(m_media, key, value);
}

QVariantList QWmpMetaData::values(const QString &key) const
{
    return values(m_media, key);
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

    emit changed();
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
