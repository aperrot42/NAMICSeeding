#list the implementations of the filters
#SET( NAMICSEEDLIB_SRC
#  itkMultiScaleLoGDistanceImageFilter.txx
# )

#list the header of the filters
#SET( NAMICSEEDLIB_HDR
#  itkMultiScaleLoGDistanceImageFilter.h
#  )


#ADD_LIBRARY( NAMICFiltersLib
#  ${NAMICSEEDLIB_SRC}
#  )

#SET_TARGET_PROPERTIES(NAMICFiltersLib PROPERTIES LINKER_LANGUAGE C++)

# we link with lots of itk libraries for developping purpose
#TARGET_LINK_LIBRARIES( NAMICFiltersLib
#  ITKIO
#  ITKQuadEdgeMesh
#  ITKCommon
#  ITKBasicFilters
#  ITKAlgorithms
#  ITKStatistics
#  ITKNumerics
#  )

