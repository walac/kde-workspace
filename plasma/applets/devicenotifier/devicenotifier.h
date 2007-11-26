/***************************************************************************
 *   Copyright (C) 2007 by Alexis Ménard <darktears31@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef DEVICENOTIFIER_H
#define DEVICENOTIFIER_H

#include <plasma/applet.h>
#include <plasma/dataengine.h>
#include <plasma/phase.h>
#include <KIcon>
#include <QModelIndex>
#include <QTimer>
#include <QListView>

#include "ui_deviceNotifierConfig.h"

namespace Plasma
{
    class Svg;
    class Widget;
    class Label;
    class Icon;
    class HBoxLayout;
    class ProgressBar;
} // namespace Plasma

class QStandardItemModel;
class KDialog;

namespace Notifier
{
    class ListView;
}

class DeviceNotifier : public Plasma::Applet
{
    Q_OBJECT


    public:
        DeviceNotifier(QObject *parent, const QVariantList &args);
        ~DeviceNotifier();

	void init();
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
	QSizeF contentSizeHint() const;
	void hoverEnterEvent ( QGraphicsSceneHoverEvent  *event);
	void paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &rect);
    public slots:
        void onSourceAdded(const QString &name);
        void onSourceRemoved(const QString &name);
        void dataUpdated(const QString &source, Plasma::DataEngine::Data data);
        void showConfigurationInterface();
	void configAccepted();
	void slotOnItemDoubleclicked(const QModelIndex & );
	void onTimerExpired();

    private:
        QModelIndex indexForUdi(const QString &udi) const;

        KIcon m_icon;
        Plasma::DataEngine *m_solidEngine;
        QStandardItemModel *m_hotplugModel;

	Notifier::ListView *m_listView;
	QWidget *m_widget;
	QVBoxLayout *m_layout;
        KDialog *m_dialog;
	int m_displayTime;
	int m_numberItems;
	int m_itemsValidity;
	QTimer * m_timer;
        /// Designer Config file
        Ui::solidNotifierConfig ui;

};

K_EXPORT_PLASMA_APPLET(devicenotifier, DeviceNotifier)

#endif
