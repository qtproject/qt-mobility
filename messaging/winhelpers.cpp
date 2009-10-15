/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
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

#ifndef PR_IPM_DRAFTS_ENTRYID
#define PR_IPM_DRAFTS_ENTRYID ((ULONG)0x36D7) //undocumented for outlook versions < 2007
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
#include "qmessagefolder_p.h"
#include "qmessageaccount_p.h"
#include "qmessageordering_p.h"
#include "qmessagefilter_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTimer>

#include <shlwapi.h>
#include <shlguid.h>
#include <tchar.h>
#include <mapitags.h>

#ifdef _WIN32_WCE
#include <cemapi.h>
#endif


namespace {

    QWeakPointer<WinHelpers::MapiInitializer> initializer;

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
        QString addressStr = addr.recipient();
#else
        QString addressStr("[%1:%2]");
        addressStr = addressStr.arg(addr.type() == QMessageAddress::Phone ? "SMS" : "SMTP");
        addressStr = addressStr.arg(addr.recipient());
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

    void addAttachment(IMessage* message, const QMessageContentContainer& attachmentContainer)
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

            result = QMessageAddress(from, QMessageAddress::Email);
        }

        return result;
    }

    QByteArray readStream(QMessageStore::ErrorCode *lastError, IStream *is)
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
                *lastError = QMessageStore::ContentInaccessible;
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }

        return result;
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

    QByteArray extractPlainText(const QString &rtf)
    {
        // Attempt to extract the HTML from the RTF
        // as per CMapiEx, http://www.codeproject.com/KB/IP/CMapiEx.aspx
        QByteArray text;

        const QString startTag("\\fs20");
        int index = rtf.indexOf(startTag);
        if (index != -1) {
            const QString par("\\par");
            const QString tab("\\tab");
            const QString li("\\li");
            const QString fi("\\fi-");
            const QString pntext("\\pntext");

            const QChar zero = QChar('\0');
            const QChar space = QChar(' ');
            const QChar openingBrace = QChar('{');
            const QChar closingBrace = QChar('}');
            const QChar ignore[] = { openingBrace, closingBrace, QChar('\r'), QChar('\n') };

            QString::const_iterator rit = rtf.constBegin() + index, rend = rtf.constEnd();
            while ((rit != rend) && (*rit != zero)) {
                if (*rit == ignore[0] || *rit == ignore[1] || *rit == ignore[2] || *rit == ignore[3]) {
                    ++rit;
                } else {
                    bool skipSection(false);
                    bool skipDigits(false);
                    bool skipSpace(false);

                    const QString remainder(QString::fromRawData(rit, (rend - rit)));

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
                    } else if (remainder.startsWith(QString("\\'"))) {
                        rit += 2;
                        QString encodedChar(QString::fromRawData(rit, 2));
                        rit += 2;
                        text += char(encodedChar.toUInt(0, 16));
                    } else if (remainder.startsWith(pntext)) {
                        rit += pntext.length();
                        skipSection = true;
                    } else if (remainder.startsWith(QString("\\{"))) {
                        rit += 2;
                        text += "{";
                    } else if (remainder.startsWith(QString("\\}"))) {
                        rit += 2;
                        text += "}";
                    } else {
                        text += char((*rit).unicode());
                        ++rit;
                    }

                    if (skipSection) {
                        while ((rit != rend) && (*rit != closingBrace)) {
                            ++rit;
                        }
                    }
                    if (skipDigits) {
                        while ((rit != rend) && (*rit).isDigit()) {
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

    QString extractHtml(const QString &rtf)
    {
        // Attempt to extract the HTML from the RTF
        // as per CMapiEx, http://www.codeproject.com/KB/IP/CMapiEx.aspx
        QString html;

        const QString htmltag("\\*\\htmltag");
        int index = rtf.indexOf("<html", Qt::CaseInsensitive);
        if (index == -1) {
            index = rtf.indexOf(htmltag, Qt::CaseInsensitive);
        }
        if (index != -1) {
            const QString mhtmltag("\\*\\mhtmltag");
            const QString par("\\par");
            const QString tab("\\tab");
            const QString li("\\li");
            const QString fi("\\fi-");
            const QString pntext("\\pntext");
            const QString htmlrtf("\\htmlrtf");

            const QChar zero = QChar('\0');
            const QChar space = QChar(' ');
            const QChar openingBrace = QChar('{');
            const QChar closingBrace = QChar('}');
            const QChar ignore[] = { openingBrace, closingBrace, QChar('\r'), QChar('\n') };

            int tagIgnored = -1;

            QString::const_iterator rit = rtf.constBegin() + index, rend = rtf.constEnd();
            while ((rit != rend) && (*rit != zero)) {
                if (*rit == ignore[0] || *rit == ignore[1] || *rit == ignore[2] || *rit == ignore[3]) {
                    ++rit;
                } else {
                    bool skipSection(false);
                    bool skipDigits(false);
                    bool skipSpace(false);

                    const QString remainder(QString::fromRawData(rit, (rend - rit)));

                    if (remainder.startsWith(htmltag)) {
                        rit += htmltag.length();

                        int tagNumber = 0;
                        while ((*rit).isDigit()) {
                            tagNumber = (tagNumber * 10) + (*rit).digitValue();
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
                        while ((*rit).isDigit()) {
                            tagNumber = (tagNumber * 10) + (*rit).digitValue();
                            ++rit;
                        }
                        skipSpace = true;

                        tagIgnored = tagNumber;
                    } else if (remainder.startsWith(par)) {
                        rit += par.length();
                        html += QChar('\r');
                        html += QChar('\n');
                        skipSpace = true;
                    } else if (remainder.startsWith(tab)) {
                        rit += tab.length();
                        html += QChar('\t');
                        skipSpace = true;
                    } else if (remainder.startsWith(li)) {
                        rit += li.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(fi)) {
                        rit += fi.length();
                        skipDigits = true;
                        skipSpace = true;
                    } else if (remainder.startsWith(QString("\\'"))) {
                        rit += 2;

                        QString encodedChar(QString::fromRawData(rit, 2));
                        rit += 2;
                        html += QChar(encodedChar.toUInt(0, 16));
                    } else if (remainder.startsWith(pntext)) {
                        rit += pntext.length();
                        skipSection = true;
                    } else if (remainder.startsWith(htmlrtf)) {
                        rit += htmlrtf.length();

                        // Find the terminating tag
                        const QString terminator("\\htmlrtf0");
                        int index = remainder.indexOf(terminator, htmlrtf.length());
                        if (index == -1) {
                            rit = rend;
                        } else {
                            rit += (index + terminator.length() - htmlrtf.length());
                            skipSpace = true;
                        }
                    } else if (remainder.startsWith(QString("\\{"))) {
                        rit += 2;
                        html += openingBrace;
                    } else if (remainder.startsWith(QString("\\}"))) {
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
                        while ((rit != rend) && (*rit).isDigit()) {
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

    void storeMessageProperties(QMessageStore::ErrorCode *lastError, const QMessage &source, IMessage *message)
    {
        if (!setMapiProperty(message, PR_SUBJECT, source.subject())) {
            qWarning() << "Unable to set subject in message.";
            *lastError = QMessageStore::FrameworkFault;
        } else {
            QString emailAddress = source.from().recipient();
            if (!setMapiProperty(message, PR_SENDER_EMAIL_ADDRESS, emailAddress)) {
                qWarning() << "Unable to set sender address in message.";
                *lastError = QMessageStore::FrameworkFault;
            } else {
#ifdef _WIN32_WCE
                unsigned long msgType  = 0;
                if(source.type() == QMessage::Email)
                    msgType = MSGSTATUS_RECTYPE_SMTP;
                else if(msgType == QMessage::Sms)
                    msgType = MSGSTATUS_RECTYPE_SMS;
                else
                    qWarning() << "Unrecognized message type";

                if(msgType && !setMapiProperty(message, PR_MSG_STATUS, static_cast<long>(msgType)))
                {
                    qWarning() << "Unable to set type of message.";
                    *lastError = QMessageStore::FrameworkFault;
                }
                else
                {
#endif
                    if (!setMapiProperty(message, PR_CLIENT_SUBMIT_TIME, toFileTime(source.date()))) {
                        qWarning() << "Unable to set submit time in message.";
                        *lastError = QMessageStore::FrameworkFault;
                    } else {
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
                                *lastError = QMessageStore::FrameworkFault;
                            }
                        }
                    }
                }
#ifdef _WIN32_WCE
            }
#endif
        }
    }

    void replaceMessageRecipients(QMessageStore::ErrorCode *lastError, const QMessage &source, IMessage *message, IMAPISession *session)
    {
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
                        rv = recipientsTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
                        if (HR_SUCCEEDED(rv)) {
                            SRowSet *rows(0);
                            while (*lastError == QMessageStore::NoError) {
                                if (recipientsTable->QueryRows(1, 0, &rows) == S_OK) {
                                    if (rows->cRows == 0) {
                                        FreeProws(rows);
                                        break;
                                    } else if (rows->cRows == 1) {
                                        // Add this row's ID to the removal list
                                        ADRENTRY &entry(list->aEntries[index]);
                                        entry.rgPropVals[0].ulPropTag = PR_ROWID;
                                        entry.rgPropVals[0].Value.l = rows->aRow[0].lpProps[0].Value.l;
                                        ++index;
                                    }
                                    FreeProws(rows);
                                } else {
                                    *lastError = QMessageStore::ContentInaccessible;
                                    qWarning() << "Unable to query rows for recipient table";
                                }
                            }

                            if (*lastError == QMessageStore::NoError) {
                                rv = message->ModifyRecipients(MODRECIP_REMOVE, list);
                                if (HR_FAILED(rv)) {
                                    qWarning() << "Unable to clear address list for message.";
                                    *lastError = QMessageStore::FrameworkFault;
                                }
                            }
                        } else {
                            *lastError = QMessageStore::ContentInaccessible;
                            qWarning() << "Unable to set columns for recipient table";
                        }

                        destroyAddressList(list, QList<LPTSTR>());
                    } else {
                        qWarning() << "Unable to allocate address list for message.";
                        *lastError = QMessageStore::FrameworkFault;
                    }
                }
            } else {
                qWarning() << "Unable to get row count for recipients table.";
                *lastError = QMessageStore::ContentInaccessible;
            }

            mapiRelease(recipientsTable);
        } else {
            qWarning() << "Unable to get recipients table from message.";
            if(rv != MAPI_E_NO_RECIPIENTS)
                *lastError = QMessageStore::FrameworkFault;
        }

        if (*lastError == QMessageStore::NoError) {
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
                            *lastError = QMessageStore::FrameworkFault;
                        }
#ifndef _WIN32_WCE
                    } else {
                        qWarning() << "Unable to resolve address list for message.";
                        *lastError = QMessageStore::FrameworkFault;
                    }
#endif

                    destroyAddressList(list, addresses);
                } else {
                    qWarning() << "Unable to allocate address list for message.";
                    *lastError = QMessageStore::FrameworkFault;
                }
            }
        }
    }

    void replaceMessageBody(QMessageStore::ErrorCode *lastError, const QMessage &source, IMessage *message)
    {
        // Remove any preexisting body elements
        SizedSPropTagArray(2, props) = {2, {PR_BODY, PR_RTF_COMPRESSED}};
        HRESULT rv = message->DeleteProps(reinterpret_cast<LPSPropTagArray>(&props), 0);
        if (HR_SUCCEEDED(rv)) {
            // Insert the current body data
            QMessageContentContainerId bodyId(source.bodyId());
            if (bodyId.isValid()) {
                QMessageContentContainer bodyContent(source.find(bodyId));
                QByteArray subType(bodyContent.contentSubType().toLower());
                QString body(bodyContent.textContent());

                if ((subType == "rtf") || (subType == "html")) {
                    LONG textFormat(EDITOR_FORMAT_RTF);
                    if (subType == "html") {
                        // Encode the HTML within RTF
                        TCHAR codePage[6] = _T("1252");
                        GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, codePage, sizeof(codePage));

                        QString format("{\\rtf1\\ansi\\ansicpg%1\\fromhtml1 {\\*\\htmltag1 %2}}");
                        body = format.arg(QString::fromUtf16(reinterpret_cast<const quint16*>(codePage))).arg(body);
                        textFormat = EDITOR_FORMAT_HTML;
                    }

                    // Mark this message as formatted
                    if (!setMapiProperty(message, PR_MSG_EDITOR_FORMAT, textFormat)) {
                        qWarning() << "Unable to set message editor format in message.";
                        *lastError = QMessageStore::FrameworkFault;
                    }

                    IStream *os(0);
                    rv = message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_CREATE | STGM_WRITE, MAPI_CREATE | MAPI_MODIFY, reinterpret_cast<LPUNKNOWN*>(&os));
                    if (HR_SUCCEEDED(rv)) {
                        IStream *compressor(0);
#ifndef _WIN32_WCE
                        rv = WrapCompressedRTFStream(os, MAPI_MODIFY, &compressor);
                        if (HR_SUCCEEDED(rv)) {
                            compressor->Write(body.utf16(), body.length() * sizeof(TCHAR), 0);
                            rv = compressor->Commit(STGC_DEFAULT);
                            if (HR_SUCCEEDED(rv)) {
                                if (HR_FAILED(os->Commit(STGC_DEFAULT))) {
                                    qWarning() << "Unable to commit write to compressed RTF stream.";
                                    *lastError = QMessageStore::FrameworkFault;
                                }
                            } else {
                                qWarning() << "Unable to commit write to RTF compressor stream.";
                                *lastError = QMessageStore::FrameworkFault;
                            }

                            compressor->Release();
                        } else {
                            qWarning() << "Unable to open RTF compressor stream for write.";
                            *lastError = QMessageStore::FrameworkFault;
                        }

                        os->Release();
#endif
                    } else {
                        qWarning() << "Unable to open compressed RTF stream for write.";
                        *lastError = QMessageStore::FrameworkFault;
                    }
                } else {
                    // Mark this message as plain text
                    LONG textFormat(EDITOR_FORMAT_PLAINTEXT);
                    if (!setMapiProperty(message, PR_MSG_EDITOR_FORMAT, textFormat)) {
                        qWarning() << "Unable to set message editor format in message.";
                        *lastError = QMessageStore::FrameworkFault;
                    }
#ifdef _WIN32_WCE
                    // Stream the body in...
                    LPSTREAM pstm = NULL;
                    HRESULT hr = message->OpenProperty(PR_BODY, NULL, STGM_WRITE, MAPI_MODIFY,(LPUNKNOWN*)&pstm);
                    pstm->Write(body.utf16(),body.count()* sizeof(WCHAR), NULL);
                    pstm->Release();
#else
                    if (!setMapiProperty(message, PR_BODY, body)) {
                        qWarning() << "Unable to set body in message.";
                        *lastError = QMessageStore::FrameworkFault;
                    }
#endif
                }
            }
        } else {
            qWarning() << "Unable to delete existing body properties from message.";
            *lastError = QMessageStore::FrameworkFault;
        }
    }

    void replaceMessageAttachments(QMessageStore::ErrorCode *lastError, const QMessage &source, IMessage *message, WinHelpers::SavePropertyOption saveOption = WinHelpers::SavePropertyChanges )
    {
        // Find any existing attachments and remove them
        IMAPITable *attachmentsTable(0);
        HRESULT rv = message->GetAttachmentTable(MAPI_UNICODE, &attachmentsTable);
        if (HR_SUCCEEDED(rv)) {
            QList<LONG> attachmentNumbers;
            SizedSPropTagArray(1, columns) = {1, {PR_ATTACH_NUM}};
            rv = attachmentsTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
            if (HR_SUCCEEDED(rv)) {
                SRowSet *rows(0);
                ULONG rowCount = 0;
                while (*lastError == QMessageStore::NoError) {
                    if (HR_SUCCEEDED(attachmentsTable->QueryRows(1, 0, &rows))) {
                        if (rows->cRows == 0) {
                            FreeProws(rows);
                            break;
                        } else if (rows->cRows == 1) {
                            // Add this attachment's number to the removal list
                            attachmentNumbers.append(rows->aRow[0].lpProps[0].Value.l);
                            rowCount+= rows->cRows;
                        }
                        FreeProws(rows);
                    } else {
                        if(rowCount)
                            *lastError = QMessageStore::ContentInaccessible;
                        break;
                        qWarning() << "Unable to query rows for recipient table";
                    }
                }

                if ((*lastError == QMessageStore::NoError) || !attachmentNumbers.isEmpty()) {
                    while (!attachmentNumbers.isEmpty()) {
                        rv = message->DeleteAttach(attachmentNumbers.takeLast(), 0, 0, 0);
                        if (HR_FAILED(rv)) {
                            qWarning() << "Unable to remove existing attachment from message.";
                            *lastError = QMessageStore::FrameworkFault;
                        }
                    }

                    if (*lastError == QMessageStore::NoError && saveOption == WinHelpers::SavePropertyChanges ) {
#ifndef _WIN32_WCE //unsupported
                        if (HR_FAILED(message->SaveChanges(KEEP_OPEN_READWRITE))) {
                            qWarning() << "Unable to save changes to message";
                            *lastError = QMessageStore::FrameworkFault;
                        }
#endif
                    }
                }
            } else {
                *lastError = QMessageStore::ContentInaccessible;
                qWarning() << "Unable to set columns for attachments table";
            }

            mapiRelease(attachmentsTable);
        } else {
            qWarning() << "Unable to get attachments table from message.";
            *lastError = QMessageStore::FrameworkFault;
        }

        if (*lastError == QMessageStore::NoError) {
            // Add any current attachments
            foreach (const QMessageContentContainerId &attachmentId, source.attachmentIds()) {
                QMessageContentContainer attachment(source.find(attachmentId));
                bool isAttachment = (!attachment.suggestedFileName().isEmpty() && attachment.isContentAvailable());
                if (isAttachment) {
                    addAttachment(message, attachment);
                }
            }
        }
    }

    //used in preference to HrQueryAllRows
    //as per: http://blogs.msdn.com/stephen_griffin/archive/2009/03/23/try-not-to-query-all-rows.aspx

    class QueryAllRows
    {
        static const int BatchSize = 20;
        public:
        QueryAllRows(LPMAPITABLE ptable,
                LPSPropTagArray ptaga,
                LPSRestriction pres,
                LPSSortOrderSet psos);
        ~QueryAllRows();

        bool query();
        LPSRowSet rows() const;
        QMessageStore::ErrorCode lastError() const;

        private:
        LPMAPITABLE m_table;
        LPSPropTagArray m_tagArray;
        LPSRestriction m_restriction;
        LPSSortOrderSet m_sortOrderSet;
        LPSRowSet m_rows;
        QMessageStore::ErrorCode m_lastError;
    };

    QueryAllRows::QueryAllRows(LPMAPITABLE ptable,
                               LPSPropTagArray ptaga,
                               LPSRestriction pres,
                               LPSSortOrderSet psos)
    :
        m_table(ptable),
        m_tagArray(ptaga),
        m_restriction(pres),
        m_sortOrderSet(psos),
        m_rows(0),
        m_lastError(QMessageStore::NoError)
    {
        bool initFailed = false;

        unsigned long flags = 0;
#ifndef _WIN32_WCE
        flags = TBL_BATCH;
#endif

        initFailed |= FAILED(m_table->SetColumns(m_tagArray, flags));

        if(m_restriction)
            initFailed |= FAILED(m_table->Restrict(m_restriction, flags));

        if(m_sortOrderSet)
            initFailed |= FAILED(m_table->SortTable(m_sortOrderSet, flags));

        if(FAILED(m_table->SeekRow(BOOKMARK_BEGINNING,0, NULL)))
            qWarning() << "SeekRow function failed. Ensure it's not being called on hierarchy tables or message stores tables";

        if(initFailed) m_lastError = QMessageStore::ContentInaccessible;
    }

    QueryAllRows::~QueryAllRows()
    {
        FreeProws(m_rows);
        m_rows = 0;
    }

    bool QueryAllRows::query()
    {
        if(m_lastError != QMessageStore::NoError)
            return false;

        FreeProws(m_rows);
        m_rows = 0;
        m_lastError = QMessageStore::NoError;

        bool failed = FAILED(m_table->QueryRows( QueryAllRows::BatchSize, NULL, &m_rows));

        if(failed)
            m_lastError = QMessageStore::ContentInaccessible;

        if(failed || m_rows && !m_rows->cRows) return false;

        return true;
    }

    LPSRowSet QueryAllRows::rows() const
    {
        return m_rows;
    }

    QMessageStore::ErrorCode QueryAllRows::lastError() const
    {
        return m_lastError;
    }

    struct FolderHeapNode
    {
        FolderHeapNode(const MapiFolderPtr &folder, const QMessageFilter &filter);

        QMessageFilter _filter;
        MapiFolderPtr _folder;
        uint _offset; // TODO replace this with LPMAPITABLE for efficiency
        QMessage _front;
    };

    FolderHeapNode::FolderHeapNode(const MapiFolderPtr &folder, const QMessageFilter &filter)
        : _filter(filter),
          _folder(folder),
          _offset(0)
    {
    }

    typedef QSharedPointer<FolderHeapNode> FolderHeapNodePtr;

    // FolderHeap is a binary heap used to merge sort messages from different folders and stores
    class FolderHeap {
    public:
        FolderHeap(QMessageStore::ErrorCode *lastError, const QList<FolderHeapNodePtr> &protoHeap, const QMessageOrdering &ordering);
        ~FolderHeap();

        QMessage takeFront(QMessageStore::ErrorCode *lastError);
        bool isEmpty() const { return _heap.count() == 0; }

    private:
        void sink(int i); // Also known as sift-down

        QMessageFilter _filter;
        QMessageOrdering _ordering;
        QList<FolderHeapNodePtr> _heap;
    };

    FolderHeap::FolderHeap(QMessageStore::ErrorCode *lastError, const QList<FolderHeapNodePtr> &protoHeap, const QMessageOrdering &ordering)
    {
        _ordering = ordering;

        foreach (const FolderHeapNodePtr &node, protoHeap) {
            if (!node->_folder) {
                qWarning() << "Unable to access folder:" << node->_folder->name();
                continue;
            }

            node->_offset = 0;

            // TODO: Would be more efficient to use a LPMAPITABLE directly instead of calling MapiFolder queryMessages and message functions.
            QMessageIdList messageIdList(node->_folder->queryMessages(lastError, node->_filter, ordering, 1));
            if (*lastError == QMessageStore::NoError) {
                if (!messageIdList.isEmpty()) {
                    node->_front = node->_folder->message(lastError, messageIdList.front());
                    if (*lastError == QMessageStore::NoError) {
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

    QMessage FolderHeap::takeFront(QMessageStore::ErrorCode *lastError)
    {
        QMessage result(_heap[0]->_front);

        FolderHeapNodePtr node(_heap[0]);
        ++node->_offset;

        // TODO: Would be more efficient to use a LPMAPITABLE directly instead of calling MapiFolder queryMessages and message functions.
        QMessageIdList messageIdList(node->_folder->queryMessages(lastError, node->_filter, _ordering, 1, node->_offset));
        if (*lastError != QMessageStore::NoError)
            return result;

        if (messageIdList.isEmpty()) {
            if (_heap.count() > 1) {
                _heap[0] = _heap.takeLast();
            } else {
                _heap.pop_back();
            }
        } else {
            node->_front = node->_folder->message(lastError, messageIdList.front());
            if (*lastError != QMessageStore::NoError)
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
            if ((rightChild < _heap.count()) && (QMessageOrderingPrivate::lessThan(_ordering, _heap[rightChild]->_front, _heap[leftChild]->_front)))
                minChild = rightChild;

            // Reverse positions only if the child sorts before the parent
            if (QMessageOrderingPrivate::lessThan(_ordering, _heap[minChild]->_front, _heap[i]->_front)) {
                FolderHeapNodePtr temp(_heap[minChild]);
                _heap[minChild] = _heap[i];
                _heap[i] = temp;
                i = minChild;
            } else {
                return;
            }
        }
    }

    QMessageIdList filterMessages(QMessageStore::ErrorCode *lastError, QList<FolderHeapNodePtr> &folderNodes, const QMessageOrdering &ordering, uint limit, uint offset)
    {
        QMessageIdList result;
        QHash<QMessageId, bool> avoidDuplicates; // For complex filters it's necessary to check for duplicates

        FolderHeap folderHeap(lastError, folderNodes, ordering);
        if (*lastError != QMessageStore::NoError)
            return result;

        int count = 0 - offset;
        while (!folderHeap.isEmpty()) {
            // TODO: avoid retrieving unwanted messages
            if (limit && (count == limit))
                break;

            QMessage front(folderHeap.takeFront(lastError));
            if (*lastError != QMessageStore::NoError)
                return result;

            if (!avoidDuplicates.contains(front.id())) {
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

    class NotifyEvent : public QEvent
    {
    public:
        static QEvent::Type eventType();

        NotifyEvent(MapiStore *store, const QMessageId &id, MapiSession::NotifyType type);

        virtual Type type();

        MapiStore *_store;
        QMessageId _id;
        MapiSession::NotifyType _notifyType;
    };

    QEvent::Type NotifyEvent::eventType()
    {
        static int result = QEvent::registerEventType();
        return static_cast<QEvent::Type>(result);
    }

    NotifyEvent::NotifyEvent(MapiStore *store, const QMessageId &id, MapiSession::NotifyType type)
        : QEvent(eventType()),
          _store(store),
          _id(id),
          _notifyType(type)
    {
    }

    QEvent::Type NotifyEvent::type()
    {
        return eventType();
    }

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

#ifndef _WIN32_WCE
        if (!extension.isEmpty()) {
            IQueryAssociations *associations(0);
            HRESULT rv = AssocCreate(CLSID_QueryAssociations, IID_PPV_ARGS(&associations));
            if (HR_SUCCEEDED(rv)) {
                // Create the extension string to search for
                QString dotExtension(extension);
                if (!dotExtension.startsWith('.')) {
                    dotExtension.prepend('.');
                }

                Lptstr ext = LptstrFromQString(dotExtension);

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
        }

#endif
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

        if (!initializer.isNull()) {
            result = initializer.toStrongRef();
        } else {
            result = MapiInitializationToken(new MapiInitializer());
            initializer = result;
        }

        return result;
    }
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

MapiFolderPtr MapiFolder::createFolder(QMessageStore::ErrorCode *lastError, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
{
    Q_UNUSED(lastError);
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

void MapiFolder::findSubFolders(QMessageStore::ErrorCode *lastError)
{
    if (!_folder) {
        *lastError = QMessageStore::FrameworkFault;
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
                        *lastError = QMessageStore::ContentInaccessible;
                        qWarning() << "Error getting rows from sub folder table.";
                        break;
                    }
                }
            } else {
                *lastError = QMessageStore::ContentInaccessible;
                qWarning() << "Unable to set columns on folder table.";
            }

            mapiRelease(subFolders);
        } else {
            *lastError = QMessageStore::ContentInaccessible;
            qWarning() << "Unable to get hierarchy table for folder:" << name();
        }
    }
}

MapiFolderPtr MapiFolder::nextSubFolder(QMessageStore::ErrorCode *lastError)
{
    MapiFolderPtr result;

    if (!_hasSubFolders)
        return result;

    if (!_init) {
        _init = true;

        findSubFolders(lastError);
        if (*lastError != QMessageStore::NoError) {
            qWarning() << "Unable to find sub folders.";
            return result;
        }
    }

    if (!_subFolders.isEmpty()) {
        result = _store->openFolder(lastError, _subFolders.takeFirst());
    } else {
        // Allow the traversal to be restarted
        _init = false;
    }

    return result;
}

QMessageIdList MapiFolder::queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    QMessageIdList result;

    if (!_valid || !_folder) {
        Q_ASSERT(_valid && _folder);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    MapiRestriction restriction(filter);
    if (!restriction.isValid()) {
        *lastError = QMessageStore::ConstraintFailure;
        return result;
    }

    LPMAPITABLE messagesTable(0);
    HRESULT rv = _folder->GetContentsTable(MAPI_UNICODE, &messagesTable);
    if (HR_SUCCEEDED(rv)) {
        // TODO remove flags, sender, subject, they are just used for debugging
        SizedSPropTagArray(5, columns) = {5, {PR_ENTRYID, PR_RECORD_KEY, PR_MESSAGE_FLAGS, PR_SENDER_NAME, PR_SUBJECT}};
        rv = messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0);
        if (HR_SUCCEEDED(rv)) {
            if (!ordering.isEmpty()) {
                QMessageOrderingPrivate::sortTable(lastError, ordering, messagesTable);
            }

            if (*lastError == QMessageStore::NoError) {
                if (!restriction.isEmpty()) {
                    ULONG flags(0);
                    if (messagesTable->Restrict(restriction.sRestriction(), flags) != S_OK)
                        *lastError = QMessageStore::ConstraintFailure;
                }
            }

            if (*lastError == QMessageStore::NoError) {
                uint workingLimit(limit);
                LPSRowSet rows(0);
                LONG ignored;
                rv = messagesTable->SeekRow(BOOKMARK_BEGINNING, offset, &ignored);
                if (HR_SUCCEEDED(rv)) {
                    while (true) {
                        rv = messagesTable->QueryRows(1, 0, &rows);
                        if (HR_SUCCEEDED(rv)) {
                            if (rows->cRows == 1) {
                                if (limit) {
                                    --workingLimit;
                                }

                                LPSPropValue entryIdProp(&rows->aRow[0].lpProps[0]);
                                LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[1]);
                                MapiRecordKey recordKey(recordKeyProp->Value.bin.lpb, recordKeyProp->Value.bin.cb);
                                MapiEntryId entryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);
#ifdef _WIN32_WCE
                                result.append(QMessageIdPrivate::from(_store->entryId(), entryId, recordKey, _entryId));
#else
                                result.append(QMessageIdPrivate::from(_store->recordKey(), entryId, recordKey, _key));
#endif

                                FreeProws(rows);
                                if (limit && !workingLimit)
                                    break;
                            } else {
                                // We have retrieved all rows
                                FreeProws(rows);
                                break;
                            }
                        } else {
                            *lastError = QMessageStore::ContentInaccessible;
                            qWarning() << "Unable to query rows in message table.";
                        }
                    }
                } else {
                    *lastError = QMessageStore::ContentInaccessible;
                    qWarning() << "Unable to seek to offset in message table.";
                }
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
            return QMessageIdList();
        }

        mapiRelease(messagesTable);
    } else {
        *lastError = QMessageStore::ContentInaccessible;
        return QMessageIdList(); // TODO set last error to content inaccessible, and review all != S_OK result handling
    }

    return result;
}

uint MapiFolder::countMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter) const
{
    uint result(0);

    if (!_valid || !_folder) {
        Q_ASSERT(_valid && _folder);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    MapiRestriction restriction(filter);
    if (!restriction.isValid()) {
        *lastError = QMessageStore::ConstraintFailure;
        return result;
    }

    IMAPITable *messagesTable(0);
    HRESULT rv = _folder->GetContentsTable(MAPI_UNICODE, &messagesTable);
    if (HR_SUCCEEDED(rv)) {
        if (!restriction.isEmpty()) {
            ULONG flags(0);
            rv = messagesTable->Restrict(restriction.sRestriction(), flags);
            if (HR_FAILED(rv)) {
                *lastError = QMessageStore::ConstraintFailure;
                qWarning() << "Unable to set restriction on message table.";
            }
        }
        if (*lastError == QMessageStore::NoError) {
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
        *lastError = QMessageStore::ContentInaccessible;
        qWarning() << "Unable to get folder contents table.";
    }

    return result;
}

void MapiFolder::removeMessages(QMessageStore::ErrorCode *lastError, const QMessageIdList &ids)
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
            *lastError = QMessageStore::ContentInaccessible;
            qWarning() << "Unable to delete messages from folder.";
        }
    }

    MAPIFreeBuffer(bin);
}

MapiEntryId MapiFolder::messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &messageKey)
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
                        *lastError = QMessageStore::InvalidId;
                    }
                    FreeProws(rows);
                } else {
                    *lastError = QMessageStore::ContentInaccessible;
                    qWarning() << "Unable to query rows from message table.";
                }
            } else {
                *lastError = QMessageStore::ContentInaccessible;
                qWarning() << "Unable to set columns on message table.";
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
            qWarning() << "Unable to restrict content table.";
        }

        mapiRelease(messagesTable);
    } else {
        *lastError = QMessageStore::ContentInaccessible;
        qWarning() << "Unable to get contents table for folder.";
    }

    return result;
}

