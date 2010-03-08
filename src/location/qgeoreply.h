/*
* ==============================================================================
*  Name        : QGeoReply.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_LOCATIONREPLY_H
#define QLOCATION_LOCATIONREPLY_H

#include <QNetworkReply>
#include <QObject>

namespace QLocation
{

/*!
* This is the base class of all classes that represent replies
* to (reverse) geocoding, route, and map tile requests.
*/
class QGeoReply : public QObject
{
    Q_OBJECT

public:
    /*!
    * Error codes that might occur
    */
    typedef quint32 ErrorCode;

public:
    /*!
    * Default constructor.
    */
    QGeoReply() : fin(false) {}

    /*!
    * @return true (reply finished) / false (reply not finished yet).
    */
    virtual bool isFinished() const {
        return fin;
    }

signals:
    /*!
    * This signal is emitted when the reply is finished. This signal will be
    * emitted after its corresponding QGeoEngine::finished() signal.
    */
    void finished();
    /*!
    * This signal is emitted when an error occured. This signal will be
    * emitted after its corresponding QGeoEngine::error() signal.
    * @note When \ref QGeoNetworkManager is used, the error code will be identical
    *       to Qt::QNetworkReply::NetworkError.
    */
    void error(ErrorCode code);

private:
    Q_DISABLE_COPY(QGeoReply)

protected:
    bool fin; //!< finished flag

};

}

#endif
