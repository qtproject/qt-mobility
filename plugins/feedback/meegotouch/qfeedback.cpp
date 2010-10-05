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
#include "qfeedback.h"
#include "qfeedbackeffect.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtPlugin>

#include <meegotouch/MComponentData>
#include <meegotouch/MFeedbackPlayer>

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
    case QFeedbackEffect::ThemeMultitouchActivate:
    case QFeedbackEffect::ThemeRotateStep:
    case QFeedbackEffect::ThemeLongPress:
    case QFeedbackEffect::ThemePositiveTacticon:
    case QFeedbackEffect::ThemeNeutralTacticon:
    case QFeedbackEffect::ThemeNegativeTacticon:
        return MFeedbackPlayer::Press;
    default:
        return QString();
    }
}

QFeedbackMeegoTouch::QFeedbackMeegoTouch(QObject *parent) :
    QObject(parent),
    QFeedbackThemeInterface()
{
    if (MComponentData::instance() != 0) {
        componentData = MComponentData::instance();
    } else {
        QByteArray argv0;
        if (!QCoreApplication::arguments().isEmpty())
            argv0 = QCoreApplication::arguments().first().toLocal8Bit();

        int argc = 1;
        char *argv[] = { argv0.data() };
        componentData = new MComponentData(argc, argv);
    }
}

bool QFeedbackMeegoTouch::play(QFeedbackEffect::ThemeEffect effect)
{
    const QString &feedbackString = convertToMeegoTouch(effect);
    MFeedbackPlayer *feedbackPlayer = componentData->feedbackPlayer();
    if (feedbackString.isEmpty() || !feedbackPlayer)
        return false;

    feedbackPlayer->play(feedbackString);
    return true;
}

QFeedbackInterface::PluginPriority QFeedbackMeegoTouch::pluginPriority()
{
    return QFeedbackInterface::PluginLowPriority;
}

