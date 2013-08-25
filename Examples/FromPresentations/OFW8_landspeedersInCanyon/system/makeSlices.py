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

exhaustAverageTemplate="""
    exhaustAveragePlane%(nr)d
    {
        type swakExpression;
        valueType surface;
        surfaceName autoPlane%(nr)d;
        verbose true;
        expression "exhaust";
        accumulations (
            min
            max
            weightedAverage
        );
    }
    exhaustAveragePlaneAverage%(nr)d
    {
        type swakExpression;
        valueType surface;
        surfaceName autoPlane%(nr)d;
        verbose true;
        expression "exhaust*area()/sum(area())";
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
             exhaustAverage
             exhaustHeight
             exhaustHeight2
         );
         variables (
             "positions=average(pos().x);"
             "exhaustAverage=sum(exhaust*area())/sum(area());"
             "exhaustHeight=max(exhaust>0.001 ? pos().z : -1);"
             "exhaustTotal=exhaustAverage>0.001 ? sum(exhaust*area()) : sum(0.001*area());"
             "exhaustHeight2=sum(exhaust*pos().z*area())/exhaustTotal;"
         );
     }
"""

print "functions {"
for i in range(nr):
    x=dx*(i+0.5)+minX
    data= { "nr": i, "x": x }
    print planeTemplate % data
    #    print exhaustAverageTemplate % data
    print addToVars % data
print "}"
