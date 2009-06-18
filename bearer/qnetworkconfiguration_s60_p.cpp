/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qnetworkconfiguration_s60_p.h"

QT_BEGIN_NAMESPACE

QNetworkConfigurationPrivate::QNetworkConfigurationPrivate()
    : isValid(false), type(QNetworkConfiguration::Invalid),
      roamingSupported(false), purpose(QNetworkConfiguration::Unknown), 
      bearer(QNetworkConfigurationPrivate::BearerUnknown), numericId(0)
{
}

QNetworkConfigurationPrivate::~QNetworkConfigurationPrivate()
{
    //release pointers to member configurations
    serviceNetworkMembers.clear(); 
}

QT_END_NAMESPACE
