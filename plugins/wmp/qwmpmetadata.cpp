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
    , m_media3(0)
{
}

QWmpMetaData::~QWmpMetaData()
{
    if (m_media)
        m_media->Release();

    if (m_media3)
        m_media3->Release();
}

QStringList QWmpMetaData::keys() const
{
    QStringList keys;

    if (m_media) {
        long count = 0;

        if (m_media->get_attributeCount(&count) == S_OK) {
            for (long i = 0; i < count; ++i) {
                BSTR string;

                if (m_media->getAttributeName(i, &string) == S_OK) {
                    keys.append(QString::fromWCharArray(string, SysStringLen(string)));

                    SysFreeString(string);
                }
            }
        }
    }
    return keys;
}


int QWmpMetaData::count(const QString &key) const
{
    long count = 0;

    if (m_media3)
        m_media3->getAttributeCountByType(QAutoBStr(key), 0, &count);

    return count;
}

QVariant QWmpMetaData::value(const QString &key, int index) const
{
    QVariant value;

    if (m_media3) {
        VARIANT var;

        VariantInit(&var);

        if (m_media3->getItemInfoByType(QAutoBStr(key), 0, index, &var) == S_OK) {
            switch (var.vt) {
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
    }
    return value;
}

QVariantList QWmpMetaData::values(const QString &key) const
{
    const int c = count(key);

    QVariantList values;

    for (int i = 0; i < c; ++i) {
        QVariant v = value(key, i);

        if (!v.isNull())
            values.append(v);
    }

    return values;
}

IWMPMedia *QWmpMetaData::media() const
{
    return m_media;
}

void QWmpMetaData::setMedia(IWMPMedia *media)
{
    if (m_media)
        m_media->Release();
    if (m_media3)
        m_media3->Release();

    m_media = media;
    m_media3 = 0;

    if (m_media) {
        m_media->AddRef();

        m_media->QueryInterface(__uuidof(IWMPMedia3), reinterpret_cast<void **>(&m_media3));
    }

    emit changed();
}
