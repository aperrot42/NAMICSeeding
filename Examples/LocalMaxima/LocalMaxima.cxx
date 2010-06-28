/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"
#include "itkMultiScaleLoGDistanceImageFilter.h"
#include "itkValuedRegionalMaximaImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkThresholdImageFilter.h"
#include "itkCastImageFilter.h"

int main(int argc, char* argv [] )
{

  // Define the dimension of the images
  const int Dimension = 3;

  // Declare the types of the images
  typedef float       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;

  typedef float               OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension> OutputImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;
  // seed writer
  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
  typedef itk::ThresholdImageFilter<InputImageType> ThresholdFilterType;

  //Seed writing
  typedef itk::ValuedRegionalMaximaImageFilter< InputImageType,InputImageType >
    FilterType;
  typedef itk::RescaleIntensityImageFilter<  InputImageType, InputImageType >
    RescaleFilterType;
  typedef itk::CastImageFilter<  InputImageType, OutputImageType >
    CastFilterType;

  typedef itk::ImageRegionIterator<OutputImageType>  IteratorType;


  // neighborhood iterators
  typedef itk::ConstantBoundaryCondition< OutputImageType >
    ConstantBoundaryType;
  typedef itk::NeighborhoodIterator< OutputImageType, ConstantBoundaryType >
    NeighborhoodIteratorType;

  if ( argc != 6 )
    {
    std::cerr << "Missing Parameters: "
              << argv[0] << std::endl
              << "inputImage outputImage outputTextFile"
              << " [SmallestCellRadius FilterThresholdValue]" << std::endl;
    return EXIT_FAILURE;
    }

  // smallest cell radius
  float m_SigmaMin = (float)atof(argv[4]);
  InputPixelType m_ThresholdValue = (InputPixelType)atof(argv[5]);


  std::cout << "reading input image" << std::endl;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();


  //EXTRACTING SEEDS (local maximas)

  //threshold output of the filter for keeping only representative max
  std::cout << "thresholding maximas" << std::endl;
  ThresholdFilterType::Pointer threshFilter = ThresholdFilterType::New();
  threshFilter->SetInput( reader->GetOutput() ) ;
  threshFilter->SetOutsideValue	( static_cast<InputPixelType>(0) );
  threshFilter->ThresholdBelow( static_cast<InputPixelType>(m_ThresholdValue) );
  threshFilter->Update();


  std::cout << "extracting maximas" << std::endl;
  FilterType::Pointer maxFilter = FilterType::New();
  maxFilter->SetInput( threshFilter->GetOutput() );
  maxFilter->SetFullyConnected( true );
  maxFilter->Update();

  //rescale seeds "confidence" in 0-255 (Uchar)
  std::cout << "rescaling maximas" << std::endl;
  RescaleFilterType::Pointer rescaleMax = RescaleFilterType::New();
  rescaleMax->SetInput( maxFilter->GetOutput() );
  rescaleMax->SetOutputMaximum( 255 );
  rescaleMax->SetOutputMinimum( 0 );
  rescaleMax->Update();

  std::cout << "casting to output type" << std::endl;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput( rescaleMax->GetOutput() );
  castFilter->Update();


  OutputImageType::Pointer LocalMaxImage = castFilter->GetOutput();

  LocalMaxImage->SetRequestedRegion(LocalMaxImage->GetLargestPossibleRegion() );

  // FILTERING SEEDS
  std::cout << "filtering out the spurious seeds" << std::endl;
  // iterator over the local Maximas output
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
  bool m_pset = false;
  unsigned int prevMax;
  long seedCount = 0;
  long analysedSeeds = 0;
  unsigned int indN;
  for (Nit.GoToBegin(); !Nit.IsAtEnd(); ++Nit)
    {
    // if we are centered on a local maxima
    if ( ( Nit.GetCenterPixel() ) > static_cast<OutputPixelType>(0) )
      {
      curentMax = Nit.GetPixel( 0 );
      prevMax = 0;
      ++analysedSeeds;
      for (indN = 1;indN < Nit.Size(); ++indN)
        {
        if (Nit.GetPixel(indN) > curentMax)
          {
          // we use the quiet verion of setpixel
          // (we can try to write in the padding)
          Nit.SetPixel(prevMax, static_cast<OutputPixelType>( 0 ), m_pset);
          curentMax = Nit.GetPixel(indN);
          // we save the index of the curent max in the neighborhood
          // in case we have to zero it (there is a bigger maxima)
          prevMax = indN;
          // if we did not write in the padding
          if (m_pset)
            {
            ++seedCount;
            std::cout << '\r' << "Seeds which are too close :" << seedCount << std::flush;
            }
          }
        else
          {
          if (Nit.GetPixel(indN) > static_cast<OutputPixelType>( 0 ) )
            {
            ++seedCount;
            std::cout << '\r' << "Seeds which are too close :" << seedCount << std::flush;
            }

          // if this pixel is not bigger than the previous maxima
          Nit.SetPixel(indN, static_cast<OutputPixelType>( 0 ), m_pset);
          }
        }
      }
    //else
    //  {
    //  Nit.SetCenterPixel( 0 );
    //  }
    }
  std::cout << std::endl;
  std::cout << "Explored Seeds :" << analysedSeeds << std::endl;


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
  IteratorType it( LocalMaxImage , LocalMaxImage->GetLargestPossibleRegion() );
  it.GoToBegin();

  // vector of pair confidence-coordinate for each seed
  typedef std::pair< float, OutputImageIndexType > seed;
  std::vector< seed > seeds;
  std::vector< seed >::iterator loc;

  while( !it.IsAtEnd() )
    {
    // if the current pixel is a maximum, then :
    if ( it.Value() > static_cast<OutputPixelType>( 0 ) )
      seeds.push_back(std::pair< float, OutputImageIndexType > ( it.Value()/255.,it.GetIndex() ) );
    ++it;
    }


  // writing a seed text file file
  std::cout << "Write out the seed file" << std::endl;
  std::fstream outfile;
  outfile.open ( argv[3],std::ios::out );
  loc = seeds.end();

  seedCount = 0;
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
    ++seedCount;
    std::cout << '\r' << "Written Seeds :" << seedCount << std::flush;
    }
  std::cout << std::endl;
  outfile.close();


  return EXIT_SUCCESS;
}