IMessage *MapiFolder::openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _folder->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (rv != S_OK) {
        *lastError = QMessageStore::InvalidId;
        qWarning() << "Invalid message entryId:" << entryId.toBase64();
    }

    return message;
}

QMessageFolder MapiFolder::folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const
{
    return _store->folder(lastError, id);
}

QMessage MapiFolder::message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const
{
    return _store->message(lastError, id);
}

QMessage::StandardFolder MapiFolder::standardFolder() const
{
    // TODO implement this function
    return QMessage::InboxFolder; // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId MapiFolder::id() const
{
#ifdef _WIN32_WCE
    return QMessageFolderIdPrivate::from(_key, _store->entryId(), _entryId);
#else
    return QMessageFolderIdPrivate::from(_key, _store->recordKey(), _entryId);
#endif
}
#endif

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
        propertyMap.insert(QMessage::DraftsFolder,PROP_TAG(PT_BINARY,PR_IPM_DRAFTS_ENTRYID));
        propertyMap.insert(QMessage::TrashFolder,PROP_TAG(PT_BINARY,0x35E3));
        propertyMap.insert(QMessage::OutboxFolder,PROP_TAG(PT_BINARY,0x35E2));
        propertyMap.insert(QMessage::SentFolder,PROP_TAG(PT_BINARY,0x35E4));

        init = true;
    }

    return propertyMap.value(folder);
}

