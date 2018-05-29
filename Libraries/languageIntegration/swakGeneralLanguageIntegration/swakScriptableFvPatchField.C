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

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakScriptableFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
swakScriptableFvPatchField<Type>::swakScriptableFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    mixedFvPatchField<Type>(p, iF),
    driver_(this->patch())
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField 1" << endl;
    }

    this->refValue() = pTraits<Type>::zero;
    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 1;
}


template<class Type>
swakScriptableFvPatchField<Type>::swakScriptableFvPatchField
(
    const swakScriptableFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFvPatchField<Type>(ptf, p, iF, mapper),
    driver_(this->patch(),ptf.driver_),
    interpreter_()
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField 2" << endl;
    }
}


template<class Type>
swakScriptableFvPatchField<Type>::swakScriptableFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFvPatchField<Type>(p, iF),
    driver_(dict,this->patch()),
    interpreter_(
        generalInterpreterWrapper::New(
            this->patch().boundaryMesh().mesh(),
            dict
        )
    ),
    initCode_(
        interpreter_->readCode("initBC")
    ),
    evaluateCode_(
        interpreter_->readCode("evaluateBC")
    ),
    writeCode_(
        interpreter_->readCode("writeBC")
    ),
    variablesToUse_(
        dict.lookup("variablesToUse")
    ),
    writtenDataStructs_(
        dict.lookup("writtenDataStructs")
    ),
    initVariables_(
        driver_.readVariableStrings(
            dict,
            "initVariables",
            0,
            true
        )
    ),
    evaluateVariables_(
        driver_.readVariableStrings(
            dict,
            "evaluateVariables",
            0,
            true
        )
    )
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField 3" << endl;
    }

    driver_.readVariablesAndTables(dict);

    if (dict.found("refValue")) {
        this->refValue() = Field<Type>("refValue", dict, p.size());
    } else {
        this->refValue() = this->patchInternalField();
    }

    if (dict.found("value"))
    {
        fvPatchField<Type>::operator=
        (
            Field<Type>("value", dict, p.size())
        );
	if (!dict.found("refValue")) {
 	    // make sure that refValue has a sensible value for the "update" below
	    this->refValue() = Field<Type>("value", dict, p.size());
	}
    }
    else
    {
        fvPatchField<Type>::operator=(this->refValue());
        WarningIn(
            "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField"
            "("
            "const fvPatch& p,"
            "const DimensionedField<Type, volMesh>& iF,"
            "const dictionary& dict"
            ")"
        ) << "No value defined for "
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
            << this->internalField().name()
#else
            << this->dimensionedInternalField().name()
#endif
            << " on " << this->patch().name() << " therefore using "
            << "the internal field next to the patch"
            << endl;
    }

    if (dict.found("refGradient")) {
        this->refGrad() = Field<Type>("refGradient", dict, p.size());
    } else {
        this->refGrad() = pTraits<Type>::zero;
    }

    if (dict.found("valueFraction")) {
        this->valueFraction() = Field<scalar>("valueFraction", dict, p.size());
    } else {
        this->valueFraction() = 1;
    }

    if(writtenDataStructs_.size()>0) {
        const dictionary &data=dict.subDict(interpreter_->interpreterName()+"Data");
        forAll(writtenDataStructs_,i) {
            const word &name=writtenDataStructs_[i];
            if(data.found(name)) {
                interpreter_->insertDictionary(
                    name,
                    data.subDict(name)
                );
            } else {
                Info << "No entry " << name << " found in "
                    << data.name() << endl;
            }
        }
    }

    interpreter_->initEnvironment(this->patch().boundaryMesh().mesh().time());
    interpreter_->setRunTime(this->patch().boundaryMesh().mesh().time());

    evaluateScript(
        initCode_,
        initVariables_
    );

    // emulate mixedFvPatchField<Type>::evaluate, but avoid calling "our" updateCoeffs
    if (!this->updated())
    {
        this->mixedFvPatchField<Type>::updateCoeffs();
    }

    Field<Type>::operator=
        (
            this->valueFraction()*this->refValue()
            +
            (1.0 - this->valueFraction())*
            (
                this->patchInternalField()
                + this->refGrad()/this->patch().deltaCoeffs()
            )
        );

    fvPatchField<Type>::evaluate();
}


