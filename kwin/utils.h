/*****************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 1999, 2000 Matthias Ettrich <ettrich@kde.org>
Copyright (C) 2003 Lubos Lunak <l.lunak@kde.org>

You can Freely distribute this program under the GNU General Public
License. See the file "COPYING" for the exact licensing terms.
******************************************************************/

#ifndef KWIN_UTILS_H
#define KWIN_UTILS_H

#include <qvaluelist.h>
#include <qwidget.h>
#include <kmanagerselection.h>
#include <netwm_def.h>

namespace KWinInternal
{

const int SUPPORTED_WINDOW_TYPES_MASK = NET::NormalMask | NET::DesktopMask | NET::DockMask
    | NET::ToolbarMask | NET::MenuMask | NET::DialogMask | NET::OverrideMask | NET::TopMenuMask
    | NET::UtilityMask | NET::SplashMask;

const long ClientWinMask = KeyPressMask | KeyReleaseMask |
                          ButtonPressMask | ButtonReleaseMask |
                  KeymapStateMask |
                  ButtonMotionMask |
                  PointerMotionMask | // need this, too!
                  EnterWindowMask | LeaveWindowMask |
                  FocusChangeMask |
                  ExposureMask |
                  StructureNotifyMask |
                  SubstructureRedirectMask;

class Client;
class Group;
class Options;

typedef QValueList< Client* > ClientList;
typedef QValueList< const Client* > ConstClientList;

typedef QValueList< Group* > GroupList;
typedef QValueList< const Group* > ConstGroupList;

extern Options* options;

enum Layer
    {
    UnknownLayer = -1,
    FirstLayer = 0,
    DesktopLayer = FirstLayer,
    BelowLayer,
    NormalLayer,
    DockLayer,
    AboveLayer,
    ActiveLayer, // active fullscreen, or active dialog
    NumLayers // number of layers, must be last
    };

// yes, I know this is not 100% like standard operator++
inline void operator++( Layer& lay )
    {
    lay = static_cast< Layer >( lay + 1 );
    }

// Some KWin classes, mainly Client and Workspace, are very tighly coupled,
// and some of the methods of one class may be called only from speficic places.
// Those methods have additional allowed_t argument. If you pass Allowed
// as an argument to any function, make sure you really know what you're doing.
enum allowed_t { Allowed };

// some enums to have more readable code, instead of using bools
enum ForceGeometry_t { NormalGeometrySet, ForceGeometrySet };

class Shape 
    {
    public:
        static bool available() { return kwin_has_shape; }
        static bool hasShape( WId w);
        static int shapeEvent();
        static void init();
    private:
        static int kwin_has_shape;
        static int kwin_shape_event;
    };

class Motif 
    {
    public:
        static bool noBorder( WId w );
        static bool funcFlags( WId w, bool& resize, bool& move, bool& minimize,
            bool& maximize, bool& close );
        struct MwmHints 
            {
            ulong flags;
            ulong functions;
            ulong decorations;
            long input_mode;
            ulong status;
            };
        enum {  MWM_HINTS_FUNCTIONS = (1L << 0),
                    MWM_HINTS_DECORATIONS =  (1L << 1),

                    MWM_FUNC_ALL = (1L << 0),
                    MWM_FUNC_RESIZE = (1L << 1),
                    MWM_FUNC_MOVE = (1L << 2),
                    MWM_FUNC_MINIMIZE = (1L << 3),
                    MWM_FUNC_MAXIMIZE = (1L << 4),
                    MWM_FUNC_CLOSE = (1L << 5)
            };
    };

class KWinSelectionOwner
    : public KSelectionOwner
    {
    Q_OBJECT
    public:
        KWinSelectionOwner( int screen );
    protected:
        virtual bool genericReply( Atom target, Atom property, Window requestor );
        virtual void replyTargets( Atom property, Window requestor );
        virtual void getAtoms();
    private:
        Atom make_selection_atom( int screen );
        static Atom xa_version;
    };


QCString getStringProperty(WId w, Atom prop, char separator=0);
void updateXTime();

// the docs say it's UrgencyHint, but it's often #defined as XUrgencyHint
#ifndef UrgencyHint
#define UrgencyHint XUrgencyHint
#endif

// for STL-like algo's
#define KWIN_CHECK_PREDICATE( name, check ) \
struct name \
    { \
    inline bool operator()( const Client* cl ) { return check; }; \
    }

#define KWIN_COMPARE_PREDICATE( name, type, check ) \
struct name \
    { \
    typedef type type_helper; /* in order to work also with type being 'const Client*' etc. */ \
    inline name( const type_helper& compare_value ) : value( compare_value ) {}; \
    inline bool operator()( const Client* cl ) { return check; }; \
    const type_helper& value; \
    }

#define KWIN_PROCEDURE( name, action ) \
struct name \
    { \
    inline void operator()( Client* cl ) { action; }; \
    }

KWIN_CHECK_PREDICATE( TruePredicate, cl == cl /*true, avoid warning about 'cl' */ );

inline
int timestampCompare( Time time1, Time time2 ) // like strcmp()
    {
    if( time1 == time2 )
        return 0;
    return ( time1 - time2 ) < 1000000000 ? 1 : -1; // time1 > time2 -> 1, handle wrapping
    }

inline
Time timestampDiff( Time time1, Time time2 ) // returns time2 - time1
    { // no need to handle wrapping?
    return time2 - time1;
    }
    
} // namespace

#endif
