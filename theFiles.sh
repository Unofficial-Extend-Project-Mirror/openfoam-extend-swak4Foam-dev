# Lists the files that are going to be used by other scripts

export SWAKLIBS=(groovyBC \
    groovyStandardBCs \
    swakSourceFields \
    simpleFunctionObjects \
    simpleSwakFunctionObjects \
    simpleLagrangianFunctionObjects \
    swakTopoSources \
    swak4FoamParsers \
    swakPythonIntegration* \
    swak*FunctionPlugin \
    swakFunctionObjects)

if [ "$FOAM_DEV" != "" ]
then
    SWAKLIBS+=(swakFiniteArea)
fi

export SWAKUTILS=(funkySetBoundaryField \
    funkySetFields \
    funkyDoCalc \
    calcNonUniformOffsetsForMapped \
    replayTransientBC)

if [ "$FOAM_DEV" != "" ]
then
    SWAKUTILS+=(funkySetAreaFields)
fi
