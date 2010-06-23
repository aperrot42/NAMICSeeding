/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiScaleHessianSmoothed3DToMembranenessMeasureImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/04/01 21:19:46 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"
#include "itkMultiScaleLoGDistanceImageFilter.h"
#include "itkValuedRegionalMaximaImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkThresholdImageFilter.h"

int main(int argc, char* argv [] )
{
  if ( argc != 5 )
    {
    std::cerr << "Missing Parameters: "
              << argv[0] << std::endl
              << "inputImage outputImage outputTextFile"
              << " [SmallestCellRadius]" << std::endl;
    return EXIT_FAILURE;
    }

  // smallest cell radius
  float m_SigmaMin = (float)atof(argv[4]);

  // Define the dimension of the images
  const int Dimension = 3;

  // Declare the types of the images
  typedef float       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;

  typedef unsigned char               OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension> OutputImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;
  // seed writer
  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
  typedef itk::ThresholdImageFilter<InputImageType> ThresholdFilterType;

  //Seed writing
  typedef itk::ValuedRegionalMaximaImageFilter< InputImageType,InputImageType >
    FilterType;
  typedef itk::RescaleIntensityImageFilter<  InputImageType, OutputImageType >
    RescaleFilterType;
  typedef itk::ImageRegionIterator<OutputImageType>  IteratorType;


  // neighborhood iterators
  typedef itk::ConstantBoundaryCondition< OutputImageType >
    ConstantBoundaryType;
  typedef itk::NeighborhoodIterator< OutputImageType, ConstantBoundaryType >
    NeighborhoodIteratorType;



  std::cout << "reading input image" << std::endl;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();


  //EXTRACTING SEEDS (local maximas)

  std::cout << "extracting maximas" << std::endl;
  FilterType::Pointer maxFilter = FilterType::New();
  maxFilter->SetInput( reader->GetOutput() );
  maxFilter->SetFullyConnected( true );
  maxFilter->Update();

  //threshold output of the filter for keeping only representative max
  std::cout << "thresholding maximas" << std::endl;
  ThresholdFilterType::Pointer threshFilter = ThresholdFilterType::New();
  threshFilter->SetInput( maxFilter->GetOutput() ) ;
  threshFilter->SetOutsideValue	( 0 );
  threshFilter->ThresholdBelow( static_cast<InputPixelType>(0.01) );
  threshFilter->Update();

  //rescale seeds "confidence" in 0-255 (Uchar)
  std::cout << "rescaling maximas" << std::endl;
  RescaleFilterType::Pointer rescaleMax = RescaleFilterType::New();
  rescaleMax->SetInput( threshFilter->GetOutput() );
  rescaleMax->SetOutputMaximum( 255 );
  rescaleMax->SetOutputMinimum( 0 );
  rescaleMax->Update();


  // FILTERING SEEDS

  std::cout << "filtering out the spurious seeds" << std::endl;
  // iterator over the local Maximas output
  OutputImageType::Pointer LocalMaxImage = rescaleMax->GetOutput();

  // definition of the neighborhood
  NeighborhoodIteratorType::RadiusType radius;
  // Set radious to min scale
  OutputImageType::SpacingType m_Spacing;
  m_Spacing = LocalMaxImage->GetSpacing();
  radius[0] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[0]) ; // radius in x
  radius[1] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[1]) ; // y
  radius[2] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[2]) ;
  std::cout << radius[0] << " "
            << radius[1] << " "
            << radius[2] << std::endl;
  NeighborhoodIteratorType Nit( radius, LocalMaxImage,
    LocalMaxImage->GetRequestedRegion() );


  OutputPixelType curentMax;
  bool m_pset;
  unsigned int prevMax;
  for (Nit.GoToBegin(); !Nit.IsAtEnd(); ++Nit)
    {
    // if we are centered on a local maxima
    if ( static_cast<OutputPixelType>((Nit.GetCenterPixel())) > static_cast<OutputPixelType>(0))
      {
      std::cout << "f0";
      curentMax = Nit.GetPixel(0);
      prevMax = 0;
      for (unsigned int indN = 1;indN < radius[0]*radius[1]*radius[2]; ++indN)
        {
        if (Nit.GetPixel(indN) > curentMax)
          {
          // we use the uiet verion of setpixel
          // (we can try to write in the padding)
          Nit.SetPixel(prevMax, 0, m_pset);
          curentMax = Nit.GetPixel(indN);
          // we save the index of the curent max in the neighborhood
          // in case we have to zero it (there is a bigger maxima)
          prevMax = indN;
          }
        else
          {
          // if this pixel is not bigger than the previous maxima
          Nit.SetPixel(indN, 0, m_pset);
          }
        }
      }
    else
      {
      Nit.SetCenterPixel( 0 );
      }
    }
  std::cout << std::endl;

  // write out the seeding image
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput ( LocalMaxImage );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }



  // filling a vector with seeds
  std::cout << "filling seeds map" << std::endl;
  typedef OutputImageType::IndexType OutputImageIndexType;
  OutputImageIndexType idx;
  // iterator over the local Maximas output
  IteratorType it(LocalMaxImage ,
  LocalMaxImage->GetLargestPossibleRegion());
  it.GoToBegin();
  it.GoToBegin();

  // vector of pair confidence-coordinate for each seed
  typedef std::pair< float, OutputImageIndexType > seed;
  std::vector< seed > seeds;
  std::vector< seed >::iterator loc;

  while( !it.IsAtEnd() )
    {
    // if the current pixel is a maximum, then :
    if ( it.Value() > 0.1 )
      seeds.push_back(std::pair< float, OutputImageIndexType > ( it.Value()/255,it.GetIndex() ) );
    }


  // writing a seed text file file
  std::cout << "Write out the seed file" << std::endl;
  std::fstream outfile;
  outfile.open ( argv[3],std::ios::out );
  loc = seeds.end();
  while ( loc != seeds.begin() )
    {
    --loc;
    float val = loc->first;
    idx = loc->second;
    outfile << val;
    for ( int i = 0; i < Dimension; i++ )
      {
      outfile << ' ' << idx[i];
      }
    outfile << std::endl;
    std::cout<<"w1";
    }
  outfile.close();


  return EXIT_SUCCESS;
}

