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

#include "qcontactinvalidbackend_p.h"
#include "qcontactmanagercapabilities_p.h"

#include <QSharedData>

/*!
 * \class QContactInvalidEngine
 * \brief This class provides an implementation of QContactManagerEngine whose functions always return an error.
 * The invalid engine may be used as a reference or test engine, but does nothing.
 */

/*! Constructs a new invalid contacts backend. */
QContactInvalidEngine::QContactInvalidEngine()
{
}

/*! Creates a clone of this invalid engine.  The caller takes ownership of the newly allocated engine */
QContactManagerEngine* QContactInvalidEngine::clone()
{
    // this engine does not allow sharing - we create a new one.
    return new QContactInvalidEngine();
}

/*! Reduces the reference count on this manager engine.  If no managers reference this engine, it will delete itself */
void QContactInvalidEngine::deref()
{
    delete this;
}

/*! Returns the capabilities of the invalid engine, which is always an empty list */
QStringList QContactInvalidEngine::capabilities() const
{
    return QStringList();
}