IMessage *MapiFolder::createMessage(QMessageStore::ErrorCode* lastError)
{
    IMessage *message = 0;

    if(FAILED(_folder->CreateMessage(NULL, 0, &message)!=S_OK))
    {
        *lastError = QMessageStore::FrameworkFault;
        mapiRelease(message);
    }
    return message;
}

IMessage* MapiFolder::createMessage(QMessageStore::ErrorCode* lastError, const QMessage& source, const MapiSessionPtr &session, PostSendAction postSendAction, SavePropertyOption saveOption )
{
    IMessage* mapiMessage(0);
    HRESULT rv = _folder->CreateMessage(0, 0, &mapiMessage);
    if (HR_SUCCEEDED(rv)) {
        // Store the message properties
        if (*lastError == QMessageStore::NoError) {
            storeMessageProperties(lastError, source, mapiMessage);
        }
        if (*lastError == QMessageStore::NoError) {
            if (postSendAction == DeleteAfterSend) {
                if (!setMapiProperty(mapiMessage, PR_DELETE_AFTER_SUBMIT, true)) {
                    qWarning() << "Unable to set delete after send flag.";
                }
            } else if (postSendAction == MoveAfterSend) {
                /*
                //move the message to the sent folder after a submission
                MapiFolderPtr sentFolder = _store->findFolder(lastError,QMessage::SentFolder);

                if (sentFolder.isNull() || *lastError != QMessageStore::NoError) {
                    qWarning() << "Unable to find the sent folder while constructing message";
                } else {
                    if (!setMapiProperty(mapiMessage, PR_SENTMAIL_ENTRYID, sentFolder->entryId())) {
                        qWarning() << "Unable to set sent folder entry id on message";
                    }
                }
                */
            }
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageRecipients(lastError, source, mapiMessage, session->session());
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageBody(lastError, source, mapiMessage);
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageAttachments(lastError, source, mapiMessage, saveOption );
        }
        if (*lastError == QMessageStore::NoError && saveOption == SavePropertyChanges ) {
#ifndef _WIN32_WCE //unsupported
            if (HR_FAILED(mapiMessage->SaveChanges(0))) {
                qWarning() << "Unable to save changes for message.";
            }
#endif
        }
        if (*lastError != QMessageStore::NoError) {
            mapiRelease(mapiMessage);
        }
    } else {
        qWarning() << "Failed to create MAPI message";
        *lastError = QMessageStore::FrameworkFault;
    }

    return mapiMessage;
}

