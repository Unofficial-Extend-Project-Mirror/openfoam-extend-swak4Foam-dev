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
    Foam::GlobalVariablesRepository

Description

SourceFiles
    GlobalVariablesRepository.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef GlobalVariablesRepository_H
#define GlobalVariablesRepository_H

#include "ExpressionResult.H"

#include "HashPtrTable.H"

#include "regIOobject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class GlobalVariablesRepository Declaration
\*---------------------------------------------------------------------------*/

class GlobalVariablesRepository
:
    public regIOobject
{
public:
    class ResultTable
    :
    public HashPtrTable<ExpressionResult>
    {
    public:
        ResultTable(const ResultTable &r);
        ResultTable(Istream &);
        ResultTable();
    };
private:
   // Private data

    typedef HashTable<ResultTable> ResultTableTable;

    ResultTableTable globalVariables_;

    // Constructors

    //- Construct null
    GlobalVariablesRepository(const objectRegistry &obr);

    //- The only instance of this repository
    static GlobalVariablesRepository *repositoryInstance;

    static ExpressionResult zero_;

    ResultTable &getScope(const word &name);

    label lastTimeIndex_;

public:
     //- Runtime type information
    TypeName("GlobalVariablesRepository");

    //- Destructor

    virtual ~GlobalVariablesRepository();

    //- Get the Singleton of this class
    static GlobalVariablesRepository &getGlobalVariables(
        const objectRegistry &obr
    );

    //- get a global variable (if there is one)
    const ExpressionResult &get(
        const word &name,
        const wordList &scopes
    );

    ExpressionResult& addValue(
        const word &name,
        const word &scope,
        const ExpressionResult &value,
        const bool overwrite=true
    );

    bool removeValue(
        const word &name,
        const word &scope
    );

    ExpressionResult& addValue(
        const word &name,
        const word &scope,
        autoPtr<ExpressionResult> value,
        const bool overwrite=true
    );

    ExpressionResult& addValue(
        const dictionary &dict,
        const word scope="",
        const bool overwrite=true
    );

    //- get a global namespace
    ResultTable &getNamespace(const word &name);

    virtual bool writeData(Ostream &) const ;

    virtual bool readData(Istream &);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
