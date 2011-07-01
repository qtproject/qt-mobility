/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qfeedback.h"
#include "qfeedbackeffect.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtPlugin>

#include <meegotouch/MFeedback>

Q_EXPORT_PLUGIN2(feedback_meegotouch, QFeedbackMeegoTouch)

// TODO Use a style property based enum to play the correct feedback
//enum MeegoTouchFeedback
//{
//    // MWidgetStyle
//    widgetPressFeedback,
//    widgetReleaseFeedback,
//    widgetCancelFeedback,
//    // MSliderStyle
//    sliderMoveFeedback,
//    // MTextEditStyle
//    textEditPressBoundaryFeedback,
//    textEditReleaseBoundaryFeedback,
//    textEditPressWordFeedback,
//    textEditReleaseWordFeedback,
//    textEditChangeSelectionFeedback
//};

static QString convertToMeegoTouch(QFeedbackEffect::ThemeEffect effect)
{
    switch (effect) {
    case QFeedbackEffect::ThemeBasic:
    case QFeedbackEffect::ThemeSensitive:
    case QFeedbackEffect::ThemeBasicButton:
    case QFeedbackEffect::ThemeSensitiveButton:
    case QFeedbackEffect::ThemeBasicKeypad:
    case QFeedbackEffect::ThemeSensitiveKeypad:
    case QFeedbackEffect::ThemeBasicSlider:
    case QFeedbackEffect::ThemeSensitiveSlider:
    case QFeedbackEffect::ThemeBasicItem:
    case QFeedbackEffect::ThemeSensitiveItem:
    case QFeedbackEffect::ThemeItemScroll:
    case QFeedbackEffect::ThemeItemPick:
    case QFeedbackEffect::ThemeItemDrop:
    case QFeedbackEffect::ThemeItemMoveOver:
    case QFeedbackEffect::ThemeBounceEffect:
    case QFeedbackEffect::ThemeCheckBox:
    case QFeedbackEffect::ThemeMultipleCheckBox:
    case QFeedbackEffect::ThemeEditor:
    case QFeedbackEffect::ThemeTextSelection:
    case QFeedbackEffect::ThemeBlankSelection:
    case QFeedbackEffect::ThemeLineSelection:
    case QFeedbackEffect::ThemeEmptyLineSelection:
    case QFeedbackEffect::ThemePopUp:
    case QFeedbackEffect::ThemePopupOpen:
    case QFeedbackEffect::ThemePopupClose:
    case QFeedbackEffect::ThemeFlick:
    case QFeedbackEffect::ThemeStopFlick:
    case QFeedbackEffect::ThemeMultiPointTouchActivate:
    case QFeedbackEffect::ThemeRotateStep:
    case QFeedbackEffect::ThemeLongPress:
    case QFeedbackEffect::ThemePositiveTacticon:
    case QFeedbackEffect::ThemeNeutralTacticon:
    case QFeedbackEffect::ThemeNegativeTacticon:
        return MFeedback::Press;
    default:
        return QString();
    }
}

QFeedbackMeegoTouch::QFeedbackMeegoTouch(QObject *parent) :
    QObject(parent),
    QFeedbackThemeInterface()
{
}

bool QFeedbackMeegoTouch::play(QFeedbackEffect::ThemeEffect effect)
{
    const QString &feedbackString = convertToMeegoTouch(effect);
    if (feedbackString.isEmpty())
        return false;

    MFeedback::play(feedbackString);
    return true;
}

QFeedbackInterface::PluginPriority QFeedbackMeegoTouch::pluginPriority()
{
    return QFeedbackInterface::PluginLowPriority;
}

