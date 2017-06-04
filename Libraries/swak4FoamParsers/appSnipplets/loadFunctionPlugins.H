/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2005 OpenCFD Ltd.
    \\/      M anipulation   |
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

Description
    Load function plugins if specified on the command line

Contributors/Copyright:
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

if(args.options().found("functionPlugins")) {
    Info << "\nLoading additional function plugins" << endl;

    string theOption(args.options()["functionPlugins"]);
    std::string::size_type start=0;
    std::string::size_type end=0;

    while(start<theOption.length()) {
        end=theOption.find(',',start);
        if(end==std::string::npos) {
            end=theOption.length();
        }
        word name(
            theOption.substr(
                start,end-start
            )
        );
        start=end+1;

        fileName theLib("libswak"+name+"FunctionPlugin.so");
        Info << "  Loading " << theLib << endl;

#ifdef FOAM_DLLIBRARY_USES_STATIC_METHODS
        dlLibraryTable::open(theLib);
#else
        table.open(theLib);
#endif
    }

}

// ************************************************************************* //
