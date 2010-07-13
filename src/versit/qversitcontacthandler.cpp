/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitcontacthandler.h"

QTM_USE_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(QVersitContactHandlerFactory::ProfileTest, "Test");
Q_DEFINE_LATIN1_CONSTANT(QVersitContactHandlerFactory::ProfileSync, "Sync");

/*!
  \class QVersitContactHandler
  \brief The QVersitContactHandler class is a union of the
  QVersitContactImporterPropertyHandlerV2 and QVersitContactExporterDetailHandlerV2 interfaces.
  \ingroup versit-extension
 */

/*!
  \class QVersitContactHandlerFactory
  \brief The QVersitContactHandlerFactory class provides the interface for Versit plugins.
  \ingroup versit-extension

  This class provides a simple interface for the creation of QVersitContactHandler instances.
  Implement this interface to write a Versit plugin.  For more details, see \l{Versit Plugins}.
 */

/*!
  \fn QVersitContactHandlerFactory::~QVersitContactHandlerFactory()
  This frees any memory used by the QVersitContactHandlerFactory.
 */

/*!
  \fn QSet<QString> QVersitContactHandlerFactory::profiles() const
  This function can be overridden to allow a plugin to report which profiles it is to be active
  under.  If this (as in the default case) returns the empty set, it indicates that the plugin
  should be loaded under all profiles.  If it returns a non-empty set, it will only be loaded for
  those profiles that are specified by the importer/exporter class.
 */

/*!
  \fn QString QVersitContactHandlerFactory::name() const
  This function should return a unique string that identifies the handlers provided by this factory.
  Typically, this will be of the form "com.nokia.qt.mobility.versit.samplehandler" with the
  appropriate domain and handler name substituted.
 */

/*!
  \fn QVersitContactHandler* QVersitContactHandlerFactory::createHandler() const
  This function is called by the Versit importer or exporter class to create an instance of the
  handler provided by this factory.
 */
