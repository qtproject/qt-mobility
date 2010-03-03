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

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef MDB_ONLINE
#define MDB_ONLINE ((ULONG) 0x00000100)
#endif

#ifndef DELETE_HARD_DELETE
#define DELETE_HARD_DELETE ((ULONG) 0x00000010)
#endif

#ifndef STORE_HTML_OK
#define STORE_HTML_OK ((ULONG)0x00010000)
#endif

#ifndef PR_IS_NEWSGROUP
#define PR_IS_NEWSGROUP PROP_TAG( PT_BOOLEAN, 0x6697 )
#endif

#ifndef PR_IS_NEWSGROUP_ANCHOR
#define PR_IS_NEWSGROUP_ANCHOR PROP_TAG( PT_BOOLEAN, 0x6696 )
#endif

#ifndef PR_EXTENDED_FOLDER_FLAGS
#define PR_EXTENDED_FOLDER_FLAGS PROP_TAG( PT_BINARY, 0x36DA )
#endif

#include "winhelpers_p.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessage_p.h"
#include "qmessagecontentcontainer_p.h"
#include "qmessagefilter_p.h"
#include "qmessagesortorder_p.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderfilter_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagestore_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QThreadStorage>
#include <QTimer>
#include <QMutexLocker>


#include <shlwapi.h>
#include <shlguid.h>
#include <tchar.h>

#ifdef _WIN32_WCE
#include "win32wce/qmailmessage.h"
#include <cemapi.h>
#endif

//unexported before Windows 7, include manually
#ifndef IID_PPV_ARGS
extern "C++"
{
    template<typename T> void** IID_PPV_ARGS_Helper(T** pp)
    {
        // make sure everyone derives from IUnknown
        static_cast<IUnknown*>(*pp);

        return reinterpret_cast<void**>(pp);
    }
}
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
#endif //IID_PPV_ARGSA

QTM_BEGIN_NAMESPACE

namespace WinHelpers
{
    bool setMapiProperty(IMAPIProp *object, ULONG tag, const QString &value)
    {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.LPSZ = reinterpret_cast<LPWSTR>(const_cast<quint16*>(value.utf16()));
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool setMapiProperty(IMAPIProp *object, ULONG tag, LONG value)
    {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.l = value;
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool setMapiProperty(IMAPIProp *object, ULONG tag, ULONG value)
    {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.ul = value;
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool setMapiProperty(IMAPIProp *object, ULONG tag, bool value)
    {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.b = value;
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool setMapiProperty(IMAPIProp *object, ULONG tag, FILETIME value)
    {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.ft = value;
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool setMapiProperty(IMAPIProp *object, ULONG tag, MapiEntryId value)
    {
        SBinary s;
        s.cb = value.count();
        s.lpb = reinterpret_cast<LPBYTE>(value.data());
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.bin = s;
        return HR_SUCCEEDED(HrSetOneProp(object, &prop));
    }

    bool getMapiProperty(IMAPIProp *object, ULONG tag, ULONG *value)
    {
        bool result(false);

        SPropValue *prop;
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            if (prop->ulPropTag == tag) {
                *value = prop->Value.ul;
                result = true;
            }

            MAPIFreeBuffer(prop);
        }

        return result;
    }

    bool getMapiProperty(IMAPIProp *object, ULONG tag, LONG *value)
    {
        bool result(false);

        SPropValue *prop;
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            if (prop->ulPropTag == tag) {
                *value = prop->Value.l;
                result = true;
            }

            MAPIFreeBuffer(prop);
        }

        return result;
    }

    bool getMapiProperty(IMAPIProp *object, ULONG tag, QByteArray *value)
    {
        bool result(false);

        SPropValue *prop;
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            if (prop->ulPropTag == tag) {
                *value = QByteArray(reinterpret_cast<const char*>(prop->Value.bin.lpb), prop->Value.bin.cb);
                result = true;
            }

            MAPIFreeBuffer(prop);
        }

        return result;
    }

    bool getMapiProperty(IMAPIProp *object, ULONG tag, QString *value)
    {
        bool result(false);

        SPropValue *prop;
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            if (prop->ulPropTag == tag) {
                *value = QStringFromLpctstr(prop->Value.lpszW);
                result = true;
            }
            MAPIFreeBuffer(prop);
        }

        return result;
    }
}

using namespace WinHelpers;

namespace {

    typedef QWeakPointer<WinHelpers::MapiInitializer> InitRecord;
    typedef InitRecord *InitRecordPtr;

    QThreadStorage<InitRecordPtr> initializer;

    GUID GuidPublicStrings = { 0x00020329, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };

    FILETIME toFileTime(const QDateTime &dt)
    {
        FILETIME ft = {0};

        QDate date(dt.date());
        QTime time(dt.time());

        SYSTEMTIME st = {0};
        st.wYear = date.year();
        st.wMonth = date.month();
        st.wDay = date.day();
        st.wHour = time.hour();
        st.wMinute = time.minute();
        st.wSecond = time.second();
        st.wMilliseconds = time.msec();

        SystemTimeToFileTime(&st, &ft);
        return ft;
    }

    QDateTime fromFileTime(const FILETIME &ft)
    {
        SYSTEMTIME st = {0};
        FileTimeToSystemTime(&ft, &st);
        QString dateStr(QString("yyyy%1M%2d%3h%4m%5s%6z%7").arg(st.wYear).arg(st.wMonth).arg(st.wDay).arg(st.wHour).arg(st.wMinute).arg(st.wSecond).arg(st.wMilliseconds));
        QDateTime dt(QDateTime::fromString(dateStr, "'yyyy'yyyy'M'M'd'd'h'h'm'm's's'z'z"));
        dt.setTimeSpec(Qt::UTC);
        return dt;
    }

    struct AccountFilterPredicate
    {
        const QMessageAccountFilter &_filter;

        AccountFilterPredicate(const QMessageAccountFilter &filter) : _filter(filter) {}

        bool operator()(const MapiStorePtr &store) const
        {
            return QMessageAccountFilterPrivate::matchesStore(_filter, store);
        }
    };

    //used in preference to HrQueryAllRows
    //as per: http://blogs.msdn.com/stephen_griffin/archive/2009/03/23/try-not-to-query-all-rows.aspx

    class QueryAllRows
    {
        static const int BatchSize = 20;
        public:
        QueryAllRows(LPMAPITABLE ptable,
                LPSPropTagArray ptaga,
                LPSRestriction pres,
                LPSSortOrderSet psos,
                bool setPosition = true);
        ~QueryAllRows();

        LONG rowCount();
        bool query();
        LPSRowSet rows() const;
        QMessageManager::Error error() const;

        private:
        LPMAPITABLE m_table;
        LPSPropTagArray m_tagArray;
        LPSRestriction m_restriction;
        LPSSortOrderSet m_sortOrderSet;
        LPSRowSet m_rows;
        QMessageManager::Error m_error;
    };

    QueryAllRows::QueryAllRows(LPMAPITABLE ptable,
                               LPSPropTagArray ptaga,
                               LPSRestriction pres,
                               LPSSortOrderSet psos,
                               bool setPosition)
    :
        m_table(ptable),
        m_tagArray(ptaga),
        m_restriction(pres),
        m_sortOrderSet(psos),
        m_rows(0),
        m_error(QMessageManager::NoError)
    {
        bool initFailed = false;

#ifndef _WIN32_WCE
        unsigned long flags = TBL_BATCH;
#else
        unsigned long flags = 0;
#endif

        if(m_tagArray)
            initFailed |= FAILED(m_table->SetColumns(m_tagArray, flags));

        if(m_restriction)
            initFailed |= FAILED(m_table->Restrict(m_restriction, flags));

        if(m_sortOrderSet)
            initFailed |= FAILED(m_table->SortTable(m_sortOrderSet, flags));

        if(setPosition) {
            if(initFailed |= FAILED(m_table->SeekRow(BOOKMARK_BEGINNING,0, NULL)))
                qWarning() << "SeekRow function failed. Ensure it's not being called on hierarchy tables or message stores tables";
        }

        if(initFailed)
            m_error = QMessageManager::ContentInaccessible;
    }

    QueryAllRows::~QueryAllRows()
    {
        FreeProws(m_rows);
        m_rows = 0;
    }

    LONG QueryAllRows::rowCount()
    {
        if (m_error != QMessageManager::NoError)
            return -1;

        ULONG count(0);
        HRESULT rv = m_table->GetRowCount(0, &count);
        if (HR_FAILED(rv)) {
            m_error = QMessageManager::ContentInaccessible;
            return -1;
        }

        return count;
    }

    bool QueryAllRows::query()
    {
        if (m_error != QMessageManager::NoError)
            return false;

        FreeProws(m_rows);
        m_rows = 0;
        m_error = QMessageManager::NoError;

        HRESULT rv = m_table->QueryRows( QueryAllRows::BatchSize, NULL, &m_rows);
        if (HR_FAILED(rv)) {
            m_error = QMessageManager::ContentInaccessible;
            return false;
        }

        return (m_rows && m_rows->cRows);
    }

    LPSRowSet QueryAllRows::rows() const
    {
        return m_rows;
    }

    QMessageManager::Error QueryAllRows::error() const
    {
        return m_error;
    }

    ADRLIST *createAddressList(int count, int propertyCount)
    {
        ADRLIST *list(0);

        uint size = CbNewADRLIST(count);
        MAPIAllocateBuffer(size, reinterpret_cast<LPVOID*>(&list));
        if (list) {
            memset(list, 0, size);
            list->cEntries = count;

            for (int i = 0; i < count; ++i) {
                list->aEntries[i].cValues = propertyCount;
                MAPIAllocateBuffer(propertyCount * sizeof(SPropValue), reinterpret_cast<LPVOID*>(&list->aEntries[i].rgPropVals));
            }
        }

        return list;
    }

    void fillAddressEntry(ADRENTRY &entry, const QMessageAddress &addr, LONG type, QList<LPTSTR> &addresses)
    {
        entry.rgPropVals[0].ulPropTag = PR_RECIPIENT_TYPE;
        entry.rgPropVals[0].Value.l = type;

#ifdef _WIN32_WCE
        QString addressStr = addr.addressee();
#else
        QString addressStr("[%1:%2]");
        addressStr = addressStr.arg(addr.type() == QMessageAddress::Phone ? "SMS" : "SMTP");
        addressStr = addressStr.arg(addr.addressee());
#endif

        // TODO: Escape illegal characters, as per: http://msdn.microsoft.com/en-us/library/cc842281.aspx

        uint len = addressStr.length();
        LPTSTR address = new TCHAR[len + 1];
        memcpy(address, addressStr.utf16(), len * sizeof(TCHAR));
        address[len] = 0;

#ifdef _WIN32_WCE

        entry.rgPropVals[1].ulPropTag = PR_EMAIL_ADDRESS;
        entry.rgPropVals[1].Value.LPSZ = address;

        if(addr.type() == QMessageAddress::Email)
        {
            //Set the address type(SMTP is the only type currently supported)
            entry.rgPropVals[2].ulPropTag = PR_ADDRTYPE;
            entry.rgPropVals[2].Value.LPSZ = L"SMTP";
        }
        else if(addr.type() == QMessageAddress::Phone)
        {
            //Set the address type(SMTP is the only type currently supported)
            entry.rgPropVals[2].ulPropTag = PR_ADDRTYPE;
            entry.rgPropVals[2].Value.LPSZ = L"SMS";
        }
        else
            qWarning() << "Unrecognized address type";

#else
        entry.rgPropVals[1].ulPropTag = PR_DISPLAY_NAME;
        entry.rgPropVals[1].Value.LPSZ = address;
#endif

        addresses.append(address);
    }

    bool resolveAddressList(ADRLIST *list, IMAPISession *session)
    {
        bool result(false);

        if (session) {
            IAddrBook *book(0);
            HRESULT rv = session->OpenAddressBook(0, 0, AB_NO_DIALOG, &book);
            if (HR_SUCCEEDED(rv)) {
                rv = book->ResolveName(0, MAPI_UNICODE, 0, list);
                if (HR_SUCCEEDED(rv)) {
                    result = true;
                } else {
                    qWarning() << "Unable to resolve addresses.";
                }

                book->Release();
            } else {
                qWarning() << "Unable to open address book.";
            }
        }

        return result;
    }

    void destroyAddressList(ADRLIST *list, QList<LPTSTR> &addresses)
    {
        foreach (LPTSTR address, addresses) {
            delete [] address;
        }

        addresses.clear();

        for (uint i = 0; i < list->cEntries; ++i) {
            MAPIFreeBuffer(list->aEntries[i].rgPropVals);
        }

        MAPIFreeBuffer(list);
    }

    ULONG addAttachment(IMessage* message, const QMessageContentContainer& attachmentContainer)
    {
        IAttach *attachment(0);
        ULONG attachmentNumber(0);

        if (HR_SUCCEEDED(message->CreateAttach(NULL, 0, &attachmentNumber, &attachment))) {
            QString fileName(attachmentContainer.suggestedFileName());
            QString extension;
            int index = fileName.lastIndexOf(".");
            if (index != -1) {
                extension = fileName.mid(index);
            }

            WinHelpers::Lptstr suggestedFileNameLptstr(WinHelpers::LptstrFromQString(fileName));
            WinHelpers::Lptstr extensionLptstr(WinHelpers::LptstrFromQString(extension));

            const int nProperties = 5;
            SPropValue prop[nProperties] = { 0 };

            prop[0].ulPropTag = PR_ATTACH_METHOD;
            prop[0].Value.ul = ATTACH_BY_VALUE;
            prop[1].ulPropTag = PR_RENDERING_POSITION;
            prop[1].Value.l = -1;
            prop[2].ulPropTag = PR_ATTACH_LONG_FILENAME;
            prop[2].Value.LPSZ = suggestedFileNameLptstr;
            prop[3].ulPropTag = PR_ATTACH_FILENAME;
            prop[3].Value.LPSZ = suggestedFileNameLptstr;
            prop[4].ulPropTag = PR_ATTACH_EXTENSION;
            prop[4].Value.LPSZ = extensionLptstr;

            if (HR_SUCCEEDED(attachment->SetProps(nProperties, prop, NULL))) {
                IStream *os(0);
                if (HR_SUCCEEDED(attachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, 0, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN*)&os))) {
                    const int BUF_SIZE=4096;
                    char pData[BUF_SIZE];
                    ULONG ulSize=0,ulRead,ulWritten;

                    QDataStream attachmentStream(attachmentContainer.content());

                    ulRead=attachmentStream.readRawData(static_cast<char*>(pData), BUF_SIZE);
                    while (ulRead) {
                        os->Write(pData,ulRead, &ulWritten);

                        ulSize += ulRead;
                        ulRead = attachmentStream.readRawData(static_cast<char*>(pData), BUF_SIZE);
                    }

                    ULARGE_INTEGER uli = { 0 };
                    uli.LowPart = ulWritten;
                    os->SetSize(uli);

                    os->Commit(STGC_DEFAULT);

                    mapiRelease(os);

                    prop[0].ulPropTag=PR_ATTACH_SIZE;
                    prop[0].Value.ul=ulSize;
                    attachment->SetProps(1, prop, NULL);
#ifndef _WIN32_WCE //unsupported
                    attachment->SaveChanges(KEEP_OPEN_READONLY);
#endif
                } else {
                    qWarning() << "Could not open MAPI attachment data stream";
                }
            } else {
                qWarning() << "Could not set MAPI attachment properties";
            }

            mapiRelease(attachment);
        } else {
            qWarning() << "Could not create MAPI attachment";
        }

        return attachmentNumber;
    }

    template<typename T> QString getLastError(T& mapiType, HRESULT hr)
    {
        LPMAPIERROR err;

        HRESULT thisResult = mapiType.GetLastError(hr,MAPI_UNICODE,&err);

        if(thisResult != S_OK || !err)
        {
            qWarning() << "Could not get last MAPI error string";
            return QString();
        }

        QString mapiErrorMsg = QStringFromLpctstr(err->lpszError);
        QString mapiComponent = QStringFromLpctstr(err->lpszComponent);

        QString result = QString("MAPI Error: %1 ; MAPI Component: %2").arg(mapiErrorMsg).arg(mapiComponent);

        MAPIFreeBuffer(err);

        return result;
    }

    typedef QPair<QString, QString> StringPair;

    QList<StringPair> decomposeHeaders(const QString &headers)
    {
        QList<StringPair> result;

        if (!headers.isEmpty()) {
            int lastIndex = 0;
            int index = -2;

            do {
                index += 2;
                lastIndex = index;

                // Find CRLF not followed by whitespace
                QRegExp lineSeparator("\r\n(?!\\s)");
                index = headers.indexOf(lineSeparator, lastIndex);

                QString line = headers.mid(lastIndex, (index == -1 ? -1 : (index - lastIndex)));

                // Split the current line
                QRegExp headerIdentifier("\\s*(\\w+)\\s*:");
                if (line.indexOf(headerIdentifier) == 0) {
                    result.append(qMakePair(headerIdentifier.cap(1), line.mid(headerIdentifier.cap(0).length()).trimmed()));
                } else {
                    // Assume the whole line is an identifier
                    result.append(qMakePair(line, QString()));
                }
            } while (index != -1);
        }

        return result;
    }

    QMessageAddress createAddress(const QString &name, const QString &address)
    {
        QMessageAddress result;

        if (!name.isEmpty() || !address.isEmpty()) {
            QString from;
            if (!name.isEmpty() && !address.isEmpty() && (name != address)) {
                from = name + " <" + address + ">";
            } else {
                from = (!name.isEmpty() ? name : address);
            }

            result = QMessageAddress(QMessageAddress::Email, from);
        }

        return result;
    }

    ULONG streamSize(QMessageManager::Error* error, IStream* is)
    {
        ULONG size = 0;
        STATSTG stg = { 0 };
        HRESULT rv = is->Stat(&stg, STATFLAG_NONAME);
        if (HR_SUCCEEDED(rv)) {
            size = stg.cbSize.LowPart;
        } else {
            *error = QMessageManager::ContentInaccessible;
        }
        return size;
    }

    QByteArray readStream(QMessageManager::Error *error, IStream *is)
    {
        QByteArray result;

        STATSTG stg = { 0 };
        HRESULT rv = is->Stat(&stg, STATFLAG_NONAME);
        if (HR_SUCCEEDED(rv)) {
            ULONG sz = stg.cbSize.LowPart;
            result.resize(sz);
            ULONG bytes = 0;
            rv = is->Read(result.data(), sz, &bytes);
            if (HR_FAILED(rv)) {
                *error = QMessageManager::ContentInaccessible;
            }
        } else {
            *error = QMessageManager::ContentInaccessible;
        }

        return result;
    }

    bool writeStream(QMessageManager::Error *error, IStream *os, const void *address, ULONG bytes, bool truncate)
    {
        ULONG existingSize = truncate ? streamSize(error, os) : 0;
        if (*error == QMessageManager::NoError) {
            HRESULT rv = S_OK;
            if (existingSize > bytes) {
                ULARGE_INTEGER uli = { 0 };
                uli.LowPart = bytes;
                rv = os->SetSize(uli);
            }

            if (HR_SUCCEEDED(rv)) {
                ULONG written(0);
                rv = os->Write(address, bytes, &written);
                if (HR_SUCCEEDED(rv)) {
                    if (written < bytes) {
                        qWarning() << "Only wrote partial data to output stream.";
                    } else {
                        rv = os->Commit(STGC_DEFAULT);
                        if (HR_SUCCEEDED(rv)) {
                            return true;
                        } else {
                            qWarning() << "Unable to commit write to output stream.";
                        }
                    }
                } else {
                    qWarning() << "Unable to write data to output stream.";
                }
            } else {
                qWarning() << "Unable to resize output stream.";
            }
        }

        *error = QMessageManager::FrameworkFault;
        return false;
    }

    QString decodeContent(const QByteArray &data, const QByteArray &charset, int length = -1)
    {
        QString result;

        QTextCodec *codec = QTextCodec::codecForName(charset);
        if (codec) {
            if (length == -1) {
                // Convert the entirety
                result = codec->toUnicode(data);
            } else {
                // Convert only the first length bytes
                QTextCodec::ConverterState state;
                result = codec->toUnicode(data, length, &state);
            }
        } else {
            qWarning() << "No codec for charset:" << charset;
        }

        return result;
    }

