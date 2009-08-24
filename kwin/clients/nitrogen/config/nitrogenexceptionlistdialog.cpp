//////////////////////////////////////////////////////////////////////////////
// nitrogenexceptionlistdialog.cpp
// -------------------
// 
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.                 
//////////////////////////////////////////////////////////////////////////////

#include <QLayout>
#include <KLocale>
#include <KMessageBox>

#include "nitrogenexceptiondialog.h"
#include "nitrogenexceptionlistdialog.h"
#include "nitrogenexceptionlistdialog.moc"

//__________________________________________________________
namespace Nitrogen
{
  
  //__________________________________________________________
  NitrogenExceptionListDialog::NitrogenExceptionListDialog( QWidget* parent, NitrogenConfiguration default_configuration ):
    KDialog( parent ),
    default_configuration_( default_configuration )
  {
    
    // define buttons
    setButtons( Ok|Cancel );

    // main widget
    QWidget* widget = new QWidget( this );
    setMainWidget( widget );

    // layout
    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(5);
    widget->setLayout( h_layout );
    
    // list
    h_layout->addWidget( list_ = new QTreeView( widget ) );
    _list().setAllColumnsShowFocus( true );
    _list().setRootIsDecorated( false );
    _list().setSortingEnabled( false );
    _list().setModel( &_model() );
    _list().sortByColumn( NitrogenExceptionModel::TYPE );
    
    // button layout
    QVBoxLayout* v_layout = new QVBoxLayout();
    v_layout->setMargin(0);
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );
    KIconLoader* icon_loader = KIconLoader::global();
    
    v_layout->addWidget( up_button_ = new KPushButton( 
      KIcon( "arrow-up", icon_loader ),
      i18n("Move &Up"), widget ) );

    v_layout->addWidget( down_button_ = new KPushButton( 
      KIcon( "arrow-down", icon_loader ),
      i18n("Move &Down"), widget ) );

    v_layout->addWidget( add_button_ = new KPushButton( 
      KIcon( "list-add", icon_loader ), 
      i18n("&Add"), widget ) );
    
    v_layout->addWidget( remove_button_ = new KPushButton( 
      KIcon( "list-remove", icon_loader ), 
      i18n("&Remove"), widget ) );
    
    v_layout->addWidget( edit_button_ = new KPushButton( 
      KIcon( "edit-rename", icon_loader ),
      i18n("&Edit"), widget ) );

    v_layout->addStretch();

    connect( add_button_, SIGNAL( clicked() ), SLOT( _add() ) );
    connect( edit_button_, SIGNAL( clicked() ), SLOT( _edit() ) );
    connect( remove_button_, SIGNAL( clicked() ), SLOT( _remove() ) );
    connect( up_button_, SIGNAL( clicked() ), SLOT( _up() ) );
    connect( down_button_, SIGNAL( clicked() ), SLOT( _down() ) );
     
    connect( &_list(), SIGNAL( activated( const QModelIndex& ) ), SLOT( _edit() ) );   
    connect( &_list(), SIGNAL( clicked( const QModelIndex& ) ), SLOT( _toggle( const QModelIndex& ) ) );
    connect( _list().selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );
    
