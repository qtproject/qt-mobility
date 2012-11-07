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

#ifndef QWMPGLOBAL_H
#define QWMPGLOBAL_H

#include <wmp.h>

#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

const char *qwmp_error_string(HRESULT hr);

class QAutoBStr
{
public:
    inline QAutoBStr(const QString &string)
        : m_string(::SysAllocString(static_cast<const wchar_t *>(string.utf16())))
    {
    }

    inline QAutoBStr(const QUrl &url)
        : m_string(::SysAllocString(static_cast<const wchar_t *>(url.toString().utf16())))
    {
    }

    inline QAutoBStr(const wchar_t *string)
        : m_string(::SysAllocString(string))
    {
    }

    inline ~QAutoBStr()
    {
        ::SysFreeString(m_string);
    }

    inline operator BSTR() const { return m_string; }

private:
    BSTR m_string;
};



#endif