#ifndef _WIN32_WCE
    QByteArray extractPlainText(const QByteArray &rtf)
    {
        // Attempt to extract the HTML from the RTF
        // as per CMapiEx, http://www.codeproject.com/KB/IP/CMapiEx.aspx
        QByteArray text;

        const QByteArray startTag("\\fs20");
        int index = rtf.indexOf(startTag);
        if (index != -1) {
            const QByteArray par("\\par");
            const QByteArray tab("\\tab");
            const QByteArray li("\\li");
            const QByteArray fi("\\fi-");
            const QByteArray pntext("\\pntext");

            const char zero = '\0';
            const char space = ' ';
            const char openingBrace = '{';
            const char closingBrace = '}';
            const char ignore[] = { openingBrace, closingBrace, '\r', '\n' };

            QByteArray::const_iterator rit = rtf.constBegin() + index, rend = rtf.constEnd();
            while ((rit != rend) && (*rit != zero)) {
                if (*rit == ignore[0] || *rit == ignore[1] || *rit == ignore[2] || *rit == ignore[3]) {
                    ++rit;
                } else {
                    bool skipSection(false);
                    bool skipDigits(false);
                    bool skipSpace(false);

                    const QByteArray remainder(QByteArray::fromRawData(rit, (rend - rit)));

                    if (remainder.startsWith(par)) {
                        rit += par.length();
                        text += "\r\n";
                        skipSpace = true;
                    } else if (remainder.startsWith(tab)) {
                        rit += tab.length();
                        text += "\t";
                        skipSpace = true;
                    } else if (remainder.startsWith(li)) {
                        rit += li.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(fi)) {
                        rit += fi.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(QByteArray("\\'"))) {
                        rit += 2;
                        QByteArray encodedChar(QByteArray::fromRawData(rit, 2));
                        rit += 2;
                        text += char(encodedChar.toUInt(0, 16));
                    } else if (remainder.startsWith(pntext)) {
                        rit += pntext.length();
                        skipSection = true;
                    } else if (remainder.startsWith(QByteArray("\\{"))) {
                        rit += 2;
                        text += "{";
                    } else if (remainder.startsWith(QByteArray("\\}"))) {
                        rit += 2;
                        text += "}";
                    } else {
                        text += *rit;
                        ++rit;
                    }

                    if (skipSection) {
                        while ((rit != rend) && (*rit != closingBrace)) {
                            ++rit;
                        }
                    }
                    if (skipDigits) {
                        while ((rit != rend) && isdigit(*rit)) {
                            ++rit;
                        }
                    }
                    if (skipSpace) {
                        if ((rit != rend) && (*rit == space)) {
                            ++rit;
                        }
                    }
                }
            }
        }

        return text;
    }

    int digitValue(char n)
    {
        if (n >= '0' && n <= '9') {
            return (n - '0');
        }

        return 0;
    }

    QByteArray extractHtml(const QByteArray &rtf)
    {
        // Attempt to extract the HTML from the RTF
        // as per CMapiEx, http://www.codeproject.com/KB/IP/CMapiEx.aspx
        QByteArray html;

        const QByteArray htmltag("\\*\\htmltag");
        int index = rtf.indexOf("<html", Qt::CaseInsensitive);
        if (index == -1) {
            index = rtf.indexOf(htmltag, Qt::CaseInsensitive);
        }
        if (index != -1) {
            const QByteArray mhtmltag("\\*\\mhtmltag");
            const QByteArray par("\\par");
            const QByteArray tab("\\tab");
            const QByteArray li("\\li");
            const QByteArray fi("\\fi-");
            const QByteArray pntext("\\pntext");
            const QByteArray htmlrtf("\\htmlrtf");

            const char zero = '\0';
            const char space = ' ';
            const char openingBrace = '{';
            const char closingBrace = '}';
            const char ignore[] = { openingBrace, closingBrace, '\r', '\n' };

            int tagIgnored = -1;

            QByteArray::const_iterator rit = rtf.constBegin() + index, rend = rtf.constEnd();
            while ((rit != rend) && (*rit != zero)) {
                if (*rit == ignore[0] || *rit == ignore[1] || *rit == ignore[2] || *rit == ignore[3]) {
                    ++rit;
                } else {
                    bool skipSection(false);
                    bool skipDigits(false);
                    bool skipSpace(false);

                    const QByteArray remainder(QByteArray::fromRawData(rit, (rend - rit)));

                    if (remainder.startsWith(htmltag)) {
                        rit += htmltag.length();

                        int tagNumber = 0;
                        while (isdigit(*rit)) {
                            tagNumber = (tagNumber * 10) + digitValue(*rit);
                            ++rit;
                        }
                        skipSpace = true;

                        if (tagNumber == tagIgnored) {
                            skipSection = true;
                            tagIgnored = -1;
                        }
                    } else if (remainder.startsWith(mhtmltag)) {
                        rit += mhtmltag.length();

                        int tagNumber = 0;
                        while (isdigit(*rit)) {
                            tagNumber = (tagNumber * 10) + digitValue(*rit);
                            ++rit;
                        }
                        skipSpace = true;

                        tagIgnored = tagNumber;
                    } else if (remainder.startsWith(par)) {
                        rit += par.length();
                        html += char('\r');
                        html += char('\n');
                        skipSpace = true;
                    } else if (remainder.startsWith(tab)) {
                        rit += tab.length();
                        html += char('\t');
                        skipSpace = true;
                    } else if (remainder.startsWith(li)) {
                        rit += li.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(fi)) {
                        rit += fi.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(QByteArray("\\'"))) {
                        rit += 2;

                        QByteArray encodedChar(QByteArray::fromRawData(rit, 2));
                        rit += 2;
                        html += char(encodedChar.toUInt(0, 16));
                    } else if (remainder.startsWith(pntext)) {
                        rit += pntext.length();
                        skipSection = true;
                    } else if (remainder.startsWith(htmlrtf)) {
                        rit += htmlrtf.length();

                        // Find the terminating tag
                        const QByteArray terminator("\\htmlrtf0");
                        int index = remainder.indexOf(terminator, htmlrtf.length());
                        if (index == -1) {
                            rit = rend;
                        } else {
                            rit += (index + terminator.length() - htmlrtf.length());
                            skipSpace = true;
                        }
                    } else if (remainder.startsWith(QByteArray("\\{"))) {
                        rit += 2;
                        html += openingBrace;
                    } else if (remainder.startsWith(QByteArray("\\}"))) {
                        rit += 2;
                        html += closingBrace;
                    } else {
                        html += *rit;
                        ++rit;
                    }

                    if (skipSection) {
                        while ((rit != rend) && (*rit != closingBrace)) {
                            ++rit;
                        }
                    }
                    if (skipDigits) {
                        while ((rit != rend) && isdigit(*rit)) {
                            ++rit;
                        }
                    }
                    if (skipSpace) {
                        if ((rit != rend) && (*rit == space)) {
                            ++rit;
                        }
                    }
                }
            }
        }

        return html;
    }
#endif

    void storeMessageProperties(QMessageManager::Error *error, const QMessage &source, IMessage *message)
    {
        if (!setMapiProperty(message, PR_SUBJECT, source.subject())) {
            qWarning() << "Unable to set subject in message.";
            *error = QMessageManager::FrameworkFault;
        } else {
            QString emailAddress = source.from().addressee();
            if (!setMapiProperty(message, PR_SENDER_EMAIL_ADDRESS, emailAddress)) {
                qWarning() << "Unable to set sender address in message.";
                *error = QMessageManager::FrameworkFault;
            } else {
#ifdef _WIN32_WCE
                unsigned long msgType  = 0;
                if(source.type() == QMessage::Email)
                    msgType = MSGSTATUS_RECTYPE_SMTP;
                else if(source.type() == QMessage::Sms)
                    msgType = MSGSTATUS_RECTYPE_SMS;
                else
                    qWarning() << "Unrecognized message type";

                if(msgType && !setMapiProperty(message, PR_MSG_STATUS, static_cast<long>(msgType)))
                {
                    qWarning() << "Unable to set type of message.";
                    *error = QMessageManager::FrameworkFault;
                }
                else
                {
#endif
                    if (!setMapiProperty(message, PR_CLIENT_SUBMIT_TIME, toFileTime(source.date()))) {
                        qWarning() << "Unable to set submit time in message.";
                        *error = QMessageManager::FrameworkFault;
                    } else {
                        QDateTime receivedDate(source.receivedDate());
                        if (receivedDate.isValid()) {
                            if (!setMapiProperty(message, PR_MESSAGE_DELIVERY_TIME, toFileTime(receivedDate))) {
                                qWarning() << "Unable to set delivery time in message.";
                                *error = QMessageManager::FrameworkFault;
                            }
                        }

                        if (*error == QMessageManager::NoError) {
                            // Mark this message as read/unread
#ifndef _WIN32_WCE
                            LONG flags = (source.status() & QMessage::Read ? 0 : CLEAR_READ_FLAG);
                            HRESULT rv = message->SetReadFlag(flags);
                            if (HR_FAILED(rv)) {
                                qWarning() << "Unable to set flags in message.";
                                *error = QMessageManager::FrameworkFault;
                            }
#else
                            LONG flags = (source.status() & QMessage::Read ? MSGFLAG_READ : 0);
                            if (!setMapiProperty(message, PR_MESSAGE_FLAGS, flags)) {
                                qWarning() << "Unable to set flags in message.";
                                *error = QMessageManager::FrameworkFault;
                            }
#endif
                        }

                        if (*error == QMessageManager::NoError) {
                            LONG priority = (source.priority() == QMessage::HighPriority ? PRIO_URGENT : (source.priority() == QMessage::NormalPriority ? PRIO_NORMAL : PRIO_NONURGENT));
                            if (!setMapiProperty(message, PR_PRIORITY, priority)) {
                                qWarning() << "Unable to set priority in message.";
                                *error = QMessageManager::FrameworkFault;
                            }
                        }

                        if (*error == QMessageManager::NoError) {
                            QStringList headers;
                            foreach (const QByteArray &name, source.headerFields()) {
                                foreach (const QString &value, source.headerFieldValues(name)) {
                                    // TODO: Do we need soft line-breaks?
                                    headers.append(QString("%1: %2").arg(QString(name)).arg(value));
                                }
                            }
                            if (!headers.isEmpty()) {
                                QString transportHeaders = headers.join("\r\n").append("\r\n\r\n");
                                if (!setMapiProperty(message, PR_TRANSPORT_MESSAGE_HEADERS, transportHeaders)) {
                                    qWarning() << "Unable to set transport headers in message.";
                                    *error = QMessageManager::FrameworkFault;
                                }
                            }
                        }
                    }
                }
#ifdef _WIN32_WCE
            }
#endif
        }
    }

    void replaceMessageRecipients(QMessageManager::Error *error, const QMessage &source, IMessage *message, IMAPISession *session)
    {
#ifdef _WIN32_WCE
        Q_UNUSED(session)

        // For CE, the only available option is to replace the existing list
        HRESULT rv;
#else
        // Find any existing recipients and remove them
        IMAPITable *recipientsTable(0);
        HRESULT rv = message->GetRecipientTable(MAPI_UNICODE, &recipientsTable);
        if (HR_SUCCEEDED(rv)) {
            ULONG count(0);
            rv = recipientsTable->GetRowCount(0, &count);
            if (HR_SUCCEEDED(rv)) {
                if (count > 0) {
                    ADRLIST *list = createAddressList(count, 1);
                    if (list) {
                        int index = 0;
                        SizedSPropTagArray(1, columns) = {1, {PR_ROWID}};

                        QueryAllRows qar(recipientsTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0);
                        while (qar.query()) {
                            for (uint n = 0; n < qar.rows()->cRows; ++n) {
                                ULONG rowId = qar.rows()->aRow[n].lpProps[0].Value.l;
                                if (rowId) {
                                    // Add this row's ID to the removal list
                                    ADRENTRY &entry(list->aEntries[index]);
                                    entry.rgPropVals[0].ulPropTag = PR_ROWID;
                                    entry.rgPropVals[0].Value.l = rowId;
                                    ++index;
                                }
                            }
                        }

                        if (qar.error() != QMessageManager::NoError) {
                            *error = qar.error();
                        } else {
                            rv = message->ModifyRecipients(MODRECIP_REMOVE, list);
                            if (HR_FAILED(rv)) {
                                qWarning() << "Unable to clear address list for message.";
                                *error = QMessageManager::FrameworkFault;
                            }
                        }

                        destroyAddressList(list, QList<LPTSTR>());
                    } else {
                        qWarning() << "Unable to allocate address list for message.";
                        *error = QMessageManager::FrameworkFault;
                    }
                }
            } else {
                qWarning() << "Unable to get row count for recipients table.";
                *error = QMessageManager::ContentInaccessible;
            }

            mapiRelease(recipientsTable);
        } else {
            if (rv != MAPI_E_NO_RECIPIENTS) {
                *error = QMessageManager::FrameworkFault;
                qWarning() << "Unable to get recipients table from message.";
            }
        }
#endif

        if (*error == QMessageManager::NoError) {
            // Add the current message recipients
            uint recipientCount(source.to().count() + source.cc().count() + source.bcc().count());
            if (recipientCount) {
#ifdef _WIN32_WCE
                unsigned int propertyCount = 3;
#else
                unsigned int propertyCount = 2;
#endif

                ADRLIST *list = createAddressList(recipientCount, propertyCount);
                if (list) {
                    int index = 0;
                    QList<LPTSTR> addresses;

                    foreach (const QMessageAddress &addr, source.to()) {
                        ADRENTRY &entry(list->aEntries[index]);
                        fillAddressEntry(entry, addr, MAPI_TO, addresses);
                        ++index;
                    }

                    foreach (const QMessageAddress &addr, source.cc()) {
                        ADRENTRY &entry(list->aEntries[index]);
                        fillAddressEntry(entry, addr, MAPI_CC, addresses);
                        ++index;
                    }

                    foreach (const QMessageAddress &addr, source.bcc()) {
                        ADRENTRY &entry(list->aEntries[index]);
                        fillAddressEntry(entry, addr, MAPI_BCC, addresses);
                        ++index;
                    }

#ifndef _WIN32_WCE
                    if (resolveAddressList(list, session)) {
#endif
                        rv = message->ModifyRecipients(MODRECIP_ADD, list);
                        if (HR_FAILED(rv)) {
                            qWarning() << "Unable to store address list for message.";
                            *error = QMessageManager::FrameworkFault;
                        }
#ifndef _WIN32_WCE
                    } else {
                        qWarning() << "Unable to resolve address list for message.";
                        *error = QMessageManager::FrameworkFault;
                    }
#endif

                    destroyAddressList(list, addresses);
                } else {
                    qWarning() << "Unable to allocate address list for message.";
                    *error = QMessageManager::FrameworkFault;
                }
            }
        }
    }

    void replaceMessageBody(QMessageManager::Error *error, const QMessage &source, IMessage *message, const MapiStorePtr &store)
    {
        Q_UNUSED(store);
        // Remove any preexisting body elements
#ifndef _WIN32_WCE
        SizedSPropTagArray(2, props) = {2, {PR_BODY, PR_RTF_COMPRESSED}};
#elif(_WIN32_WCE > 0x501)
        SizedSPropTagArray(2, props) = {2, {PR_BODY_HTML_A, PR_BODY_W}};
#else
        SizedSPropTagArray(2, props) = {2, {PR_BODY, PR_BODY_W}};
#endif
        HRESULT rv = message->DeleteProps(reinterpret_cast<LPSPropTagArray>(&props), 0);
#ifdef _WIN32_WCE
        if (rv == E_FAIL) {
            // On CE, deleting nonexisting properties fails... assume that they're not present
            rv = S_OK;
        }
#endif
        if (HR_SUCCEEDED(rv)) {
            // Insert the current body data
            QMessageContentContainerId bodyId(source.bodyId());
            if (bodyId.isValid()) {
                QMessageContentContainer bodyContent(source.find(bodyId));
                QByteArray subType(bodyContent.contentSubType().toLower());

#ifdef _WIN32_WCE
                if (subType == "rtf") {
                    // CE does not support RTF; just store it as plain text
                    subType = "plain";
                } else if (subType == "html") {
                    // If the store doesn't support HTML then fallback to text
                    if (!store->supports(STORE_HTML_OK)) {
                        subType = "plain";
                        qWarning() << "Store does not support HTML.";
                    }
                }

                if (subType == "html") {
                    IStream *os(0);
#if(_WIN32_WCE > 0x501)
                    HRESULT rv = message->OpenProperty(PR_BODY_HTML_A, 0, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE,(LPUNKNOWN*)&os);
#else
                    // TODO: If we store the HTML in the body property, how do we know that it is HTML on extraction?
                    HRESULT rv = message->OpenProperty(PR_BODY, 0, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE,(LPUNKNOWN*)&os);
#endif
                    if (HR_SUCCEEDED(rv)) {
                        QByteArray body(bodyContent.textContent().toLatin1());
                        writeStream(error, os, body.data(), body.count(), true);

                        mapiRelease(os);
                    } else {
                        qWarning() << "Unable to write HTML to body";
                        *error = QMessageManager::FrameworkFault;
                    }
#else
                if ((subType == "rtf") || (subType == "html")) {
                    QByteArray body(bodyContent.textContent().toLatin1());
                    LONG textFormat(EDITOR_FORMAT_RTF);
                    if (subType == "html") {
                        // Encode the HTML within RTF
                        TCHAR codePage[6] = _T("1252");
                        GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, codePage, sizeof(codePage));

                        QByteArray format("{\\rtf1\\ansi\\ansicpg" + 
                                          QString::fromUtf16(reinterpret_cast<const quint16*>(codePage)).toLatin1() +
                      "\\fromhtml1 {\\*\\htmltag1 ");
                      body = format + body + "}}";
                        textFormat = EDITOR_FORMAT_HTML;
                    }

                    // Mark this message as formatted
                    if (!setMapiProperty(message, PR_MSG_EDITOR_FORMAT, textFormat)) {
                        qWarning() << "Unable to set message editor format in message.";
                        *error = QMessageManager::FrameworkFault;
                    }

                    IStream *os(0);
                    rv = message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_CREATE | STGM_WRITE, MAPI_CREATE | MAPI_MODIFY, reinterpret_cast<LPUNKNOWN*>(&os));
                    if (HR_SUCCEEDED(rv)) {
                        IStream *compressor(0);
                        rv = WrapCompressedRTFStream(os, MAPI_MODIFY, &compressor);
                        if (HR_SUCCEEDED(rv)) {
                            // The wrapper stream does not support Stat()
                            writeStream(error, compressor, body.data(), body.length(), false);

                            compressor->Release();
                        } else {
                            qWarning() << "Unable to open RTF compressor stream for write.";
                            *error = QMessageManager::FrameworkFault;
                        }

                        os->Release();
                    } else {
                        qWarning() << "Unable to open compressed RTF stream for write.";
                        *error = QMessageManager::FrameworkFault;
                    }
#endif
                } else {
                    QString body(bodyContent.textContent());

#ifdef _WIN32_WCE
                    IStream *os(0);
                    HRESULT rv = message->OpenProperty(PR_BODY_W, 0, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE,(LPUNKNOWN*)&os);
                    if (HR_SUCCEEDED(rv)) {
                        writeStream(error, os, body.utf16(), body.count() * sizeof(WCHAR), true);

                        mapiRelease(os);
                    } else {
                        qWarning() << "Unable to write text to body";
                        *error = QMessageManager::FrameworkFault;
                    }
#else
                    // Mark this message as plain text
                    LONG textFormat(EDITOR_FORMAT_PLAINTEXT);
                    if (!setMapiProperty(message, PR_MSG_EDITOR_FORMAT, textFormat)) {
                        qWarning() << "Unable to set message editor format in message.";
                        *error = QMessageManager::FrameworkFault;
                    }

                    if (!setMapiProperty(message, PR_BODY, body)) {
                        qWarning() << "Unable to set body in message.";
                        *error = QMessageManager::FrameworkFault;
                    }
#endif
                }
            }
        } else {
            qWarning() << "Unable to delete existing body properties from message.";
            *error = QMessageManager::FrameworkFault;
        }
    }

    void addMessageAttachments(QMessageManager::Error *error, const QMessage &source, IMessage *message, WinHelpers::SavePropertyOption saveOption = WinHelpers::SavePropertyChanges )
    {
        Q_UNUSED(saveOption);

        QList<LONG> attachmentNumbers;

        if (MapiSession::messageImpl(source)->_hasAttachments) {
            // Find any existing attachments
            IMAPITable *attachmentsTable(0);
            HRESULT rv = message->GetAttachmentTable(MAPI_UNICODE, &attachmentsTable);
            if (HR_SUCCEEDED(rv)) {
                SizedSPropTagArray(1, columns) = {1, {PR_ATTACH_NUM}};

                QueryAllRows qar(attachmentsTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0, false);
                while (qar.query()) {
                    for (uint n = 0; n < qar.rows()->cRows; ++n) {
                        attachmentNumbers.append(qar.rows()->aRow[n].lpProps[0].Value.l);
                    }
                }

                *error = qar.error();
                if (*error != QMessageManager::NoError) {
                    qWarning() << "Unable to get attachments numbers from table.";
                }
                mapiRelease(attachmentsTable);
            } else {
                qWarning() << "Unable to get attachments table from message.";
                *error = QMessageManager::FrameworkFault;
            }
        }

        if (*error == QMessageManager::NoError) {
            // Add any current attachments not present
            foreach (const QMessageContentContainerId &attachmentId, source.attachmentIds()) {
                QMessageContentContainer attachment(source.find(attachmentId));
                bool isAttachment = (!attachment.suggestedFileName().isEmpty() && attachment.isContentAvailable());
                if (isAttachment) {
                    LONG number(MapiSession::containerImpl(attachment)->_attachmentNumber);
                    if (!number || !attachmentNumbers.contains(number)) {
                        addAttachment(message, attachment);
                    }
                }
            }
        }
    }

    struct FolderHeapNode
    {
        FolderHeapNode(const MapiFolderPtr &folder, const QMessageFilter &filter);

        QMessageFilter _filter;
        MapiFolderPtr _folder;
        LPMAPITABLE _table;
        QMessage _front;
    };

    FolderHeapNode::FolderHeapNode(const MapiFolderPtr &folder, const QMessageFilter &filter)
        : _filter(filter),
          _folder(folder)
    {
    }

    typedef QSharedPointer<FolderHeapNode> FolderHeapNodePtr;

    // FolderHeap is a binary heap used to merge sort messages from different folders and stores
    class FolderHeap {
    public:
        FolderHeap(QMessageManager::Error *error, const QList<FolderHeapNodePtr> &protoHeap, const QMessageSortOrder &sortOrder);
        ~FolderHeap();

        QMessage takeFront(QMessageManager::Error *error);
        bool isEmpty() const { return _heap.count() == 0; }

    private:
        void sink(int i); // Also known as sift-down

        QMessageFilter _filter;
        QMessageSortOrder _ordering;
        QList<FolderHeapNodePtr> _heap;
    };

    FolderHeap::FolderHeap(QMessageManager::Error *error, const QList<FolderHeapNodePtr> &protoHeap, const QMessageSortOrder &sortOrder)
    {
        _ordering = sortOrder;

        foreach (const FolderHeapNodePtr &node, protoHeap) {
            if (!node->_folder) {
                qWarning() << "Unable to access folder:" << node->_folder->name();
                continue;
            }

            QMessageIdList messageIdList;
            node->_table = node->_folder->queryBegin(error, node->_filter, sortOrder);
            if (*error == QMessageManager::NoError) {
                if (node->_table) {
                    messageIdList = node->_folder->queryNext(error, node->_table, node->_filter);
                    if (messageIdList.isEmpty() || (*error != QMessageManager::NoError)) {
                        node->_folder->queryEnd(node->_table);
                    }
                }

                if (!messageIdList.isEmpty()) {
                    node->_front = node->_folder->message(error, messageIdList.front());

                    if (*error == QMessageManager::NoError) {
                        _heap.append(node);
                    }
                }
            }
        }

        if (!_ordering.isEmpty()) {
            for (int i = _heap.count()/2 - 1; i >= 0; --i)
                sink(i);
        }
    }

    FolderHeap::~FolderHeap()
    {
    }

    QMessage FolderHeap::takeFront(QMessageManager::Error *error)
    {
        QMessage result(_heap[0]->_front);

        FolderHeapNodePtr node(_heap[0]);

        QMessageIdList messageIdList;
        if (node->_table) {
            messageIdList = node->_folder->queryNext(error, node->_table, node->_filter);
            if (messageIdList.isEmpty() || (*error != QMessageManager::NoError)) {
                node->_folder->queryEnd(node->_table);
            }
        }
        if (*error != QMessageManager::NoError)
            return result;

        if (messageIdList.isEmpty()) {
            if (_heap.count() > 1) {
                _heap[0] = _heap.takeLast();
            } else {
                _heap.pop_back();
            }
        } else {
            node->_front = node->_folder->message(error, messageIdList.front());
            if (*error != QMessageManager::NoError)
                return result;
        }

        if (!_ordering.isEmpty()) {
            // Reposition this folder in the heap based on the new front message
            sink(0);
        }
        return result;
    }

    void FolderHeap::sink(int i)
    {
        while (true) {
            const int leftChild(2*i + 1);
            if (leftChild >= _heap.count())
                return;

            const int rightChild(leftChild + 1);
            int minChild(leftChild);
            if ((rightChild < _heap.count()) && (QMessageSortOrderPrivate::lessThan(_ordering, _heap[rightChild]->_front, _heap[leftChild]->_front)))
                minChild = rightChild;

            // Reverse positions only if the child sorts before the parent
            if (QMessageSortOrderPrivate::lessThan(_ordering, _heap[minChild]->_front, _heap[i]->_front)) {
                FolderHeapNodePtr temp(_heap[minChild]);
                _heap[minChild] = _heap[i];
                _heap[i] = temp;
                i = minChild;
            } else {
                return;
            }
        }
    }

    bool bodyMatches(const QMessage &message, const QString &body, QMessageDataComparator::MatchFlags matchFlags)
    {
        if (body.isEmpty())
            return true;

        QMessageContentContainer bodyContainer(message.find(message.bodyId()));
        if (matchFlags & QMessageDataComparator::MatchCaseSensitive) {
            return bodyContainer.textContent().contains(body, Qt::CaseSensitive);
        }
        return bodyContainer.textContent().contains(body, Qt::CaseInsensitive);
    }

    QMessageIdList filterMessages(QMessageManager::Error *error, QList<FolderHeapNodePtr> &folderNodes, const QMessageSortOrder &sortOrder, uint limit, uint offset, const QString &body = QString(), QMessageDataComparator::MatchFlags matchFlags = 0)
    {
        QMessageIdList result;
        QHash<QMessageId, bool> avoidDuplicates; // For complex filters it's necessary to check for duplicates

        FolderHeap folderHeap(error, folderNodes, sortOrder);
        if (*error != QMessageManager::NoError)
            return result;

        int count = 0 - offset;
        while (!folderHeap.isEmpty()) {
            // Ideally would not retrieve unwanted messages using IMAPITable::SeekRow
            // But this is not feasible on Windows Mobile http://msdn.microsoft.com/en-us/library/bb446068.aspx:
            // 'If there are large numbers of rows in the table, the SeekRow oepration can be slow. Do not set lRowCount ot a numer greater than 50'
            if (limit && (count == limit))
                break;

            QMessage front(folderHeap.takeFront(error));
            if (*error != QMessageManager::NoError)
                return result;

            if (!avoidDuplicates.contains(front.id()) && bodyMatches(front, body, matchFlags)) {
                avoidDuplicates.insert(front.id(), true);
                if (count >= 0) {
                    result.append(front.id());
                }

                ++count;
            }
        }

        if (offset) {
            return result.mid(offset, (limit ? limit : -1));
        } else {
            return result;
        }
    }

}

    QEvent::Type MapiSession::NotifyEvent::eventType()
    {
        static int result = QEvent::registerEventType();
        return static_cast<QEvent::Type>(result);
    }

    MapiSession::NotifyEvent::NotifyEvent(MapiStore *store, const QMessageId &id, MapiSession::NotifyType type)
        : QEvent(eventType()),
          _store(store),
          _id(id),
          _notifyType(type)
    {
    }

    QEvent::Type MapiSession::NotifyEvent::type()
    {
        return eventType();
    }

