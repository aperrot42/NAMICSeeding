#include "RandomMatrixGeneration.h"

#include <itkRecursiveGaussianImageFilter.h>

#include <iostream>
#include <ostream>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
template<class T>
RandomMatrixGeneration<T>::
RandomMatrixGeneration( ):
    m_ImageSize( 0 ), m_NumberOfSeeds( 0 ), m_RandomVectorMaxNorm( 0 )
{
  // Get the dimesion of the input image
  m_Dimension = T::ImageDimension;
  m_OriginalsPointsContainer = ImageType::New();
  m_TransformedPointsContainer = ImageType::New();
  m_PointSet = PointSetType::New();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
template<class T>
RandomMatrixGeneration<T>::
~RandomMatrixGeneration()
{

}

//-----------------------------------------------------------------------------
// Method to define the size of the image
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
SetImageSize( int iSize)
{
  m_ImageSize = iSize;
}

//-----------------------------------------------------------------------------
// Method to define the number of points to be generated randomly
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
SetNumberOfPoints( int iNumberOfSeeds)
{
  m_NumberOfSeeds = iNumberOfSeeds;
}

//-----------------------------------------------------------------------------
// Method to define the norm of the random vector
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
SetRamdomVectorsMaxNorm( int iMaxNorm)
{
  m_RandomVectorMaxNorm = iMaxNorm;
}

//-----------------------------------------------------------------------------
// Method to create the "random evolution vector"
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
CreateEvolutionVectors()
{
  // single random vector use to fill the list of all random vectors
  std::vector< int > randomVector;
  std::vector< int >::const_iterator it;

  // for each point, get norm in each dimension
  // then fill the vector container
  for (int i = 0; i < m_NumberOfSeeds; ++i)
    {
    for ( int j = 0; j < m_Dimension; ++j)
      {
      // m_RandomVectorMaxNorm +1 to go from 0 to m_RandomVectorMaxNorm
      randomVector.push_back( ( rand()%(m_RandomVectorMaxNorm +1) ) );
      }
    std::cout << "random vector"<<std::endl;
    for( it = randomVector.begin();
         it != randomVector.end();
       ++it)
      {
      std::cout << *it << std::endl;
      }

    m_RandomEvolutionVectors.push_back( randomVector );
    randomVector.clear();
    }
}

//-----------------------------------------------------------------------------
// Method to create the "input image" : 0 everywhere but m_NumberOfSeeds points
// randomly
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
CreateInputImage()
{
  // The image region and start should be initialized
  typename ImageType::IndexType start;
  typename ImageType::SizeType  size;

  for ( int j = 0; j < m_Dimension; ++j)
    {
    size[j]  = m_ImageSize;
    start[j] = 0;
    }

  typename ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  // Pixel data is allocated
  m_OriginalsPointsContainer->SetRegions( region );
  m_OriginalsPointsContainer->Allocate();

  // Initialize value in the image to 0
  PixelType  initialValue;
  initialValue.Fill( 0.0 );

  m_OriginalsPointsContainer->FillBuffer( initialValue );

  // Create the random iterator
  IteratorType randomIterator(m_OriginalsPointsContainer, region);

  // Go to the beginning of the region
  randomIterator.GoToBegin();
  // Increment iterator to avoid starting at the beginning of the region
  ++randomIterator;

  ///////////////////////////////////////////////////////////////////////////////
  //TESTING for point + vector
  ///////////////////////////////////////////////////////////////////////////////
  int pointId = 0;
  typename PointSetType::PixelType   tangent;
  typename PointSetType::PointType   point;

  // Create input image and update the seeds container
  for ( int j = 0; j < m_NumberOfSeeds; ++j)
    {
    // Generate Random point
    ++randomIterator;
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    tangent[0] = 255;
    tangent[1] = 255;
    tangent[2] = 255; // flat on the Z plane
    point[0] = randomIterator.GetIndex()[0];
    point[1] = randomIterator.GetIndex()[1];
    point[2] = randomIterator.GetIndex()[2];
    m_PointSet->SetPoint( pointId, point );
    m_PointSet->SetPointData( pointId, tangent );
    pointId++;
////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    //Update image content
    // -rand to change randomly intensity of the point
    // check how to do on each channel
    randomIterator.Set( 255 - ( rand()%50));
    // Store position in vector
    m_SeedsContainer.push_back( randomIterator.GetIndex() );
    }
}

//-----------------------------------------------------------------------------
// Apply Gaussian Filter to create "cells like" points
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
ApplyGaussianFilter()
{
  typename RecursiveGaussianFilterType::Pointer m_recursiveGaussianFilter
      = RecursiveGaussianFilterType::New();
  m_recursiveGaussianFilter->SetSigma( 1 );
  m_recursiveGaussianFilter->SetDirection(0);
  m_recursiveGaussianFilter->SetNormalizeAcrossScale(true);
  m_recursiveGaussianFilter->SetOrder(RecursiveGaussianFilterType::ZeroOrder);
  m_recursiveGaussianFilter->SetInput(m_OriginalsPointsContainer);
  m_recursiveGaussianFilter->Update();

  m_TransformedPointsContainer = m_recursiveGaussianFilter->GetOutput();
}

//-----------------------------------------------------------------------------
// Apply vector to point
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
ApplyVectorToPoints()
{
  typename std::vector< IndexType >::const_iterator it;
  std::vector< std::vector< int > >::const_iterator it2;
  it2 = m_RandomEvolutionVectors.begin();
/*
  typename PointSetType::Pointer pointSet = PointSetType::New();

  typename PointSetType::PixelType   pixel;
  typename PointSetType::PointType   point;
  unsigned int pointId = 0;
  const double radius = 3.0;

*/


  /*
  for( it = m_SeedsContainer.begin();
       it != m_SeedsContainer.end();
     ++it, ++it2)
    {
    std::cout << "before point"<<std::endl;
    std::cout << *it << std::endl;
    it = it +it2.GetPixel();
    std::cout << "after point"<<std::endl;
    std::cout << *it << std::endl;
    }*//*

	typedef typename PointSetType::PointDataContainer::ConstIterator PointDataIterator;
	PointDataIterator pixelIterator = m_PointSet->GetPointData()->Begin();
	PointDataIterator pixelEnd      = m_PointSet->GetPointData()->End();

	typedef typename PointSetType::PointsContainer::Iterator      PointIterator;
	PointIterator pointIterator = m_PointSet->GetPoints()->Begin();
	PointIterator pointEnd      = m_PointSet->GetPoints()->End();

	while( pixelIterator != pixelEnd && pointIterator != pointEnd )
	  {
	  pointIterator.Value() = pointIterator.Value() + pixelIterator.Value();
	  ++pixelIterator;
	  ++pointIterator;
	  }*/

}

//-----------------------------------------------------------------------------
// Export as tiff
//-----------------------------------------------------------------------------
template<class T>
void
RandomMatrixGeneration<T>::
ExportAsTiff()
{

  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "/home/nr52/Desktop/output.mhd" );
  writer->SetInput( m_TransformedPointsContainer );
  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    }
}
//-----------------------------------------------------------------------------
// Printself method
// TO FIX
//-----------------------------------------------------------------------------
/* Printself method
 *
 *     // print out each random vector
    std::cout << "random vector " << i << std::endl;
    for(std::vector<int>::const_iterator it = randomVector.begin();
                                         it != randomVector.end();
                                       ++it)
      {
      std::cout << *it << std::endl;
      }
 */
template<class T>
void
RandomMatrixGeneration<T>::
PrintSelf(std::ostream& os, itk::Indent indent) const
{
  os << indent << "Debug: ";
 }
