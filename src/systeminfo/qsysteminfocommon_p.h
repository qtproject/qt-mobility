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

#ifndef QSYSTEMINFOCOMMON_H
#define QSYSTEMINFOCOMMON_H

#include "qmobilityglobal.h"

#if defined(QT_SIMULATOR) || defined(SIMULATOR_APPLICATION)
#define SIMULATOR
#include "qsysteminfo_simulator_p.h"
#else

#ifndef TESTR
#ifdef Q_OS_LINUX
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "qsysteminfo_maemo_p.h"
#else
#include "linux/qsysteminfo_linux_p.h"
#endif //Q_WS_MAEMO_5 & Q_WS_MAEMO_6
#endif //Q_OS_LINUX

#ifdef Q_OS_WIN
#include "qsysteminfo_win_p.h"
#endif
#ifdef Q_OS_MAC
#include "qsysteminfo_mac_p.h"
#endif
#ifdef Q_OS_SYMBIAN
#include "qsysteminfo_s60_p.h"
#endif
#else
#include "qsysteminfo_simulator_p.h"
#endif

#endif // QT_SIMULATOR

#if defined(ALIGNEDTIMER_MEEGO)
#include "qsystemalignedtimer_meego_p.h"
#elif defined(ALIGNEDTIMER_SYMBIAN)
#include "qsystemalignedtimer_symbian_p.h"
#else
#include "qsystemalignedtimer_stub_p.h"
#endif

#endif // QSYSTEMINFOCOMMON_H
