/*=========================================================================

  Program:   Monteverdi
  Language:  C++


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See Copyright.txt for details.

  Monteverdi is distributed under the CeCILL licence version 2. See
  Licence_CeCILL_V2-en.txt or
  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt for more details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mvdDatabaseBrowserController.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)
//
#include "Core/mvdDatabaseConnection.h"
#include "Core/mvdDatabaseQueries.h"
#include "Core/mvdDatabaseModel.h"
#include "Core/mvdVectorImageModel.h"
#include "Core/mvdSystemError.h"
//
#if ENABLE_TREE_WIDGET_TEST
#  include "Gui/mvdDatabaseBrowserWidgetTest.h"
#else // ENABLE_TREE_WIDGET_TEST
// #  include "Gui/mvdDatabaseTreeWidget.h"
#  include "Gui/mvdDatabaseBrowserWidget.h"
#endif // ENABLE_TREE_WIDGET_TEST
#if !DISABLE_CUSTOM_TW_ITEM
#  include "Gui/mvdTreeWidgetItem.h"
#endif // !DISABLE_CUSTOM_TW_ITEM

namespace mvd
{
/*
  TRANSLATOR mvd::DatabaseBrowserController

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*******************************************************************************/
DatabaseBrowserController
#if ENABLE_TREE_WIDGET_TEST
::DatabaseBrowserController( DatabaseBrowserWidgetTest * widget,
                             QObject* parent ) :
#else // #if ENABLE_TREE_WIDGET_TEST
::DatabaseBrowserController( DatabaseBrowserWidget* widget, QObject* parent ) :
#endif // #if ENABLE_TREE_WIDGET_TEST
  AbstractModelController( widget, parent )
{
}

/*******************************************************************************/
DatabaseBrowserController
::~DatabaseBrowserController()
{
}