MapiStorePtr MapiStore::createStore(QMessageStore::ErrorCode *lastError, const MapiSessionPtr &session, IMsgStore *store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode)
{
    Q_UNUSED(lastError);
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

MapiFolderPtr MapiStore::findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key)
{
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
        if (!subFolder.isNull() && subFolder->isValid()) {
            if (subFolder->recordKey() == key) {
                return subFolder;
            }
            folders.append(subFolder);
        } else {
            folders.pop_back();
        }
    }

    return MapiFolderPtr();
}

MapiFolderPtr MapiStore::findFolder(QMessageStore::ErrorCode *lastError, QMessage::StandardFolder sf)
{
    MapiFolderPtr result;

    //check if the store supports the common folder
    //assume drafts exists in every store since there is no mask for it

    unsigned long tag =0;
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
    }

    LPSPropValue props=0;
    ULONG cValues=0;
    ULONG rTags[]={ 1,PR_VALID_FOLDER_MASK};

    if(FAILED(_store->GetProps((LPSPropTagArray) rTags, 0, &cValues, &props)))
    {
        qWarning() << "Could not get folder mask property";
        MAPIFreeBuffer(props);
        return result;
    }

    bool commonFolderSupported = tag ? tag & props[0].Value.ul : true; //true for drafts

    MAPIFreeBuffer(props);

#ifndef _WIN32_WCE
    if(!commonFolderSupported)
    {
        return result;
    }
