/* * This file is part of qtcontacts-tracker *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef TRACKER2QCONTACT_H
#define TRACKER2QCONTACT_H

#include <QtTracker/ontologies/nco.h>

class QContact;

class Tracker2QContact
{
 public:
    static bool copyContactData(const SopranoLive::Live<SopranoLive::nco::PersonContact>& ncoContact, QContact& qcontact );
};

#endif
