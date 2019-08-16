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

Description
    Backport macros that are not available in all OpenFOAM-versions

SourceFiles

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2019 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SWAK_BACKPORT_MACROS_H
#define SWAK_BACKPORT_MACROS_H

#ifndef FUNCTION_NAME
// Compiler provided function name string:
//     for gcc-compatible compilers use __PRETTY_FUNCTION__
//     otherwise use the standard __func__
#ifdef __GNUC__
    #define FUNCTION_NAME __PRETTY_FUNCTION__
#else
    #define FUNCTION_NAME __func__
#endif
#endif

#ifndef FatalErrorInFunction
#define FatalErrorInFunction FatalErrorIn(FUNCTION_NAME)
#endif

#ifndef NotImplemented
#define NotImplemented notImplemented(FUNCTION_NAME)
#endif

#ifndef WarningInFunction
#define WarningInFunction WarningIn(FUNCTION_NAME)
#endif

// As per OpenFOAM-v1912
//
//- Report an error message using Foam::FatalError
//  about unknown lookup type in table
#ifndef FatalErrorInLookup
#define FatalErrorInLookup(lookupTag, lookupName, lookupTable)                 \
    ::Foam::FatalError(FUNCTION_NAME, __FILE__, __LINE__)                      \
    << "Unknown " << (lookupTag) << " type " << (lookupName)                   \
    << "\n\nValid " << (lookupTag) << " types :\n"                             \
    << ((lookupTable).sortedToc()) << '\n'
#endif


//- Report an error message using Foam::FatalIOError
//  about unknown lookup type in table
#ifndef FatalIOErrorInLookup
#define FatalIOErrorInLookup(ios, lookupTag, lookupName, lookupTable)          \
    ::Foam::FatalIOError(FUNCTION_NAME, __FILE__, __LINE__, (ios))             \
    << "Unknown " << (lookupTag) << " type " << (lookupName)                   \
    << "\n\nValid " << (lookupTag) << " types :\n"                             \
    << ((lookupTable).sortedToc()) << '\n'
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