#endif

    MapiFolderPtr baseFolder = receiveFolder(lastError); //start with inbox

    switch(sf)
    {
        case QMessage::InboxFolder:
        {
            result = baseFolder;
            return result;
        } break;

        case QMessage::DraftsFolder:
        {
            if(*lastError != QMessageStore::NoError || baseFolder.isNull())
            {
                //try and use the root folder

                baseFolder = rootFolder(lastError);

                if(*lastError != QMessageStore::NoError || baseFolder.isNull())
                {
                    qWarning() << "Could not get a base folder";
                    return result;
                }
            }

            props= 0;
            cValues=0;
            ULONG gTags[]={1,commonFolderMap(sf)};

            if(FAILED(baseFolder->folder()->GetProps((LPSPropTagArray) gTags, 0, &cValues, &props)))
            {
                qWarning() << "Failed to get common folder from root folder";
                *lastError = QMessageStore::ContentInaccessible;
                return result;
            }

        } break;

        case QMessage::TrashFolder:
        case QMessage::SentFolder:
        case QMessage::OutboxFolder:
        {
            props= 0;
            cValues=0;
            ULONG gTags[]={1,commonFolderMap(sf)};

            if(FAILED(_store->GetProps((LPSPropTagArray) gTags, 0, &cValues, &props)) || props[0].ulPropTag != gTags[1])
            {
                qWarning() << "Failed to get common folder from store";
                *lastError = QMessageStore::ContentInaccessible;
                return result;
            }
        }
        break;
    }

    MapiEntryId entryId(props[0].Value.bin.lpb, props[0].Value.bin.cb);
    MAPIFreeBuffer(props);
    result = openFolder(lastError, entryId);
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList MapiStore::folderIds(QMessageStore::ErrorCode *lastError)
{
    QMessageFolderIdList folderIds;

    MapiFolderPtr root(rootFolder(lastError));
    if (*lastError == QMessageStore::NoError) {
        QList<MapiFolderPtr> folders;
        folders.append(root);
        // No valid reason to list the root folder.

        while (!folders.isEmpty()) {
            MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
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

QMessageFolder MapiStore::folderFromId(QMessageStore::ErrorCode *lastError, const QMessageFolderId &folderId)
{
    QMessageFolder result;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
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
#endif

MapiEntryId MapiStore::messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey)
{
    MapiEntryId result;

    MapiFolderPtr folder(openFolderWithKey(lastError, folderKey));
    if (*lastError == QMessageStore::NoError) {
        result = folder->messageEntryId(lastError, messageKey);
    }

    return result;
}

MapiFolderPtr MapiStore::openFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId& entryId) const
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
    IMAPIFolder *folder = openMapiFolder(lastError, entryId);
    if (folder && (*lastError == QMessageStore::NoError)) {
        SizedSPropTagArray(4, columns) = {4, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
            uint messageCount = properties[2].Value.ul;
            bool hasSubFolders = properties[3].Value.b;

            MapiFolderPtr folderPtr = MapiFolder::createFolder(lastError, _self.toStrongRef(), folder, recordKey, name, entryId, hasSubFolders, messageCount);
            if (*lastError == QMessageStore::NoError) {
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

MapiFolderPtr MapiStore::openFolderWithKey(QMessageStore::ErrorCode *lastError, const MapiRecordKey &recordKey) const
{
    MapiFolderPtr result;

    MapiFolderPtr root(rootFolder(lastError));
    if (*lastError == QMessageStore::NoError) {
        if (root->recordKey() == recordKey) {
            result = root;
        } else {
            IMAPITable *folderTable(0);
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
                                result = openFolder(lastError, entryId);
                            } else {
                                *lastError = QMessageStore::InvalidId;
                            }
                            FreeProws(rows);
                        } else {
                            *lastError = QMessageStore::InvalidId;
                        }
                    } else {
                        *lastError = QMessageStore::ContentInaccessible;
                        qWarning() << "Unable to set columns for folder table";
                    }
                } else {
                    *lastError = QMessageStore::ContentInaccessible;
                    qWarning() << "Unable to restrict folder table";
                }

                mapiRelease(folderTable);
            } else {
                *lastError = QMessageStore::ContentInaccessible;
                qWarning() << "Unable to open hierarchy table for store.";
            }
        }
    }

    return result;
}

bool MapiStore::supports(ULONG featureFlag) const
{
    LONG supportMask(0);

    if (getMapiProperty(store(), PR_STORE_SUPPORT_MASK, &supportMask)) {
        return supportMask & featureFlag;
    }
    else
        qWarning() << "Unable to query store support mask.";

    return false;
}

IMessage *MapiStore::openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _store->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (HR_SUCCEEDED(rv)) {
        if (objectType != MAPI_MESSAGE) {
            qWarning() << "Not a message - wrong object type:" << objectType;
            mapiRelease(message);
            *lastError = QMessageStore::InvalidId;
        }
    } else {
        *lastError = QMessageStore::InvalidId;
        qWarning() << "Invalid message entryId:" << entryId.toBase64();
    }

    return message;
}

IMAPIFolder *MapiStore::openMapiFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const
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
            *lastError = QMessageStore::InvalidId;
        }
    } else {
        *lastError = QMessageStore::InvalidId;
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

QMessageAddress MapiStore::address() const
{
    QMessageAddress result;


    return result;
}

MapiFolderPtr MapiStore::rootFolder(QMessageStore::ErrorCode *lastError) const
{
    MapiFolderPtr result;

    if (!_valid || !_store) {
        Q_ASSERT(_valid && _store);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    SPropValue *rgProps(0);
    ULONG rgTags[] = {1, PR_IPM_SUBTREE_ENTRYID};
    ULONG cCount;

    HRESULT rv = _store->GetProps(reinterpret_cast<LPSPropTagArray>(rgTags), MAPI_UNICODE, &cCount, &rgProps);
        if (HR_FAILED(rv)) {
        *lastError = QMessageStore::ContentInaccessible;
        qWarning() << "Unable to get properties for root folder - rv:" << hex << (ULONG)rv;
        return result;
    }

    MapiEntryId entryId(rgProps[0].Value.bin.lpb, rgProps[0].Value.bin.cb);
    MAPIFreeBuffer(rgProps);

    return openFolder(lastError, entryId);
}

MapiFolderPtr MapiStore::receiveFolder(QMessageStore::ErrorCode *lastError) const
{
    MapiFolderPtr result;

    ULONG entryIdSize = 0;
    LPENTRYID entryIdPtr = 0;

    if(FAILED(_store->GetReceiveFolder(NULL, 0, &entryIdSize, &entryIdPtr, NULL)))
    {
       *lastError = QMessageStore::FrameworkFault;
       qWarning() << "Could not get the receive folder";
       return result;
    }

    MapiEntryId entryId(entryIdPtr, entryIdSize);
    MAPIFreeBuffer(entryIdPtr);

    return openFolder(lastError, entryId);
}

QMessageIdList MapiStore::queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    QList<FolderHeapNodePtr> folderNodes;

    MapiStorePtr store(_self.toStrongRef());

    MapiFolderIterator folderIt(QMessageFilterPrivate::folderIterator(filter, lastError, store));
    for (MapiFolderPtr folder(folderIt.next()); folder && folder->isValid(); folder = folderIt.next()) {
        QList<QMessageFilter> orderingFilters;
        orderingFilters.append(filter);
        foreach(QMessageFilter orderingFilter, QMessageOrderingPrivate::normalize(orderingFilters, ordering)) {
            folderNodes.append(FolderHeapNodePtr(new FolderHeapNode(folder, orderingFilter)));
        }
    }

    if (*lastError != QMessageStore::NoError)
        return QMessageIdList();

    return filterMessages(lastError, folderNodes, ordering, limit, offset);
}

QMessageFolder MapiStore::folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const
{
    return _session.toStrongRef()->folder(lastError, id);
}

QMessage MapiStore::message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const
{
    return _session.toStrongRef()->message(lastError, id);
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
                        QCoreApplication::postEvent(session.data(), new NotifyEvent(_store, messageId, notifyType));
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
    ptr = MapiSessionPtr(newSession);
    ptr->_self = ptr;
    return true;
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

MapiSessionPtr MapiSession::createSession(QMessageStore::ErrorCode *lastError)
{
    static bool initialized(manager()->initialize(new MapiSession(lastError)));

    MapiSessionPtr ptr(manager()->session());
    if (ptr.isNull()) {
        if (*lastError == QMessageStore::NoError) {
            *lastError = QMessageStore::ContentInaccessible;
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

MapiSession::MapiSession(QMessageStore::ErrorCode *lastError)
    :QObject(),
     _token(WinHelpers::initializeMapi()),
     _mapiSession(0),
     _filterId(0),
     _registered(false)
{
    if (!_token->_initialized) {
        *lastError = QMessageStore::ContentInaccessible;
    } else {
        // Attempt to start a MAPI session on the default profile
        HRESULT rv = MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &_mapiSession);
        if (HR_FAILED(rv)) {
            qWarning() << "Failed to login to MAPI session - rv:" << hex << (ULONG)rv;
            *lastError = QMessageStore::ContentInaccessible;
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

MapiStorePtr MapiSession::findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id, bool cachedMode) const
{
    MapiStorePtr result(0);
    if (!_mapiSession) {
        Q_ASSERT(_mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result;
    }

    const int nCols(3);
    enum { defaultStoreColumn = 0, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_ENTRYID, PR_RECORD_KEY}};

    QueryAllRows qar(mapiMessageStoresTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0);

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
                result = openStore(lastError, entryId, cachedMode);
                break;
            }
        }
    }

    *lastError = qar.lastError();

    mapiRelease(mapiMessageStoresTable);

    return result;
}

QList<MapiStorePtr> MapiSession::allStores(QMessageStore::ErrorCode *lastError, bool cachedMode) const
{
    QList<MapiStorePtr> result;
    if (!_mapiSession) {
        Q_ASSERT(_mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
    } else {
        SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};

        QueryAllRows qar(mapiMessageStoresTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0);

        while (qar.query()) {
            for (uint n = 0; n < qar.rows()->cRows; ++n) {
                MapiEntryId entryId(qar.rows()->aRow[n].lpProps[0].Value.bin.lpb, qar.rows()->aRow[n].lpProps[0].Value.bin.cb);
                MapiStorePtr store(openStore(lastError, entryId, cachedMode));
                if (!store.isNull()) {
                    // We only want stores that contain private messages
#ifndef _WIN32_WCE
                    if(!store->supports(STORE_PUBLIC_FOLDERS))
#endif
                        result.append(store);
                }
            }
        }

        *lastError = qar.lastError();

        mapiRelease(mapiMessageStoresTable);
    }

    return result;
}

IMsgStore *MapiSession::openMapiStore(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId, bool cachedMode) const
{
    IMsgStore *store(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    unsigned long openFlags = MAPI_BEST_ACCESS | MDB_WRITE;

    if(!cachedMode)
        openFlags |= MDB_ONLINE;

    HRESULT rv = _mapiSession->OpenMsgStore(0, entryId.count(), entryIdPtr, 0, openFlags, reinterpret_cast<LPMDB*>(&store));
    if (HR_FAILED(rv)) {
        *lastError = QMessageStore::InvalidId;
        qWarning() << "Invalid store entryId:" << entryId.toBase64();
    }

    return store;
}

MapiStorePtr MapiSession::openStore(QMessageStore::ErrorCode *lastError, const MapiEntryId& entryId, bool cachedMode) const
{
    MapiStorePtr result(0);

    // See if we can create a new pointer to an existing store
    MapiStorePtr &existing = _storeMap[entryId];
    if (!existing.isNull()) {
        return existing;
    }

    // We need to create a new instance
    IMsgStore *store = openMapiStore(lastError, entryId, cachedMode);
    if (store && (*lastError == QMessageStore::NoError)) {
        // Find the other properties of this store
        SizedSPropTagArray(2, columns) = {2, {PR_DISPLAY_NAME, PR_RECORD_KEY}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = store->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);

            MapiStorePtr storePtr = MapiStore::createStore(lastError, _self.toStrongRef(), store, recordKey, entryId, name, cachedMode);
            if (*lastError == QMessageStore::NoError) {
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

MapiStorePtr MapiSession::openStoreWithKey(QMessageStore::ErrorCode *lastError, const MapiRecordKey &recordKey, bool cachedMode) const
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

                        result = openStore(lastError, entryId, cachedMode);
                    } else {
                        *lastError = QMessageStore::InvalidId;
                    }
                    FreeProws(rows);
                } else {
                    *lastError = QMessageStore::InvalidId;
                }
            } else {
                *lastError = QMessageStore::ContentInaccessible;
                qWarning() << "Unable to set columns for stores table";
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
            qWarning() << "Unable to restrict stores table";
        }

        mapiRelease(storesTable);
    } else {
        *lastError = QMessageStore::ContentInaccessible;
        qWarning() << "Unable to open stores table.";
    }

    return result;
}

QMessageAccountId MapiSession::defaultAccountId(QMessageStore::ErrorCode *lastError, QMessage::Type type) const
{
#ifdef _WIN32_WCE
    if (type == QMessage::Sms) {
        foreach (MapiStorePtr store, allStores(lastError)) {
            if (store->name() == "SMS") {
                return store->id();
            }
        }
    }

#endif
    if (type == QMessage::Email) {
        // Return the account of the default store
        MapiStorePtr store(defaultStore(lastError));
        if (*lastError == QMessageStore::NoError) {
            return store->id();
        }
    }

    return QMessageAccountId();
}

IMessage *MapiSession::openMapiMessage(QMessageStore::ErrorCode *lastError, const QMessageId &id) const
{
    IMessage *message(0);

#ifdef _WIN32_WCE
    MapiEntryId storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
#else
    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
#endif

    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError == QMessageStore::NoError) {
        MapiEntryId entryId(QMessageIdPrivate::entryId(id));

        message = mapiStore->openMessage(lastError, entryId);
        if (*lastError != QMessageStore::NoError) {
            qWarning() << "Invalid message entryId:" << entryId.toBase64();
            mapiRelease(message);
        }
    } else {
        qWarning() << "Invalid store recordKey:" << storeRecordKey.toBase64();
    }

    return message;
}

MapiEntryId MapiSession::messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &storeKey, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey)
{
    MapiEntryId result;

    MapiStorePtr store(openStoreWithKey(lastError, storeKey));
    if (*lastError == QMessageStore::NoError) {
        result = store->messageEntryId(lastError, folderKey, messageKey);
    }

    return result;
}

