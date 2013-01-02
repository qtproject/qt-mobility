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

#ifndef XARECORDSESSIONCOMMON_H
#define XARECORDSESSIONCOMMON_H

#include <e32base.h>
#include "xacommon.h"

#define MAX_NUMBER_INTERFACES 20
#define MAX_NUMBER_INPUT_DEVICES 10
#define MAX_NUMBER_ENCODERS 10

//const TInt32 KExtErr = (TInt32)(-2147483648);
const TInt32 KExtErr = -32768;
const TInt32 KExtErrUnspecifiedCodecForContainer = (KExtErr+1);
const TInt32 KExtErrUnsupportedCodecForContainer = (KExtErr+2);
const TInt32 KExtErrUnsupportedURISuffixForContainer = (KExtErr+3);

class XARecordObserver
{
public:
    virtual void cbDurationChanged(TInt64 new_pos) = 0;
    virtual void cbAvailableAudioInputsChanged() = 0;
    virtual void cbRecordingStarted() = 0;
    virtual void cbRecordingStopped() = 0;
};

#endif /* XARECORDSESSIONCOMMON_H */
