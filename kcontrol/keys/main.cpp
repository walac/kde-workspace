//
// KDE Shotcut config module
//
// Copyright (c)  Mark Donohoe 1998
// Copyright (c)  Matthias Ettrich 1998
//
/*

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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */


#include <qdir.h>
#include <qslider.h>

#include <kcontrol.h>
#include "keyconfig.h"
#include <kwm.h>
#include <klocale.h>
#include <stdio.h>

bool global_switch = false ;

class KKeyApplication : public KControlApplication
{
public:

  KKeyApplication(int &argc, char **arg, const char *name);

  void init();
  void apply();
  void defaultValues();

private:

//  KStdConfig *standard;
//  KGlobalConfig *global;
  KKeyConfig *standard;
  KKeyConfig *global;
};


KKeyApplication::KKeyApplication(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
  standard = 0;
  global = 0;

  if (runGUI())
    {

      if (!pages || pages->contains("standard")){
	printf("create standard\n");
 	global_switch = false ;
 	addPage(standard = new KKeyConfig(dialog),
		i18n("&Standard shortcuts"), "index-1.html");
      }
      if (!pages || pages->contains("global")){
	printf("create global\n");
 	global_switch = true ;
	addPage(global = new KKeyConfig(dialog),
		i18n("&Global shortcuts"), "index-1.html");

      }

      if (standard || global)
        dialog->show();
      else
        {
          fprintf(stderr, i18n("usage: kcmkeys [-init | {standard,global}]\n").ascii());
          justInit = TRUE;
        }

    }
}


void KKeyApplication::init()
{
//  KStdConfig *standard=new KStdConfig(0);
  KKeyConfig *standard=new KKeyConfig(0);
  standard->keys->writeSettings();
  delete standard;
//  KGlobalConfig *global = new KGlobalConfig(0);
  KKeyConfig *global = new KKeyConfig(0);
  global->keys->writeSettings();
  delete global;
}


void KKeyApplication::apply()
{

  if (standard)
    standard->applySettings();
  if (global)
    global->applySettings();
  // tell kwm to re-parse the config file
  KWM::configureWm();

}

void KKeyApplication::defaultValues(){
  if (standard)
    standard->defaultSettings();
  if (global)
    global->defaultSettings();
}

int main(int argc, char **argv)
{

  KKeyApplication app(argc, argv, "kcmkeys");
  app.setTitle(i18n("Key binding settings"));

  if (app.runGUI())
    return app.exec();
  else
    {
      app.init();
      return 0;
    }
}
