/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QStringList>
#include <QFile>
#include <QXmlStreamAttributes>

QTM_BEGIN_NAMESPACE

KeyData::KeyData(const QString &path, quint64 uid, Target target, quint32 bitIndex)
{
    m_path = path;
    m_UID = uid;
    m_target = target;
    m_bitIndex = bitIndex;
}

QList<KeyData> QCrmlParser::parseQCrml(const QString &filePath)
{
    QList<KeyData> rv;
    QFile inputFile(filePath);

    if (!inputFile.exists()) {
        setError(FileNotFound, QObject::tr("File does not exist: %1").arg(filePath));
        return rv;
    }

    if (!inputFile.open(QFile::ReadOnly)) {
        setError(FileOpenError, QObject::tr("Error opening file: %1").arg(filePath));
    }

    setDevice(&inputFile);

    readNext();
    if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
        setError(ParseError, QXmlStreamReader::errorString());
        rv.clear();
        return rv;
    }

    if(isStartDocument()) {
        readNext();
        if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
            setError(ParseError, QXmlStreamReader::errorString());
            rv.clear();
            return rv;
        }
    }

    if (isStartElement()) {
        if (name() == "repository") {
            rv = parseRepository();
        } else {
            setError(ParseError, QObject::tr("root element is not a repository element"));
        }
    }
    return rv;
}

QList<KeyData> QCrmlParser::parseRepository()
{
    QList<KeyData> rv;
    QStringList mandatoryAttributes;
    mandatoryAttributes << QLatin1String("uidValue");
    setError(NoError, QString());
    if (!checkMandatoryAttributes(mandatoryAttributes))
        return rv;

    bool ok;
    quint32 uidValue =
        uidStringToUInt32(attributes().value(QLatin1String("uidValue")).toString(), &ok);
    if (!ok) {
        setError(ParseError, QObject::tr("repository element has invalid uidValue on line %1")
                               .arg(QString::number(lineNumber())));
        return rv;
    }

    QString targetStr = attributes().value(QLatin1String("target")).toString();
    if (targetStr.isEmpty() || targetStr == QLatin1String("CRepository")) {
        m_target = KeyData::CRepository;
    } else if (targetStr == QLatin1String("RProperty")) {
        m_target = KeyData::RProperty;
    } else {
        setError(ParseError, QObject::tr("repository element has unrecognised target attribute "
                                        "on line %1, attribute must be CRepository, RProperty or "
                                        "be left undefined").arg(QString::number(lineNumber())));
        return rv;
    }

    while (!atEnd())
    {
        readNext();
         if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
            setError(ParseError, QXmlStreamReader::errorString());
            rv.clear();
           return rv;
        }

        if (isEndElement()  && name() == "repository")
            break;

        if (isStartElement()) {
            if (name() == "key")
                rv.append(parseKey(uidValue));
            else if (name() == "keyRange")
                rv.append(parseKeyRange(uidValue));
            else
                parseUnknownElement();
        }

        if (m_error != NoError) {
            rv.clear();
            break;
        }
    }

    if (!isEndElement() && name() != "repository") {
        setError(ParseError, QObject::tr("File did not end with a repository end tag"));
        rv.clear();
        return rv;
    }

    return rv;

}

QList<KeyData> QCrmlParser::parseKey(quint32 repoUid)
{
    QList<KeyData> rv;
    QStringList mandatoryAttributes;
    mandatoryAttributes << QLatin1String("int");
    if (!checkMandatoryAttributes(mandatoryAttributes))
        return rv;

    QXmlStreamAttributes attribs = attributes();
    QString keyIntStr = attribs.value(QLatin1String("int")).toString();
    bool ok =false;
    quint32 keyInt = uidStringToUInt32(keyIntStr, &ok);
    if (!ok) {
        setError(ParseError,QObject::tr("key element has invalid int attribute on line %1").
                arg(QString::number(lineNumber())));
        return rv;
    }

    if (attribs.value(QLatin1String("ref")).isNull()) {
        //no ref attribute so this must be
        //a bitmask key
        while (!atEnd()) {
            readNext();
            if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
                 setError(ParseError, QXmlStreamReader::errorString());
                 rv.clear();
                 return rv;
            }
            if (isEndElement())
                break;

            if (isStartElement()) {
                if (name() == "bit") {
                    rv.append(parseBit(repoUid, keyInt));
                } else {
                    parseUnknownElement();
                }
            }
        }
    } else {
        QString keyRef = attribs.value(QLatin1String("ref")).toString();
        if (keyRef.isEmpty()) {
            setError(ParseError, QObject::tr("ref attribute of key element is empty on line %1")
                    .arg(QString::number(lineNumber())));
            rv.clear();
            return rv;
        }

        while (!atEnd()) {
            readNext();
            if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
                setError(ParseError, QXmlStreamReader::errorString());
                rv.clear();
                return rv;
            }
            if (isEndElement() && name() == "key") {
                break;
            }

            if (isStartElement()) {
                if (name() == "key" || name() == "keyRange") {
                    setError(ParseError, QObject::tr("key and/or keyRange element has "
                                "been nested in a key element on line %1").arg(QString::number(lineNumber())));
                    rv.clear();
                    return rv;
                } else {
                    parseUnknownElement();
                }
            }
        }

        QString keyPath(keyRef);
        if (!keyPath.startsWith(QLatin1Char('/')))
            keyPath.prepend(QLatin1Char('/'));
        quint64 uid = repoUid;
        uid = uid << 32;
        uid += keyInt;
        rv.append(KeyData(keyPath, uid,m_target));
    }
    return rv;
}

