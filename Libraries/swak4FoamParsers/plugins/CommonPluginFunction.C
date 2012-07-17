/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CommonPluginFunction.H"

namespace Foam {

defineTypeNameAndDebug(CommonPluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CommonPluginFunction::CommonPluginFunction(
    const CommonValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnType,
    const string &argumentSpecificationString
):
    parentDriver_(parentDriver),
    name_(name),
    returnType_(returnType)
{
    DynamicList<word> argumentSpecification;
    {
        if(debug) {
            Info << "Parsing arguments: " << argumentSpecificationString << endl;
        }
        word next;
        forAll(argumentSpecificationString,i) {
            char c=argumentSpecificationString[i];
            if(c!=',') {
                next+=c;
            } else {
                if(next!="") {
                    argumentSpecification.append(next);
                    next="";
                }
            }
        }
        if(next!="") {
            argumentSpecification.append(next);
            next="";
        }

        argumentSpecification.shrink();

        if(debug) {
            Info << "Split arguments: " << argumentSpecification << endl;
        }
    }
    argumentNames_.resize(argumentSpecification.size());
    argumentParsers_.resize(argumentSpecification.size());
    argumentTypes_.resize(argumentSpecification.size());

    forAll(argumentSpecification,i)
    {
	const string &spec=argumentSpecification[i];
        label space1=spec.find(' ');
        label space2=spec.find(' ',space1+1);
        label space3=spec.find(' ',space2+1);

        if(
            space1 < 0 || space2 < 0 || space3 > 0
        ) {
            FatalErrorIn("CommonValueExpressionDriver::CommonPluginFunction::CommonPluginFunction")
                << "The argument specification " << spec
                    << " does not fit template '<name> <parser> <type>'"
                    << endl
                    << exit(FatalError);
        }
        argumentNames_[i]=spec(space1);
        argumentParsers_[i]=spec(space1+1,space2-space1-1);
        argumentTypes_[i]=spec(space2+1,spec.size()-space2-1);
        if(debug) {
            Info << "Argument " << i << ": "
                << argumentNames_[i] << " " << argumentParsers_[i]
                << " " << argumentTypes_[i] << endl;
        }
    }
    if(debug) {
        Info << "Help text: " << this->helpText() << endl;
    }
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

string CommonPluginFunction::helpText() const
{
    string result=returnType_+" "+name_+"(";
    forAll(argumentNames_,i)
    {
        if(i>0) {
            result+=",";
        }
        result+=argumentParsers_[i] + "/" + argumentTypes_[i]
            + " " + argumentNames_[i];
    }
    result+=")";

    return result;
}

label CommonPluginFunction::readArgument(
    label index,
    const string &content,
    CommonValueExpressionDriver &driver
)
{
    if(index>=argumentNames_.size()) {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Index " << index << " too big. Only values from 0 to"
                << argumentNames_.size()-1 << " possible"
                << endl
                << exit(FatalError);
    }
    word startSymbol=argumentTypes_[index];
    if(index+1 < argumentTypes_.size()) {
        startSymbol+="_SC";
    } else {
        startSymbol+="_CL";
    }
    if(debug) {
        Info << "Using start symbol " << startSymbol << endl;
    }
    driver.parse(content,startSymbol);
    if(driver.getResultType()!=argumentTypes_[index]) {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Result type " << driver.getResultType()
                << "differs from expected type " << argumentTypes_[index]
                << endl
                << exit(FatalError);
    }

    setArgument(index,content,driver);

    return driver.parserLastPos();
}

    // Workaround because steam() is protected
class IStringStreamWithPos
:
    public IStringStream
{
public:
    IStringStreamWithPos(const string &content)
        :
        IStringStream(content)
        {}

    label pos()
        {
            return label(stream().tellg());
        }
};

label CommonPluginFunction::readArgument(
    label index,
    const string &content,
    const word &type
) {
    if(debug) {
        Info << "CommonPluginFunction::readArgument "
            << index << " " << type << ":" << content << endl;
    }
    if(index>=argumentNames_.size()) {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Index " << index << " too big. Only values from 0 to"
                << argumentNames_.size()-1 << " possible"
                << endl
                << exit(FatalError);
    }

    IStringStreamWithPos is(content);

    if(type=="word") {
        word value;
        is.read(value);
        is.read(value);
        if(debug) {
            Info << "Read word: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="scalar") {
        scalar value;
        is.read(value);
        if(debug) {
            Info << "Read scalar: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="label") {
        label value;
        is.read(value);
        if(debug) {
            Info << "Read label: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Unsupported type " << type
                << endl
                << exit(FatalError);
    }

    label consumed=is.pos();

    return consumed;
}

label CommonPluginFunction::scanEmpty(
    const string &content,
    word sym
)
{
    if(debug) {
        Info << "Searching symbol " << sym << " in " << content << endl;
    }

    autoPtr<CommonValueExpressionDriver> driver=
        CommonValueExpressionDriver::New(
            "internalField",
            "scanEmpty",
            parentDriver_.mesh()
        );

    driver->setTrace(
        parentDriver_.traceScanning(),
        parentDriver_.traceParsing()
    );

    return driver().parse(content,sym);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
