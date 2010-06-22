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
#include "itkConstNeighborhoodIterator.h"
#include "itkRescaleIntensityImageFilter.h"

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
  typedef unsigned char       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;
  typedef unsigned int        SegmentPixelType;
  typedef itk::Image< SegmentPixelType, Dimension>   SegmentImageType;
  typedef float               OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension> OutputImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;
  // seed writer
  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;


  //Seed writing
  typedef itk::ValuedRegionalMaximaImageFilter< InputImageType,OutputImageType >
    FilterType;
  typedef itk::RescaleIntensityImageFilter<  OutputImageType, OutputImageType >
    RescaleFilterType;
  typedef itk::ImageRegionIterator<OutputImageType>  IteratorType;


  // neighborhood iterators
  typedef itk::ConstNeighborhoodIterator<OutputImageType> NeighborhoodIteratorType;



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

  //filling seeds map


  std::cout << "rescaling maximas" << std::endl;
  RescaleFilterType::Pointer rescaleMax = RescaleFilterType::New();
  rescaleMax->SetInput( maxFilter->GetOutput() );
  rescaleMax->Update();
  rescaleMax->SetOutputMaximum( 1.0 );
  rescaleMax->SetOutputMinimum( rescaleMax->GetInputMinimum() );
  rescaleMax->Update();



  std::cout << "filtering out the spurious seeds" << std::endl;
  // iterator over the local Maximas output
  OutputImageType::Pointer LocalMaxImage = rescaleMax->GetOutput();

  IteratorType it(LocalMaxImage ,
    LocalMaxImage->GetLargestPossibleRegion());
  it.GoToBegin();


  itk::Neighborhood<OutputPixelType, Dimension> nhood;
  NeighborhoodIteratorType::RadiusType radius;

  NeighborhoodIteratorType NIt(radius, LocalMaxImage, LocalMaxImage->GetRequestedRegion());
  NeighborhoodIteratorType::IndexType loc;

  itk::Offset<3> off_set;

  OutputImageType::SpacingType m_Spacing;

  // define the radius of te neighborhood :
  m_Spacing = LocalMaxImage->GetSpacing();

  // Set radious to min scale
  radius[0] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[0]) ; // radius in x
  radius[1] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[1]) ; // y
  radius[2] = static_cast<unsigned int>( m_SigmaMin/m_Spacing[2]) ; // z

  int current_max;
  std::vector<OutputPixelType> neighbor(Dimension);
  OutputImageType::SizeType imagesize = LocalMaxImage->GetLargestPossibleRegion().GetSize();
  OutputImageType::IndexType pixelIndex;

  // filtering out the spurious seeds (retains biggest seed of a region)
  while( !it.IsAtEnd() )
    {

    current_max = 0;

    // if the current pixel is a maximum, then :
    if ( it.Value() > 0 )
      {
      current_max = it.Value();

      // Do this
      //Set co ordinates
      loc = it.GetIndex();
      NIt.SetLocation(loc);
      //nhood = NIt.GetNeighborhood();

      for (int i = 0; i<nhood.Size(); ++i)
        {
        off_set = nhood.GetOffset(i);
        neighbor[0] = loc[0] + off_set[0];
        neighbor[1] = loc[1] + off_set[1];
        neighbor[2] = loc[2] + off_set[2];

        //Check Boundary Conditions

        if(neighbor[0]<1) neighbor[0] =0;
        if(neighbor[1]<1) neighbor[1] =0;
        if(neighbor[2]<1) neighbor[2] =0;

        if(neighbor[0]>=imagesize[0]) neighbor[0] =imagesize[0]-1;
        if(neighbor[1]>=imagesize[1]) neighbor[1] =imagesize[1]-1;
        if(neighbor[2]>=imagesize[2]) neighbor[2] =imagesize[2]-1;

        pixelIndex[0] = neighbor[0];
        pixelIndex[1] = neighbor[1];
        pixelIndex[2] = neighbor[2];


        if (LocalMaxImage->GetPixel(pixelIndex) > 0)
          {
          if(LocalMaxImage->GetPixel(pixelIndex) > current_max )
            {
            current_max = LocalMaxImage->GetPixel(pixelIndex);
            it.Value()=0;
            }
          else
            {
            LocalMaxImage->SetPixel(pixelIndex,0);
            }
          }
        }
      }
    }

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


  /*
  // filling a vector with seeds
  std::cout << "filling seeds map" << std::endl;
  typedef OutputImageType::IndexType InputImageIndexType;
  InputImageIndexType idx;
  // iterator over the local Maximas output
  it.GoToBegin();

  // vector of pair confidence-coordinate for each seed
  typedef std::pair< float, InputImageIndexType > seed;
  std::vector< seed > seeds;
  std::vector< seed >::iterator loc;

  while( !it.IsAtEnd() )
    {
    // if the current pixel is a maximum, then :
    if ( it.Value() > 0.1 )
      seeds.push_back(std::pair< float, InputImageIndexType > ( it.Value(),it.GetIndex() ) );
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
    }
  outfile.close();
*/

  return EXIT_SUCCESS;
}

