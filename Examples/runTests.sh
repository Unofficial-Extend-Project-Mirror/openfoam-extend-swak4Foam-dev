#! /bin/bash

topDir=$(pwd)
marker="testStarted"

function runIt {
    local do_fpe=$1
    shift 1
    local here=$1
    shift 1
    cd $here
    if [ -e $marker ]; then
        echo "$here already tested"
        cd $topDir
        return
    fi
    touch $marker
    if [ "$do_fpe" == "0" ]; then
        echo "Unsetting FOAM_SIGFPE"
        unset FOAM_SIGFPE
    else
        echo "Setting FOAM_SIGFPE to $do_fpe"
        export FOAM_SIGFPE=$do_fpe
    fi
    echo
    echo "$here starting"
    echo
    pyFoamPrepareCase.py .
    pyFoamRunner.py $* --progress --auto auto
    echo
    echo "$here finished"
    echo
    cd $topDir
}

export FOAM_ABORT=1

if [ -n "$FOAM_API" ]; then
    switch_off_FPE=false
    switch_on_FPE=true
else
    switch_off_FPE=0
    switch_on_FPE=1
fi

# stored_FPE=$FOAM_SIGFPE
# unset FOAM_SIGFPE

# switch_off_FPE="FOAM_SIGFPE=false"
# switch_off_FPE="FOAM_SIGFPE=0"
# switch_off_FPE=

# switch_on_FPE=
# export FOAM_SIGFPE=true

runIt $switch_on_FPE DynamicMesh/hecticInletACMI2D
runIt $switch_on_FPE DynamicMesh/rotatingTank2D  --run-until=5
runIt $switch_on_FPE DynamicMesh/switchedInletACMI2D
runIt $switch_on_FPE DynamicMesh/switchedTJunction --run-until=0.5
runIt $switch_on_FPE FiniteArea/planeTransport
runIt $switch_on_FPE functionPlugins/chemPluginCounterFlowFlame2D
runIt $switch_on_FPE functionPlugins/shiftCavity
runIt $switch_off_FPE FvOptions/angleDuctWithSources
runIt $switch_on_FPE FvOptions/heatExchangerSources
runIt $switch_on_FPE groovyBC/average-t-junction
runIt $switch_on_FPE groovyBC/circulatingSplash --run-until=0.004
runIt $switch_on_FPE groovyBC/delayed-t-junction
runIt $switch_off_FPE groovyBC/fillingTheDam
runIt $switch_on_FPE groovyBC/jumpChannel
runIt $switch_on_FPE groovyBC/movingConeDistorted
runIt $switch_off_FPE groovyBC/pulsedPitzDaily --run-until=1e-3
runIt $switch_on_FPE groovyBC/wobbler
runIt $switch_on_FPE ImmersedBC/pitzDailyImmersed
runIt $switch_on_FPE Lagrangian/functionObjects/angledDuctWithBalls
runIt $switch_on_FPE Lagrangian/functionObjects/hotStream
runIt $switch_on_FPE Lagrangian/functionObjects/icoFoamCavityWithParcel
runIt $switch_on_FPE Lagrangian/LanguageIntegration/angledDuctWithLuaInject
runIt $switch_off_FPE Lagrangian/LanguageIntegration/angledDuctWithPython2Inject
runIt $switch_on_FPE Lagrangian/LanguageIntegration/angledDuctWithPython3Inject
runIt $switch_on_FPE Lagrangian/parser/parcelInBoxWithExpressions
runIt $switch_on_FPE Lagrangian/parser/simplifiedSiwek
runIt $switch_on_FPE LuaIntegration/flowStatePitzDaily
runIt $switch_on_FPE LuaIntegration/luaScriptedBCAngledDuct
runIt $switch_on_FPE LuaIntegration/raiseRelaxationPitzDaily
runIt $switch_on_FPE MakeAxialMesh/axialCavity
runIt $switch_on_FPE MakeAxialMesh/axialPitzDaily
runIt $switch_on_FPE manipulateFvSolutionFvSchemes/pitzDailyStateMachineSwitched
runIt $switch_on_FPE manipulateFvSolutionFvSchemes/pitzDailyTimeSwitched
runIt $switch_off_FPE other/angledDuctImplicit
runIt $switch_on_FPE other/capillaryRise --run-until=0.05
runIt $switch_on_FPE other/counterFlowFlame2DInitialized
runIt $switch_on_FPE other/simpleBendedPipe
runIt $switch_on_FPE other/topoSetDamBreak
runIt $switch_on_FPE Python3Integration/flowStatePitzDaily
runIt $switch_on_FPE Python3Integration/python3ScriptedBCAngledDuct
runIt $switch_on_FPE RegionSolvers/cavityAndPlate
runIt $switch_on_FPE RegionSolvers/cavityWithHeater
runIt $switch_on_FPE RegionSolvers/icoStructChannel
runIt $switch_on_FPE RegionSolvers/pitzDailyWithPotential
runIt $switch_on_FPE RegionSolvers/pitzDailyWithRASInlet  --run-until=0.01
runIt $switch_on_FPE RegionSolvers/thingOnAStick
runIt $switch_on_FPE runTimeCondition/simpleSwakCar
runIt $switch_on_FPE SimpleSurface/littleVolcano --run-until=0.1
runIt $switch_on_FPE solvePDE/flangeWithPDE
runIt $switch_on_FPE solvePDE/pitzDailyWithPDE
runIt $switch_on_FPE StateMachine/stateCavity
runIt $switch_off_FPE tests/AMIPipe --run-until=0.02
runIt $switch_on_FPE tests/delayedMappingChannels
runIt $switch_on_FPE tests/languageIntegration/luaIntegration
runIt $switch_off_FPE tests/languageIntegration/pythonIntegration
runIt $switch_off_FPE tests/languageIntegration/python3Integration
runIt $switch_on_FPE tests/mappingChannels
runIt $switch_on_FPE tests/mappingChannelsNonUniform
runIt $switch_on_FPE tests/SearchableSurfaces/objectTests
runIt $switch_on_FPE tests/SearchableSurfaces/operationsTest
runIt $switch_on_FPE tests/SearchableSurfaces/operationsTestRotated
runIt $switch_on_FPE tests/SearchableSurfaces/scaledIglooWithFridges
runIt $switch_on_FPE tests/swakDataEntry/flowRateAngledDuct
runIt $switch_on_FPE tests/testFOExecution
