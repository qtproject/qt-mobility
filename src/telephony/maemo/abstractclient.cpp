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

#include "abstractclient.h"

#include <QtCore/QString>

namespace Tp
{

/**
 * \class AbstractClient
 * \ingroup clientclient
 * \headerfile TelepathyQt4/abstract-client.h AbstractClient>
 *
 * \brief The AbstractClient class provides an object representing a Telepathy
 * client.
 *
 * Clients are programs used to process channels, approving, handling or
 * observing them. User interface processes are the obvious example of clients,
 * but they can provide other functionality, such as address-book
 * synchronization, message logging, etc.
 *
 * Each client is either an observer, an approver, a handler, or some
 * combination of these.
 *
 * Clients can be activatable services (those with a D-Bus .service file)
 * so that they can run in response to channel creation, or non-activatable
 * services (those that do not register a D-Bus .service file
 * for their well-known name, but do request it at runtime) so
 * that they can process channels, but only if they are already
 * running - for instance, a full-screen media center application might do this.
 *
 * As an optimization, service-activatable clients should install a file
 * $XDG_DATA_DIRS/telepathy/clients/clientname.client containing a cached version
 * of their immutable properties. The syntax of these files is <a
 * href="http://telepathy.freedesktop.org/spec/org.freedesktop.Telepathy.Client.html">
 * documented in the Telepathy D-Bus API Specification</a>.
 *
 * Non-activatable clients may install a .client file, but there's not much
 * point in them doing so.
 *
 * This is a base class and should not be used directly, use the
 * specialized classes AbstractClientObserver, AbstractClientApprover and
 * AbstractClientHandler instead.
 *
 * If the same process wants to be either a mix of observer, approver and
 * handler, or a combination of those it can multiple inherit the specialized
 * abstract classes.
 *
 * \sa AbstractClientObserver, AbstractClientApprover, AbstractClientHandler
 */

/**
 * Construct a new AbstractClient object.
 *
 * Note that this is a base class and should not be used directly, use the
 * specialized classes AbstractClientObserver, AbstractClientApprover and
 * AbstractClientHandler instead.
 */
AbstractClient::AbstractClient()
{
}

/**
 * Class destructor.
 */
AbstractClient::~AbstractClient()
{
}

struct AbstractClientObserver::Private
{
    Private(const ChannelClassList &channelFilter, bool shouldRecover)
        : channelFilter(channelFilter), shouldRecover(shouldRecover)
    {
    }

