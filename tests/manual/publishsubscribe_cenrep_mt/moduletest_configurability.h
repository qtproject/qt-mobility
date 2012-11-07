/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

    void threads();
    void threads2();

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
