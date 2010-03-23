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

#include "qws4galleryfileoperationresponse_p.h"

#include "qws4galleryrowsetresponse_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qthread.h>

#include <propkey.h>

static QString qReadStringProperty(IPropertyStore *store, const PROPERTYKEY &property)
{
    QString value;

    PROPVARIANT variant;
    if (SUCCEEDED(store->GetValue(property, &variant))) {
        switch (variant.vt) {
        case VT_UI1:
            value = QString::number(variant.bVal);
            break;
        case VT_UI2:
            value = QString::number(variant.uiVal);
            break;
        case VT_UI4:
            value = QString::number(variant.ulVal);
            break;
        case VT_UI8:
            value = QString::number(variant.uhVal.QuadPart);
            break;
        case VT_UINT:
            value = QString::number(variant.uintVal);
            break;
        case VT_INT:
            value = QString::number(variant.intVal);
            break;
        case VT_I1:
            value = QString::number(variant.cVal);
            break;
        case VT_I2:
            value = QString::number(variant.iVal);
            break;
        case VT_I4:
            value = QString::number(variant.lVal);
            break;
        case VT_I8:
            value = QString::number(variant.hVal.QuadPart);
            break;
        case VT_R4:
            value = QString::number(variant.fltVal);
            break;
        case VT_R8:
            value = QString::number(variant.dblVal);
            break;
        case VT_BSTR:
            value = QString::fromWCharArray(variant.bstrVal, ::SysStringLen(variant.bstrVal));
            break;
        case VT_ARRAY | VT_BSTR:
            {
                QStringList strings;
                BSTR *bStrings = 0;
                const int count = ::SafeArrayGetDim(variant.parray);

                if (SUCCEEDED(::SafeArrayAccessData(
                        variant.parray, reinterpret_cast<void **>(&bStrings)))) {
                    for (int i = 0; i < count; ++i) {
                        strings.append(QString::fromWCharArray(
                                bStrings[i], ::SysStringLen(bStrings[i])));
                    }
                    ::SafeArrayUnaccessData(variant.parray);
                }
                value = strings.join(QLatin1String("; "));
            }
            break;
        default:
            break;
        }
        ::PropVariantClear(&variant);
    }
    return value;
}

static QVariant qReadVariantProperty(IPropertyStore *store, const PROPERTYKEY &property)
{
    QVariant value;

    PROPVARIANT variant;
    if (SUCCEEDED(store->GetValue(property, &variant))) {
        switch (variant.vt) {
        case VT_UI1:
            value = variant.bVal;
            break;
        case VT_UI2:
            value = variant.uiVal;
            break;
        case VT_UI4:
            value = quint32(variant.ulVal);
            break;
        case VT_UI8:
            value = variant.uhVal.QuadPart;
            break;
        case VT_UINT:
            value = variant.uintVal;
            break;
        case VT_INT:
            value = variant.intVal;
            break;
        case VT_I1:
            value = variant.cVal;
            break;
        case VT_I2:
            value = variant.iVal;
            break;
        case VT_I4:
            value = variant.lVal;
            break;
        case VT_I8:
            value = variant.hVal.QuadPart;
            break;
        case VT_R4:
            value = variant.fltVal;
            break;
        case VT_R8:
            value = variant.dblVal;
            break;
        case VT_BSTR:
            value = QString::fromWCharArray(variant.bstrVal, ::SysStringLen(variant.bstrVal));
            break;
        case VT_ARRAY | VT_BSTR:
            {
                QStringList strings;
                BSTR *bStrings = 0;
                const int count = ::SafeArrayGetDim(variant.parray);

                if (SUCCEEDED(::SafeArrayAccessData(
                        variant.parray, reinterpret_cast<void **>(&bStrings)))) {
                    for (int i = 0; i < count; ++i) {
                        strings.append(QString::fromWCharArray(
                                bStrings[i], ::SysStringLen(bStrings[i])));
                    }
                    ::SafeArrayUnaccessData(variant.parray);
                }
                value = strings;
            }
            break;
        default:
            break;
        }
        ::PropVariantClear(&variant);
    }
    return value;
}

namespace QWS4Gallery
{
    struct PropertyKeyMap
    {
        QLatin1String field;
        PROPERTYKEY propertyKey;
    };
}

