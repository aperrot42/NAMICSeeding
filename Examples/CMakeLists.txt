#list of example directories
SET( NAMICSEED_EXAMPLE_DIR
  DistanceLoG
#  Median
  LocalMaxima
  RadialVoting
  )

FOREACH( var ${NAMICSEED_EXAMPLE_DIR} )
  ADD_SUBDIRECTORY( ${var} )
ENDFOREACH( var ${NAMICSEED_EXAMPLE_DIR} )