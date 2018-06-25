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
    Foam::SubsetValueExpressionDriver

Description
    print a little banner with the swak-version to ease support

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "printSwakVersion.H"
#include "swak.H"

namespace Foam {
    void printSwakVersion() {
        Info << "swakVersion: " SWAK_VERSION_STRING;
#ifdef SWAK_VERSION_EXTENSION
        Info << " - " SWAK_VERSION_EXTENSION;
#endif
        Info << " (Release date: " SWAK_RELEASE_DATE ")";
#ifdef SWAK_HGBRANCH
        Info << " - HG Branch: " SWAK_HGBRANCH;
#endif
        Info << endl;
        Info << "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //";
        Info << endl;
    }
}
