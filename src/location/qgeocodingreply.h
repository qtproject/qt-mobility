/*
* ==============================================================================
*  Name        : QGeocodingReply.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEOCODINGREPLY_H
#define QLOCATION_GEOCODINGREPLY_H

#include <QString>
#include <QObject>

#include "qgeolocation.h"
#include "qaddress.h"
#include "qgeoreply.h"

namespace QLocation
{
/*!
* This class represents a geododing reply in response to a
* previous (reverse) geocoding request.
* @see QGeocodingRequest
* @see QReverseGeocodingRequest
*/
class QGeocodingReply : public QGeoReply
{
    Q_OBJECT

    friend class QGeoNetworkManager;
    friend class QGeocodingXmlHandler;

public:

    //! Possible result codes as reported by the geo engine.
    enum ResultCode {
        OK = 0, //!< request succeeded
        Failed //!< request failed
    };

public:
    /*!
    * @return The result code as reported by the geo engine.
    */
    ResultCode resultCode() const {
        return code;
    }
    /*!
    * @return A textual description of the result.
    */
    QString resultDescription() const {
        return descr;
    }
    /*!
    * @return The number of places found.
    */
    quint32 count() const {
        return plcs.length();
    }
    /*!
    * @return A list of all places found.
    */
    QList<QGeoLocation> places() const {
        return plcs;
    }

private:
    Q_DISABLE_COPY(QGeocodingReply)

    QGeocodingReply() {}

private:
    ResultCode code;
    QString descr;
    QList<QGeoLocation> plcs;
};

}

#endif
