/* -------------------------------------------------------------

   configdialog.h (part of Klipper - Cut & paste history for KDE)

   $Id$

   (C) 2000 by Carsten Pfeiffer <pfeiffer@kde.org>

   Licensed under the Artistic License

 ------------------------------------------------------------- */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qcheckbox.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qvgroupbox.h>

#include <kdialogbase.h>
#include <kkeydialog.h>
#include <knuminput.h>

#include "urlgrabber.h"

class KGlobalAccel;
class KEditableListView;

class GeneralWidget : public QVGroupBox
{
    Q_OBJECT

    friend class ConfigDialog;

public:
    GeneralWidget( QWidget *parent, const char *name );
    ~GeneralWidget();

private:
    QCheckBox *cbMousePos, *cbSaveContents;
    KIntNumInput *popupTimeout;

};

class ActionWidget : public QVGroupBox
{
    Q_OBJECT

    friend class ConfigDialog;

public:
    ActionWidget( const ActionList *list, QWidget *parent, const char *name );
    ~ActionWidget();

    /**
     * Creates a list of actions from the listView and returns a pointer to the
     * list.
     * Make sure to free that pointer when you don't need it anymore.
     */
    ActionList * actionList();

private slots:
    void slotAddAction();
    void slotDeleteAction();
    void slotRightPressed( QListViewItem *, const QPoint&, int col );
    void slotItemChanged( QListViewItem *, int );

private:
    KEditableListView *listView;

};

class KeysWidget : public QVGroupBox
{
    Q_OBJECT

    friend class ConfigDialog;

public:
    KeysWidget( KKeyEntryMap *keyMap, QWidget *parent, const char *name );
    ~KeysWidget();

private:
    KKeyChooser *keyChooser;
};


class ConfigDialog : public KDialogBase
{
    Q_OBJECT

public:
    ConfigDialog( const ActionList *list, KKeyEntryMap *keyMap );
    ~ConfigDialog();

    ActionList * actionList() const { return actionWidget->actionList(); }

    bool keepContents()    const {
	return generalWidget->cbSaveContents->isChecked();
    }
    bool popupAtMousePos() const {
	return generalWidget->cbMousePos->isChecked();
    }

    int popupTimeout() const {
	return generalWidget->popupTimeout->value();
    }
    
    void setKeepContents( bool enable ) {
	generalWidget->cbSaveContents->setChecked( enable );
    }
    void setPopupAtMousePos( bool enable ) {
	generalWidget->cbMousePos->setChecked( enable );
    }
    void setPopupTimeout( int timeout ) {
	generalWidget->popupTimeout->setValue( timeout );
    }
    
private:
    GeneralWidget *generalWidget;
    ActionWidget *actionWidget;
    KeysWidget *keysWidget;

};



#endif // CONFIGDIALOG_H
