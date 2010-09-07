/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ichecklib.h"
#include "parsemanager.h"
#include <QtCore/QCoreApplication>
#include <QString>
#include <QStringList>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QLibraryInfo>

QT_USE_NAMESPACE

QStringList getQTIncludePath()
{
    QStringList ret;
    QString qtpath = QLibraryInfo::location(QLibraryInfo::HeadersPath);

    ret << qtpath;
    ret << qtpath + "/ActiveQt";
    ret << qtpath + "/phonon";
    ret << qtpath + "/phonon_compat";
    ret << qtpath + "/Qt";
    ret << qtpath + "/Qt3Support";
    ret << qtpath + "/QtAssistant";
    ret << qtpath + "/QtCore";
    ret << qtpath + "/QtDBus";
    ret << qtpath + "/QtDeclarative";
    ret << qtpath + "/QtDesigner";
    ret << qtpath + "/QtGui";
    ret << qtpath + "/QtHelp";
    ret << qtpath + "/QtMultimedia";
    ret << qtpath + "/QtNetwork";
    ret << qtpath + "/QtOpenGL";
    ret << qtpath + "/QtOpenVG";
    ret << qtpath + "/QtScript";
    ret << qtpath + "/QtScriptTools";
    ret << qtpath + "/QtSql";
    ret << qtpath + "/QtSvg";
    ret << qtpath + "/QtTest";
    ret << qtpath + "/QtUiTools";
    ret << qtpath + "/QtWebKit";
    ret << qtpath + "/QtXml";
    ret << qtpath + "/QtXmlPatterns";

    return ret;
}

ICheckLib::ICheckLib()
: pParseManager(0)
{
}

void ICheckLib::ParseHeader(const QStringList& includePath, const QStringList& filelist)
{
    if(pParseManager)
        delete pParseManager;
    pParseManager = 0;
    pParseManager = new CPlusPlus::ParseManager();
    pParseManager->setIncludePath(includePath);
    pParseManager->parse(filelist);
}

bool ICheckLib::check(const ICheckLib& ichecklib /*ICheckLib from interface header*/, QString outputfile)
{
    if(pParseManager){
        CPlusPlus::ParseManager* cpparsemanager = ichecklib.pParseManager;
        return pParseManager->checkAllMetadatas(cpparsemanager, outputfile);
    }
    return false;
}

QStringList ICheckLib::getErrorMsg()
{
    QStringList ret;
    if(pParseManager){
        ret.append(pParseManager->getErrorMsg());
    }
    else
        ret << "no file was parsed.";
    return ret;
}
