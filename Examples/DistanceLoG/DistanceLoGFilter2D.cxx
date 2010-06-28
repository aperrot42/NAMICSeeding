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

int main(int argc, char* argv [] )
{
  if ( argc < 8 )
    {
    std::cerr << "Missing Parameters: "
              << argv[0] << std::endl
              << "inputImage foregroundImage outputImage"
              << " [SigmaMin SigmaMax NumberOfScales]" << std::endl
              << "Allow outside of foreground detection (0-1)"<< std::endl;
    return EXIT_FAILURE;
    }

  // default sigma min-max in world coordinate?
  float m_SigmaMin = (float)atof(argv[4]);
  float m_SigmaMax = (float)atof(argv[5]);
  int m_NumberOfSigmaSteps = atoi(argv[6]);
  bool m_OutsideComputation = static_cast<bool>(atoi(argv[7]));

  // Define the dimension of the images
  const int Dimension = 2;

  // Declare the types of the images
  typedef float       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;
  typedef unsigned int        MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension>   MaskImageType;
  typedef float               OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension> OutputImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;
  // foreground image reader
  typedef itk::ImageFileReader< MaskImageType  > SegmentReaderType;

  // distance function filter
  typedef itk::SignedMaurerDistanceMapImageFilter< MaskImageType , OutputImageType >
    DistanceFilterType;

  // LoG filter
  typedef itk::MultiScaleLoGDistanceImageFilter< InputImageType, OutputImageType,OutputImageType >
    MultiScaleLoGDistanceFilterType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;


  //READING image
  std::cout << "reading input image" << std::endl;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();

  //READING foreground
  std::cout << "Reading  foreground" << std::endl;
  SegmentReaderType::Pointer readerFore = SegmentReaderType::New();
  readerFore->SetFileName ( argv[2] );
  readerFore->Update();

  // set the spacing of the foreground image to the spacing of the input
  readerFore->GetOutput()->SetSpacing( reader->GetOutput()->GetSpacing() );
  //SIGNED SQUARE DISTANCE COMPUTING
  std::cout << "foreground distance map computing" << std::endl;
  DistanceFilterType::Pointer distanceFilter
    = DistanceFilterType::New();
  distanceFilter->SetInput(readerFore->GetOutput());
  distanceFilter->SetUseImageSpacing(true);
  distanceFilter->SetInsideIsPositive(false);
  distanceFilter->SetSquaredDistance (true);
  distanceFilter->SetBackgroundValue(itk::NumericTraits< OutputPixelType >::Zero );
  distanceFilter->Update();

  //MULTISCALE LOG FILTERING
  MultiScaleLoGDistanceFilterType::Pointer LoGFilter
                               = MultiScaleLoGDistanceFilterType::New();
  LoGFilter->SetInput( reader->GetOutput() ); // input image
  LoGFilter->SetDistanceMap( distanceFilter->GetOutput() ); // squared sdf
  LoGFilter->SetSigmaMin(m_SigmaMin);
  LoGFilter->SetSigmaMax(m_SigmaMax);
  LoGFilter->SetNumberOfSigmaSteps(m_NumberOfSigmaSteps);
  LoGFilter->SetComputeOutsideForeground( m_OutsideComputation );
  LoGFilter->Update();


  //WRITING
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput ( LoGFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