template <typename T, int N>
int qt_arraySize(const T (&)[N]) { return N; }

#define QT_DEFINE_GALLERY_PROPERTY_MAP(FIELD, PROPERTY) \
    { QLatin1String(#FIELD), PROPERTY }

static const QWS4Gallery::PropertyKeyMap qt_propertyKeyMap[] =
{
    QT_DEFINE_GALLERY_PROPERTY_MAP(author, PKEY_Author),
    QT_DEFINE_GALLERY_PROPERTY_MAP(copyright, PKEY_Copyright),
    QT_DEFINE_GALLERY_PROPERTY_MAP(description, PKEY_FileDescription),
    QT_DEFINE_GALLERY_PROPERTY_MAP(mimeType, PKEY_MIMEType),
    QT_DEFINE_GALLERY_PROPERTY_MAP(rating, PKEY_Rating),
    QT_DEFINE_GALLERY_PROPERTY_MAP(title, PKEY_Title),
    QT_DEFINE_GALLERY_PROPERTY_MAP(url, PKEY_ItemUrl),

    // Media
    QT_DEFINE_GALLERY_PROPERTY_MAP(duration, PKEY_Media_Duration),
    QT_DEFINE_GALLERY_PROPERTY_MAP(producer, PKEY_Media_Producer),
    QT_DEFINE_GALLERY_PROPERTY_MAP(writer, PKEY_Media_Writer),

    // Audio/Music
    QT_DEFINE_GALLERY_PROPERTY_MAP(artist, PKEY_Music_Artist),
    QT_DEFINE_GALLERY_PROPERTY_MAP(albumArtist, PKEY_Music_AlbumArtist),
    QT_DEFINE_GALLERY_PROPERTY_MAP(albumTitle, PKEY_Music_AlbumTitle),
    QT_DEFINE_GALLERY_PROPERTY_MAP(composer, PKEY_Music_Composer),
    QT_DEFINE_GALLERY_PROPERTY_MAP(genre, PKEY_Music_Genre),
    QT_DEFINE_GALLERY_PROPERTY_MAP(trackNumber, PKEY_Music_TrackNumber),

    // Image, Video common_
    QT_DEFINE_GALLERY_PROPERTY_MAP(width, PKEY_Image_HorizontalSize),
    QT_DEFINE_GALLERY_PROPERTY_MAP(width, PKEY_Video_FrameWidth),
    QT_DEFINE_GALLERY_PROPERTY_MAP(height, PKEY_Image_VerticalSize),
    QT_DEFINE_GALLERY_PROPERTY_MAP(height, PKEY_Video_FrameHeight),

    // Image/Photo
    QT_DEFINE_GALLERY_PROPERTY_MAP(dateTaken, PKEY_Photo_DateTaken),
    QT_DEFINE_GALLERY_PROPERTY_MAP(cameraManufacturer, PKEY_Photo_CameraManufacturer),
    QT_DEFINE_GALLERY_PROPERTY_MAP(cameraModel, PKEY_Photo_CameraModel),

    // Video
    QT_DEFINE_GALLERY_PROPERTY_MAP(director, PKEY_Video_Director)
};

class QWS4GalleryFileOperationResponseThread : public QThread
{
public:
    QWS4GalleryFileOperationResponseThread(QWS4GalleryFileOperationResponse *response)
        : m_response(response)
    {
    }

protected:
    void run()
    {
        m_response->run();
    }

private:
    QWS4GalleryFileOperationResponse *m_response;
};

QWS4GalleryFileOperationResponse::QWS4GalleryFileOperationResponse(
        const QStringList &fields, QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_ref(1)
    , m_adviseCookie(0)
    , m_thread(new QWS4GalleryFileOperationResponseThread(this))
    , m_result(QGalleryAbstractRequest::NoResult)
    , m_cancelled(false)
    , m_currentEmpty(true)
    , m_pendingEmpty(true)
    , m_changePending(false)
    , m_fields(fields)
{
    m_propertyKeys.resize(m_fields.count());

    for (int i = 0; i < m_fields.count(); ++i) {
        m_listKeys.append(i);
        m_propertyKeys[i] = PKEY_Null;

        for (int j = 0; j < qt_arraySize(qt_propertyKeyMap); ++j) {
            if (m_fields.at(i) == qt_propertyKeyMap[j].field) {
                m_propertyKeys[i] = qt_propertyKeyMap[j].propertyKey;
                break;
            }
        }
    }

    connect(m_thread, SIGNAL(finished()), this, SLOT(threadFinished()));

    m_thread->start();
}

QWS4GalleryFileOperationResponse::~QWS4GalleryFileOperationResponse()
{
    cancel();

    m_thread->wait();

    delete m_thread;

    Q_ASSERT(m_ref == 1);
}

QList<int> QWS4GalleryFileOperationResponse::keys() const
{
    return m_listKeys;
}

QString QWS4GalleryFileOperationResponse::toString(int key) const
{
    return m_fields.at(key);
}

int QWS4GalleryFileOperationResponse::count() const
{
    return !m_currentEmpty ? 1 : 0;
}

QString QWS4GalleryFileOperationResponse::id(int) const
{
    return m_current.url;
}

QUrl QWS4GalleryFileOperationResponse::url(int) const
{
    return m_current.url;
}

QString QWS4GalleryFileOperationResponse::type(int) const
{
    return m_current.type;
}

QList<QGalleryResource> QWS4GalleryFileOperationResponse::resources(int) const
{
    return !m_currentEmpty
            ? QList<QGalleryResource>() << QGalleryResource(m_current.url)
            : QList<QGalleryResource>();
}

QVariant QWS4GalleryFileOperationResponse::metaData(int index, int key) const
{
    return !m_currentEmpty && index == 0
            ? m_current.metaData.at(key)
            : QVariant();
}

void QWS4GalleryFileOperationResponse::setMetaData(int, int, const QVariant &)
{
}

QGalleryItemList::MetaDataFlags QWS4GalleryFileOperationResponse::metaDataFlags(int, int) const
{
    return 0;
}

void QWS4GalleryFileOperationResponse::cancel()
{
    QMutexLocker locker(&m_mutex);

    m_cancelled = true;
}

bool QWS4GalleryFileOperationResponse::waitForFinished(int msecs)
{
    if (m_thread->wait(msecs)) {
        finish(m_result);

        return true;
    } else {
        return false;
    }
}

// IUnknown
HRESULT QWS4GalleryFileOperationResponse::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IFileOperationProgressSink) {
        *ppvObject = static_cast<IFileOperationProgressSink *>(this);
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG QWS4GalleryFileOperationResponse::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWS4GalleryFileOperationResponse::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IFileOperationProgressSink
HRESULT QWS4GalleryFileOperationResponse::StartOperations()
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::FinishOperations(HRESULT)
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::PreRenameItem(DWORD, IShellItem *psiItem, LPCWSTR)
{
    return setCurrentItem(psiItem) ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PostRenameItem(
        DWORD, IShellItem *, LPCWSTR, HRESULT, IShellItem *)
{
    return clearCurrentItem() ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PreMoveItem(
        DWORD, IShellItem *psiItem, IShellItem *, LPCWSTR)
{
    return setCurrentItem(psiItem) ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PostMoveItem(
        DWORD, IShellItem *, IShellItem *, LPCWSTR, HRESULT, IShellItem *)
{
    return clearCurrentItem() ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PreCopyItem(
        DWORD, IShellItem *psiItem, IShellItem *, LPCWSTR)
{
    return setCurrentItem(psiItem) ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PostCopyItem(
        DWORD, IShellItem *, IShellItem *, LPCWSTR, HRESULT, IShellItem *)
{
    return clearCurrentItem() ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PreDeleteItem(DWORD, IShellItem *psiItem)
{
    return setCurrentItem(psiItem) ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PostDeleteItem(DWORD, IShellItem *, HRESULT, IShellItem *)
{
    return clearCurrentItem() ? S_OK : COPYENGINE_E_USER_CANCELLED;
}

HRESULT QWS4GalleryFileOperationResponse::PreNewItem(DWORD, IShellItem *, LPCWSTR)
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::PostNewItem(
        DWORD, IShellItem *, LPCWSTR, LPCWSTR, DWORD, HRESULT, IShellItem *)
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::UpdateProgress(UINT iWorkTotal, UINT iWorkSoFar)
{
    QCoreApplication::postEvent(this, new QWS4GalleryProgressEvent(iWorkSoFar, iWorkTotal));

    QMutexLocker locker(&m_mutex);

    return m_cancelled ? COPYENGINE_E_USER_CANCELLED : S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::ResetTimer()
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::PauseTimer()
{
    return S_OK;
}

HRESULT QWS4GalleryFileOperationResponse::ResumeTimer()
{
    return S_OK;
}

IShellItem *QWS4GalleryFileOperationResponse::createShellItem(const QString &id) const
{
    IShellItem *item = 0;
    // Can't invoke directly as not available in XP.
    //SHCreateItemFromParsingName(
    //        reinterpret_cast<const wchar_t *>(id.constData()),
    //        0,
    //        IID_IShellItem,
    //        reinterpret_cast<void **>(&item));
    return item;
}

void QWS4GalleryFileOperationResponse::customEvent(QEvent *event)
{
    if (event->type() == QWS4GalleryEvent::Progress) {
        QWS4GalleryProgressEvent *progressEvent = static_cast<QWS4GalleryProgressEvent *>(event);

        emit progressChanged(progressEvent->currentProgress, progressEvent->maximumProgress);
    } else if (event->type() == QWS4GalleryEvent::ItemsChanged) {
        bool pendingEmpty;
        {
            QMutexLocker locker(&m_mutex);

            pendingEmpty = m_pendingEmpty;
            m_current = m_pending;
            m_changePending = false;
        }

        if (!m_currentEmpty) {
            m_currentEmpty = true;

            emit removed(0, 1);
        }

        if (!pendingEmpty) {
            m_currentEmpty = false;

            emit inserted(0, 1);
        }
    }
}

void QWS4GalleryFileOperationResponse::threadFinished()
{
    finish(m_result);
}

void QWS4GalleryFileOperationResponse::run()
{
    ::CoInitialize(0);

    IFileOperation *operation;

    if(SUCCEEDED(CoCreateInstance(
            CLSID_FileOperation,
            0,
            CLSCTX_ALL,
            IID_IFileOperation,
            reinterpret_cast<void **>(&operation)))) {
        operation->Advise(this, &m_adviseCookie);

        m_result = initialize(operation);

        if (m_result == QGalleryAbstractRequest::NoResult) {
            HRESULT hr = operation->PerformOperations();

            if (!SUCCEEDED(hr)) {
                switch (hr) {
                case COPYENGINE_E_USER_CANCELLED:
                    m_result = QGalleryAbstractRequest::Cancelled;
                    break;
                default:
                    m_result = QGalleryAbstractRequest::RequestError;
                    break;
                }
            } else {
                m_result = QGalleryAbstractRequest::Succeeded;
            }
        }

        operation->Unadvise(m_adviseCookie);
        operation->Release();
    } else {
        m_result = QGalleryAbstractRequest::RequestError;
    }

    ::CoUninitialize();
}

bool QWS4GalleryFileOperationResponse::setCurrentItem(IShellItem *shellItem)
{
    Item item;
    bool itemValid = false;

    IPropertyStore *store = 0;
    if (SUCCEEDED(shellItem->QueryInterface(
            IID_IPropertyStore, reinterpret_cast<void **>(&store)))) {
        item.url = qReadStringProperty(store, PKEY_ItemUrl);
        item.metaData.reserve(m_propertyKeys.count());

        QVector<PROPERTYKEY>::const_iterator it = m_propertyKeys.constBegin();
        QVector<PROPERTYKEY>::const_iterator end = m_propertyKeys.end();

        for (; it != end; ++it)
            item.metaData.append(qReadVariantProperty(store, *it));
        itemValid = true;
    }

    QMutexLocker locker(&m_mutex);

    if (m_cancelled) {
        return false;
    } else if (itemValid) {
        m_pendingEmpty = false;
        m_pending = item;

        if (!m_changePending) {
            m_changePending = true;

            QCoreApplication::postEvent(this, new QWS4GalleryEvent(QWS4GalleryEvent::ItemsChanged));
        }
    }
    return true;
}

bool QWS4GalleryFileOperationResponse::clearCurrentItem()
{
    QMutexLocker locker(&m_mutex);

    m_pendingEmpty = true;

    if (!m_changePending) {
        m_changePending = true;

        QCoreApplication::postEvent(this, new QWS4GalleryEvent(QWS4GalleryEvent::ItemsChanged));
    }

    return m_cancelled;
}
