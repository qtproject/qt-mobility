/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "testconfiguration.h"
#include <stdio.h>

// XML configuration elements
const char setupElement[] = "setup";
const char contactElement[] = "contact";
const char testElement[] = "test";
const char logElement[] = "log";

// XML configuration attriputes
const char nameAttr[] = "name";
const char saveAttr[] = "save";
const char typeAttr[] = "type";
const char loggingAttr[] = "logging";

// XML configuration values
const char valueYes[] = "yes";
const char valuePerformanceTest[] = "0";


// -----------------------------------------------------------------------------
// TestConfiguration::TestConfiguration
// -----------------------------------------------------------------------------
//
TestConfiguration::TestConfiguration()
: mSaveContact(true),
  mPerformanceTest(true),
  mLogToFile(false)
{    
}

// -----------------------------------------------------------------------------
// TestConfiguration::TestConfiguration
// -----------------------------------------------------------------------------
//
TestConfiguration::~TestConfiguration()
{
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::parse
// -----------------------------------------------------------------------------
//
int TestConfiguration::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    return reader.parse(inputSource);
}

// -----------------------------------------------------------------------------
// TestConfiguration::startElement
// -----------------------------------------------------------------------------
//
bool TestConfiguration::startElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName,
    const QXmlAttributes& atts)
{
    if( qName == setupElement) {
        mTestName = atts.value(nameAttr);
    }
    else if (qName == contactElement) {
        mSaveContact = atts.value(saveAttr).contains(valueYes,
                                                     Qt::CaseInsensitive);
    }
    else if (qName == testElement) {
        mPerformanceTest = atts.value(typeAttr).contains(valuePerformanceTest,
                                                         Qt::CaseInsensitive);
    }
    else if (qName == logElement) {
        mLogToFile = atts.value(loggingAttr).contains(valueYes,
                                                      Qt::CaseInsensitive);
    }
    return true;
}

// End of File.
