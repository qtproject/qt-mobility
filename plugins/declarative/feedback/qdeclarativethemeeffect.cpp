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

#include "qdeclarativethemeeffect_p.h"
/*!
    \qmlclass ThemeEffect
    \brief The ThemeEffect element represents a themed feedback effect.
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    This element is used for playing feedback effects that follow the
    system theme.  The actual feedback might be haptic, audio or some other
    method.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml Theme
*/
QDeclarativeThemeEffect::QDeclarativeThemeEffect(QObject *parent)
    : QObject(parent),
    m_effect(QDeclarativeThemeEffect::Basic)
{
}

/*!
    \qmlproperty bool ThemeEffect::supported

    This property is true if the system supports themed feedback effects.
*/
bool QDeclarativeThemeEffect::effectSupported() {
    return QFeedbackEffect::supportsThemeEffect();
}

/*!
    \qmlproperty ThemeEffect ThemeEffect::effect

    This property holds the specific themed effect type to play.  It is one of:

  \list
    \o ThemeEffect.Basic - Generic feedback.
    \o ThemeEffect.Sensitive - Generic sensitive feedback.
    \o ThemeEffect.BasicButton - Feedback for interacting with a button (e.g. pressing).
    \o ThemeEffect.SensitiveButton - Sensitive feedback for interacting with a button (e.g. auto repeat).
    \o ThemeEffect.BasicKeypad - Feedback for interacting with a keypad button.
    \o ThemeEffect.SensitiveKeypad - Sensitive feedback for interacting with a keypad button.
    \o ThemeEffect.BasicSlider - Feedback for moving a slider.
    \o ThemeEffect.SensitiveSlider - Sensitive feedback for moving a slider.
    \o ThemeEffect.BasicItem - Feedback when interacting with a list or grid item.
    \o ThemeEffect.SensitiveItem - Sensitive feedback when interacting with a list or grid item.
    \o ThemeEffect.ItemScroll - Feedback when scrolling a list or grid item view.
    \o ThemeEffect.ItemPick - Feedback when selecting an item to move in a list or grid view.
    \o ThemeEffect.ItemDrop - Feedback when dropping an item in a list or grid view.
    \o ThemeEffect.ItemMoveOver - Feedback when moving an item in a list or grid view.
    \o ThemeEffect.BounceEffect - Feedback for a bounce effect.
    \o ThemeEffect.CheckBox - Feedback for selecting a checkbox.
    \o ThemeEffect.MultipleCheckBox - Feedback for selecting checkboxes of multiple items.
    \o ThemeEffect.Editor - Feedback for interacting with an editor.
    \o ThemeEffect.TextSelection - Feedback for selecting text.
    \o ThemeEffect.BlankSelection - Feedback for a blank selection.
    \o ThemeEffect.LineSelection - Feedback for selecting a line.
    \o ThemeEffect.EmptyLineSelection - Feedback for selecting an empty line.
    \o ThemeEffect.PopUp - Generic feedback for interacting with a popup.
    \o ThemeEffect.PopupOpen - Generic feedback when a popup opens.
    \o ThemeEffect.PopupClose - Generic feedback when a popup closes.
    \o ThemeEffect.Flick - Generic feedback when starting a flick gesture.
    \o ThemeEffect.StopFlick - Generic feedback when stopping a flick.
    \o ThemeEffect.MultiPointTouchActivate - Generic feedback when a touch gesture with more than one point is started.
    \o ThemeEffect.RotateStep - Feedback when rotating using a gesture.
    \o ThemeEffect.LongPress - Feedback for a long press (or tap and hold) gesture.
    \o ThemeEffect.PositiveTacticon - Generic feedback for notification of a successful operation.
    \o ThemeEffect.NeutralTacticon - Generic feedback for notification.
    \o ThemeEffect.NegativeTacticon - Generic feedback for notification of a failed operation.
  \endlist

  \sa QFeedbackEffect::ThemeEffect
*/
void QDeclarativeThemeEffect::setEffect(QDeclarativeThemeEffect::ThemeEffect effect)
{
    if (m_effect != effect) {
        m_effect = effect;
        emit effectChanged();
    }
}

QDeclarativeThemeEffect::ThemeEffect QDeclarativeThemeEffect::effect() const
{
    return m_effect;
}

/*!
    \qmlmethod ThemeEffect::play()

    Call this to play the themed effect.
*/
void QDeclarativeThemeEffect::play()
{
    QFeedbackEffect::playThemeEffect(static_cast<QFeedbackEffect::ThemeEffect>(m_effect));
}