QList<KeyData> QCrmlParser::parseKeyRange(quint32 repoUid)
{
    QList<KeyData> rv;

    //if keyRange has no ref attribute it must
    //only be used for creating access control
    //policies which we do not need to worry about
    if (attributes().value(QLatin1String("ref")).isNull())
        return rv;

    QStringList mandatoryAttributes;
    mandatoryAttributes << QLatin1String("firstInt") << QLatin1String("lastInt");
    if (!checkMandatoryAttributes(mandatoryAttributes))
        return rv;

    bool ok = false;
    QString pathPrefix;
    pathPrefix = attributes().value(QLatin1String("ref")).toString();
    if (!pathPrefix.startsWith(QLatin1Char('/')))
        pathPrefix.prepend(QLatin1Char('/'));

    if (!attributes().value(QLatin1String("countInt")).isNull()) {
        quint32 countInt =
            uidStringToUInt32(attributes().value(QLatin1String("countInt")).toString(), &ok);
        if (!ok) {
            setError(ParseError, QObject::tr("keyRange element has invalid countInt attribute on line %1")
                    .arg(QString::number(lineNumber())));
            rv.clear();
            return rv;
        }

        rv.append(KeyData(pathPrefix,(quint64)countInt + (((quint64)repoUid) << 32), m_target));
    }

    if (!pathPrefix.endsWith(QLatin1Char('/')))
        pathPrefix.append(QLatin1Char('/'));

    quint32 firstInt =
        uidStringToUInt32(attributes().value(QLatin1String("firstInt")).toString(), &ok);
    if (!ok) {
        setError(ParseError, QObject::tr("keyRange element has invalid firstInt attribute on line %1")
                .arg(QString::number(lineNumber())));
        rv.clear();
        return rv;
    }

    quint32 lastInt =
        uidStringToUInt32(attributes().value(QLatin1String("lastInt")).toString(),&ok);
    if (!ok) {
        setError(ParseError, QObject::tr("keyRange element has invalid lastInt attribute on line %1")
                .arg(QString::number(lineNumber())));
        rv.clear();
        return rv;
    }

    quint32 maxNum =0;
    quint32 indexBits = 0;
    quint32 firstIndex = 0;
    if (attributes().value(QLatin1String("indexBits")).isNull()) {
        //keyRange doesn't map to sequence setting

        maxNum = lastInt - firstInt + 1;
        for (quint32 i=0; i < maxNum; i++) {
            rv.append(KeyData(pathPrefix + QString::number(i),
                                (quint64)firstInt + (((quint64)repoUid) << 32) + i,
                                m_target));
        }

        while (!atEnd()) {
            readNext();
            if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
                setError(ParseError, QXmlStreamReader::errorString());
                rv.clear();
                return rv;
            }

            if (isEndElement())
                break;

            if (isStartElement())
                parseUnknownElement();
        }
    } else {
        //keyRanges does  map to sequence setting
        indexBits =
            uidStringToUInt32(attributes().value(QLatin1String("indexBits")).toString(), &ok);
        if (!ok) {
            setError(ParseError, QObject::tr("keyRange elment has invalid indexBits attribute on line %1")
                    .arg(QString::number(lineNumber())));
            rv.clear();
            return rv;
        }

        if (!attributes().value(QLatin1String("firstIndex")).isNull()) {
            QString firstIndexStr = attributes().value(QLatin1String("firstIndex")).toString();
            firstIndex = firstIndexStr.toUInt(&ok, 10);
            if (!ok) {
                setError(ParseError, QObject::tr("keyRange element has invalid firstIndex attribute on line %1")
                        .arg(QString::number(lineNumber())));
                rv.clear();
                return rv;
            }
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

        QList<KeyData> subSettings;

        while (!atEnd()) {
            readNext();
            if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
                setError(ParseError, QXmlStreamReader::errorString());
                rv.clear();
                return rv;
            }

            if (isEndElement())
                break;

            if (isStartElement()) {
                if (name() == "key") {
                    subSettings.append(parseKey(repoUid));

                    if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
                        rv.clear();
                        return rv;
                    }
                } else {
                    parseUnknownElement();
                }
            }

        }

        for(quint32 i = 0; i < maxNum; i++) {
            for(int j = 0; j < subSettings.count(); j++) {
                rv.append(KeyData(pathPrefix + QString::number(i) + subSettings.at(j).path(),
                                 subSettings.at(j).uid() + settingIdentifier + ((firstIndex + 1*i)  << indexBitsLSB),
                                 m_target, subSettings.at(j).bitIndex()));
            }
        }
    }

    return rv;
}

