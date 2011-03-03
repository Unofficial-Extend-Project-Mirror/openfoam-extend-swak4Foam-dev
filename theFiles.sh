export SWAKLIBS=(groovyBC \
    simpleFunctionObjects \
    simpleSwakFunctionObjects \
    swakTopoSources \
    swak4FoamParsers \
    swakFunctionObjects)

if [ "$FOAM_DEV" != "" ]
then
    SWAKLIBS+=(swakFiniteArea)
fi

export SWAKUTILS=(funkySetBoundaryField \
    funkySetFields \
    replayTransientBC)

if [ "$FOAM_DEV" != "" ]
then
    SWAKUTILS+=(funkySetAreaFields)
fi
