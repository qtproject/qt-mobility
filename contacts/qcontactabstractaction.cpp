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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactabstractaction.h"

QContactAbstractAction::~QContactAbstractAction()
{
}

/*!
 * \class QContactAbstractAction
 * \brief This class provides an interface for performing an action on a QContact or QContactDetail
 *
 * An action is anything that can be performed on a contact, or a detail of a contact.  An example
 * of an action might be "Send Email" or "Dial" or "Plot Navigation Route".  One action may be
 * implemented by multiple vendors, and indeed one vendor may provide multiple implementations of
 * the same action.  The name of an action identifies its semantics, while its implementation version
 * distinguishes it from other implementations of the action by the same vendor.
 */

/*!
 * \fn QContactAbstractAction::~QContactAbstractAction()
 * Clears any memory in use by this instance of the action implementation
 */

/*!
 * \fn QContactAbstractAction::actionName() const
 * Returns the name of the action provided by this implementation.
 * The name of the action identifies the action provided; different implementations of an action
 * with the same name must provide the same functionality, but may differ in implementation semantics.
 * Hence, the action name includes the major version of the interface definition implemented.
 */

/*!
 * \fn QContactAbstractAction::metadata() const
 * Returns the metadata associated with this action, such as icons, labels or sound cues
 */

/*!
 * \fn QContactAbstractAction::vendor() const
 * Returns the identification string of the vendor which has provided this implementation
 */

/*!
 * \fn QContactAbstractAction::implementationVersion() const
 * Returns the (minor) version of the implementation of this action.  Note that the major version
 * (which describes the semantics/outcomes of the action) is included in the action name itself.
 */

/*!
 * \fn QContactAbstractAction::contactFilter() const
 * Returns a filter which may be used to filter contacts by the availability of this action implementation for them.
 */

/*!
 * \fn QContactAbstractAction::supportsDetail(const QContactDetail& detail) const
 * Returns true if the provided \a detail contains the fields required for this action to be
 * performed on it; otherwise, returns false
 */

/*!
 * \fn QContactAbstractAction::supportedDetails(const QContact& contact) const
 * Returns a list of the details saved in the given \a contact which contain the fields required
 * for this action to be performed on them.
 *
 * The default implementation of this function simply tests all the details in the contact
 * using \l supportsDetail()
 */
QList<QContactDetail> QContactAbstractAction::supportedDetails(const QContact& contact) const
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
 * \fn QContactAbstractAction::performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
 * Performs the implemented action on the specified \a detail of the given \a contact, or on the first
 * eligible detail saved in the contact if the given \a detail is empty.
 */

