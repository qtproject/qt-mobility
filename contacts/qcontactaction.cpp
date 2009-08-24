/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactaction.h"
#include "qcontactmanager_p.h"
#include "qcontactactiondescriptor.h"

#include <QSet>
#include <QString>

QContactAction::~QContactAction()
{
}

/*!
 * \class QContactAction
 * \brief This class provides an interface for performing an action on a QContact or QContactDetail
 *
 * An action is anything that can be performed on a contact, or a detail of a contact.  An example
 * of an action might be "Send Email" or "Dial" or "Plot Navigation Route".  One action may be
 * implemented by multiple vendors, and indeed one vendor may provide multiple implementations of
 * the same action.  The name of an action identifies its semantics, while its implementation version
 * distinguishes it from other implementations of the action by the same vendor.
 */

/*!
 * \fn QContactAction::~QContactAction()
 * Clears any memory in use by this instance of the action implementation
 */

/*!
 * \fn QContactAction::actionName() const
 * Returns the name of the action provided by this implementation.
 * The name of the action identifies the action provided; different implementations of an action
 * with the same name must provide the same functionality, but may differ in implementation semantics.
 * Hence, the action name includes the major version of the interface definition implemented.
 */

/*!
 * \fn QContactAction::metadata() const
 * Returns the metadata associated with this action, such as icons, labels or sound cues
 */

/*!
 * \fn QContactAction::vendor() const
 * Returns the identification string of the vendor which has provided this implementation
 */

/*!
 * \fn QContactAction::implementationVersion() const
 * Returns the (minor) version of the implementation of this action.  Note that the major version
 * (which describes the semantics/outcomes of the action) is included in the action name itself.
 */

/*!
 * \fn QContactAction::contactFilter(const QVariant& value) const
 * Returns a filter which may be used to filter contacts by the availability of this action implementation for them.
 * If \a value is valid, only contacts which have a detail with the given value and for which the action is available are returned
 */

/*!
 * \fn QContactAction::supportsDetail(const QContactDetail& detail) const
 * Returns true if the provided \a detail contains the fields required for this action to be
 * performed on it; otherwise, returns false
 */

/*!
 * \fn QContactAction::supportedDetails(const QContact& contact) const
 * Returns a list of the details saved in the given \a contact which contain the fields required
 * for this action to be performed on them.
 *
 * The default implementation of this function simply tests all the details in the contact
 * using \l supportsDetail()
 */
QList<QContactDetail> QContactAction::supportedDetails(const QContact& contact) const
{
    QList<QContactDetail> ret;
    QList<QContactDetail> details = contact.details();
    for (int j=0; j < details.count(); j++) {
        if (supportsDetail(details.at(j)))
            ret.append(details.at(j));
    }
    return ret;
}

/*!
 * \fn QContactAction::performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
 * Performs the implemented action on the specified \a detail of the given \a contact, or on the first
 * eligible detail saved in the contact if the given \a detail is empty.
 */

/*!
 * Returns a list of identifiers of the available actions which are provided by the given \a vendor and of the given \a implementationVersion.
 * If \a vendor is empty, actions from all vendors and of any implementation version are returned; if \a implementationVersion is empty,
 * any actions from the given \a vendor (regardless of implementation version) are returned.
 */
QStringList QContactAction::availableActions(const QString& vendor, int implementationVersion)
{
    // SLOW naive implementation...
    QSet<QString> ret;
    QContactManagerData::loadFactories();
    QList<QContactAction*> actionImpls = QContactManagerData::actions(QString(), vendor, implementationVersion);
    for (int i = 0; i < actionImpls.size(); i++) {
        QContactAction* actionImpl = actionImpls.at(i);
        ret.insert(actionImpl->actionName());

        // we took ownership; clean up.
        // TODO: fix this.  currently, we don't take ownership, so don't delete.
        //delete actionImpl;
    }

    return ret.toList();
}

/*!
 * Returns a list of QContactAction instances which implement the given \a actionName and is provided by the
 * given \a vendor and is of the given \a implementationVersion.  If \a actionName is empty,
 * implementations of all actions are returned; if \a vendor is empty, implementations provided by any vendor and
 * of any implementation version are returned; if \a implementationVersion is empty, any implementations provided by the
 * given \a vendor of the given \a actionName are returned.
 *
 * The caller DOES NOT take ownership of each instance returned in the list, and MUST NOT delete them when finished using them
 * to avoid leaking memory as this will result in undefined behaviour.
 *
 * TODO: fix this.
 */
QList<QContactAction*> QContactAction::actions(const QString& actionName, const QString& vendor, int implementationVersion)
{
    // the caller takes ownership?
    QContactManagerData::loadFactories();
    return QContactManagerData::actions(actionName, vendor, implementationVersion);
}

/*!
 * Returns a pointer to a new instance of the action implementation identified by the given \a descriptor.
 * The caller takes ownership of the newly constructed action implementation.
 */
QContactAction* QContactAction::action(const QContactActionDescriptor& descriptor)
{
    // the caller takes ownership
    QContactManagerData::loadFactories();
    QList<QContactAction*> matching = QContactManagerData::actions(descriptor.actionName(), descriptor.vendorName(), descriptor.vendorVersion());
    if (matching.isEmpty())
        return 0; // no such action.
    return matching.first();
}
