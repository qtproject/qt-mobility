/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "qlatin1constant.h"

/*!
    \class QLatin1Constant
    \headerfile
    \brief The QLatin1Constant class holds a Latin 1 string constant

*/

/*!
    \fn QLatin1Constant::operator QString() const
    \internal
 */
/*!
    \fn QLatin1Constant::operator QLatin1String() const
    \internal
 */
/*!
    \fn QLatin1Constant::operator QVariant() const
    \internal
 */
/*!
    \fn bool QLatin1Constant::operator ==(const QLatin1Constant& other) const

    Returns true if this QLatin1Constant is the same as \a other (either same object or
    same string literal), and false otherwise.
 */
/*!
    \fn bool QLatin1Constant::operator !=(const QLatin1Constant& other) const

    Returns false if this QLatin1Constant is the same as \a other (either same object or
    same string literal), and true otherwise.
*/
/*!
    \fn inline const char * QLatin1Constant::latin1() const

    Returns the value of this literal as a C style string (null terminated).
*/


/*!
  \macro Q_DECLARE_LATIN1_CONSTANT
  \relates QLatin1Constant

  This macro, along with the related Q_DEFINE_LATIN1_CONSTANT macro,
  allows you to describe a "Latin 1 string constant".

  The resulting constant can be passed to functions accepting a
  QLatin1String, a QString, or a QVariant.

  The first parameter is the name of the variable to declare.  The
  second parameter is the value of the constant, as a string literal.

  For example:
  \code
  // in a header file
  Q_DECLARE_LATIN1_CONSTANT(MyConstant, "MYCONSTANT");
  \endcode

  The declaration should be paired with a matching Q_DEFINE_LATIN1_CONSTANT
  with the same arguments to actually define the constant.

  \sa Q_DEFINE_LATIN1_CONSTANT
*/

/*!
  \macro Q_DEFINE_LATIN1_CONSTANT
  \relates QLatin1Constant

  This macro, along with the related Q_DECLARE_LATIN1_CONSTANT macro,
  allows you to describe a "Latin 1 string constant".

  The resulting constant can be passed to functions accepting a
  QLatin1String, a QString, or a QVariant.

  The first parameter is the name of the variable to define.  The
  second parameter is the value of the constant, as a string literal.

  For example:
  \code
  // in a header file
  Q_DECLARE_LATIN1_CONSTANT(MyConstant, "MYCONSTANT");

  // in source file
  Q_DEFINE_LATIN1_CONSTANT(MyConstant, "MYCONSTANT");
  \endcode

  You can use this macro without the matching DECLARE macro if
  you are using the constant only in a single compilation unit.

  \sa Q_DECLARE_LATIN1_CONSTANT
*/
