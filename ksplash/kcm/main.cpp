/***************************************************************************
 *   Copyright Ravikiran Rajagopal 2003                                    *
 *   ravi@ee.eng.ohio-state.edu                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License (version 2) as   *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlayout.h>
#include <qtabwidget.h>

#include <kaboutdata.h>
#include <kcmodule.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>

#include "installer.h"

class KSplashThemeMgr : public KCModule
{
public:
  KSplashThemeMgr( QWidget *parent, const char *name, const QStringList &/*unused*/);
  ~KSplashThemeMgr();

  QString quickHelp() const;

  virtual void init();
  virtual void save();
  virtual void load();
  virtual void defaults();

  const KAboutData *aboutData() const;

private:
  SplashInstaller *mInstaller;
};

typedef KGenericFactory< KSplashThemeMgr, QWidget > KSplashThemeMgrFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_ksplashthemes, KSplashThemeMgrFactory("ksplashthemes") )

// -----------------------------------------------------------------------------------------

KSplashThemeMgr::KSplashThemeMgr( QWidget *parent, const char *name, const QStringList &)
  : KCModule( KSplashThemeMgrFactory::instance(), parent, name ), mInstaller(new SplashInstaller(this))
{
  init();

  QHBoxLayout *box = new QHBoxLayout(this);
  QTabWidget *tab = new QTabWidget(this); // There will be more tabs in the future.
  box->addWidget(tab);
  tab->addTab( mInstaller, i18n("&Theme Installer") );
  connect( mInstaller, SIGNAL(changed(bool)), SIGNAL(changed(bool)) );
}

KSplashThemeMgr::~KSplashThemeMgr()
{
  // Do not delete the installer as it is now owned by the tab widget.
}

QString KSplashThemeMgr::quickHelp() const
{
  return i18n("<h1>Splash Screen Theme Manager </h1> Install and view splash screen themes.");
}

void KSplashThemeMgr::init()
{
  KGlobal::dirs()->addResourceType("ksplashthemes", KStandardDirs::kde_default("data") + "ksplash/Themes");
}

void KSplashThemeMgr::save()
{
  mInstaller->save();
}

void KSplashThemeMgr::load()
{
  mInstaller->load();
}

void KSplashThemeMgr::defaults()
{
  mInstaller->defaults();
}

const KAboutData* KSplashThemeMgr::aboutData() const
{
  KAboutData *about = new KAboutData( I18N_NOOP("kcmksplash")
                                      ,I18N_NOOP("KDE Splash Screen Theme Manager")
                                      ,"0.1"
                                      ,0
                                      ,KAboutData::License_GPL
                                      ,I18N_NOOP("(c) 2003 KDE developers") );
  about->addAuthor("Ravikiran Rajagopal", 0, "ravi@ee.eng.ohio-state.edu");
  about->addCredit("Brian Ledbetter", I18N_NOOP("Original KSplash/ML author"), "brian@shadowcom.net");
  about->addCredit("KDE Theme Manager authors", I18N_NOOP("Original installer code") );
  return about;
}
