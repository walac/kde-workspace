/* This file is part of the KDE Display Manager Configuration package
    Copyright (C) 1997-1998 Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <unistd.h>
#include <sys/types.h>


#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <ksimpleconfig.h>
#include <karrowbutton.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdialog.h>
#include <kurlrequester.h>

#include "kdm-sess.h"


extern KSimpleConfig *config;

KDMSessionsWidget::KDMSessionsWidget(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
      QString wtstr;


      QGroupBox *group0 = new QGroupBox( i18n("Allow Shutdown"), this );

      sdlcombo = new QComboBox( FALSE, group0 );
      sdllabel = new QLabel (sdlcombo, i18n ("Co&nsole:"), group0);
      sdlcombo->insertItem(i18n("Everybody"), SdAll);
      sdlcombo->insertItem(i18n("Only root"), SdRoot);
      sdlcombo->insertItem(i18n("Nobody"), SdNone);
      connect(sdlcombo, SIGNAL(activated(int)), SLOT(changed()));
      sdrcombo = new QComboBox( FALSE, group0 );
      sdrlabel = new QLabel (sdrcombo, i18n ("Re&mote:"), group0);
      sdrcombo->insertItem(i18n("Everybody"), SdAll);
      sdrcombo->insertItem(i18n("Only root"), SdRoot);
      sdrcombo->insertItem(i18n("Nobody"), SdNone);
      connect(sdrcombo, SIGNAL(activated(int)), SLOT(changed()));
      QWhatsThis::add( group0, i18n("Here you can select who is allowed to shutdown"
        " the computer using KDM. You can specify different values for local (console) and remote displays. "
	"Possible values are:<ul>"
        " <li><em>Everybody:</em> everybody can shutdown the computer using KDM</li>"
        " <li><em>Only root:</em> KDM will only allow shutdown after the user has entered the root password</li>"
        " <li><em>Nobody:</em> nobody can shutdown the computer using KDM</li></ul>") );


      QGroupBox *group1 = new QGroupBox( i18n("Commands"), this );

      shutdown_lined = new KURLRequester(group1);
      QLabel *shutdown_label = new QLabel(shutdown_lined, i18n("Ha&lt:"), group1);
      connect(shutdown_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(changed()));
      wtstr = i18n("Command to initiate the system halt. Typical value: /sbin/halt");
      QWhatsThis::add( shutdown_label, wtstr );
      QWhatsThis::add( shutdown_lined, wtstr );

      restart_lined = new KURLRequester(group1);
      QLabel *restart_label = new QLabel(restart_lined, i18n("&Reboot:"), group1);
      connect(restart_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(changed()));
      wtstr = i18n("Command to initiate the system reboot. Typical value: /sbin/reboot");
      QWhatsThis::add( restart_label, wtstr );
      QWhatsThis::add( restart_lined, wtstr );

#if defined(__linux__) && defined(__i386__)
      QGroupBox *group4 = new QGroupBox( i18n("LILO"), this );

      lilo_check = new QCheckBox(i18n("Show boot opt&ions"), group4);
      connect(lilo_check, SIGNAL(toggled(bool)),
	      SLOT(slotLiloCheckToggled(bool)));
      connect(lilo_check, SIGNAL(toggled(bool)),
	      SLOT(changed()));
      wtstr = i18n("Enable LILO boot options in the \"Shutdown...\" dialog.");
      QWhatsThis::add( lilo_check, wtstr );

      lilocmd_lined = new KURLRequester(group4);
      lilocmd_label = new QLabel(lilocmd_lined , i18n("LILO command:"), group4);
      connect(lilocmd_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(changed()));
      wtstr = i18n("Command to run LILO. Typical value: /sbin/lilo");
      QWhatsThis::add( lilocmd_label, wtstr );
      QWhatsThis::add( lilocmd_lined, wtstr );

      lilomap_lined = new KURLRequester(group4);
      lilomap_label = new QLabel(lilomap_lined, i18n("LILO map file:"), group4);
      connect(lilomap_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(changed()));
      wtstr = i18n("Position of Lilo's map file. Typical value: /boot/map");
      QWhatsThis::add( lilomap_label, wtstr );
      QWhatsThis::add( lilomap_lined, wtstr );
#endif

      QGroupBox *group2 = new QGroupBox( i18n("Session Types"), this );

      session_lined = new QLineEdit(group2);
      QLabel *type_label = new QLabel(session_lined, i18n("New t&ype:"), group2);
      connect(session_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(slotCheckNewSession(const QString&)));
      connect(session_lined, SIGNAL(returnPressed()),
	      SLOT(slotAddSessionType()));
      connect(session_lined, SIGNAL(returnPressed()),
	      SLOT(changed()));
      wtstr = i18n( "To create a new session type, enter its name here and click on <em>Add new</em>" );
      QWhatsThis::add( type_label, wtstr );
      QWhatsThis::add( session_lined, wtstr );

      btnadd = new QPushButton( i18n("Add Ne&w"), group2 );
      btnadd->setEnabled(false);
      connect( btnadd, SIGNAL( clicked() ), SLOT( changed() ) );
      connect( btnadd, SIGNAL( clicked() ), SLOT( slotAddSessionType() ) );
      QWhatsThis::add( btnadd, i18n( "Click here to add the new session type entered in the <em>New type</em> field to the list of available sessions." ) );

      sessionslb = new MyListBox(group2);
      QLabel *types_label = new QLabel(sessionslb, i18n("Available &types:"), group2);
      connect(sessionslb, SIGNAL(highlighted(int)),
	      SLOT(slotSessionHighlighted(int)));
      wtstr = i18n( "This box lists the available session types that will be presented to the user."
		    " Names other than \"default\" and \"failsafe\" are usually treated as program names,"
		    " but it depends on your Xsession script what the session type means." );
      QWhatsThis::add( types_label, wtstr );
      QWhatsThis::add( sessionslb, wtstr );

      btnrm = new QPushButton( i18n("R&emove"), group2 );
      btnrm->setEnabled(false);
      connect( btnrm, SIGNAL( clicked() ), SLOT( slotRemoveSessionType() ) );
      connect( btnrm, SIGNAL( clicked() ), SLOT( changed() ) );
      QWhatsThis::add( btnrm, i18n( "Click here to remove the currently selected session type" ) );

      btnup = new KArrowButton(group2, UpArrow);
      btnup->setEnabled(false);
      btnup->setFixedSize(20, 20);
      connect(btnup, SIGNAL( clicked() ), SLOT( slotSessionUp() ));
      connect(btnup, SIGNAL( clicked() ), SLOT( changed() ));
      btndown = new KArrowButton(group2, DownArrow);
      btndown->setEnabled(false);
      btndown->setFixedSize(20, 20);
      connect(btndown,SIGNAL( clicked() ), SLOT( slotSessionDown() ));
      connect(btndown,SIGNAL( clicked() ), SLOT( changed() ));
      wtstr = i18n( "With these two arrow buttons, you can change the order in which the available session types are presented to the user" );
      QWhatsThis::add( btnup, wtstr );
      QWhatsThis::add( btndown, wtstr );

      QBoxLayout *main = new QVBoxLayout( this, 10 );
      QGridLayout *lgroup0 = new QGridLayout( group0, 3, 5, 10);
      QGridLayout *lgroup1 = new QGridLayout( group1, 3, 5, 10);
#if defined(__linux__) && defined(__i386__)
      QGridLayout *lgroup4 = new QGridLayout( group4, 3, 4, 10);
#endif
      QGridLayout *lgroup2 = new QGridLayout( group2, 5, 5, 10);

      main->addWidget(group0);
      main->addWidget(group1);
#if defined(__linux__) && defined(__i386__)
      main->addWidget(group4);
#endif
      main->addWidget(group2);

      lgroup0->addRowSpacing(0, group0->fontMetrics().height()/2);
      lgroup0->addColSpacing(2, KDialog::spacingHint() * 2);
      lgroup0->setColStretch(1, 1);
      lgroup0->setColStretch(4, 1);
      lgroup0->addWidget(sdllabel, 1, 0);
      lgroup0->addWidget(sdlcombo, 1, 1);
      lgroup0->addWidget(sdrlabel, 1, 3);
      lgroup0->addWidget(sdrcombo, 1, 4);

      lgroup1->addRowSpacing(0, group1->fontMetrics().height()/2);
      lgroup1->addColSpacing(2, KDialog::spacingHint() * 2);
      lgroup1->setColStretch(1, 1);
      lgroup1->setColStretch(4, 1);
      lgroup1->addWidget(shutdown_label, 1, 0);
      lgroup1->addWidget(shutdown_lined, 1, 1);
      lgroup1->addWidget(restart_label, 1, 3);
      lgroup1->addWidget(restart_lined, 1, 4);

#if defined(__linux__) && defined(__i386__)
      lgroup4->addRowSpacing(0, group4->fontMetrics().height()/2);
      lgroup4->addColSpacing(1, KDialog::spacingHint() * 2);
      lgroup4->setColStretch(3, 1);
      lgroup4->addWidget(lilo_check, 1, 0);
      lgroup4->addWidget(lilocmd_label, 1, 2);
      lgroup4->addWidget(lilocmd_lined, 1, 3);
      lgroup4->addWidget(lilomap_label, 2, 2);
      lgroup4->addWidget(lilomap_lined, 2, 3);
#endif

      lgroup2->addRowSpacing(0, group2->fontMetrics().height()/2);
      lgroup2->addWidget(type_label, 1, 0);
      lgroup2->addMultiCellWidget(session_lined, 2, 2, 0, 2);
      lgroup2->addWidget(types_label, 1, 3);
      lgroup2->addMultiCellWidget(sessionslb, 2, 5, 3, 3);
      lgroup2->addWidget(btnadd, 3, 0);
      lgroup2->addWidget(btnrm, 3, 2);
      lgroup2->addWidget(btnup, 2, 4);
      lgroup2->addWidget(btndown, 3, 4);
      lgroup2->setColStretch(1, 1);
      lgroup2->setColStretch(3, 2);
      lgroup2->setRowStretch(4, 1);

      main->activate();

}

void KDMSessionsWidget::makeReadOnly()
{
    sdlcombo->setEnabled(false);
    sdrcombo->setEnabled(false);

    restart_lined->lineEdit()->setReadOnly(true);
    restart_lined->button()->setEnabled(false);
    shutdown_lined->lineEdit()->setReadOnly(true);
    shutdown_lined->button()->setEnabled(false);
#if defined(__linux__) && defined(__i386__)
    lilo_check->setEnabled(false);
    lilocmd_lined->lineEdit()->setReadOnly(true);
    lilocmd_lined->button()->setEnabled(false);
    lilomap_lined->lineEdit()->setReadOnly(true);
    lilomap_lined->button()->setEnabled(false);
#endif
    session_lined->setReadOnly(true);
    sessionslb->setEnabled(false);
    btnup->setEnabled(false);
    btndown->setEnabled(false);
    btnrm->setEnabled(false);
    btnadd->setEnabled(false);
}

void KDMSessionsWidget::slotLiloCheckToggled(bool on)
{
#if defined(__linux__) && defined(__i386__)
    lilocmd_label->setEnabled(on);
    lilocmd_lined->setEnabled(on);
    lilomap_label->setEnabled(on);
    lilomap_lined->setEnabled(on);
#endif
}

void KDMSessionsWidget::slotSessionHighlighted(int s)
{
  session_lined->setText(sessionslb->text(s));
  btnup->setEnabled(s > 0);
  btndown->setEnabled(s < (int)sessionslb->count()-1);
  btnrm->setEnabled(sessionslb->currentItem() > -1);
  if(!sessionslb->isItemVisible(s))
    sessionslb->centerCurrentItem();
}

void KDMSessionsWidget::slotCheckNewSession(const QString& str)
{
  btnadd->setEnabled(!str.isEmpty());
}

void KDMSessionsWidget::slotSessionUp()
{
  moveSession(-1);
}

void KDMSessionsWidget::slotSessionDown()
{
  moveSession(1);
}

void KDMSessionsWidget::moveSession(int d)
{
  int id = sessionslb->currentItem();
  QString str = sessionslb->text(id);
  sessionslb->removeItem(id);
  sessionslb->insertItem(str, id+d);
  sessionslb->setCurrentItem(id+d);
}

void KDMSessionsWidget::slotAddSessionType()
{
  if(!session_lined->text().isEmpty())
  {
    sessionslb->insertItem(session_lined->text());
    session_lined->clear();
  }
}

void KDMSessionsWidget::slotRemoveSessionType()
{
  int i = sessionslb->currentItem();
  if(i > -1)
    sessionslb->removeItem(i);
}

void KDMSessionsWidget::writeSD(QComboBox *combo)
{
    QString what;
    switch (combo->currentItem()) {
    case SdAll: what = "All"; break;
    case SdRoot: what = "Root"; break;
    default: what = "None"; break;
    }
    config->writeEntry( "AllowShutdown", what);
}

void KDMSessionsWidget::save()
{
    config->setGroup("X-:*-Core");
    writeSD(sdlcombo);

    config->setGroup("X-*-Core");
    writeSD(sdrcombo);

    config->setGroup("X-*-Greeter");
    QString sesstr;
    for(uint i = 0; i < sessionslb->count(); i++)
    {
      sesstr.append(sessionslb->text(i));
      sesstr.append(",");
    }
    config->writeEntry( "SessionTypes", sesstr );

    config->setGroup("Shutdown");
    config->writeEntry("HaltCmd", shutdown_lined->url(), true);
    config->writeEntry("RebootCmd", restart_lined->url(), true);
#if defined(__linux__) && defined(__i386__)
    config->writeEntry("UseLilo", lilo_check->isChecked());
    config->writeEntry("LiloCmd", lilocmd_lined->url());
    config->writeEntry("LiloMap", lilomap_lined->url());
#endif
}

void KDMSessionsWidget::readSD(QComboBox *combo, QString def)
{
  QString str = config->readEntry("AllowShutdown", def);
  SdModes sdMode;
  if(str == "All")
    sdMode = SdAll;
  else if(str == "Root")
    sdMode = SdRoot;
  else
    sdMode = SdNone;
  combo->setCurrentItem(sdMode);
}

void KDMSessionsWidget::load()
{
  QString str;

  config->setGroup("X-:*-Core");
  readSD(sdlcombo, "All");

  config->setGroup("X-*-Core");
  readSD(sdrcombo, "Root");

  config->setGroup("X-*-Greeter");
  QStringList sessions = config->readListEntry( "SessionTypes");
  if (sessions.isEmpty())
    sessions << "default" << "kde" << "failsafe";
  sessionslb->clear();
  sessionslb->insertStringList(sessions);

  config->setGroup("Shutdown");
  restart_lined->setURL(config->readEntry("RebootCmd", "/sbin/reboot"));
  shutdown_lined->setURL(config->readEntry("HaltCmd", "/sbin/halt"));
#if defined(__linux__) && defined(__i386__)
  bool lien = config->readBoolEntry("UseLilo", false);
  lilo_check->setChecked(lien);
  slotLiloCheckToggled(lien);
  lilocmd_lined->setURL(config->readEntry("LiloCmd", "/sbin/lilo"));
  lilomap_lined->setURL(config->readEntry("LiloMap", "/boot/map"));
#endif
}



void KDMSessionsWidget::defaults()
{
  restart_lined->setURL("/sbin/reboot");
  shutdown_lined->setURL("/sbin/halt");

  sdlcombo->setCurrentItem(SdAll);
  sdrcombo->setCurrentItem(SdRoot);

  sessionslb->clear();
  sessionslb->insertItem("default");
  sessionslb->insertItem("kde");
  sessionslb->insertItem("failsafe");

#if defined(__linux__) && defined(__i386__)
    lilo_check->setChecked(false);
    slotLiloCheckToggled(false);

    lilocmd_lined->setURL("/sbin/lilo");
    lilomap_lined->setURL("/boot/map");
#endif
}


void KDMSessionsWidget::changed()
{
  emit changed(true);
}

#include "kdm-sess.moc"
