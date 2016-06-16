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
#ifndef __mvdDatabaseBrowserController_h
#define __mvdDatabaseBrowserController_h

//
// Configuration include.
//// Included at first position before any other ones.
#include "ConfigureMonteverdi.h"


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
#include "Gui/mvdAbstractModelController.h"
#include "Gui/mvdGui.h"

#define ENABLE_TREE_WIDGET_TEST ((defined( _DEBUG ) && 0) || 0)


/*****************************************************************************/
/* PRE-DECLARATION SECTION                                                   */

//
// External classes pre-declaration.
namespace
{
}

namespace mvd
{
//
// Internal classes pre-declaration.
#if ENABLE_TREE_WIDGET_TEST
class DatabaseBrowserWidgetTest;
#else // ENABLE_TREE_WIDGET_TEST
class DatabaseBrowserWidget;
#endif // ENABLE_TREE_WIDGET_TEST
class DatasetModel;

/*****************************************************************************/
/* CLASS DEFINITION SECTION                                                  */

/**
 * \class DatabaseBrowserController
 *
 * \brief Controller of database browser widget (for
 * DatabaseModeleModel objects).
 */
class Monteverdi_EXPORT DatabaseBrowserController :
    public AbstractModelController
{

  /*-[ QOBJECT SECTION ]-----------------------------------------------------*/

  Q_OBJECT;

  /*-[ PUBLIC SECTION ]------------------------------------------------------*/

//
// Public methods.
public:

  /**
   * \brief Constructor.
   *
   * \param widget Controlled widget.
   * \param parent Parent QObject of this QObject.
   */
#if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserController( DatabaseBrowserWidgetTest * widget,
			     QObject* parent =NULL );
#else // #if ENABLE_TREE_WIDGET_TEST
  DatabaseBrowserController( DatabaseBrowserWidget* widget,
			     QObject* parent =NULL );
#endif // #if ENABLE_TREE_WIDGET_TEST

  /**
   * \brief Destructor.
   */
  virtual ~DatabaseBrowserController();


  /*-[ SIGNALS SECTION ]-----------------------------------------------------*/

//
// Signals.
signals:

  /**
   */
  // void SelectedDatasetFilenameChanged( const QString& );

  /*-[ PUBLIC SLOTS SECTION ]------------------------------------------------*/

//
// Slots.
public slots:

  /**
   *  \brief
   */
  void CheckDatasetsConsistency();

  /*-[ PROTECTED SECTION ]---------------------------------------------------*/

//
// Protected methods.
protected:

//
// Protected attributes.
protected:

  /*-[ PRIVATE SECTION ]-----------------------------------------------------*/

//
// Private methods.
private:

  /**
   */
  void ResetDatasetTree( const StringPairListType& datasets );

  /**
   */
  void UpdateTree( QTreeWidgetItem* item, SqlId nodeId, int level );
  void UpdateTree2( QTreeWidgetItem* item, SqlId nodeId, int level );

  //
  // AbstractModelController overrides.

  virtual void Connect( AbstractModel* );

  virtual void ClearWidget();

  virtual void virtual_ResetWidget( bool );

  virtual void Disconnect( AbstractModel* );


//
// Private attributes.
private:
  /**
   */
  void DeleteDataset( const QString& hash );
  /**
   */
  void DeleteGroup( SqlId id );

  /**
   */
  bool DeleteFile( const QFileInfo& finfo );

  /*-[ PRIVATE SLOTS SECTION ]-----------------------------------------------*/

//
// Slots.
private slots:

  /**
   * \brief Slot called when the currently selected dataset has changed.
   *
   * \param hash The hash string (unique name) of the newly selected
   * dataset.
   * \param previousHash The hash string (unique name) of the
   * previously selected dataset.
   */
  void OnCurrentDatasetChanged( const QString& hash,
                                const QString& previousHash );

  /**
   */
  void OnSelectedDatasetModelChanged( DatasetModel* );

  /**
   */
  void OnItemMoved( QTreeWidgetItem * item, QTreeWidgetItem * target );

  /**
   */
  void OnAddItemRequested( QTreeWidgetItem* parent );

  /**
   */
  void OnDeleteItemRequested( QTreeWidgetItem* item );

  /**
   */
  void OnItemTextChanged( QTreeWidgetItem* item, int column );

  //
  // AbstractModelController overloads.

  virtual void RefreshWidget();
};

} // end namespace 'mvd'.

/*****************************************************************************/
/* INLINE SECTION                                                            */

namespace mvd
{
} // end namespace 'mvd'

#endif // __mvdDatabaseBrowserController_h
