//-----------------------------------------------------------------------------
//
// KDE Display color scheme setup module
//
// Copyright (c)  Mark Donohoe 1997
//

#ifndef __COLORSCM_H__
#define __COLORSCM_H__

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <kdialogbase.h>

#include <kcmodule.h>

#include "widgetcanvas.h"

class QSlider;
class QComboBox;
class QPushButton;
class QCheckBox;
class QResizeEvent;
class KLineEdit;
class QPalette;
class KListBox;
class KColorButton;
class KConfig;
class KStdDirs;
class KColorSchemeList;

/**
 * The Desktop/Colors tab in kcontrol.
 */
class KColorScheme: public KCModule
{
    Q_OBJECT

public:
    KColorScheme(QWidget *parent, const char *name, const QStringList &);
    ~KColorScheme();

    virtual void load();
    virtual void save();
    virtual void defaults();
    QString quickHelp() const;

signals:
    void changed(bool);

private slots:
    void sliderValueChanged(int val);
    void slotSave();
    void slotAdd();
    void slotRemove();
    void slotSelectColor(const QColor &col);
    void slotWidgetColor(int);
    void slotColorForWidget(int, const QColor &);
    void slotPreviewScheme(int);
    void slotChanged();

private:
    void setColorName( const QString &name, int id );
    void readScheme(int index=0);
    void readSchemeNames();
    int findSchemeByName(const QString &scheme);
    QPalette createPalette();
    
    QColor &color(int index);

    int nSysSchemes;
    bool m_bChanged, useRM;

    QColor colorPushColor;
    QSlider *sb;
    QComboBox *wcCombo;
    QPushButton *addBt, *removeBt;
    KListBox *sList;
    KColorSchemeList *mSchemeList;
    QString sCurrentScheme;

    KColorButton *colorButton;
    WidgetCanvas *cs;
    
    QCheckBox *cbExportColors;
};

#endif
