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
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkNumericTraits.h"
#include "itkMultiScaleLoGDistanceImageFilter.h"
#include "itkImage.h"

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
  float m_SigmaMin = (float)atof(argv[4])/sqrt(2);
  float m_SigmaMax = (float)atof(argv[5])/sqrt(2);
  int m_NumberOfSigmaSteps = atoi(argv[6]);
  bool m_OutsideComputation = static_cast<bool>(atoi(argv[7]));


  std::cout << "Distance LoG filter for input : "
            << "Sigma = [" << m_SigmaMin << " ; " << m_SigmaMax <<"]"
            << std::endl;
  // Define the dimension of the images
  const int Dimension = 3;

  // Declare the types of the images
  typedef float       InputPixelType;
  typedef itk::Image< InputPixelType, Dimension>  InputImageType;
  typedef unsigned char        MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension>   MaskImageType;
  typedef float               OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension> OutputImageType;

  // input image reader
  typedef itk::ImageFileReader< InputImageType  > ImageReaderType;
  // foreground image reader
  typedef itk::ImageFileReader< MaskImageType  > SegmentReaderType;


  // closing filter type for foreground
  typedef itk::BinaryBallStructuringElement< MaskPixelType, Dimension > KernelType;
  typedef itk::BinaryMorphologicalClosingImageFilter<MaskImageType, MaskImageType, KernelType>
    ClosingFilterType;
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
  std::cout << "Reading  foreground";
  SegmentReaderType::Pointer readerFore = SegmentReaderType::New();
  readerFore->SetFileName ( argv[2] );
  readerFore->Update();

  // set the spacing of the foreground image to the spacing of the input
  MaskImageType::Pointer maskImage = readerFore->GetOutput();
  InputImageType::SpacingType m_Spacing;//sPACINGtYPE M-s
  m_Spacing = reader->GetOutput()->GetSpacing();
  maskImage->SetSpacing( m_Spacing );
  std::cout << " . Spacing :" << maskImage->GetSpacing() << std::endl;

  //FOREGROUND DENOISING
  std::cout << "closing filter for sdf" << std::endl;
  KernelType ball;
  KernelType::SizeType ballSize;

  ballSize[0] = static_cast<MaskPixelType>( m_SigmaMin/(2*m_Spacing[0]) ) ; // radius in x
  ballSize[1] = static_cast<MaskPixelType>( m_SigmaMin/(2*m_Spacing[1]) ) ; // y
  ballSize[2] = static_cast<MaskPixelType>( m_SigmaMin/(m_Spacing[2]) ) ; // z

  std::cout << "closing filter mask ellipse : radius in pixels :"
            << ballSize[0] << " "
            << ballSize[1] << " "
            << ballSize[2] << std::endl;
  //ballSize.Fill (1);
  ball.SetRadius( ballSize );
  ball.CreateStructuringElement();

  ClosingFilterType::Pointer closingFilter = ClosingFilterType::New();
  closingFilter->SetKernel( ball );
  closingFilter->SetInput(maskImage);
  closingFilter->Update();





  //SIGNED SQUARE DISTANCE COMPUTING
  std::cout << "foreground distance map computing" << std::endl;
  DistanceFilterType::Pointer distanceFilter
    = DistanceFilterType::New();
  distanceFilter->SetInput( closingFilter->GetOutput() );
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