namespace WinHelpers {

    // Note: UNICODE is always defined
    QString QStringFromLpctstr(LPCTSTR lpszValue)
    {
        bool isBadStringPointer = false;
#ifndef _WIN32_WCE
        isBadStringPointer = ::IsBadStringPtr(lpszValue, (UINT_PTR)-1); // Don't crash when MAPI returns a bad string (and it does).
#endif

        if (!lpszValue || isBadStringPointer)
            return QString();

        return QString::fromUtf16(reinterpret_cast<const quint16*>(lpszValue));
    }

    Lptstr LptstrFromQString(const QString &src)
    {
        uint length(src.length());
        Lptstr dst(length+1);

        const quint16 *data = src.utf16();
        const quint16 *it = data, *end = data + length;
        TCHAR *oit = dst;
        for ( ; it != end; ++it, ++oit) {
            *oit = static_cast<TCHAR>(*it);
        }
        *oit = TCHAR('\0');
        return dst;
    }

    ULONG createNamedProperty(IMAPIProp *object, const QString &name)
    {
        ULONG result = 0;

        if (!name.isEmpty()) {
            Lptstr nameBuffer = LptstrFromQString(name);

            MAPINAMEID propName = { 0 };
            propName.lpguid = &GuidPublicStrings;
            propName.ulKind = MNID_STRING;
            propName.Kind.lpwstrName = nameBuffer;

            LPMAPINAMEID propNames = &propName;

            SPropTagArray *props;
            HRESULT rv = object->GetIDsFromNames(1, &propNames, MAPI_CREATE, &props);
            if (HR_SUCCEEDED(rv)) {
                result = props->aulPropTag[0] | PT_UNICODE;

                MAPIFreeBuffer(props);
            } else {
                qWarning() << "createNamedProperty: GetIDsFromNames failed";
            }
        }

        return result;
    }

    ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name)
    {
        ULONG result = 0;

        if (!name.isEmpty()) {
            LPTSTR nameBuffer = LptstrFromQString(name);

            MAPINAMEID propName = { 0 };
            propName.lpguid = &GuidPublicStrings;
            propName.ulKind = MNID_STRING;
            propName.Kind.lpwstrName = nameBuffer;

            LPMAPINAMEID propNames = &propName;

            SPropTagArray *props;
            HRESULT rv = object->GetIDsFromNames(1, &propNames, 0, &props);
            if (HR_SUCCEEDED(rv)) {
                if (props->aulPropTag[0] != PT_ERROR) {
                    result = props->aulPropTag[0] | PT_UNICODE;
                }

                MAPIFreeBuffer(props);
            } else {
                qWarning() << "getNamedPropertyTag: GetIDsFromNames failed";
            }
        }

        return result;
    }

    bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value)
    {
        if (object && tag && !value.isEmpty()) {
            SPropValue prop = { 0 };
            prop.ulPropTag = tag;
            prop.Value.LPSZ = reinterpret_cast<LPTSTR>(const_cast<quint16*>(value.utf16()));

            HRESULT rv = object->SetProps(1, &prop, 0);
            if (HR_SUCCEEDED(rv)) {
                return true;
            } else {
                qWarning() << "setNamedProperty: SetProps failed";
            }
        }

        return false;
    }

    QString getNamedProperty(IMAPIProp *object, ULONG tag)
    {
        QString result;

        if (object && tag) {
            SPropValue *prop(0);
            HRESULT rv = HrGetOneProp(object, tag, &prop);
            if (HR_SUCCEEDED(rv)) {
                result = QStringFromLpctstr(prop->Value.LPSZ);

                MAPIFreeBuffer(prop);
            } else if (rv != MAPI_E_NOT_FOUND) {
                qWarning() << "getNamedProperty: HrGetOneProp failed";
            }
        }

        return result;
    }

    QByteArray contentTypeFromExtension(const QString &extension)
    {
        QByteArray result("application/octet-stream");

        if (!extension.isEmpty()) {
            // Create the extension string to search for
            QString dotExtension(extension);
            if (!dotExtension.startsWith('.')) {
                dotExtension.prepend('.');
            }

            Lptstr ext = LptstrFromQString(dotExtension);

#if !defined(_WIN32_WCE) && (_MSC_VER>=1500)
            IQueryAssociations *associations(0);
            HRESULT rv = AssocCreate(CLSID_QueryAssociations, IID_PPV_ARGS(&associations));
            if (HR_SUCCEEDED(rv)) {
                rv = associations->Init(0, ext, 0, 0);
                if (HR_SUCCEEDED(rv)) {
                    // Find the length of the content-type string
                    DWORD length = 0;
                    rv = associations->GetString(0, ASSOCSTR_CONTENTTYPE, 0, 0, &length);
                    if ((rv == S_FALSE) && length) {
                        // Retrieve the string
                        wchar_t *buffer = new wchar_t[length + 1];
                        buffer[length] = '\0';
                        rv = associations->GetString(0, ASSOCSTR_CONTENTTYPE, 0, buffer, &length);
                        if (rv == S_OK) {
                            QString output(QString::fromUtf16(reinterpret_cast<quint16*>(buffer)));
                            result = output.toLatin1();
                        }

                        delete [] buffer;
                    }
                }
                mapiRelease(associations);
            }
#elif  !defined(_WIN32_WCE) && (_MSC_VER<1500)
            //TODO Windows 2005 and 2003 don't have IID_PPV_ARGS.
            //find alternative
#else
            // Find any registry entry for this extension
            HKEY key = { 0 };
            LONG rv = RegOpenKeyEx(HKEY_CLASSES_ROOT, ext, 0, 0, &key);
            if (rv == ERROR_SUCCESS) {
                WCHAR value[512] = { 0 };
                DWORD valueBytes = sizeof(value);
                rv = RegQueryValueEx(key, L"Content Type", 0, 0, reinterpret_cast<LPBYTE>(&value), &valueBytes);
                if (rv == ERROR_SUCCESS) {
                    if (valueBytes > 1) {
                        result = QStringFromLpctstr(value).toLatin1();
                    }
                } else {
                    qWarning() << "Unable to query key for extension:" << extension;
                }

                RegCloseKey(key);
            }
#endif
        }

        return result;
    }

    MapiInitializer::MapiInitializer()
        : _initialized(false)
    {
#ifndef QT_NO_THREAD
        // Note MAPIINIT is ignored on Windows Mobile but used on Outlook 2007 see msdn ms862621 vs cc842343
        MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
        LPVOID arg(&MAPIINIT);
#else
        LPVOID arg(0);
#endif
        HRESULT rv = MAPIInitialize(arg);
        if (HR_SUCCEEDED(rv)) {
            _initialized = true;
        } else {
            _initialized = false;
            qWarning() << "Unable to initialize MAPI - rv:" << hex << (ULONG)rv;
        }
    }

    MapiInitializer::~MapiInitializer()
    {
        if (_initialized) {
            MAPIUninitialize();
            _initialized = false;
        }
    }

    MapiInitializationToken initializeMapi()
    {
        MapiInitializationToken result;

        if (!initializer.hasLocalData()) {
            initializer.setLocalData(new InitRecord);
        }

        InitRecordPtr &threadInitializer(initializer.localData());
        if (!(*threadInitializer).isNull()) {
            result = (*threadInitializer).toStrongRef();
        } else {
            result = MapiInitializationToken(new MapiInitializer());
            (*threadInitializer) = result;
        }

        return result;
    }

    class StoreSortHelper {
    public:
        StoreSortHelper(const QMessageAccountSortOrder *sortOrder, MapiStorePtr storePtr)
            :_ordering(sortOrder),
             _storePtr(storePtr)
        {}

        MapiStorePtr store()
        {
            return _storePtr;
        }

        StoreSortHelper& operator=(const StoreSortHelper &other) {
            if (&other == this)
                return *this;
            _ordering = other._ordering;
            _storePtr = other._storePtr;
            return *this;
        }

        bool operator<(const StoreSortHelper &other) const
        {
            bool result (_storePtr->name() < other._storePtr->name());
            if (QMessageAccountSortOrderPrivate::order(*_ordering) == Qt::DescendingOrder)
                result = !result;
            return result;
        }
    private:
        const QMessageAccountSortOrder *_ordering;
        MapiStorePtr _storePtr;
    };

    class FolderSortHelper {
    public:
        FolderSortHelper(const QMessageFolderSortOrder *sortOrder, MapiFolderPtr folderPtr, const QMessageFolder &folder)
            :_ordering(sortOrder),
             _folderPtr(folderPtr),
             _folder(folder)
        {}

        MapiFolderPtr mapiFolderPtr()
        {
            return _folderPtr;
        }

        QMessageFolder folder()
        {
            return _folder;
        }

        FolderSortHelper& operator=(const FolderSortHelper &other) {
            if (&other == this)
                return *this;
            _ordering = other._ordering;
            _folderPtr = other._folderPtr;
            _folder = other._folder;
            return *this;
        }

        bool operator<(const FolderSortHelper &other) const
        {
            return QMessageFolderSortOrderPrivate::lessthan(*_ordering, _folder, other._folder);
        }
    private:
        const QMessageFolderSortOrder *_ordering;
        MapiFolderPtr _folderPtr;
        QMessageFolder _folder;
    };
}

using namespace WinHelpers;

MapiFolder::MapiFolder()
    :_valid(false),
     _folder(0),
     _hasSubFolders(false),
     _messageCount(0),
     _init(false)
{
}

