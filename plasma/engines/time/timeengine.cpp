/*
 *   Copyright (C) 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#include "timeengine.h"

#include <QDate>
#include <QTime>
#include <QTimer>

#include <KDebug>
#include <KLocale>
//#include <KSystemTimeZones> TODO: uncomment and remove include below on monday
#include <ksystemtimezone.h>

#include "plasma/datasource.h"

TimeEngine::TimeEngine(QObject* parent, const QStringList& args)
    : Plasma::DataEngine(parent)
{
    Q_UNUSED(args)
    //TODO: we need to add the ability to configure this so that can limit
    //      it to minutes only as well as set which TZs to publish
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
}

TimeEngine::~TimeEngine()
{
}

bool TimeEngine::reportSeconds()
{
    return m_seconds;
}

void TimeEngine::setReportSeconds(bool seconds)
{
    //kDebug() << "only report seconds? " << seconds << endl;
    if (m_seconds == seconds) {
        return;
    }

    m_seconds = seconds;
    if (seconds) {
        m_timer->setInterval(500);
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(setTimerTo60()));
    } else {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(setTimerTo60()));
        m_timer->setInterval(((60 - QTime::currentTime().second()) * 1000) + 500);
    }
}

void TimeEngine::setTimerTo60()
{
    disconnect(m_timer, SIGNAL(timeout()), this, SLOT(setTimerTo60()));
    m_timer->setInterval(60000);
}

bool TimeEngine::sourceRequested(const QString &name)
{
    if (name == "Local") {
        setData(I18N_NOOP("Local"), II8N_NOOP("Time"), QTime::currentTime());
        setData(I18N_NOOP("Local"), I18N_NOOP("Date"), QDate::currentDate());

        if (!m_timer->isActive()) {
            m_timer->start(500);
        }
        return true;
    }

    // reenable when KTimeZone/kded isn't broken
    const KTimeZone *local = KSystemTimeZones::local();
    const KTimeZone *newTz  = KSystemTimeZones::zone(name);
    if (!newTz) {
        return false;
    }

    QDateTime dt = local->convert(newTz, QDateTime::currentDateTime());
    setData(name, I18N_NOOP("Time"), dt.time());
    setData(name, I18N_NOOP("Date"), dt.date());

    if (!m_timer->isActive()) {
        m_timer->start(500);
    }

    return true;
}

void TimeEngine::updateTime()
{
    //kDebug() << "TimeEngine::updateTime()" << endl;

    QDateTime dt = QDateTime::currentDateTime();
    const KTimeZone *local = KSystemTimeZones::local();
    DataEngine::SourceDict sources = sourceDict();
    DataEngine::SourceDict::iterator it = sources.begin();
    QString localName = I18N_NOOP("Local");

    if (!m_seconds) {
        int seconds = dt.time().second();
        if (seconds > 2) {
            // we've drifted more than 2s off the minute, let's reset this
            connect(m_timer, SIGNAL(timeout()), this, SLOT(setTimerTo60()));
            m_timer->setInterval(((60 - seconds) * 1000) + 500);
        }
    }

    while (it != sources.end()) {
        QString tz = it.key();
        if (tz == localName) {
            it.value()->setData(I18N_NOOP("Time"), dt.time());
            it.value()->setData(I18N_NOOP("Date"), dt.date());
        } else {
            const KTimeZone *newTz = KSystemTimeZones::zone(tz);
            QDateTime localizeDt = local->convert(newTz, dt);
            it.value()->setData(I18N_NOOP("Time"), localizeDt.time());
            it.value()->setData(I18N_NOOP("Date"), localizeDt.date());
        }
        ++it;
    }

    checkForUpdates();
}

#include "timeengine.moc"
