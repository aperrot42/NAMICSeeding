#include "hungarian_algorithm.h"
#include "RandomMatrixGeneration.h"
#include <vcl_limits.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>

#include <vcl_iostream.h>
#include <vcl_cstdio.h>

#include "itkVector.h"
#include "itkImage.h"

#include "itkRGBPixel.h"
#include "itkPointSet.h"


#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

vnl_matrix<double> generateRandomMatrixNxM(int rows, int columns, int maxValue)
{
  vnl_matrix<double> cost( rows, columns );

  vnl_random random;
  random.reseed( 50 );

  for (int i = 0; i < rows; ++i)
  {
    for(int j = 0; j < columns; ++j)
    {
    cost[i][j] = random.drand32( maxValue );
    }
  }

  return cost;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

int main( int argc, char** argv )
{

  // Definition of the image type
  typedef itk::Vector< double, 3 >       PixelType;
  //typedef itk::RGBPixel< float >         PixelType;
  typedef itk::Image< PixelType, 3 >     ImageType;

  // Create matrix with the good image type
  RandomMatrixGeneration< ImageType > RandomMatrixGenerationTest;

  RandomMatrixGenerationTest.SetImageSize( 30 );
  RandomMatrixGenerationTest.SetNumberOfPoints(10);
  RandomMatrixGenerationTest.SetRamdomVectorsMaxNorm(3);
  RandomMatrixGenerationTest.CreateEvolutionVectors();
  RandomMatrixGenerationTest.CreateInputImage();
  RandomMatrixGenerationTest.ApplyGaussianFilter();
  RandomMatrixGenerationTest.ApplyVectorToPoints();
  RandomMatrixGenerationTest.ExportAsTiff();

// Create input image
  /*int cost_val[6][6] = { {  4, 12, 11, 20, 16, 19 },
                         { 12,  8, 20, 13, 22, 18 },
                         {  6,  9,  4, 15,  9, 12 },
                         { 12,  4, 12,  6, 14,  8 },
                         { 12, 10,  6,  9,  5,  3 },
                         { 13,  7, 12,  2, 10,  4 }
                       };

  // Create the cost matrix
  vnl_matrix< int > cost( &cost_val[0][0], 6, 6 );

  // Create a random Matrix of size N*M
  //vnl_matrix<double> cost = generateRandomMatrixNxM( atoi(argv[1]),
  //atoi(argv[2]), atoi(argv[3]) );
  hungarian_algorithm< int > HungarianClassTest;
  HungarianClassTest.SetCostMatrix( cost );
  HungarianClassTest.StartAssignment();
  std::cout<<"//-----------------------//"<<std::endl;
  std::cout<<"//       Matrix 1        //"<<std::endl;
  std::cout<<"//-----------------------//"<<std::endl;
  // the input matrix
  std::cout<< "input matrix: \n" << cost << std::endl;

  //returns the assignment matrix
  std::cout<< "assignment matrix: \n"
    << HungarianClassTest.GetAssignmentMatrix() << std::endl;

  // there should be a better way but we use vcl_vector because no push_back
  // method in vnl_vector
  vcl_vector<unsigned> assignment = HungarianClassTest.GetAssignmentVector();

  std::cout<< "assignment vector:" << std::endl;
  vcl_cout << "( ";
  for (unsigned int i=0; i<assignment.size() ;++i) vcl_cout << assignment[i] << ' ';
  vcl_cout << ")\n";

  //returns the cost of the assignment
  std::cout<< "\ntotal cost: " << HungarianClassTest.GetTotalCost() << "\n"
    << std::endl;

  int cost_val2[6][6] = { { 2, 3.0, 1, 0.1, 2, 7 },
                            { 1, 0, 1, 2, 3.0, 4 },
                            { 0, 0, 9, 5, 4.4, 2 },
                            { 1, 5, 6, 3, 0, 1 },
                            { 0, 1, 2, 0, 1, 2 },
                            { 2, 3, 1, 0.1, 1, 1 }
                          };

  // Create the cost matrix
  vnl_matrix< int > cost2( &cost_val2[0][0], 6, 6 );

  HungarianClassTest.SetCostMatrix( cost2 );
  HungarianClassTest.StartAssignment();
  std::cout<<"//-----------------------//"<<std::endl;
  std::cout<<"//       Matrix 2        //"<<std::endl;
  std::cout<<"//-----------------------//"<<std::endl;
  // the input matrix
  std::cout<< "input matrix: \n" << cost2 << std::endl;

  //returns the assignment matrix
  std::cout<< "assignment matrix: \n"
    << HungarianClassTest.GetAssignmentMatrix() << std::endl;

  // there should be a better way but we use vcl_vector because no push_back
  // method in vnl_vector
  vcl_vector<unsigned> assignment2 = HungarianClassTest.GetAssignmentVector();

  std::cout<< "assignment vector:" << std::endl;
  vcl_cout << "( ";
  for (unsigned int i=0; i<assignment2.size() ;++i) vcl_cout << assignment2[i] << ' ';
  vcl_cout << ")\n";

  //returns the cost of the assignment
  std::cout<< "\ntotal cost: " << HungarianClassTest.GetTotalCost() << "\n"
    << std::endl;*/

  return 0;
}