MapiFolderPtr MapiFolder::createFolder(QMessageManager::Error *error, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
{
    Q_UNUSED(error);
    MapiFolderPtr ptr = MapiFolderPtr(new MapiFolder(store, folder, recordKey, name, entryId, hasSubFolders, messageCount));
    if (!ptr.isNull()) {
        ptr->_self = ptr;
    }
    return ptr;
}

MapiFolder::MapiFolder(const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
    :_store(store),
     _valid(folder != 0),
     _folder(folder),
     _key(recordKey),
     _name(name),
     _entryId(entryId),
     _hasSubFolders(hasSubFolders),
     _messageCount(messageCount),
     _init(false)
{
}

MapiFolder::~MapiFolder()
{
    mapiRelease(_folder);
    _valid = false;
    _init = false;
}

void MapiFolder::findSubFolders(QMessageManager::Error *error)
{
    if (!_folder) {
        *error = QMessageManager::FrameworkFault;
        qWarning() << "No folder to search for subfolders";
    } else if (_hasSubFolders) {
        IMAPITable *subFolders(0);
        HRESULT rv = _folder->GetHierarchyTable(0, &subFolders);
        if (HR_SUCCEEDED(rv)) {
            SizedSPropTagArray(5, columns) = {5, {PR_ENTRYID, PR_IS_NEWSGROUP, PR_IS_NEWSGROUP_ANCHOR, PR_EXTENDED_FOLDER_FLAGS, PR_FOLDER_TYPE}};
            rv = subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
            if (HR_SUCCEEDED(rv)) {
                // Extract the Entry IDs for the subfolders
                while (true) {
                    LPSRowSet rows(0);
                    if (subFolders->QueryRows(1, 0, &rows) == S_OK) {
                        if (rows->cRows == 1) {
                            SRow *row(&rows->aRow[0]);

                            MapiEntryId entryId(row->lpProps[0].Value.bin.lpb, row->lpProps[0].Value.bin.cb);
                            bool isNewsGroup = (row->lpProps[1].ulPropTag == PR_IS_NEWSGROUP && row->lpProps[1].Value.b);
                            bool isNewsGroupAnchor = (row->lpProps[2].ulPropTag == PR_IS_NEWSGROUP_ANCHOR && row->lpProps[2].Value.b);

                            bool special(isNewsGroup || isNewsGroupAnchor);
#ifndef _WIN32_WCE
                            // Skip slow folders, necessary evil
                            if (row->lpProps[3].ulPropTag == PR_EXTENDED_FOLDER_FLAGS) {
                                QByteArray extendedFlags(reinterpret_cast<const char*>(row->lpProps[3].Value.bin.lpb), row->lpProps[3].Value.bin.cb);
                                if (extendedFlags[2] & 8) { // Synchronization issues, skip like Outlook
                                    special = true;
                                }
                            } else if (row->lpProps[4].ulPropTag == PR_FOLDER_TYPE) {
                                if (row->lpProps[4].Value.ul != FOLDER_GENERIC) {
                                    special = true;
                                }
                            } else {
                                special = true;
                            }
#endif
                            FreeProws(rows);

                            if (special) {
                                // Doesn't contain messages that should be searched...
                            }  else {
                                _subFolders.append(entryId);
                            }
                        } else {
                            // We have retrieved all rows
                            FreeProws(rows);
                            break;
                        }
                    } else {
                        *error = QMessageManager::ContentInaccessible;
                        qWarning() << "Error getting rows from sub folder table.";
                        break;
                    }
                }
            } else {
                *error = QMessageManager::ContentInaccessible;
                qWarning() << "Unable to set columns on folder table.";
            }

            mapiRelease(subFolders);
        } else {
            *error = QMessageManager::ContentInaccessible;
            qWarning() << "Unable to get hierarchy table for folder:" << name();
        }
    }
}

MapiFolderPtr MapiFolder::nextSubFolder(QMessageManager::Error *error)
{
    MapiFolderPtr result;

    if (!_hasSubFolders)
        return result;

    if (!_init) {
        _init = true;

        findSubFolders(error);
        if (*error != QMessageManager::NoError) {
            qWarning() << "Unable to find sub folders.";
            return result;
        }
    }

    if (!_subFolders.isEmpty()) {
        result = _store->openFolder(error, _subFolders.takeFirst());
    } else {
        // Allow the traversal to be restarted
        _init = false;
    }

    return result;
}

LPMAPITABLE MapiFolder::queryBegin(QMessageManager::Error *error, const QMessageFilter &filter, const QMessageSortOrder &sortOrder)
{
    if (!_valid || !_folder) {
        Q_ASSERT(_valid && _folder);
        *error = QMessageManager::FrameworkFault;
        return 0;
    }

    MapiRestriction restriction(filter);
    if (!restriction.isValid()) {
        *error = QMessageManager::ConstraintFailure;
        return 0;
    }

    LPMAPITABLE messagesTable(0);
    HRESULT rv = _folder->GetContentsTable(MAPI_UNICODE, &messagesTable);
    if (HR_SUCCEEDED(rv)) {
        SizedSPropTagArray(2, columns) = {2, {PR_ENTRYID, PR_RECORD_KEY}};
        rv = messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
        if (HR_SUCCEEDED(rv)) {
            if (!sortOrder.isEmpty()) {
                QMessageSortOrderPrivate::sortTable(error, sortOrder, messagesTable);
            }
            if (!restriction.isEmpty()) {
                ULONG flags(0);
                if (messagesTable->Restrict(restriction.sRestriction(), flags) != S_OK)
                    *error = QMessageManager::ConstraintFailure;
            }
            if (*error != QMessageManager::NoError) {
                return 0;
            }
            return messagesTable;
        } else {
            *error = QMessageManager::ContentInaccessible;
            return 0;
        }

        mapiRelease(messagesTable);
        messagesTable = 0;
    } else {
        *error = QMessageManager::ContentInaccessible;
        return 0;
    }

    return 0;
}

QMessageIdList MapiFolder::queryNext(QMessageManager::Error *error, LPMAPITABLE messagesTable, const QMessageFilter &filter)
{
    QMessageIdList result;
    while (true) {
        LPSRowSet rows(0);
        HRESULT rv = messagesTable->QueryRows(1, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            if (rows->cRows == 1) {
                LPSPropValue entryIdProp(&rows->aRow[0].lpProps[0]);
                LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[1]);
                MapiRecordKey recordKey(recordKeyProp->Value.bin.lpb, recordKeyProp->Value.bin.cb);
                MapiEntryId entryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);
        #ifdef _WIN32_WCE
                QMessageId id(QMessageIdPrivate::from(_store->entryId(), entryId, recordKey, _entryId));
        #else
                QMessageId id(QMessageIdPrivate::from(_store->recordKey(), entryId, recordKey, _key));
        #endif
                FreeProws(rows);

                if (QMessageFilterPrivate::matchesMessageRequired(filter)
                    && !QMessageFilterPrivate::matchesMessageSimple(filter, QMessage(id)))
                    continue;
                result.append(id);
                return result;
            } else {
                // We have retrieved all rows
                FreeProws(rows);
                return result;
            }
        } else {
            *error = QMessageManager::ContentInaccessible;
            qWarning() << "Unable to query rows in message table.";
            return result;
        }
    }
}

void MapiFolder::queryEnd(LPMAPITABLE messagesTable)
{
    mapiRelease(messagesTable);
}

uint MapiFolder::countMessages(QMessageManager::Error *error, const QMessageFilter &filter) const
{
    uint result(0);

    if (!_valid || !_folder) {
        Q_ASSERT(_valid && _folder);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    MapiRestriction restriction(filter);
    if (!restriction.isValid()) {
        *error = QMessageManager::ConstraintFailure;
        return result;
    }

    IMAPITable *messagesTable(0);
    HRESULT rv = _folder->GetContentsTable(MAPI_UNICODE, &messagesTable);
    if (HR_SUCCEEDED(rv)) {
        if (!restriction.isEmpty()) {
            ULONG flags(0);
            rv = messagesTable->Restrict(restriction.sRestriction(), flags);
            if (HR_FAILED(rv)) {
                *error = QMessageManager::ConstraintFailure;
                qWarning() << "Unable to set restriction on message table.";
            }
        }
        if (*error == QMessageManager::NoError) {
            ULONG count(0);
            rv = messagesTable->GetRowCount(0, &count);
            if (HR_SUCCEEDED(rv)) {
                result = count;
            } else {
                qWarning() << "Unable to count messages in folder.";
            }
        }

        mapiRelease(messagesTable);
    } else {
        *error = QMessageManager::ContentInaccessible;
        qWarning() << "Unable to get folder contents table.";
    }

    return result;
}

void MapiFolder::removeMessages(QMessageManager::Error *error, const QMessageIdList &ids)
{
    SBinary *bin(0);
    if (MAPIAllocateBuffer(ids.count() * sizeof(SBinary), reinterpret_cast<LPVOID*>(&bin)) != S_OK) {
        return;
    }

    int index = 0;
    foreach (const QMessageId &id, ids) {
        MapiEntryId entryId(QMessageIdPrivate::entryId(id));

        bin[index].cb = entryId.count();
        if (MAPIAllocateMore(bin[index].cb, bin, reinterpret_cast<LPVOID*>(&bin[index].lpb)) != S_OK) {
            break;
        }

        memcpy(bin[index].lpb, entryId.constData(), bin[index].cb);
        ++index;
    }

    if (index > 0) {
        ENTRYLIST entryList = { 0 };
        entryList.cValues = index;
        entryList.lpbin = bin;

        ULONG flags(0);
        //flags |= DELETE_HARD_DELETE;  this flag is only available when the store is exchange...
        HRESULT rv = _folder->DeleteMessages(&entryList, 0, 0, flags);
        if (HR_FAILED(rv)) {
            *error = QMessageManager::ContentInaccessible;
            qWarning() << "Unable to delete messages from folder.";
        }
    }

    MAPIFreeBuffer(bin);
}

MapiEntryId MapiFolder::messageEntryId(QMessageManager::Error *error, const MapiRecordKey &messageKey)
{
    MapiEntryId result;
    MapiRecordKey key(messageKey);

    IMAPITable *messagesTable(0);
    HRESULT rv = _folder->GetContentsTable(MAPI_UNICODE, &messagesTable);
    if (HR_SUCCEEDED(rv)) {
        SPropValue keyProp;
        keyProp.ulPropTag = PR_RECORD_KEY;
        keyProp.Value.bin.cb = key.count();
        keyProp.Value.bin.lpb = reinterpret_cast<LPBYTE>(key.data());

        SRestriction restriction;
        restriction.rt = RES_PROPERTY;
        restriction.res.resProperty.relop = RELOP_EQ;
        restriction.res.resProperty.ulPropTag = PR_RECORD_KEY;
        restriction.res.resProperty.lpProp = &keyProp;

        ULONG flags(0);
        rv = messagesTable->Restrict(&restriction, flags);
        if (HR_SUCCEEDED(rv)) {
            SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};
            rv = messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
            if (HR_SUCCEEDED(rv)) {
                LPSRowSet rows(0);
                rv = messagesTable->QueryRows(1, 0, &rows);
                if (HR_SUCCEEDED(rv)) {
                    if (rows->cRows == 1) {
                        LPSPropValue entryIdProp(&rows->aRow[0].lpProps[0]);
                        result = MapiEntryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);
                    } else {
                        *error = QMessageManager::InvalidId;
                    }
                    FreeProws(rows);
                } else {
                    *error = QMessageManager::ContentInaccessible;
                    qWarning() << "Unable to query rows from message table.";
                }
            } else {
                *error = QMessageManager::ContentInaccessible;
                qWarning() << "Unable to set columns on message table.";
            }
        } else {
            *error = QMessageManager::ContentInaccessible;
            qWarning() << "Unable to restrict content table.";
        }

        mapiRelease(messagesTable);
    } else {
        *error = QMessageManager::ContentInaccessible;
        qWarning() << "Unable to get contents table for folder.";
    }

    return result;
}

IMessage *MapiFolder::openMessage(QMessageManager::Error *error, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _folder->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (rv != S_OK) {
        *error = QMessageManager::InvalidId;
        qWarning() << "Invalid message entryId:" << entryId.toBase64();
    }

    return message;
}

QMessageFolder MapiFolder::folder(QMessageManager::Error *error, const QMessageFolderId& id) const
{
    return _store->folder(error, id);
}

QMessage MapiFolder::message(QMessageManager::Error *error, const QMessageId& id) const
{
    return _store->message(error, id);
}

QMessage::StandardFolder MapiFolder::standardFolder() const
{
    return _store->standardFolder(_entryId);
}

QMessageFolderId MapiFolder::id() const
{
#ifdef _WIN32_WCE
    return QMessageFolderIdPrivate::from(_key, _store->entryId(), _entryId);
#else
    return QMessageFolderIdPrivate::from(_key, _store->recordKey(), _entryId);
#endif
}

QMessageAccountId MapiFolder::accountId() const
{
    return _store->id();
}

QMessageFolderId MapiFolder::parentId() const
{
    MapiEntryId parentEntryId;
    if (getMapiProperty(_folder, PR_PARENT_ENTRYID, &parentEntryId)) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
        MapiFolderPtr parent(_store->openFolder(&ignoredError, parentEntryId));
        if (!parent.isNull()) {
            QMessageManager::Error ignoredError(QMessageManager::NoError);
            MapiFolderPtr root(_store->rootFolder(&ignoredError));
            if ((ignoredError != QMessageManager::NoError) 
                || !_store->session()->equal(parent->entryId(), root->entryId())) {
                return parent->id();
            }
        }
    }

    return QMessageFolderId();
}

QList<QMessageFolderId> MapiFolder::ancestorIds() const
{
    QList<QMessageFolderId> result;

    QMessageManager::Error ignoredError(QMessageManager::NoError);
    MapiFolderPtr root(_store->rootFolder(&ignoredError));

    if (ignoredError == QMessageManager::NoError) {
        MapiFolderPtr current = _self.toStrongRef();
        MapiSessionPtr session = _store->session();

        while ((ignoredError == QMessageManager::NoError) && !session->equal(current->entryId(), root->entryId())) {
            // Find the parent of this folder and append to the list of ancestors
            MapiEntryId parentEntryId;
            if (getMapiProperty(current->_folder, PR_PARENT_ENTRYID, &parentEntryId)) {
                QMessageManager::Error ignoredError(QMessageManager::NoError);
                current = _store->openFolder(&ignoredError, parentEntryId);
                if (!current.isNull() && !session->equal(current->entryId(), root->entryId())) {
                    result.append(current->id());
                }
            }
        }
    }

    return result;
}

MapiRecordKey MapiFolder::storeKey() const
{
    return _store->recordKey();
}

#ifdef _WIN32_WCE
MapiEntryId MapiFolder::storeEntryId() const
{
    return _store->entryId();
}
#endif

static unsigned long commonFolderMap(QMessage::StandardFolder folder)
{
    static bool init = false;
    static QMap<QMessage::StandardFolder,unsigned long> propertyMap;

    if(!init)
    {
        propertyMap.insert(QMessage::DraftsFolder,PROP_TAG(PT_BINARY,0x36D7));
        propertyMap.insert(QMessage::TrashFolder,PROP_TAG(PT_BINARY,0x35E3));
        propertyMap.insert(QMessage::OutboxFolder,PROP_TAG(PT_BINARY,0x35E2));
        propertyMap.insert(QMessage::SentFolder,PROP_TAG(PT_BINARY,0x35E4));

        init = true;
    }

    return propertyMap.value(folder);
}

IMessage *MapiFolder::createMessage(QMessageManager::Error* error)
{
    IMessage *message = 0;

    if(FAILED(_folder->CreateMessage(NULL, 0, &message)!=S_OK))
    {
        *error = QMessageManager::FrameworkFault;
        mapiRelease(message);
    }
    return message;
}

IMessage* MapiFolder::createMessage(QMessageManager::Error* error, const QMessage& source, const MapiSessionPtr &session, SavePropertyOption saveOption )
{
    IMessage* mapiMessage(0);
    HRESULT rv = _folder->CreateMessage(0, 0, &mapiMessage);
    if (HR_SUCCEEDED(rv)) {
        // Store the message properties
        if (*error == QMessageManager::NoError) {
            storeMessageProperties(error, source, mapiMessage);
        }

#ifndef _WIN32_WCE
        //Ensure the message is moved to the sent folder after submission
        //On Windows Mobile occurs by default and at discretion of mail client settings.

        MapiFolderPtr sentFolder = _store->findFolder(error,QMessage::SentFolder);
        if (!sentFolder.isNull() && *error == QMessageManager::NoError) {
            if (!setMapiProperty(mapiMessage, PR_SENTMAIL_ENTRYID, sentFolder->entryId())) {
                qWarning() << "Unable to set sent folder entry id on message";
            }
        }
#endif

        if (*error == QMessageManager::NoError) {
            replaceMessageRecipients(error, source, mapiMessage, session->session());
        }
        if (*error == QMessageManager::NoError) {
            replaceMessageBody(error, source, mapiMessage, _store);
        }
        if (*error == QMessageManager::NoError) {
            addMessageAttachments(error, source, mapiMessage, saveOption );
        }
#ifndef _WIN32_WCE //unsupported
        if (*error == QMessageManager::NoError && saveOption == SavePropertyChanges ) {
            if (HR_FAILED(mapiMessage->SaveChanges(0))) {
                qWarning() << "Unable to save changes for message.";
            }
        }
#endif
        if (*error != QMessageManager::NoError) {
            mapiRelease(mapiMessage);
        }
    } else {
        qWarning() << "Failed to create MAPI message";
        *error = QMessageManager::FrameworkFault;
    }

    return mapiMessage;
}

MapiStorePtr MapiStore::createStore(QMessageManager::Error *error, const MapiSessionPtr &session, IMsgStore *store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode)
{
    Q_UNUSED(error);
    MapiStorePtr ptr = MapiStorePtr(new MapiStore(session, store, key, entryId, name, cachedMode));
    if (!ptr.isNull()) {
        ptr->_self = ptr;
    }
    return ptr;
}

MapiStore::MapiStore()
    :_valid(false),
     _store(0),
     _cachedMode(true),
     _adviseConnection(0)
{
}

MapiStore::MapiStore(const MapiSessionPtr &session, IMsgStore *store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode)
    :_session(session),
     _valid(true),
     _store(store),
     _key(key),
     _entryId(entryId),
     _name(name),
     _cachedMode(cachedMode),
     _adviseConnection(0)
{
    // Find which standard folders the store contains
    foreach (QMessage::StandardFolder sf, QList<QMessage::StandardFolder>() << QMessage::InboxFolder
                                                                            << QMessage::DraftsFolder
                                                                            << QMessage::TrashFolder
                                                                            << QMessage::SentFolder
                                                                            << QMessage::OutboxFolder) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
        MapiEntryId entryId(standardFolderId(&ignoredError, sf));
        if (!entryId.isEmpty()) {
            _standardFolderId[sf] = entryId;
        }
    }
}

MapiStore::~MapiStore()
{
    _folderMap.clear();

    if (_adviseConnection != 0) {
        _store->Unadvise(_adviseConnection);
    }
    mapiRelease(_store);
    _valid = false;
}

MapiFolderPtr MapiStore::findFolder(QMessageManager::Error *error, QMessage::StandardFolder sf)
{
    MapiFolderPtr result;

    if (_standardFolderId.contains(sf)) {
        result = openFolder(error, _standardFolderId[sf]);
    }

    return result;
}

MapiEntryId MapiStore::standardFolderId(QMessageManager::Error *error, QMessage::StandardFolder sf) const
{
    MapiEntryId result;

#ifndef _WIN32_WCE
    //check if the store supports the common folder (not possible for mobile)
    bool commonFolderSupported(false);

    unsigned long tag(0);
    switch(sf)
    {
    case QMessage::InboxFolder:
        tag = FOLDER_IPM_INBOX_VALID;
        break;
    case QMessage::OutboxFolder:
        tag = FOLDER_IPM_OUTBOX_VALID;
        break;
    case QMessage::TrashFolder:
        tag = FOLDER_IPM_WASTEBASKET_VALID;
        break;
    case QMessage::SentFolder:
        tag = FOLDER_IPM_SENTMAIL_VALID;
        break;
    case QMessage::DraftsFolder:
        //assume drafts exists in every store since there is no mask for it
        commonFolderSupported = true;
        break;
    }

    if (tag) {
        ULONG folderSupportMask(0);
        if (getMapiProperty(_store, PR_VALID_FOLDER_MASK, &folderSupportMask)) {
            if (folderSupportMask & tag) {
                commonFolderSupported = true;
            }
        } else {
            qWarning() << "Could not get folder mask property for store:" << _name;
        }
    }

    if (!commonFolderSupported) {
        return result;
    }
#endif

    if (sf == QMessage::InboxFolder) {
        result = receiveFolderId(error);
    } else {
        IMAPIProp* source = _store;
        MapiFolderPtr rf;

#ifndef _WIN32_WCE
        //the drafts entryid can only be queried on the inbox or root folder.

        if(sf == QMessage::DraftsFolder) {
            rf = receiveFolder(error);
            if(*error != QMessageManager::NoError) {
                //inbox failed, try root folder.
                rf = rootFolder(error);
                if(*error != QMessageManager::NoError)
                {
                    return result;
                }
            }
            source = rf->folder();
        }
#endif

        MapiEntryId entryId;
        if (getMapiProperty(source, commonFolderMap(sf), &entryId)) {
            result = entryId;
        } else {
            *error = QMessageManager::ContentInaccessible;
        }
    }


    return result;
}

QMessageFolderIdList MapiStore::folderIds(QMessageManager::Error *error) const
{
    QMessageFolderIdList folderIds;

    MapiFolderPtr root(rootFolder(error));
    if (*error == QMessageManager::NoError) {
        QList<MapiFolderPtr> folders;
        folders.append(root);
        // No valid reason to list the root folder.

        while (!folders.isEmpty()) {
            MapiFolderPtr subFolder(folders.back()->nextSubFolder(error));
            if (!subFolder.isNull() && subFolder->isValid()) {
                folderIds.append(subFolder->id());
                folders.append(subFolder);
            } else {
                folders.pop_back();
            }
        }
    } else {
        qWarning() << "Unable to open root folder for store:" << _name;
    }

    return folderIds;
}

QMessageFolder MapiStore::folderFromId(QMessageManager::Error *error, const QMessageFolderId &folderId)
{
    QMessageFolder result;
    QList<MapiFolderPtr> folders;
    MapiFolderPtr root(rootFolder(error));
    if (!root.isNull() && root->isValid())
        folders.append(root);

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(error));
        if (!subFolder.isNull() && subFolder->isValid()) {
            if (folderId == subFolder->id()) {
                QStringList path;
                for (int i = 0; i < folders.count(); ++i) {
                    if (!folders[i]->name().isEmpty()) {
                        path.append(folders[i]->name());
                    }
                }
                path.append(subFolder->name());
                result = QMessageFolderPrivate::from(subFolder->id(), id(), folders.last()->id(), subFolder->name(), path.join("/"));
                return result;
            }
            folders.append(subFolder);
        } else {
            folders.pop_back();
        }
    }

    return result;
}

