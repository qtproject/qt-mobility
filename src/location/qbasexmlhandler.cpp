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
