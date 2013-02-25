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

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "timelineFunctionObject.H"
#include "IOmanip.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {
/*---------------------------------------------------------------------------*\
                           Class timelineFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template<>
inline void timelineFunctionObject::writeData(const word &name,const UList<string> &data) {
    Ostream &s=*filePtrs_[name];
    forAll(data,index) {
        switch(outputFileMode_) {
            case ofmFoam:
            case ofmRaw:
                s << setw(IOstream::defaultPrecision() + 7) << data[index].c_str();
                break;
            case ofmCsv:
                s << separator_.c_str() << data[index].c_str();
                break;
            default:
                FatalErrorIn("timelineFunctionObject::write(const word &name,const UList<T> &data)")
                    << "Don't know how to write files of type " 
                        << outputFileModeNames_[outputFileMode_] << endl
                        << exit(FatalError);
        }
    }
}

template<>
inline void timelineFunctionObject::writeData(const word &name,const UList<scalar> &data) {
    Ostream &s=*filePtrs_[name];
    forAll(data,index) {
        switch(outputFileMode_) {
            case ofmFoam:
            case ofmRaw:
                s << setw(IOstream::defaultPrecision() + 7) << data[index];
                break;
            case ofmCsv:
                s << separator_.c_str() << data[index];
                break;
            default:
                FatalErrorIn("timelineFunctionObject::write(const word &name,const UList<T> &data)")
                    << "Don't know how to write files of type " 
                        << outputFileModeNames_[outputFileMode_] << endl
                        << exit(FatalError);
        }
    }
}

template<class T>
void timelineFunctionObject::writeData(const word &name,const UList<T> &data) {
    Ostream &s=*filePtrs_[name];
    forAll(data,index) {
        if(outputFileMode_==ofmFoam) {
            s << setw(IOstream::defaultPrecision() + 7) << data[index];
        } else {
            for(label i=0;i<pTraits<T>::nComponents;i++) {
                switch(outputFileMode_) {
                    case ofmRaw:
                        s << setw(IOstream::defaultPrecision() + 7) << data[index][i];
                        break;
                    case ofmCsv:
                        s << separator_.c_str() << data[index][i];
                        break;
                    default:
                        FatalErrorIn("timelineFunctionObject::write(const word &name,const UList<T> &data)")
                            << "Don't know how to write files of type " 
                                << outputFileModeNames_[outputFileMode_] << endl
                                << exit(FatalError);
                }
            }
        }
    }
}

template<class T>
void timelineFunctionObject::writeTime(const word &name,const T &time) {
    Ostream &s=*filePtrs_[name];
    switch(outputFileMode_) {
        case ofmFoam:
        case ofmRaw:
            s << setw(IOstream::defaultPrecision() + 7) << time;
            break;
        case ofmCsv:
            s <<  time;
            break;
        default:
            FatalErrorIn("timelineFunctionObject::writeTime(const word &name,const UList<T> &data)")
                    << "Don't know how to write files of type " 
                        << outputFileModeNames_[outputFileMode_] << endl
                        << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// ************************************************************************* //
