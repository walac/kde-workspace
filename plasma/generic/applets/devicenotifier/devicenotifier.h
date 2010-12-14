/***************************************************************************
 *   Copyright (C) 2007 by Alexis Ménard <darktears31@gmail.com>           *
 *   Copyright 2009 by Giulio Camuffo <giuliocamuffo@gmail.com>            *
 *   Copyright 2009 by Jacopo De Simoi <wilderkde@gmail.com>               *
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

//Solid
#include <Solid/Device>

//Plasma
#include <Plasma/PopupApplet>
#include <Plasma/DataEngine>

///Ui includes
#include "ui_configurationpage.h"

class KCModuleProxy;

namespace Notifier
{

class NotifierDialog;

class HotplugDataConsumer : public QObject
{
    Q_OBJECT
public:
    HotplugDataConsumer(NotifierDialog *parent);

protected Q_SLOTS:
        /**
     * slot called when a source of the hotplug engine is updated
     * @param udi the udi of the source
     * @param data the data of the source
     **/
    void dataUpdated(const QString &udi, Plasma::DataEngine::Data data);

private:
    NotifierDialog * const m_dialog;
};

class DeviceDataConsumer : public QObject
{
    Q_OBJECT
public:
    DeviceDataConsumer(NotifierDialog *parent);

protected Q_SLOTS:
        /**
     * slot called when a source of the hotplug engine is updated
     * @param udi the udi of the source
     * @param data the data of the source
     **/
    void dataUpdated(const QString &udi, Plasma::DataEngine::Data data);

private:
    NotifierDialog * const m_dialog;
};

/**
 * @short Applet used to display devices
 *
 */
class DeviceNotifier : public Plasma::PopupApplet
{
    Q_OBJECT

    public:
        enum {
            RemovableOnly = 0,
            NonRemovableOnly = 1,
            AllDevices = 2
        };

        /**
         * Constructor of the applet
         * @param parent the parent of this object
         **/
        DeviceNotifier(QObject *parent, const QVariantList &args);

        /**
         * Default destructor
         **/
        ~DeviceNotifier();

        /**
         * initialize the applet (called by plasma automatically)
         **/
        void init();

        /**
         * Used to know it there are hidden devices
         * @return true if there are hidden device
         **/	
        bool areThereHiddenDevices();

        /**
         *  allow to change the icon of the notifier if this applet is in icon mode
         *  @param name icon name
         *  @param timeout if not 0, time to wait before resetting the icon (in msecs)
         **/
        void changeNotifierIcon(const QString &name = QString(), uint timeout = 0);

        /**
         * The graphics widget that displays the list of devices.
         */
        QGraphicsWidget *graphicsWidget();

        QList<QAction *> contextualActions();

        static const int LONG_NOTIFICATION_TIMEOUT = 7500;
        static const int SHORT_NOTIFICATION_TIMEOUT = 2500;

    protected:
        /**
         * Reimplemented from Plasma::Applet
         **/
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

        /**
         * Reimplemented from Plasma::Applet
         **/
        void createConfigurationInterface(KConfigDialog *parent);

        /**
        * Reimplemented from Plasma::PopupApplet
        **/
        void popupEvent(bool show);

    public slots:
        /**
        * Reimplemented from Plasma::Applet
        **/
        void configChanged();
        
        /**
        * @internal Sets the tooltip content properly before showing.
        */
        void toolTipAboutToShow();

        /**
        * @internal Clears memory when needed.
        */
        void toolTipHidden();

        /**
        * Sets the visibility of a device
        * @param udi the udi of the device
        * @param visibility true if it is visible
        **/
        void setDeviceVisibility(const QString &udi, bool visibility);

        /**
        * Define if the devices that would be hidden will be showed anyway
        * @param visibility true if they have to be visible
        **/
        void setGlobalVisibility(bool visibility);

        /**
        * Shows a message
        * @param message the text of the message
        * @param details details of the notification
        * @param udi the udi of the device
        **/
        void showNotification(const QString &message, const QString &details, const QString &udi);

    protected slots:
        /**
        * slot called when a source/device is added in the hotplug engine
        * @param udi the udi of the new source
        **/
        void onSourceAdded(const QString &udi);

        /**
        * @internal slot called when a source/device is removed in the hotplug engine
        * @param udi the udi of the removed source
        **/
        void onSourceRemoved(const QString &udi);

        /**
        * Reimplemented from Plasma::Applet
        **/
        void configAccepted();

        void newNotification(const QString &source);

    private slots:
        /**
        * @internal Used to recreate the devices in the menu.
        */
        void resetDevices();

        /**
        * @internal slot called to restore to the notifier his icon
        **/
        void resetNotifierIcon();

    private:
        /**
         * @internal used to register a new device that has appeared to us
         */
        void deviceAdded(const Solid::Device &device, bool hotplugged = true);

        /**
        * @internal Used to fill the notifier from previous plugged devices
        **/
        void fillPreviousDevices();

        /**
         * @internal Used to popup the device view.
         */
        void notifyDevice(const QString &udi);

        /**
         * @internal Used to remove the last device notification.
         */
        void removeLastDeviceNotification(const QString &udi);

        // Data consumers that relay information to the main applet
        HotplugDataConsumer *m_hotplugDataConsumer;
        DeviceDataConsumer *m_deviceDataConsumer;

        ///the engine used to get hot plug devices
        Plasma::DataEngine *m_hotplugEngine;

        ///The engine used to manage devices in the applet (unmount,...)
        Plasma::DataEngine *m_solidDeviceEngine;

	///The engine used to retrieve device notifications
        Plasma::DataEngine *m_deviceNotificationsEngine;

        ///The dialog where devices are displayed
        Notifier::NotifierDialog * m_dialog;

        ///the time durin when the dialog will be show
        int m_displayTime;

        ///the number of items displayed in the dialog
        int m_numberItems;

        ///the time during when the item will be displayed
        int m_itemsValidity;

        ///the timer for different use cases
        QTimer *m_timer;

        //Timer to reset the icon
        QTimer *m_iconTimer;

        QList<QString> m_lastPlugged;

        ///list of the hidden devices
        QList<QString> m_hiddenDevices;

        ///configuration page
        Ui::configurationPage m_configurationUi;

        ///tells which devices to show
        int m_showDevices;

        ///bool to know if notifications are enabled
        bool isNotificationEnabled : 1;

        ///list of the devices
        ///true if fillPreviousDevices is running
        bool m_fillingPreviousDevices : 1;

        ///if true all the devices will be shown anyway
        bool m_globalVisibility : 1;

        ///if true the applet will check if the removed devices were hidded
        bool m_checkHiddenDevices : 1;

        ///if true we are triggering the popup internally 
        bool m_triggeringPopupinternally : 1;

        ///embedded KCM modules in the configuration dialog
        KCModuleProxy *m_autoMountingWidget;
        KCModuleProxy *m_deviceActionsWidget;
};

}
#endif
