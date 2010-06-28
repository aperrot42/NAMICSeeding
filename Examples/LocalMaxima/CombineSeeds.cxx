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
#include "itkRescaleIntensityImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkNumericTraits.h"
#include "itkMultiScaleLoGDistanceImageFilter.h"
#include "itkValuedRegionalMaximaImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkRGBPixel.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"

#include "itkLabelOverlayImageFilter.h"
//#include <sstream>


int main(int argc, char* argv [] )
{
  if ( argc < 5 )
    {
    std::cerr << "Missing Parameters: "
              << argv[0] << std::endl
              << "inputImage inputSeeds1 inputSeeds2 outputSeedsImage"
              << std::endl;
    return EXIT_FAILURE;
    }

  // Define the dimension of the images
  const unsigned int Dimension = 3;

  // Declare the types of the images
  //input
  typedef unsigned char       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;
  //Seed
  typedef unsigned char       SeedPixelType;
  typedef itk::Image< SeedPixelType, Dimension>   SeedImageType;
  //RGB
  typedef itk::RGBPixel<unsigned char>            RGBPixelType;
  typedef itk::Image< RGBPixelType, Dimension>   RGBImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;



  // seed dilatation filter
  typedef itk::BinaryBallStructuringElement < SeedPixelType, Dimension >	StructuringElementType;

  typedef itk::GrayscaleDilateImageFilter< SeedImageType, SeedImageType, StructuringElementType >
    DilateFilterType;

  // seed overlay filter
  typedef itk::LabelOverlayImageFilter< InputImageType, SeedImageType, RGBImageType >
    OverlayType;


  // output image writer
  typedef itk::ImageFileWriter< RGBImageType  > RGBWriterType;


  //*************READING image
  std::cout << "reading input image" << std::endl;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();


  //*************CREATING OUTPUT
  SeedImageType::Pointer SeedImage = SeedImageType::New();
  SeedImage->SetSpacing(  reader->GetOutput()->GetSpacing() );
  SeedImage->SetOrigin(   reader->GetOutput()->GetOrigin()  );
  SeedImage->SetRegions(  reader->GetOutput()->GetLargestPossibleRegion() );
  SeedImage->Allocate();

  // fill output with 0
  SeedPixelType NullPix;
  NullPix = 0;
  SeedImage->FillBuffer ( NullPix );




  std::cout << "Write " << argv[2] << " to Red channel" << std::endl;

  std::string line;

  SeedPixelType ConfidencePix;
  ConfidencePix = 0;


  SeedImageType::IndexType indexSeed;
  indexSeed[0] = 0;
  indexSeed[1] = 0;
  indexSeed[2] = 0;

  float fConfidence;
  std::stringstream sStream;
  std::ifstream seedFile (argv[2]);
  if (seedFile.is_open())
  {
    while (! seedFile.eof() )
    {
      getline (seedFile,line);
      if (line != "") // last line is empty...
      {
      sStream.clear();
      sStream.str("");
      sStream.str(line);
      sStream >> fConfidence
              >> indexSeed[0]
              >> indexSeed[1]
              >> indexSeed[2];

      ConfidencePix = static_cast<unsigned char>(ceil(fConfidence*255));
      std::cout <<" seeding at " << std::endl
                << " X= "<< indexSeed[0] << std::endl
                << " Y= "<< indexSeed[1] << std::endl
                << " Z= "<< indexSeed[2] << std::endl
                << " Conf= "<< (int)ConfidencePix <<std::endl;


      SeedImage->SetPixel( indexSeed, ConfidencePix );

      }
    }
    seedFile.close();
  }

  else
    {
    std::cerr << "Unable to open file";
    return EXIT_FAILURE;
    }



/*
  std::cout << "Write " << argv[3] << " to Green channel" << std::endl;

  ConfidencePix[0] = 0;
  ConfidencePix[1] = 0;
  ConfidencePix[2] = 0;


  seedFile.open (argv[3], std::ifstream::in );

  if (seedFile.is_open())
  {
    while (! seedFile.eof() )
    {
      getline (seedFile,line);
      if (line != "") // last line is empty...
      {
      sStream.clear();
      sStream.str("");
      sStream.str(line);
      sStream >> fConfidence
              >> indexRGB[0]
              >> indexRGB[1]
              >> indexRGB[2];

      ConfidencePix[1] = static_cast<unsigned char>(ceil(fConfidence*255));
      std::cout <<" seeding at " << std::endl
                << " X= "<< indexRGB[0] << std::endl
                << " Y= "<< indexRGB[0] << std::endl
                << " Z= "<< indexRGB[0] << std::endl
                << " Conf= "<< (int)ConfidencePix[1] <<std::endl;

      RGBImage->SetPixel( indexRGB, RGBImage->GetPixel(indexRGB)+ConfidencePix );
      }
    }
    seedFile.close();
  }

  else
    {
    std::cout << "Unable to open file";
    return EXIT_FAILURE;
    }
*/
  typedef InputImageType::SizeType SizeType;
  typedef SizeType::SizeValueType SizeValueType;

  double m_LargestCellRadius = 4.0;
  InputImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
  SizeType radius;
  for ( unsigned int j = 0; j < Dimension; j++ )
      {
      radius[j] = static_cast<SizeValueType> (
                    0.3*m_LargestCellRadius/spacing[j] );
      }

  std::cout << "dilating" << std::endl;
  DilateFilterType::Pointer grayscaleDilate = DilateFilterType::New();
  StructuringElementType structuringElement;
  structuringElement.SetRadius( radius );
  structuringElement.CreateStructuringElement();

  grayscaleDilate->SetKernel( structuringElement );
  grayscaleDilate->SetInput( SeedImage );
  grayscaleDilate->Update();

  std::cout << "overlaying" << std::endl;
  OverlayType::Pointer overlay = OverlayType::New();
  overlay->SetInput( reader->GetOutput() );
  overlay->SetLabelImage( grayscaleDilate->GetOutput() );
  overlay->SetOpacity( 1.0 );
  overlay->Update();



  RGBImageType::Pointer rgbImage = overlay->GetOutput();
  RGBImageType::IndexType index;
  index[0] = index[1] = index[2] = 0;
  RGBImageType::PixelType pixel;
  pixel[0] = pixel[1] = pixel[2] = 255;
  rgbImage->SetPixel(index, pixel);

  RGBWriterType::Pointer rgbwriter = RGBWriterType::New();
  rgbwriter->SetInput( rgbImage );
  rgbwriter->SetFileName( argv[4] );

  try
    {
    rgbwriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }




  return EXIT_SUCCESS;
}

