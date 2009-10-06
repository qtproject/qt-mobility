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

#include "testresultxmlparser.h" 
#include <stdio.h>

const char testFunctionElement[] = "TestFunction";
const char incidentElement[] = "Incident";
const char descriptionElement[] = "Description";
const char nameAttr[] = "name";
const char typeAttr[] = "type";
const char fileAttr[] = "file";
const char lineAttr[] = "line";
const char attrValueFail[] = "fail";


// -----------------------------------------------------------------------------
// TestResultXmlParser::TestResultXmlParser
// -----------------------------------------------------------------------------
//
TestResultXmlParser::TestResultXmlParser() 
: mTestCount(0), 
  mParsingIncidentElement(false),
  mParsingDescriptionElement(false),
  mCurrentTestFailed(false),
  mCurrentTestFailureLine(0)
{
    mErrors = new QStringList;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::TestResultXmlParser
// -----------------------------------------------------------------------------
//
TestResultXmlParser::~TestResultXmlParser()
{
    delete mErrors;   
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::startElement
// -----------------------------------------------------------------------------
//
bool TestResultXmlParser::startElement(
    const QString& /*namespaceURI*/, 
    const QString& /*localName*/, 
    const QString& qName, 
    const QXmlAttributes& atts)
{
    if (qName == testFunctionElement) {
        mTestCount++;
        mCurrentTestName = atts.value(nameAttr);
        return true;
    }
    if (qName == incidentElement) {
        mParsingIncidentElement = true;
        if (atts.value(typeAttr) == attrValueFail) {
            mCurrentTestFailed = true;
            mCurrentTestFile = atts.value(fileAttr);
            mCurrentTestFailureLine = atts.value(lineAttr).toInt();
        }
        return true;
    }
    mParsingDescriptionElement = (qName == descriptionElement);
    return true;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::endElement
// -----------------------------------------------------------------------------
//
bool TestResultXmlParser::endElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName)
{
    if (qName == incidentElement) {
        mParsingIncidentElement = false;
        mCurrentTestFailed = false;
        return true;
    }
    if (qName == descriptionElement) {
        mParsingDescriptionElement = false;
    }    
    return true;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::characters
// -----------------------------------------------------------------------------
//
bool TestResultXmlParser::characters(const QString& ch)
{
    if (mParsingIncidentElement && 
        mParsingDescriptionElement &&
        mCurrentTestFailed) {
        QString testResult = mCurrentTestName + " failed:\n";
        testResult += "File: ";
        testResult += mCurrentTestFile;
        testResult += "\n";
        testResult += "Line: ";
        testResult += QString::number(mCurrentTestFailureLine);
        testResult += "\n";
        testResult += "Reason: ";
        testResult += ch;
        testResult += "\n";
        mErrors->append(testResult);
    }
    return true;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::parse
// -----------------------------------------------------------------------------
//
int TestResultXmlParser::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    return reader.parse(inputSource);
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::parseAndPrintResults
// -----------------------------------------------------------------------------
//
int TestResultXmlParser::parseAndPrintResults(
    const QString& fileName,
    bool printDetails)
{
    printf("Parsing: %s\n", fileName.toUtf8().data());
    int error = parse(fileName);
    printf("%d tests executed. Failed total: %d\n", mTestCount, mErrors->count());
    if (printDetails) {
        printf("\n");
        foreach(QString error, *mErrors) {
            printf(error.toUtf8().data());
            printf("\n");
        }
    }
    return error;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::testCount
// -----------------------------------------------------------------------------
//
int TestResultXmlParser::testCount()
{
    return mTestCount;
}

// -----------------------------------------------------------------------------
// TestResultXmlParser::errors
// -----------------------------------------------------------------------------
//
QStringList TestResultXmlParser::errors()
{
    return *mErrors;
}

// End of File.
