/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::SwakSetValue

Description
    Fixes field values according to an expression

SourceFiles
    SwakSetValue.C

Contributors/Copyright:
    2010, 2012-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakSetValue_H
#define SwakSetValue_H

#include "SwakBasicSourceCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SwakSetValue Declaration
\*---------------------------------------------------------------------------*/

#ifdef FOAM_HAS_FVOPTIONS
    namespace fv {
#endif

template<class T>
class SwakSetValue
:
    public SwakBasicSourceCommon<T>
{
    // Private data

    SwakSetValue(const SwakSetValue&);

    bool useMaskExpression_;

    exprString maskExpression_;

    bool getMask(DynamicList<label> &,const word &psi);

public:

    //- Runtime type information
    TypeName("SwakSetValue");

    // Constructors

        //- Construct from a dictionary
        SwakSetValue
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

    // Destructor

        virtual ~SwakSetValue();

        // Evaluation

            //- Set value on vector field
            virtual void setValue(fvMatrix<T>& eqn, const label fieldI);

            //- in newer versions setValue was renamed
            virtual void constrain(fvMatrix<T>& eqn, const label fieldI) {
                setValue(eqn,fieldI);
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#ifdef FOAM_HAS_FVOPTIONS
    }
#endif

} // End namespace Foam

#ifdef NoRepository
#   include "SwakSetValue.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
