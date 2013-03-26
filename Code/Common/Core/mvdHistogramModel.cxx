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
#include "mvdHistogramModel.h"


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
#include "mvdVectorImageModel.h"

namespace mvd
{
/*
  TRANSLATOR mvd::HistogramModel

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*******************************************************************************/
HistogramModel
::HistogramModel( QObject* parent ) :
  AbstractModel( parent ),
  m_Histograms(),
  m_MinPixel(),
  m_MaxPixel()
{
}

/*******************************************************************************/
HistogramModel
::~HistogramModel()
{
}

/*******************************************************************************/
double
HistogramModel
::Percentile( CountType band, MeasurementType intensity, Bound bound ) const
{
  // Get histogram of band.
  Histogram::Pointer histogram( m_Histograms->GetNthElement( band ) );

  // Contruct 1D measurement vector.
  Histogram::MeasurementVectorType measurement( 1 );
  measurement[ 0 ] = intensity;
  
  // Due to float/double conversion, it can happen
  // that the minimum or maximum value go slightly outside the histogram
  // Clamping the value solves the issue and avoid RangeError
 // itk::NumericsTraits<>::Clamp(...) was removed 
  // TODO : when otb::Clamp will be developped, use this function
  measurement[0]  = 
    measurement[0]<histogram->GetBinMin(0, 0)?
    histogram->GetBinMin(0, 0):(measurement[0]>histogram->GetBinMax(0, histogram->GetSize(0) - 1)?
                                histogram->GetBinMax(0, histogram->GetSize(0) - 1):measurement[0]);

    
  // Get the index of measurement in 1D-histogram.
  Histogram::IndexType index;
  if( !histogram->GetIndex( measurement, index ) )
    throw itk::RangeError( __FILE__, __LINE__ );

  assert( Histogram::IndexType::GetIndexDimension()==1 );

  // Min/max intensities of bin.
  MeasurementType minI = histogram->GetBinMin( 0, index[ 0 ] );
  MeasurementType maxI = histogram->GetBinMax( 0, index[ 0 ] );

  // Frequency of current bin
  Histogram::FrequencyType frequency( histogram->GetFrequency( index ) );

  // Initialize result (contribution of current bin)
  const MeasurementType epsilon = 1.0E-6;
  double percent = 0.;
  
  if ( vcl_abs(maxI - minI) > epsilon )
    {
    percent = frequency 
      * (bound == BOUND_LOWER ? (intensity - minI) : (maxI - intensity) ) 
      / ( maxI - minI );
    }
  
  // Number of bins of histogram.
  Histogram::SizeType::SizeValueType binCount = histogram->Size();

  // Initialize bound indices.
  assert( index[ 0 ]>=0 );
  Histogram::SizeType::SizeValueType index0 = index[ 0 ];
  Histogram::SizeType::SizeValueType i0 = 0;
  Histogram::SizeType::SizeValueType iN = binCount;

  switch( bound )
    {
    case BOUND_LOWER:
      i0 = 0;
      iN = index[ 0 ];
      break;

    case BOUND_UPPER:
      i0 = index0 < binCount ? index0 + 1 : binCount;
      iN = binCount;
      break;

    default:
      assert( false && "Implement case statement of switch instruction." );
      break;
    };

  // Traverse lower/upper bound (contribution of other bins)
  Histogram::SizeType::SizeValueType i;

  for( i=i0; i<iN; ++i )
    percent += histogram->GetFrequency( i, 0 );
  
  // Calculate frequency rate.
  percent /= histogram->GetTotalFrequency();
  
  // Return frequency rate.
  return percent;
}

/*******************************************************************************/
void
HistogramModel
::virtual_BuildModel( void* context )
{
  // template_BuildModel_I< VectorImageModel::SourceImageType >();
  template_BuildModel_M< VectorImageModel >();

  // template_BuildModel< otb::Image< FixedArray< double, 4 >, 2 > >();
  // template_BuildModel< otb::Image< itk::FixedArray< float, 4 >, 2 > >();

  /*
  template_BuildModel< otb::VectorImage< float, 2 > >();
  template_BuildModel< otb::Image< float, 2 > >();
  */
}

/*******************************************************************************/
/* SLOTS                                                                       */
/*******************************************************************************/

/*******************************************************************************/

} // end namespace 'mvd'