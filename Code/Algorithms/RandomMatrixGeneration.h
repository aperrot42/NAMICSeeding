#ifndef RandomMatrixGeneration_h_
#define RandomMatrixGeneration_h_

//#include <vcl_vector.h>
//#include <vnl/vnl_matrix.h>

//#include <itkMatrix.h>

#include "itkPoint.h"
#include "itkVector.h"
#include "itkImage.h"

#include "itkObject.h"
#include "itkImageRandomIteratorWithIndex.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "itkImageFileWriter.h"

#include "itkPointSet.h"

#include <iostream>
#include <ostream>


//Template on the dimension + pixel type instead?
template < typename TImage >
class RandomMatrixGeneration
{

  typedef TImage                                                    ImageType;
  typedef typename ImageType::PixelType                             PixelType;
  typedef typename ImageType::IndexType                             IndexType;
  typedef itk::PointSet< PixelType, ImageType::ImageDimension>      PointSetType;
  typedef itk::ImageRandomIteratorWithIndex< ImageType >            IteratorType;
  typedef itk::RecursiveGaussianImageFilter< ImageType,ImageType >  RecursiveGaussianFilterType;
  typedef itk::ImageFileWriter<  ImageType  > WriterType;


  public:
    RandomMatrixGeneration();
    ~RandomMatrixGeneration();

    // check usage and utility
    //typedef typename TPoint::CoordRepType                         RepType;

    // Method to define the size of the image
    void SetImageSize( int iSize);

    // Method to define the number of points to be generated randomly
    void SetNumberOfPoints( int iNumberOfSeeds);

    // Method to define the norm of the random vector
    void SetRamdomVectorsMaxNorm( int iMaxNorm);

    // Method to create the "random evolution vector"
    void CreateEvolutionVectors();

    // Method to define randomly the intensity of each point

    // Method to create the random points
    void CreateInputImage();

    // Method to apply a Gaussian filter on the image
    void ApplyGaussianFilter();

    // Export output image as tiff
    void ExportAsTiff();

    // Apply vector to point
    void ApplyVectorToPoints();


  private:
    // contains the points before evolution
    typename ImageType::Pointer                m_OriginalsPointsContainer;

    // contains the points before evolution
    typename ImageType::Pointer                m_TransformedPointsContainer;

    std::vector< IndexType >                   m_SeedsContainer;

    typename PointSetType::Pointer             m_PointSet;

    // contains the "random evolution vectors"
    std::vector< std::vector< int > >  m_RandomEvolutionVectors;

    int m_Dimension;

    // Size of the image
    int m_ImageSize;

    // Number of points to be created
    int m_NumberOfSeeds;

    // Norm of the random vector
    int m_RandomVectorMaxNorm;

  protected:
    void PrintSelf(std::ostream& os, itk::Indent indent) const;

};

#include "RandomMatrixGeneration.txx"

#endif
