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

#ifndef QLOCATION_BASEXMLHANDLER_H
#define QLOCATION_BASEXMLHANDLER_H

#include "qmobilityglobal.h"

#include <QXmlDefaultHandler>
#include <QList>
#include <QRectF>

QTM_BEGIN_NAMESPACE

/*!
* This is the base class for all classes that handle the parsing of
* a raw Qt::QNetworkReply to populate \ref QGeoReply, \ref QRouteReply and \ref QMapTileReply objects.
*/
class QBaseXmlHandler : public QXmlDefaultHandler
{
public:
    /*!
    * Reimplemented from Qt::QXmlDefaultHandler::startElement().
    */
    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    /*!
    * Reimplemented from Qt::QXmlDefaultHandler::endElement().
    */
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    /*!
    * Reimplemented from Qt::QXmlDefaultHandler::characters().
    */
    virtual bool characters(const QString& ch);

private:
    Q_DISABLE_COPY(QBaseXmlHandler)

protected:
    //! Internal parse states
    enum ParseState {
        Root,
        BoundingBox,
        BoundingBox_NW,
        BoundingBox_SE,
        Latitude,
        Longitude,
        Last
    };

    /*!
    * Constructor.
    */
    QBaseXmlHandler();
    /*!
    * Starts the parsing of bounding box XML data.
    */
    virtual bool startBoundingBox() = 0;

protected:
    QList<quint16> parseStates; //!< The current sequence of parse states.
    QRectF* currBox; //!< A pointer to the current box for which data is being parsed.
};

QTM_END_NAMESPACE

#endif
