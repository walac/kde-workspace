/*
 *  advancedDialog.cpp
 *
 *  Copyright (c) 2002 Aaron J. Seigo <aseigo@olympusproject.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include <kcolorbutton.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include <klocale.h>

#include "advancedDialog.h"
#include "advancedOptions.h"
#include "main.h"

advancedDialog::advancedDialog(QWidget* parent, const char* name)
    : KDialogBase(KDialogBase::Plain,
                  i18n("Advanced Options"),
                  KDialogBase::Ok |
                  KDialogBase::Apply |
                  KDialogBase::Cancel,
                  KDialogBase::Cancel,
                  parent,
                  name,
                  true, true)
{
    connect(this, SIGNAL(applyClicked()),
            this, SLOT(save()));
    connect(this, SIGNAL(okClicked()),
            this, SLOT(save()));
    actionButton(Apply)->setEnabled(false);
    QFrame* page = plainPage();
    QVBoxLayout* layout = new QVBoxLayout(page);
    m_advancedWidget = new advancedKickerOptions(page);
    layout->addWidget(m_advancedWidget);

    connect(m_advancedWidget->fadeOutHandles, SIGNAL(toggled(bool)),
            this, SLOT(changed()));
    connect(m_advancedWidget->hideButtonSize, SIGNAL(valueChanged(int)),
            this, SLOT(changed()));
    connect(m_advancedWidget->resizeableHandle, SIGNAL(toggled(bool)),
            this, SLOT(changed()));
    connect(m_advancedWidget->tintColorB, SIGNAL(clicked()),
            this, SLOT(changed()));
    connect(m_advancedWidget->tintSlider, SIGNAL(valueChanged(int)),
            this, SLOT(changed()));
    load();
}

advancedDialog::~advancedDialog()
{
}

void advancedDialog::load()
{
    KConfig c(KickerConfig::configName(), false, false);
    c.setGroup("General");
    bool fadedOut = c.readBoolEntry("FadeOutAppletHandles", false);
    m_advancedWidget->fadeOutHandles->setChecked(fadedOut);
    int defaultHideButtonSize = c.readNumEntry("HideButtonSize", 14);
    m_advancedWidget->hideButtonSize->setValue(defaultHideButtonSize);
    bool resizeableHandle = c.readBoolEntry( "ResizeableHandle", false);
    m_advancedWidget->resizeableHandle->setChecked(resizeableHandle);
    QColor color = c.readColorEntry( "TintColor", &colorGroup().mid() );
    m_advancedWidget->tintColorB->setColor( color );
    int tintValue = c.readNumEntry( "TintValue", 0 );
    m_advancedWidget->tintSlider->setValue( tintValue );

    actionButton(Apply)->setEnabled(false);
}

void advancedDialog::save()
{
    KConfig c(KickerConfig::configName(), false, false);

    c.setGroup("General");
    c.writeEntry("FadeOutAppletHandles",
                 m_advancedWidget->fadeOutHandles->isChecked());
    c.writeEntry("HideButtonSize",
                 m_advancedWidget->hideButtonSize->value());
    c.writeEntry("TintColor",
                 m_advancedWidget->tintColorB->color());
    c.writeEntry("TintValue",
                 m_advancedWidget->tintSlider->value());

    QStringList elist = c.readListEntry("Extensions2");
    for (QStringList::Iterator it = elist.begin(); it != elist.end(); ++it)
    {
        // extension id
        QString group(*it);

        // is there a config group for this extension?
        if(!c.hasGroup(group) ||
           group.contains("Extension") < 1)
        {
            continue;
        }

        // set config group
        c.setGroup(group);
        KConfig extConfig(c.readEntry("ConfigFile"));
        extConfig.setGroup("General");
        extConfig.writeEntry("FadeOutAppletHandles",
                             m_advancedWidget->fadeOutHandles->isChecked());
        extConfig.writeEntry("HideButtonSize",
                             m_advancedWidget->hideButtonSize->value());
        extConfig.writeEntry("TintColor",
                             m_advancedWidget->tintColorB->color());
        extConfig.writeEntry("TintValue",
                             m_advancedWidget->tintSlider->value());

        extConfig.sync();
    }
    c.writeEntry("ResizeableHandle",
                 m_advancedWidget->resizeableHandle->isChecked());
    c.sync();

    KickerConfig::notifyKicker();
    actionButton(Apply)->setEnabled(false);
}

void advancedDialog::changed()
{
    actionButton(Apply)->setEnabled(true);
}

#include "advancedDialog.moc"

