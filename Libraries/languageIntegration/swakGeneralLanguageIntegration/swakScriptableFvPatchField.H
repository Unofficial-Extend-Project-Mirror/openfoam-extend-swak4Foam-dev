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

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::swakScriptableFvPatchField

Description
    Foam::swakScriptableFvPatchField

SourceFiles
    swakScriptableFvPatchField.C

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakScriptableFvPatchField_H
#define swakScriptableFvPatchField_H

#include "mixedFvPatchField.H"

#include "PatchValueExpressionDriver.H"

#include "generalInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class swakScriptableFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class swakScriptableFvPatchField
:
    public mixedFvPatchField<Type>
{

protected:

    // Protected data

    PatchValueExpressionDriver driver_;

    autoPtr<generalInterpreterWrapper> interpreter_;

    string initCode_;

    string evaluateCode_;

    string writeCode_;

    wordList variablesToUse_;

    wordList writtenDataStructs_;

    List<exprString> initVariables_;

    List<exprString> evaluateVariables_;

    void evaluateScript(
        const string &script,
        const List<exprString> &variables
    );

public:

    //- Runtime type information
    TypeName("swakScriptable");


    // Constructors

    //- Construct from patch and internal field
    swakScriptableFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct from patch, internal field and dictionary
    swakScriptableFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const dictionary&
    );

    //- Construct by mapping given swakScriptableFvPatchField onto a new patch
    swakScriptableFvPatchField
    (
        const swakScriptableFvPatchField<Type>&,
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const fvPatchFieldMapper&
    );

    //- Construct as copy
    swakScriptableFvPatchField
    (
        const swakScriptableFvPatchField<Type>&
    );

    //- Construct and return a clone
    virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
                (
                    new swakScriptableFvPatchField<Type>(*this)
                );
        }

    //- Construct as copy setting internal field reference
    swakScriptableFvPatchField
    (
        const swakScriptableFvPatchField<Type>&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct and return a clone setting internal field reference
    virtual tmp<fvPatchField<Type> > clone
    (
        const DimensionedField<Type, volMesh>& iF
    ) const
        {
            return tmp<fvPatchField<Type> >
                (
                    new swakScriptableFvPatchField<Type>(*this, iF)
                );
        }


    // Member functions

    //- Update the coefficients associated with the patch field
    virtual void updateCoeffs();

    //- Write
    virtual void write(Ostream&) const;
};


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "swakScriptableFvPatchField.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #endif

// ************************************************************************* //
