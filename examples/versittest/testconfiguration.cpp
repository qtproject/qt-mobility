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

#include "testconfiguration.h"

// XML elements
const char iterationsElement[] = "test_iterations";
const char savingElement[] = "save_contacts";
const char outputElement[] = "output_format";

// XML attriputes
const char valueAttr[] = "value";

// XML values
const char valueYes[] = "yes";


TestConfiguration::TestConfiguration()
: mSaveContact(true),
  mOutputFormat(QString::fromAscii("xml")),
  mIterationCount(1)
{    
}

TestConfiguration::~TestConfiguration()
{
}

int TestConfiguration::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    return reader.parse(inputSource);
}

bool TestConfiguration::startElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName,
    const QXmlAttributes& atts)
{
    QString value = atts.value(QString::fromAscii(valueAttr));
    if (qName == iterationsElement) {
        mIterationCount = value.toInt();
    } else if (qName == savingElement) {
        mSaveContact = (value.compare(valueYes,Qt::CaseInsensitive) == 0);
    } else if (qName == outputElement) {
        mOutputFormat = value;
    } else {
        // NOP
    }
    return true;
}

// End of File.
