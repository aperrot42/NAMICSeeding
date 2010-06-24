cmake_minimum_required(VERSION 2.6)

project(Hungarian)


INCLUDE( CTest )

  # Then find ITK

  FIND_PACKAGE( ITK )

  IF( ITK_FOUND )

    INCLUDE( ${ITK_USE_FILE} )

  ENDIF( ITK_FOUND )

# Set your files and resources here  
SET( Srcs 
  Hungarian_Main.cxx
  )

SET( Hdrs
  hungarian_algorithm.h
  RandomMatrixGeneration.h
  )

ADD_EXECUTABLE( Hungarian_Main
  MACOSX_BUNDLE 
  ${Srcs}
  ${Hdrs}
)
  
TARGET_LINK_LIBRARIES( Hungarian_Main
  itkvnl
  itkvnl_algo
  ITKIO
  ITKCommon
  ITKStatistics
)
