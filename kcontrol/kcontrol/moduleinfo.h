/*

  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 
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



#ifndef _MODULEINFO_H_
#define _MODULEINFO_H_


#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qstringlist.h>


class ModuleInfo : public QObject
{
  Q_OBJECT 

public:

  enum ModuleType { Library, Executable };

  ModuleInfo(QString desktopFile);

  const QString fileName() const { return _fileName; };
  const QStringList &groups() const { return _groups; };
  QString name() const { return _name; };
  QPixmap icon();
  bool isDirectory() const { return _directory; };
  QString library() const { return _lib; };
  QString handle() const { return _handle; };
  QString executable() const { return _exec; };
  bool onlyRoot() const { return _root; };
  bool localUser() const { return _local; };
  ModuleType type() const { return _type; };
  QString docPath() const { return _doc; };

  QCString moduleId() const;


protected:

  void setGroups(QStringList &groups) { _groups = groups; };
  void setName(QString name) { _name = name; };
  void setIcon(QString icon) { _icon = icon; };
  void setDirectory(bool dir) { _directory = dir; };
  void setLibrary(QString lib) { _lib = lib; };
  void setHandle(QString handle) { _handle = handle; };
  void setOnlyRoot(bool only) { _root = only; };
  void setLocalUser(bool local) { _local = local; };
  void setType(ModuleType t) { _type = t; };
  void setExecutable(QString e) { _exec = e; };
  void setDocPath(QString p) { _doc = p; };
private:
  
  QStringList _groups;
  QString     _name, _icon, _lib, _handle, _fileName, _exec, _doc;
  bool        _directory, _root, _local;
  ModuleType  _type;

};


#endif