MapiRecordKey MapiSession::messageRecordKey(QMessageStore::ErrorCode *lastError, const QMessageId &id)
{
    MapiRecordKey result;

    IMessage *message = openMapiMessage(lastError, id);
    if (*lastError == QMessageStore::NoError) {
        if (!getMapiProperty(message, PR_RECORD_KEY, &result)) {
            qWarning() << "Unable to query message record key.";
        }

        mapiRelease(message);
    }

    return result;
}

MapiRecordKey MapiSession::folderRecordKey(QMessageStore::ErrorCode *lastError, const QMessageId &id)
{
    MapiRecordKey result;

    IMessage *message = openMapiMessage(lastError, id);
    if (*lastError == QMessageStore::NoError) {
        // Find the other properties of this store
        SizedSPropTagArray(2, columns) = {2, {PR_STORE_ENTRYID, PR_PARENT_ENTRYID}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            if ((properties[0].ulPropTag == PR_STORE_ENTRYID) && (properties[1].ulPropTag == PR_PARENT_ENTRYID)) {
                MapiEntryId storeId(properties[0].Value.bin.lpb, properties[0].Value.bin.cb);
                MapiEntryId folderId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);

                MapiStorePtr store = openStore(lastError, storeId, true);
                if (*lastError == QMessageStore::NoError) {
                    MapiFolderPtr folder = store->openFolder(lastError, folderId);
                    if (*lastError == QMessageStore::NoError) {
                        result = folder->recordKey();
                    }
                }
            } else {
                qWarning() << "Unable to extract store and folder entry IDs from message.";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to query store and folder entry IDs from message.";
        }

        mapiRelease(message);
    }

    return result;
}

#ifdef _WIN32_WCE

MapiEntryId MapiSession::folderEntryId(QMessageStore::ErrorCode *lastError, const QMessageId &id)
{
    MapiEntryId result;

    IMessage *message = openMapiMessage(lastError, id);
    if (*lastError == QMessageStore::NoError) {
        // Find the other properties of this store
        SizedSPropTagArray(2, columns) = {2, {PR_STORE_ENTRYID, PR_PARENT_ENTRYID}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            if ((properties[0].ulPropTag == PR_STORE_ENTRYID) && (properties[1].ulPropTag == PR_PARENT_ENTRYID)) {
                MapiEntryId storeId(properties[0].Value.bin.lpb, properties[0].Value.bin.cb);
                MapiEntryId folderId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);

                MapiStorePtr store = openStore(lastError, storeId, true);
                if (*lastError == QMessageStore::NoError) {
                    MapiFolderPtr folder = store->openFolder(lastError, folderId);
                    if (*lastError == QMessageStore::NoError) {
                        result = folder->entryId();
                    }
                }
            } else {
                qWarning() << "Unable to extract store and folder entry IDs from message.";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to query store and folder entry IDs from message.";
        }

        mapiRelease(message);
    }

    return result;
}

#endif

bool MapiSession::flushQueues()
{
    LPMAPISTATUS  pStat = 0;    //MAPI Status Pointer
    LPMAPITABLE     pTbl  = 0;
    HRESULT         hRes;
    SRestriction    sres;
    SPropValue      spv;
    ULONG           ulObjType = 0;

    const static SizedSPropTagArray(2,sptCols) = {2,PR_RESOURCE_TYPE,PR_ENTRYID};

    if (FAILED(hRes = _mapiSession->GetStatusTable(0,&pTbl)))
    {
        mapiRelease(pTbl);
        return false;
    }

    sres.rt = RES_PROPERTY;
    sres.res.resProperty.relop     = RELOP_EQ;
    sres.res.resProperty.ulPropTag = PR_RESOURCE_TYPE;
    sres.res.resProperty.lpProp    = &spv;

    spv.ulPropTag = PR_RESOURCE_TYPE;
    spv.Value.l   = MAPI_SPOOLER;

    QueryAllRows qar(pTbl,
            (LPSPropTagArray) &sptCols,
            &sres,
            0);

    if(!qar.query())
        goto Quit;

    if (!qar.rows()->cRows || PR_ENTRYID != qar.rows()->aRow[0].lpProps[1].ulPropTag)
    {
        hRes = MAPI_E_NOT_FOUND;
        goto Quit;
    }

    hRes =  _mapiSession->OpenEntry(qar.rows()->aRow[0].lpProps[1].Value.bin.cb,
            (LPENTRYID)qar.rows()->aRow[0].lpProps[1].Value.bin.lpb,
            NULL,
            MAPI_BEST_ACCESS,
            &ulObjType,
            (LPUNKNOWN *)&(*pStat));

    if (FAILED(hRes) || MAPI_STATUS != ulObjType)
    {
        hRes = hRes ? hRes : MAPI_E_INVALID_OBJECT;
        goto Quit;
    }

    if(pStat) //if we successfully got a status pointer call FlushQueues on it.
    {
        if(FAILED(pStat->FlushQueues(NULL, 0, NULL, FLUSH_UPLOAD | FLUSH_DOWNLOAD)))
        {
            qWarning() << "Failed to flush MAPI queues";
            goto Quit;
        }
    }
    else goto Quit;

    mapiRelease(pStat);
    return true;

Quit:
    if (pTbl) pTbl -> Release();
    return false;
}

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

