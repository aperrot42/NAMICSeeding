/*
 *  radialvoting.cpp
 *  
 *
 *  Created by Raghav Padmanabhan on 6/22/10.
 *  Copyright 2010 RPI. All rights reserved.
 *
 */

#include "radialvoting.h"

int main(int argc, char ** argv)
{
	
	if ( argc != 8 )
    {
		std::cerr << "Missing Parameters: "
		<< argv[0] << std::endl
		<< "inputImage outputImage" << "Min-Radius Max-Radius" << "Threshold" "Gradient Threshold" << " Number of cells " << std::endl;
		return EXIT_FAILURE;
    }
    
	// Initialize the reader 
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(argv[1]);
	try
	{
		reader->Update();
	}
	catch(itk::ExceptionObject &err)
	{
		std::cerr << "ExceptionObject caught!aaa" <<std::endl;
		std::cerr << err << std::endl;
		//return EXIT_FAILURE;
	}
	printf("Done Reading.\n");

	InputImageType::Pointer im;
	im = reader->GetOutput();
	
	//Instantiate the filter
	 FilterType::Pointer filter = FilterType::New();
	 filter->SetInput(im);
 	 filter->SetMinimumRadius(atoi(argv[3]));
	 filter->SetMaximumRadius(atoi(argv[4]));
	 filter->SetThreshold(atoi(argv[5])); //assuming it is an 8 bit image we are working with.
	 filter->SetGradientThreshold(atoi(argv[6])); // what is a good value ??? 
	 filter->SetNumberOfSpheres(atoi(argv[7])); //Assume there are thousand cells in the image
	 OutputImageType::Pointer Oim;
	 Oim = filter->GetOutput();

	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(argv[2]);
	writer->SetInput( Oim );
	
	// Exception Handling block 
	try 
    { 
		writer->Update(); 
    } 
	catch( itk::ExceptionObject & err ) 
    { 
		std::cerr << "ExceptionObject caught!!!! !" << std::endl; 
		std::cerr << err << std::endl; 
		return 1;
    } 
	return 0; 
	
	
}	