QList<MapiFolderPtr> MapiStore::filterFolders(QMessageManager::Error *error, const QMessageFolderFilter &afilter) const
{
    QList<MapiFolderPtr> result;
    QMessageFolderFilter filter(QMessageFolderFilterPrivate::preprocess(error, _session, afilter));
    if (*error != QMessageManager::NoError)
        return result;

#if 0 //(was ifndef _WIN32_WCE) TODO: fix issue with GetHierarchyTable only returning top level folders
    MapiFolderPtr root(rootFolder(error));
    if (*error == QMessageManager::NoError) {
        IMAPITable *folderTable(0);
        HRESULT rv = root->_folder->GetHierarchyTable(CONVENIENT_DEPTH | MAPI_UNICODE, &folderTable);
        if (HR_SUCCEEDED(rv)) {
            SizedSPropTagArray(5, columns) = {5, {PR_ENTRYID, PR_IS_NEWSGROUP, PR_IS_NEWSGROUP_ANCHOR, PR_EXTENDED_FOLDER_FLAGS, PR_FOLDER_TYPE}};
            QueryAllRows qar(folderTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0, false);
            while (qar.query()) {
                for (uint n = 0; n < qar.rows()->cRows; ++n) {
                    SPropValue *props = qar.rows()->aRow[n].lpProps;

                    bool isNewsGroup = (props[1].ulPropTag == PR_IS_NEWSGROUP && props[1].Value.b);
                    bool isNewsGroupAnchor = (props[2].ulPropTag == PR_IS_NEWSGROUP_ANCHOR && props[2].Value.b);
                    bool special(isNewsGroup || isNewsGroupAnchor);

                    // Skip slow folders, necessary evil
                    if (props[3].ulPropTag == PR_EXTENDED_FOLDER_FLAGS) {
                        QByteArray extendedFlags(reinterpret_cast<const char*>(props[3].Value.bin.lpb), props[3].Value.bin.cb);
                        if (extendedFlags[2] & 8) { // Synchronization issues, skip like Outlook
                            special = true;
                        }
                    } else if (props[4].ulPropTag == PR_FOLDER_TYPE) {
                        if (props[4].Value.ul != FOLDER_GENERIC) {
                            special = true;
                        }
                    } else {
                        special = true;
                    }

                    if (!special) {
                        MapiEntryId entryId(props[0].Value.bin.lpb, props[0].Value.bin.cb);
                        MapiFolderPtr folder(openFolder(error, entryId));
                        if (QMessageFolderFilterPrivate::matchesFolder(filter, folder)) {
                            result.append(folder);
                        }
                    }
                }
            }

            *error = qar.error();

            mapiRelease(folderTable);
        }
    }
#else
    // Windows mobile does not support CONVENIENT_DEPTH...
    foreach (const QMessageFolderId &folderId, folderIds(error)) {
        MapiEntryId entryId = QMessageFolderIdPrivate::entryId(folderId);
        MapiFolderPtr folder(openFolder(error, entryId));
        if (QMessageFolderFilterPrivate::matchesFolder(filter, folder)) {
            result.append(folder);
        }
    }
#endif

    return result;
}

MapiEntryId MapiStore::messageEntryId(QMessageManager::Error *error, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey)
{
    MapiEntryId result;

    MapiFolderPtr folder(openFolderWithKey(error, folderKey));
    if (*error == QMessageManager::NoError) {
        result = folder->messageEntryId(error, messageKey);
    }

    return result;
}

MapiFolderPtr MapiStore::openFolder(QMessageManager::Error *error, const MapiEntryId& entryId) const
{
    MapiFolderPtr result(0);

    // See if we can create a new pointer to an existing folder
    QWeakPointer<MapiFolder> &existing = _folderMap[entryId];
    if (!existing.isNull()) {
        // Get a pointer to the existing folder
        result = existing.toStrongRef();
        if (!result.isNull()) {
            return result;
        }
    }

    // We need to create a new instance
    IMAPIFolder *folder = openMapiFolder(error, entryId);
    if (folder && (*error == QMessageManager::NoError)) {
        SizedSPropTagArray(4, columns) = {4, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
            uint messageCount = properties[2].Value.ul;
            bool hasSubFolders = properties[3].Value.b;

            MapiFolderPtr folderPtr = MapiFolder::createFolder(error, _self.toStrongRef(), folder, recordKey, name, entryId, hasSubFolders, messageCount);
            if (*error == QMessageManager::NoError) {
                result = folderPtr;

                // Add to the map
                _folderMap.insert(entryId, result);
            } else {
                qWarning() << "Error creating folder object";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to access folder properties";
            mapiRelease(folder);
        }
    } else {
        qWarning() << "Unable to open folder.";
    }

    return result;
}

MapiFolderPtr MapiStore::openFolderWithKey(QMessageManager::Error *error, const MapiRecordKey &recordKey) const
{
#if 0 // Doesn't work on desktop or mobile, only searches top level folders
    MapiFolderPtr result;

    MapiFolderPtr root(rootFolder(error));
    if (*error == QMessageManager::NoError) {
        if (root->recordKey() == recordKey) {
            result = root;
        } else {
            IMAPITable *folderTable(0);
            // TODO: this won't work on windows mobile:
            HRESULT rv = root->_folder->GetHierarchyTable(CONVENIENT_DEPTH | MAPI_UNICODE, &folderTable);
            if (HR_SUCCEEDED(rv)) {
                // Find the entry ID corresponding to this recordKey
                SPropValue keyProp = { 0 };
                keyProp.ulPropTag = PR_RECORD_KEY;
                keyProp.Value.bin.cb = recordKey.count();
                keyProp.Value.bin.lpb = reinterpret_cast<LPBYTE>(const_cast<char*>(recordKey.data()));

                SRestriction restriction = { 0 };
                restriction.rt = RES_PROPERTY;
                restriction.res.resProperty.relop = RELOP_EQ;
                restriction.res.resProperty.ulPropTag = PR_RECORD_KEY;
                restriction.res.resProperty.lpProp = &keyProp;

                ULONG flags(0);
                rv = folderTable->Restrict(&restriction, flags);
                if (HR_SUCCEEDED(rv)) {
                    SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};
                    rv = folderTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
                    if (HR_SUCCEEDED(rv)) {
                        SRowSet *rows(0);
                        if (folderTable->QueryRows(1, 0, &rows) == S_OK) {
                            if (rows->cRows == 1) {
                                MapiEntryId entryId(rows->aRow[0].lpProps[0].Value.bin.lpb, rows->aRow[0].lpProps[0].Value.bin.cb);

                                 // Open the folder using its entry ID
                                result = openFolder(error, entryId);
                            } else {
                                *error = QMessageManager::InvalidId;
                            }
                            FreeProws(rows);
                        } else {
                            *error = QMessageManager::InvalidId;
                        }
                    } else {
                        *error = QMessageManager::ContentInaccessible;
                        qWarning() << "Unable to set columns for folder table";
                    }
                } else {
                    *error = QMessageManager::ContentInaccessible;
                    qWarning() << "Unable to restrict folder table";
                }

                mapiRelease(folderTable);
            } else {
                *error = QMessageManager::ContentInaccessible;
                qWarning() << "Unable to open hierarchy table for store.";
            }
        }
    }

    return result;
#else
    foreach (const QMessageFolderId &folderId, folderIds(error)) {
        MapiRecordKey key = QMessageFolderIdPrivate::folderRecordKey(folderId);
        if (key == recordKey) {
            MapiEntryId entryId = QMessageFolderIdPrivate::entryId(folderId);
            MapiFolderPtr folder(openFolder(error, entryId));
            return folder;
        }
    }

    return MapiFolderPtr();
#endif
}

bool MapiStore::supports(ULONG featureFlag) const
{
    LONG supportMask(0);

    if (getMapiProperty(store(), PR_STORE_SUPPORT_MASK, &supportMask)) {
        return supportMask & featureFlag;
    }

    // Otherwise, we can't query the store support, so just assume that the feature is supported
    return true;
}

IMessage *MapiStore::openMessage(QMessageManager::Error *error, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _store->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (HR_SUCCEEDED(rv)) {
        if (objectType != MAPI_MESSAGE) {
            qWarning() << "Not a message - wrong object type:" << objectType;
            mapiRelease(message);
            *error = QMessageManager::InvalidId;
        }
    } else {
        *error = QMessageManager::InvalidId;
        qWarning() << "Invalid message entryId:" << entryId.toBase64();
    }

    return message;
}

IMAPIFolder *MapiStore::openMapiFolder(QMessageManager::Error *error, const MapiEntryId &entryId) const
{
    IMAPIFolder *folder(0);

    // TODO:See MS KB article 312013, OpenEntry is not re-entrant, also true of MAPI functions in general?
    // TODO:See ms859378, GetPropsExample for alternative memory allocation technique
    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _store->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&folder));
    if (HR_SUCCEEDED(rv)) {
        if (objectType != MAPI_FOLDER) {
            qWarning() << "Not a folder - wrong object type:" << objectType;
            mapiRelease(folder);
            *error = QMessageManager::InvalidId;
        }
    } else {
        *error = QMessageManager::InvalidId;
        qWarning() << "Invalid folder entryId:" << entryId.toBase64();
    }

    return folder;
}


QMessageAccountId MapiStore::id() const
{
#ifdef _WIN32_WCE
    return QMessageAccountIdPrivate::from(_entryId);
#else
    return QMessageAccountIdPrivate::from(_key);
#endif
}

QMessage::TypeFlags MapiStore::types() const
{
    QMessage::TypeFlags flags(QMessage::Email);

#ifdef _WIN32_WCE
    if (name().toUpper() == "SMS") {
        // On Windows Mobile SMS store is named "SMS"
        flags = QMessage::Sms;
    }
#endif

    return flags;
}

// TODO: can we remove this?
QMessageAddress MapiStore::address() const
{
    QMessageAddress result;


    return result;
}

MapiSessionPtr MapiStore::session() const
{
    return _session.toStrongRef();
}

MapiFolderPtr MapiStore::rootFolder(QMessageManager::Error *error) const
{
    MapiFolderPtr result;

    MapiEntryId entryId(rootFolderId(error));
    if (*error == QMessageManager::NoError) {
        result = openFolder(error, entryId);
    }

    return result;
}