QMessageFolder MapiSession::folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const
{
    QMessageFolder result;

    if(!id.isValid())
    {
        *lastError = QMessageStore::InvalidId;
        return result;
    }

#ifdef _WIN32_WCE
    MapiEntryId storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
#else
    MapiRecordKey storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
#endif
    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError != QMessageStore::NoError)
        return result;

    // Find the root folder for this store
    MapiFolderPtr storeRoot(mapiStore->rootFolder(lastError));
    if (*lastError != QMessageStore::NoError)
        return result;

    MapiEntryId entryId(QMessageFolderIdPrivate::entryId(id));
    MapiFolderPtr folder = mapiStore->openFolder(lastError, entryId);
    if (folder && (*lastError == QMessageStore::NoError)) {
        SizedSPropTagArray(3, columns) = {3, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_PARENT_ENTRYID}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString displayName(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey folderKey(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
            MapiEntryId parentEntryId(properties[2].Value.bin.lpb, properties[2].Value.bin.cb);

            MAPIFreeBuffer(properties);

            QMessageFolderId folderId(QMessageFolderIdPrivate::from(folderKey, storeRecordKey, entryId));

            QStringList path;
            path.append(displayName);

            QMessageFolderId parentId;
            MapiEntryId ancestorEntryId(parentEntryId);
            MapiFolderPtr ancestorFolder;

            // Iterate through ancestors towards the root
            while ((ancestorFolder = mapiStore->openFolder(lastError, ancestorEntryId)) &&
                   (ancestorFolder && (*lastError == QMessageStore::NoError))) {
                SPropValue *ancestorProperties(0);
                if (ancestorFolder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &ancestorProperties) == S_OK) {
                    SPropValue &ancestorRecordKeyProp(ancestorProperties[1]);
                    MapiRecordKey ancestorRecordKey(ancestorRecordKeyProp.Value.bin.lpb, ancestorRecordKeyProp.Value.bin.cb);

                    if (ancestorEntryId == parentEntryId) {
                        // This ancestor is the parent of the folder being retrieved, create a QMessageFolderId for the parent
                        parentId = QMessageFolderIdPrivate::from(ancestorRecordKey, storeRecordKey, parentEntryId);
                    }

                    SPropValue &entryIdProp(ancestorProperties[2]);
                    ancestorEntryId = MapiEntryId(entryIdProp.Value.bin.lpb, entryIdProp.Value.bin.cb);

                    QString ancestorName(QStringFromLpctstr(ancestorProperties[0].Value.LPSZ));

                    MAPIFreeBuffer(ancestorProperties);

                    if (ancestorRecordKey == storeRoot->recordKey()) {
                        // Reached the root and have a complete path for the folder being retrieved
                        QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));
                        return QMessageFolderPrivate::from(folderId, accountId, parentId, displayName, path.join("/"));
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
    result = mapiStore->folderFromId(lastError, id);
    return result;
}

QMessage MapiSession::message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const
{
    Q_UNUSED(lastError);

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

bool MapiSession::updateMessageProperties(QMessageStore::ErrorCode *lastError, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.properties) {
        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.properties = 1;

        IMessage *message = openMapiMessage(lastError, msg->id());
        if (*lastError == QMessageStore::NoError) {
            SizedSPropTagArray(14, msgCols) = {14, { PR_RECORD_KEY,
                                                     PR_MESSAGE_FLAGS,
                                                     PR_MSG_STATUS,
                                                     PR_MESSAGE_CLASS,
                                                     PR_SENDER_NAME,
                                                     PR_SENDER_EMAIL_ADDRESS,
                                                     PR_CLIENT_SUBMIT_TIME,
                                                     PR_MESSAGE_DELIVERY_TIME,
                                                     PR_TRANSPORT_MESSAGE_HEADERS,
                                                     PR_HASATTACH,
                                                     PR_SUBJECT,
                                                     PR_MSG_EDITOR_FORMAT,
                                                     PR_RTF_IN_SYNC,
#ifdef _WIN32_WCE
                                                     PR_CONTENT_LENGTH
#else
                                                     PR_MESSAGE_SIZE
#endif
                                                     }};
            ULONG count = 0;
            LPSPropValue properties;
            HRESULT rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&msgCols), MAPI_UNICODE, &count, &properties);
            if (HR_SUCCEEDED(rv)) {
                QString senderName;
                QString senderAddress;
                QString messageClass;

                QMessage::StatusFlags flags(0);

                for (ULONG n = 0; n < count; ++n) {
                    SPropValue &prop(properties[n]);

                    switch (prop.ulPropTag) {
                    case PR_MESSAGE_FLAGS:
                        if (prop.Value.ul & MSGFLAG_READ) {
                            flags |= QMessage::Read;
                        }
                        break;
                    case PR_MSG_STATUS:
                        if (prop.Value.l & (MSGSTATUS_DELMARKED | MSGSTATUS_REMOTE_DELETE)) {
                            flags |= QMessage::Removed;
                        }
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
                    case PR_TRANSPORT_MESSAGE_HEADERS:
                        // This message must have come from an external source
                        flags |= QMessage::Incoming;
                        break;
                    case PR_SUBJECT:
                        msg->setSubject(QStringFromLpctstr(prop.Value.LPSZ));
                        break;
                    case PR_HASATTACH:
                        msg->d_ptr->_hasAttachments = (prop.Value.b != FALSE);
                        break;
                    case PR_MSG_EDITOR_FORMAT:
                        msg->d_ptr->_contentFormat = prop.Value.l;
                        break;
                    case PR_RTF_IN_SYNC:
                        msg->d_ptr->_rtfInSync = (prop.Value.b != FALSE);;
                        break;
#ifdef _WIN32_WCE
                    case PR_CONTENT_LENGTH:
#else
                    case PR_MESSAGE_SIZE:
#endif
                        // Increase the size estimate by a third to allow for transfer encoding
                        QMessagePrivate::setSize(*msg, prop.Value.ul * 4 / 3);
                        break;
                    default:
                        break;
                    }
                }

                msg->setStatus(flags);

                if (!senderName.isEmpty() || !senderAddress.isEmpty()) {
                    msg->setFrom(createAddress(senderName, senderAddress));
                    QMessagePrivate::setSenderName(*msg, senderName);
                }

                if (!isModified) {
                    msg->d_ptr->_modified = false;
                }
                result = true;

                MAPIFreeBuffer(properties);
            } else {
                *lastError = QMessageStore::ContentInaccessible;
            }

            mapiRelease(message);
        }
    }

    return result;
}

