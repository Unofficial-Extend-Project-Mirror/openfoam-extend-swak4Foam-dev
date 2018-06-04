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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SWAK_PTRAITS_SPECIALIZATION_H
#define SWAK_PTRAITS_SPECIALIZATION_H

namespace Foam {

// I have NO idea why this is necessary, but since the introduction of
// the enable_if_rank0-stuff the output-methods in ExpressionResult.C
// do not compile without it. Seems like certain compilers (Intel and
// Fujitsu need it in the header)

template<>
class pTraits<token::punctuationToken>
{};

template<int N>
class pTraits<char [N]>
{};

template<>
class pTraits<Ostream&(Ostream&)>
{};

template<>
class pTraits<char>
{};

template<>
class pTraits<const char *>
{};


} // namespace

#endif

// ************************************************************************* //