    resize( 500, 250 );
    _updateButtons();
    _resizeColumns();
    
    
  }

  //__________________________________________________________
  void NitrogenExceptionListDialog::setExceptions( const NitrogenExceptionList& exceptions )
  { 
    _model().set( NitrogenExceptionModel::List( exceptions.begin(), exceptions.end() ) ); 
    _resizeColumns();
  }

  //__________________________________________________________
  NitrogenExceptionList NitrogenExceptionListDialog::exceptions( void ) const
  { 
    
    NitrogenExceptionModel::List exceptions( _model().get() );
    NitrogenExceptionList out;
    for( NitrogenExceptionModel::List::const_iterator iter = exceptions.begin(); iter != exceptions.end(); iter++ )
    { out.push_back( *iter ); }
    return out;
    
  }
  
  //__________________________________________________________
  void NitrogenExceptionListDialog::_updateButtons( void )
  {
    
    bool has_selection( !_list().selectionModel()->selectedRows().empty() );
    remove_button_->setEnabled( has_selection );
    edit_button_->setEnabled( has_selection );
    
    up_button_->setEnabled( has_selection && !_list().selectionModel()->isRowSelected( 0, QModelIndex() ) );
    down_button_->setEnabled( has_selection && !_list().selectionModel()->isRowSelected( _model().rowCount()-1, QModelIndex() ) );
    
  }

  
  //_______________________________________________________
  void NitrogenExceptionListDialog::_add( void )
  {
    
    // map dialog
    NitrogenExceptionDialog dialog( this );
    dialog.setException( default_configuration_ );
    if( dialog.exec() == QDialog::Rejected ) return;
    
    // retrieve exception and check
    NitrogenException exception( dialog.exception() );
    if( !_checkException( exception ) ) return;
    
    // create new item
    _model().add( exception );
  
    // make sure item is selected
    QModelIndex index( _model().index( exception ) );
    if( index != _list().selectionModel()->currentIndex() )
    {
      _list().selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
      _list().selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }
  
    _resizeColumns();
    return;
    
  }

  //_______________________________________________________
  void NitrogenExceptionListDialog::_edit( void )
  {
    
    // retrieve selection
    QModelIndex current( _list().selectionModel()->currentIndex() );
    if( !current.isValid() ) return;
    
    NitrogenException& exception( _model().get( current ) );
  
    // create dialog
    NitrogenExceptionDialog dialog( this );
    dialog.setException( exception );
    
    // map dialog
    if( dialog.exec() == QDialog::Rejected ) return;
    NitrogenException new_exception = dialog.exception();
    
    if( !_checkException( new_exception ) ) return;
    *&exception = new_exception; 
    
    _resizeColumns();
    return;

  }

  //_______________________________________________________
  void NitrogenExceptionListDialog::_remove( void )
  {

    // remove
    _model().remove( _model().get( _list().selectionModel()->selectedRows() ) );
    _resizeColumns();
    return;
    
  }
  
  //_______________________________________________________
  void NitrogenExceptionListDialog::_toggle( const QModelIndex& index )
  {
    
    if( !index.isValid() ) return;
    if( index.column() != NitrogenExceptionModel::ENABLED ) return;
  
    // get matching exception
    NitrogenException& exception( _model().get( index ) );
    exception.setEnabled( !exception.enabled() );
    _model().add( exception );
    
  }

  //_______________________________________________________
  void NitrogenExceptionListDialog::_up( void )
  {

    NitrogenExceptionModel::List selection( _model().get( _list().selectionModel()->selectedRows() ) );
    if( selection.empty() ) { return; }
  
    // retrieve selected indexes in list and store in model
    QModelIndexList selected_indexes( _list().selectionModel()->selectedRows() );
    NitrogenExceptionModel::List selected_exceptions( _model().get( selected_indexes ) );
  
    NitrogenExceptionModel::List current_exceptions( _model().get() );
    NitrogenExceptionModel::List new_exceptions;
  
    for( NitrogenExceptionModel::List::const_iterator iter = current_exceptions.begin(); iter != current_exceptions.end(); iter++ )
    {

      // check if new list is not empty, current index is selected and last index is not.
      // if yes, move.
      if( 
        !( new_exceptions.empty() || 
        selected_indexes.indexOf( _model().index( *iter ) ) == -1 ||
        selected_indexes.indexOf( _model().index( new_exceptions.back() ) ) != -1 
        ) )
      { 
        NitrogenException last( new_exceptions.back() );
        new_exceptions.pop_back();
        new_exceptions.push_back( *iter );
        new_exceptions.push_back( last );
      } else new_exceptions.push_back( *iter );
      
    }
    
    _model().set( new_exceptions );
    
    // restore selection
    _list().selectionModel()->select( _model().index( selected_exceptions.front() ),  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    for( NitrogenExceptionModel::List::const_iterator iter = selected_exceptions.begin(); iter != selected_exceptions.end(); iter++ )
    { _list().selectionModel()->select( _model().index( *iter ), QItemSelectionModel::Select|QItemSelectionModel::Rows ); }
    
    return;
    
  }
  
  //_______________________________________________________
  void NitrogenExceptionListDialog::_down( void )
  {
    
    NitrogenExceptionModel::List selection( _model().get( _list().selectionModel()->selectedRows() ) );
    if( selection.empty() )
    { return; }
    
    // retrieve selected indexes in list and store in model
    QModelIndexList selected_indexes( _list().selectionModel()->selectedIndexes() );
    NitrogenExceptionModel::List selected_exceptions( _model().get( selected_indexes ) );
    
    NitrogenExceptionModel::List current_exceptions( _model().get() );
    NitrogenExceptionModel::List new_exceptions;
    
    for( NitrogenExceptionModel::List::reverse_iterator iter = current_exceptions.rbegin(); iter != current_exceptions.rend(); iter++ )
    {
      
      // check if new list is not empty, current index is selected and last index is not.
      // if yes, move.
      if( 
        !( new_exceptions.empty() || 
        selected_indexes.indexOf( _model().index( *iter ) ) == -1 ||
        selected_indexes.indexOf( _model().index( new_exceptions.back() ) ) != -1 
        ) )
      { 
        
        NitrogenException last( new_exceptions.back() );
        new_exceptions.pop_back();
        new_exceptions.push_back( *iter );
        new_exceptions.push_back( last );
        
      } else new_exceptions.push_back( *iter );
    }
    
    _model().set( NitrogenExceptionModel::List( new_exceptions.rbegin(), new_exceptions.rend() ) );
    
    // restore selection
    _list().selectionModel()->select( _model().index( selected_exceptions.front() ),  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    for( NitrogenExceptionModel::List::const_iterator iter = selected_exceptions.begin(); iter != selected_exceptions.end(); iter++ )
    { _list().selectionModel()->select( _model().index( *iter ), QItemSelectionModel::Select|QItemSelectionModel::Rows ); }
    
    return;
    
  }
  
  //_______________________________________________________
  void NitrogenExceptionListDialog::_resizeColumns( void ) const
  {
    _list().resizeColumnToContents( NitrogenExceptionModel::ENABLED );
    _list().resizeColumnToContents( NitrogenExceptionModel::TYPE );
    _list().resizeColumnToContents( NitrogenExceptionModel::REGEXP );
  }
  
  //_______________________________________________________
  bool NitrogenExceptionListDialog::_checkException( NitrogenException& exception )
  {
    
    while( !exception.regExp().isValid() )
    {

      KMessageBox::error( this, i18n("Regular Expression syntax is incorrect") );
      NitrogenExceptionDialog dialog( this );
      dialog.setException( exception );
      if( dialog.exec() == QDialog::Rejected ) return false;
      exception = dialog.exception();
      
    }
    
    return true;
  }
  
}
