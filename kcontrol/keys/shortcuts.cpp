/*
 * shortcuts.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 * Copyright (c) 2001 Ellis Whitehead <ellis@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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

#include "shortcuts.h"
#include "savescm.h"

#include <qdir.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qregexp.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kipc.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>

ShortcutsModule::ShortcutsModule( QWidget *parent, const char *name )
: QWidget( parent, name )
{
	initGUI();
}

// Called when [Reset] is pressed
void ShortcutsModule::load()
{
	kdDebug(125) << "ShortcutsModule::load()" << endl;
	slotSchemeCur();
}

// When [Apply] or [OK] are clicked.
void ShortcutsModule::save()
{
	kdDebug(125) << "ShortcutsModule::save()" << endl;

	m_pkcGeneral->commitChanges();
	m_pkcSequence->commitChanges();
	m_pkcApplication->commitChanges();

	m_actionsGeneral.writeActions( "Global Shortcuts", 0, true, true );
	m_actionsSequence.writeActions( "Global Shortcuts", 0, true, true );
	m_actionsApplication.writeActions( "Shortcuts", 0, true, true );

	KIPC::sendMessageAll( KIPC::SettingsChanged, KApplication::SETTINGS_SHORTCUTS );
}

void ShortcutsModule::defaults()
{
	m_pkcGeneral->allDefault();
	m_pkcSequence->allDefault();
	m_pkcApplication->allDefault();
}

QString ShortcutsModule::quickHelp() const
{
  return i18n("<h1>Key Bindings</h1> Using key bindings you can configure certain actions to be"
    " triggered when you press a key or a combination of keys, e.g. CTRL-C is normally bound to"
    " 'Copy'. KDE allows you to store more than one 'scheme' of key bindings, so you might want"
    " to experiment a little setting up your own scheme while you can still change back to the"
    " KDE defaults.<p> In the tab 'Global shortcuts' you can configure non-application specific"
    " bindings like how to switch desktops or maximize a window. In the tab 'Application shortcuts'"
    " you'll find bindings typically used in applications, such as copy and paste.");
}

void ShortcutsModule::initGUI()
{
	kdDebug(125) << "A-----------" << endl;
	KAccelActions* keys = &m_actionsGeneral;
// see also KShortcutsModule::init() below !!!
#define NOSLOTS
#define KICKER_ALL_BINDINGS
#include "../../kwin/kwinbindings.cpp"
#include "../../kicker/core/kickerbindings.cpp"
#include "../../kdesktop/kdesktopbindings.cpp"
#include "../../klipper/klipperbindings.cpp"
#include "../../kxkb/kxkbbindings.cpp"

	kdDebug(125) << "B-----------" << endl;
	m_actionsSequence.init( m_actionsGeneral );

	kdDebug(125) << "C-----------" << endl;
	createActionsGeneral();
	kdDebug(125) << "D-----------" << endl;
	createActionsSequence();
	kdDebug(125) << "E-----------" << endl;
	createActionsApplication();

	kdDebug(125) << "F-----------" << endl;
	QVBoxLayout* pVLayout = new QVBoxLayout( this, KDialog::marginHint() );

	pVLayout->addSpacing( KDialog::marginHint() );

	// (o) [Current      ] <Remove>   ( ) New <Save>

	QHBoxLayout *pHLayout = new QHBoxLayout( pVLayout, KDialog::spacingHint() );
	QButtonGroup* pGroup = new QButtonGroup( this );
	pGroup->hide();

	m_prbPre = new QRadioButton( "", this );
	connect( m_prbPre, SIGNAL(clicked()), SLOT(slotSchemeCur()) );
	pGroup->insert( m_prbPre );
	pHLayout->addWidget( m_prbPre );

	m_pcbSchemes = new KComboBox( this );
	connect( m_pcbSchemes, SIGNAL(activated(int)), SLOT(slotSelectScheme()) );
	pHLayout->addWidget( m_pcbSchemes );

	pHLayout->addSpacing( KDialog::marginHint() );

	m_pbtnRemove = new QPushButton( i18n("&Remove"), this );
	m_pbtnRemove->setEnabled( false );
	connect( m_pbtnRemove, SIGNAL(clicked()), SLOT(slotRemoveScheme()) );
	QWhatsThis::add( m_pbtnRemove, i18n("Click here to remove the selected key bindings scheme. You can not"
		" remove the standard system wide schemes, 'Current scheme' and 'KDE default'.") );
	pHLayout->addWidget( m_pbtnRemove );

	pHLayout->addSpacing( KDialog::marginHint() * 3 );

	m_prbNew = new QRadioButton( i18n("New scheme"), this );
	m_prbNew->setEnabled( false );
	pGroup->insert( m_prbNew );
	pHLayout->addWidget( m_prbNew );

	m_pbtnSave = new QPushButton( i18n("&Save..."), this );
	m_pbtnSave->setEnabled( false );
	QWhatsThis::add( m_pbtnSave, i18n("Click here to add a new key bindings scheme. You will be prompted for a name.") );
	connect( m_pbtnSave, SIGNAL(clicked()), SLOT(slotSaveSchemeAs()) );
	pHLayout->addWidget( m_pbtnSave );

	pHLayout->addStretch( 1 );

	m_pTab = new QTabWidget( this );
	pVLayout->addWidget( m_pTab );

	m_pkcGeneral = new KKeyChooser( m_actionsGeneral, this, true, false, true );
	m_pTab->addTab( m_pkcGeneral, i18n("&Global Shortcuts") );
	connect( m_pkcGeneral, SIGNAL(keyChange()), SLOT(slotKeyChange()) );

	m_pkcSequence = new KKeyChooser( m_actionsSequence, this, true, false, true );
	m_pTab->addTab( m_pkcSequence, i18n("Shortcut Se&quences") );
	connect( m_pkcGeneral, SIGNAL(keyChange()), SLOT(slotKeyChange()) );

	m_pkcApplication = new KKeyChooser( m_actionsApplication, this, true, false, false );
	m_pTab->addTab( m_pkcApplication, i18n("&Application Shortcuts") );
	connect( m_pkcApplication, SIGNAL(keyChange()), SLOT(slotKeyChange()) );

	kdDebug(125) << "G-----------" << endl;
	readSchemeNames();

	kdDebug(125) << "I-----------" << endl;
	slotSchemeCur();

	kdDebug(125) << "J-----------" << endl;
}

void ShortcutsModule::createActionsGeneral()
{
	KAccelActions& actions = m_actionsGeneral;

	for( uint i = 0; i < actions.size(); i++ ) {
		QString sConfigKey = actions[i].name();
		//kdDebug(125) << "sConfigKey: " << sConfigKey << endl;
		int iLastSpace = sConfigKey.findRev( ' ' );
		bool bIsNum = false;
		if( iLastSpace >= 0 )
			sConfigKey.mid( iLastSpace+1 ).toInt( &bIsNum );

		//kdDebug(125) << "sConfigKey: " << sConfigKey
		//	<< " bIsNum: " << bIsNum << endl;
		if( bIsNum && !sConfigKey.contains( ':' ) ) {
			actions[i].setConfigurable( false );
			actions[i].setName( QString::null );
		}
	}
}

void ShortcutsModule::createActionsSequence()
{
	KAccelActions& actions = m_actionsSequence;

	for( uint i = 0; i < actions.size(); i++ ) {
		QString sConfigKey = actions[i].name();
		//kdDebug(125) << "sConfigKey: " << sConfigKey << endl;
		int iLastSpace = sConfigKey.findRev( ' ' );
		bool bIsNum = false;
		if( iLastSpace >= 0 )
			sConfigKey.mid( iLastSpace+1 ).toInt( &bIsNum );

		//kdDebug(125) << "sConfigKey: " << sConfigKey
		//	<< " bIsNum: " << bIsNum << endl;
		if( !bIsNum && !sConfigKey.contains( ':' ) ) {
			actions[i].setConfigurable( false );
			actions[i].setName( QString::null );
		}
	}
}

void ShortcutsModule::createActionsApplication()
{
	for( uint i=0; i < KStdAccel::NB_STD_ACCELS; i++ ) {
		KStdAccel::StdAccel id = (KStdAccel::StdAccel) i;
		m_actionsApplication.insertAction( KStdAccel::action(id),
			KStdAccel::description(id),
			QString::null, // sHelp
			KStdAccel::shortcutDefault3(id),
			KStdAccel::shortcutDefault4(id) );
	}
}

void ShortcutsModule::readSchemeNames()
{
	//QStringList schemes = KGlobal::dirs()->findAllResources("data", "kcmkeys/" + KeyType + "/*.kksrc");
	QStringList schemes = KGlobal::dirs()->findAllResources("data", "kcmkeys/*.kksrc");
	//QRegExp r( "-kde[34].kksrc$" );
	//QRegExp r( "-kde3.kksrc$" );

	m_pcbSchemes->clear();
	m_rgsSchemeFiles.clear();

	i18n("User-Defined Scheme");
	m_pcbSchemes->insertItem( i18n("Current Scheme") );
	m_rgsSchemeFiles.append( "cur" );

	// This for system files
	for ( QStringList::ConstIterator it = schemes.begin(); it != schemes.end(); it++) {
	// KPersonalizer relies on .kksrc files containing all the keyboard shortcut
	//  schemes for various setups.  It also requires the KDE defaults to be in
	//  a .kksrc file.  The KDE defaults shouldn't be listed here.
		//if( r.search( *it ) != -1 )
		//   continue;

		KSimpleConfig config( *it, true );
		// TODO: Put 'Name' in "Settings" group
		//config.setGroup( KeyScheme );
		config.setGroup( "Settings" );
		QString str = config.readEntry( "Name" );

		m_pcbSchemes->insertItem( str );
		m_rgsSchemeFiles.append( *it );
	}
}

void ShortcutsModule::resizeEvent( QResizeEvent * )
{
	//m_pTab->setGeometry(0,0,width(),height());
}

void ShortcutsModule::slotSchemeCur()
{
	kdDebug(125) << "ShortcutsModule::slotSchemeCur()" << endl;
	//m_pcbSchemes->setCurrentItem( 0 );
	slotSelectScheme();
}

void ShortcutsModule::slotKeyChange()
{
	kdDebug(125) << "ShortcutsModule::slotKeyChange()" << endl;
	m_prbNew->setEnabled( true );
	m_prbNew->setChecked( true );
	m_pbtnSave->setEnabled( true );
	emit changed( true );
}

void ShortcutsModule::slotSelectScheme()
{
	kdDebug(125) << "ShortcutsModule::slotSelectScheme( " << m_pcbSchemes->currentItem() << " )" << endl;
	QString sFilename = m_rgsSchemeFiles[ m_pcbSchemes->currentItem() ];

	if( sFilename == "cur" ) {
		m_actionsGeneral.readActions( "Global Shortcuts", 0 );
		m_actionsSequence.readActions( "Global Shortcuts", 0 );
		m_actionsApplication.readActions( "Shortcuts", 0 );
	} else {
		KSimpleConfig config( sFilename );

		m_actionsGeneral.readActions( "Global Shortcuts", &config );
		m_actionsSequence.readActions( "Global Shortcuts", &config );
		m_actionsApplication.readActions( "Shortcuts", &config );
	}

	i18n("This scheme requires the \"%1\" modifier key, which is not available on your keyboard layout. "
		"Do you wish to view it anyway?" ).arg( i18n("Win") );

	m_prbPre->setChecked( true );
	m_prbNew->setEnabled( false );
	m_pbtnSave->setEnabled( false );

	m_pkcGeneral->listSync();
	m_pkcSequence->listSync();
	m_pkcApplication->listSync();
}

void ShortcutsModule::slotSaveSchemeAs()
{
	QString sName, sFile;
	bool bNameValid;
	int iScheme = -1;

	sName = m_pcbSchemes->currentText();

	SaveScm ss( 0, "save scheme", sName );
	do {
		bNameValid = true;

		if( ss.exec() ) {
			sName = ss.nameLine->text();
			if( sName.stripWhiteSpace().isEmpty() )
				return;

			sName = sName.simplifyWhiteSpace();
			sFile = sName;

			int ind = 0;
			while( ind < (int) sFile.length() ) {
				// parse the string for first white space
				ind = sFile.find(" ");
				if( ind == -1 ) {
					ind = sFile.length();
					break;
				}

				// remove from string
				sFile.remove( ind, 1 );

				// Make the next letter upper case
				QString s = sFile.mid( ind, 1 );
				s = s.upper();
				sFile.replace( ind, 1, s );
			}

			iScheme = -1;
			for( int i = 0; i < (int) m_pcbSchemes->count(); i++ ) {
				if( sName.lower() == (m_pcbSchemes->text(i)).lower() ) {
					iScheme = i;

					int result = KMessageBox::warningContinueCancel( 0,
					i18n("A key scheme with the name '%1' already exists.\n"
						"Do you want to overwrite it?\n").arg(sName),
					i18n("Save key scheme"),
					i18n("Overwrite"));
					bNameValid = (result == KMessageBox::Continue);
				}
			}
		} else
			return;
	} while( !bNameValid );

	disconnect( m_pcbSchemes, SIGNAL(activated(int)), this, SLOT(slotSelectScheme()) );

	QString kksPath = KGlobal::dirs()->saveLocation( "data", "kcmkeys/" );

	QDir dir( kksPath );
	if( !dir.exists() && !dir.mkdir( kksPath ) ) {
		qWarning("KShortcutsModule: Could not make directory to store user info.");
		return;
	}

	sFile.prepend( kksPath );
	sFile += ".kksrc";
	if( iScheme == -1 ) {
		m_pcbSchemes->insertItem( sName );
		//m_pcbSchemes->setFocus();
		m_pcbSchemes->setCurrentItem( m_pcbSchemes->count()-1 );
		m_rgsSchemeFiles.append( sFile );
	} else {
		//m_pcbSchemes->setFocus();
		m_pcbSchemes->setCurrentItem( iScheme );
	}

	KSimpleConfig *config = new KSimpleConfig( sFile );

	config->setGroup( "Settings" );
	config->writeEntry( "Name", sName );
	delete config;

	saveScheme();

	connect( m_pcbSchemes, SIGNAL(activated(int)), SLOT(slotSelectScheme()) );
	slotSelectScheme();
}

void ShortcutsModule::saveScheme()
{
	QString sFilename = m_rgsSchemeFiles[ m_pcbSchemes->currentItem() ];
	KSimpleConfig config( sFilename );

	m_pkcGeneral->commitChanges();
	m_pkcSequence->commitChanges();
	m_pkcApplication->commitChanges();

	m_actionsGeneral.writeActions( "Global Shortcuts", &config, true );
	m_actionsSequence.writeActions( "Global Shortcuts", &config, true );
	m_actionsApplication.writeActions( "Shortcuts", &config, true );
}

void ShortcutsModule::slotRemoveScheme()
{
}

#include "shortcuts.moc"