MapiEntryId MapiStore::rootFolderId(QMessageManager::Error *error) const
{
    MapiEntryId result;

    if (!_valid || !_store) {
        Q_ASSERT(_valid && _store);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    MapiEntryId entryId;
    if (getMapiProperty(_store, PR_IPM_SUBTREE_ENTRYID, &entryId)) {
        result = entryId;
    } else {
        *error = QMessageManager::ContentInaccessible;
        qWarning() << "Unable to get root folder entry ID for store:" << _name;
    }

    return result;
}

MapiFolderPtr MapiStore::receiveFolder(QMessageManager::Error *error) const
{
    MapiFolderPtr result;

    MapiEntryId entryId(receiveFolderId(error));
    if (*error == QMessageManager::NoError) {
        result = openFolder(error, entryId);
    }

    return result;
}

MapiEntryId MapiStore::receiveFolderId(QMessageManager::Error *error) const
{
    MapiEntryId result;

    if (!_valid || !_store) {
        Q_ASSERT(_valid && _store);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    ULONG entryIdSize = 0;
    LPENTRYID entryIdPtr = 0;
    HRESULT rv = _store->GetReceiveFolder(0, 0, &entryIdSize, &entryIdPtr, 0);
    if (HR_SUCCEEDED(rv)) {
        result = MapiEntryId(entryIdPtr, entryIdSize);

        MAPIFreeBuffer(entryIdPtr);
    } else {
        *error = QMessageManager::FrameworkFault;
    }

    return result;
}

QMessageFolder MapiStore::folder(QMessageManager::Error *error, const QMessageFolderId& id) const
{
    return _session.toStrongRef()->folder(error, id);
}

QMessage MapiStore::message(QMessageManager::Error *error, const QMessageId& id) const
{
    return _session.toStrongRef()->message(error, id);
}

QMessage::StandardFolder MapiStore::standardFolder(const MapiEntryId &entryId) const
{
    QMap<QMessage::StandardFolder, MapiEntryId>::const_iterator it = _standardFolderId.begin(), end = _standardFolderId.end();
    for ( ; it != end; ++it) {
        if (_session.toStrongRef()->equal(it.value(), entryId)) {
            return it.key();
        }
    }

    return QMessage::InboxFolder;
}

bool MapiStore::setAdviseSink(ULONG mask, IMAPIAdviseSink *sink)
{
    if (_adviseConnection != 0) {
        _store->Unadvise(_adviseConnection);
    }

    HRESULT rv = _store->Advise(0, 0, mask, sink, &_adviseConnection);
    if (HR_FAILED(rv)) {
        qWarning() << "Unable to register for notifications from store.";
        return false;
    }

    return true;
}

void MapiStore::notifyEvents(ULONG mask)
{
    // Test whether this store supports notifications

    if (supports(STORE_NOTIFY_OK)) {
        AdviseSink *sink(new AdviseSink(this));
        if (setAdviseSink(mask, sink)) {
            // sink will be deleted when the store releases it
        } else {
            delete sink;
        }
    } else {
        qWarning() << "Store does not support notifications.";
    }

 }

#ifdef _WIN32_WCE

QString MapiStore::transportName() const
{
    QString result;
        if(!getMapiProperty(_store,PR_CE_TRANSPORT_NAME,&result))
            qWarning() << "Could not query transport name for store " << name();

    return result;
}

#endif

HRESULT MapiStore::AdviseSink::QueryInterface(REFIID id, LPVOID FAR* o)
{
    if (id == IID_IUnknown) {
        *o = this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG MapiStore::AdviseSink::AddRef()
{
    return InterlockedIncrement(&_refCount);
}

ULONG MapiStore::AdviseSink::Release()
{
    ULONG result = InterlockedDecrement(&_refCount);
    if (result == 0) {
        delete this;
    }

    return result;
}

ULONG MapiStore::AdviseSink::OnNotify(ULONG notificationCount, LPNOTIFICATION notifications)
{
    MapiSessionPtr session = _store->_session.toStrongRef();
    if (!session.isNull()) {
        for (uint i = 0; i < notificationCount; ++i) {
            NOTIFICATION &notification(notifications[i]);

            if (notification.ulEventType == fnevNewMail) {
                // TODO: Do we need to process this, or is handling object-created sufficient?
                //NEWMAIL_NOTIFICATION &newmail(notification.info.newmail);
            } else {
                OBJECT_NOTIFICATION &object(notification.info.obj);

                if (object.ulObjType == MAPI_MESSAGE) {
                    MapiEntryId entryId(object.lpEntryID, object.cbEntryID);

                    if (!entryId.isEmpty()) {
                        // Create a partial ID from the information we have (a message can be
                        // retrieved using only the store key and the message entry ID)
#ifdef _WIN32_WCE
                        QMessageId messageId(QMessageIdPrivate::from(_store->entryId(),entryId));
#else
                        QMessageId messageId(QMessageIdPrivate::from(_store->recordKey(), entryId));
#endif

                        MapiSession::NotifyType notifyType;
                        switch (notification.ulEventType)
                        {
                        case fnevObjectCopied: notifyType = MapiSession::Added; break;
                        case fnevObjectCreated: notifyType = MapiSession::Added; break;
                        case fnevObjectDeleted: notifyType = MapiSession::Removed; break;
                        case fnevObjectModified: notifyType = MapiSession::Updated; break;
                        case fnevObjectMoved: notifyType = MapiSession::Updated; break;
                        }

                        // Create an event to be processed by the UI thread
                        QCoreApplication::postEvent(session.data(), new MapiSession::NotifyEvent(_store, messageId, notifyType));
                    } else {
                        qWarning() << "Received notification, but no entry ID";
                    }
                }
            }
        }
    }

    return 0;
}

class SessionManager : public QObject
{
    Q_OBJECT

public:
    SessionManager();
    bool initialize(MapiSession *newSession);

    const MapiSessionPtr &session() const;

private slots:
    void appDestroyed();

private:
    MapiSessionPtr ptr;
};

SessionManager::SessionManager()
{
    connect(QCoreApplication::instance(), SIGNAL(destroyed()), this, SLOT(appDestroyed()));
}

bool SessionManager::initialize(MapiSession *newSession)
{
    if(newSession->_mapiSession)
    {
        ptr = MapiSessionPtr(newSession);
        ptr->_self = ptr;
        return true;
    }
    return false;
}

const MapiSessionPtr &SessionManager::session() const
{
    return ptr;
}

void SessionManager::appDestroyed()
{
    // We need to terminate the MAPI session before main ends
    ptr.clear();
}

Q_GLOBAL_STATIC(SessionManager, manager);

MapiSessionPtr MapiSession::createSession(QMessageManager::Error *error)
{
    static bool initialized(manager()->initialize(new MapiSession(error)));

    MapiSessionPtr ptr(manager()->session());
    if (ptr.isNull()) {
        if (*error == QMessageManager::NoError) {
            *error = QMessageManager::ContentInaccessible;
        }
        qWarning() << "Unable to instantiate session";
    }

    return ptr;
}

MapiSession::MapiSession()
    :_token(0),
     _mapiSession(0),
     _filterId(0),
     _registered(false)
{
}

MapiSession::MapiSession(QMessageManager::Error *error)
    :QObject(),
     _token(WinHelpers::initializeMapi()),
     _mapiSession(0),
     _filterId(0),
     _registered(false)
{
    if (!_token->_initialized) {
        *error = QMessageManager::ContentInaccessible;
    } else {
        // Attempt to start a MAPI session on the default profile
        HRESULT rv = MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &_mapiSession);
        if (HR_FAILED(rv)) {
            qWarning() << "Failed to login to MAPI session - rv:" << hex << (ULONG)rv;
            *error = QMessageManager::ContentInaccessible;
            _mapiSession = 0;
        }

        // This thread must run the message pump for the hidden MAPI window
        // http://blogs.msdn.com/stephen_griffin/archive/2009/05/22/the-fifth-mapi-multithreading-rule.aspx
        dispatchNotifications();
    }
}

MapiSession::~MapiSession()
{
    _storeMap.clear();

    if (_mapiSession) {
        _mapiSession->Logoff(0, 0, 0);
        mapiRelease(_mapiSession);
    }
}

MapiStorePtr MapiSession::findStore(QMessageManager::Error *error, const QMessageAccountId &id, bool cachedMode) const
{
    MapiStorePtr result(0);
    if (!_mapiSession) {
        Q_ASSERT(_mapiSession);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *error = QMessageManager::ContentInaccessible;
        return result;
    }

    const int nCols(3);
    enum { defaultStoreColumn = 0, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_ENTRYID, PR_RECORD_KEY}};

    QueryAllRows qar(mapiMessageStoresTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0, false);

    while(qar.query()) {
        for (uint n = 0; n < qar.rows()->cRows; ++n) {
            SPropValue *props = qar.rows()->aRow[n].lpProps;
#ifdef _WIN32_WCE
            MapiEntryId storeKey(props[entryIdColumn].Value.bin.lpb, props[entryIdColumn].Value.bin.cb);
#else
            MapiRecordKey storeKey(props[recordKeyColumn].Value.bin.lpb, props[recordKeyColumn].Value.bin.cb);
#endif

            if ((!id.isValid() && props[defaultStoreColumn].Value.b) ||  // default store found
                (id.isValid() && (id == QMessageAccountIdPrivate::from(storeKey)))) {    // specified store found
                MapiEntryId entryId(props[entryIdColumn].Value.bin.lpb, props[entryIdColumn].Value.bin.cb);
                result = openStore(error, entryId, cachedMode);
                break;
            }
        }
    }

    *error = qar.error();

    mapiRelease(mapiMessageStoresTable);

    return result;
}

template<typename Predicate, typename Ordering>
QList<MapiStorePtr> MapiSession::filterStores(QMessageManager::Error *error, Predicate predicate, Ordering sortOrder, uint limit, uint offset, bool cachedMode) const
{
    if (!predicate._filter.isSupported()) {
        *error = QMessageManager::ConstraintFailure;
        return QList<MapiStorePtr>();
    }

    QList<MapiStorePtr> result;
    if (!_mapiSession) {
        Q_ASSERT(_mapiSession);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *error = QMessageManager::ContentInaccessible;
    } else {
        SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};

        QueryAllRows qar(mapiMessageStoresTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0, false);

        while (qar.query()) {
            for (uint n = 0; n < qar.rows()->cRows; ++n) {
                MapiEntryId entryId(qar.rows()->aRow[n].lpProps[0].Value.bin.lpb, qar.rows()->aRow[n].lpProps[0].Value.bin.cb);
                MapiStorePtr store(openStore(error, entryId, cachedMode));
                if (!store.isNull()) {
#ifndef _WIN32_WCE
                    // We only want stores that contain private messages
                    if (store->supports(STORE_PUBLIC_FOLDERS)) {
                        continue;
                    }
#endif
                    if (predicate(store)) {
                        result.append(store);
                    }
                }
            }
        }

        *error = qar.error();

        mapiRelease(mapiMessageStoresTable);
    }

    if (!sortOrder.isEmpty()) {
        QList<StoreSortHelper> accountList;
        foreach (MapiStorePtr storePtr, result) {
            accountList.append(StoreSortHelper(&sortOrder, storePtr));
        }
        qSort(accountList.begin(), accountList.end());
        result.clear();
        foreach (StoreSortHelper alt, accountList) {
            result.append(alt.store());
        }
        accountList.clear();
    }

    // See note in filerMessages explaining why IMAPITable::SeekRow can't be used to skip unwanted items
    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

QList<MapiStorePtr> MapiSession::filterStores(QMessageManager::Error *error, const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset, bool cachedMode) const
{
    AccountFilterPredicate pred(filter);
    return filterStores<const AccountFilterPredicate&, const QMessageAccountSortOrder &>(error, pred, sortOrder, limit, offset, cachedMode);
}

QList<MapiStorePtr> MapiSession::allStores(QMessageManager::Error *error, bool cachedMode) const
{
    return filterStores(error, QMessageAccountFilter(), QMessageAccountSortOrder(), 0, 0, cachedMode);
}

QList<MapiFolderPtr> MapiSession::filterFolders(QMessageManager::Error *error, const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset, bool cachedMode) const
{
    if (!filter.isSupported()) {
        *error = QMessageManager::ConstraintFailure;
        return QList<MapiFolderPtr>();
    }

    QList<MapiFolderPtr> result;
    if (!_mapiSession) {
        Q_ASSERT(_mapiSession);
        *error = QMessageManager::FrameworkFault;
        return result;
    }

    foreach (const MapiStorePtr &store, allStores(error, cachedMode)) {
        result.append(store->filterFolders(error, filter));
    }

    if (!sortOrder.isEmpty()) {
        QList<FolderSortHelper> folderList;
        foreach (MapiFolderPtr folderPtr, result) {
            QMessageFolder orderFolder(folder(error, folderPtr->id()));
            if (*error != QMessageManager::NoError) {
                folderList.clear();
                break;
            }
            folderList.append(FolderSortHelper(&sortOrder, folderPtr, orderFolder));
        }
        qSort(folderList.begin(), folderList.end());
        result.clear();
        foreach (FolderSortHelper helper, folderList) {
            result.append(helper.mapiFolderPtr());
        }
        folderList.clear();
    }

    // See note in filerMessages explaining why IMAPITable::SeekRow can't be used to skip unwanted items
    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

IMsgStore *MapiSession::openMapiStore(QMessageManager::Error *error, const MapiEntryId &entryId, bool cachedMode) const
{
    IMsgStore *store(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    unsigned long openFlags = MAPI_BEST_ACCESS | MDB_WRITE;

    if(!cachedMode)
        openFlags |= MDB_ONLINE;

    HRESULT rv = _mapiSession->OpenMsgStore(0, entryId.count(), entryIdPtr, 0, openFlags, reinterpret_cast<LPMDB*>(&store));
    if (HR_FAILED(rv)) {
        *error = QMessageManager::InvalidId;
        qWarning() << "Invalid store entryId:" << entryId.toBase64();
    }

    return store;
}

MapiStorePtr MapiSession::openStore(QMessageManager::Error *error, const MapiEntryId& entryId, bool cachedMode) const
{
    MapiStorePtr result(0);

    // See if we can create a new pointer to an existing store
    MapiStorePtr &existing = _storeMap[entryId];
    if (!existing.isNull()) {
        return existing;
    }

    // We need to create a new instance
    IMsgStore *store = openMapiStore(error, entryId, cachedMode);
    if (store && (*error == QMessageManager::NoError)) {
        // Find the other properties of this store
        SizedSPropTagArray(2, columns) = {2, {PR_DISPLAY_NAME, PR_RECORD_KEY}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = store->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);

            MapiStorePtr storePtr = MapiStore::createStore(error, _self.toStrongRef(), store, recordKey, entryId, name, cachedMode);
            if (*error == QMessageManager::NoError) {
                result = storePtr;

                // Add to the map
                _storeMap.insert(entryId, result);
            } else {
                qWarning() << "Error creating store object";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to access store properties";
            mapiRelease(store);
        }
    }

    return result;
}

MapiStorePtr MapiSession::openStoreWithKey(QMessageManager::Error *error, const MapiRecordKey &recordKey, bool cachedMode) const
{
    MapiStorePtr result;

    IMAPITable *storesTable(0);
    HRESULT rv = _mapiSession->GetMsgStoresTable(0, &storesTable);
    if (HR_SUCCEEDED(rv)) {
        // Find the entry ID corresponding to this recordKey
        SPropValue keyProp = { 0 };
        keyProp.ulPropTag = PR_RECORD_KEY;
        keyProp.Value.bin.cb = recordKey.count();
        keyProp.Value.bin.lpb = reinterpret_cast<LPBYTE>(const_cast<char*>(recordKey.data()));

        SRestriction restriction = { 0 };
        restriction.rt = RES_PROPERTY;
        restriction.res.resProperty.relop = RELOP_EQ;
        restriction.res.resProperty.ulPropTag = PR_RECORD_KEY;
        restriction.res.resProperty.lpProp = &keyProp;

        ULONG flags(0);
        rv = storesTable->Restrict(&restriction, flags);
        if (HR_SUCCEEDED(rv)) {
            SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};
            rv = storesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
            if (HR_SUCCEEDED(rv)) {
                SRowSet *rows(0);
                if (storesTable->QueryRows(1, 0, &rows) == S_OK) {
                    if (rows->cRows == 1) {
                        MapiEntryId entryId(rows->aRow[0].lpProps[0].Value.bin.lpb, rows->aRow[0].lpProps[0].Value.bin.cb);

                        result = openStore(error, entryId, cachedMode);
                    } else {
                        *error = QMessageManager::InvalidId;
                    }
                    FreeProws(rows);
                } else {
                    *error = QMessageManager::InvalidId;
                }
            } else {
                *error = QMessageManager::ContentInaccessible;
                qWarning() << "Unable to set columns for stores table";
            }
        } else {
            *error = QMessageManager::ContentInaccessible;
            qWarning() << "Unable to restrict stores table";
        }

        mapiRelease(storesTable);
    } else {
        *error = QMessageManager::ContentInaccessible;
        qWarning() << "Unable to open stores table.";
    }

    return result;
}

QMessageAccountId MapiSession::defaultAccountId(QMessageManager::Error *error, QMessage::Type type) const
{
#ifdef _WIN32_WCE
    if (type == QMessage::Sms) {
        foreach (MapiStorePtr store, allStores(error)) {
            if (store->name() == "SMS") {
                return store->id();
            }
        }
    }

#endif
    if (type == QMessage::Email) {
        // Return the account of the default store
        MapiStorePtr store(defaultStore(error));
        if (*error == QMessageManager::NoError) {
            return store->id();
        }
    }

    return QMessageAccountId();
}

IMessage *MapiSession::openMapiMessage(QMessageManager::Error *error, const QMessageId &id, MapiStorePtr *storePtr) const
{
    IMessage *message(0);

#ifdef _WIN32_WCE
    MapiEntryId storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
#else
    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
#endif

    MapiStorePtr mapiStore(findStore(error, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*error == QMessageManager::NoError) {
        MapiEntryId entryId(QMessageIdPrivate::entryId(id));

        message = mapiStore->openMessage(error, entryId);
        if (*error == QMessageManager::NoError) {
            if (storePtr) {
                *storePtr = mapiStore;
            }
        } else {
            qWarning() << "Invalid message entryId:" << entryId.toBase64();
            mapiRelease(message);
        }
    } else {
        qWarning() << "Invalid store recordKey:" << storeRecordKey.toBase64();
    }

    return message;
}

MapiEntryId MapiSession::messageEntryId(QMessageManager::Error *error, const MapiRecordKey &storeKey, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey)
{
    MapiEntryId result;

    MapiStorePtr store(openStoreWithKey(error, storeKey));
    if (*error == QMessageManager::NoError) {
        result = store->messageEntryId(error, folderKey, messageKey);
    }

    return result;
}

MapiRecordKey MapiSession::messageRecordKey(QMessageManager::Error *error, const QMessageId &id)
{
    MapiRecordKey result;

    IMessage *message = openMapiMessage(error, id);
    if (*error == QMessageManager::NoError) {
        if (!getMapiProperty(message, PR_RECORD_KEY, &result)) {
            qWarning() << "Unable to query message record key.";
        }

        mapiRelease(message);
    }

    return result;
}

MapiRecordKey MapiSession::folderRecordKey(QMessageManager::Error *error, const QMessageId &id)
{
    MapiRecordKey result;

    MapiStorePtr store;
    IMessage *message = openMapiMessage(error, id, &store);
    if (*error == QMessageManager::NoError) {
        // Find the parent folder for the message
        MapiEntryId folderId;
        if (getMapiProperty(message, PR_PARENT_ENTRYID, &folderId)) {
            MapiFolderPtr folder = store->openFolder(error, folderId);
            if (*error == QMessageManager::NoError) {
                result = folder->recordKey();
            }
        } else {
            qWarning() << "Unable to query folder entry ID from message.";
        }

        mapiRelease(message);
    }

    return result;
}

#ifdef _WIN32_WCE

MapiEntryId MapiSession::folderEntryId(QMessageManager::Error *error, const QMessageId &id)
{
    MapiEntryId result;

    IMessage *message = openMapiMessage(error, id);
    if (*error == QMessageManager::NoError) {
        // Find the parent folder for the message
        MapiEntryId folderId;
        if (getMapiProperty(message, PR_PARENT_ENTRYID, &folderId)) {
            result = folderId;
        } else {
            qWarning() << "Unable to query folder entry ID from message.";
        }

        mapiRelease(message);
    }

    return result;
}

#endif

bool MapiSession::equal(const MapiEntryId &lhs, const MapiEntryId &rhs) const
{
    ULONG result(0);

    LPENTRYID lhsEntryId(reinterpret_cast<LPENTRYID>(const_cast<char*>(lhs.data())));
    LPENTRYID rhsEntryId(reinterpret_cast<LPENTRYID>(const_cast<char*>(rhs.data())));

    HRESULT rv = _mapiSession->CompareEntryIDs(lhs.count(), lhsEntryId, rhs.count(), rhsEntryId, 0, &result);
    if (HR_FAILED(rv)) {
        qWarning() << "Unable to compare entry IDs.";
    }

    return (result != FALSE);
}

QMessageFolder MapiSession::folder(QMessageManager::Error *error, const QMessageFolderId& id) const
{
    QMessageFolder result;

    if(!id.isValid())
    {
        *error = QMessageManager::InvalidId;
        return result;
    }

#ifdef _WIN32_WCE
    MapiEntryId storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
#else
    MapiRecordKey storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
#endif
    MapiStorePtr mapiStore(findStore(error, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*error != QMessageManager::NoError)
        return result;

    // Find the root folder for this store
    MapiFolderPtr storeRoot(mapiStore->rootFolder(error));
    if (*error != QMessageManager::NoError)
        return result;

    MapiEntryId entryId(QMessageFolderIdPrivate::entryId(id));
    MapiFolderPtr folder = mapiStore->openFolder(error, entryId);
    if (folder && (*error == QMessageManager::NoError)) {
#ifndef _WIN32_WCE
        SizedSPropTagArray(3, columns) = {3, {PR_DISPLAY_NAME, PR_PARENT_ENTRYID, PR_RECORD_KEY}};
#else
        SizedSPropTagArray(2, columns) = {2, {PR_DISPLAY_NAME, PR_PARENT_ENTRYID}};
#endif
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiEntryId parentEntryId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
#ifndef _WIN32_WCE
            MapiRecordKey folderKey(properties[2].Value.bin.lpb, properties[2].Value.bin.cb);
#else
            MapiRecordKey folderKey;
#endif

            MAPIFreeBuffer(properties);

            QMessageFolderId folderId(QMessageFolderIdPrivate::from(folderKey, storeRecordKey, entryId));

            if (equal(parentEntryId, storeRoot->entryId())) {
                // This folder is a direct child of the root folder
                QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));
                return QMessageFolderPrivate::from(folderId, accountId, QMessageFolderId(), name, name);
            }

            QStringList path;
            path.append(name);

            QMessageFolderId parentId;
            MapiEntryId ancestorEntryId(parentEntryId);
            MapiFolderPtr ancestorFolder;

            // Iterate through ancestors towards the root
            while ((ancestorFolder = mapiStore->openFolder(error, ancestorEntryId)) &&
                   (ancestorFolder && (*error == QMessageManager::NoError))) {
                SPropValue *ancestorProperties(0);
                if (ancestorFolder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &ancestorProperties) == S_OK) {
#ifndef _WIN32_WCE
                    SPropValue &ancestorRecordKeyProp(ancestorProperties[2]);
                    MapiRecordKey ancestorRecordKey(ancestorRecordKeyProp.Value.bin.lpb, ancestorRecordKeyProp.Value.bin.cb);
                    bool reachedRoot(ancestorRecordKey == storeRoot->recordKey());
#else
                    MapiRecordKey ancestorRecordKey;
                    bool reachedRoot(equal(ancestorEntryId, storeRoot->entryId()));
#endif

                    if (ancestorEntryId == parentEntryId) {
                        // This ancestor is the parent of the folder being retrieved, create a QMessageFolderId for the parent
                        parentId = QMessageFolderIdPrivate::from(ancestorRecordKey, storeRecordKey, parentEntryId);
                    }

                    SPropValue &entryIdProp(ancestorProperties[1]);
                    ancestorEntryId = MapiEntryId(entryIdProp.Value.bin.lpb, entryIdProp.Value.bin.cb);

                    QString ancestorName(QStringFromLpctstr(ancestorProperties[0].Value.LPSZ));

                    MAPIFreeBuffer(ancestorProperties);

                    if (reachedRoot) {
                        // Reached the root and have a complete path for the folder being retrieved
                        QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));
                        return QMessageFolderPrivate::from(folderId, accountId, parentId, name, path.join("/"));
                    }

                    // Prepare to consider next ancestor
                    if (!ancestorName.isEmpty())
                        path.prepend(ancestorName);
                } else {
                    break;
                }
            }
        }
    }

    // Failed to quickly retrieve the folder, fallback to an exhaustive search of all folders
    result = mapiStore->folderFromId(error, id);
    return result;
}

QMessage MapiSession::message(QMessageManager::Error *error, const QMessageId& id) const
{
    Q_UNUSED(error);

    QMessage result = QMessagePrivate::from(id);

    // How do we find what type of message this is?
    result.setType(QMessage::Email);

    result.d_ptr->_elementsPresent.properties = 0;
    result.d_ptr->_elementsPresent.recipients = 0;
    result.d_ptr->_elementsPresent.body = 0;
    result.d_ptr->_elementsPresent.attachments = 0;

    result.d_ptr->_modified = false;

    return result;
}

bool MapiSession::updateMessageProperties(QMessageManager::Error *error, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.properties) {
        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.properties = 1;

        MapiStorePtr store;
        IMessage *message = openMapiMessage(error, msg->id(), &store);
        if (*error == QMessageManager::NoError) {
#ifndef _WIN32_WCE
            const int np = 14;
#else
            const int np = 12;
#endif
            SizedSPropTagArray(np, msgCols) = {np, { PR_PARENT_ENTRYID,
                                                     PR_MESSAGE_FLAGS,
                                                     PR_MSG_STATUS,
                                                     PR_MESSAGE_CLASS,
                                                     PR_SENDER_NAME,
                                                     PR_SENDER_EMAIL_ADDRESS,
                                                     PR_CLIENT_SUBMIT_TIME,
                                                     PR_MESSAGE_DELIVERY_TIME,
                                                     PR_SUBJECT,
                                                     PR_HASATTACH,
                                                     PR_PRIORITY,
#ifndef _WIN32_WCE
                                                     PR_MSG_EDITOR_FORMAT,
                                                     PR_RTF_IN_SYNC,
#endif
                                                     PR_MESSAGE_SIZE
                                                     }};
            ULONG count = 0;
            LPSPropValue properties;
            HRESULT rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&msgCols), MAPI_UNICODE, &count, &properties);
            if (HR_SUCCEEDED(rv)) {
                QString senderName;
                QString senderAddress;
                QString messageClass;
                MapiEntryId parentEntryId;

                QMessage::StatusFlags flags(0);

                //assumption that stores support only a single message type
                msg->d_ptr->_type = store->types() & QMessage::Sms ? QMessage::Sms : QMessage::Email;

                for (ULONG n = 0; n < count; ++n) {
                    SPropValue &prop(properties[n]);

                    switch (prop.ulPropTag) {
                    case PR_MESSAGE_FLAGS:
                        if (prop.Value.ul & MSGFLAG_READ) {
                            flags |= QMessage::Read;
                        }
                        break;
                    case PR_PARENT_ENTRYID:
                        parentEntryId = MapiEntryId(prop.Value.bin.lpb, prop.Value.bin.cb);
                        break;
                    case PR_MSG_STATUS:
                        if (prop.Value.l & (MSGSTATUS_DELMARKED | MSGSTATUS_REMOTE_DELETE)) {
                            flags |= QMessage::Removed;
                        }
#ifdef _WIN32_WCE
                        if (prop.Value.l & MSGSTATUS_HAS_PR_BODY) {
                            msg->d_ptr->_contentFormat = EDITOR_FORMAT_PLAINTEXT;
#if(_WIN32_WCE > 0x501)
                        } else if (prop.Value.l & MSGSTATUS_HAS_PR_BODY_HTML) {
                            msg->d_ptr->_contentFormat = EDITOR_FORMAT_HTML;
#endif
                        } else if (prop.Value.l & MSGSTATUS_HAS_PR_CE_MIME_TEXT) {
                            // This is how MS providers store HTML, as per http://msdn.microsoft.com/en-us/library/bb446140.aspx
                            msg->d_ptr->_contentFormat = EDITOR_FORMAT_MIME;
                        }
#endif
                        break;
                    case PR_MESSAGE_CLASS:
                        messageClass = QStringFromLpctstr(prop.Value.LPSZ);
                        break;
                    case PR_SENDER_NAME:
                        senderName = QStringFromLpctstr(prop.Value.LPSZ);
                        break;
                    case PR_SENDER_EMAIL_ADDRESS:
                        senderAddress = QStringFromLpctstr(prop.Value.LPSZ);
                        break;
                    case PR_CLIENT_SUBMIT_TIME:
                        msg->setDate(fromFileTime(prop.Value.ft));
                        break;
                    case PR_MESSAGE_DELIVERY_TIME:
                        msg->setReceivedDate(fromFileTime(prop.Value.ft));
                        break;
                    case PR_SUBJECT:
                        msg->setSubject(QStringFromLpctstr(prop.Value.LPSZ));
                        break;
                    case PR_HASATTACH:
                        msg->d_ptr->_hasAttachments = (prop.Value.b != FALSE);
                        if (prop.Value.b) {
                            flags |= QMessage::HasAttachments;
                        }
                        break;
                    case PR_PRIORITY:
                        if (prop.Value.l == PRIO_URGENT) {
                            msg->setPriority(QMessage::HighPriority);
                        } else if (prop.Value.l == PRIO_NONURGENT) {
                            msg->setPriority(QMessage::LowPriority);
                        } else {
                            msg->setPriority(QMessage::NormalPriority);
                        }
                        break;
#ifndef _WIN32_WCE
                    case PR_MSG_EDITOR_FORMAT:
                        msg->d_ptr->_contentFormat = prop.Value.l;
                        break;
                    case PR_RTF_IN_SYNC:
                        msg->d_ptr->_rtfInSync = (prop.Value.b != FALSE);;
                        break;
#endif
                    case PR_MESSAGE_SIZE:
                        QMessagePrivate::setSize(*msg, prop.Value.l);
                        break;
                    default:
                        break;
                    }
                }

                msg->setStatus(flags);
                msg->setParentAccountId(store->id());

                if (!senderName.isEmpty() || !senderAddress.isEmpty()) {
                    msg->setFrom(createAddress(senderName, senderAddress));
                    QMessagePrivate::setSenderName(*msg, senderName);
                }

                if (!parentEntryId.isEmpty()) {
                    QMessagePrivate::setStandardFolder(*msg, store->standardFolder(parentEntryId));

                    // MAPI does not support the notion of incoming/outgoing messages.  Instead, we
                    // will treat Outgoing as meaning 'in the Sent folder'
                    msg->setStatus(QMessage::Incoming, (msg->standardFolder() != QMessage::SentFolder));

                    MapiFolderPtr parentFolder(store->openFolder(error, parentEntryId));
                    if (parentFolder) {
                        QMessagePrivate::setParentFolderId(*msg, parentFolder->id());
                    }
                }

                if (!isModified) {
                    msg->d_ptr->_modified = false;
                }
                result = true;

                MAPIFreeBuffer(properties);
            } else {
                *error = QMessageManager::ContentInaccessible;
            }

            mapiRelease(message);
        }
    }

    return result;
}

