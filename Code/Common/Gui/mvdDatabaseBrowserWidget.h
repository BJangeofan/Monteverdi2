/*=========================================================================

  Program:   Monteverdi2
  Language:  C++


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See Copyright.txt for details.

  Monteverdi2 is distributed under the CeCILL licence version 2. See
  Licence_CeCILL_V2-en.txt or
  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt for more details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mvdDatabaseBrowserWidget_h
#define __mvdDatabaseBrowserWidget_h

//
// Configuration include.
//// Included at first position before any other ones.
#include "ConfigureMonteverdi2.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

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

//
// Monteverdi includes (sorted by alphabetic order)


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
namespace Ui
{
class DatabaseBrowserWidget;
};


/*****************************************************************************/
/* CLASS DEFINITION SECTION                                                  */

/**
 * \class DatabaseBrowserWidget
 *
 * \brief Widget template skeleton to copy-paste when adding a new
 * widget class.
 */
class Monteverdi2_EXPORT DatabaseBrowserWidget :
    public QWidget
{

  /*-[ QOBJECT SECTION ]-----------------------------------------------------*/

  Q_OBJECT;

  /*-[ PUBLIC SECTION ]------------------------------------------------------*/

//
// Public methods.
public:

  /** \brief Constructor. */
  DatabaseBrowserWidget( QWidget* parent =NULL, Qt::WindowFlags flags =0 );

  /** \brief Destructor. */
  virtual ~DatabaseBrowserWidget();

  /**
   */
  void SetDatasetList( const QStringList& list );

  /**
   */
  void SetCurrentDataset( const QString& name );

  /*-[ PUBLIC SLOTS SECTION ]------------------------------------------------*/

//
// Public SLOTS.
public slots:

  /*-[ SIGNALS SECTION ]-----------------------------------------------------*/

//
// Signals.
signals:

  /**
   */
  void CurrentDatasetChanged( const QString& text );

  /*-[ PROTECTED SECTION ]---------------------------------------------------*/

//
// Protected methods.
protected:

  void mousePressEvent( QMouseEvent * event );
  void mouseMoveEvent ( QMouseEvent * event );

//
// Protected attributes.
protected:

  /*-[ PRIVATE SECTION ]-----------------------------------------------------*/

//
// Private types.
private:
  /**
   */
  QList< QTreeWidgetItem* > TreeWidgetItemList;

//
// Private methods.
private:
  /**
   */
  void SetupUI();

//
// Private attributes.
private:
  /**
   * \brief uic generated.
   */
  Ui::DatabaseBrowserWidget* m_UI;

  /**
   */
  QTreeWidgetItem* m_DatasetRootItem;


  /**
   */
  QPoint    m_StartDragPosition;

  /*-[ PRIVATE SLOTS SECTION ]-----------------------------------------------*/

//
// Slots.
private slots:

  /**
   */
  void on_databaseTreeWidget_currentItemChanged( QTreeWidgetItem* current,
						 QTreeWidgetItem* previous );
};

} // end namespace 'mvd'

/*****************************************************************************/
/* INLINE SECTION                                                            */

namespace mvd
{
} // end namespace 'mvd'

#endif // __mvdDatabaseBrowserWidget_h