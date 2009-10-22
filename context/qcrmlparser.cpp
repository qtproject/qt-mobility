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

#include "qcrmlparser_p.h"
#include <QDomDocument>
#include <QDomElement>
#include <QStringList>
#include <QFile>

KeyData::KeyData(const QString &path, quint64 uid, quint32 bitIndex)
{
    m_path = path;
    m_UID = uid;
    m_bitIndex = bitIndex;
}

void QCRMLParser::setError(ErrorType type, const QString &errorString)
{
    m_error = type;
    m_errorString = errorString;
}

QCRMLParser::ErrorType QCRMLParser::error()
{
    return m_error;
}

QString QCRMLParser::errorString()
{
    return m_errorString;
}

QList<KeyData> QCRMLParser::parseQCRML(const QString &filePath)
{
    QList<KeyData> rv;
    QFile inputFile(filePath);

    if (!inputFile.exists()) {
        setError(FileDoesNotExist, "QCRML file does not exist");
        return rv;
    }

    QDomDocument doc;
    QString errorStr;
    if (!doc.setContent(&inputFile, false, &errorStr)) {
        setError(ParseError, errorStr);
        return rv;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "repository") {
        setError(ParseError, "Root element is not a repository element");
        return rv;
    }

    if(!parseRepository(root))
        return rv;

    QDomElement element = root.firstChildElement();

    QList<KeyData> keyData;
    while(!element.isNull()) {
        if (element.tagName() == "key")
            keyData = parseKey(element);
        else if (element.tagName() == "keyRange") {
            keyData = parseKeyRange(element);
        } else { //unknown element so ignore and continue
            element = element.nextSiblingElement();
            continue;
        }

        if (error() != NoError) {
            rv.clear();
            return rv;
        }
        m_keyData.append(keyData);
        element = element.nextSiblingElement();
    }

    setError(NoError, "");
    return m_keyData;
}

bool QCRMLParser::parseRepository(const QDomElement &element)
{
    m_repoUID = 0;

    QStringList mandatoryAttributes;
    mandatoryAttributes << "uidName" << "uidValue";

    if (!checkMandatoryAttributes(element, mandatoryAttributes))
        return false;

    QString uidValueStr = element.attribute("uidValue");
    bool ok = false;
    quint32 uidValue = uidStringToUInt32(uidValueStr, &ok);
    if (!ok) {
        setError(ParseError, "repository element has invalid uidValue");
        return false;
    }

    QString uidName = element.attribute("uidName");
    if (uidName.isEmpty()) {
        setError(ParseError, "repository element has invalid uidName");
        return false;
    }

    m_repoUID = uidValue;
    return true;
}

QList<KeyData> QCRMLParser::parseKeyRange(const QDomElement &element)
{
    QList<KeyData> rv;

    //if keyRange has no ref attribute it must
    //only be used for creating access control
    //policies which we do not need to worry about
    if (!element.hasAttribute("ref"))
        return rv;

    QStringList mandatoryAttributes;
    mandatoryAttributes << "firstInt" << "lastInt";
    if (!checkMandatoryAttributes(element, mandatoryAttributes))
        return rv;

    bool ok = false;
    QString pathPrefix;
     pathPrefix = element.attribute("ref");
     if (!pathPrefix.startsWith("/"))
        pathPrefix.prepend("/");

    if (element.hasAttribute("countInt")) {
        quint32 countInt = uidStringToUInt32(element.attribute("countInt"), &ok);
        if (!ok) {
            setError(ParseError, "keyRange element has invalid countIn attribute");
            rv.clear();
            return rv;
        }

        rv.append(KeyData(pathPrefix,(quint64)countInt + (((quint64)m_repoUID) << 32)));
    }

     if (!pathPrefix.endsWith("/"))
        pathPrefix.append("/");

    quint32 firstInt = uidStringToUInt32(element.attribute("firstInt"), &ok);
    if (!ok) {
        setError(ParseError, "keyRange element has invalid firstInt attribute");
        rv.clear();
        return rv;
    }

    quint32 lastInt = uidStringToUInt32(element.attribute("lastInt"),&ok);
    if (!ok) {
        setError(ParseError, "keyRange element has invalid lastInt attribute");
        rv.clear();
        return rv;
    }

    quint32 maxNum;
    quint32 indexBits;
    quint32 firstIndex;
    if (!element.hasAttribute("indexBits")) {
        //keyRange doesn't map to sequence setting

        maxNum = lastInt - firstInt + 1;
        for (quint32 i=0; i < maxNum; i++) {
            m_keyData.append(KeyData(pathPrefix + i,(quint64)firstInt + (((quint64)m_repoUID) << 32) + i));
        }
    } else {
        //keyRanges does  map to sequence setting
        indexBits = uidStringToUInt32(element.attribute("indexBits"), &ok);
        if (!ok) {
            setError(ParseError, "keyRange elment has invalid indexBits attribute");
            rv.clear();
            return rv;
        }

        QString firstIndexStr = element.attribute("firstIndex");
        firstIndex = firstIndexStr.toUInt(&ok, 10);
        if (!ok) {
            setError(ParseError, "keyRange element has invalid firstIndex attribute");
            rv.clear();
            return rv;
        }

        int indexBitsLSB =0;
        quint32 bitmask = 1;
        while ( (bitmask & indexBits) == 0) {
            bitmask = bitmask << 1;
            bitmask +=1;
            indexBitsLSB+=1;
        }

        maxNum =( ((lastInt - firstInt) & indexBits) >> indexBitsLSB) + 1 - firstIndex;

        int indexBitsMSB=31;
        bitmask = 0x80000000;
        while ((bitmask & indexBits) == 0) {
            bitmask = bitmask >> 1;
            bitmask += 0x80000000;
            indexBitsMSB -=1;
        }
        bitmask = bitmask << 1;
        quint32 settingIdentifier = lastInt & bitmask;

        QDomNodeList children = element.childNodes();
        QList<KeyData> subSettings;

        for (int i=0; i < children.count(); i++) {
            if (children.at(i).toElement().tagName() == "key") {
                subSettings.append(parseKey(children.at(i).toElement()));
                if ( error() != NoError) {
                    rv.clear();
                    return rv;
                }
            }
        }

        for(quint32 i = 0; i < maxNum; i++) {
            for(int j = 0; j < subSettings.count(); j++) {
                rv.append(KeyData(pathPrefix + QString::number(i) + subSettings.at(j).path(),
                                 subSettings.at(j).UID() + settingIdentifier + ((firstIndex + 1*i)  << indexBitsLSB),
                                 subSettings.at(j).bitIndex()));
            }
        }
    }

    return rv;
}

