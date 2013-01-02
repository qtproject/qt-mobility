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
#ifndef QDECLARATIVETHEMEEFFECT_P_H
#define QDECLARATIVETHEMEEFFECT_P_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>

QTM_USE_NAMESPACE

// Wrapper for theme effects
class QDeclarativeThemeEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool supported READ effectSupported)
    Q_PROPERTY(ThemeEffect effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_CLASSINFO("DefaultMethod", "play()")
    Q_ENUMS(ThemeEffect)

public:

    enum ThemeEffect {
        Basic = QFeedbackEffect::ThemeBasic,
        Sensitive = QFeedbackEffect::ThemeSensitive,
        BasicButton = QFeedbackEffect::ThemeBasicButton,
        SensitiveButton = QFeedbackEffect::ThemeSensitiveButton,
        BasicKeypad = QFeedbackEffect::ThemeBasicKeypad,
        SensitiveKeypad = QFeedbackEffect::ThemeSensitiveKeypad,
        BasicSlider = QFeedbackEffect::ThemeBasicSlider,
        SensitiveSlider = QFeedbackEffect::ThemeSensitiveSlider,
        BasicItem = QFeedbackEffect::ThemeBasicItem,
        SensitiveItem = QFeedbackEffect::ThemeSensitiveItem,
        ItemScroll = QFeedbackEffect::ThemeItemScroll,
        ItemPick = QFeedbackEffect::ThemeItemPick,
        ItemDrop = QFeedbackEffect::ThemeItemDrop,
        ItemMoveOver = QFeedbackEffect::ThemeItemMoveOver,
        BounceEffect = QFeedbackEffect::ThemeBounceEffect,
        CheckBox = QFeedbackEffect::ThemeCheckBox,
        MultipleCheckBox = QFeedbackEffect::ThemeMultipleCheckBox,
        Editor = QFeedbackEffect::ThemeEditor,
        TextSelection = QFeedbackEffect::ThemeTextSelection,
        BlankSelection = QFeedbackEffect::ThemeBlankSelection,
        LineSelection = QFeedbackEffect::ThemeLineSelection,
        EmptyLineSelection = QFeedbackEffect::ThemeEmptyLineSelection,
        PopUp = QFeedbackEffect::ThemePopUp,
        PopupOpen = QFeedbackEffect::ThemePopupOpen,
        PopupClose = QFeedbackEffect::ThemePopupClose,
        Flick = QFeedbackEffect::ThemeFlick,
        StopFlick = QFeedbackEffect::ThemeStopFlick,
        MultiPointTouchActivate = QFeedbackEffect::ThemeMultiPointTouchActivate,
        RotateStep = QFeedbackEffect::ThemeRotateStep,
        LongPress = QFeedbackEffect::ThemeLongPress,
        PositiveTacticon = QFeedbackEffect::ThemePositiveTacticon,
        NeutralTacticon = QFeedbackEffect::ThemeNeutralTacticon,
        NegativeTacticon = QFeedbackEffect::ThemeNegativeTacticon,

        NumberOfThemeEffects = QFeedbackEffect::NumberOfThemeEffects,
        ThemeUser = QFeedbackEffect::ThemeUser
    };

    QDeclarativeThemeEffect(QObject *parent = 0);
    bool effectSupported();
    void setEffect(ThemeEffect effect);
    ThemeEffect effect() const;
public slots:
    void play();
signals:
    void effectChanged();
public:
    ThemeEffect m_effect;
};

QML_DECLARE_TYPE(QDeclarativeThemeEffect);

#endif // QDECLARATIVETHEMEEFFECT_P_H
