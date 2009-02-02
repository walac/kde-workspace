/****************************************************************************

 KHotKeys

 Copyright (C) 1999-2001 Lubos Lunak <l.lunak@kde.org>

 Distributed under the terms of the GNU General Public License version 2.

****************************************************************************/

#ifndef _CONDITIONS_H_
#define _CONDITIONS_H_

#include "action_data/action_data_group.h"

#include "windows_handler.h"

#include <QtCore/QList>
#include <QtCore/QObject>

class KConfig;

namespace KHotKeys
{

class ActionDataBase;
class ActionData;
class Condition_list_base;

class KDE_EXPORT Condition
    {
    Q_DISABLE_COPY( Condition )

    public:
        Condition( Condition_list_base* parent_P );
        Condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual ~Condition();
        virtual bool match() const = 0;
        virtual void updated() const; // called when the condition changes
        virtual void cfg_write( KConfigGroup& cfg_P ) const = 0;
        virtual const QString description() const = 0;
        virtual Condition* copy( Condition_list_base* parent_P ) const = 0;
        const Condition_list_base* parent() const;
        Condition_list_base* parent();
        static Condition* create_cfg_read( KConfigGroup& cfg_P, Condition_list_base* parent_P );
    protected:
        Condition_list_base* const _parent;
    };

class KDE_EXPORT Condition_list_base : public Condition, public QList < Condition* >
    {
    typedef Condition base;
    public:
        Condition_list_base( Condition_list_base* parent_P );

        Condition_list_base(
                const QList< Condition* >& children_P,
            Condition_list_base* parent_P );

        Condition_list_base( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual ~Condition_list_base();
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        virtual bool accepts_children() const;
        typedef QList< Condition* >::iterator Iterator;
        typedef QList< Condition* >::const_iterator ConstIterator;
    };

class KDE_EXPORT Condition_list
    : public Condition_list_base
    {
    typedef Condition_list_base base;
    public:
        Condition_list( const QString& comment_P, ActionDataBase* data_P );
        Condition_list( KConfigGroup& cfg_P, ActionDataBase* data_P );
        void cfg_write( KConfigGroup& cfg_P ) const;
        Condition_list* copy( ActionDataBase* data_P ) const;
        virtual bool match() const;
        const QString& comment() const;
        void set_data( ActionDataBase* data_P );
        virtual void updated() const;
        virtual Condition_list* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
    private:
        QString _comment;
        ActionDataBase* data;
    };

class KDE_EXPORT Active_window_condition
    : public QObject, public Condition
    {
    Q_OBJECT
    typedef Condition base;
    public:
        Active_window_condition( Windowdef_list* window_P, Condition_list_base* parent_P );
        Active_window_condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual ~Active_window_condition();
        virtual bool match() const;
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        const Windowdef_list* window() const;
        virtual Active_window_condition* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
    public Q_SLOTS:
        void active_window_changed( WId );
    private:
        void init();
        void set_match();
        Windowdef_list* _window;
        bool is_match;
    };

class KDE_EXPORT Existing_window_condition
    : public QObject, public Condition
    {
    Q_OBJECT
    typedef Condition base;
    public:
        Existing_window_condition( Windowdef_list* window_P, Condition_list_base* parent_P );
        Existing_window_condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual ~Existing_window_condition();
        virtual bool match() const;
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        const Windowdef_list* window() const;
        virtual Existing_window_condition* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
    public Q_SLOTS:
        void window_added( WId w_P );
        void window_removed( WId w_P );
    private:
        void init();
        void set_match( WId w_P = None );
        Windowdef_list* _window;
        bool is_match;
    };

class KDE_EXPORT Not_condition
    : public Condition_list_base
    {
    typedef Condition_list_base base;
    public:
        Not_condition( Condition_list_base* parent_P );
        Not_condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual bool match() const;
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        virtual Not_condition* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
        const Condition* condition() const;
        virtual bool accepts_children() const;
    };

class KDE_EXPORT And_condition
    : public Condition_list_base
    {
    typedef Condition_list_base base;
    public:
        And_condition( Condition_list_base* parent_P );
        And_condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual bool match() const;
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        virtual And_condition* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
    };

class KDE_EXPORT Or_condition
    : public Condition_list_base
    {
    typedef Condition_list_base base;
    public:
        Or_condition( Condition_list_base* parent_P );
        Or_condition( KConfigGroup& cfg_P, Condition_list_base* parent_P );
        virtual bool match() const;
        virtual void cfg_write( KConfigGroup& cfg_P ) const;
        virtual Or_condition* copy( Condition_list_base* parent_P ) const;
        virtual const QString description() const;
    };

//***************************************************************************
// Inline
//***************************************************************************

// Condition

inline
const Condition_list_base* Condition::parent() const
    {
    return _parent;
    }

inline
Condition_list_base* Condition::parent()
    {
    return _parent;
    }

// Condition_list_base

inline
Condition_list_base::Condition_list_base( Condition_list_base* parent_P )
    : Condition( parent_P ), QList< Condition* >()
    {
    }

inline
Condition_list_base::Condition_list_base(
        const QList< Condition* >& children_P,
        Condition_list_base* parent_P )
    : Condition( parent_P ),
      QList< Condition* >( children_P )
    {
    }

// Condition_list

inline
Condition_list::Condition_list( const QString& comment_P, ActionDataBase* data_P )
    : Condition_list_base( NULL ), _comment( comment_P ), data( data_P )
    {
    }

inline
const QString& Condition_list::comment() const
    {
    return _comment;
    }

// Active_window_condition

inline
Active_window_condition::Active_window_condition( Windowdef_list* window_P,
    Condition_list_base* parent_P )
    : Condition( parent_P ), _window( window_P )
    {
    init();
    set_match();
    }

inline
const Windowdef_list* Active_window_condition::window() const
    {
    return _window;
    }

// Existing_window_condition

inline
Existing_window_condition::Existing_window_condition( Windowdef_list* window_P,
    Condition_list_base* parent_P )
    : Condition( parent_P ), _window( window_P ), is_match( false )
    {
    init();
    set_match();
    }

inline
const Windowdef_list* Existing_window_condition::window() const
    {
    return _window;
    }

// Not_condition

inline
Not_condition::Not_condition( Condition_list_base* parent_P )
    : Condition_list_base( parent_P )
    {
    }

inline
const Condition* Not_condition::condition() const
    {
    return first();
    }

// And_condition

inline
And_condition::And_condition( Condition_list_base* parent_P )
    : Condition_list_base( parent_P )
    {
    }

// Or_condition

inline
Or_condition::Or_condition( Condition_list_base* parent_P )
    : Condition_list_base( parent_P )
    {
    }

} // namespace KHotKeys

#endif
