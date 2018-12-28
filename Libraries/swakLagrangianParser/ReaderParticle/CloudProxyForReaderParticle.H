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
    Foam::CloudProxyForReaderParticle

Description

SourceFiles
    CloudProxyForReaderParticle.C

Contributors/Copyright:
    2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxyForReaderParticle_H
#define CloudProxyForReaderParticle_H

#include "CloudProxyForParticle.H"

#include "ReaderParticleCloud.H"

#include "HashPtrTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class CloudProxyForReaderParticle Declaration
\*---------------------------------------------------------------------------*/

class CloudProxyForReaderParticle
:
    public CloudProxyForParticle<ReaderParticleCloud>
{
    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForReaderParticle&);

    //- Disallow copy construct
    CloudProxyForReaderParticle(const CloudProxyForReaderParticle &);

    const ReaderParticleCloud &readerCloud_;

    template <typename T>
    void internalAddFields(const wordList &);

public:
    TypeName("CloudProxyForReaderParticle");

    // Constructors

    //- Construct from components
    CloudProxyForReaderParticle
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForReaderParticle();

    virtual tmp<Field<scalar> > getScalarField(
        const word &name
    ) const;

    virtual tmp<Field<vector> > getVectorField(
        const word &name
    ) const;

    virtual tmp<Field<tensor> > getTensorField(
        const word &name
    ) const;

    virtual tmp<Field<symmTensor> > getSymmTensorField(
        const word &name
    ) const;

    virtual tmp<Field<sphericalTensor> > getSphericalTensorField(
        const word &name
    ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CloudProxyForReaderParticleI.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
