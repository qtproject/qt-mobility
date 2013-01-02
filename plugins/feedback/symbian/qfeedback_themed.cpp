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

#include "qfeedback_symbian.h"
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

// This is included if we have themed effect support

#include <touchfeedback.h>

MTouchFeedback *QFeedbackSymbian::touchInstance()
{
    static MTouchFeedback *ret = 0;
    if (!ret) {
        QT_TRAP_THROWING(
            ret = MTouchFeedback::Instance();
            if (!ret)
                ret = MTouchFeedback::CreateInstanceL();
        )
    }
    return ret;
}

static TTouchLogicalFeedback convertToSymbian(QFeedbackEffect::ThemeEffect effect)
{

    TTouchLogicalFeedback themeFeedbackSymbian = ETouchFeedbackBasic;

    switch (effect) {
    case QFeedbackEffect::ThemeBasic:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeSensitive:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
    case QFeedbackEffect::ThemeBasicButton:
        themeFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case QFeedbackEffect::ThemeSensitiveButton:
        themeFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case QFeedbackEffect::ThemeBasicItem:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeSensitiveItem:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case QFeedbackEffect::ThemeBounceEffect:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemePopupOpen:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemePopupClose:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeBasicSlider:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeSensitiveSlider:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case QFeedbackEffect::ThemeStopFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeEditor:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeTextSelection:
        themeFeedbackSymbian = ETouchFeedbackTextSelection;
        break;
    case QFeedbackEffect::ThemeBlankSelection:
        themeFeedbackSymbian = ETouchFeedbackBlankSelection;
        break;
    case QFeedbackEffect::ThemeLineSelection:
        themeFeedbackSymbian = ETouchFeedbackLineSelection;
        break;
    case QFeedbackEffect::ThemeEmptyLineSelection:
        themeFeedbackSymbian = ETouchFeedbackEmptyLineSelection;
        break;
    case QFeedbackEffect::ThemeCheckBox:
        themeFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
    case QFeedbackEffect::ThemeMultipleCheckBox:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeSensitiveKeypad:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case QFeedbackEffect::ThemeBasicKeypad:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeMultiPointTouchActivate:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeRotateStep:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeItemDrop:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeItemMoveOver:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeItemPick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeItemScroll:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemePopUp:
        themeFeedbackSymbian = ETouchFeedbackPopUp;
        break;
    case QFeedbackEffect::ThemeLongPress:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
#endif //HAS_ADVANCED_TACTILE_SUPPORT
    default:
        break;
    }
    return themeFeedbackSymbian;
}

bool QFeedbackSymbian::play(QFeedbackEffect::ThemeEffect effect)
{
    TInt err = KErrNone;
    TRAP(err, touchInstance()->InstantFeedback(convertToSymbian(effect)));
    return err == KErrNone;
}
