/*
 * This file was generated by dbusidl2cpp version 0.5
 * when processing input file org.kde.kwin.Decoration.xml
 *
 * dbusidl2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 */

#include "kwindecorationadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DecorationAdaptor
 */

DecorationAdaptor::DecorationAdaptor(QObject *parent)
   : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DecorationAdaptor::~DecorationAdaptor()
{
    // destructor
}

void DecorationAdaptor::dcopUpdateClientList()
{
    // handle method call org.kde.kwin.Decoration.dcopUpdateClientList
    QMetaObject::invokeMethod(parent(), "dcopUpdateClientList");

    // Alternative:
    //static_cast<YourObjectType *>(parent())->dcopUpdateClientList();
}


#include "kwindecorationadaptor.moc"
