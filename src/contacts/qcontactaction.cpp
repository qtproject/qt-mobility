/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactaction.h"
#include "qcontactmanager_p.h"
#include "qcontactactiondescriptor.h"

#include "qcontactactionmanager_p.h"

#include <QSet>
#include <QString>

QTM_BEGIN_NAMESPACE

QContactAction::~QContactAction()
{
}

/*!
  \class QContactAction
  \brief The QContactAction class provides an interface for performing an
  action on a QContact or QContactDetail.
  \ingroup contacts-main
  \ingroup contacts-actions

  \inmodule QtContacts
  
  An action is anything that can be performed on a contact, or a detail of a contact.  An example
  of an action might be "Send Email" or "Dial" or "Plot Navigation Route".  One action may be
  implemented by multiple vendors, and indeed one vendor may provide multiple implementations of
  the same action.  The name of an action identifies its semantics, while its implementation version
  distinguishes it from other implementations of the action by the same vendor.

  Invocation of an action is asynchronous; at some stage after calling \l invokeAction() the
  action instance will emit the \l stateChanged() signal.  Any results of the action may be retrieved
  by calling \l results(), and as results become available the action will emit \l resultsAvailable().

  Each instance of a QContactAction is created by a \l QContactActionFactory when
  \l QContactAction::action() is called; the caller takes ownership of the action instance.
  Each action is uniquely described by a \l QContactActionDescriptor, which is passed to the
  \l QContactAction::action() function to instantiate an action.
 
  \sa QContactActionFactory, QContactActionFilter
 */

/*!
  \fn QContactAction::~QContactAction()
  Clears any memory in use by this instance of the action implementation
 */

/*!
  \fn QContactAction::state() const
  Returns the current state of the action.
  \sa stateChanged()
 */

/*!
  \fn QContactAction::invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap())
  Initiates the implemented action on the specified \a detail of the given \a contact, or on the first
  eligible detail saved in the contact if the given \a detail is empty, with the given \a parameters specified.
  At some point after invocation, one or more \l resultsAvailable() signals will be emitted by the action instance.
  The results of the action (if any) may be retrieved by calling \l results().
  When the state of the action changes, the \l stateChanged() signal will be emitted.

  Returns true if the action was invoked successfully, otherwise false.  The return value says nothing
  about whether the action which was invoked was successful or not, only whether it was initiated or
  the request for it to be initiated was sent successfully (e.g., if the action is implemented as a one-way
  RPC call).

  \sa results(), stateChanged()
 */

/*!
  \fn QContactAction::results() const
  Returns the result of the action, if any exists.  Calling this function prior to receiving the \l resultsAvailable()
  signal will not return a meaningful result.
 */

/*!
  \enum QContactAction::State
  Describes the current status of the asynchronous action operation
  \value InactiveState The operation has not yet been initiated
  \value FinishedDetachedState The operation was initiated but no further information is or will be available
  \value ActiveState The operation was initiated and is not yet finished
  \value FinishedState The operation successfully completed
  \value FinishedWithErrorState The operation has finished, but an error occurred
 */

/*!
  \fn QContactAction::resultsAvailable()
  This signal is emitted by an action instance whose functionality has been initiated with \l invokeAction()
  when results of the action are available.  Not all actions will have results, and these
  actions will not emit the resultsAvailable() signal.

  If the action implementation is incapable of reporting results of the operation (for example, the
  action is implemented via a one-way IPC call) it should transition to the \c QContactAction::FinishedDetachedState state
  immediately upon invocation.
 */

/*!
  \fn QContactAction::stateChanged(QContactAction::State newState)
  This signal is emitted when the state of an action changes to the given \a newState.
  \sa state()
 */

/*!
  Returns a list of identifiers of the available actions which are provided by the service provider with the given \a serviceName.
  If \a serviceName is empty, actions from all service providers and of any implementation version are returned.
 */
QStringList QContactAction::availableActions(const QString& serviceName)
{
    // SLOW naive implementation...
    QSet<QString> ret;
    QList<QContactActionDescriptor> actionDescriptors = QContactActionManager::instance()->actionDescriptors();
    for (int i = 0; i < actionDescriptors.size(); i++) {
        QContactActionDescriptor descriptor = actionDescriptors.at(i);
        if (serviceName.isEmpty() || serviceName == descriptor.serviceName()) {
            ret.insert(descriptor.actionName());
        }
    }

    return ret.toList();
}


/*!
   \variable QContactAction::ActionCall
   The name of the default call action.
   Actions of this name will allow the client to call
   the specified action target (contact or detail of a contact).
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionCall, "Call");

/*!
   \variable QContactAction::ActionEmail
   The name of the default send email action.
   Actions of this name will either open a graphical element
   which allows the client to send the specified action
   target an email, or directly send the specified action
   target an email if the correct parameters to invocation
   are specified.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionEmail, "email");

/*!
   \variable QContactAction::ActionSms
   The name of the default send sms action.
   Actions of this name will allow the client to send
   the specified action target an sms.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionSms, "sms");

/*!
   \variable QContactAction::ActionMms
   The name of the default send mms action.
   Actions of this name will allow the client to send
   the specified action target an mms.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionMms, "mms");

/*!
   \variable QContactAction::ActionChat
   The name of the default IM chat action.
   Actions of this name will allow the client to begin
   an IM chat session with the specified action target.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionChat, "chat");

/*!
   \variable QContactAction::ActionVideoCall
   The name of the default video call action.
   Actions of this name will allow clients to initiate
   a video call with the specified action target.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionVideoCall, "videocall");

/*!
   \variable QContactAction::ActionOpenInEditor
   The name of the default "edit contact" action.
   Actions of this name will open a graphical element
   which allows the user to edit the contact.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionOpenInEditor, "edit");

/*!
   \variable QContactAction::ActionOpenInViewer
   The name of the default view contact action.
   Actions of this name will open a graphical element
   which allows the user to view the contact.
   \sa actionDescriptors()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAction::ActionOpenInViewer, "view");

/*!
  Returns a list of QContactActionDescriptor instances which identified implementations of the given \a actionName.
  The action name may either be one of the default action names, or any other arbitrary string.

  Example:
  \code
      QList<QContactActionDescriptor> availableCallActions = QContactAction::actionDescriptors(QContactAction::ActionCall);
  \endcode

  Example 2:
  \code
      QList<QContactActionDescriptor> customActions = QContactAction::actionDescriptors("customActionName");
  \endcode

  The actions which are available depend on which action plugins have been installed.  For more information
  on this topic (for example, if you are interested in providing an action plugin for third-party developers
  to use) please see the relevant documentation for \l{Qt Contacts Action API}{action providers}.
 */
QList<QContactActionDescriptor> QContactAction::actionDescriptors(const QString& actionName)
{
    QContactActionManager* qcam = QContactActionManager::instance();
    return qcam->actionDescriptors(actionName);
}

/*!
  Returns a pointer to a new instance of the action implementation identified by the given \a descriptor.
  The caller takes ownership of the action implementation and must delete it to avoid leaking memory.
  The caller is able to delete the action at any time, however doing so prior to when the action
  transitions to a finished state may have an undefined outcome depending on the implementation of the action.
 */
QContactAction* QContactAction::action(const QContactActionDescriptor& descriptor)
{
    QContactActionManager* qcam = QContactActionManager::instance();
    return qcam->action(descriptor);
}

#include "moc_qcontactaction.cpp"

QTM_END_NAMESPACE
