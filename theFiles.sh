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
    swak*Integration* \
    swakScriptableLagrangian \
    swak*FunctionPlugin \
    swak*CloudAdaptor \
    swakFvOptions \
    swakStateMachine \
    swakDynamicMesh \
    swakFunctionObjects)

if [[ "$FOAM_DEV" != "" || ( ${WM_PROJECT_VERSION:0:1} == "v" && ${WM_PROJECT_VERSION:1} -gt 1707 ) ]]
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
    writeBoundarySubfields \
    replayTransientBC)

if [ "$FOAM_DEV" != "" ]
then
    SWAKUTILS+=(funkySetAreaFields)
fi
