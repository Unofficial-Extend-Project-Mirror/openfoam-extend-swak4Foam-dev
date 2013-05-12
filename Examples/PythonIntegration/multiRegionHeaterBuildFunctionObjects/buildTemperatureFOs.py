
from PyFoam.RunDictionary.ParsedParameterFile import ParsedParameterFile
from PyFoam.Basics.TemplateFile import TemplateFile

from os import path

rInfo=ParsedParameterFile(path.join(constantDir,"regionProperties"))

template="""
functions {
<!--(for name in fluids)-->
   energyFluid_In_|name| {
        type swakExpression;
        region |name|;
        expression "h*vol()";
        accumulations ( sum );
        valueType internalField;
        verbose true;
   }
<!--(end)-->
<!--(for name in solids)-->
   energySolid_In_|name| {
        type swakExpression;
        region |name|;
//        expression "rho*cp*T*vol()";
        expression "rho*T*vol()";
        accumulations ( sum );
        valueType internalField;
        verbose true;
   }
<!--(end)-->
}
"""

t=TemplateFile(content=template)

values = { 'fluids' : rInfo["fluidRegionNames"],
           'solids' : rInfo["solidRegionNames"] }

print t.getString(values)