QList<KeyData> QCrmlParser::parseBit(quint32 repoUid, quint32 keyInt)
{
    QList <KeyData> rv;
    QStringList mandatoryAttributes;
    mandatoryAttributes << QLatin1String("ref");
    if (!checkMandatoryAttributes(mandatoryAttributes)) {
        rv.clear();
        return rv;
    }

    QString keyPath = attributes().value(QLatin1String("ref")).toString();
    if (!keyPath.startsWith(QLatin1Char('/')))
        keyPath.prepend(QLatin1Char('/'));

    int bitIndex = 0;
    while(!atEnd()) {
        readNext();
        if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
            setError(ParseError, QXmlStreamReader::errorString());
            rv.clear();
            return rv;
        }

        if (isEndElement() && name() == "bit")
            break;
        if(isStartElement()) {
            parseUnknownElement();
            if (error() != NoError) {
                rv.clear();
                return rv;
            }
        } else if (isCharacters()) {
            bool ok;
            QString txt = text().toString();
            if (txt.simplified().isEmpty())
                continue;
            bitIndex = txt.toInt(&ok);
            if (!ok || bitIndex <= 0) {
                //Note: binary keys have no maximum bit index
                setError(ParseError, QObject::tr("bit element has invalid text value on line %1")
                        .arg(QString::number(lineNumber())));
                rv.clear();
                return rv;
            }
        }
    }

    if (!isEndElement() && name() !="bit") {
        setError(ParseError, QObject::tr("bit element does not have end tag at line: %1")
                            .arg(QString::number(lineNumber())));
        rv.clear();
        return rv;
    }

    quint64 uid = repoUid;
    uid = uid << 32;
    uid += keyInt;
    rv.append(KeyData(keyPath,uid, m_target, bitIndex));
    return rv;
}

void QCrmlParser::parseUnknownElement()
{
    Q_ASSERT(isStartElement());
    while(!atEnd()) {
        readNext();

        if (QXmlStreamReader::error() != QXmlStreamReader::NoError) {
            setError(ParseError, QXmlStreamReader::errorString());
            return;
        }

        if (isEndElement()) {
            break;
        }

        if (isStartElement())
            parseUnknownElement();
    }
}

bool QCrmlParser::checkMandatoryAttributes(const QStringList &mandatoryAttributes)
{
    QXmlStreamAttributes attrs= attributes() ;
    for (int i = 0; i < mandatoryAttributes.count(); ++i) {
        if (attrs.value(mandatoryAttributes.at(i)).isNull()) {
            setError(ParseError, QObject::tr("%1 element does not contain %2 attribute")
                    .arg(name().toString()).arg(mandatoryAttributes.at(i)));
            return false;
        }
    }
    return true;
}

QCrmlParser::Error QCrmlParser::error()
{
    return m_error;
}

QString QCrmlParser::errorString()
{
    return m_errorString;
}

void QCrmlParser::setError(Error error, const QString &errorString)
{
    m_error = error;
    m_errorString = errorString;
}

quint32 QCrmlParser::uidStringToUInt32(const QString &uidString, bool *ok)
{
    quint32 uid = 0;
    if (!uidString.startsWith(QLatin1String("0x"))) {
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

QTM_END_NAMESPACE
