/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description
    It is not a good idea to reuse through #include. But this is the easiest way
    to not duplicate these things between Python 2 and 3

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

namespace Foam {

    template<class T>
    inline PyObject *makePrimitiveElement(const T &el) {
        notImplemented(string("makePrimitiveElement ")+string(pTraits<T>::typeName));
        return NULL;
    }

    template<>
    inline PyObject *makePrimitiveElement(const label &el) {
        return PyInt_FromLong(el);
    }

    template<>
    inline PyObject *makePrimitiveElement(const scalar &el) {
        return PyFloat_FromDouble(el);
    }

    template<>
    inline PyObject *makePrimitiveElement(const bool &el) {
        return PyBool_FromLong(el);
    }

    template<>
    inline PyObject *makePrimitiveElement(const word &el) {
        return PyString_FromString(el.c_str());
    }

    template<>
    inline PyObject *makePrimitiveElement(const string &el) {
        return PyString_FromString(el.c_str());
    }

    class checkBoolType {
    public:
        bool operator()(PyObject *el) {
            return PyBool_Check(el);
        }
    };

    class checkStringType {
    public:
        bool operator()(PyObject *el) {
            return PyString_Check(el);
        }
    };

    class checkIntType {
    public:
        bool operator()(PyObject *el) {
            return PyInt_Check(el);
        }
    };

    class checkFloatType {
    public:
        bool operator()(PyObject *el) {
            // scalar val=PyFloat_AsDouble(el);
            // PyObject *err=PyErr_Occurred();
            // return !err;
            return PyNumber_Check(el);
        }
    };

    class checkWordType {
    public:
        bool operator()(PyObject *el) {
            if(!PyString_Check(el)) {
                return false;
            }
            string val(PyString_AsString(el));
            return Foam::generalInterpreterWrapper::ValidWord()(val);
        }
    };

    template<class FCheck>
    bool isList(PyObject *list) {
        Py_ssize_t size=PySequence_Length(list);
        for(Py_ssize_t i=0;i<size;i++) {
            PyObject *el=PySequence_GetItem(list,i);
            if(el==NULL) {
                return false;
            }
            bool ok=FCheck()(el);
            Py_DECREF(el);
            if(!ok) {
                return false;
            }
        }
        return true;
    }

    template<class FCheck>
    bool isListList(PyObject *list) {
        Py_ssize_t size=PySequence_Length(list);
        for(Py_ssize_t i=0;i<size;i++) {
            PyObject *el=PySequence_GetItem(list,i);
            if(el==NULL) {
                return false;
            }
            bool ok=PySequence_Check(el);
            if(ok){
                ok=isList<FCheck>(el);
            }
            Py_DECREF(el);
            if(!ok) {
                return false;
            }
        }
        return true;
    }

    class getBoolValue {
    public:
        bool operator()(PyObject *el) {
            return el==Py_True;
        }
    };

    class getWordValue {
    public:
        word operator()(PyObject *el) {
            return word(PyString_AsString(el));
        }
    };

    class getStringValue {
    public:
        string operator()(PyObject *el) {
            return string(PyString_AsString(el));
        }
    };

    class getIntValue {
    public:
        label operator()(PyObject *el) {
            return PyInt_AsLong(el);
        }
    };

    class getFloatValue {
    public:
        scalar operator()(PyObject *el) {
            scalar val=PyFloat_AsDouble(el);
            PyObject *err=PyErr_Occurred();
            if(err) {
                WarningIn("getFloat()")
                    << "Not a float (probably 2D numpy)"
                        << endl;
                PyErr_WriteUnraisable(err);
            }
            return val;
        }
    };

    template<class FConv,class T>
    autoPtr<List<T> > getList(PyObject *list)
    {
        Py_ssize_t size=PySequence_Length(list);
        autoPtr<List<T> > val(new List<T>(size));

        for(Py_ssize_t i=0;i<size;i++) {
            PyObject *el=PySequence_GetItem(list,i);
            if(el==NULL) {
                continue;
            }
            val()[i]=FConv()(el);
            Py_DECREF(el);
        }
        return val;
    }

    template<class FConv,class T>
    autoPtr<List<List<T> > > getListList(PyObject *list)
    {
        Py_ssize_t size=PySequence_Length(list);
        autoPtr<List<List<T> > > val(new List<List<T> >(size));

        for(Py_ssize_t i=0;i<size;i++) {
            PyObject *el=PySequence_GetItem(list,i);
            if(el==NULL) {
                continue;
            }
            val()[i]=getList<FConv,T>(el)();
            Py_DECREF(el);
        }
        return val;
    }

}
