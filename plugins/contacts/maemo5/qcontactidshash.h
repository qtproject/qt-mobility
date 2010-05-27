/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTIDSHASH_H
#define QCONTACTIDSHASH_H

#include <QByteArray>
#include "qcontactmaemo5debug_p.h"
#include "qtcontacts.h"

QTM_USE_NAMESPACE

/* QContactIDsHash stores abookContact IDs (strings)*/
class QContactIDsHash{
public:
  QContactIDsHash(){};
  
  /* Append */
  QContactIDsHash& operator<< (const QByteArray& eContactId){
    const QContactLocalId key = qChecksum(eContactId, eContactId.size());
    if (m_localIds.contains(key))
      return (*this);
    m_localIds[key] = eContactId;
    QCM5_DEBUG << "Add key:" << key << "eContactId:" << eContactId;
    return (*this);
  };
  
  const QContactLocalId append(const QByteArray& eContactId){
    const QContactLocalId key = qChecksum(eContactId, eContactId.size());
    if (m_localIds.contains(key))
      return key;
    m_localIds[key] = eContactId;
    QCM5_DEBUG << "Add key:" << key << "eContactId:" << eContactId;
    return key;
  };
  
  /* Find */
  const char* operator[] (const QContactLocalId localId) { return m_localIds.value(localId).constData(); };
  const char* find(const QContactLocalId localId) { return m_localIds.value(localId).constData(); };
  const QContactLocalId operator[] (const QByteArray& eContactId) { return m_localIds.key(eContactId, 0); };
  const QContactLocalId find(const QByteArray& eContactId) { return m_localIds.key(eContactId, 0); };
  
  /* Remove */
  bool remove(const QContactLocalId localId){ bool removed = (m_localIds.remove(localId) == 1) ? true : false;
                                              QCM5_DEBUG << "Remove QContactLocalId:" << localId << ((removed) ? "OK" : "NO");
                                              return removed;
                                            };
  bool remove(const QByteArray& eContactId){ 
    const QContactLocalId hashKey = m_localIds.key(eContactId, 0);
    bool removed = remove(hashKey);
    QCM5_DEBUG << "Remove QContactLocalId:" << hashKey << ((removed) ? "OK" : "NO");
    return removed;
  };
  
  /* Take */
  const QContactLocalId take(const QByteArray& eContactId){
    const QContactLocalId hashKey = m_localIds.key(eContactId, 0);
    remove(hashKey);
    return hashKey;
  };

  /* Keys */
  const QList<QContactLocalId> keys() {return m_localIds.keys();}
  
private:
  QHash<QContactLocalId, QByteArray> m_localIds; //[int/QContactLocalId Maemo5LocalId, QByteArray eContactID]
};

#endif
