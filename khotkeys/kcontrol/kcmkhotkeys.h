/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 kcmkhotkeys.h  -

 $Id$

****************************************************************************/

#ifndef __kcmkhotkeys_H
#define __kcmkhotkeys_H

// see also kdebase/kmenuedit/khotkeys.h
extern "C"
    {
// initializes khotkeys DSO - loads i18n catalogue
// handled automatically by KHotKeys wrapper class in kmenuedit
void khotkeys_init( void );
// return keyboard shortcut ( e.g. "ALT+T" ) for given menu entry ( e.g.
// "System/Konsole.desktop"
QString khotkeys_get_menu_entry_shortcut( const QString& entry_P );
// changes assigned shortcut to menu entry a updates config file
QString khotkeys_change_menu_entry_shortcut( const QString& entry_P,
    const QString& shortcut_P );
// creates a dialog for assigning a shortcut to the menu entry,
// with shortcut_P as default, if save_if_edited_P is true
// and user doesn't cancel the dialog, config file is updated
//QString khotkeys_edit_menu_entry_shortcut( const QString& entry_P,
//    const QString& shortcut_P, bool save_if_edited_P );
// menu entry was moved in K Menu
bool khotkeys_menu_entry_moved( const QString& new_P, const QString& old_P );
// menu entry was removed
void khotkeys_menu_entry_deleted( const QString& entry_P );
    } // extern "C"

#endif
