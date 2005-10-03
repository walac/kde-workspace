/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>

#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kapplication.h>
#include <krun.h>
#include <ktoolinvocation.h>

#include "global.h"
#include "quickhelp.h"
#include "helpwidget.h"
#include "helpwidget.moc"

HelpWidget::HelpWidget(QWidget *parent , const char *name)
  : QWidget(parent, name)
{
  QVBoxLayout *l = new QVBoxLayout(this);

  _browser = new QuickHelp(this);
  connect(_browser, SIGNAL(urlClick(const QString &)),
	  SLOT(urlClicked(const QString &)));
  connect(_browser, SIGNAL(mailClick(const QString &,const QString &)),
	  SLOT(mailClicked(const QString &,const QString &)));

  l->addWidget(_browser);

  setBaseText();
}

void HelpWidget::setText( const QString& docPath, const QString& text)
{
  docpath = docPath;
  if (text.isEmpty() && docPath.isEmpty())
    setBaseText();
  else if (docPath.isEmpty())
    _browser->setText(text);
  else
  {
    QByteArray a = docPath.toLocal8Bit();
    QString path = QString::fromLocal8Bit (a.data(), a.size());

    _browser->setText(text + i18n("<p>Use the \"Whats This\" (Shift+F1) to get help on specific options.</p><p>To read the full manual click <a href=\"%1\">here</a>.</p>")
		      .arg(path));
  }
}

void HelpWidget::setBaseText()
{
  if (KCGlobal::isInfoCenter())
     _browser->setText(i18n("<h1>KDE Info Center</h1>"
			 "There is no quick help available for the active info module."
			 "<br><br>"
			 "Click <a href = \"kinfocenter/index.html\">here</a> to read the general Info Center manual.") );
  else
     _browser->setText(i18n("<h1>KDE Control Center</h1>"
			 "There is no quick help available for the active control module."
			 "<br><br>"
			 "Click <a href = \"kcontrol/index.html\">here</a> to read the general Control Center manual.") );
}

void HelpWidget::urlClicked(const QString & _url)
{
    KProcess process;
    KURL url(KURL("help:/"), _url);

    if (url.protocol() == "help" || url.protocol() == "man" || url.protocol() == "info") {
        process << "khelpcenter"
                << url.url();
        process.start(KProcess::DontCare);
    } else {
        new KRun(url, this);
    }
}

void HelpWidget::mailClicked(const QString &,const QString & addr)
{
  KToolInvocation::invokeMailer(addr, QString::null);
}
