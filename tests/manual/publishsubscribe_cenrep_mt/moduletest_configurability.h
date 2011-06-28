/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MODULETEST_CONFIGURABILITY
#define MODULETEST_CONFIGURABILITY

#include <QObject>

class ModuletestConfigurability : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase();     //before first
    void init();             //before each
    void cleanup();          //after each
    void cleanupTestCase();  //after last

    void readAndSetCenrep_data();
    void readAndSetCenrep();
    void readAndSetPubsub_data();
    void readAndSetPubsub();

    void invalidPath_data();
    void invalidPath();

    void cenrepInteroperabilityQtToSymbian_data();
    void cenrepInteroperabilityQtToSymbian();
    void cenrepInteroperabilitySymbianToQt_data();
    void cenrepInteroperabilitySymbianToQt();
    void cenrepInteroparabilityDecodableBytearray_data();
    void cenrepInteroparabilityDecodableBytearray();

    void floatType();
    void floatTypeProperty();
    void stringParameter1_data();
    void stringParameter1();
    void stringParameter2_data();
    void stringParameter2();
    void stringRProperty1_data();
    void stringRProperty1();
    void stringRProperty2_data();
    void stringRProperty2();

    void featManagerSimpleSubscriber_data();
    void featManagerSimpleSubscriber();

#ifdef Q_OS_SYMBIAN
    void featManagerAdvSubscriber_data();
    void featManagerAdvSubscriber();
#endif /* Q_OS_SYMBIAN */

    void featManagerMapperCase_data();
    void featManagerMapperCase();

public slots:
    void dummy();
};

#endif // MODULETEST_CONFIGURABILITY
