/*
 *   Copyright 2006-2008 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KIcon>
#include <KConfigGroup>

#include "plasmaapp.h"

static const char description[] = I18N_NOOP( "The KDE workspace application optimized for Netbook devices." );
static const char version[] = "0.2";

extern "C"
KDE_EXPORT int kdemain(int argc, char **argv)
{
    KAboutData aboutData("plasma-netbook", 0, ki18n("Plasma Netbook Shell"),
                         version, ki18n(description), KAboutData::License_GPL,
                         ki18n("Copyright 2006-2009, The KDE Team"));
    aboutData.addAuthor(ki18n("Aaron J. Seigo"),
                        ki18n("Author and maintainer"),
                        "aseigo@kde.org");


    bool customGraphicsSystem = false;
    for (int i = 0; i < argc; ++i) {
        if (QString(argv[i]) == "-graphicssystem") {
            customGraphicsSystem = true;
            break;
        }
    }

    if (!customGraphicsSystem) {
        KConfigGroup cg(KSharedConfig::openConfig("plasma-netbookrc"), "General");
        QApplication::setGraphicsSystem(cg.readEntry("GraphicsSystem", "native"));
    }

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("nodesktop", ki18n("Starts as a normal application instead of as the primary user interface"));
    options.add("opengl", ki18n("Use OpenGL to draw the main screen"));
    options.add("screen <geometry>", ki18n("The geometry of the screen"), "800x480");
    KCmdLineArgs::addCmdLineOptions(options);

    PlasmaApp *app = PlasmaApp::self();
    QApplication::setWindowIcon(KIcon("plasma"));
    app->disableSessionManagement(); // autostarted
    int rc = app->exec();
    delete app;
    return rc;
}