    ChannelClassList channelFilter;
    bool shouldRecover;
};

/**
 * \class AbstractClientObserver
 * \ingroup clientclient
 * \headerfile TelepathyQt4/abstract-client.h AbstractClientObserver>
 *
 * \brief The AbstractClientObserver class provides an object representing a
 * Telepathy observer.
 *
 * Observers are clients that monitor the creation of new channels.
 * This functionality can be used for things like message logging.
 *
 * Observers should not modify the state of a channel except via user
 * interaction.
 *
 * Observers must not carry out actions that exactly one process must take
 * responsibility for (e.g. acknowledging text messages, or carrying out
 * the actual file transfer), since arbitrarily many observers can be
 * activated for each channel. The handler is responsible for such tasks.
 *
 * Handlers may, of course, delegate responsibility for these tasks to other
 * clients (including those run as observers), but this must be done
 * explicitly via a request from the handler to the observer.
 *
 * Whenever a collection of new channels is signalled, the channel dispatcher
 * will notify all running or activatable observers whose filter indicates that
 * they are interested in some of the channels.
 *
 * Observers are activated for all channels in which they have registered an
 * interest - incoming, outgoing or automatically created - although of course
 * the filter property can be set to filter specific channels.
 *
 * To become an observer one should inherit AbstractClientObserver and
 * implement the pure virtual observeChannels() method. After that the object
 * representing the observer must be registered using
 * ClientRegistrar::registerClient().
 *
 * When new channels in which the observer has registered an interest are
 * announced, the method observeChannels() is invoked. All observers are
 * notified simultaneously.
 *
 * \section observer_usage_sec Usage
 *
 * \subsection observer_create_sec Implementing an observer
 *
 * \code
 *
 * class MyObserver : public AbstractClientObserver
 * {
 * public:
 *     MyObserver(const ChannelClassList &channelFilter);
 *     ~MyObserver() { }
 *
 *     void observeChannels(const MethodInvocationContextPtr<> &context,
 *             const AccountPtr &account,
 *             const ConnectionPtr &connection,
 *             const QList<ChannelPtr> &channels,
 *             const ChannelDispatchOperationPtr &dispatchOperation,
 *             const QList<ChannelRequestPtr> &requestsSatisfied,
 *             const QVariantMap &observerInfo);
 * };
 *
 * MyObserver::MyObserver(const ChannelClassList &channelFilter)
 *     : AbstractClientObserver(channelFilter)
 * {
 * }
 *
 * void MyObserver::observeChannels(const MethodInvocationContextPtr<> &context,
 *         const AccountPtr &account,
 *         const ConnectionPtr &connection,
 *         const QList<ChannelPtr> &channels,
 *         const ChannelDispatchOperationPtr &dispatchOperation,
 *         const QList<ChannelRequestPtr> &requestsSatisfied,
 *         const QVariantMap &observerInfo)
 * {
 *     // do something, log messages, ...
 *
 *     context->setFinished();
 * }
 *
 * \endcode
 *
 * \subsection observer_register_sec Registering an observer
 *
 * \code
 *
 * ChannelClassList filters;
 * QMap<QString, QDBusVariant> filter;
 * filter.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
 *               QDBusVariant(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT)));
 * filter.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
 *               QDBusVariant((uint) Tp::HandleTypeContact));
 * filters.append(filter);
 * ClientRegistrar registrar = ClientRegistrar::create();
 * AbstractClientPtr observer = AbstractClientPtr::dynamicCast(
 *         SharedPtr<MyObserver>(new MyObserver(filter)));
 * registrar->registerClient(observer, "myobserver");
 *
 * \endcode
 *
 * \sa AbstractClient
 */

/**
 * Construct a new AbstractClientObserver object.
 *
 * Note that using this constructor the shouldRecover() method will return
 * \c false, meaning that on crash the observer won't be able to "catch up"
 * on channels that match its observerChannelFilter() automatically.
 *
 * \param channelFilter A specification of the channels in which this observer
 *                      is interested.
 */
AbstractClientObserver::AbstractClientObserver(
        const ChannelClassList &channelFilter)
    : mPriv(new Private(channelFilter, false))
{
}

/**
 * Construct a new AbstractClientObserver object.
 *
 * \param channelFilter A specification of the channels in which this observer
 *                      is interested.
 * \param shouldRecover Whether upon the startup of this observer,
 *                      observeChannels() will be called for every already
 *                      existing channel matching its observerChannelFilter().
 */
AbstractClientObserver::AbstractClientObserver(
        const ChannelClassList &channelFilter,
        bool shouldRecover)
    : mPriv(new Private(channelFilter, shouldRecover))
{
}
/**
 * Class destructor.
 */
AbstractClientObserver::~AbstractClientObserver()
{
    delete mPriv;
}

/**
 * Return the property containing a specification of the channels that this
 * channel observer is interested. The observeChannels() method should be called
 * by the channel dispatcher whenever any of the newly created channels match
 * this description.
 *
 * Only certain types have useful semantics for matching like this,
 * so only certain types are allowed:
 *
 * See <a
 * href="http://telepathy.freedesktop.org/spec/org.freedesktop.Telepathy.Client.Observer.html#org.freedesktop.Telepathy.Client.Observer.ObserverChannelFilter">
 * the Telepathy D-Bus API Specification</a> for documentation about the allowed
 * types and how to define filters.
 *
 * This property never changes while the observer process owns its client bus
 * name. If an observer wants to add extra channels to its list of interests at
 * runtime, it can register an additional client bus name using
 * ClientRegistrar::registerClient().
 * To remove those filters, it can release the bus name using
 * ClientRegistrar::unregisterClient().
 *
 * The same principle is applied to approvers and handlers.
 *
 * \return A specification of the channels that this channel observer is
 *         interested.
 * \sa observeChannels()
 */
ChannelClassList AbstractClientObserver::observerChannelFilter() const
{
    return mPriv->channelFilter;
}

/**
 * Return whether upon the startup of this observer, observeChannels()
 * will be called for every already existing channel matching its
 * observerChannelFilter().
 *
 * \param \c true if this observer observerChannels() will be called for every
 *        already existing channel matching its observerChannelFilter(),
 *        \c false otherwise.
 */
bool AbstractClientObserver::shouldRecover() const
{
    return mPriv->shouldRecover;
}

/**
 * \fn void AbstractClientObserver::observeChannels(
 *                  const MethodInvocationContextPtr<> &context,
 *                  const AccountPtr &account,
 *                  const ConnectionPtr &connection,
 *                  const QList<ChannelPtr> &channels,
 *                  const ChannelDispatchOperationPtr &dispatchOperation,
 *                  const QList<ChannelRequestPtr> &requestsSatisfied,
 *                  const QVariantMap &observerInfo);
 *
 * Called by the channel dispatcher when channels in which the observer has
 * registered an interest are announced.
 *
 * If the announced channels contains channels that match the
 * observerChannelFilter(), and some that do not, then only a subset of the
 * channels (those that do match the filter) are passed to this method.
 *
 * If the channel dispatcher will split up the channels from a single
 * announcement and dispatch them separately (for instance because no
 * installed handler can handle all of them), it will call this method
 * several times.
 *
 * The observer must not call MethodInvocationContext::setFinished() until it
 * is ready for a handler for the channel to run (which may change the
 * channel's state). For instance the received \a context object should be
 * stored until this method is finished processing and then
 * MethodInvocationContext::setFinished() or
 * MethodInvocationContext::setFinishedWithError() should be called on the
 * received \a context object.
 *
 * Specialized observers must reimplement this method.
 *
 * \param context A MethodInvocationContextPtr object that must be used to
 *                indicate whether this method finished processing.
 * \param account The account with which the channels are associated.
 * \param connection The connection with which the channels are associated.
 * \param channels The channels to be observed.
 * \param dispatchOperation The dispatch operation for these channels.
 *                          The object will be invalid (DBusProxy::isValid()
 *                          will be false) if there is no dispatch
 *                          operation in place (because the channels were
 *                          requested, not incoming).
 *                          If the Observer calls
 *                          ChannelDispatchOperation::claim() or
 *                          ChannelDispatchOperation::handleWith() on this
 *                          object, it must be careful to avoid deadlock, since
 *                          these methods cannot return until the observer has
 *                          returned from observeChannels().
 * \param requestsSatisfied The requests satisfied by these channels.
 * \param observerInfo Additional information about these channels. No keys are
 *                     currently defined.
 *                     If keys are defined for this dictionary, all will be
 *                     optional; observers may safely ignore any entry in this
 *                     dictionary.
 */

} // Tp
