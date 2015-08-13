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
#ifndef __mvdLayerStackWidget_h
#define __mvdLayerStackWidget_h

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
class LayerStackItemModel;

//
// Internal classes pre-declaration.
namespace Ui
{
class LayerStackWidget;
};


/*****************************************************************************/
/* CLASS DEFINITION SECTION                                                  */

/**
 * \class LayerStackWidget
 *
 * \brief Widget template skeleton to copy-paste when adding a new
 * widget class.
 */
class Monteverdi2_EXPORT LayerStackWidget :
    public QWidget
{

  /*-[ QOBJECT SECTION ]-----------------------------------------------------*/

  Q_OBJECT;

  /*-[ PUBLIC SECTION ]------------------------------------------------------*/

//
// Public methods.
public:

  /** \brief Constructor. */
  LayerStackWidget( QWidget* parent =NULL, Qt::WindowFlags flags =0 );

  /** \brief Destructor. */
  virtual ~LayerStackWidget();

  /**
   */
  const LayerStackItemModel * GetItemModel() const;

  /**
   */
  LayerStackItemModel * GetItemModel();

  /**
   */
  void SetCurrent( int );

  /**
   */
  void SetProjectionEnabled( bool );

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
  void CurrentChanged( int );
  /**
   */
  void SelectionChanged( int );
  /**
   */
  void TopButtonClicked();
  /**
   */
  void BottomButtonClicked();
  /**
   */
  void UpButtonClicked();
  /**
   */
  void DownButtonClicked();
  /**
   */
  void DeleteButtonClicked();
  /**
   */
  void ProjectionButtonClicked();
  /**
   */
  void ApplyButtonClicked();
  /**
   */
  void ReloadButtonClicked();

  /*-[ PROTECTED SECTION ]---------------------------------------------------*/

//
// Protected methods.
protected:

  /*-[ PRIVATE SECTION ]-----------------------------------------------------*/

//
// Protected attributes.
protected:
  /**
   * \see http://qt-project.org/doc/qt-4.8/qabstractitemview.html#dropEvent
   */
  // virtual void dopEvent( QDropEvent * event );

//
// Private methods.
private:

//
// Private attributes.
private:
  /**
   * \brief uic generated.
   */
  Ui::LayerStackWidget * m_UI;

  /*-[ PRIVATE SLOTS SECTION ]-----------------------------------------------*/

//
// Slots.
private slots:
  /**
   * \see http://qt-project.org/doc/qt-4.8/qitemselectionmodel.html#currentRowChanged
   */
  void OnCurrentRowChanged( const QModelIndex &, const QModelIndex & );

  /**
   * \see http://qt-project.org/doc/qt-4.8/qitemselectionmodel.html#selectionChanged
   */
  void OnSelectionChanged( const QItemSelection & selected,
                           const QItemSelection & deselected );
};

} // end namespace 'mvd'

/*****************************************************************************/
/* INLINE SECTION                                                            */

namespace mvd
{
} // end namespace 'mvd'

#endif // __mvdLayerStackWidget_h