bool MapiSession::updateMessageRecipients(QMessageManager::Error *error, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.recipients) {
        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.recipients = 1;

        IMessage *message = openMapiMessage(error, msg->id());
        if (*error == QMessageManager::NoError) {
            // Extract the recipients for the message
            IMAPITable *recipientsTable(0);
            HRESULT rv = message->GetRecipientTable(0, &recipientsTable);
            if (HR_SUCCEEDED(rv)) {
                QMessageAddressList to;
                QMessageAddressList cc;
                QMessageAddressList bcc;

#ifndef _WIN32_WCE
                SizedSPropTagArray(3, rcpCols) = {3, { PR_DISPLAY_NAME, PR_EMAIL_ADDRESS, PR_RECIPIENT_TYPE}};

                QueryAllRows qar(recipientsTable, reinterpret_cast<LPSPropTagArray>(&rcpCols), 0, 0);
#else
                // CE does not support SetColumns on recipient tables
                QueryAllRows qar(recipientsTable, 0, 0, 0, false);
#endif

                while(qar.query()) {
                    for (uint n = 0; n < qar.rows()->cRows; ++n) {
                        QString name;
                        QString address;
                        LONG type(0);

                        SPropValue *props = qar.rows()->aRow[n].lpProps;
#ifndef _WIN32_WCE
                        if (props[0].ulPropTag == PR_DISPLAY_NAME)
                            name = QStringFromLpctstr(props[0].Value.LPSZ);
                        if (props[1].ulPropTag == PR_EMAIL_ADDRESS)
                            address = QStringFromLpctstr(props[1].Value.LPSZ);
                        if (props[2].ulPropTag == PR_RECIPIENT_TYPE)
                            type = props[2].Value.l;
#else
                        for (uint i = 0; i < qar.rows()->aRow[n].cValues; ++i) {
                            if (props[i].ulPropTag == PR_DISPLAY_NAME) {
                                name = QStringFromLpctstr(props[i].Value.LPSZ);
                            } else if (props[i].ulPropTag == PR_EMAIL_ADDRESS) {
                                address = QStringFromLpctstr(props[i].Value.LPSZ);
                            } else if (props[i].ulPropTag == PR_RECIPIENT_TYPE){
                                type = props[i].Value.l;
                            }
                        }
#endif

                        if (!name.isEmpty() || !address.isEmpty()) {
                            QMessageAddressList *list = 0;

                            switch (type) {
                            case MAPI_TO:
                                list = &to;
                                break;
                            case MAPI_CC:
                                list = &cc;
                                break;
                            case MAPI_BCC:
                                list = &bcc;
                                break;
                            default:
                                break;
                            }

                            if (list) {
                                list->append(createAddress(name, address));
                            }
                        }
                    }
                }

                if (!to.isEmpty()) {
                    msg->setTo(to);
                }
                if (!cc.isEmpty()) {
                    msg->setCc(cc);
                }
                if (!bcc.isEmpty()) {
                    msg->setBcc(bcc);
                }

                if (!isModified) {
                    msg->d_ptr->_modified = false;
                }

                if (qar.error() != QMessageManager::NoError) {
                    *error = qar.error();
                    result = false;
                } else {
                    result = true;
                }

                mapiRelease(recipientsTable);
            } else {
#ifdef _WIN32_WCE
                if (rv == MAPI_E_NO_RECIPIENTS) {
                    updateMessageProperties(error, msg);
                    if (*error == QMessageManager::NoError) {
                        if (msg->d_ptr->_contentFormat == EDITOR_FORMAT_MIME) {
                            // The recipient info can be extracted from the MIME body
                            updateMessageBody(error, msg);
                        }
                    }
                } else {
#endif
                    *error = QMessageManager::ContentInaccessible;
#ifdef _WIN32_WCE
                }
#endif
            }

            mapiRelease(message);
        }
    }

    return result;
}

bool MapiSession::updateMessageBody(QMessageManager::Error *error, QMessage *msg) const
{
    bool result(false);

    // TODO: Signed messages are stored with the body as an attachment; we need to implement this

    if (!msg->d_ptr->_elementsPresent.body) {
        if (!msg->d_ptr->_elementsPresent.properties) {
            // We need the properties before we can fetch the body
            if (!updateMessageProperties(error, msg)) {
                return false;
            }
        }

        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.body = 1;

        QByteArray messageBody;
        QByteArray bodySubType;

        MapiStorePtr store;
        IMessage *message = openMapiMessage(error, msg->id(), &store);
        if (*error == QMessageManager::NoError) {
            //SMS body stored in subject on CEMAPI
            if (store->types() & QMessage::Sms) {
                messageBody.append(reinterpret_cast<const char*>(msg->subject().utf16()),msg->subject().count()*sizeof(quint16));
                bodySubType = "plain";
            } else {
                IStream *is(0);
                bool asciiData(false);
                LONG contentFormat(msg->d_ptr->_contentFormat);

                if (contentFormat == EDITOR_FORMAT_DONTKNOW) {
                    // Attempt to read HTML first
                    contentFormat = EDITOR_FORMAT_HTML;
                }
                if (contentFormat == EDITOR_FORMAT_PLAINTEXT) {
#ifdef _WIN32_WCE
                    ULONG tags[] = { PR_BODY, PR_BODY_W, PR_BODY_A };
#else
                    ULONG tags[] = { PR_BODY };
#endif
                    const int n = sizeof(tags)/sizeof(tags[0]);
                    for (int i = 0; i < n; ++i) {
                        HRESULT rv = message->OpenProperty(tags[i], &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                        if (HR_SUCCEEDED(rv)) {
                            messageBody = readStream(error, is);
                            bodySubType = "plain";
                            if (i == 2) {
                                asciiData = true;
                            }
                            break;
                        } 
                    }
                    if (messageBody.isEmpty()) {
                        qWarning() << "Unable to open PR_BODY!";
                    }
                } else if (contentFormat == EDITOR_FORMAT_HTML) {
                    // See if there is a body HTML property
                    // Correct variants discussed at http://blogs.msdn.com/raffael/archive/2008/09/08/mapi-on-windows-mobile-6-programmatically-retrieve-mail-body-sample-code.aspx
#if(_WIN32_WCE > 0x501)
                    ULONG tags[] = { PR_BODY_HTML, PR_BODY_HTML_W, PR_BODY_HTML_A };
#else
                    ULONG tags[] = { PR_BODY_HTML };
#endif
                    const int n = sizeof(tags)/sizeof(tags[0]);
                    for (int i = 0; i < n; ++i) {
                        HRESULT rv = message->OpenProperty(tags[i], &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                        if (HR_SUCCEEDED(rv)) {
                            messageBody = readStream(error, is);
                            bodySubType = "html";
                            if (i == 2) {
                                asciiData = true;
                            }
                            break;
                        }
                    }
                    if (messageBody.isEmpty()) {
#ifdef _WIN32_WCE
                        qWarning() << "Unable to open PR_BODY_HTML!";
#else
                        // We couldn't get HTML; try RTF
                        contentFormat = EDITOR_FORMAT_DONTKNOW;
#endif
                    }
#ifdef _WIN32_WCE
                } else if (contentFormat == EDITOR_FORMAT_MIME) {
                    // MIME format is only used on mobile
                    HRESULT rv = message->OpenProperty(PR_CE_MIME_TEXT, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                    if (HR_SUCCEEDED(rv)) {
                        messageBody = readStream(error, is);

                        QMailMessage mimeMsg(QMailMessage::fromRfc2822(messageBody));

                        // Extract the recipient info from the message headers
                        QMessageAddressList addresses;
                        foreach (const QMailAddress &addr, mimeMsg.to()) {
                            QString addressString(addr.address());
                            if(!addressString.isEmpty())
                                addresses.append(QMessageAddress(QMessageAddress::Email, addressString));
                        }
                        if (!addresses.isEmpty()) {
                            msg->setTo(addresses);
                        }

                        addresses.clear();
                        foreach (const QMailAddress &addr, mimeMsg.cc()) {
                            QString addressString(addr.address());
                            if(!addressString.isEmpty())
                                addresses.append(QMessageAddress(QMessageAddress::Email, addressString));
                        }
                        if (!addresses.isEmpty()) {
                            msg->setCc(addresses);
                        }

                        addresses.clear();
                        foreach (const QMailAddress &addr, mimeMsg.bcc()) {
                            QString addressString(addr.address());
                            addresses.append(QMessageAddress(QMessageAddress::Email, addressString));
                        }
                        if (!addresses.isEmpty()) {
                            msg->setBcc(addresses);
                        }

                        // Extract the text of the message body
                        if (mimeMsg.multipartType() == QMailMessage::MultipartNone) {
                            if (mimeMsg.contentType().type().toLower() == "text") {
                                QByteArray subType(mimeMsg.contentType().subType().toLower());
                                if ((subType == "plain") || (subType == "html") || (subType == "rtf")) {
                                    messageBody = QTextCodec::codecForName("utf-16")->fromUnicode(mimeMsg.body().data());
                                    bodySubType = subType;
                                }
                            }
                        } else if ((mimeMsg.multipartType() == QMailMessage::MultipartAlternative) ||
                                   (mimeMsg.multipartType() == QMailMessage::MultipartMixed) ||
                                   (mimeMsg.multipartType() == QMailMessage::MultipartRelated)) {
                            // For multipart/related, just try the first part
                            int maxParts(mimeMsg.multipartType() == QMailMessage::MultipartRelated ? 1 : mimeMsg.partCount());
                            for (int i = 0; i < maxParts; ++i) {
                                const QMailMessagePart &part(mimeMsg.partAt(i));

                                if (part.contentType().type().toLower() == "text") {
                                    QByteArray subType(part.contentType().subType().toLower());
                                    if ((subType == "plain") || (subType == "html") || (subType == "rtf")) {
                                        messageBody = QTextCodec::codecForName("utf-16")->fromUnicode(part.body().data());
                                        bodySubType = subType;
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        qWarning() << "Unable to open PR_CE_MIME_TEXT!";
                    }
#endif
                }

#ifndef _WIN32_WCE // RTF not supported on mobile
                if (bodySubType.isEmpty()) {
                    if (!msg->d_ptr->_rtfInSync) {
                        // See if we need to sync the RTF
                        if (!store->supports(STORE_RTF_OK)) {
                            BOOL updated(FALSE);
                            HRESULT rv = RTFSync(message, RTF_SYNC_BODY_CHANGED, &updated);
                            if (HR_SUCCEEDED(rv)) {
                                if (updated) {
                                    if (HR_FAILED(message->SaveChanges(0))) {
                                        qWarning() << "Unable to save changes after synchronizing RTF.";
                                    }
                                }
                            } else {
                                qWarning() << "Unable to synchronize RTF.";
                            }
                        }
                    }

                    // Either the body is in RTF, or we need to read the RTF to know that it is text...
                    HRESULT rv = message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                    if (HR_SUCCEEDED(rv)) {
                        IStream *decompressor(0);
                        if (WrapCompressedRTFStream(is, 0, &decompressor) == S_OK) {
                            ULONG bytes = 0;
                            char buffer[BUFSIZ] = { 0 };
                            do {
                                decompressor->Read(buffer, BUFSIZ, &bytes);
                                messageBody.append(buffer, bytes);
                            } while (bytes == BUFSIZ);

                            decompressor->Release();

                            // RTF is stored in ASCII
                            asciiData = true;

                            if (contentFormat == EDITOR_FORMAT_DONTKNOW) {
                                // Inspect the message content to see if we can tell what is in it
                                if (!messageBody.isEmpty()) {
                                    QByteArray initialText(messageBody.left(256));
                                    if (initialText.indexOf("\\fromtext") != -1) {
                                        // This message originally contained text
                                        contentFormat = EDITOR_FORMAT_PLAINTEXT;

                                        // See if we can get the plain text version instead
                                        IStream *ts(0);
                                        rv = message->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, 0, (IUnknown**)&ts);
                                        if (HR_SUCCEEDED(rv)) {
                                            messageBody = readStream(error, ts);
                                            bodySubType = "plain";
                                            asciiData = false;

                                            ts->Release();
                                        } else {
                                            qWarning() << "Unable to prefer plain text body.";
                                        }
                                    } else if (initialText.indexOf("\\fromhtml1") != -1) {
                                        // This message originally contained text
                                        contentFormat = EDITOR_FORMAT_HTML;
                                    }
                                }
                            }

                            if (bodySubType.isEmpty()) {
                                if (contentFormat == EDITOR_FORMAT_PLAINTEXT) {
                                    messageBody = extractPlainText(messageBody);
                                    bodySubType = "plain";
                                } else if (contentFormat == EDITOR_FORMAT_HTML) {
                                    messageBody = extractHtml(messageBody);
                                    bodySubType = "html";
                                } else {
                                    // I guess we must just have RTF
                                    bodySubType = "rtf";
                                }
                            }
                        } else {
                            *error = QMessageManager::ContentInaccessible;
                            qWarning() << "Unable to decompress RTF";
                            bodySubType = "plain";
                        }
                    }
                }
#endif
                if (bodySubType.isEmpty()) {
                    qWarning() << "Unable to locate body for message.";
                }

                mapiRelease(is);

                if (asciiData) {
                    // Convert the ASCII content back to UTF-16
                    messageBody = QTextCodec::codecForName("utf-16")->fromUnicode(decodeContent(messageBody, "Latin-1"));
                }
            }

            if (*error == QMessageManager::NoError) {
                QMessageContentContainerPrivate *messageContainer(((QMessageContentContainer *)(msg))->d_ptr);

                bool bodyDownloaded = true;
#ifdef _WIN32_WCE
                ULONG status = 0;
                if(getMapiProperty(message,PR_MSG_STATUS,&status)) {
                    bodyDownloaded = !((status & MSGSTATUS_HEADERONLY) || (status & MSGSTATUS_PARTIAL));
                }
#else
                //TODO windows
#endif

                if (!msg->d_ptr->_hasAttachments) {
                    // Make the body the entire content of the message
                    messageContainer->setContent(messageBody, QByteArray("text"), bodySubType, QByteArray("utf-16"));
                    msg->d_ptr->_bodyId = messageContainer->bodyContentId();
                    messageContainer->_available = bodyDownloaded;
                } else {
                    // Add the message body data as the first part
                    QMessageContentContainer bodyPart;
                    {
                        QMessageContentContainerPrivate *bodyContainer(((QMessageContentContainer *)(&bodyPart))->d_ptr);
                        bodyContainer->setContent(messageBody, QByteArray("text"), bodySubType, QByteArray("utf-16"));
                        bodyContainer->_available = bodyDownloaded;
                    }

                    messageContainer->setContentType(QByteArray("multipart"), QByteArray("mixed"), QByteArray());
                    msg->d_ptr->_bodyId = messageContainer->appendContent(bodyPart);
                }

                if (!isModified) {
                    msg->d_ptr->_modified = false;
                }
                result = true;
            }

            mapiRelease(message);
        }
    }

    return result;
}

bool MapiSession::updateMessageAttachments(QMessageManager::Error *error, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.attachments) {
        if (!msg->d_ptr->_elementsPresent.properties) {
            // We need the properties before we can fetch the attachments
            if (!updateMessageProperties(error, msg)) {
                return false;
            }
        }

        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.attachments = 1;

        if (msg->d_ptr->_hasAttachments) {
            IMessage *message = openMapiMessage(error, msg->id());
            if (*error == QMessageManager::NoError) {
                QMessageContentContainerPrivate *messageContainer(((QMessageContentContainer *)(msg))->d_ptr);

                // Find any attachments for this message
                IMAPITable *attachmentsTable(0);
                HRESULT rv = message->GetAttachmentTable(0, &attachmentsTable);
                if (HR_SUCCEEDED(rv)) {
                    // Find the properties of these attachments
                    SizedSPropTagArray(7, attCols) = {7, { PR_ATTACH_NUM,
                                                           PR_ATTACH_EXTENSION,
                                                           PR_ATTACH_LONG_FILENAME,
                                                           PR_ATTACH_FILENAME,
                                                           PR_ATTACH_CONTENT_ID,
                                                           PR_ATTACH_SIZE,
                                                           PR_RENDERING_POSITION }};

                    QueryAllRows qar(attachmentsTable, reinterpret_cast<LPSPropTagArray>(&attCols), NULL, NULL);
                    while(qar.query()) {
                        for (uint n = 0; n < qar.rows()->cRows; ++n) {
                            LONG number(0);
                            QString extension;
                            QString filename;
                            QString contentId;
                            LONG size(0);
                            LONG renderingPosition(0);

                            // If not available, the output tag will not match our requested content tag
                            if (qar.rows()->aRow[n].lpProps[0].ulPropTag == PR_ATTACH_NUM) {
                                number = qar.rows()->aRow[n].lpProps[0].Value.l;
                            } else {
                                // We can't access this part...
                                continue;
                            }

                            if (qar.rows()->aRow[n].lpProps[1].ulPropTag == PR_ATTACH_EXTENSION) {
                                extension = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[1].Value.LPSZ);
                            }
                            if (qar.rows()->aRow[n].lpProps[2].ulPropTag == PR_ATTACH_LONG_FILENAME) {
                                filename = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[2].Value.LPSZ);
                            } else if (qar.rows()->aRow[n].lpProps[3].ulPropTag == PR_ATTACH_FILENAME) {
                                filename = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[3].Value.LPSZ);
                            }
                            if (qar.rows()->aRow[n].lpProps[4].ulPropTag == PR_ATTACH_CONTENT_ID) {
                                contentId = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[4].Value.LPSZ);
                            }
                            if (qar.rows()->aRow[n].lpProps[5].ulPropTag == PR_ATTACH_SIZE) {
                                // Increase the size estimate by a third to allow for transfer encoding
                                size = (qar.rows()->aRow[n].lpProps[5].Value.l * 4 / 3);
                            }
                            if (qar.rows()->aRow[n].lpProps[6].ulPropTag == PR_RENDERING_POSITION) {
                                renderingPosition = qar.rows()->aRow[n].lpProps[6].Value.l;
                            }

                            WinHelpers::AttachmentLocator locator(msg->id(), number);

                            QMessageContentContainer attachment(WinHelpers::fromLocator(locator));
                            QMessageContentContainerPrivate *container(((QMessageContentContainer *)(&attachment))->d_ptr);

                            if (!extension.isEmpty()) {
                                QByteArray contentType(contentTypeFromExtension(extension));
                                if (!contentType.isEmpty()) {
                                    int index = contentType.indexOf('/');
                                    if (index != -1) {
                                        container->setContentType(contentType.left(index), contentType.mid(index + 1), QByteArray());
                                    } else {
                                        container->setContentType(contentType, QByteArray(), QByteArray());
                                    }
                                }
                            }
                            if (!contentId.isEmpty()) {
                                container->setHeaderField("Content-ID", contentId.toAscii());
                            }
                            if (renderingPosition == -1) {
                                QByteArray value("attachment");
                                if (!filename.isEmpty()) {
                                    value.append("; filename=" + filename.toAscii());
                                }
                                container->setHeaderField("Content-Disposition", value);
                            }

                            container->_name = filename.toAscii();
                            container->_size = size;
                            container->_available = haveAttachmentData(error,msg->id(),number);

                            messageContainer->appendContent(attachment);
                            if (!isModified) {
                                msg->d_ptr->_modified = false;
                            }
                        }
                    }

                    if (qar.error() != QMessageManager::NoError) {
                        *error = qar.error();
                    } else {
                        result = true;
                    }

                    mapiRelease(attachmentsTable);
                } else {
                    *error = QMessageManager::ContentInaccessible;
                    qWarning() << "Unable to access attachments table.";
                }

                mapiRelease(message);
            }
        }
    }

    return result;
}

bool MapiSession::haveAttachmentData(QMessageManager::Error *error, const QMessageId& id, ULONG number) const
{
    bool result = false;

    IMessage *message = openMapiMessage(error, id);
    if (*error == QMessageManager::NoError) {
        LPATTACH attachment(0);
        HRESULT rv = message->OpenAttach(number, 0, 0, &attachment);
        if (HR_SUCCEEDED(rv)) {
            IStream *is(0);
            rv = attachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
            if (HR_SUCCEEDED(rv)) {
                result = streamSize(error, is) > 0;
                mapiRelease(is);
            }
            mapiRelease(attachment);
        } else {
            qWarning() << "Unable to open attachment:" << number;
        }

        mapiRelease(message);
    }

    return result;
}


QByteArray MapiSession::attachmentData(QMessageManager::Error *error, const QMessageId& id, ULONG number) const
{
    QByteArray result;

    IMessage *message = openMapiMessage(error, id);
    if (*error == QMessageManager::NoError) {
        LPATTACH attachment(0);
        HRESULT rv = message->OpenAttach(number, 0, 0, &attachment);
        if (HR_SUCCEEDED(rv)) {
            IStream *is(0);
            rv = attachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
            if (HR_SUCCEEDED(rv)) {
                result = readStream(error, is);
                mapiRelease(is);
            }

            mapiRelease(attachment);
        } else {
            qWarning() << "Unable to open attachment:" << number;
        }

        mapiRelease(message);
    }

    return result;
}

QMessageIdList MapiSession::queryMessages(QMessageManager::Error *error, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset, const QString &body, QMessageDataComparator::MatchFlags matchFlags) const
{
    if (!filter.isSupported()) {
        *error = QMessageManager::ConstraintFailure;
        return QMessageIdList();
    }

    if (QMessageFilterPrivate::isNonMatching(filter)) { // Avoid unnecessary preprocessing/evaluation
        return QMessageIdList();
    }

    QList<FolderHeapNodePtr> folderNodes;
    QMessageFilter processedFilter(QMessageFilterPrivate::preprocess(error, _self.toStrongRef(), filter));
    if (*error != QMessageManager::NoError)
        return  QMessageIdList();

    if (QMessageFilterPrivate::isNonMatching(processedFilter)) { // Filter maybe be simplified by preprocessing
        return QMessageIdList();
    }

    foreach (QMessageFilter subfilter, QMessageFilterPrivate::subfilters(processedFilter)) {
        MapiStoreIterator storeIt(QMessageFilterPrivate::storeIterator(subfilter, error, _self.toStrongRef()));
        for (MapiStorePtr store(storeIt.next()); store && store->isValid(); store = storeIt.next()) {
            MapiFolderIterator folderIt(QMessageFilterPrivate::folderIterator(subfilter, error, store));
            for (MapiFolderPtr folder(folderIt.next()); folder && folder->isValid(); folder = folderIt.next()) {
                QList<QMessageFilter> orderingFilters;
                orderingFilters.append(subfilter);
                foreach(QMessageFilter orderingFilter, QMessageSortOrderPrivate::normalize(orderingFilters, sortOrder)) {
                    folderNodes.append(FolderHeapNodePtr(new FolderHeapNode(folder, orderingFilter)));
                }
            }
        }
    }

    if (*error != QMessageManager::NoError)
        return QMessageIdList();

    return filterMessages(error, folderNodes, sortOrder, limit, offset, body, matchFlags);
}

void MapiSession::updateMessage(QMessageManager::Error* error, const QMessage& source)
{
    MapiStorePtr store;
    IMessage *mapiMessage = openMapiMessage(error, source.id(), &store);
    if (*error == QMessageManager::NoError) {
        // Update the stored properties
        if (*error == QMessageManager::NoError) {
            storeMessageProperties(error, source, mapiMessage);
        }
        if (*error == QMessageManager::NoError) {
            replaceMessageRecipients(error, source, mapiMessage, _mapiSession);
        }
        if (*error == QMessageManager::NoError) {
            replaceMessageBody(error, source, mapiMessage, store);
        }
        if (*error == QMessageManager::NoError) {
            // Attachments cannot be removed from a QMessage but new ones can be added
            addMessageAttachments(error, source, mapiMessage);
        }
#ifndef _WIN32_WCE //unsupported
        if (*error == QMessageManager::NoError) {
            if (HR_FAILED(mapiMessage->SaveChanges(0))) {
                qWarning() << "Unable to save changes for message.";
            }
        }
#endif

        mapiRelease(mapiMessage);
    }
}

void MapiSession::removeMessages(QMessageManager::Error *error, const QMessageIdList &ids)
{
#ifdef _WIN32_WCE
    typedef QPair<MapiEntryId, MapiEntryId> FolderKey;
#else
    typedef QPair<MapiRecordKey, MapiRecordKey> FolderKey;
#endif

    QMap<FolderKey, QMessageIdList> folderMessageIds;

    // Group messages by folder
    foreach (const QMessageId &id, ids) {
        folderMessageIds[qMakePair(QMessageIdPrivate::storeRecordKey(id), QMessageIdPrivate::folderRecordKey(id))].append(id);
    }

    QMap<FolderKey, QMessageIdList>::const_iterator it = folderMessageIds.begin(), end = folderMessageIds.end();
    for ( ; it != end; ++it) {

#ifdef _WIN32_WCE
        const MapiEntryId storeKey(it.key().first);
        const MapiEntryId folderKey(it.key().second);
#else
        const MapiRecordKey storeKey(it.key().first);
        const MapiRecordKey folderKey(it.key().second);
#endif

        QMessageManager::Error localError(QMessageManager::NoError);

#ifdef _WIN32_WCE
        MapiStorePtr store = openStore(&localError,storeKey,true);
#else
        MapiStorePtr store = openStoreWithKey(&localError, storeKey, true);
#endif
        if (localError == QMessageManager::NoError) {
#ifdef _WIN32_WCE
            MapiFolderPtr folder = store->openFolder(&localError, folderKey);
#else
            MapiFolderPtr folder = store->openFolderWithKey(&localError, folderKey);
#endif
            if (localError == QMessageManager::NoError) {
                folder->removeMessages(&localError, it.value());
            }
        }

        if ((localError != QMessageManager::NoError) && (*error == QMessageManager::NoError)) {
            *error = localError;
        }
    }
}

bool MapiSession::event(QEvent *e)
{
    if (e->type() == NotifyEvent::eventType()) {
        if (NotifyEvent *ne = static_cast<NotifyEvent*>(e)) {

            QMutex* storeMutex = QMessageStorePrivate::mutex(QMessageManager());

            if(!storeMutex->tryLock())
                addToNotifyQueue(*ne);
            else
            {
                notify(ne->_store, ne->_id, ne->_notifyType);
                storeMutex->unlock();
            }
            return true;
        }
    }

    return QObject::event(e);
}

void MapiSession::notify(MapiStore *store, const QMessageId &id, MapiSession::NotifyType notifyType)
{
    QMessageManager::NotificationFilterIdSet matchingFilterIds;

    QMap<QMessageManager::NotificationFilterId, QMessageFilter>::const_iterator it = _filters.begin(), end = _filters.end();
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());

        if (!filter.isSupported())
            continue;

         // no message properties are available for a removed message, so only empty filter can match
        if (notifyType == MapiSession::Removed) {
            if (filter.isEmpty()) {
                matchingFilterIds.insert(it.key());
                break;
            }
            continue;
        }

        QMessageManager::Error ignoredError(QMessageManager::NoError);
        QMessageFilter processedFilter(QMessageFilterPrivate::preprocess(&ignoredError, _self.toStrongRef(), filter));
        if (ignoredError != QMessageManager::NoError)
            continue;

        QMessage message(id);
        foreach (QMessageFilter subfilter, QMessageFilterPrivate::subfilters(processedFilter)) {
            if (QMessageFilterPrivate::matchesMessage(subfilter, message, store)) {
                matchingFilterIds.insert(it.key());
                break; // subfilters are or'd together
            }
        }
    }

    if (!matchingFilterIds.isEmpty()) {
        void (MapiSession::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &) =
            ((notifyType == Added) ? &MapiSession::messageAdded
                                   : ((notifyType == Removed) ? &MapiSession::messageRemoved
                                                              : &MapiSession::messageUpdated));
        emit (this->*signal)(id, matchingFilterIds);
    }
}

