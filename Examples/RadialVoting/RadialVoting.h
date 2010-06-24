/*
 *  radialvoting.h
 *  
 *
 *  Created by Raghav Padmanabhan on 6/22/10.
 *  Copyright 2010 RPI. All rights reserved.
 *
 */

//Including the necessary header files
#include <stdio.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <vector>
#include <algorithm> // not sure if this is required ; check again later
#include <iostream>
#include <sstream>
#include <string>
#include <iterator> 
#include "itkHoughTransformRadialVotingImageFilter.h"
#include "itkHoughTransformRadialVotingImageFilter.txx"

typedef unsigned char InputPixelType;
typedef unsigned char OutputPixelType;
typedef unsigned char          PixelType;
const unsigned short int             Dimension = 2;
//typedef itk::Image< PixelType, Dimension >   ImageType;
//typedef itk::Image<short int,3> LabelImageType; // Check again if the image should be of type "short int"
//Remove later if these will not be required ! 

typedef itk::Image<InputPixelType,Dimension> InputImageType;
typedef itk::Image<OutputPixelType,Dimension> OutputImageType;
typedef itk::ImageFileReader< InputImageType >  ReaderType;
//WriterType writes labeled images
typedef itk::ImageFileWriter< InputImageType >  WriterType;
typedef itk::HoughTransformRadialVotingImageFilter<InputImageType,InputImageType> FilterType; 
