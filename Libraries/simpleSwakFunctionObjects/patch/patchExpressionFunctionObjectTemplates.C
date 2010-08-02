//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "patchExpressionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
void patchExpressionFunctionObject::writeData(const word &pName,PatchValueExpressionDriver &driver)
{
    Field<T> result=driver.getResult<T>();

    Field<T> results(accumulations_.size());

    forAll(accumulations_,i) {
        const word &aName=accumulations_[i];
        T val=pTraits<T>::zero;

        if(aName=="min") {
            val=gMin(result);
        } else if(aName=="max") {
            val=gMax(result);
        } else if(aName=="sum") {
            val=gSum(result);
        } else if(aName=="average") {
            val=gAverage(result);
        } else {
            WarningIn("patchExpressionFunctionObject::writeData")
                << "Unknown accumultation type " << aName
                    << ". Currently only 'min', 'max', 'sum' and 'average' are supported"
                    << endl;
        }
        results[i]=val;
        if(verbose()) {
            Info << " " << aName << "=" << val;
        }
    }

    if (Pstream::master()) {
        unsigned int w = IOstream::defaultPrecision() + 7;
        
        OFstream& o=*filePtrs_[pName];
        
        o << setw(w) << time().value();
        forAll(results,i) {
            o << setw(w) << results[i];
        }
        o << nl;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
