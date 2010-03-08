/*
* ==============================================================================
*  Name        : QBaseXmlHandler.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

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