bool MapiSession::updateMessageRecipients(QMessageStore::ErrorCode *lastError, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.recipients) {
        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.recipients = 1;

        IMessage *message = openMapiMessage(lastError, msg->id());
        if (*lastError == QMessageStore::NoError) {
            // Extract the recipients for the message
            IMAPITable *recipientsTable(0);
            HRESULT rv = message->GetRecipientTable(0, &recipientsTable);
            if (HR_SUCCEEDED(rv)) {
                SizedSPropTagArray(3, rcpCols) = {3, { PR_DISPLAY_NAME, PR_EMAIL_ADDRESS, PR_RECIPIENT_TYPE}};

                QMessageAddressList to;
                QMessageAddressList cc;
                QMessageAddressList bcc;

                QueryAllRows qar(recipientsTable, reinterpret_cast<LPSPropTagArray>(&rcpCols), 0, 0);

                while(qar.query()) {
                    for (uint n = 0; n < qar.rows()->cRows; ++n) {
                        QMessageAddressList *list = 0;

                        SPropValue *props = qar.rows()->aRow[n].lpProps;
                        switch (props[2].Value.l) {
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
                            QString name;
                            QString address;

                            if (props[0].ulPropTag == PR_DISPLAY_NAME)
                                name = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[0].Value.LPSZ);
                            if (props[1].ulPropTag == PR_EMAIL_ADDRESS)
                                address = QStringFromLpctstr(qar.rows()->aRow[n].lpProps[1].Value.LPSZ);

                            if (!name.isEmpty() || ! address.isEmpty()) {
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

                if(qar.lastError() != QMessageStore::NoError)
                {
                    *lastError = qar.lastError();
                    result = false;
                }
                else
                    result = true;

                mapiRelease(recipientsTable);

            } else {
                *lastError = QMessageStore::ContentInaccessible;
            }

            mapiRelease(message);
        }
    }

    return result;
}

bool MapiSession::updateMessageBody(QMessageStore::ErrorCode *lastError, QMessage *msg) const
{
    bool result(false);

    // TODO: Signed messages are stored with the body as an attachment; we need to implement this

    if (!msg->d_ptr->_elementsPresent.body) {
        if (!msg->d_ptr->_elementsPresent.properties) {
            // We need the properties before we can fetch the body
            if (!updateMessageProperties(lastError, msg)) {
                return false;
            }
        }

        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.body = 1;

        QByteArray messageBody;
        QByteArray bodySubType;

        IMessage *message(0);

#ifdef _WIN32_WCE
        MapiEntryId storeRecordKey(QMessageIdPrivate::storeRecordKey(msg->id()));
#else
        MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(msg->id()));
#endif

        MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
        if (*lastError == QMessageStore::NoError) {
            MapiEntryId entryId(QMessageIdPrivate::entryId(msg->id()));

            message = mapiStore->openMessage(lastError, entryId);
            if (*lastError != QMessageStore::NoError) {
                qWarning() << "Invalid message entryId:" << entryId.toBase64();
            } else {
                IStream *is(0);
                LONG contentFormat(msg->d_ptr->_contentFormat);

                if (contentFormat == EDITOR_FORMAT_PLAINTEXT) {
                    HRESULT rv = message->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                    if (HR_SUCCEEDED(rv)) {
                        messageBody = readStream(lastError, is);
                        bodySubType = "plain";
                    }
                } else if (contentFormat == EDITOR_FORMAT_HTML) {
                    // See if there is a body HTML property
                    HRESULT rv = message->OpenProperty(PR_BODY_HTML, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                    if (HR_SUCCEEDED(rv)) {
                        messageBody = readStream(lastError, is);
                        bodySubType = "html";
                    }
                }

                if (bodySubType.isEmpty()) {
                    if (!msg->d_ptr->_rtfInSync) {
                        // See if we need to sync the RTF
#ifndef _WIN32_WCE
                        if (!mapiStore->supports(STORE_RTF_OK)) {
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
#endif
                    }

                    // Either the body is in RTF, or we need to read the RTF to know that it is text...
                    HRESULT rv = message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
                    if (HR_SUCCEEDED(rv)) {
#ifndef _WIN32_WCE
                        IStream *decompressor(0);
                        if (WrapCompressedRTFStream(is, 0, &decompressor) == S_OK) {
                            ULONG bytes = 0;
                            char buffer[BUFSIZ] = { 0 };
                            do {
                                decompressor->Read(buffer, BUFSIZ, &bytes);
                                messageBody.append(buffer, bytes);
                            } while (bytes == BUFSIZ);

                            decompressor->Release();

                            if (contentFormat == EDITOR_FORMAT_DONTKNOW) {
                                // Inspect the message content to see if we can tell what is in it
                                QString initialText = decodeContent(messageBody, "utf-16", 256);
                                if (!initialText.isEmpty()) {
                                    if (initialText.indexOf("\\fromtext") != -1) {
                                        // This message originally contained text
                                        contentFormat = EDITOR_FORMAT_PLAINTEXT;

                                        // See if we can get the plain text version instead
                                        IStream *ts(0);
                                        rv = message->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, 0, (IUnknown**)&ts);
                                        if (HR_SUCCEEDED(rv)) {
                                            messageBody = readStream(lastError, ts);
                                            bodySubType = "plain";

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
                                    messageBody = extractPlainText(decodeContent(messageBody, "utf-16"));
                                    bodySubType = "plain";
                                } else if (contentFormat == EDITOR_FORMAT_HTML) {
                                    QString html = extractHtml(decodeContent(messageBody, "utf-16"));
                                    messageBody = QTextCodec::codecForName("utf-16")->fromUnicode(html.constData(), html.length());
                                    bodySubType = "html";
                                }
                            }

                            if (bodySubType.isEmpty()) {
                                // I guess we must have RTF
                                bodySubType = "rtf";
                            }
                        } else {
                            *lastError = QMessageStore::ContentInaccessible;
                            qWarning() << "Unable to decompress RTF";
                            bodySubType = "plain";
                        }
#endif
                    } else {
                        bodySubType = "unknown";
                    }
                }

                mapiRelease(is);

                if (*lastError == QMessageStore::NoError) {
                    QMessageContentContainerPrivate *messageContainer(((QMessageContentContainer *)(msg))->d_ptr);

                    if (!msg->d_ptr->_hasAttachments) {
                        // Make the body the entire content of the message
                        messageContainer->setContent(messageBody, QByteArray("text"), bodySubType, QByteArray("utf-16"));
                        msg->d_ptr->_bodyId = QMessageContentContainerPrivate::bodyContentId();
                    } else {
                        // Add the message body data as the first part
                        QMessageContentContainer bodyPart;
                        {
                            QMessageContentContainerPrivate *bodyContainer(((QMessageContentContainer *)(&bodyPart))->d_ptr);
                            bodyContainer->setContent(messageBody, QByteArray("text"), bodySubType, QByteArray("utf-16"));
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
        } else {
            qWarning() << "Invalid store recordKey:" << storeRecordKey.toBase64();
        }
    }

    return result;
}

bool MapiSession::updateMessageAttachments(QMessageStore::ErrorCode *lastError, QMessage *msg) const
{
    bool result(false);

    if (!msg->d_ptr->_elementsPresent.attachments) {
        if (!msg->d_ptr->_elementsPresent.properties) {
            // We need the properties before we can fetch the attachments
            if (!updateMessageProperties(lastError, msg)) {
                return false;
            }
        }

        bool isModified(msg->d_ptr->_modified);
        msg->d_ptr->_elementsPresent.attachments = 1;

        if (msg->d_ptr->_hasAttachments) {
            IMessage *message = openMapiMessage(lastError, msg->id());
            if (*lastError == QMessageStore::NoError) {
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

                            messageContainer->appendContent(attachment);
                        }

                        if (!isModified) {
                            msg->d_ptr->_modified = false;
                        }
                    }

                    if(qar.lastError() != QMessageStore::NoError)
                        *lastError = qar.lastError();
                    else
                        result = true;

                    mapiRelease(attachmentsTable);
                }
                else
                    *lastError = QMessageStore::ContentInaccessible;

                mapiRelease(message);
            }
        }
    }

    return result;
}

QByteArray MapiSession::attachmentData(QMessageStore::ErrorCode *lastError, const QMessageId& id, ULONG number) const
{
    QByteArray result;

    IMessage *message = openMapiMessage(lastError, id);
    if (*lastError == QMessageStore::NoError) {
        LPATTACH attachment(0);
        HRESULT rv = message->OpenAttach(number, 0, 0, &attachment);
        if (HR_SUCCEEDED(rv)) {
            IStream *is(0);
            rv = attachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
            if (HR_SUCCEEDED(rv)) {
                result = readStream(lastError, is);
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

QMessageIdList MapiSession::queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    QList<FolderHeapNodePtr> folderNodes;

    foreach (QMessageFilter subFilter, QMessageFilterPrivate::subFilters(filter)) {
        MapiStoreIterator storeIt(QMessageFilterPrivate::storeIterator(subFilter, lastError, _self.toStrongRef()));
        for (MapiStorePtr store(storeIt.next()); store && store->isValid(); store = storeIt.next()) {
            MapiFolderIterator folderIt(QMessageFilterPrivate::folderIterator(subFilter, lastError, store));
            for (MapiFolderPtr folder(folderIt.next()); folder && folder->isValid(); folder = folderIt.next()) {
                QList<QMessageFilter> orderingFilters;
                orderingFilters.append(subFilter);
                foreach(QMessageFilter orderingFilter, QMessageOrderingPrivate::normalize(orderingFilters, ordering)) {
                    folderNodes.append(FolderHeapNodePtr(new FolderHeapNode(folder, orderingFilter)));
                }
            }
        }
    }

    if (*lastError != QMessageStore::NoError)
        return QMessageIdList();

    return filterMessages(lastError, folderNodes, ordering, limit, offset);
}

void MapiSession::updateMessage(QMessageStore::ErrorCode* lastError, const QMessage& source)
{
    IMessage *mapiMessage = openMapiMessage(lastError, source.id());
    if (*lastError == QMessageStore::NoError) {
        // Update the stored properties
        if (*lastError == QMessageStore::NoError) {
            storeMessageProperties(lastError, source, mapiMessage);
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageRecipients(lastError, source, mapiMessage, _mapiSession);
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageBody(lastError, source, mapiMessage);
        }
        if (*lastError == QMessageStore::NoError) {
            replaceMessageAttachments(lastError, source, mapiMessage);
        }
        if (*lastError == QMessageStore::NoError) {
#ifndef _WIN32_WCE //unsupported
            if (HR_FAILED(mapiMessage->SaveChanges(0))) {
                qWarning() << "Unable to save changes for message.";
            }
#endif
        }

        mapiRelease(mapiMessage);
    }
}

void MapiSession::removeMessages(QMessageStore::ErrorCode *lastError, const QMessageIdList &ids)
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

        QMessageStore::ErrorCode error(QMessageStore::NoError);

#ifdef _WIN32_WCE
        MapiStorePtr store = openStore(&error,storeKey,true);
#else
        MapiStorePtr store = openStoreWithKey(&error, storeKey, true);
#endif
        if (error == QMessageStore::NoError) {
#ifdef _WIN32_WCE
            MapiFolderPtr folder = store->openFolder(&error, folderKey);
#else
            MapiFolderPtr folder = store->openFolderWithKey(&error, folderKey);
#endif
            if (error == QMessageStore::NoError) {
                folder->removeMessages(&error, it.value());
            }
        }

        if ((error != QMessageStore::NoError) && (*lastError == QMessageStore::NoError)) {
            *lastError = error;
        }
    }
}

bool MapiSession::showForm(IMessage* message, IMAPIFolder* folder, LPMDB store)
{
    ULONG messageToken;

    if(_mapiSession->PrepareForm(NULL,message, &messageToken )== S_OK)
    {
        ULONG messageStatus = 0;
        LPSPropValue pProp = 0;
        ULONG propertyCount = 0;
        ULONG p[2]={ 1,PR_MSG_STATUS};

        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageStatus = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        ULONG messageFlags = 0;
        p[1] = PR_MESSAGE_FLAGS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageFlags = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        ULONG messageAccess = 0;
        p[1] = PR_ACCESS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageAccess = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        propertyCount = 0;
        p[1] = PR_MESSAGE_CLASS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
#ifdef UNICODE
            char szMessageClass[256];
            WideCharToMultiByte(CP_ACP, 0, pProp->Value.LPSZ,-1, szMessageClass,255, NULL, NULL);
#else
            char* szMessageClass=pProp->Value.LPSZ;
#endif
            HRESULT hr=_mapiSession->ShowForm(NULL, store, folder, NULL,messageToken, NULL, 0,messageStatus, messageFlags & MAPI_NEW_MESSAGE, messageAccess, szMessageClass);
            MAPIFreeBuffer(pProp);
            if(hr==MAPI_E_USER_CANCEL)
            {
                qWarning() << "Show form cancelled";
                return false;
            }
            else if(hr!=S_OK)
            {
                qWarning() << "Show form failed";
                return false;
            }
        }
        else
        {
            qWarning() << "Failed to show form";
            return false;
        }
    }
    return true;
}

bool MapiSession::event(QEvent *e)
{
    if (e->type() == NotifyEvent::eventType()) {
        if (NotifyEvent *ne = static_cast<NotifyEvent*>(e)) {
            notify(ne->_store, ne->_id, ne->_notifyType);
            return true;
        }
    }

    return QObject::event(e);
}

void MapiSession::notify(MapiStore *store, const QMessageId &id, MapiSession::NotifyType notifyType)
{
    QMessageStore::NotificationFilterIdSet matchingFilterIds;

    QMap<QMessageStore::NotificationFilterId, QMessageFilter>::const_iterator it = _filters.begin(), end = _filters.end();
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());

        QSet<QMessageId> filteredIds;
        if (!filter.isEmpty()) {
            // Empty filter matches all messages; otherwise get the filtered set
            QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
            filteredIds = store->queryMessages(&ignoredError, filter, QMessageOrdering(), 0, 0).toSet();
        }
        if (filter.isEmpty() || filteredIds.contains(id)) {
            matchingFilterIds.insert(it.key());
        }
    }

    if (!matchingFilterIds.isEmpty()) {
        void (MapiSession::*signal)(const QMessageId &, const QMessageStore::NotificationFilterIdSet &) =
            ((notifyType == Added) ? &MapiSession::messageAdded
                                   : ((notifyType == Removed) ? &MapiSession::messageRemoved
                                                              : &MapiSession::messageUpdated));
        emit (this->*signal)(id, matchingFilterIds);
    }
}

QMessageStore::NotificationFilterId MapiSession::registerNotificationFilter(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter)
{
    QMessageStore::NotificationFilterId filterId = ++_filterId;
    _filters.insert(filterId, filter);

    if (!_registered) {
        _registered = true;

        foreach (MapiStorePtr store, allStores(lastError)) {
            store->notifyEvents(fnevNewMail | fnevObjectCreated | fnevObjectCopied | fnevObjectDeleted | fnevObjectModified | fnevObjectMoved);
        }
    }

    return filterId;
}

void MapiSession::unregisterNotificationFilter(QMessageStore::ErrorCode *lastError, QMessageStore::NotificationFilterId filterId)
{
    _filters.remove(filterId);

    Q_UNUSED(lastError)
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

#include "winhelpers.moc"
