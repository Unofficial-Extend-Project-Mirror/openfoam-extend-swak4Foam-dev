nr=int(nrPlanes)
dx=(maxX-minX)/nr

planeTemplate="""
    createsPlane%(nr)d
    {
        type createSampledSurface;
        outputControl timeStep;
        outputInterval 1;
        surfaceName autoPlane%(nr)d;
        surface {
            type plane;
            basePoint       (%(x)f 0 0);
            normalVector    (1 0 0);
            interpolate true;
        }
        writeSurfaceOnConstruction true;
        autoWriteSurface true;
        surfaceFormat vtk;
    }
"""

pressureTemplate="""
    pressurePlane%(nr)d
    {
        type swakExpression;
        valueType surface;
        surfaceName autoPlane%(nr)d;
        verbose true;
        expression "p";
        accumulations (
            min
            max
            weightedAverage
        );
    }
    pressurePlaneAverage%(nr)d
    {
        type swakExpression;
        valueType surface;
        surfaceName autoPlane%(nr)d;
        verbose true;
        expression "p*area()/sum(area())";
        accumulations (
           sum
        );
    }
"""
addToVars="""
     appendToVariable%(nr)d {
         type calculateGlobalVariables;
         outputControl timeStep;
         outputInterval 1;
         valueType surface;

         surfaceName autoPlane%(nr)d;
         toGlobalNamespace planeValues;
         toGlobalVariables (
             positions
             pressure
         );
         variables (
             "positions=average(pos().x);"
             "pressure=sum(p*area())/sum(area());"
         );
     }
"""

print "functions {"
for i in range(nr):
    x=dx*(i+0.5)+minX
    data= { "nr": i, "x": x }
    print planeTemplate % data
    print pressureTemplate % data
    print addToVars % data
print "}"
