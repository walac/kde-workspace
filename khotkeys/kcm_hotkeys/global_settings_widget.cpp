/*
   Copyright (C) 2008 Michael Jansen <kde@michael-jansen.biz>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "global_settings_widget.h"

#include "hotkeys_model.h"
#include "settings.h"

#include <KDebug>
#include <KDesktopFile>
#include <KGlobal>
#include <KStandardDirs>


GlobalSettingsWidget::GlobalSettingsWidget( QWidget *parent )
    :   HotkeysWidgetIFace( parent )
        ,_model(NULL)
    {
    ui.setupUi(this);

    QString path = KGlobal::dirs()->findResource( "services", "kded/khotkeys.desktop");
    if ( KDesktopFile::isDesktopFile(path) )
        {
        _config = KSharedConfig::openConfig(
                path,
                KConfig::NoGlobals,
                "services" );
        }

    connect(
            ui.enabled, SIGNAL(stateChanged(int)),
            _changedSignals, SLOT(map()) );
    _changedSignals->setMapping(ui.enabled, "enabled" );

    connect(
            ui.gestures_enabled, SIGNAL(stateChanged(int)),
            _changedSignals, SLOT(map()) );
    _changedSignals->setMapping(ui.gestures_enabled, "gestures_enabled" );
    }


GlobalSettingsWidget::~GlobalSettingsWidget()
    {
    }


void GlobalSettingsWidget::doCopyFromObject()
    {
    if (_config)
        {
        KConfigGroup file(_config, "Desktop Entry");
        ui.enabled->setChecked(file.readEntry("X-KDE-Kded-autoload", false));
        }

    ui.gestures_group->setVisible(_model);
    if (_model)
        {
        KHotKeys::Settings *settings = _model->settings();
        Q_ASSERT(settings);
        kDebug() << settings->areGesturesDisabled();
        ui.gestures_enabled->setChecked(!settings->areGesturesDisabled());
        }

    }


void GlobalSettingsWidget::doCopyToObject()
    {
    kDebug();
    if (_config)
        {
        KConfigGroup file(_config, "Desktop Entry");
        file.writeEntry("X-KDE-Kded-autoload", ui.enabled->checkState()==Qt::Checked);
        _config->sync();
        }

    if (_model)
        {
        KHotKeys::Settings *settings = _model->settings();
        Q_ASSERT(settings);
        ui.gestures_enabled->isChecked()
            ? settings->enableGestures()
            : settings->disableGestures();
        }
    }


bool GlobalSettingsWidget::isChanged() const
    {
    if (_config)
        {
        KConfigGroup file(_config, "Desktop Entry");
        bool enabled = file.readEntry("X-KDE-Kded-autoload", false);

        if (enabled!=ui.enabled->isChecked())
            {
            return true;
            }
        }

    if (_model)
        {
        KHotKeys::Settings *settings = _model->settings();
        Q_ASSERT(settings);
        if ((!settings->areGesturesDisabled()) != ui.gestures_enabled->isChecked())
            {
            return true;
            }
        }

    return false;
    }


void GlobalSettingsWidget::setModel(KHotkeysModel *model)
    {
    _model = model;
    }


#include "moc_global_settings_widget.cpp"
