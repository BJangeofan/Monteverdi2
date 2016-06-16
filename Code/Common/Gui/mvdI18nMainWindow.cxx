/*=========================================================================

  Program:   Monteverdi
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#include "mvdI18nMainWindow.h"

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.
#include <QtGui>

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)
#include "otbSystem.h"

//
// Monteverdi includes (sorted by alphabetic order)
#include "Core/mvdBackgroundTask.h"
#include "Core/mvdDatabaseConnection.h"
#include "Core/mvdDatasetModel.h"
#include "Core/mvdImageImporter.h"
#include "Core/mvdOverviewBuilder.h"
#include "Core/mvdVectorImageModel.h"
#include "Gui/mvdAboutDialog.h"
#include "Gui/mvdI18nApplication.h"
#include "Gui/mvdImportImagesDialog.h"
#include "Gui/mvdTaskProgressDialog.h"

namespace mvd
{
  
/*
  TRANSLATOR mvd::I18nMainWindow

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CONSTANTS                                                                 */

namespace
{
} // end of anonymous namespace.

/*****************************************************************************/
/* STATIC IMPLEMENTATION SECTION                                             */

QDir
I18nMainWindow::m_LastAcceptedDir( QDir::home().path() );

/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*****************************************************************************/
I18nMainWindow
::I18nMainWindow( QWidget* parent, Qt::WindowFlags flags ) :
  QMainWindow( parent, flags )
{
}

/*****************************************************************************/
I18nMainWindow
::~I18nMainWindow()
{
}

/*****************************************************************************/
void
I18nMainWindow
::Initialize()
{
  /*
  // Default setup.
  setObjectName( "mvd::I18nMainWindow" );
  setWindowTitle( PROJECT_NAME );
  */

  virtual_SetupUI();

  // Connect Appllication and MainWindow when selected model is about
  // to change.
  QObject::connect(
    I18nApplication::Instance(),
    SIGNAL( AboutToChangeModel( const AbstractModel* ) ),
    this,
    SLOT( OnAboutToChangeModel( const AbstractModel* ) )
  );

  // Connect Application and MainWindow when selected model has been
  // changed.
  QObject::connect(
    I18nApplication::Instance(),
    SIGNAL( ModelChanged( AbstractModel* ) ),
    this,
    SLOT( OnModelChanged( AbstractModel* ) )
  );

  virtual_ConnectUI();

  virtual_InitializeUI();
}

/*****************************************************************************/
QDockWidget*
I18nMainWindow
::AddWidgetToDock( QWidget* widget,
                   const QString& dockName,
                   const QString& dockTitle,
                   Qt::DockWidgetArea dockArea,
                   bool isFloating )
{
  // New dock.
  QDockWidget* dockWidget = new QDockWidget( dockTitle, this );

  // You can use findChild( dockName ) to get dock-widget.
  dockWidget->setObjectName( dockName );
  dockWidget->setWidget( widget );

  // Features.
  dockWidget->setFloating( isFloating );
  dockWidget->setFeatures(
    QDockWidget::DockWidgetMovable |
    QDockWidget::DockWidgetFloatable |
    QDockWidget::DockWidgetClosable
  );

  // dockWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

  // dockWidget->adjustSize();

  // Add dock.
  addDockWidget( dockArea, dockWidget );

  //
  return dockWidget;
}

/*****************************************************************************/
DatasetModel *
I18nMainWindow
::ImportDataset( const QString& filename,
                 int width,
                 int height,
                 bool forceCreate )
{
  return
    QObjectCast< DatasetModel * >(
      Import(
        // New dataset-importer worker.
        // It will be auto-deleted by background-task.
        new ImageImporter(
          filename,
          forceCreate,
          width, height
        )
      ),
      "QObject is not a DatasetModel."
    );
}

/*****************************************************************************/
VectorImageModel *
I18nMainWindow
::ImportImage( const QString & filename,
	       bool isComplex,
               int width,
               int height )
{
  return
    QObjectCast< VectorImageModel * >(
      Import(
        // New dataset-importer worker.
        // It will be auto-deleted by background-task.
        new ImageImporter(
          filename,
	  isComplex,
          width, height
        )
      ),
      "QObject is not a VectorImageModel."
    );
}

/*****************************************************************************/
bool
I18nMainWindow
::BuildGDALOverviews( const QStringList & filenames )
{
  ImportImagesDialog * importDialog = new ImportImagesDialog( filenames, this );

  if( importDialog->GetEffectiveCount()<1 )
    return true;

  int result = importDialog->exec();

  if( result== QDialog::Rejected )
    return false;

  if( result==QDialog::Accepted )
    {
    // AbstractWorker will be automatically deleted by BackgroundTask in
    // ::Import().
    OverviewBuilder * builder =
      new OverviewBuilder(
	importDialog->GetGDALOverviewsBuilders()
      );

    delete importDialog;
    importDialog = NULL;

    Import( builder );
    }

  return true;
}