template<class Type>
swakScriptableFvPatchField<Type>::swakScriptableFvPatchField
(
    const swakScriptableFvPatchField<Type>& ptf
)
:
    mixedFvPatchField<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField 4" << endl;
    }
}


template<class Type>
swakScriptableFvPatchField<Type>::swakScriptableFvPatchField
(
    const swakScriptableFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    mixedFvPatchField<Type>(ptf, iF),
    driver_(this->patch(),ptf.driver_),
    interpreter_(
        const_cast<autoPtr<generalInterpreterWrapper>&>(
            ptf.interpreter_
        ).ptr()
    ),
    initCode_(
        ptf.initCode_
    ),
    evaluateCode_(
        ptf.evaluateCode_
    ),
    writeCode_(
        ptf.writeCode_
    ),
    variablesToUse_(
        ptf.variablesToUse_
    ),
    writtenDataStructs_(
        ptf.writtenDataStructs_
    ),
    initVariables_(
        ptf.initVariables_
    ),
    evaluateVariables_(
        ptf.evaluateVariables_
    )
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::swakScriptableFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void swakScriptableFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::updateCoeffs" << endl;
        Info << "Variables: ";
        driver_.writeVariableStrings(Info) << endl;
    }
    if (this->updated())
    {
        return;
    }

    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::updateCoeffs - updating" << endl;
    }

    interpreter_->setRunTime(this->patch().boundaryMesh().mesh().time());

    evaluateScript(
        evaluateCode_,
        evaluateVariables_
    );

    mixedFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void swakScriptableFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "swakScriptableFvPatchField<Type>::write" << endl;
    }
    mixedFvPatchField<Type>::write(os);

    driver_.writeCommon(os,debug);

    List<exprString> dummyVariables;

    const_cast<swakScriptableFvPatchField<Type>*>(
        this
    )->interpreter_->setRunTime(this->patch().boundaryMesh().mesh().time());

    const_cast<swakScriptableFvPatchField<Type>*>(
        this
    )->evaluateScript(
        writeCode_,
        dummyVariables
    );

    if(writtenDataStructs_.size()>0) {
        dictionary data;
        forAll(writtenDataStructs_,i) {
            const word &name=writtenDataStructs_[i];
            dictionary subData;
            const_cast<swakScriptableFvPatchField<Type>&>(
                *this
            ).interpreter_->extractDictionary(
                name,
                subData
            );
            data.add(name,subData);
        }
        os.writeKeyword(word(interpreter_->interpreterName()+"Data"))
            << data << nl;
    }

    interpreter_->write(os);

    interpreter_->writeCode(
        word("initBC"),
        os
    );
    interpreter_->writeCode(
        word("evaluateBC"),
        os
    );
    interpreter_->writeCode(
        word("writeBC"),
        os
    );
    os.writeKeyword("variablesToUse")
        << variablesToUse_ << token::END_STATEMENT << nl;
    os.writeKeyword("writtenDataStructs")
        << writtenDataStructs_ << token::END_STATEMENT << nl;
    os.writeKeyword("initVariables")
        << initVariables_ << token::END_STATEMENT << nl;
    os.writeKeyword("evaluateVariables")
        << evaluateVariables_ << token::END_STATEMENT << nl;
}

template<class Type>
void swakScriptableFvPatchField<Type>::evaluateScript(
    const string &script,
    const List<exprString> &variables
) {
    Dbug << "Evaluating " << script << endl;

    driver_.addVariables(
        variables,
        true
    );

    DynamicList<word> vars;
    forAll(variablesToUse_,i) {
        if(driver_.hasVariable(variablesToUse_[i])){
            vars.append(variablesToUse_[i]);
        }
    }
    interpreter_->getVariablesFromDriver(
        driver_,
        vars
    );

    interpreter_->setReference("refValue",this->refValue());
    interpreter_->setReference("refGrad",this->refGrad());
    interpreter_->setReference("valueFraction",this->valueFraction());

    interpreter_->executeCode(
        script, false
    );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
