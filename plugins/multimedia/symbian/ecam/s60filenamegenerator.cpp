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

#include "s60filenamegenerator.h"

/*
 * This method generates a default image or video file name. On Symbian it may
 * be e.g. one of the following:
 * c:/Data/Images/Camera/201105/201105A0/16052011001.jpg
 * e:/Images/Camera/201105/201105A0/16052011001.jpg
 * e./Videos/Camera/201105/201105A0/16052011001.mp4
 * The file name and path are returned in native format (i.e. using '\' separators).
 */
QString S60FileNameGenerator::defaultFileName(const FileNameType nameType, const QString postfix)
{
    QDir directory;
    QString newFileName = detectDestinationDrive(directory);

    if (nameType == ImageFileName)
        newFileName.append(QLatin1String("Images/"));
    else
        newFileName.append(QLatin1String("Videos/"));

    newFileName.append(QLatin1String("Camera"));
    newFileName.append(QLatin1Char('/'));

    QDate date = QDate::currentDate();
    directory.setPath(newFileName);
    int fileNameIndex = 0;
    generateFolder(directory, date, fileNameIndex);
    newFileName = directory.path();

    // Detect file name
    QString dateForFileName = date.toString(QLatin1String("ddMMyyyy"));

    while (true) {
        QString indexString = QString::number(fileNameIndex);
        while (indexString.length() < 3)
            indexString.prepend(QLatin1Char('0'));
        if (directory.entryList().contains(dateForFileName + indexString + postfix)) {
            // Check next file name
            ++fileNameIndex;
            continue;
        } else {
            // Proper name found
            newFileName.append(QLatin1Char('/'));
            newFileName.append(dateForFileName);
            newFileName.append(indexString);
            newFileName.append(postfix);
            break;
        }
    }

    return QDir::toNativeSeparators(newFileName);
}

/*
 * Generates a image/video file name based on the given parameters:
 *  * Type (image or video)
 *  * Base (either absolute or relative file name of type QString)
 *  * File type
 */
QString S60FileNameGenerator::generateFileNameFromString(const FileNameType nameType,
                                                         const QString base,
                                                         const QString postfix)
{
    QString baseString(base);
    return generateFileName(nameType, baseString, postfix);
}

/*
 * Generates a image/video file name based on the given parameters:
 *   * Type (image or video)
 *   * Base (either absolute or relative file name of type QUrl)
 *   * File type
 */
QString S60FileNameGenerator::generateFileNameFromUrl(const FileNameType nameType,
                                                      const QUrl base,
                                                      const QString postfix)
{

    QString baseString(base.toLocalFile());
    return generateFileName(nameType, baseString, postfix);
}

QString S60FileNameGenerator::generateFileName(const FileNameType nameType,
                                               QString base,
                                               const QString postfix)
{

    // To Qt format
    base.replace(QLatin1Char('\\'), QLatin1Char('/'));

    QString newFileName;
    // Relative FileName
    if (!base.contains(QLatin1Char(':'))) {
        QDir directory;
        newFileName = detectDestinationDrive(directory);

        if (nameType == ImageFileName)
            newFileName.append(QLatin1String("Images/"));
        else
            newFileName.append(QLatin1String("Videos/"));
        newFileName.append(QLatin1String("Camera"));

        if (base.at(0) != QLatin1Char('/'))
            newFileName.append(QLatin1Char('/'));
        newFileName.append(base);

        // Make sure full path exists
        QString path = newFileName.left(newFileName.lastIndexOf(QLatin1Char('/')));
        directory.setPath(path);
        if (!directory.exists())
            directory.mkpath(path);
        newFileName = QDir::toNativeSeparators(newFileName);

    // Absolute FileName
    } else {
        // Make sure path exists
        QString path = base.left(base.lastIndexOf(QLatin1Char('/')));
        QDir directory(path);
        if (!directory.exists())
            directory.mkpath(path);
        newFileName = QDir::toNativeSeparators(base);
    }

    // Confirm file name has proper postfix
    if (!newFileName.contains(QRegExp(QLatin1String(".+\\..+$"))))
        newFileName.append(postfix);

    return newFileName;
}

/*
 * Method to detect whether the device has mass memory available
 * Returns the drive and base folder to be used in file name generation
 * In practive it is either of the following:
 *   * c:\Data
 *   * e:\
 */
QString S60FileNameGenerator::detectDestinationDrive(const QDir currentDirectory)
{
    QFileInfoList driveList = currentDirectory.drives();
    bool isMassMemoryAvailable = false;
    foreach (QFileInfo info, driveList) {
        if (info.path() == QLatin1String("E:/") && info.isWritable())
            isMassMemoryAvailable = true;
    }

    if (isMassMemoryAvailable) {
        return QLatin1String("e:/");
    } else {
        return QLatin1String("c:/Data/");
    }
}

/*
 * Method to generate Symbian specific date based folder structure where the
 * captured image or video will be saved.
 */
void S60FileNameGenerator::generateFolder(QDir &currentDirectory,
                                          QDate &date,
                                          int &fileNameOffset)
{
    // Folder of format: YYYYMM
    QString newFolder = date.toString(QLatin1String("yyyyMM"));
    if (!currentDirectory.cd(newFolder)) {
        if (!currentDirectory.mkpath(newFolder)) {
            currentDirectory.mkpath(currentDirectory.path());
            return;
        }
        if (!currentDirectory.cd(newFolder)) {
            currentDirectory.mkpath(currentDirectory.path());
            return;
        }
    }

    // Now create subfolder of format: YYYYMM[A-Z][0-9]
    fileNameOffset = 0;
    char indexChar('A');
    int indexInt = 0;
    QString testFolder;
    while (true) {
        testFolder = currentDirectory.path() + QLatin1Char('/') + newFolder + indexChar + QString::number(indexInt);
        if (currentDirectory.cd(testFolder)) {
            // Check file count (max 100 files per folder)
            if (currentDirectory.entryList().count() < 100) {
                return;
            } else {
                // Folder full, check next
                if (indexInt < 9) {
                    ++indexInt;
                } else {
                    indexInt = 0;
                    indexChar += 1;
                }
                fileNameOffset += 100;
                continue;
            }
        } else {
            if (currentDirectory.mkpath(testFolder)) {
                if (currentDirectory.cd(testFolder))
                    return;
            }
        }
    }
}

// End of file