/*****************************************************************************/
QObject *
I18nMainWindow
::Import( AbstractWorker * importer )
{
  assert( importer );

  //
  // Background task.

  // New background-task running worker.
  // Will be self auto-deleted when worker has finished.
  BackgroundTask* task = new BackgroundTask( importer, false, this );

  //
  // Progress dialog.
  TaskProgressDialog progress(
    task,
    this,
    Qt::CustomizeWindowHint | Qt::WindowTitleHint
  );

  progress.setWindowModality( Qt::WindowModal );
  progress.setAutoReset( false );
  progress.setAutoClose( false );
  progress.setCancelButton( NULL );
  progress.setMinimumDuration( 0 );

  //
  // Result.
  int button = progress.Exec();

  // MANTIS-921 (synchronize deletion of BackgroungTask).
  task->wait();
  delete task;
  task = NULL;

  // MANTIS-921 (then, process result).
  if( button!=QDialog::Accepted )
    {
    assert( progress.GetObject()==NULL );

    return NULL;
    }

  // qDebug() << "object:" << progress.GetObject< DatasetModel >();

  // assert( progress.GetObject()!=NULL );

  return progress.GetObject();
}

/*****************************************************************************/
void
I18nMainWindow
::closeEvent( QCloseEvent * event )
{
  QMainWindow::closeEvent( event );
}

/*****************************************************************************/
void
I18nMainWindow
::SetupCacheDir()
{
  if( I18nCoreApplication::ConstInstance()->IsCacheDirValid() )
    return;

  QFileInfo fileInfo( QDir::home(), I18nApplication::DEFAULT_CACHE_DIR_NAME );
  QDir dir( fileInfo.dir() );

  QMessageBox messageBox;

  messageBox.setWindowTitle( tr( "Welcome new Monteverdi user!" ) );

  messageBox.setText(
    tr(
      "Monteverdi maintains a repository where cached data related to images is stored "
      "(such as, for example, color-settings, histogram, quicklook etc.)."
      "\n\n"
      "Default cache-directory location is: '%1'."
      "\n\n"
      "Warning:\n"
      "- Your account needs to have write access to this directory;\n"
      "- Please be aware that this directory can consume significant disk "
      "space."
    )
    .arg( dir.path() )
  );

  messageBox.setInformativeText(
    tr( "Do you want to specify a custom repository location?" )
  );

  messageBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
  messageBox.setDefaultButton( QMessageBox::No );

  if( messageBox.exec()==QMessageBox::Yes )
    {
    QString path(
      I18nMainWindow::GetExistingDirectory(
        this,
        tr(
          "Please, select directory where the Monteverdi"
          " cache repository will be stored."
        ),
        dir.path()
      )
    );

    if( !path.isEmpty() )
      dir.setPath( path );
    }

  bool isNew = I18nApplication::Instance()->MakeCacheDir( dir.path() );

  //
  // Setup initial empty database
  if( isNew )
    DatabaseConnection::InitializeDatabase();
  else
    qWarning()<<tr("The selected directory already contain a mvd2 directory, no new database will be created"); 
}

/*****************************************************************************/
void
I18nMainWindow
::virtual_InitializeUI()
{
  // Change to NULL model to force emitting GUI signals when GUI is
  // instanciated. So, GUI will be initialized and controller-widgets
  // disabled.
  I18nApplication::Instance()->SetModel( NULL );
}

/*****************************************************************************/
void
I18nMainWindow
::SaveLayout( int version ) const
{
  // qDebug() << this << "::SaveLayout()";

  assert( I18nCoreApplication::Instance()!=NULL );

  QString name( objectName() );

  I18nCoreApplication::Instance()
    ->StoreSettingsKey( name + "Geometry", saveGeometry() );

  I18nCoreApplication::Instance()
    ->StoreSettingsKey( name + "State", saveState( version ) );
}

/*****************************************************************************/
bool
I18nMainWindow
::RestoreLayout( int version )
{
  // qDebug() << this << "::RestoreLayout()";

  I18nCoreApplication * application = I18nCoreApplication::Instance();
  assert( application!=NULL );

  QString name( objectName() );
  assert( !name.isEmpty() );

  if( !restoreGeometry(
	application->RetrieveSettingsKey( name + "Geometry" ).toByteArray() ) )
    return false;

  return
    restoreState(
      application->RetrieveSettingsKey( name + "State" ).toByteArray(),
      version
    );
}

/*****************************************************************************/
/* SLOTS                                                                     */
/*****************************************************************************/
void
I18nMainWindow
::on_action_Quit_triggered()
{
  close();
}

/*****************************************************************************/
void
I18nMainWindow
::on_action_About_triggered()
{
  AboutDialog aboutDialog( this );

  aboutDialog.exec();
}

/*****************************************************************************/

} // end namespace 'mvd'
