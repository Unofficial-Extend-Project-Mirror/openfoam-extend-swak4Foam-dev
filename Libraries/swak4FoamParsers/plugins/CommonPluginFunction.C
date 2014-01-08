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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
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
        if(debug || parentDriver_.traceParsing()) {
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

        if(debug || parentDriver_.traceParsing()) {
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
                    << " of function " << name_
                    << " does not fit template '<name> <parser> <type>'"
                    << endl
                    << exit(FatalError);
        }
        argumentNames_[i]=spec(space1);
        argumentParsers_[i]=spec(space1+1,space2-space1-1);
        argumentTypes_[i]=spec(space2+1,spec.size()-space2-1);
        if(debug || parentDriver_.traceParsing()) {
            Info << "Argument " << i << ": "
                << argumentNames_[i] << " " << argumentParsers_[i]
                << " " << argumentTypes_[i] << endl;
        }
    }
    if(debug || parentDriver_.traceParsing()) {
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
    if(debug || parentDriver_.traceParsing()) {
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

    // Workaround because stdStream() is protected
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
#ifdef FOAM_ISTREAM_HAS_NO_STDSTREAM
            return label(stream().tellg());
#else
            return label(stdStream().tellg());
#endif
        }
};

label CommonPluginFunction::readArgument(
    label index,
    const string &content,
    const word &type
) {
    if(debug || parentDriver_.traceParsing()) {
        Info << "CommonPluginFunction::readArgument "
            << index << " " << type << " : " << content << endl;
    }
    if(index>=argumentNames_.size()) {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Index " << index << " too big. Only values from 0 to"
                << argumentNames_.size()-1 << " possible"
                << endl
                << exit(FatalError);
    }

    label consumed=0;

    string tc="";
    {
        bool atStart=true;
        forAll(content,i){
            if(isspace(content[i]) && atStart) {
                consumed++;
            } else {
                atStart=false;
                tc+=content[i];
            }
        }
        if(debug || parentDriver_.traceParsing()) {
            Info << "Removed leading spaces: " << tc << endl;
        }
    }

    if(type=="word") {
        tc.replace(",",";");
        if(debug || parentDriver_.traceParsing()) {
            Info << "Replacing , with ; :" << tc << endl;
        }
    }

    IStringStreamWithPos is(tc);

    if(type=="word") {
        word value;
        is.read(value);
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read word: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="string") {
        string  value;
        is.read(value);
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read string: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="scalar") {
        scalar value;
        is.read(value);
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read scalar: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="bool") {
        bool value(readBool(is));
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read bool: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="label") {
        label value;
        is.read(value);
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read label: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else if(type=="vector") {
        vector value;
        is >> value;
        if(debug || parentDriver_.traceParsing()) {
            Info << "Read vector: " << value
                << "(stream pos: " << label(is.pos()) << ")" << endl;
        }
        setArgument(index,value);
    } else {
        FatalErrorIn("CommonPluginFunction::readArgument")
            << "Unsupported type " << type
                << endl
                << "Currently supported: scalar, label, word, string, vector"
                << exit(FatalError);
    }

    consumed+=is.pos();

    return consumed;
}

label CommonPluginFunction::scanEmpty(
    const string &content,
    word sym
)
{
    if(debug || parentDriver_.traceParsing()) {
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

void CommonPluginFunction::evaluateInternal(
    const string &content,
    label &consumed
)
{
    if(debug || parentDriver_.traceParsing()) {
        Info << "Evaluating " << helpText() << " with " << content << endl;
    }
    consumed=0;

    forAll(argumentTypes_,i)
    {
        string currentContent=content.substr(
            consumed
        );

	if(debug || parentDriver_.traceParsing()) {
            Info << "Argument" << i << " with " << currentContent << endl;
        }

        label used=0;

        if(argumentParsers_[i]=="primitive") {
            if(debug || parentDriver_.traceParsing()) {
                Info << "Reading primitive argument" << i << endl;
            }
            used=readArgument(i,currentContent,argumentTypes_[i]);
            word lookFor="SC";
            if(i==(argumentParsers_.size()-1)) {
                lookFor="CL";
            }
            if(debug || parentDriver_.traceParsing()) {
                Info << "Used " << used << " characters "
                    << ". Now looking for " << lookFor << endl;
            }
            used+=scanEmpty(currentContent.substr(used),lookFor);
            if(debug || parentDriver_.traceParsing()) {
                Info << "Used " << used << " characters after looking for "
                    << lookFor << endl;
            }
        } else {
            autoPtr<CommonValueExpressionDriver> driver=
                CommonValueExpressionDriver::New(
                    argumentParsers_[i],
                    getID(i),
                    parentDriver_.mesh()
                );
            driver->setTrace(
                parentDriver_.traceScanning(),
                parentDriver_.traceParsing()
            );
            driver->setSearchBehaviour(
                parentDriver_.cacheReadFields(),
                parentDriver_.searchInMemory(),
                parentDriver_.searchOnDisc()
            );

            driver->appendToContext(
                parentDriver_.contextString()+"\n"+
                "  Evaluating expression \""+parentDriver_.content()+"\"\n"+
                "  Plugin Function \""+this->name()+"\" Substring \""+
                content+"\""
            );

            used=readArgument(i,currentContent,driver());
        }

        if(used<=0) {
            FatalErrorIn("CommonPluginFunction::evaluate")
                << "Evaluation of argument " << i+1
                    << " of " << this->helpText()
                    << " consumed no characters"
                    << endl
                    << exit(FatalError);

        }
        if(debug || parentDriver_.traceParsing()) {
            Info << "Argument" << i << " used "
                <<  used  << " charcters: ";
        }
        consumed+=used;
        if(debug || parentDriver_.traceParsing()) {
            Info << consumed << " charcters used till now" << endl;
        }
    }
    if(argumentTypes_.size()==0) {
        if(debug || parentDriver_.traceParsing()) {
            Info << "No parameters. Looking for closing ')'" << endl;
        }

        consumed+=scanEmpty(content,"CL");
        if(debug || parentDriver_.traceParsing()) {
            Info << consumed << " characters consumed" << endl;
        }
    }

    if(debug || parentDriver_.traceParsing()) {
        Info << "Starting the actual evaluation" << endl;
    }

    doEvaluation();

    if(debug || parentDriver_.traceParsing()) {
        Info << "Getting result and returning it" << endl;
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
