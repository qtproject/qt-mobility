/*
* ==============================================================================
*  Name        : QBaseXmlHandler.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include "qbasexmlhandler.h"

QTM_BEGIN_NAMESPACE

QBaseXmlHandler::QBaseXmlHandler() : currBox(NULL)
{
}

bool QBaseXmlHandler::startElement(const QString& /*namespaceURI*/,
                                   const QString& /*localName*/,
                                   const QString& qName,
                                   const QXmlAttributes& /*atts*/)
{
    if (qName == "boundingBox") {
        if (startBoundingBox())
            parseStates.push_front(BoundingBox);
        else
            return false;
    } else if (qName == "northWest") {
        if (parseStates.count() == 0 || parseStates[0] != BoundingBox)
            return false;

        parseStates.push_front(BoundingBox_NW);
    } else if (qName == "southEast") {
        if (parseStates.count() == 0 || parseStates[0] != BoundingBox)
            return false;

        parseStates.push_front(BoundingBox_SE);
    } else if (qName == "latitude") {
        if (parseStates.count() == 0 ||
                (parseStates[0] != BoundingBox_NW && parseStates[0] != BoundingBox_SE)) {
            return false;
        }

        parseStates.push_front(Latitude);
    } else if (qName == "longitude") {
        if (parseStates.count() == 0 ||
                (parseStates[0] != BoundingBox_NW && parseStates[0] != BoundingBox_SE)) {
            return false;
        }

        parseStates.push_front(Longitude);
    } else
        return false;

    return true;
}

bool QBaseXmlHandler::characters(const QString& ch)
{
    if (parseStates.count() == 0)
        return false;

    if (parseStates[0] == Latitude) {
        if (parseStates[1] == BoundingBox_NW)
            currBox->setTop(ch.toDouble());
        else if (parseStates[1] == BoundingBox_SE)
            currBox->setBottom(ch.toDouble());
        else
            return false;
    } else if (parseStates[0] == Longitude) {
        if (parseStates[1] == BoundingBox_NW)
            currBox->setLeft(ch.toDouble());
        else if (parseStates[1] == BoundingBox_SE)
            currBox->setRight(ch.toDouble());
        else
            return false;
    } else
        return false;

    return true;
}

bool QBaseXmlHandler::endElement(const QString& , const QString& , const QString& qName)
{
    if (parseStates.count() == 0)
        return false;

    if (qName == "latitude") {
        if (parseStates[0] != Latitude)
            return false;
    } else if (qName == "longitude") {
        if (parseStates[0] != Longitude)
            return false;
    } else if (qName == "northWest") {
        if (parseStates[0] != BoundingBox_NW)
            return false;
    } else if (qName == "southEast") {
        if (parseStates[0] != BoundingBox_SE)
            return false;
    } else if (qName == "boundingBox") {
        if (parseStates[0] != BoundingBox)
            return false;

        currBox = NULL;
    } else
        return false;

    parseStates.removeFirst();
    return true;
}

QTM_END_NAMESPACE
