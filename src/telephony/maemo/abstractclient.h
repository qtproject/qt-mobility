/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TelepathyQt4_abstract_client_h_HEADER_GUARD_
#define _TelepathyQt4_abstract_client_h_HEADER_GUARD_

#include "maemo/shared-ptr.h"
#include "maemo/types.h"
#include "maemo/interfaces/cli-types.h"


#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtDBus/QDBusMessage>

namespace Tp
{

class AbstractClient : public RefCounted
{
    Q_DISABLE_COPY(AbstractClient)

public:
    AbstractClient();
    virtual ~AbstractClient();
};

class AbstractClientObserver : public virtual AbstractClient
{
    Q_DISABLE_COPY(AbstractClientObserver)

public:
    virtual ~AbstractClientObserver();

    ChannelClassList observerChannelFilter() const;

    bool shouldRecover() const;

    virtual void observeChannels(const QDBusObjectPath &accountPath,
                                 const QDBusObjectPath &connectionPath,
                                 const Tp::ChannelDetailsList &channelDetailsList,
                                 const QDBusObjectPath &dispatchOperationPath,
                                 const Tp::ObjectPathList &requestsSatisfied,
                                 const QVariantMap &observerInfo,
                                 const QDBusMessage &message) = 0;

protected:
    AbstractClientObserver(const ChannelClassList &channelFilter);
    AbstractClientObserver(const ChannelClassList &channelFilter,
            bool shouldRecover);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Tp

#endif
