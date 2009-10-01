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

#ifndef QVERSITPROPERTY_H
#define QVERSITPROPERTY_H

#include <QVariant>
#include <QList>
#include <QString>
#include <QByteArray>

class QVersitDocument;
class QVersitPropertyPrivate;
class QVersitProperty
{
public:
    QVersitProperty() {}
    ~QVersitProperty() {}

    // might not be required!
    //QVersitDocument::VersitType versitType() const;

    enum Error {
        InvalidKey = 0,
        InvalidTypes,
        InvalidValueType,
        IncompatibleData,
        IncompatibleDetail,
        InvalidEncoding,
        InvalidFormat
    };

    virtual Error error() const;

    enum BinaryEncoding {
        // whatever values required here.
        Base64 = 0,
        Uucode,
    };

    enum CharacterEncoding {
        // whatever values required here.
        Latin1 = 0,
        Utf8,
        Utf16,
        Utf32
    };

    enum ImageFormat {
        // whatever values required here.
        Jpeg = 0,
        Bitmap,
        Gif,
        Png,
        Svg
    };

    QString key() const;
    QStringList types() const;
    QByteArray value() const;

    // XXX virtual QPair<QVariant, Encoding> value() const;

    void setKey(const QString& key);         // TEL,ADR,FN,etc
    void setTypes(const QStringList& types); // TYPE=home,work,pref,voice,cell,intl,dom,post,etc
    void setValue(const QString& value, CharacterEncoding encoding);
    void setValue(const QByteArray& value, BinaryEncoding encoding);
    void setValue(const QImage& value, ImageFormat format);

    // note that some properties can have an embedded versit property (or versit document) (eg AGENT)
    void setValue(const QVersitDocument& embedded);

    QStringList validKeys() const;
    QStringList validTypes(const QString& key = QString()) const;
    QVariant::Type valueType() const;

private:
    QVersitPropertyPrivate* d;
};

#endif
