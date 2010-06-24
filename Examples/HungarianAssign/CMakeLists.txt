SET( NAMICSEED_EXAMPLE_SRC
  MedianFilter3D
  )


FOREACH( var ${NAMICSEED_EXAMPLE_SRC} )

  ADD_EXECUTABLE( ${var} ${var} )
  TARGET_LINK_LIBRARIES( ${var}
  ITKIO
  ITKQuadEdgeMesh
  ITKCommon
  ITKBasicFilters
  ITKAlgorithms
  ITKStatistics
  ITKNumerics
  itkvnl
  itkvnl_algo

  )


ENDFOREACH( var ${NAMICSEED_EXAMPLE_SRC} )
