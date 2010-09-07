/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCRMLPARSER_H
#define QCRMLPARSER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>
#include <QList>
#include <QString>
#include <QXmlStreamReader>

QTM_BEGIN_NAMESPACE

class KeyData{
public:
    enum Target{CRepository, RProperty};

    KeyData():m_UID(0), m_bitIndex(0), m_target(RProperty){}
    KeyData(const QString &path, quint64 UID,
            Target target, quint32 bitmask=0);
    quint64 uid() const {return m_UID;}
    quint32 repoId() const { return (quint32)(m_UID >> 32);}
    quint32 keyId() const {return (quint32)m_UID;}
    quint32 bitIndex() const { return m_bitIndex;}
    QString path()const {return m_path;}
    Target target() const {return m_target;}

private:
    quint64 m_UID;
    QString m_path;
    quint32 m_bitIndex;
    Target m_target;
};

class QCrmlParser : private QXmlStreamReader
{
public:
    enum Error{NoError, FileNotFound, FileOpenError, ParseError};
    QList<KeyData> parseQCrml(const QString &filePath);
    Error error();
    QString errorString();
private:
    void parseUnknownElement();
    QList<KeyData> parseRepository();
    QList<KeyData> parseKey(quint32 repoUid);
    QList<KeyData> parseKeyRange(quint32 repoUid);
    QList<KeyData> parseBit(quint32 repoUid, quint32 keyInt);

    bool checkMandatoryAttributes(const QStringList &mandatoryAttributes);
    quint32 uidStringToUInt32(const QString &uidString, bool*ok);
    void setError(Error error, const QString &errorString);

    Error m_error;
    QString m_errorString;
    KeyData::Target m_target;
};

QTM_END_NAMESPACE

#endif


