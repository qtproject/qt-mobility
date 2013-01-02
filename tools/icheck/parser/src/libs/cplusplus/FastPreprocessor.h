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

#ifndef FASTPREPROCESSOR_H
#define FASTPREPROCESSOR_H

#include "PreprocessorClient.h"
#include "CppDocument.h"
#include "pp.h"

#include <CppControl.h>

#include <QtCore/QSet>
#include <QtCore/QString>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT FastPreprocessor: public Client
{
    Environment _env;
    Snapshot _snapshot;
    Preprocessor _preproc;
    QSet<QString> _merged;

    void mergeEnvironment(const QString &fileName);

public:
    FastPreprocessor(const Snapshot &snapshot);

    QByteArray run(QString fileName, const QString &source);

    // CPlusPlus::Client
    virtual void sourceNeeded(QString &fileName, IncludeType, unsigned);

    virtual void macroAdded(const Macro &) {}

    virtual void passedMacroDefinitionCheck(unsigned, const Macro &) {}
    virtual void failedMacroDefinitionCheck(unsigned, const QByteArray &) {}

    virtual void startExpandingMacro(unsigned,
                                     const Macro &,
                                     const QByteArray &,
                                     bool,
                                     const QVector<MacroArgumentReference> &) {}

    virtual void stopExpandingMacro(unsigned, const Macro &) {}

    virtual void startSkippingBlocks(unsigned) {}
    virtual void stopSkippingBlocks(unsigned) {}
};

} // end of namespace CPlusPlus

#endif // FASTPREPROCESSOR_H
