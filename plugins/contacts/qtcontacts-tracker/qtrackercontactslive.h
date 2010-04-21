/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTRACKERCONTACTSLIVE_H
#define QTRACKERCONTACTSLIVE_H

#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactphonenumber.h>
#include <qcontactname.h>
#include <qcontactnickname.h>

#include <QtTracker/Tracker>
#include <QtTracker/QLive>
#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

QTM_USE_NAMESPACE

/**
 * This class will abstact and hide how contact information is saved to Tracker
 * by using Live node.
 */
class QTrackerContactsLive
{
public:
    QTrackerContactsLive();

    /**
     * Set the QContact object that we are editing.
     *
     * \param qc The QContact object that is used for reading the data that
     *           will be stored in Tracker.
     */
    void setQContact(const QContact& qc);

    /**
     * Give the Live node object that is used for this transaction. This
     * object will be used internally by this object and will contain the data
     * that will be stored into Tracker.
     *
     * \param lc A Live node object representing a contact. See NCO ontology
     *           for details.
     */
    void setLiveContact(const Live<nco::PersonContact>& lc);

    /**
     * Return a service pointer that is used for this transaction. The Live
     * node in setLiveContact() is retrieved from this service.
     */
    RDFServicePtr service();

    /**
     * When all data is saved, we need to call this method to send the data to
     * tracker in one batch.
     */
    void commit();

    /**
     * Worker method that is doing the saving of the name properties. This method
     * will use the objects given by setQContact() and setLiveContact() to determine
     * what will be stored into Tracker.
     */
    void saveName();

private:
    QContact editedContact_;
    Live<nco::PersonContact> liveContact_;

    RDFServicePtr service_;
    RDFTransactionPtr transaction_;
};

#endif // QTRACKERCONTACTSLIVE_H
