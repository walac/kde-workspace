/*
 * Copyright (c) 2000 Yves Arrouye <yves@realnames.com>
 * Copyright (c) 2001, 2002 Dawit Alemayehu <adawit@kde.org>
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <qfile.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

#include <kdebug.h>
#include <kconfig.h>
#include <ktrader.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klistview.h>
#include <dcopclient.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>

#include "ikwsopts.h"
#include "ikwsopts_ui.h"
#include "kuriikwsfiltereng.h"
#include "searchprovider.h"
#include "searchproviderdlg.h"


class SearchProviderItem : public QListViewItem
{
public:
    SearchProviderItem(QListView *parent, SearchProvider *provider)
    :QListViewItem(parent), m_provider(provider)
    {
      update();
    };

    virtual ~SearchProviderItem()
    {
      delete m_provider;
    }

    void update()
    {
      setText(0, m_provider->name());
      setText(1, m_provider->keys().join(","));
    }

    SearchProvider *provider() const { return m_provider; }

private:
    SearchProvider *m_provider;
};

FilterOptions::FilterOptions(KInstance *instance, QWidget *parent, const char *name)
              :KCModule(instance, parent, name)
{
    QVBoxLayout *mainLayout = new QVBoxLayout( this, 0, 0);

    m_dlg = new FilterOptionsUI (this);
    mainLayout->addWidget(m_dlg);

    QString wtstr = i18n("Enable shortcuts that allow you to quickly search for "
                         "information on the web. For example, entering the "
                         "shortcut <em>gg:KDE</em> will result in a search of "
                         "the word <em>KDE</em> on the Google(TM) search engine.");
    QWhatsThis::add(m_dlg->cbEnableShortcuts, wtstr);


    wtstr = i18n("List of search providers and their associated shortcuts.");
    QWhatsThis::add(m_dlg->lvSearchProviders, wtstr);
    m_dlg->lvSearchProviders->setMultiSelection(false);
    m_dlg->lvSearchProviders->addColumn(i18n("Name"));
    m_dlg->lvSearchProviders->addColumn(i18n("Shortcuts"));
    m_dlg->lvSearchProviders->setSorting(0);

    QWhatsThis::add(m_dlg->pbNew, i18n("Add a search provider"));
    QWhatsThis::add(m_dlg->pbChange, i18n("Modify a search provider"));
    QWhatsThis::add(m_dlg->pbDelete, i18n("Delete the selected search provider"));

    wtstr = i18n("Select the search engine to use for input boxes that provide "
                 "automatic lookup services when you type in normal words and "
                 "phrases instead of a URL. To disable this feature select "
                 "<em>None</em> from the list.");
    QWhatsThis::add(m_dlg->lbDefaultEngine, wtstr);
    QWhatsThis::add(m_dlg->cmbDefaultEngine, wtstr);

    wtstr = i18n("Choose the delimiter that separates the word or phrase "
                 "to be looked up from the keyword.");
    QWhatsThis::add(m_dlg->lbDelimiter, wtstr);
    QWhatsThis::add(m_dlg->cmbDelimiter, wtstr);

    m_dlg->pbChange->setEnabled(false);
    m_dlg->pbDelete->setEnabled(false);

    connect(m_dlg->lvSearchProviders, SIGNAL(selectionChanged(QListViewItem *)),
           this, SLOT(updateSearchProvider()));
    connect(m_dlg->lvSearchProviders, SIGNAL(doubleClicked(QListViewItem *)),
           this, SLOT(changeSearchProvider()));
    connect(m_dlg->lvSearchProviders, SIGNAL(returnPressed(QListViewItem *)),
           this, SLOT(changeSearchProvider()));

    connect(m_dlg->cbEnableShortcuts, SIGNAL(clicked()), this,
            SLOT(setWebShortcutState()));
    connect(m_dlg->cmbDefaultEngine, SIGNAL(activated(const QString &)), this,
            SLOT(moduleChanged()));
    connect(m_dlg->cmbDelimiter, SIGNAL(activated(const QString &)), this,
            SLOT(moduleChanged()));

    connect(m_dlg->pbNew, SIGNAL(clicked()), this, SLOT(addSearchProvider()));
    connect(m_dlg->pbChange, SIGNAL(clicked()), this, SLOT(changeSearchProvider()));
    connect(m_dlg->pbDelete, SIGNAL(clicked()), this, SLOT(deleteSearchProvider()));

    // Load the options
    load();
}

QString FilterOptions::quickHelp() const
{
    return i18n("In this module you can configure the web shortcuts feature. "
                "Web shortcuts allow you to quickly search or lookup words on "
                "the Internet. For example, to search for information about the "
                "KDE project using the Google engine, you simply type <b>gg:KDE</b> "
                "or <b>google:KDE</b>."
                "<p>If you select a default search engine, normal words or phrases "
                "will be looked up at the specified search engine by simply typing "
                "them into applications, such as Konqueror, that have built-in support "
                "for such a feature.");
}

void FilterOptions::load()
{
    // Clear state first.
    m_dlg->lvSearchProviders->clear();
    m_dlg->cmbDefaultEngine->clear();

    // Populate the delimiter combobox.
    m_dlg->cmbDelimiter->insertItem (i18n("Colon"), 0);
    m_dlg->cmbDelimiter->insertItem (i18n("Space"), 1);

    // Populate the default search engine combobox.
    m_dlg->cmbDefaultEngine->insertItem (i18n("None"), 0);

    KConfig config( KURISearchFilterEngine::self()->name() + "rc", false, false );
    config.setGroup("General");

    QString defaultSearchEngine = config.readEntry("DefaultSearchEngine");

    const KTrader::OfferList services = KTrader::self()->query("SearchProvider");

    for (KTrader::OfferList::ConstIterator it = services.begin();
         it != services.end(); ++it)
    {
      displaySearchProvider(new SearchProvider(*it),
                            ((*it)->desktopEntryName() == defaultSearchEngine));
    }

    bool webShortcutsEnabled = config.readBoolEntry("EnableWebShortcuts", true);
    m_dlg->cbEnableShortcuts->setChecked( webShortcutsEnabled );

    setDelimiter (config.readNumEntry ("KeywordDelimiter", ':'));

    setWebShortcutState();
    if (m_dlg->lvSearchProviders->childCount())
      m_dlg->lvSearchProviders->setSelected(m_dlg->lvSearchProviders->firstChild(), true);
}

char FilterOptions::delimiter ()
{
  switch (m_dlg->cmbDelimiter->currentItem())
  {
    case 1:
      return ' ';
    case 0:
    default:
      return ':';
  };
}

void FilterOptions::setDelimiter (char sep)
{
  switch (sep)
  {
    case ' ':
      m_dlg->cmbDelimiter->setCurrentItem (1);
      break;
    case ':':
    default:
      m_dlg->cmbDelimiter->setCurrentItem (0);
  };
}

void FilterOptions::save()
{
  KConfig config( KURISearchFilterEngine::self()->name() + "rc", false, false );

  config.setGroup("General");
  config.writeEntry("EnableWebShortcuts", m_dlg->cbEnableShortcuts->isChecked());
  config.writeEntry("KeywordDelimiter", delimiter() );

  QString engine;

  if (m_dlg->cmbDefaultEngine->currentItem() == 0)
    engine = QString::null;
  else
    engine = m_dlg->cmbDefaultEngine->currentText();

  config.writeEntry("DefaultSearchEngine", m_defaultEngineMap[engine]);

  kdDebug () << "Engine: " << m_defaultEngineMap[engine] << endl;

  QString path = kapp->dirs()->saveLocation("services", "searchproviders/");

  for (QListViewItemIterator it(m_dlg->lvSearchProviders); it.current(); ++it)
  {
    SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(it.current());

    Q_ASSERT(item);

    SearchProvider *provider = item->provider();

    QString name = provider->desktopEntryName();

    if (provider->isDirty())
    {
      if (name.isEmpty())
      {
        // New provider
        // Take the longest search shortcut as filename,
        // if such a file already exists, append a number and increase it
        // until the name is unique
        for (QStringList::ConstIterator it = provider->keys().begin(); it != provider->keys().end(); ++it)
        {
            if ((*it).length() > name.length())
                name = (*it).lower();
        }
        for (int suffix = 0; ; ++suffix)
        {
            QString located, check = name;
            if (suffix)
                check += QString().setNum(suffix);
            if ((located = locate("services", "searchproviders/" + check + ".desktop")).isEmpty())
            {
                name = check;
                break;
            }
            else if (located.left(path.length()) == path)
            {
                // If it's a deleted (hidden) entry, overwrite it
                if (KService(located).isDeleted())
                    break;
            }
        }
      }

      KSimpleConfig service(path + name + ".desktop");
      service.setGroup("Desktop Entry");
      service.writeEntry("Type", "Service");
      service.writeEntry("ServiceTypes", "SearchProvider");
      service.writeEntry("Name", provider->name());
      service.writeEntry("Query", provider->query());
      service.writeEntry("Keys", provider->keys());
      service.writeEntry("Charset", provider->charset());

      // we might be overwriting a hidden entry
      service.writeEntry("Hidden", false);
    }
  }

  for (QStringList::ConstIterator it = m_deletedProviders.begin();
      it != m_deletedProviders.end(); ++it)
  {
      QStringList matches = kapp->dirs()->findAllResources("services", "searchproviders/" + *it + ".desktop");

      // Shouldn't happen
      if (!matches.count())
          continue;

      if (matches.count() == 1 && matches[0].left(path.length()) == path)
      {
          // If only the local copy existed, unlink it
          // TODO: error handling
          QFile::remove(matches[0]);
          continue;
      }
      KSimpleConfig service(path + *it + ".desktop");
      service.setGroup("Desktop Entry");
      service.writeEntry("Type", "Service");
      service.writeEntry("ServiceTypes", "SearchProvider");
      service.writeEntry("Hidden", true);
  }

  config.sync();

  QByteArray data;
  kapp->dcopClient()->send("*", "KURIIKWSFilterIface", "configure()", data);
  kapp->dcopClient()->send("*", "KURISearchFilterIface", "configure()", data);
  kapp->dcopClient()->send( "kded", "kbuildsycoca", "recreate()", data);
}

void FilterOptions::defaults()
{
  setDelimiter (':');
  m_dlg->cmbDefaultEngine->setCurrentItem (0);
}

void FilterOptions::moduleChanged()
{
  // Removed the bool parameter, this way this can be directly connected
  // as it was alwayw called with true as argument anyway (malte)
  emit changed(true);
}

void FilterOptions::setAutoWebSearchState()
{
  moduleChanged();
}

void FilterOptions::setWebShortcutState()
{
  bool use_keywords = m_dlg->cbEnableShortcuts->isChecked();
  m_dlg->lvSearchProviders->setEnabled(use_keywords);
  m_dlg->pbNew->setEnabled(use_keywords);
  m_dlg->pbChange->setEnabled(use_keywords);
  m_dlg->pbDelete->setEnabled(use_keywords);
  m_dlg->lbDelimiter->setEnabled (use_keywords);
  m_dlg->cmbDelimiter->setEnabled (use_keywords);
  m_dlg->lbDefaultEngine->setEnabled (use_keywords);
  m_dlg->cmbDefaultEngine->setEnabled (use_keywords);
  moduleChanged();
}

void FilterOptions::addSearchProvider()
{
  SearchProviderDialog dlg(0, this);
  if (dlg.exec())
  {
      m_dlg->lvSearchProviders->setSelected(displaySearchProvider(dlg.provider()), true);
      moduleChanged();
  }
}

void FilterOptions::changeSearchProvider()
{
  SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(m_dlg->lvSearchProviders->currentItem());
  Q_ASSERT(item);

  SearchProviderDialog dlg(item->provider(), this);

  if (dlg.exec())
  {
    m_dlg->lvSearchProviders->setSelected(displaySearchProvider(dlg.provider()), true);
    moduleChanged();
  }
}

void FilterOptions::deleteSearchProvider()
{
  SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(m_dlg->lvSearchProviders->currentItem());
  Q_ASSERT(item);

  // Update the combo box to go to None if the fallback was deleted.
  int current = m_dlg->cmbDefaultEngine->currentItem();
  for (int i = 1, count = m_dlg->cmbDefaultEngine->count(); i < count; ++i)
  {
    if (m_dlg->cmbDefaultEngine->text(i) == item->provider()->name())
    {
      m_dlg->cmbDefaultEngine->removeItem(i);
      if (i == current)
        m_dlg->cmbDefaultEngine->setCurrentItem(0);
      else if (current > i)
        m_dlg->cmbDefaultEngine->setCurrentItem(current - 1);

      break;
    }
  }

  if (item->nextSibling())
      m_dlg->lvSearchProviders->setSelected(item->nextSibling(), true);
  else if (item->itemAbove())
      m_dlg->lvSearchProviders->setSelected(item->itemAbove(), true);

  if (!item->provider()->desktopEntryName().isEmpty())
      m_deletedProviders.append(item->provider()->desktopEntryName());

  delete item;
  updateSearchProvider();
  moduleChanged();
}

void FilterOptions::updateSearchProvider()
{
  m_dlg->pbChange->setEnabled(m_dlg->lvSearchProviders->currentItem());
  m_dlg->pbDelete->setEnabled(m_dlg->lvSearchProviders->currentItem());
}

SearchProviderItem *FilterOptions::displaySearchProvider(SearchProvider *p, bool fallback)
{
  // Show the provider in the list.
  SearchProviderItem *item = 0L;

  QListViewItemIterator it(m_dlg->lvSearchProviders);

  for (; it.current(); ++it)
  {
    if (it.current()->text(0) == p->name())
    {
      item = dynamic_cast<SearchProviderItem *>(it.current());
      Q_ASSERT(item);
      break;
    }
  }

  if (item)
    item->update ();
  else
  {
    // Put the name in the default search engine combo box.
    int itemCount;
    int totalCount = m_dlg->cmbDefaultEngine->count();

    item = new SearchProviderItem(m_dlg->lvSearchProviders, p);

    for (itemCount = 1; itemCount < totalCount; itemCount++)
    {
      if (m_dlg->cmbDefaultEngine->text(itemCount) > p->name())
      {
        int currentItem = m_dlg->cmbDefaultEngine->currentItem();
        m_dlg->cmbDefaultEngine->insertItem(p->name(), itemCount);
        m_defaultEngineMap[p->name ()] = p->desktopEntryName ();
        if (currentItem >= itemCount)
          m_dlg->cmbDefaultEngine->setCurrentItem(currentItem+1);
        break;
      }
    }

    // Append it to the end of the list...
    if (itemCount == totalCount)
    {
      m_dlg->cmbDefaultEngine->insertItem(p->name(), itemCount);
      m_defaultEngineMap[p->name ()] = p->desktopEntryName ();
    }

    if (fallback)
      m_dlg->cmbDefaultEngine->setCurrentItem(itemCount);
  }

  if (!it.current())
    m_dlg->lvSearchProviders->sort();

  return item;
}

#include "ikwsopts.moc"
