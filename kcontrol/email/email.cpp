/**
 * email.cpp
 *
 * Copyright (c) 1999, 2000 Preston Brown <pbrown@kde.org>
 * Copyright (c) 2000 Frerich Raabe <raabe@kde.org>
 *
 * $Id$
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qcheckbox.h>

#include <kfiledialog.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kdialog.h>

#include "email.h"

KEmailConfig::KEmailConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QString wtstr;
  QVBoxLayout *topLayout = new QVBoxLayout(this, KDialog::marginHint(),
                       KDialog::spacingHint());
  QGroupBox *uBox = new QGroupBox(2, Qt::Horizontal, i18n("User information"),
                  this);
  topLayout->addWidget(uBox);

  QLabel *label = new QLabel(i18n("&Full Name:"), uBox);

  fullName = new KLineEdit(uBox);
  connect(fullName, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(fullName);

  wtstr = i18n("Enter your full name here, e.g. \"John Doe\" (without the quotation"
     " marks).  Some people like to provide a nick name only. You can leave this field"
     " blank and still use email. However, providing your full name is <em>recommended</em> as"
     " this makes it much easier for your recipient to browse his or her email.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( fullName, wtstr );

  label = new QLabel(i18n("Or&ganization:"), uBox);

  organization = new KLineEdit(uBox);
  connect(organization, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(organization);

  wtstr = i18n("Here you can enter the name of your organization, company"
     " or university. This field is <em>optional</em>. However, if"
     " you are using a business account and communicate with persons working for other"
     " companies, providing the name of your organization is recommended.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( organization, wtstr );

  label = new QLabel(i18n("E-mail &Address:"), uBox);

  emailAddr = new KLineEdit(uBox);
  connect(emailAddr, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(emailAddr);

  wtstr = i18n("Enter your email address here, e.g. \"john@doe.com\" (without "
     "the quotation marks). This information is mandatory if you want to use email.<p>"
     "Do <em>not</em> enter something like \"John Doe &lt;john@doe.com&gt;\", just a plain email address. "
     "Your email address may not contain any blank spaces.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( emailAddr, wtstr );

  label = new QLabel(i18n("&Reply Address:"), uBox);

  replyAddr = new KLineEdit(uBox);
  connect(replyAddr, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(replyAddr);

  wtstr = i18n("You can set a reply address if you want replies to your e-mail messages"
     " to go to a different address than the e-mail address above. Most likely, you should"
     " leave the reply address blank, so replies go to the e-mail address you entered above.<p>"
     " <em>Please note:</em> <ul><li>You do not need to enter the same email"
     " address as above.</li><li>You should not use a reply address if you frequently"
     " use discussion mailing lists.</li></ul>");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( replyAddr, wtstr );

  uBox = new QGroupBox(2, Qt::Horizontal, i18n("Server information"),
               this);
  topLayout->addWidget(uBox);

  label = new QLabel(i18n("User &name:"), uBox);

  userName = new KLineEdit(uBox);
  connect(userName, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(userName);

  wtstr = i18n("The user name you use to login to your e-mail server (sometimes just called \"login\")."
     " Your e-mail provider should have supplied this information. Your login name is often (but"
     " not always) identical to the part of your email address that comes before the \"@\".");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( userName, wtstr );

  label = new QLabel(i18n("&Password:"), uBox);

  password = new KLineEdit(uBox);
  password->setEchoMode(QLineEdit::Password);
  connect(password, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(password);

  wtstr = i18n("Your password for the e-mail server. Your e-mail provider should have supplied"
     " this information along with your user name. <br>Your password will not"
     " appear on screen and will not be readable by other normal users on the system.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( password, wtstr );

  label = new QLabel(i18n("&Incoming host:"), uBox);

  inServer = new KLineEdit(uBox);
  connect(inServer, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));
  label->setBuddy(inServer);

  wtstr = i18n("The server you get incoming e-mail from (this <em>may</em> be identical to your outgoing host)."
     " Your e-mail provider should have supplied this information. It may have been labeled \"POP3 server/host\" or"
     " \"IMAP server/host\". If you are using a local mailbox, you may leave this blank.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( inServer, wtstr );

  label = new QLabel(i18n("O&utgoing host:"), uBox);

  outServer = new KLineEdit(uBox);
  connect(outServer, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));

  label->setBuddy(outServer);

  wtstr = i18n("The server you use for sending e-mail (this <em>may</em> be identical to your incoming host)."
     " Your e-mail provider should have supplied this information. It may have been labeled \"SMTP server\" or"
     " \"SMTP host\". If you are using a local mailbox, you may leave this blank.");
  QWhatsThis::add( label, wtstr );
  QWhatsThis::add( outServer, wtstr );

  bGrp = new QButtonGroup(1, Qt::Vertical,
               i18n("Incoming mail server type"), this);
  connect(bGrp, SIGNAL(clicked(int)), this, SLOT(configChanged()));

  topLayout->addWidget(bGrp);

  imapButton = new QRadioButton(i18n("&IMAP"), bGrp);
  pop3Button = new QRadioButton(i18n("P&OP3"), bGrp);
  localButton = new QRadioButton(i18n("&Local mailbox"), bGrp);

  wtstr = i18n("This is the protocol used by your incoming e-mail server. Your e-mail provider should have"
     " supplied this information. If you use dial-up networking, you are probably using a POP3 server.");
  QWhatsThis::add( bGrp, wtstr );
  QWhatsThis::add( imapButton, wtstr );
  QWhatsThis::add( pop3Button, wtstr );
  QWhatsThis::add( localButton, wtstr );

  uBox = new QGroupBox(2, Qt::Horizontal, i18n("Preferred email client"),
                        this);
  topLayout->addWidget(uBox);

  emailClient = new KLineEdit(uBox);
  connect(emailClient, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));

  bEmailClient = new QPushButton(i18n("Bro&wse..."), uBox);
  connect(bEmailClient, SIGNAL(clicked()), this, SLOT(selectEmailClient()));

  cTerminalClient = new QCheckBox(i18n("Run in &terminal"), uBox);

  wtstr = i18n("Enter the path to your preferred email client (KMail, Mutt, etc.) here or"
        " choose it with the <em>Browse...</em> button. If no client is specified here,"
        " KMail will be used (if available) instead.");

  QWhatsThis::add(emailClient, wtstr);
  QWhatsThis::add(bEmailClient, i18n("Press this button to select your favorite email client. Please"
        " note that the file you select has to have the executable attribute set in order to be"
        " accepted."));
  QWhatsThis::add(cTerminalClient, i18n("Activate this option if you want the selected email client"
        " to be executed in a terminal (e.g. <em>Konsole</em>)."));

  topLayout->addSpacing(KDialog::spacingHint());

  load();
}

KEmailConfig::~KEmailConfig()
{
}

void KEmailConfig::configChanged()
{
    emit changed(true);
}


void KEmailConfig::load()
{
  KConfig *config = new KConfig("emaildefaults");
  char hostname[80];
  struct passwd *p;

  p = getpwuid(getuid());
  gethostname(hostname, 80);

  config->setGroup("UserInfo");
  fullName->setText(config->readEntry("FullName", p->pw_gecos));
  QString tmp = p->pw_name;
  tmp += "@"; tmp += hostname;
  emailAddr->setText(config->readEntry("EmailAddress", tmp));
  organization->setText(config->readEntry("Organization", "None"));
  replyAddr->setText(config->readEntry("ReplyAddr"));

  config->setGroup("ServerInfo");
  userName->setText(config->readEntry("UserName", p->pw_name));
  password->setText(config->readEntry("Password"));
  inServer->setText(config->readEntry("Incoming"));
  outServer->setText(config->readEntry("Outgoing", hostname));

  bGrp->setButton(config->readNumEntry("ServerType", 0));

  emailClient->setText(config->readEntry("EmailClient", "kmail"));
  cTerminalClient->setChecked(config->readBoolEntry("TerminalClient", false));

  emit changed(false);
  delete config;
}

void KEmailConfig::save()
{
  KConfig *config = new KConfig("emaildefaults");

  config->setGroup("UserInfo");
  config->writeEntry("FullName", fullName->text());
  config->writeEntry("EmailAddress", emailAddr->text());
  config->writeEntry("Organization", organization->text());
  config->writeEntry("ReplyAddr", replyAddr->text());

  config->setGroup("ServerInfo");
  config->writeEntry("UserName", userName->text());
  config->writeEntry("Password", password->text());
  config->writeEntry("Incoming", inServer->text());
  config->writeEntry("Outgoing", outServer->text());
  int sType;
  if (imapButton->isChecked())
    sType = 0;
  else if (pop3Button->isChecked())
    sType = 1;
  else
    sType = 2;
  config->writeEntry("ServerType", sType);

  config->writeEntry("EmailClient", emailClient->text());
  config->writeEntry("TerminalClient", cTerminalClient->isChecked());

  config->sync();

  // insure proper permissions -- contains sensitive data
  QString cfgName(KGlobal::dirs()->findResource("config", "kcmemailrc"));
  if (!cfgName.isEmpty())
    ::chmod(cfgName.utf8(), 0600);

  emit changed(false);
  delete config;
}

void KEmailConfig::defaults()
{
  char hostname[80];
  struct passwd *p;

  p = getpwuid(getuid());
  gethostname(hostname, 80);

  fullName->setText(p->pw_gecos);

  QString tmp = p->pw_name;
  tmp += "@"; tmp += hostname;

  emailAddr->setText(tmp);

  QString client;
  client = KGlobal::dirs()->findResource("exe", "kmail");

  if (client.isEmpty())
    client = "kmail";

  emailClient->setText(client);

  cTerminalClient->setChecked(false);

  emit changed(true);
}

QString KEmailConfig::quickHelp() const
{
  return i18n("<h1>e-mail</h1> This module allows you to enter basic e-mail"
     " information for the current user. The information here is used,"
     " among other things, for sending bug reports to the KDE developers"
     " when you use the bug report dialog.<p>"
     " Note that e-mail programs like KMail and Empath offer many more"
     " features, but they provide their own configuration facilities.");
}

void KEmailConfig::selectEmailClient()
{
  QString client = KFileDialog::getOpenFileName(QString::null, "*", this);

  QFileInfo *clientInfo = new QFileInfo(client);
  if ((clientInfo->exists()) && (clientInfo->isExecutable()))
    emailClient->setText(client);
}

extern "C"
{
  KCModule *create_email(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmemail");
    return new KEmailConfig(parent, name);
  };
}
#include "email.moc"
