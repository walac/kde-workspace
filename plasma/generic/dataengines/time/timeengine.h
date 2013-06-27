/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef TIMEENGINE_H
#define TIMEENGINE_H

#include <kqpluginfactory.h>
#include <Plasma/DataEngine>

#include <QDebug>



// class name : public baseFactory \
// { \
//     public: \
//         explicit name(const char * = 0, const char * = 0, QObject * = 0); \
//         ~name(); \
//     private: \
//         void init(); \
// };
//
/*
class TimeEngineFactory : public QObject, public KQPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.KQPluginFactory" FILE "plasma_engine_time.json")
    Q_INTERFACES(KQPluginFactory)

    public:
        QObject* createPlugin(const QString &name);

};
*/


/**
 * This engine provides the current date and time for a given
 * timezone. Optionally it can also provide solar position info.
 *
 * "Local" is a special source that is an alias for the current
 * timezone.
 */
class TimeEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        explicit TimeEngine(const KPluginInfo &plugin, QObject *parent=0);
        TimeEngine(QObject* parent, const QVariantList& args);
        ~TimeEngine();

        void init();
        QStringList sources() const;

    protected:
        bool sourceRequestEvent(const QString &name);
        bool updateSourceEvent(const QString &source);

    protected Q_SLOTS:
        void clockSkewed(); // call when system time changed and all clocks should be updated
        void tzConfigChanged();
};

K_PLUGIN_HEADER(TimeEngineFactory, TimeEngine, "plasma_engine_time.json")
/*
K_PLUGIN_DEFINITION(TimeEngineFactory, TimeEngine)*/

// inline QObject* TimeEngineFactory::createPlugin(const QString& name)
// {
//     qDebug() << "TimeEngineFactory::createPlugin implementation" << name;
//     //QObject *o = new QObject(); // getter is expected to take ownership
//     //o->setObjectName(name);
//
//     QVariantList args;
//     args << name;
//     QObject *time_engine = new TimeEngine(0, args);
//
//     return time_engine;
// }
//

#endif // TIMEENGINE_H
