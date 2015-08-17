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

#include "mvdImageViewManipulator.h"

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
#include "Gui/mvdImageViewRenderer.h"

#define MOUSE_WHEEL_STEP_DEGREES 15

namespace mvd
{
/*
  TRANSLATOR mvd::ImageViewManipulator

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CONSTANTS                                                                 */

const int ImageViewManipulator::DEFAULT_GRANULARITY = 1;
const int ImageViewManipulator::DEFAULT_ALPHA_GRANULARITY = 10;
const double ImageViewManipulator::DEFAULT_DYNAMICS_SHIFT_GRANULARITY = 0.01;
const int ImageViewManipulator::DEFAULT_SCROLL_GRANULARITY = 4;
const int ImageViewManipulator::DEFAULT_ZOOM_GRANULARITY = 2;

const double ImageViewManipulator::DEFAULT_DELTA = 0.1;

/*****************************************************************************/
/* STATIC IMPLEMENTATION SECTION                                             */
/*****************************************************************************/

/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */
/*****************************************************************************/
#if USE_VIEW_SETTINGS_SIDE_EFFECT

ImageViewManipulator
::ImageViewManipulator( const otb::ViewSettings::Pointer& viewSettings,
                        QObject* parent ) :
  AbstractImageViewManipulator( parent ),
  m_MousePressPosition(),
  m_ViewSettings( viewSettings ),
  m_NativeSpacing(),
  m_MousePressOrigin(),
  m_RenderMode( AbstractImageViewRenderer::RenderingContext::RENDER_MODE_FULL ),
  m_ZoomFactor( 1.0 ),
  m_AlphaGranularity( ImageViewManipulator::DEFAULT_ALPHA_GRANULARITY ),
  m_DynamicsShiftGranularity( ImageViewManipulator::DEFAULT_DYNAMICS_SHIFT_GRANULARITY ),
  m_ScrollGranularity( ImageViewManipulator::DEFAULT_SCROLL_GRANULARITY ),
  m_ZoomGranularity( ImageViewManipulator::DEFAULT_ZOOM_GRANULARITY ),
  m_IsMouseDragging( false )
{
  m_NativeSpacing.Fill( 1.0 );
}

#else // USE_VIEW_SETTINGS_SIDE_EFFECT

ImageViewManipulator
::ImageViewManipulator( QObject* parent ) :
  AbstractImageViewManipulator( parent ),
  m_MousePressPosition(),
  m_ViewSettings( otb::ViewSettings::New() ),
  m_NativeSpacing(),
  m_MousePressOrigin(),
  m_RenderMode( AbstractImageViewRenderer::RenderingContext::RENDER_MODE_FULL ),
  m_ZoomFactor( 1.0 ),
  m_AlphaGranularity( ImageViewManipulator::DEFAULT_ALPHA_GRANULARITY ),
  m_DynamicsShiftGranularity( ImageViewManipulator::DEFAULT_DYNAMICS_GRANULARITY ),
  m_ScrollGranularity( ImageViewManipulator::DEFAULT_SCROLL_GRANULARITY ),
  m_ZoomGranularity( ImageViewManipulator::DEFAULT_ZOOM_GRANULARITY ),
  m_IsMouseDragging( false )
{
  m_NativeSpacing.Fill( 1.0 );
}

#endif // USE_VIEW_SETTINGS_SIDE_EFFECT

/*****************************************************************************/
ImageViewManipulator
::~ImageViewManipulator()
{
}

/******************************************************************************/
void
ImageViewManipulator
::SetViewportSize( int width, int height )
{
  SizeType size;

  size[ 0 ] = width;
  size[ 1 ] = height;

  assert( !m_ViewSettings.IsNull() );

  m_ViewSettings->SetViewportSize( size );
}

/******************************************************************************/
SizeType
ImageViewManipulator
::GetViewportSize() const
{
  assert( !m_ViewSettings.IsNull() );

  return m_ViewSettings->GetViewportSize();
}

/******************************************************************************/
void
ImageViewManipulator
::SetOrigin( const PointType& origin )
{
  assert( !m_ViewSettings.IsNull() );

  m_ViewSettings->SetOrigin( origin );
}

/******************************************************************************/
PointType
ImageViewManipulator
::GetOrigin() const
{
  assert( !m_ViewSettings.IsNull() );

  return m_ViewSettings->GetOrigin();
}

/******************************************************************************/
void
ImageViewManipulator
::SetSpacing( const SpacingType& spacing )
{
  assert( !m_ViewSettings.IsNull() );

  qDebug() << this << "::SetSpacing(" << spacing[ 0 ] << "," << spacing[ 1 ] << ")";

  m_ViewSettings->SetSpacing( spacing );
}

/******************************************************************************/
SpacingType
ImageViewManipulator
::GetSpacing() const
{
  assert( !m_ViewSettings.IsNull() );

  return m_ViewSettings->GetSpacing();
}

/******************************************************************************/
void
ImageViewManipulator
::SetNativeSpacing( const SpacingType& spacing )
{
  m_NativeSpacing = spacing;
}

/******************************************************************************/
void
ImageViewManipulator
::SetWkt( const std::string& wkt )
{
  assert( !m_ViewSettings.IsNull() );

  m_ViewSettings->SetWkt( wkt );
}

/******************************************************************************/
void
ImageViewManipulator
::SetKeywordList( const DefaultImageType::ImageKeywordlistType& kwl )
{
  assert( !m_ViewSettings.IsNull() );

  m_ViewSettings->SetKeywordList( kwl );
}

/******************************************************************************/
PointType
ImageViewManipulator
::GetCenter() const
{
  assert( !m_ViewSettings.IsNull() );

  return m_ViewSettings->GetViewportCenter();
}

/******************************************************************************/
void
ImageViewManipulator
::SetupRenderingContext(
  AbstractImageViewRenderer::RenderingContext * const c ) const
{
  assert(
    c==dynamic_cast< ImageViewRenderer::RenderingContext const * >( c )
  );

  ImageViewRenderer::RenderingContext * const context =
    dynamic_cast< ImageViewRenderer::RenderingContext * const >( c );

  context->m_RenderMode =
    m_IsMouseDragging
    ? AbstractImageViewRenderer::RenderingContext::RENDER_MODE_LIGHT
    : m_RenderMode;

#if USE_VIEW_SETTINGS_SIDE_EFFECT
#else // USE_VIEW_SETTINGS_SIDE_EFFECT
  context->m_ViewSettings->SetOrigin( m_ViewSettings->GetOrigin() );
  context->m_ViewSettings->SetSpacing( m_ViewSettings->GetSpacing() );
  context->m_ViewSettings->SetViewportSize( m_ViewSettings->GetViewportSize() );
  context->m_ViewSettings->SetWkt( m_ViewSettings->GetWkt() );
  context->m_ViewSettings->SetKeywordList( m_ViewSettings->GetKeywordList() );
  context->m_ViewSettings->SetUseProjection( m_ViewSettings->GetUseProjection() );
  context->m_ViewSettings->SetGeometryChanged(
    m_ViewSettings->GetGeometryChanged()
  );
#endif // USE_VIEW_SETTINGS_SIDE_EFFECT
}

/******************************************************************************/
void
ImageViewManipulator
::CenterOn( const PointType& point )
{
  assert( !m_ViewSettings.IsNull() );

  qDebug() << this << "::CenterOn(" << point[ 0 ] << "," << point[ 1 ] << ")";

  m_ViewSettings->Center( point );

  emit RoiChanged( GetOrigin(), GetViewportSize(), GetSpacing(), point );
}

/******************************************************************************/
void
ImageViewManipulator
::ZoomTo( double scale )
{
  // qDebug() << this << "::ZoomTo(" << scale << ")";

  assert( scale!=0.0 );

  assert( !m_ViewSettings.IsNull() );

  // Remember center of viewport.
  otb::ViewSettings::PointType center( m_ViewSettings->GetViewportCenter() );

  // Calculate spacing based on scale.
#if 0
  otb::ViewSettings::SpacingType spacing( m_ViewSettings->GetSpacing() );

  spacing[ 0 ] = ( spacing[ 0 ]>0.0 ? 1.0 : -1.0 ) / scale;
  spacing[ 1 ] = ( spacing[ 1 ]>0.0 ? 1.0 : -1.0 ) / scale;
#else
  otb::ViewSettings::SpacingType spacing( m_NativeSpacing );

  // Here, zoom to arbitray scale-factor relative to
  // viewport spacing.
  //
  // If viewport spacing has previously been set to
  // image-spacing, it zooms to arbitrary scale-factor.
  //
  // This is especially usefull to set user-arbitrary scale level
  // such as when editing scale-level in status-bar.
  spacing[ 0 ] /= scale;
  spacing[ 1 ] /= scale;
#endif

  // Change spacing and center.
  m_ViewSettings->SetSpacing( spacing );
  m_ViewSettings->Center( center );

  // Emit ROI changed.
  emit RoiChanged( GetOrigin(), GetViewportSize(), GetSpacing(), center );

  // emit RenderingContextChanged(center,GetSpacing()[0]);
}

/******************************************************************************/
void
ImageViewManipulator
::ZoomIn()
{
  otb::ViewSettings::SizeType size( GetViewportSize() );

  PointType point;

  Scale(
    QPoint( size[ 0 ] / 2, size[ 1 ] / 2 ),
    m_ZoomGranularity * MOUSE_WHEEL_STEP_DEGREES,
    &point
  );

  emit RoiChanged( GetOrigin(), GetViewportSize(), GetSpacing(), point );
}

/******************************************************************************/
void
ImageViewManipulator
::ZoomOut()
{
  otb::ViewSettings::SizeType size( GetViewportSize() );

  PointType point;

  Scale(
    QPoint( size[ 0 ] / 2, size[ 1 ] / 2 ),
    -m_ZoomGranularity * MOUSE_WHEEL_STEP_DEGREES,
    &point
  );

  emit RoiChanged( GetOrigin(), GetViewportSize(), GetSpacing(), point );
}

/******************************************************************************/
const PointType&
ImageViewManipulator
::Transform( PointType& viewport, const QPoint& screen ) const
{
  assert( !m_ViewSettings.IsNull() );

  m_ViewSettings->ScreenToViewPortTransform(
    static_cast< double >( screen.x() ),
    static_cast< double >( screen.y() ),
    viewport[ 0 ],
    viewport[ 1 ]
  );

  return viewport;
}

/******************************************************************************/
void
ImageViewManipulator
::MousePressEvent( QMouseEvent* event )
{
  assert( event!=NULL );

  // qDebug() << this << ":" << event;

  switch( event->button() )
    {
    case Qt::NoButton:
      break;

    case Qt::LeftButton:
      m_MousePressPosition = event->pos();
      m_MousePressOrigin = m_ViewSettings->GetOrigin();
      m_IsMouseDragging = true;
      break;

    case Qt::RightButton:
      emit ToggleLayerVisibilityRequested( false );
      break;

    case Qt::MidButton:
      break;

    case Qt::XButton1:
      break;

    case Qt::XButton2:
      break;

    default:
      assert( false && "Unhandled Qt::MouseButton." );
      break;
    }

  /*
    Qt::NoModifier	0x00000000	No modifier key is pressed.
    Qt::ShiftModifier	0x02000000	A Shift key on the keyboard is pressed.
    Qt::ControlModifier	0x04000000	A Ctrl key on the keyboard is pressed.
    Qt::AltModifier	0x08000000	An Alt key on the keyboard is pressed.
    Qt::MetaModifier	0x10000000	A Meta key on the keyboard is pressed.
    Qt::KeypadModifier	0x20000000	A keypad button is pressed.
    Qt::GroupSwitchModifier
  */
}

/******************************************************************************/
void
ImageViewManipulator
::MouseMoveEvent( QMouseEvent* event )
{
  assert( event!=NULL );

  /*
  qDebug() << "------------------------------------------------";

  qDebug() << this << ":" << event;
  */

  Qt::MouseButtons buttons = event->buttons();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  if( buttons==Qt::LeftButton &&
      ( modifiers==Qt::NoModifier || modifiers== Qt::ControlModifier ) )
    {
    // Cursor moves from press position to current position;
    // Image moves the same direction, so apply the negative translation.
    Translate( m_MousePressPosition - event->pos() );

    m_MousePressPosition = event->pos();

    emit RefreshViewRequested();

    emit RoiChanged(
      GetOrigin(),
      GetViewportSize(),
      GetSpacing(),
      m_ViewSettings->GetViewportCenter()
    );
    }
}

/******************************************************************************/
void
ImageViewManipulator
::MouseReleaseEvent( QMouseEvent* event)
{
  assert( event!=NULL );

  /*
  qDebug() << this << ":" << event;
  */

  /*
  Qt::MouseButtons buttons = event->buttons();
  Qt::KeyboardModifiers modifiers = event->modifiers();
  */
  // PointType center;

  switch( event->button() )
    {
    case Qt::NoButton:
      break;

    case Qt::LeftButton:
      m_MousePressPosition = QPoint();
      m_MousePressOrigin = PointType();
      m_IsMouseDragging = false;

      emit RefreshViewRequested();
      break;

    case Qt::RightButton:
      emit ToggleLayerVisibilityRequested( true );
      break;

    case Qt::MidButton:
      break;

    case Qt::XButton1:
      break;

    case Qt::XButton2:
      break;

    default:
      assert( false && "Unhandled Qt::MouseButton." );
      break;
    }
}

/******************************************************************************/
void
ImageViewManipulator
::ResizeEvent( QResizeEvent* event )
{
  assert( event!=NULL );

  // qDebug() << this << "::ResizeEvent(" << event << ")";

  /*
  qDebug() << m_ViewSettings.GetPointer();

  otb::ViewSettings::SizeType size( m_ViewSettings->GetViewportSize() );

  qDebug() << size[ 0 ] << "," << size[ 1 ] << "\t" << event->size();
  */

#if USE_VIEW_SETTINGS_SIDE_EFFECT 
#else // USE_VIEW_SETTINGS_SIDE_EFFECT
  SetViewportSize( event->size().width(), event->size().height() );
#endif // USE_VIEW_SETTINGS_SIDE_EFFECT
}

/******************************************************************************/
void
ImageViewManipulator
::WheelEvent( QWheelEvent* event )
{
  assert( event!=NULL );

  Qt::MouseButtons buttons = event->buttons();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  if( buttons!=Qt::NoButton )
    return;

  // Delta is rotation distance in number of 8th of degrees (see
  // http://qt-project.org/doc/qt-4.8/qwheelevent.html#delta).
  assert( event->delta()!=0 );
  int degrees = event->delta() / 8;

  if( modifiers==Qt::ControlModifier )
    {
    PointType point;

    Scale( event->pos(), degrees, &point );

    emit RefreshViewRequested();

    emit RoiChanged( GetOrigin(), GetViewportSize(), GetSpacing(), point );
    }
  //
  else if( modifiers==Qt::MetaModifier )
    {
    // qDebug() << "META+Wheel" << event->delta();

    emit ShiftAlphaRequested(
      static_cast< double >(
	m_AlphaGranularity * event->delta() / ( 8 * MOUSE_WHEEL_STEP_DEGREES )
      ) / 100.0
    );
    }
  else if( modifiers==(Qt::MetaModifier | Qt::ShiftModifier) )
    {
    // qDebug() << "META+SHIFT+Wheel" << event->delta();

    emit UpdateGammaRequested(
      ImageViewManipulator::Factor(
    	degrees,
    	MOUSE_WHEEL_STEP_DEGREES
      )
    );
    }
  //
  else if( modifiers==Qt::AltModifier )
    {
    // qDebug() << "ALT+Wheel" << event->delta();

    emit ResizeShaderRequested(
      ImageViewManipulator::Factor(
	degrees,
	MOUSE_WHEEL_STEP_DEGREES
      )
    );
    }
  else if( modifiers==(Qt::AltModifier | Qt::ShiftModifier) )
    {
    // qDebug() << "ALT+SHIFT+Wheel" << event->delta();

    emit ReparamShaderRequested(
      ImageViewManipulator::Factor(
	degrees,
	MOUSE_WHEEL_STEP_DEGREES
      )
    );
    }
  //
  else if( modifiers==(Qt::ControlModifier | Qt::AltModifier) )
    {
    // qDebug() << "CTRL+ALT+Wheel" << event->delta();

    emit ShiftDynamicsRequested(
      m_DynamicsShiftGranularity *
      static_cast< double >( event->delta() / ( 8 * MOUSE_WHEEL_STEP_DEGREES ) )
    );
    }
  else if( modifiers==(Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier) )
    {
    // qDebug() << "CTRL+ALT+SHIFT+Wheel" << event->delta();

    emit ScaleDynamicsRequested(
      ImageViewManipulator::Factor(
	degrees,
	MOUSE_WHEEL_STEP_DEGREES
      )
    );
    }
  //
  else if( modifiers==Qt::NoModifier )
    emit RotateLayersRequested(
      event->delta() / (8 * MOUSE_WHEEL_STEP_DEGREES)
    );
}

/******************************************************************************/
void
ImageViewManipulator
::KeyPressEvent( QKeyEvent* event )
{
  assert( event!=NULL );

  // qDebug() << this << "::KeyPressEvent(" << event << ")";

  QPoint vector( 0, 0 );
  int steps = 0;

  int key = event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  switch( key )
    {
    case Qt::Key_Up:
      vector.setY( -1 );
      break;

    case Qt::Key_Down:
      vector.setY( +1 );
      break;

    case Qt::Key_Left:
      vector.setX( -1 );
      break;

    case Qt::Key_Right:
      vector.setX( +1 );
      break;

    case Qt::Key_Plus:
      steps = m_ZoomGranularity;
      break;

    case Qt::Key_Minus:
      steps = -m_ZoomGranularity;
      break;

    case Qt::Key_Control:
      SetFastRenderMode( true );
      break;

    case Qt::Key_PageUp:
      emit RaiseLayerRequested();
      break;

    case Qt::Key_PageDown:
      emit LowerLayerRequested();
      break;

    case Qt::Key_Home:
      emit SelectPreviousLayerRequested();
      break;

    case Qt::Key_End:
      emit SelectNextLayerRequested();
      break;

    case Qt::Key_Delete:
      if( modifiers.testFlag( Qt::ShiftModifier ) )
        emit DeleteAllRequested();
      else
        emit DeleteSelectedRequested();
      break;

    case Qt::Key_1:
      emit ZoomToFullResolutionRequested();
      break;

    case Qt::Key_2:
      emit ZoomToLayerExtentRequested();
      break;

    case Qt::Key_3:
      emit ZoomToFullExtentRequested();
      break;

    case Qt::Key_A:
      emit ApplyAllRequested();
      break;

    case Qt::Key_C:
      emit ShaderEffectRequested( EFFECT_CHESSBOARD );
      break;

    case Qt::Key_G:
      emit ShaderEffectRequested( EFFECT_GRADIENT );
      break;

    case Qt::Key_D:
      emit ShaderEffectRequested( EFFECT_LOCAL_CONTRAST );
      break;

    case Qt::Key_H:
      emit ShaderEffectRequested( EFFECT_SWIPE_H );
      break;

    case Qt::Key_N:
      emit ShaderEffectRequested( EFFECT_NORMAL );
      break;

    case Qt::Key_P:
      emit SetReferenceRequested();

    case Qt::Key_Q:
      emit ResetQuantilesRequested( modifiers.testFlag( Qt::ShiftModifier ) );
      break;

    case Qt::Key_S:
      emit ShaderEffectRequested( EFFECT_SPECTRAL_ANGLE );
      break;

    case Qt::Key_T:
      emit ShaderEffectRequested( EFFECT_LOCAL_TRANSLUCENCY );
      break;

    case Qt::Key_V:
      emit ShaderEffectRequested( EFFECT_SWIPE_V );
      break;

    default:
      break;
    }

  assert( !m_ViewSettings.IsNull() );

  bool needsRefresh = false;

  //
  // Translate

  if( !vector.isNull() )
    {
    otb::ViewSettings::SizeType size( m_ViewSettings->GetViewportSize() );

    if( modifiers==Qt::NoModifier )
      {
      size[ 0 ] /= m_ScrollGranularity;
      size[ 1 ] /= m_ScrollGranularity;
      }
    else if( modifiers==Qt::ControlModifier )
      {
      size[ 0 ] /= m_ScrollGranularity * 2;
      size[ 1 ] /= m_ScrollGranularity * 2;
      }

    vector.rx() *= size[ 0 ];
    vector.ry() *= size[ 1 ];

    Translate( vector );

    needsRefresh = true;
    }

  //
  // Scale

  if( steps!=0 )
    {
    // Qt::ControlModifier doest not work with keypard Qt::Key_Plus/Minus keys.

    otb::ViewSettings::SizeType size( m_ViewSettings->GetViewportSize() );

    Scale(
      QPoint( size[ 0 ] / 2.0, size[ 1 ] / 2.0 ),
      steps * MOUSE_WHEEL_STEP_DEGREES
    );

    needsRefresh = true;
    }

  //
  // Refresh
  if( needsRefresh )
    {
    emit RefreshViewRequested();

    emit RoiChanged(
      GetOrigin(),
      GetViewportSize(),
      GetSpacing(),
      m_ViewSettings->GetViewportCenter()
    );
    }
}

/******************************************************************************/
void
ImageViewManipulator
::KeyReleaseEvent( QKeyEvent* event )
{
  assert( event!=NULL );

  // qDebug() << this << "::KeyPressEvent(" << event << ")";

  switch( event->key() )
    {
    case Qt::Key_Control:
      SetFastRenderMode( false );
      emit RefreshViewRequested();
      break;

    default:
      break;
    }
}

/******************************************************************************/
void
ImageViewManipulator
::Translate( const QPoint& vector )
{
  // qDebug() << this << "::Translate(" << vector << ")";

  m_ViewSettings->SetOrigin(
    ImageViewManipulator::Translate(
      vector,
      m_ViewSettings->GetOrigin(),
      m_ViewSettings->GetSpacing()
    )
  );
}

/******************************************************************************/
PointType
ImageViewManipulator
::Translate( const QPoint& vector,
             const PointType& origin,
             const SpacingType& spacing )
{
  // qDebug() << this << "::Translate(...)";

  otb::ViewSettings::PointType origin2( origin );

  origin2[ 0 ] += static_cast< double >( vector.x() ) * spacing[ 0 ];
  origin2[ 1 ] += static_cast< double >( vector.y() ) * spacing[ 1 ];

  /*
    qDebug()
    << "(" << m_MousePressOrigin[ 0 ] << "," << m_MousePressOrigin[ 1 ] << ")"
    << "(" << spacing[ 0 ] << "," << spacing[ 1 ] << ")"
    << "(" << origin[ 0 ] << "," << origin[ 1 ] << ")";
  */

  return origin2;
}

/******************************************************************************/
void
ImageViewManipulator
::Scale( const QPoint& center, int degrees, PointType* centerPoint )
{
  assert( degrees!=0 );

  if( degrees==0 )
    return;

  otb::ViewSettings::PointType point;

  Transform( point, center );

  if( centerPoint!=NULL )
    *centerPoint = point;

  // See http://qt-project.org/doc/qt-4.8/qwheelevent.html#delta .
  assert( m_ZoomGranularity!=0 );

  int granularity = m_ZoomGranularity;

  if( granularity==0 )
    granularity = 1;

  double factor = pow(
    2.0,
    -static_cast< double >( degrees ) /
    static_cast< double >( granularity * MOUSE_WHEEL_STEP_DEGREES )
  );

  m_ZoomFactor *= factor;

  /*
  qDebug()
    << "(" << point[ 0 ] << "," << point[ 1 ] << ")"
    << "g:" << granularity
    << "d:" << degrees
    << "s:" << (static_cast< double >( degrees ) / 15.0)
    << "f:" << factor
    << "z:" << m_ZoomFactor;
  */

  m_ViewSettings->Zoom( point, factor );
}

/*****************************************************************************/
/* SLOTS                                                                     */
/*****************************************************************************/

/*****************************************************************************/
} // end namespace 'mvd'