QList<KeyData> QCRMLParser::parseKey(const QDomElement &element)
{
    QList<KeyData> rv;

    QStringList mandatoryAttributes;
    mandatoryAttributes << "int";
    if (!checkMandatoryAttributes(element, mandatoryAttributes))
        return rv;

    QString keyIntStr = element.attribute("int");
    bool ok =false;
    quint32 keyInt = uidStringToUInt32(keyIntStr, &ok);
    if (!ok) {
        setError(ParseError, "key element has invalid int attribute");
        return rv;
    }

    if (!element.hasAttribute("ref")) {
        QDomNodeList children = element.childNodes();
        QDomElement childElement;
        QString text;
        QString keyPath;
        bool ok;
        for(int i = 0; i < children.count(); ++i) {
            childElement = children.at(i).toElement();
            if (childElement.tagName() == "bit") {
                mandatoryAttributes.clear();
                mandatoryAttributes << "ref";
                if (!checkMandatoryAttributes(childElement, mandatoryAttributes)) {
                    rv.clear();
                    return rv;
                }
                text = childElement.text();
                int bitIndex = text.toInt(&ok);
                if (!ok || bitIndex <= 0) {
                    //Note: binary keys have no maximum bit index
                    setError(ParseError, "bit element has invalid text value");
                    rv.clear();
                    return rv;
                }
                keyPath = childElement.attribute("ref");
               if (!keyPath.startsWith("/"))
                    keyPath.prepend("/");
                quint64 uid = m_repoUID;
                uid = uid << 32;
                uid += keyInt;
                rv.append(KeyData(keyPath,uid, bitIndex));
            }
        }
    } else {
        QString keyRef = element.attribute("ref");
        if (keyRef.isEmpty()) {
            setError(ParseError, "ref attribute of key element is empty");
            rv.clear();
            return rv;
        }

        QString keyPath(keyRef);
        if (!keyPath.startsWith("/"))
            keyPath.prepend("/");
        quint64 uid = m_repoUID;
        uid = uid << 32;
        uid += keyInt;
        rv.append(KeyData(keyPath, uid));
}

    setError(NoError,"");
    return rv;
}

quint32 QCRMLParser::uidStringToUInt32(const QString &uidString, bool *ok)
{
    quint32 uid = 0;
    if (!uidString.startsWith("0x")) {
        if (ok != NULL)
            *ok = false;
         return 0;
    }

    bool isOk = false;
    uid =  uidString.toUInt(&isOk, 16);
    if (!isOk) {
        if (ok !=NULL)
            *ok =false;
        return 0;
    }

    if (ok != NULL)
        *ok = true;
    return uid;
}

bool QCRMLParser::checkMandatoryAttributes(const QDomElement &element, const QStringList &mandatoryAttributes)
{
    foreach(const QString &attribute, mandatoryAttributes) {
        if (!element.hasAttribute(attribute)) {
            QString errorString("%1 element does not contain %2 attribute");
            errorString = errorString.arg(element.tagName()).arg(attribute);
            setError(ParseError, errorString);
            return false;
        }
    }
    return true;
}
