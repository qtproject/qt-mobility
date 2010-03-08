/*
* ==============================================================================
*  Name        : QRouteReply.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ROUTEREPLY_H
#define QLOCATION_ROUTEREPLY_H

#include <QString>
#include <QDateTime>
#include <QList>

#include "qroute.h"
#include "qgeoreply.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represents a routing response.
*/
class QRouteReply : public QGeoReply
{
    Q_OBJECT

    friend class QRouteXmlHandler;
    friend class QGeoNetworkManager;

public:

    //! Possible result codes as returned by the geo engine
    enum ResultCode {
        OK = 0, //!< request succeeded
        Failed, //!< request failed
        FailedWithAlternative //!< request failed, but a close alternative was found
    };

public:

    /*!
    * @return The result code as reported by the geo engine.
    */
    ResultCode resultCode() const {
        return rCode;
    }
    /*!
    * @return A textual description of the result.
    */
    QString resultDescription() const {
        return descr;
    }
    /*!
    * @return The RFC 3066 language code of the reply.
    */
    QString language() const {
        return lang;
    }
    /*!
    * @return The number of routes contained in this reply.
    */
    int count() const {
        return rt.size();
    }
    /*!
    * @see QRoute
    * @return A list containing all found routes.
    */
    const QList<QRoute>& routes() const {
        return rt;
    }

private:
    Q_DISABLE_COPY(QRouteReply)

    QRouteReply() : rCode(Failed) {}

private:
    ResultCode rCode;
    QString    descr;
    QString    lang;

    QList<QRoute> rt;
};

QTM_END_NAMESPACE

#endif
