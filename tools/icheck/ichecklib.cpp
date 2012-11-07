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