QMessageManager::NotificationFilterId MapiSession::registerNotificationFilter(QMessageManager::Error *error, const QMessageFilter &filter)
{
    QMessageManager::NotificationFilterId filterId = ++_filterId;
    _filters.insert(filterId, filter);

    if (!_registered) {
        _registered = true;

        foreach (MapiStorePtr store, allStores(error)) {
            store->notifyEvents(fnevNewMail | fnevObjectCreated | fnevObjectCopied | fnevObjectDeleted | fnevObjectModified | fnevObjectMoved);
        }
    }

    return filterId;
}

void MapiSession::unregisterNotificationFilter(QMessageManager::Error *error, QMessageManager::NotificationFilterId filterId)
{
    _filters.remove(filterId);

    Q_UNUSED(error)
}

void MapiSession::dispatchNotifications()
{
    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    QTimer::singleShot(1000, this, SLOT(dispatchNotifications()));
}

void MapiSession::processNotifyQueue()
{
     foreach(const NotifyEvent& e, _notifyEventQueue)
     {
         QMutex* storeMutex = QMessageStorePrivate::mutex(QMessageStore::instance());
         if(storeMutex->tryLock())
         {
             NotifyEvent ne = _notifyEventQueue.dequeue();
             notify(e._store,e._id,e._notifyType);
             storeMutex->unlock();
         }
         else break;
     }
     if(!_notifyEventQueue.isEmpty())
        qWarning() << QString("Notify queue processing interrupted: pending %1").arg(_notifyEventQueue.length());
}

QMessagePrivate *MapiSession::messageImpl(const QMessage &message)
{
    return message.d_ptr;
}

QMessageContentContainerPrivate *MapiSession::containerImpl(const QMessageContentContainer &container)
{
    return container.d_ptr;
}

void MapiSession::addToNotifyQueue(const NotifyEvent& e)
{
    _notifyEventQueue.enqueue(e);
    qWarning() << QString("Store busy...adding notify to queue: pending %1").arg(_notifyEventQueue.length());
}

void MapiSession::flushNotifyQueue()
{
    QTimer::singleShot(0, this, SLOT(processNotifyQueue()));
}

#ifndef _WIN32_WCE

MapiForm::MapiForm(IMsgStore* mapiStore,
                   IMAPISession* mapiSession,
                   IMAPIFolder* mapiFolder,
                   IMessage* mapiMessage)
:
m_mapiStore(mapiStore),
m_mapiSession(mapiSession),
m_mapiFolder(mapiFolder),
m_mapiMessage(mapiMessage),
m_mapiPersistMessage(0),
m_referenceCount(1)
{
    if (m_mapiStore) m_mapiStore->AddRef();
    if (m_mapiSession) m_mapiSession->AddRef();
    if (m_mapiFolder) m_mapiFolder->AddRef();
    if (m_mapiMessage) m_mapiMessage->AddRef();
}

MapiForm::~MapiForm()
{
    releaseAll();
}

void MapiForm::releaseAll()
{
    releasePersistMessage();
    mapiRelease(m_mapiMessage);
    mapiRelease(m_mapiFolder);
    mapiRelease(m_mapiStore);
    mapiRelease(m_mapiSession);
}

STDMETHODIMP MapiForm::QueryInterface(REFIID riid, void** iface)
{
    *iface = 0;

    if (riid == IID_IMAPIMessageSite)
    {
        *iface = (IMAPIMessageSite*)this;
        AddRef();
        return S_OK;
    }

    if (riid == IID_IUnknown)
    {
        *iface = (LPUNKNOWN)((IMAPIMessageSite*)this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) MapiForm::AddRef()
{
    long refCount = InterlockedIncrement(&m_referenceCount);

    return refCount;
}

STDMETHODIMP_(ULONG) MapiForm::Release()
{
    long refCount = InterlockedDecrement(&m_referenceCount);

    if (!refCount)
          delete this;

    return refCount;
}

STDMETHODIMP MapiForm::GetSession (IMAPISession* FAR * mapiSession)
{
    HRESULT hResult = S_FALSE;

    if (mapiSession)
    {
        *mapiSession = m_mapiSession;
        if (m_mapiSession)
        {
            m_mapiSession->AddRef();
            hResult = S_OK;
        }
    }
    return hResult;
}

STDMETHODIMP MapiForm::GetStore(IMsgStore* FAR * mapiStore)
{
    HRESULT hResult = S_FALSE;

    if (mapiStore)
    {
        *mapiStore = m_mapiStore;
        if (m_mapiStore)
        {
            m_mapiStore->AddRef();
            hResult = S_OK;
        }
    }
    return hResult;
}

STDMETHODIMP MapiForm::GetFolder(IMAPIFolder* FAR * mapiFolder)
{
    HRESULT hResult = S_FALSE;

    if (mapiFolder)
    {
        *mapiFolder = m_mapiFolder;
        if (m_mapiFolder)
        {
            m_mapiFolder->AddRef();
            hResult = S_OK;
        }
    }
    return hResult;
}

STDMETHODIMP MapiForm::GetMessage(IMessage* FAR * mapiMessage)
{
    HRESULT hResult = S_FALSE;

    if (mapiMessage)
    {
        *mapiMessage = m_mapiMessage;
        if(m_mapiMessage)
        {
            m_mapiMessage->AddRef();
            hResult = S_OK;
        }
    }
    return hResult;
}

STDMETHODIMP MapiForm::GetFormManager(IMAPIFormMgr* FAR * mapiFormManager)
{
    HRESULT hRes = S_OK;
    hRes = MAPIOpenFormMgr(m_mapiSession,mapiFormManager);
    if(FAILED(hRes))
        qWarning() << "MAPIOpenFormMgr failed";
    return hRes;
}

STDMETHODIMP MapiForm::NewMessage(ULONG isComposeInFolder,
                                        IMAPIFolder* mapiFolder,
                                        IPersistMessage* mapiPersistMessage,
                                        IMessage* FAR * mapiMessage,
                                        IMAPIMessageSite* FAR * mapiMessageSite,
                                        LPMAPIVIEWCONTEXT FAR * mapiViewContext)
{

    *mapiMessage = 0;
    *mapiMessageSite = 0;

    HRESULT hRes = S_OK;

    if (mapiViewContext) *mapiViewContext = 0;

    if ((isComposeInFolder == false) || !mapiFolder)
        mapiFolder = m_mapiFolder;

    if (mapiFolder)
    {
        hRes = mapiFolder->CreateMessage(0, 0, mapiMessage);

        if (*mapiMessage)
        {
            MapiForm *mapiForm = 0;
            mapiForm = new MapiForm(m_mapiStore, m_mapiSession, mapiFolder, *mapiMessage);
            if (mapiForm)
            {
                hRes = mapiForm->setPersistMessage(NULL,mapiPersistMessage);
                *mapiMessageSite = (IMAPIMessageSite*)mapiForm;
            }
        }
    }

    return hRes;
}

STDMETHODIMP MapiForm::CopyMessage(IMAPIFolder*)
{
    return MAPI_E_NO_SUPPORT;
}

STDMETHODIMP MapiForm::MoveMessage(IMAPIFolder*, LPMAPIVIEWCONTEXT, LPCRECT)
{
    return MAPI_E_NO_SUPPORT;
}

STDMETHODIMP MapiForm::DeleteMessage(LPMAPIVIEWCONTEXT, LPCRECT)
{
    return MAPI_E_NO_SUPPORT;
}

STDMETHODIMP MapiForm::SaveMessage()
{
    HRESULT hRes = S_OK;

    if (!m_mapiPersistMessage || !m_mapiMessage)
        return MAPI_E_INVALID_PARAMETER;

    hRes = m_mapiPersistMessage->Save(0, true);

    if (FAILED(hRes))
    {
        qWarning() << "IMessage::Save failed";
        return hRes;
    }
    else
    {
        hRes = (m_mapiMessage->SaveChanges(KEEP_OPEN_READWRITE));
        hRes = (m_mapiPersistMessage->SaveCompleted(NULL));
    }

    return hRes;
}

STDMETHODIMP MapiForm::SubmitMessage(ulong flags)
{
    Q_UNUSED(flags);

    HRESULT hRes = S_OK;
    if (!m_mapiPersistMessage || !m_mapiMessage) return MAPI_E_INVALID_PARAMETER;

    hRes = (m_mapiPersistMessage->Save(m_mapiMessage,true));

    if (FAILED(hRes))
    {
        qWarning() << "IPersistMessage::Save failed";
        return hRes;
    }
    else
    {
        hRes = (m_mapiPersistMessage->HandsOffMessage());
        hRes = (m_mapiMessage->SubmitMessage(NULL));
    }

    mapiRelease(m_mapiMessage);

    return hRes;
}

STDMETHODIMP MapiForm::GetSiteStatus(ULONG* status)
{
    *status = VCSTATUS_NEW_MESSAGE;

    if (m_mapiPersistMessage)
        *status |= VCSTATUS_SAVE | VCSTATUS_SUBMIT | NULL;

    return S_OK;
}

bool MapiForm::show()
{
    IMAPIFormMgr* mapiFormManager = 0;

    if(GetFormManager(&mapiFormManager) != S_OK)
        return false;

    IMAPIForm* MAPIForm = 0;

    if(mapiFormManager->LoadForm((ULONG_PTR) 0, 0, 0, 0, 0, m_mapiFolder,
                                 this,
                                 m_mapiMessage,
                                 0,
                                 IID_IMAPIForm,
                                 (LPVOID *) &MAPIForm) != S_OK)
    {
        qWarning() << "IMAPIFormMgr::LoadForm failed";
        mapiFormManager->Release();
        return false;
    }

    mapiFormManager->Release();

    RECT rect;
    bool result = true;

    if (MAPIForm)
    {
            setPersistMessage(MAPIForm,NULL);

            HRESULT hRes = S_OK;

            hRes = MAPIForm->DoVerb(0,0,0,&rect);
            if (S_OK != hRes)
            {
                rect.left = 0;
                rect.right = 500;
                rect.top = 0;
                rect.bottom = 400;
                hRes = MAPIForm->DoVerb(0, 0, 0, &rect);
            }

            if(S_OK != hRes)
            {
                qWarning() << "IMapiForm::DoVerb Failed";
                result = false;
            }

            MAPIForm->Release();
    }

    return  result;
}

STDMETHODIMP MapiForm::GetLastError(HRESULT, ULONG, LPMAPIERROR FAR*)
{
    return MAPI_E_NO_SUPPORT;
}

void MapiForm::releasePersistMessage()
{
    if (m_mapiPersistMessage)
    {
        m_mapiPersistMessage->HandsOffMessage();
        mapiRelease(m_mapiPersistMessage);
    }
}

HRESULT MapiForm::setPersistMessage(LPMAPIFORM lpForm, IPersistMessage* mapiPersistMessage)
{
    HRESULT hRes = S_OK;
    releasePersistMessage();

    if (mapiPersistMessage)
    {
        m_mapiPersistMessage = mapiPersistMessage;
        m_mapiPersistMessage->AddRef();
    }
    else if (lpForm) hRes = (lpForm->QueryInterface(IID_IPersistMessage ,(LPVOID *)&m_mapiPersistMessage));

    return hRes;
}

#endif

#include "winhelpers.moc"
#include "moc_winhelpers_p.cpp"

QTM_END_NAMESPACE
