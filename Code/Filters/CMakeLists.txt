SET( NAMICSEEDLIB_SRC


  )

SET( NAMICSEEDLIB_HDR




  )


ADD_LIBRARY( NAMICFilters
  ${NAMICSEEDLIB_SRC}
  )

TARGET_LINK_LIBRARIES( NAMICFilters
  ITKIO
  ITKQuadEdgeMesh
  ITKCommon
  ITKBasicFilters
  ITKAlgorithms
  ITKStatistics
  ITKNumerics
  )

