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

Class
    timelineFunctionObject

Description
    Basis for function objects that generate timelines

SourceFiles
    timelineFunctionObject.C

Contributors/Copyright:
    2008-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef timelineFunctionObject_H
#define timelineFunctionObject_H

#include "simpleDataFunctionObject.H"
#include "NamedEnum.H"
#include "stringList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class timelineFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class timelineFunctionObject
:
    public simpleDataFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    timelineFunctionObject(const timelineFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const timelineFunctionObject&);

    //- Factor to multiply results with (to account for symmetries etc)
    scalar factor_;

public:
    enum outputFileMode {
        ofmFoam,
        ofmRaw,
        ofmCsv
    };

private:

    //- Current open files
    HashPtrTable<OFstream> filePtrs_;

protected:

    //- Name of the sub-dictionary of the case
    virtual word dirName()=0;

    //- Names of the files
    virtual wordList fileNames()=0;

    //- Text in the first line
    virtual stringList columnNames()=0;

    //- Factor to multiply results with (for symmetries etc)
    scalar factor() const { return factor_; }

    //- possible output-modes
    static const NamedEnum<outputFileMode,3> outputFileModeNames_;
    outputFileMode outputFileMode_;

    //- the file extension that fits the file-mode
    word fileExtension_;

    //- character that separates fields
    std::string separator_;

    //- write similar data items
    template<class T>
    void writeData(const word &name,const UList<T> &data);

    //- write the time
    template<class T>
    void writeTime(const word &name,const T &time);

    //- ends a dataset (usually by writing an endline)
    void endData(const word &name);

    //- close all files to enable a new slate
    void closeAllFiles();

private:
    //- should we write immediately at startup
    bool writeStartTime_;

public:

    //- Runtime type information
    TypeName("timelineFunctionObject");


    // Constructors

    //- Construct from components
    timelineFunctionObject
    (
        const word &name,
        const Time&,
        const dictionary&,
        bool writeStartTime=false
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    virtual void flush();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "timelineFunctionObjectI.H"
#endif

#endif

// ************************************************************************* //
