# Lists the files that are going to be used by other scripts

export SWAKLIBS=(groovyBC \
    groovyStandardBCs \
    swakSourceFields \
    simpleFunctionObjects \
    simpleSwakFunctionObjects \
    simpleLagrangianFunctionObjects \
    simpleSearchableSurfaces \
    swakTopoSources \
    swak4FoamParsers \
    swakLagrangianParser \
    swakPythonIntegration* \
    swak*FunctionPlugin \
    swak*CloudAdaptor \
    swakFvOptions \
    swakFunctionObjects)

if [ "$FOAM_DEV" != "" ]
then
    SWAKLIBS+=(swakFiniteArea)
else
    SWAKLIBS+=(simpleCloudFunctionObjects \
         swakCloudFunctionObjects)
fi

export SWAKUTILS=(funkySetBoundaryField \
    funkySetFields \
    funkyDoCalc \
    funkyWarpMesh \
    calcNonUniformOffsetsForMapped \
    fieldReport \
    funkyPythonPostproc \
    funkySetLagrangianField \
    replayTransientBC)

if [ "$FOAM_DEV" != "" ]
then
    SWAKUTILS+=(funkySetAreaFields)
fi
