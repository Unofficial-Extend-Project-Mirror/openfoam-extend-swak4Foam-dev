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
