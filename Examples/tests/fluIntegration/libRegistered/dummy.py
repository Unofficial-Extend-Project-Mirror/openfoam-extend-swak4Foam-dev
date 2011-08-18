#!/usr/bin/env python

#------------------------------------------------------------------------------------
import Foam.functionObjects
# import os; os._exit( os.EX_OK )


#----------------------------------------------------------------------------------------------------------
import sys
argv = sys.argv
argc = len( argv )

from Foam.OpenFOAM.include import setRootCase
args = setRootCase( argc, argv )

from Foam.OpenFOAM.include import createTime
runTime = createTime( args )

while runTime.loop() :
    pass


#------------------------------------------------------------------------------------