/*******************************************************************************/
void
DatabaseBrowserController
::Connect( AbstractModel* model )
{
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST

  QObject::connect(
    widget,
    SIGNAL( CurrentDatasetChanged( const QString&, const QString& ) ),
    // to:
    this,
    SLOT( OnCurrentDatasetChanged( const QString&, const QString& ) )
  );

  QObject::connect(
    model,
    SIGNAL( SelectedDatasetModelChanged( DatasetModel* ) ),
    // to:
    this,
    SLOT( OnSelectedDatasetModelChanged( DatasetModel* ) )
  );

  //
  QObject::connect(
    widget, 
    SIGNAL( ItemMoved( QTreeWidgetItem*, QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnItemMoved( QTreeWidgetItem*, QTreeWidgetItem* ) )
    );

  //
  QObject::connect(
    widget,
    SIGNAL( AddItemRequested( QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnAddItemRequested( QTreeWidgetItem* ) )
    );

  //
  QObject::connect(
    widget,
    SIGNAL( DeleteItemRequested( QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnDeleteItemRequested( QTreeWidgetItem* ) )
    );

  //
  QObject::connect(
    widget,
    SIGNAL( ItemTextChanged( QTreeWidgetItem*, int ) ),
    // to:
    this,
    SLOT( OnItemTextChanged( QTreeWidgetItem*, int ) )
    );

  //
  QObject::connect(
    this,
    SIGNAL( ModelUpdated() ),
    // to:
    model,
    SIGNAL( DatabaseChanged() )
  );

  //
  QObject::connect(
    model,
    SIGNAL( DatabaseChanged() ),
    this,
    SLOT( RefreshWidget() )
    );
}

/*******************************************************************************/
void
DatabaseBrowserController
::Disconnect( AbstractModel* model )
{
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest* widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST

  QObject::disconnect(
    widget,
    SIGNAL( CurrentDatasetChanged( const QString&, const QString& ) ),
    // from:
    this,
    SLOT( OnCurrentDatasetChanged( const QString&, const QString& ) )
  );

  QObject::disconnect(
    model,
    SIGNAL( SelectedDatasetModelChanged( DatasetModel* ) ),
    // from:
    this,
    SLOT( OnSelectedDatasetModelChanged( DatasetModel* ) )
  );

  //
  QObject::connect(
    widget, 
    SIGNAL( ItemMoved( QTreeWidgetItem*, QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnItemMoved( QTreeWidgetItem*, QTreeWidgetItem* ) )
    );

  //
  QObject::disconnect(
    widget,
    SIGNAL( AddItemRequested( QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnAddItemRequested( QTreeWidgetItem* ) )
    );

  //
  QObject::disconnect(
    widget,
    SIGNAL( DeleteItemRequested( QTreeWidgetItem* ) ),
    // to:
    this,
    SLOT( OnDeleteItemRequested( QTreeWidgetItem* ) )
    );

  //
  QObject::disconnect(
    widget,
    SIGNAL( ItemTextChanged( QTreeWidgetItem*, int ) ),
    // to:
    this,
    SLOT( OnItemTextChanged( QTreeWidgetItem*, int ) )
    );

  //
  QObject::disconnect(
    this,
    SIGNAL( ModelUpdated() ),
    // to:
    model,
    SIGNAL( DatabaseChanged() )
  );

  //
  QObject::disconnect(
    model,
    SIGNAL( DatabaseChanged() ),
    this,
    SLOT( RefreshWidget() )
    );
}

/*******************************************************************************/
void
DatabaseBrowserController
::ClearWidget()
{
  ResetDatasetTree( StringPairListType() );
}

/*******************************************************************************/
void
DatabaseBrowserController
::virtual_ResetWidget( bool )
{
  RefreshWidget();
}

/*******************************************************************************/
void
DatabaseBrowserController
::ResetDatasetTree( const StringPairListType & )
{
  //
  // Access widget.
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST

  // Block this controller's signals to prevent display refreshes
  // but let let widget(s) signal their changes so linked values
  // will be correctly updated.
  this->blockSignals( true );
  {
  // Block widget's signals...
  //...but force call to valueChanged() slot to force refresh.
  widget->blockSignals( true );
  {
#if 1
  // if( !datasets.isEmpty() )
  UpdateTree( widget->GetRootItem(), -1, 1 );
#else
  widget->Clear();
  UpdateTree2( widget->GetRootItem(), -1, 1 );
#endif
  }
  widget->blockSignals( false );
  }
  this->blockSignals( false );
}

/*******************************************************************************/
#define ENABLE_TREE_QDEBUG 0

void
DatabaseBrowserController
::UpdateTree( QTreeWidgetItem* item, SqlId nodeId, int level )
{
  // qDebug()
  //    << this << "::UpdateTree(" << item << "," << nodeId << "," << level << ")";

  assert( GetModel()==GetModel< DatabaseModel >() );
  DatabaseModel* model = GetModel< DatabaseModel >();

  if( model==NULL )
    return;

  assert( model->GetDatabaseConnection() );
  const DatabaseConnection* db = model->GetDatabaseConnection();

#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST
  assert( widget!=NULL );

  //
  // I. Classify QTreeWidgetNodes (Nodes/Groups, Leaves/Items,
  // Others) and find .

  typedef QList< QTreeWidgetItem* > TreeWidgetItemList;
  typedef QMap< QString, QTreeWidgetItem* > TreeWidgetItemMap;

  TreeWidgetItemMap nodes;
  TreeWidgetItemMap leaves;
  TreeWidgetItemList duplicates;
  TreeWidgetItemList others;

  for( int i=0; i<item->childCount(); ++i )
    {
    QTreeWidgetItem* child = item->child( i );
    assert( child!=NULL );

    TreeWidgetItemMap* container = NULL;

    switch( child->type() )
      {
      // Nodes.
#if DISABLE_CUSTOM_TW_ITEM
      case DatabaseBrowserWidgetTest::ITEM_TYPE_NODE:
#else // DISABLE_CUSTOM_TW_ITEM
      case TreeWidgetItem::ITEM_TYPE_NODE:
#endif // DISABLE_CUSTOM_TW_ITEM
        // assert( child->data( 1, Qt::UserRole + 1 ).isValid() );
#if ENABLE_TREE_QDEBUG
        qDebug()
          << "Node:"
          << child->text( TreeWidgetItem::COLUMN_INDEX_TEXT )
          << child->text( TreeWidgetItem::COLUMN_INDEX_ID )
          << child->data( TreeWidgetItem::COLUMN_INDEX_ID,
                          TreeWidgetItem::ITEM_ROLE_ID );
#endif
        container = &nodes;
        break;

      // Leaves.
#if DISABLE_CUSTOM_TW_ITEM
      case DatabaseBrowserWidgetTest::ITEM_TYPE_LEAF:
#else // DISABLE_CUSTOM_TW_ITEM
      case TreeWidgetItem::ITEM_TYPE_LEAF:
#endif // DISABLE_CUSTOM_TW_ITEM
        // assert( item->data( 1, Qt::UserRole + 1 ).isValid() );
#if ENABLE_TREE_QDEBUG
        qDebug()
          << "Leaf:"
          << child->text( TreeWidgetItem::COLUMN_INDEX_TEXT )
          << child->text( TreeWidgetItem::COLUMN_INDEX_ID )
          << child->data( TreeWidgetItem::COLUMN_INDEX_ID,
                          TreeWidgetItem::ITEM_ROLE_ID )
          << child->text( TreeWidgetItem::COLUMN_INDEX_HASH );
#endif
        container = &leaves;
        break;

      // Others.
      default:
        others.push_back( child );
        qDebug()
          << "Other:"
          << child->type()
#if DISABLE_CUSTOM_TW_ITEM
          << child->text( DatabaseBrowserWidgetTest::COLUMN_INDEX_TEXT );
#else // DISABLE_CUSOM_TW_ITEM
          << child->text( TreeWidgetItem::COLUMN_INDEX_TEXT );
#endif // DISABLE_CUSOM_TW_ITEM
        break;
      }

    if( container!=NULL )
      {
      TreeWidgetItemMap::iterator it(
        container->insert(
          child->data(
#if DISABLE_CUSTOM_TW_ITEM
            DatabaseBrowserWidgetTest::COLUMN_INDEX_ID,
            DatabaseBrowserWidgetTest::ITEM_ROLE_ID
#else // DISABLE_CUSOM_TW_ITEM
            TreeWidgetItem::COLUMN_INDEX_ID,
            TreeWidgetItem::ITEM_ROLE_ID
#endif // DISABLE_CUSOM_TW_ITEM
          ).toString(),
          child
        )
      );

      // assert( it==map->end() );
      if( it!=container->end() )
        {
#if ENABLE_TREE_QDEBUG
        qDebug()
          << "Duplicate:"
          << child->type()
#if DISABLE_CUSTOM_TW_ITEM
          << it.value()->text( DatabaseBrowserWidgetTest::COLUMN_INDEX_TEXT )
          << it.value()->text( DatabaseBrowserWidgetTest::COLUMN_INDEX_ID )
          << it.value()->data( DatabaseBrowserWidgetTest::COLUMN_INDEX_ID,
                               DatabaseBrowserWidgetTest::ITEM_ROLE_ID );
#else // DISABLE_CUSOM_TW_ITEM
          << it.value()->text( TreeWidgetItem::COLUMN_INDEX_TEXT )
          << it.value()->text( TreeWidgetItem::COLUMN_INDEX_ID )
          << it.value()->data( TreeWidgetItem::COLUMN_INDEX_ID,
                               TreeWidgetItem::ITEM_ROLE_ID );
#endif // DISABLE_CUSOM_TW_ITEM
#endif

        duplicates.push_back( it.value() );
        }
      }
    }

  //
  // Get node id.

  SqlId safeNodeId = 
    nodeId < 0
    ? GetRootNodeFields( db->FindRootNode() )
    : nodeId;

  //
  //  Leaves/Items (datasets).

  QSqlQuery datasetsQuery( db->SelectNodeDatasets( safeNodeId ) );

  while( datasetsQuery.next() )
    {
    QString hash;
    QString alias;

    QVariant id( GetDatasetFields( datasetsQuery, &hash, &alias ) );

#if ENABLE_TREE_QDEBUG
    qDebug() << "+Leaf:" << id.toString() << hash << alias;
#endif

    TreeWidgetItemMap::iterator it(
      leaves.find( id.toString() )
    );

    // QTreeWidgetItem* childItem = NULL;

    if( it==leaves.end() )
      {
      /*childItem = */widget->InsertLeafItem(
        item,
        alias.isEmpty()
        ? hash
        : alias,
        id,
        QStringList( hash ) );
      }
    else
      {
      /*childItem = */it.value();
      }

    leaves.erase( it );
    }

  while( !leaves.empty() )
    {
    TreeWidgetItemMap::iterator it( leaves.begin() );

#if ENABLE_TREE_QDEBUG
    qDebug()
      << "-Leaf:"
      << it.key()
      << it.value()->text( TreeWidgetItem::COLUMN_INDEX_TEXT );
#endif

    delete it.value();

    leaves.erase( it );
    }

  //
  // Nodes/Groups.

  QSqlQuery nodesQuery( db->SelectNodeChildren( safeNodeId ) );

  while( nodesQuery.next() )
    {
    QString label;

    QVariant id( GetChildNodeFields( nodesQuery, &label ) );

#if ENABLE_TREE_QDEBUG
    qDebug() << "+Node:" << id.toString() << label;
#endif

    TreeWidgetItemMap::iterator it(
      nodes.find( id.toString() )
    );

    QTreeWidgetItem* childItem = NULL;

    if( it==nodes.end() )
      {
      childItem = widget->InsertNodeItem( item, label, id );

      SqlId nodeId = -1;

      switch( level )
        {
        // Root-node level.
        case 0 :
          nodeId = GetNodeFields( db->FindRootNode() );
          break;

        // Datasets-node level.
        case 1 :
          nodeId = GetNodeFields( db->FindDatasetNode() );
          break;

        // Temporary-node level.
        case 2 :
          nodeId = GetNodeFields( db->FindTemporaryNode() );
          break;
        }

      // qDebug() << "CHECKPOINT";

      // qDebug() << level << nodeId << id << childItem->text( 0 );

      if( nodeId==id.toLongLong() )
        {
        // qDebug() << level << nodeId << id << childItem->text( 0 );
      
        childItem->setFlags( childItem->flags() & ~Qt::ItemIsEditable );
        }
      }
    else
      {
      childItem = it.value();
      }

    nodes.erase( it );

    // Recurse.
    UpdateTree( childItem, id.toLongLong(), level + 1 );
    }

  while( !nodes.empty() )
    {
    TreeWidgetItemMap::iterator it( nodes.begin() );

#if ENABLE_TREE_QDEBUG
    qDebug()
      << "-Node:"
      << it.key()
      << it.value()->text( TreeWidgetItem::COLUMN_INDEX_TEXT );
#endif

    delete it.value();

    nodes.erase( it );
    }
}

/*******************************************************************************/
void
DatabaseBrowserController
::UpdateTree2( QTreeWidgetItem* item, SqlId nodeId, int level )
{
  qDebug()
    << this << "::UpdateTree2(" << item << "," << nodeId << "," << level << ")";

  assert( GetModel()==GetModel< DatabaseModel >() );
  DatabaseModel* model = GetModel< DatabaseModel >();

  if( model==NULL )
    return;

  assert( model->GetDatabaseConnection() );
  const DatabaseConnection* db = model->GetDatabaseConnection();

#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST
  assert( widget!=NULL );

  //
  // Get node id.

  SqlId safeNodeId = 
    nodeId < 0
    ? GetRootNodeFields( db->FindRootNode() )
    : nodeId;

  //
  //  Leaves/Items (datasets).

  QSqlQuery datasetsQuery( db->SelectNodeDatasets( safeNodeId ) );

  while( datasetsQuery.next() )
    {
    QString hash;
    QString alias;

    QVariant id( GetDatasetFields( datasetsQuery, &hash, &alias ) );

#if ENABLE_TREE_QDEBUG
    qDebug() << "+Leaf:" << id.toString() << hash << alias;
#endif

    /*childItem = */widget->InsertLeafItem(
      item,
      alias.isEmpty()
      ? hash
      : alias,
      id,
      QStringList( hash )
    );
    }

  //
  // Nodes/Groups.

  QSqlQuery nodesQuery( db->SelectNodeChildren( safeNodeId ) );

  while( nodesQuery.next() )
    {
    QString label;

    QVariant id( GetChildNodeFields( nodesQuery, &label ) );

#if ENABLE_TREE_QDEBUG
    qDebug() << "+Node:" << id.toString() << label;
#endif

    QTreeWidgetItem * childItem = widget->InsertNodeItem( item, label, id );

    SqlId nodeId = -1;

    switch( level )
      {
      // Root-node level.
      case 0 :
        nodeId = GetNodeFields( db->FindRootNode() );
        break;

        // Datasets-node level.
      case 1 :
        nodeId = GetNodeFields( db->FindDatasetNode() );
        break;

        // Temporary-node level.
      case 2 :
        nodeId = GetNodeFields( db->FindTemporaryNode() );
        break;
      }

    // qDebug() << "CHECKPOINT";

    // qDebug() << level << nodeId << id << childItem->text( 0 );

    if( nodeId==id.toLongLong() )
      {
      // qDebug() << level << nodeId << id << childItem->text( 0 );
      
      childItem->setFlags( childItem->flags() & ~Qt::ItemIsEditable );
      }

    // Recurse.
    UpdateTree( childItem, id.toLongLong(), level + 1 );
    }
}

/*******************************************************************************/
void
DatabaseBrowserController
::CheckDatasetsConsistency()
{
  //
  // Access widget.
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST

  //
  // Access model.
  const DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  //
  // Retrieve items.
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidgetTest::QTreeWidgetItemList items( widget->GetItems() );
#else // ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserWidget::QTreeWidgetItemList items( widget->GetItems() );
#endif // ENABLE_TREE_WIDGET_TEST

  //
  // Traverse item list.
#if ENABLE_TREE_WIDGET_TEST
  for( DatabaseBrowserWidgetTest::QTreeWidgetItemList::iterator it( items.begin() );
#else // ENABLE_TREE_WIDGET_TEST
  for( DatabaseBrowserWidget::QTreeWidgetItemList::iterator it( items.begin() );
#endif // ENABLE_TREE_WIDGET_TEST
       it!=items.end();
       ++it )
    {
    QTreeWidgetItem* qtwi = *it;
    assert( qtwi!=NULL );

#if DISABLE_CUSTOM_TW_ITEM
#else // DISABLE_CUSTOM_TW_ITEM
    assert( qtwi==dynamic_cast< TreeWidgetItem* >( qtwi ) );
    TreeWidgetItem* item = dynamic_cast< TreeWidgetItem* >( qtwi );

    if( item!=NULL )
      {
      if( item->GetType()==TreeWidgetItem::ITEM_TYPE_LEAF )
        {
        QString hash( item->GetHash() );

        // qDebug()
        //   << "Checking dataset:"
        //   << item->GetText()
        //   << item->GetId()
        //   << item->GetHash();

        bool isConsistent = true;

        // Find database-mode.
        try
          {
          const DatasetModel* datasetModel
            = model->FindDatasetModel( hash );

          assert( datasetModel!=NULL );

          isConsistent = datasetModel->IsConsistent();
          }

        // If exception is raised, consider dataset-model as NOT consistent.
        catch( std::exception& )
          {
          isConsistent = false;
          }

        // Setup item depending on dataset-model consistency.
        if ( isConsistent )
          {
          item->setDisabled( false );
          item->setIcon( 0, QIcon() );
          item->setToolTip( 0, QString() );
          }
        else
          {
          item->setDisabled( true );
          item->setIcon( 0, QIcon( ":/icons/forbidden" ) );
          item->setToolTip( 0, tr( "Data is not consistent." )
          );
          }
        }
      }
#endif // DISABLE_CUSTOM_TW_ITEM
    }
}

/*******************************************************************************/
void
DatabaseBrowserController
::DeleteDataset( const QString& hash )
{
  assert( !hash.isEmpty() );

  // Access model.
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  // Access selected dataset-model (candidate to deletion).
  DatasetModel* datasetModel = model->FindDatasetModel( hash );
  assert( datasetModel!=NULL );

  // Create question message-box
  QMessageBox messageBox(
    QMessageBox::Question,
    tr( "Warning!" ),
    tr( "Are you sure you want to delete dataset '%1'?" )
    .arg( datasetModel->GetAlias() ),
    QMessageBox::Yes | QMessageBox::No,
    GetWidget()
  );

  messageBox.setDefaultButton( QMessageBox::No );

  VectorImageModel* imageModel =
    datasetModel->GetSelectedImageModel< VectorImageModel >();

  QCheckBox * checkBox = NULL;

  QFileInfo imgFInfo;
  QFileInfo ovrFInfo;
  QFileInfo geoFInfo;

  // Customize message-box if there are alternate files.
  if( imageModel!=NULL )
    {
    imgFInfo = QFileInfo( imageModel->GetFilename() );
    ovrFInfo = QFileInfo( imageModel->GetFilename() + ".ovr" );
    geoFInfo = QFileInfo( imgFInfo.dir(), imgFInfo.completeBaseName() + ".geom" );

    /*
    qDebug() << imgFInfo.filePath();
    qDebug() << ovrFInfo.filePath();
    qDebug() << geoFInfo.filePath();
    */

    if( imgFInfo.exists() || ovrFInfo.exists() || geoFInfo.exists() )
      {
      //
      // 1. Create a pannel widget.
      QWidget * widget = new QWidget( &messageBox );

      //
      // 2. Create it's layout.
      QVBoxLayout * layout = new QVBoxLayout();

      //
      // 3. Create and add check-box widget.
      assert( checkBox==NULL );
      checkBox = new QCheckBox( tr( "Also delete:" ), &messageBox );

      layout->addWidget( checkBox );

      //
      // 4. Create and add list widget.
      QListWidget * listWidget = new QListWidget( &messageBox );

      listWidget->setSelectionMode( QAbstractItemView::NoSelection );

      // listWidget->setEnabled( false );

      if( imgFInfo.exists() )
        listWidget->addItem( imgFInfo.filePath() );

      if( ovrFInfo.exists() )
        listWidget->addItem( ovrFInfo.filePath() );

      if( geoFInfo.exists() )
        listWidget->addItem( geoFInfo.filePath() );

      layout->addWidget( listWidget );

      //
      // 5. Add pannel to message-box.
      widget->setLayout( layout );

      CustomizeMessageBox( messageBox, widget );


      /*
      QObject::connect(
        checkBox, SIGNAL( toggled( bool ) ),
        listWidget, SLOT( setEnagled( bool ) )
      );
      */
      }
    }

  // Pop confirm delete dialog.
  if( messageBox.exec()==QMessageBox::No )
    return;

  // Remove dataset.
  model->RemoveDatasetModel( hash );

  // Remove alternate files, if any.
  if( checkBox!=NULL && checkBox->isChecked() )
    {
    DeleteFile( imgFInfo );
    DeleteFile( ovrFInfo );
    DeleteFile( geoFInfo );
    }
}

/*******************************************************************************/
void
DatabaseBrowserController
::DeleteGroup( SqlId id )
{
  // Access model.
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  // Access selected dataset-model (candidate to deletion).
  DatabaseConnection* dbc = model->GetDatabaseConnection();
  assert( dbc!=NULL );

  // Delete group from database.
  dbc->DeleteNode( id );

  // Refresh model.
  emit ModelUpdated();
}

/*******************************************************************************/
bool
DatabaseBrowserController
::DeleteFile( const QFileInfo& finfo )
{
  if( !finfo.exists() )
    return false;

  if( !finfo.dir().remove( finfo.fileName() ) )
    {
    QMessageBox::warning(
      GetWidget(),
      tr( "Error!" ),
      tr( "Failed to remove '%1'." ).arg( finfo.filePath() )
    );

    return false;
    }

  return true;
}

/*******************************************************************************/
/* SLOTS                                                                       */
/*******************************************************************************/
void
DatabaseBrowserController
::OnCurrentDatasetChanged( const QString & id, const QString & )
{
  // qDebug() << this << "::OnCurrentDatasetChanged(" << id << ")";

  if( id.isEmpty() )
    return;

  assert( GetModel()==GetModel< DatabaseModel >() );
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

#if ENABLE_TREE_WIDGET_TEST
  assert( GetWidget()==GetWidget< DatabaseBrowserWidgetTest >() );
  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  assert( GetWidget()==GetWidget< DatabaseBrowserWidget >() );
  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST
  assert( widget!=NULL );

  try
    {
    // set the newly selected dataset id
    model->SelectDatasetModel( id );
    }
  catch( std::exception& exc )
    {
    QMessageBox::warning(
      GetWidget(),
      PROJECT_NAME,
      tr( "Failed to load dataset.\n\n%1" ).arg( exc.what() ),
      QMessageBox::Ok
    );

    // assert( false && "Debug SetCurrentDataset() or remove call." );

    /*
    assert( GetWidget< DatabaseBrowserWidget >()==GetWidget() );
    GetWidget< DatabaseBrowserWidget >()->SetCurrentDataset( previousId );
    */

    // emit SelectedDatasetFilenameChanged( QString() );

    widget->SetCurrentDataset( QString() );

    return;
    }
  catch( ... )
    {
    QMessageBox::warning(
      GetWidget(),
      PROJECT_NAME,
      tr( "Failed to load dataset.\n" ),
      QMessageBox::Ok
    );

    /*
    assert( GetWidget< DatabaseBrowserWidget >()==GetWidget() );
    GetWidget< DatabaseBrowserWidget >()->SetCurrentDataset( previousId );
    */

    // emit SelectedDatasetFilenameChanged( QString() );

    widget->SetCurrentDataset( QString() );

    return;
    }

  /*
  // emit selected dataset image filename
  VectorImageModel * imageModel
    =  qobject_cast< VectorImageModel *>(
      model->GetSelectedDatasetModel()->GetSelectedImageModel()
    );

  //
  // this signal is used to pass the current dataset input filename.
  // it is connected to the DatabaseBrowserWidget custom QTreeWidget
  emit SelectedDatasetFilenameChanged( imageModel->GetFilename() );
  */

  widget->SetCurrentDataset( id );
}

/*******************************************************************************/
void
DatabaseBrowserController
::RefreshWidget()
{
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  // Reset widget.
  ResetDatasetTree( model->QueryDatasetModels() );

  // Check every dataset consistency  
  CheckDatasetsConsistency();
}

/*******************************************************************************/
void
DatabaseBrowserController
::OnSelectedDatasetModelChanged( DatasetModel* datasetModel )
{
  if( datasetModel==NULL )
    return;

  assert( GetWidget()!=NULL );
#if ENABLE_TREE_WIDGET_TEST
  assert( GetWidget()==GetWidget< DatabaseBrowserWidgetTest >() );

  DatabaseBrowserWidgetTest * widget = GetWidget< DatabaseBrowserWidgetTest >();
#else // ENABLE_TREE_WIDGET_TEST
  assert( GetWidget()==GetWidget< DatabaseBrowserWidget >() );

  DatabaseBrowserWidget* widget = GetWidget< DatabaseBrowserWidget >();
#endif // ENABLE_TREE_WIDGET_TEST

  //
  // Block widget's signals here to avoid recursive
  // SelectedDatasetModelChanged(), CurrentDatasetModelChanged()
  // loop.
  bool areSignalsBlocked = widget->blockSignals( true );
  {
  widget->SetCurrentDataset( datasetModel->GetHash() );
  }
  widget->blockSignals( areSignalsBlocked );
}

/*******************************************************************************/
void
DatabaseBrowserController
::OnItemMoved( QTreeWidgetItem * item, QTreeWidgetItem * target )
{
  // Trace.
  qDebug() << this << "::OnItemMoved(" << item << "," << target << ");";

  // Check inputs.
  assert( item!=NULL );

#if DISABLE_CUSTOM_TW_ITEM

#else // DISABLE_CUSTOM_TW_ITEM

  // Access item and parent.
  assert( item==dynamic_cast< TreeWidgetItem* >( item ) );
  TreeWidgetItem* childItem = dynamic_cast< TreeWidgetItem* >( item );

#if 0
  QTreeWidgetItem * parent = item->parent();

#if 1
  qDebug()
    << "Item (parent):"
    << parent << "\n"
    << "text[ 0 ]:" << parent->text( 0 ) << "\n"
    << "text[ 1 ]:" << parent->text( 1 ) << "\n"
    << "text[ 2 ]:" << parent->text( 2 );

  if( parent->parent()!=NULL )
    qDebug()
      << "parent:" << parent->parent() << "\n"
      << "text[ 0 ]:" << parent->parent()->text( 0 ) << "\n"
      << "text[ 1 ]:" << parent->parent()->text( 1 );
#endif

#else
  // Find parent-item by climbing parent hierarchy based on
  // drop-enabled flag.
  //
  // N.B.: item->parent() may not be updated yet by QTreeWidget.
  QTreeWidgetItem * parent = target;
  assert( parent!=NULL );

  while( !parent->flags().testFlag( Qt::ItemIsDropEnabled ) )
    {
#if 1
    qDebug()
      << "Item (parent):"
      << parent << "\n"
      << "text[ 0 ]:" << parent->text( 0 ) << "\n"
      << "text[ 1 ]:" << parent->text( 1 ) << "\n"
      << "text[ 2 ]:" << parent->text( 2 );

    if( parent->parent()!=NULL )
      qDebug()
        << "parent:" << parent->parent() << "\n"
        << "text[ 0 ]:" << parent->parent()->text( 0 ) << "\n"
        << "text[ 1 ]:" << parent->parent()->text( 1 );
#endif

    parent = parent->parent();
    assert( parent!=NULL ); 
    }
#endif

  // Access item-parent.
  assert( parent==dynamic_cast< TreeWidgetItem* >( parent ) );
  TreeWidgetItem* parentItem = dynamic_cast< TreeWidgetItem* >( parent );

  /*
  if( parentItem==NULL )
    return;
  */

  // Check item type.
  assert( childItem->GetType()==TreeWidgetItem::ITEM_TYPE_LEAF );


  // Access model & database.
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  // Access database connection.
  assert( model->GetDatabaseConnection() );
  DatabaseConnection* db = model->GetDatabaseConnection();


  /*
  qDebug()
    << "dataset_node_membership("
    << childItem->GetId().toLongLong()
    << parentItem->GetId().toLongLong()
    << ")";
  */

  // Update database.
  db->UpdateDatasetNodeMembership(
    childItem->GetId().toLongLong(),
    parentItem->GetId().toLongLong()
  );

  // No need to update model/refresh view because widget has already
  // been repainted.

#endif // DISABLE_CUSTOM_TW_ITEM
}

/*******************************************************************************/
void
DatabaseBrowserController
::OnAddItemRequested( QTreeWidgetItem* parent )
{
  // Trace.
  // qDebug() << this << "::OnAddItemRequested(" << parent << ");";

  // Check inputs.
  assert( parent!=NULL );

#if DISABLE_CUSTOM_TW_ITEM

#else // DISABLE_CUSTOM_TW_ITEM

  // Access item and parent.
  assert( parent==dynamic_cast< TreeWidgetItem* >( parent ) );
  TreeWidgetItem* parentItem = dynamic_cast< TreeWidgetItem* >( parent );


  // Access model & database.
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  // Access database connection.
  assert( model->GetDatabaseConnection() );
  DatabaseConnection* db = model->GetDatabaseConnection();

  if( parentItem->type()!=TreeWidgetItem::ITEM_TYPE_NODE )
    return;

  // Add group.
  db->InsertNode(
    tr( "New group" ),
    parentItem->GetId().toLongLong()
  );

  // Refresh model.
  emit ModelUpdated();

#endif // DISABLE_CUSTOM_TW_ITEM
}

/*******************************************************************************/
void
DatabaseBrowserController
::OnDeleteItemRequested( QTreeWidgetItem* item )
{
  // Trace.
  // qDebug() << this << "::OnDeleteItemRequested(" << item << ");";

  // Check inputs.
  assert( item!=NULL );

#if DISABLE_CUSTOM_TW_ITEM

#else // DISABLE_CUSTOM_TW_ITEM

  // Access item and parent.
  assert( item==dynamic_cast< TreeWidgetItem* >( item ) );
  TreeWidgetItem* twi = dynamic_cast< TreeWidgetItem* >( item );

  switch( twi->type() )
    {
    case TreeWidgetItem::ITEM_TYPE_NONE:
      break;

    case TreeWidgetItem::ITEM_TYPE_NODE:
      DeleteGroup( twi->GetId().toLongLong() );
      break;

    case TreeWidgetItem::ITEM_TYPE_LEAF:
      DeleteDataset( twi->GetHash() );
      break;
    }

#endif // DISABLE_CUSTOM_TW_ITEM
}

/*******************************************************************************/
void
DatabaseBrowserController
::OnItemTextChanged( QTreeWidgetItem* item, int column )
{
  // Trace.
  // qDebug() << this << "::OnItemTextChanged(" << item << "," << column << ");";

  // Check inputs.
  assert( item!=NULL );
  assert( column>=0 );

#if DISABLE_CUSTOM_TW_ITEM

#else // DISABLE_CUSTOM_TW_ITEM

  // Access item and parent.
  assert( item==dynamic_cast< TreeWidgetItem* >( item ) );
  TreeWidgetItem* twi = dynamic_cast< TreeWidgetItem* >( item );

  // Access model & database.
  DatabaseModel* model = GetModel< DatabaseModel >();
  assert( model!=NULL );

  switch( twi->type() )
    {
    case TreeWidgetItem::ITEM_TYPE_NONE:
      break;

    case TreeWidgetItem::ITEM_TYPE_NODE:
      if( column==0 )
        {
        DatabaseConnection* dbc = model->GetDatabaseConnection();
        assert( dbc!=NULL );

        dbc->UpdateNodeLabel( twi->GetId().toLongLong(), twi->GetText() );
        }
      break;

    case TreeWidgetItem::ITEM_TYPE_LEAF:
      if( column==0 )
        model->RenameDataset( twi->GetHash(), twi->GetText() );    
      break;
    }

#endif // DISABLE_CUSTOM_TW_ITEM
}

} // end namespace 'mvd'
