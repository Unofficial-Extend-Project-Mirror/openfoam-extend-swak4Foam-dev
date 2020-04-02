# Description

A collection of libraries and tools that let the user handle
OpenFOAM-data based on expressions


# About this document


## Scope

This file gives an overview of `swak4Foam` and a history of the
features. It is not a canonical documentation.


## Technical

This file is written in the mighty `org-mode` (see
<http://orgmode.org/>) a markup/outline-mode for (X)Emacs. Using this
mode it can be easily (using 3 keystrokes &#x2026; it's Emacs) to PDF or
HTML to make it more readable (and add a table of contents).

Please don't try to "beautify" it with any other text editor as
this will surly mess up the markup (and keeping the file
*org-compatible* outside of the `org-mode` is a pain in the neck.

The Markdown-variant `README.md` is only there to be displayed in a
human-readable form by the web-interface to `mercurial` at
`sourceforge.net`. It is generated from `REAME` and only updated
during releases. **Don't edit it**.


# Contributors etc


## Original Author

Bernhard Gschaider (bgschaid@hfd-research.com)


## Current Maintainer

Bernhard Gschaider (bgschaid@hfd-research.com)


## Contributors

In alphabetical order of the surname

-   **Martin Beaudoin:** `trackDictionary` in `simpleFunctionObjects`
-   **Martin Becker:** The `potentialPitzDaily`-case (demonstrating a
    problem with `groovyBC`)
-   **Oliver Borm:** `patchMassFlowAverage` in `simpleFunctionObjects`
-   **Peter Keller:** `sprinklerInlet`-case
-   **Martin Kroeger:** `mtv` surface writer in `simpleFunctionObjects`
-   **Andreas Otto:** fixed the `circulatingSplash`-case
-   **Alexey Petrov:** `pythonFlu`-integration
-   **Bruno Santos:** -   Compilation with Intel compiler and Mingw
    -   Rewrite of `mybison` and `myflex` to allow parallel compilation
        with `WM_COMPPROCS`
    -   Generation of dependencies for files that need Bison generated
        tab.hh files. This allows not having race conditions during
        parallel compilation
-   **Hrvoje Jasak:** Fixes to compile on Intel and CLang
-   **E.David Huckaby:** Add the writing of particles to `writeFieldsOften`
-   **Alexey Matveichev:** -   release generation script.
    -   Automatic `swakConfiguration`
-   **Mark Olesen:** -   port to OpenFOAM+ since v1612
    -   improvements to scripts
-   **Philippose Rajan:** -   Bugfix for segmentation faults in parallel
-   **Matti Rauter:** -   Spell-checking the compilation scripts

If anyone is forgotten: let me know

According to the commits in the `mercurial`-repository (and the
repositories of the projects from which swak emerged) contributors
are (ordered by the year of their first contribution. EMail is the
latest under which this author submitted):

-   2006-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
-   2008 Hannes Kroeger (hannes@kroegeronline.net)
-   2008-2009, 2012 Martin Beaudoin, Hydro-Quebec (beaudoin.martin@ireq.ca)
-   2010 Marianne Mataln <mmataln@ice-sf.at>
-   2010 Oliver Borm (oli.borm@web.de)
-   2011 Alexey Petrov <alexey.petrov.nnov@gmail.com>
-   2011 Petr Vita <petr.vita@unileoben.ac.at>
-   2012-2016 Bruno Santos <wyldckat@gmail.com>
-   2013 Georg Reiss <georg.reiss@ice-sf.at>
-   2014 Hrvoje Jasak <h.jasak@wikki.co.uk>
-   2014 David Huckaby <e.david.huckaby@netl.doe.gov>
-   2015 Domink Christ <d.christ@wikki.co.uk>
-   2015 Alexey Matveichev <alexey.matveichev@gmail.com>
-   2016-2018 Mark Olesen <Mark.Olesen@esi-group.com>


## Documentation

See: <http://openfoamwiki.net/index.php/contrib/swak4Foam>


# Installation/Compilation

If everything goes well then

    ./AllwmakeAll

should compile the essential requirements before compiling
`swak4Foam` itself (remember: there is an **if** in that sentence)


## Requirements

-   Version 2.0 or higher of OpenFOAM and version 3.0 or higher of
    Foam.

    The oldest version that this is release has been tested with is
    OpenFOAM 2.3

    To see which versions this release has been tested with see
    `Documentation/examplesCompatibilityMatrix.org`
-   the compiler generators `bison` and `flex`
    -   **bison:** `swak4Foam` is known to work with `bison` version 3.3 and
        higher.
    -   **flex:** since the introduction of the plugin functions at least
        a flex version of `2.5.33` is required (`2.5.35` is the
        lowest **confirmed** version)

Both of these are mainstream packages (they are for instance needed
to compile `gcc`) and should exist on every Linux distribution. Use
the package manager of your distribution to install them and **only**
if the compilation process of `swak4Foam` complains about too low
or too high versions compile special version. This can be easily
done by calling

    ./maintainanceScripts/compileRequirements.sh

in the directory of the sources. This will download a fitting
`bison` version, compile it and install it in such a way that it is
**only** used during the compilation of `swak4Foam`.

`swak4Foam` tries to keep the requirements on these as low as
possible and sometimes lower versions than the ones reported **may**
work. If they do please report so.

The version of bison can be checked with

    bison -V

The version of flex with

    flex -V


### Scripting languages

If some scripting language integration should be used then the
appropriate development packages for that language must be
installed (something like `python-devel`) for Python. This differs
on the various platforms. The settings for your machine read from
the file `swakConfiguation` if it is present. The file
`swakConfiguation.automatic` tries to guess these settings so
either copy it with

    cp swakConfiguation.automatic swakConfiguation

and modify it or link it

    ln -s swakConfiguation.automatic swakConfiguation

but be aware that if modifying it the original is modified.

Because of its small footprint and to avoid problems on
distributions where it is outdated `Lua` is automatically compiled
by the `maintainanceScripts/compileRequirements` script and
`swakConfiguration.automatic` uses this version if found. It may
be necessary to install the development package of `readline` for
this

The script will also install `luarocks` (the Lua package manager)
and using that `luaprompt`. The `luaprompt`-library will be used
for interactive interactions with the Lua-integration (making it
more comfortable)


## Building

    wmake all

at the base directory should build all the libraries and
tools.

Rerun the command to make sure that there was no problem with the
compilation (this should be quite fast and only report libraries
being created and some administrative stuff)


### Additional configuration

Some features (currently only the `Python`-integration may need
third party software. The paths to these packages can be
configured in a file `swakConfiguration` (an example file
`swakConfiguration.example` is provided. There is also a file
`swakConfiguration.automatic` that tries to automatically
determine the location of Python. It does not work on all systems
and therefor is not enabled by default). If that file is not
present these unconfigured features will not be compiled.

Environment variables that can be set in this file are:

-   **SWAK<sub>PYTHON2</sub><sub>INCLUDE</sub>:** Path to the `Python.h` file of the used
    `python`-installation
-   **SWAK<sub>PYTHON2</sub><sub>LINK</sub>:** Options to link the `python`-library to the
    library for the `python`-integration
-   **SWAK<sub>PYTHON3</sub><sub>INCLUDE</sub>:** Path to the `Python.h` file of the used
    `python3`-installation
-   **SWAK<sub>PYTHON3</sub><sub>LINK</sub>:** Options to link the `python3`-library to the
    library for the `python3`-integration
-   **SWAK<sub>LUA</sub><sub>INCLUDE</sub>:** Path to the header files of the used
    `Lua`-installation
-   **SWAK<sub>LUA</sub><sub>LINK</sub>:** Options to link the `Lua`-library to the
    library for the `Lua`-integration
-   **SWAK<sub>USER</sub><sub>PLUGINS</sub>:** A list of paths separated by
    semicolons. These are the directories of libraries with
    function-plugins. They are compiled in the course of the
    normal `swak`-compilation. This makes sure that they are
    consistent with the `swak`-release in the case of an update
-   **SWAK<sub>COMPILE</sub><sub>GRAMMAR</sub><sub>OPTION</sub>:** Additional compiler switches for
    the C-files generated by `bison`. Usually used if the default
    optimization switches are too aggressive and compilation
    takes to long (then a value like `-O0` would for instance
    switch off all optimizations)
-   **SWAK<sub>COMPILE</sub><sub>LEXER</sub><sub>OPTION</sub>:** Additional compiler switches for
    files generated by `flex`. For some compilers (Intel)
    aggressive optimization when compiling these files may cause
    uninitialized memory and crashes. This variable can be used
    to switch off the optimization (usually this fixes the
    problems)
-   **SWAK<sub>TEST</sub><sub>RACE</sub><sub>CONDITIONS</sub>:** Meant to be used only by developers,
    for performing a verification on whether race conditions occur
    during parallel compilation, regarding the grammar parsers.


### Possible compilation failure with old 2.0.x-versions

With older versions of 2.0.x (or 2.0 or 2.0.1) it is possible that
the compilation of  `swakCodedFunctionObject` will fail. In that
case remove the last parameter to the
`codedFunctionObject`-constructor in `swakCodedFunctionObject.C`
(it is clearly marked by a comment)


### Special versions of the python integration

**Note**: This section is very special and won't be needed by most
people

The library `libswakPythonIntegration.so` links against a specific
version of the python library. In certain circumstances (for
instance the OS of the cluster is different from the one that swak
was compiled on) this python-version is not available on the
target machine. While all other parts of swak will work the
python-integration won't load on the target machine. To fix this
problem a workaround is provided:

The script `maintainanceScripts/makeSpecialPythonLibrary.sh`
compiles a special version of the `libswakPythonIntegration.so`
that is identified by an additional suffix (for instance the
suffix `Cluster` makes a file
`libswakPythonIntegrationCluster.so`). That script has to be run
**on the target system** and needs 3 parameters:

1.  `SWAK_PYTHON2_INCLUDE`
2.  `SWAK_PYTHON2_LINK`
3.  The suffix

For instance

    ./maintainanceScripts/makeSpecialPythonLibrary.sh -I/opt/local/Library/Frameworks/Python.framework/Versions/2.5/include/python2.5 "-L/opt/local/lib -lpython2.5" Version2.5

compiles a version for an old python.

To enable the python-integration in `libs` in `controlDict` **both**
python-integrations have to be specified. One of them will fail on
each machine (because the required python-library is not there),
the other one will provide the python-functionObjects.

Note: the special libraries won't be updated automatically on the
target system. Doing so is the responsibility of the user.


## Global installation

If the libraries and utilities are considered stable and the should
be available to everyone (without having to recompile them) the
script `copySwakFilesToSite.sh` in the directory
`maintainanceScripts` can be used to copy them to the
global site-specific directories.

The script `removeSwakFilesFromSite.sh` in the directory
`maintainanceScripts` removes all these files
from the global directories. The `removeSwakFilesFromLocal.sh` does
the same for the user directories (this makes sure that no
self-compiled version *shadows* the global version (which would
lead to strange results)

There is a `Makefile` attached. `make globalinstall` compiles `swak4Foam`
and installs it into the global directories

**Note:** Due to the order in which library direcories are searched
for with `-L` a global installation might break the
compilation. **If you don't know what this means: don't attempt a
global installation**


## Packaging


### Debian

The command `build dpkg` builds a Debian/Ubuntu package for
the **currently enabled** OpenFOAM-package. Note:

-   it is assumed that the currently used OF-version was also
    installed by the package manager
-   the `dev` package is built but poorly maintained

Changes in the packaging should be done in the branch
`debianPackaging` of the Mercurial-repository and later be
merged to the `default`-branch.

Packaging for OpenFOAM 2.x should be done in the branch
`debianPackaging_2.x`

**Note:** Due to the problem described with the global installation
it might be necessary to deinstall a previously installed package
to successfully build a new package


# Contents


## Libraries

Collection of Libraries


### `swak4FoamParsers`

The basis of `swak4Foam`: the expression parsers with the logic to
access the *OpenFOAM* data-structures.

None of the other software pieces compile without it.

Also defines a subclass to `DataEntry` that uses
`swak`-expressions and a function object `initSwakFunctionObject`
that might be used if this fails


### `simpleFunctionObjects`

A collection of function objects that was previously separately
available at
<http://openfoamwiki.net/index.php/Contrib_simpleFunctionObjects>.

Provides consistent output of values (on patches and fields) and
more.


### `groovyBC`

Implements the infamous `groovyBC`. A boundary condition that
allows arbitrary expressions in the field-file


### `swakFunctionObjects`

Function objects that have no additional requirements. Mainly used
for manipulating and creating fields with expressions

-   **addGlobalVariable:** Adds a variable to a global
    swak-namespace. Mainly used for debugging and resolving
    issues where a variable is needed in a BC before it is defined.
-   **expressionField:** Create a new field from an expression
-   **clearExpressionField:** Erase a field created with
    `expressionField`
-   **manipulateField:** Modify a field in memory
-   **createSampledSet:** Create a sampled set that can be used by
    other swak-entities (mainly boundary conditions)
-   **createSampledSurface:** Create a sampled surface that can be used by
    other swak-entities (mainly boundary conditions)
-   **swakCoded:** Child of the `coded`-functionObject that can read
    and write global variables from and to
    swak-namespaces

    Assumes that the `SWAK4FOAM_SRC` environment
    variable is set to the `Libraries`-directory of the
    `swak4Foam`-sources
-   **solveLaplacianPDE:** Solve the Poisson equation

\begin{equation}
\frac{\partial \rho T}{\partial t} - \nabla \lambda \nabla T =
S_{expl} + S_{impl} T
\end{equation}

for \(T\) where \(\rho\), \(\lambda\) and \(S\) can be specified

-   **solveTransportPDE:** Solve the transport equation

\begin{equation}
\frac{\partial \rho T}{\partial t} + \div(\phi,T) - \nabla \lambda \nabla T =
S_{expl} + S_{impl} T
\end{equation}

for \(T\) where \(\rho\), \(\lambda\) and \(S\) can be
specified. Plus the name of the field \(\phi\)

-   **addForeignMeshes:** this allows specifying new foreign
    meshes. These can be used in subsequent function-objects or
    other swak-expressions on `internalFields`
-   **foreignMeshesFollowTime:** this allows specifying a list of
    meshes whose time is set to the current time of the running
    case


### `simpleSwakFunctionObjects`

Function objects based on the `simpleFunctionObjects`-library
(which is a prerequisite for compiling it).

Evaluate expressions and output the results


### `swakSourceFields`

These classes allow to manipulate the solution. To use these the
solver has to be modified.

-   **expressionSource:** Field that is calculated from an
    expression. To be used as source-term or coefficient in some
    solver
-   **forceEquation:** force an equation to fixed values in selected
    locations. Has to be used after constructing
    the equation and before solving

These sources are based on `basicSource` and can be used
**without** a modification of the solver (they are only available in
the 2.x version):

-   **SwakSetValue:** sets values according to a mask or the
    mechanism provided by `basicSource`
-   **SwakExplicitSource:** Uses the calculated source term on the
    right hand side of the equation
-   **SwakImplicitSource:** Uses a calculated scalar-field to add an
    implicit source term (source is **without** the actual field)


### `swakFvOptions`

Starting from OpenFOAM 2.2 this library has additional `fvOptions`


### `swakTopoSources`

`topoSources` for `cellSet` and `faceSet`. Can be used with the
`cellSet` and `faceSet`-utilities


### `swakFiniteArea`

Implements parsers for the `finiteArea`-stuff in 1.6-ext. Also
implements `groovyBC` for `areaField` and `expressionField` and
other function objects


### `groovyStandardBCs`

Collection of boundary conditions that give standard boundary
conditions the possibility to use expression for the coefficients

Contributions to this library are explicitly encouraged. Please
use the Mercurial-branch `groovyStandardBCs` to *groovyify*
standard boundary conditions.


### `simpleLagrangianFunctionObjects`

Function objects that allow the easy addition of lagrangian
clouds to a case. No other libraries from `swak4Foam` are needed
for this


### `simpleSearchableSurfaces`

Additional `searchableSurfaces` (for use in `snappyHexMesh`)
which include boolean operations for other surfaces and
coordinate transformations


### `fluFunctionObjectDriver`

Driver for `functionObjects` that implemented entirely in `Python`
using the `pythonFlu`-library


### `swakLagrangianParser`

Parser for calculating expressions on clouds of lagrangian particles


### `swakStateMachine`

Library with function objects and plugin functions to work with
simple state machines


### `swakDynamicMesh`

Library with classes that combine swak with dynamic meshes


### `functionPlugins`

Directory with a number of libraries with function-plugins:

-   **swakFacSchemesFunctionPlugin:** functions with selectable
    discretization schemes for FAM (only used in 1.6-ext)
-   **swakFvcSchemesFunctionPlugin:** functions with selectable
    schemes for FVM
-   **swakLocalCalculationsFunctionPlugin:** calculations that are
    local to a cell (Minimum of the face values or so)
-   **swakMeshQualityFunctionPlugin:** calculate mesh quality criteria
    like orthogonality, skewness and ascpect ratio
-   **swakRandomFunctionPlugin:** different random number
    distributions. Currently only exponential
-   **swakSurfacesAndSetsFunctionPlugin:** calculates distances from
    `sampledSurfaces` and `sampledSets` and projects calculated
    values from these onto a volume field
-   **swakThermoTurbFunctionPlugin:** Access functions from the
    thermophysical model and the turbulence model in the current
    region. Loads the model only if necessary
-   **swakTransportTurbFunctionPlugin:** Same as above but for
    incompressible models
-   **swakLagrangianCloudSourcesFunctionPlugin:** Functions that get
    informations like source terms from clouds of particles (due
    to technical reasons this works only for the regular
    *intermediate* clouds)
-   **swakVelocityFunctionPlugin:** Functions that work on the flow
    field (currently only the local Courant-number)
-   **swakChemistryModelFunctionPlugin:** Functions that calculate
    chemistry properties (reaction rates etc)
-   **swakRadiationModelFunctionPlugin:** Functions that calculate
    properties of the radiation model
-   **swakMeshWaveFunctionPlugin:** Functions that use the
    `MeshWave`-algorithm to calculate grid-dependent properties
    like number of mesh layers


#### lagrangianCloudAdaptors-directory

Because of the templating all plugin-functions have to be
reinstaniated for new particle classes. The libraries in this
directory

-   reimplement the functions from
    `swakLagrangianCloudSourcesFunctionPlugin`
-   the `CloudProxy` for the `cloud` parser

for special particle classes. These are

-   **coalCloudAdaptor:** the library `libswakCoalCloudAdaptor` that
    handels the `CoalParcel`-class

These libraries have to be included in the `libs`-entry to
be able to handle these libraries


### `languageIntegration`

Libraries that integrate various scripting languages with swak4Foam


### `simpleRegionSolvers`

Library that has function objects that allow solving an equation
independently in a different region


#### `swakGeneralLanguageIntegration`

This library implements a common interface for the actual
language integrations.


#### `swakPythonIntegration`

Embeds a `Python 2`-interpreter.

-   **`pythonIntegrationFunctionObject`:** Executes `Python`-code
    at the usual execution times of functionObjects. The
    interpreter keeps its state

This library is only compiled if the paths to the Python-Headers
are configured in the `swakConfiguration`-file (see above)


#### `swakPython3Integration`

Embeds a `Python 3`-interpreter.


#### `swakLuaIntegration`

Embeds a `Lua`-interpreter.


## Utilities


### `funkySetFields`

Utility that allows creation and manipulation of files with
expressions


### `funkySetAreaFields`

Utility like `funkySetFields` for `areaFields` (only works with
1.6-ext)


### `funkySetBoundaryField`

Sets any field on a boundary to a non-uniform value based on an
expression.

Acts without deeper understanding of the underlying boundary condition


### `replayTransientBC`

Utility to quickly test whether a groovyBC gives the expected
results. Writes the specified fields with the applied boundary
condition but doesn't do anything else.

Can be used for other BCs as well


### `funkyDoCalc`

Evaluates expressions that are listed in a dictionary using data
that is found on the disc and prints summarized data (min, max,
average, sum) to the screen


### `calcNonUniformOffsetsForMapped`

Calculates the `offsets`-entry in the `polyMesh/boundary`-file
according to the specification in a dictionary. Only needed if you
have mapped patches and the regular uniform offset is not enough
for your purposes


### `fieldReport`

Utility that quickly does some quantitative analysis (minimum,
maximum, average etc ) on a field on the disc (internal field but
also patches, sets, zones, &#x2026;)


### `funkyPythonPostproc`

This utility loads specified fields into memory, executes a list
of user-specified function objects whose data is then passed to a
python script which does the user-specified analysis.


### `funkySetLagrangianField`

Utility to calculate fields for a lagrangian cloud (or setting it
up from scratch)


### `writeBoundarySubfields`

Utility to write fields with subfields from boundary conditions
like `refValue` for post-processing


## Examples

If not otherwise noted cases are prepared by a simple
`blockMesh`-call.

**Note**: All the cases here are strictly for demonstration purposes and
resemble nothing from the 'real world'

**Note**: Due to various changes in the case syntax between OpenFOAM
 2.0 and 2.2 (for instance `thermophysicalProperties`, wall
 functions etc) not all of the examples work with all
 OpenFOAM-installations "out of the box". Slight adaptions may be
 necessary

Some of the cases use the `pyFoamPrepareCase.py`-utility of [PyFoam](https://openfoamwiki.net/index.php/Contrib/PyFoam)
to set up the case for the current OpenFOAM-version

To run on multiple OpenFOAM-versions the cases sometimes have more
entries in the dictionaries

Also have a look at `Documentation/exampleCompatibilityMatrix.org`
to see with which versions which cases should work.

Usually the cases have been derived from regular tutorial
cases. After their first implementation they were only adapted to
run in new OpenFOAM versions but improvements in the tutorials are
usually not ported. The reason is that these are just example
cases for `swak4Foam`, not *best practice* examples for OpenFOAM


### groovyBC

The old `groovyBC`-Demos and newer cases that use `groovyBC`


#### pulsedPitzDaily

-   **Solver:** pisoFoam
-   **Also demonstrates:** `manipulateField`, `expressionField` and
    `clearField` from the
    `swakFunctionObjects`. `patchExpression` from
    `simpleSwakFunctionObjects`. `solveLaplacianPDE` and
    `solveTransportPDE` for solving equations,
    `pythonIntegration` with calculations using `numpy`
-   **Preparation:** `pyFoamPrepareCase.py`


#### wobbler

-   **Solver:** solidDisplacementFoam


#### circulatingSplash

-   **Solver:** interDyMFoam
-   **Preparation:** `pyFoamPrepareCase.py`


#### movingConeDistorted

-   **Solver:** pimpleDyMFoam
-   **Also demonstrates:** `swakExpression` with `surface`. Due to a
    problem described below this currently doesn't work


#### average-t-junction

-   **Solver:** pimpleFoam
-   **Mesh preparation:** `pyFoamPrepareCase.sh`


#### delayed-t-junction

-   **Solver:** pimpleFoam
-   **Mesh preparation:** `pyFoamPrepareCase.sh`
-   **Demonstrates:** Delayed variables to simulate an inflow that
    depends on the value of the outflow


#### multiRegionHeaterFeedback

-   **Solver:** chtMultiRegionFoam
-   **Mesh preparation:** Execute the script `prepare.sh` in that
    directory
-   **Also demonstrated:** `patchExpression` and `swakExpression` from
    `simpleSwakFunctionObjects`.


#### fillingTheDam

-   **Solver:** interFoam
-   **Also demonstrates:** Usage of a `sampledSet` defined in the
    `controlDict` do determine the average filling height. Also
    stored variables for not switching back once the criterion
    is reached. Global variables defined by a function object


#### sprinklingInlet

-   **Solver:** interFoam
-   **Description:** Winner of the `swak4Foam`-competition at the 6th
    OpenFOAM-Workshop (2011). By Peter Keller


#### potentialPitzDaily

-   **Solver:** potentialFoam
-   **Description:** Demonstrates the use of `groovyB` with
    `potentialFoam` (also a problem connected with
    that). Provided by Martin Backer


#### lookupDambreak

-   **Solver:** interFoam
-   **Preparation:** run `pyFoamPrepareCase.py`
-   **Description:** Demonstrates the use of 2-dimensional lookup
    tables. For the initialization as well as the boundary
    condition


#### jumpChannel

-   **Solver:** icoFoam
-   **Preparation:** run `pyFoamPrepareCase.py`
-   **Description:** Demonstrates `groovyBC` on cyclic patches and
    the `groovyBCJump`-condition


#### nonBreakingDam

-   **Solver:** interFoam
-   **Preparation:** run `pyFoamPrepareCase.py`
-   **Description:** OpenFOAM macro-expansion inside of expression
    strings


### FunkyDoCalc

Example dictionaries for `funkyDoCalc`


### FunkySetFields

Example dictionary for `funkySetFields`


### FunkySetBoundaryFields

Example dictionary for `funkySetBoundaryFields`. Sets nonsense
boundary conditions for the world famous `damBreak`-case


### ImmersedBC

Cases that demonstrate the immersed boundary conditions of `foam-extend`


### InterFoamWithSources

Demonstrates usage of `expressionSource`

Due to differences in the original `interFoam`-solver this doesn't
work on certain OpenFOAM-versions (most specifically
`1.6-ext`). The current solver works with 2.1. For older
OF-versions use the sources labeled `_pre2.1`.

The only modifications to the original solver are found at the end
of `createFields.H` and in `UEqn.H` (the added source terms).


### InterFoamWithFixed

Demonstrates usage of `forceEquation`

Due to differences in the original `interFoam`-solver this doesn't
work on certain OpenFOAM-versions (most specifically
`1.6-ext`). The current solver works with 2.1. For older
OF-versions use the sources labeled `_pre2.1`.

The only modifications to the original solver are found at the end
of `createFields.H` and in `UEqn.H` (the fixing of the velocities).


#### `interFoamWithSources`

Slightly modified version of `interFoam`. Adds a source term to
the momentum equation. The source term is an expression that is
defined at run-time


#### `mixingThing`

Demonstration case for it.

-   **Preparation:** Run the script `prepare.sh` to prepare the case


### FiniteArea

Demonstration of the `finiteArea`-stuff that works with 1.6-ext


#### `swakSurfactantFoam`

Variation of `surfactantFoam` that adds an `expressionSource`


#### `planeTransport`

Demonstration case

-   **Preparation:** Use `pyFoamPrepareCase.py`
-   **Solver:** `surfactantFoam` (without source term) or
    `swakSurfactantFoam`
-   **Demonstrates:** FAM-specific `swakExpressions` and `groovyBC`
    (as well as the `expressionSource`)


### FvOptions

Cases that demonstrate `swakSourceFields`


#### angleDuctWithSources

-   **Solver:** rhoPimpleFoam
-   **Mesh preparation:** Execute `pyFoamPrepareCase.py .`
-   **Demonstrates:** Simple expression sources (adds a *heat source*
    to the solid)


#### heatExchangerSources

-   **Solver:** chtMultiRegionFoam
-   **Mesh preparation:** Execute `pyFoamPrepareCase.py .`
-   **Demonstrates:** Getting the residuals of the matrix with the
    `matrixChangeBefore` and `matrixChangeAfter`
    `fvOptions`


### other

Cases that don't have a `groovyBC`


#### `angledDuctImplicit`

-   **Solver:** rhoPorousMRFSimpleFoam
-   **Mesh preparation:** Execute `pyFoamPrepareCase.py .`
-   **Demonstrates:** Usage of the `swakTopoSources`. Compares
    different approaches to evaluating with the
    `swakExpression`-functionObject. Also an example dictionary
    that demonstrates the use of `funkyDoCalc`. Demonstrates
    "live" comparing to another case using *foreign meshes*


#### `angledDuctImplicitTransient`

-   **Solver:** rhoPorousMRFPimpleFoam
-   **Mesh preparation:** Execute the `makeMesh.sh`-script in that
    directory. If you want to run in parallel call the
    `decomposeMesh.sh`-script with the number of processors as
    an argument
-   **Demonstrates:** The same as `angledDuctImplicit` but also the
    output of temporal changes


#### `capillaryRise`

-   **Solver:** interFoam
-   **Case preparation:** run the `pyFoam`-utility `pyFoamPrepareCase.py`
-   **Demonstrates:** Usage of a sampled surface to track the
    interface in a VOF-simulation


#### `mixingDam`

-   **Solver:** interFoam
-   **Case preparation:** run the supplied script `prepareCase.sh`
-   **Demonstrates:** Emulate a "moving gravitation" by using the
    `manipulateField`-functionObject to recalculate
    `gh` and `ghf`


#### `counterFlowFlame2DInitialized`

Demonstrates initializing a case with initial conditions that
approximate the real flow.

-   **Solver:** reactingFoam
-   **Case preparation:** run `pyFoamPrepareCase.py`. **Caution:** this
    has to be done **every** time before running the case because
    the initialized fields are rewritten
-   **Demonstrates:** Demonstrates setting calculated initial
    conditions with functionObjects


#### `simpleBendedPipe`

Demonstrates the use of `funkyWarpMesh` and post-processing on
bended pip geometries

-   **Solver:** simpleFoam
-   **Case preparation:** Run `prepare.sh`


#### `topoSetDamBreak`

Case to demonstrate various things about `cell` and `faceSet`

-   **Solver:** interFoam
-   **Case preparation:** Run `pyFoamPrepareCase.py .`


### FromPresentations

Cases that were shown in some presentations


#### OSCFD<sub>cleaningTank3D</sub>

-   **Solver:** interFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** The case described on the slides of the
    talk about `swak4Foam` at the  OSCFD-conference
    2012 in London
-   **Demonstrates:** Boundary conditions, function objects, global
    variables and delayed variables


#### OSCFD<sub>cleaningTank2D</sub>

A 2D-variant of the above case


#### OFW8<sub>sandPitOfCarcoon</sub>

-   **Solver:** twoPhaseEulerFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** Simulate a sand-monster from the StarWars-movie
    "Return of the Jedi"
-   **Demonstrates:** Use of `funkySetFields`, `groovyBC` and
    functionObjects for lagrangian particles


#### OFW8<sub>landspeedersInCanyon</sub>

-   **solver:** simpleFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** Simulates two landSpeeders (as seen in the
    StarWars-movie "A New Hope")
-   **Demonstrates:** Advanced searchableSurfaces (for
    `snappyHexMesh`), functionObject for passive
    scalar, functionObject to calculate
    distributions


### PythonIntegration

Demonstrate the integration of `Python`. Mostly using `PyFoam` but
also with `pythonFlu`


#### `manipulatedPitzDaily`

-   **Solver:** simpleFoam
-   **Demonstrates:** Usage of `PyFoam` to manipulate the
    `fvSolution`-file during the run (possible
    application: unphysical initial conditions
    cause the run to fail during startup with
    "normal" relaxation values)


#### `findPointPitzDaily`

-   **Solver:** simpleFoam
-   **Demonstrates:** Usage of the `pythonFlu`-integration to find
    the point where the recirculation behind the
    step ends. Also tries to plot the result using
    the `matplotlib`-library


#### `bed2OfPisa`

-   **Solver:** twoPhaseEulerFoam
-   **Demonstrates:** Usage of `PyFoam` to read the direction of
    gravity and feeding it into a `goovyBC` via
    global variables
-   **Case preparation:** Just call `funkySetFields -time 0`


#### `multiRegionHeaterBuildFunctionObjects`

-   **Solver:** chtMultiRegionFoam
-   **Demonstrates:** Building the specification of function objects
    at run-time via a Python-script


#### `slicedPitzDaily`

-   **Solver:** simpleFoam
-   **Demonstrates:** Using a python-script to dynamically generate
    multiple function objects (sampled
    surfaces). Using stack-variables to calculate
    the results and write them. Using a stored
    stack-variable to monitor the pressure at a
    point and stop the run if the pressure didn't
    change there for the last 50 iterations


### `CodeStream`

Demonstrates working together with the `coded`-stuff in OpenFOAM 2.0


### solvePDE

Examples for the `functionObjects` that can solve *Partial
Differential equations*


#### flangeWithPDE

-   **Solver:** laplacianFoam
-   **Demonstrates:** The usage of the `functionObject` that solves
    the laplacian (Poisson) equation and
    (hopefully) that it gets the same result as the
    native solver
-   **Case preparation:** `pyFoamPrepareCase.py`


#### pitzDailyWithPDE

-   **Solver:** scalarTransportFoam
-   **Demonstrates:** Solving additional transport equations


### BasicSourceSubclasses

These examples test the source terms based on `basicSource`. They
only work with OpenFOAM 2.x and all use the `simpleFoam`-solver


#### pitzDailyWithSwirl

-   **Demonstrates:** Fixing the values of the velocity in a region
    with `SwakSetValues`


#### pitzDailyWithExplicitPoroPlug

-   **Demonstrates:** Implementing a simple porous plug by adding the
    Darcy-term as a source term with
    `SwakExplicitSource`


#### pitzDailyWithImplicitPoroPlug

-   **Demonstrates:** Same as `pitzDailyWithExplicitPoroPlug` but
    with an implicit source term with
    `SwakImplicitSource`


### Lagrangian

Stuff that has to do with lagrangian particles


#### functionObjects

Tests for the functionObjects that create and evolve a cloud of
particles (library `simpleLagrangianFunctionObjects`)

1.  hotStream

    -   **Solver:** `replayTransientBC`
    -   **Mesh preparation:** `pyFoamPrepareCase.py .`
    -   **Demonstrates:** 3 clouds (kinematic, reacting,
        solidParticle). Loading of a thermophysical
        model with a functionObject. Plugin functions
        for information about the clouds

2.  angledDuctWithBalls

    -   **Solver:** rhoPimpleFoam
    -   **Demonstrates:** Thermo-cloud. Functions for lagrangian
        particles

3.  icoFoamCavityWithParcel

    -   **Solver:** icoFoam
    -   **Demonstrates:** Simplest way to add particles to a case


#### parser

Testing the `cloud`-parser for lagrangiant particles

1.  parcelInBoxWithExpressions

    -   **Solver:** reactingParcelFoam
    -   **Demonstrates:** Adding evaluations on the cloud to a regular
        case

2.  simplifiedSiwek

    Variation of the tutorial case

    -   **Solver:** coalChemistryFoam
    -   **Demonstrates:** creating new clouds with
        `funkySetLagrangianField` and evaluations on
        clouds during the simulation
    -   **Mesh preparation:** run the `pyFoamPrepareCase.py .` script to set up
        the mesh and the fields


### SimpleSurface

Examples for the use of the `simpleSearchableSurfaces`-library.


#### twoMotorbikesAndAWall

-   **Solver:** simpleFoam (alternatively use the
    `caseRun.sh`-script)
-   **Case preparation:** Script `caseSetup.sh` is provided
-   **Demonstrates:** Using the same STL-file more than once in a
    case by applying coordinate
    transformations. **Note:** the physics of this
    case are more than questionable as the frame
    of reference for both motorbikes is wrong


#### littleVolcano

-   **Solver:** interFoam
-   **Case preparation:** Use `pyFoamPrepareCase.py`
-   **Demonstrates:** Boolean operations on STL-files


#### chargedPlates

-   **Solver:** electrostaticFoam
-   **Case preparation:** Script `caseSetup.sh` is provided
-   **Demonstrates:** Boolean operations with regular surfaces

Physics of the case not as expected (charge-distribution)


### manipulateFvSolutionFvSchemes

Demonstrates the use of function objects that change the numerics
during the run


#### pitzDailyTimeSwitched

The regular `simpleFoam`-tutorial. Modified so that it switches
to higher relaxation factors during the run


#### pitzDailyStateMachineSwitched

`simpleFoam`-tutorial. Uses state machines to change the
discretization periodically


### StateMachine

Demonstrate the usage of state machines


#### stateCavity

The case is only an excuse for the state machines

-   **Running:** `replayTransientBC -allowFunctionObjects`
-   **Case setup:** `pyFoamPrepareCase.py`


### DynamicMesh

Examples that demonstrate dynamic mesh applications


#### hecticInletACMI2D

Variation of a tutorial case where the oscillation has been
replaced by an expression

-   **Solver:** `pimpleDyMFoam`
-   **Case setup:** `pyFoamPrepareCase.py`
-   **Demonstrates:** The `swakMotion` function for `solidBodyMotion`


#### rotatingTank2D

-   **Solver:** `interDyMFoam`
-   **Case setup:** `pyFoamPrepareCase.py`
-   **Demonstrates:** The `swakMotion` function for `solidBodyMotion`


#### switchedInletACMI2D

-   **Solver:** `pimpleDyMFoam`
-   **Case setup:** `pyFoamPrepareCase.py`
-   **Demonstrates:** The `groovyCyclicACMI` patch type


#### switchedTJunction

-   **Solver:** `compressibleInterDyMFoam`
-   **Case setup:** `pyFoamPrepareCase`
-   **Demonstrates:** The `groovyCyclicACMI` patch type. State machines. `funkyWarpMesh` utility


### runTimeControl

Demonstrating the run-time trigger


#### simpleSwakCar

Simple modification of the `simpleCar`-case

-   **Solver:** `simpleFoam`
-   **Case setup:** `pyFoamPrepareCase`
-   **Demonstrates:** the run-time trigger with expressions.


### RegionSolvers

Demonstrating the function objects that emulate solvers in
different regions


#### cavityWithHeater

A driven cavity that has to solid heat conduction zones at the
sides with which it interacts

-   **Solver:** `rhoPimpleFoam`
-   **Case setup:** `pyFoamPrepareCase`
-   **Demonstrates:** different heat conductions


### tests

Simple test cases for specific features. The names of the
directories should give you a hint what is tested there. Most of
them don't need a real solver but `replayTransientBC` or
`scalarTransportFoam`.

**Note**: for some features these cases are the only example there
 is. Sorry


#### `randomCavity`

Testing of different seeds for the `rand`-function. Also tests
the `randFixed`-function


#### `AMIPipe`

Simple geometry for testing function objects with AMI. "Execute"
with `replayTransientBC`, `pimpleFoam` or `pimpleDyMFoam`

Features can be "switched off" with `pyFoamPrepareCase.py`. What
can be switched on is a location dependent pressure jump on one
of the interfaces implemented with `groovyBCJumpAMI`


### BugCases

These are cases provided by users to demonstrate bugs. Not
maintained nor documented and may be removed at any time


## `maintainanceScripts`

Undocumented scripts used for maintaining `swak4Foam`. **If you
don't understand them, don't use them**


## Documentation

Additional documentation. Most written in `org-mode`

Current contents:

-   **swak4FoamReference:** Reference manual of swak4Foam. Especially
    concepts like expression syntax and parameters


## releaseTesting

Scripts and configuration to test for a release in a virtual
machine using `vagrant`. Also to be used for packaging


## distroPatches

Patches that have to be applied to releases to get swak4Foam to
work with them. These are only needed to fix problems of the
distributions


## Development

Programs and libraries that are only used for testing/developing
swak


# Bug reporting and Development


## Bug reports

The preferred place for bug reports is
<http://sourceforge.net/apps/mantisbt/openfoam-extend/search.php?project_id=10&sticky_issues=on&sortby=last_updated&dir=DESC&hide_status_id=90>

A sourceforge-account is required for reporting

Please **always** report the FOAM-version you use. As `OpenFOAM-dev`
is a moving target only bug-reports that add fixes are handled for this.


### Things to do before reporting bug

If you're reporting a bug about the compilation please run
`Allwmake` **twice** and only report the messages from the second
run. This makes analyzing the log easier as only the unsuccessful
commands will be reported.

If the problem seems to be a missing library rerun the compilation
to make sure that there wasn't a problem with that.


## Development

Contributions to to `swak4Foam` are most welcome. If you want to
contribute clone the Mercurial archive of the sources

    hg clone http://hg.code.sf.net/p/openfoam-extend/swak4Foam

or use the *Fork* link at
<https://sourceforge.net/p/openfoam-extend/swak4Foam/ci/develop/tree/>
to create your own fork and clone that (there is also copy of the
repository at
<https://bitbucket.org/bgschaid/swak4foam-temporary-replacement-for-original-repo-on>
if you prefer that. But it does not receive pushes as often as the
Sourceforge repository. Please avoid any `git`-clones of the
repository. They are unsupported and contributions through them
will take longer to be incorporated - if at all. The maintainer is
aware that `git` is the Windows of the `DVCS`-world: used because
it is the first thing people are exposed to. Not because of the
technical merit)

Change to the branch that you want to improve (usually `develop`)
and create a new branch

    hg branch <branchName>

where `<branchname>` is an easily identifiable name that makes the
purpose of the branch clear (for instance
`hotfix/WrongRandomFunction` or `feature/HyperbolicFunctions`. For
details see [6.2.2](#org5cf6c3a) below). Don't work on the
`default` branch or any other branches that are not "yours". Such
contributions will not be merged

Once development on the branch is finished you can either issue a
*Pull request* (if you forked on SourceForge or BitBucket) or
export the relevant changesets with

    hg export <nodeID>

(`nodeID` being the ids of "your" changesets) and send them to the
maintainer (or attach them to a bug report on Manits). The changes
will be reviewed and merged into the `develop` branch (do not
attempt to do this yourself) and subsequently into the
release. Patches generated with `hg export` make sure that all
changes are attributed to the original developer (you).

An alternative would be the `bundle` command. Just do

    hg bundle <bundlefile>

and then send the `bundlefile`. This will include **all** commits
that are not in the upstream repository and will allow similar
inclusion in the upstream as `export`.

Only if you got through Mercurial it can be ensured that your
contribution is recognized (if you want to stay anonymous send
patches).

Before submitting the branch please add a description to the
`README` (have a look at the `History`-part below). This will be
merged


### Suggest reading

These topics may be "new" for the average OF-developer:

-   **Mercurial:** A short tutorial on this can be found at
    <http://mercurial.selenic.com/guide/>. If you already
    know `git` the
    <http://mercurial.selenic.com/wiki/GitConcepts> may
    be enough for you
-   **bison/flex:** This pair of compiler generator tools generate the
    parsers for the expressions. Google for a tutorial
    that looks promising to you.

    For a short example that shows how a new function
    was added to two parsers have a look at this
    changeset that added the `cpu()`-function to the
    field and the the patch-parser (usually you'll
    have to write a new method for the driver too):

    hg diff -c 8604e865cce6


<a id="org5cf6c3a"></a>

### Repository organization

The organization of the repository is according to the Driessen
branching model described here
<https://nvie.com/posts/a-successful-git-branching-model/> . To
enforce it this mercurial extension is recommended
<https://bitbucket.org/yujiewu/hgflow/wiki/Home>
For instance if a new feature `foo` should be added then a command

    hg flow feature start foo

creates a new branch `feature/foo` from the develop branch which
later can be merged back with

    hg flow feature finish


#### Special branches

Currently the main branches are:

-   **default:** The **main** branch. This is the brancht that the
    general public will receive. It compiles under
    OpenFOAM 2.0 and higher
-   **debianPackaging:** Branch for generating new Debian-packages of
    `swak4Foam`. If somebody wants to "inherit" this: contact the
    maintainer
-   **develop:** Actual development branch


### Maintaining feature and hotfix-branches

The repository comes with a `.hgflow`-file that is set for the
`hgflow`-extension found at
<https://bitbucket.org/yujiewu/hgflow/wiki/Home> (there are multiple
branches of this extension. This **seems** to be the most up-to date
and still under active development)

In the future this repository will try to stick to the model
described in <http://nvie.com/posts/a-successful-git-branching-model/>


# Copyright

`swak4Foam` is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.  See the file COPYING in this directory,
for a description of the GNU General Public License terms under which
you can copy the files.


# Known bugs

The following list is not complete. If the Artemis extension (see
above) is installed then

    hg ilist

gives a more up-to-date list


## Moving meshes and `sampledSurfaces`

It seems that with moving meshes `sampledSurfaces` don't get
updated. This seems to be a problem with OpenFOAM itself (the
regular `surfaces`-functionObject doesn't get updated. This is
currently investigated


## Missing support for interpolation and point-Fields

Apart from patches and internal fields the support for
interpolation from cells to faces (and vice versa) is incomplete as
well as point fields (although they are supported in the grammar)


## Caching of loaded fields not working

This is especially evident for the `funkyDoCalc`-example


## Possible enhancements of the code

Not really bugs, but stuff that bugs me


### Pointers in the driver code

This is necessary because of `bison`. Investigate possibilities to
replace these by `tmp` and `autoPtr`


## Possible memory loss

`valgrind` reports some lost memory for stuff that is not directly
allocated by `swak4Foam` (in OpenFOAM-sources)

Will investigate. Relevant places are marked by comments in the
code. Also the construction of `sampledSet` seems to loose memory

Currenly problematice parts seem to be:

-   update and `magSf`-calculation of `SampledSurface` (marked in
    `SampledSurfaceExpressionDriver.C`)
-   Python interpreter (but no calls in swak were found in the
    stack-trace that could be responsible)


## Non-treatment of the inner product `&` of symmetric tensors

Before OpenFOAM 2.1 the inner product of two symmetric tensors was
a symmetric tensor. Since 2.1 it is a general tensor. As the
general treatment in the grammar would be confusing currently the
this product was removed from the grammar and therefor will not be
correctly parsed


## No point-vector construction for Subsets

The same problem that was mentioned in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=130>
is also true for subsets. But as the interpolation is not
implemented for most subsets this will be postponed


## No tab-completion for regular Python-shell and old IPython-versions

The tab-completion does not work except for up-to-date versions of
IPython. This seems to be a problem with the `readline`-library
inside an embedded Python. Low priority


## Failure of `groovyBC` for `commsType` `blocking`

If the `commsType` is set to `blocking` then the MPI-communications
of `groovyBC` cause problems in MPI and a fatal error.

Currently the only workaround is to edit
`$WM_PROJECT_DIR/etc/controlDict` so that a different `commsType`
is used (`nonBlocking` or `scheduled`)


## Failure with `sampledSurfaces` that change size on certain distros

Older Foam-versions had the problem that when the surface changes
the size then certain values (especially the `mag` are not properly
recalculated).

This causes this bug:
<https://sourceforge.net/p/openfoam-extend/ticketsswak4foam/231/>

The bug can not be fixed in `swak4Foam` but has to be fixed in the
Foam-Distro


## `solverPerformanceToGlobalVariables` only works for scalar fields

Currently only the data from scalar fields can be correctly
parsed. If `vector`-fields are specified the function object fails


## Failing `Python2` and `Python3` integration if Floating Point Exception is enabled

On some platforms if floating point exceptions are enabled then
importing `numpy` fails because it seems to use FPEs to detect
properties of the floating point implementation. This makes the
whole program fail.

Only known workaround is switching FPE-trapping off by

    export FOAM_SIGFPE=false


# History


## 2010-09-13 - version number : 0.1

First Release


## 2010-12-18 - version number : 0.1.1

New release
Most important changes


### Parser for `sampledSurfaces`

Now expressions for the field on a sampled surface can be
evaluated. All `sampledSurfaces` offered by OpenFOAM now can be
used


### Multiline `variables`

The `variables` entry (most notably used in `groovyBC` and
`swakExpression`) now can be a list of strings. This allows some
kind of "formatting" (one expression per line) and should improve
readability


### Two maintainance-scripts were added

These can copy the libraries and utilities to the global
installation (for those who think that the swak4Foam-stuff is
stable enough and want to 'bless' all users at their site with
it). Note that any local installation still takes precedence
(because `$FOAM_USER_APPBIN` is before `$FOAM_APPBIN` in the
`$PATH`


### Parsers using 'external variables' are now run-time selectable

This allows the inclusion of other parsers with the regular
`swak4Foam` parsers and include them seamlessly with the
`variables`-mechanism for 'externals' (in other words: you can add
your own parser in a separate library without having to change
anything about the overall `swak4Foam`, but it behaves as if it
was part of it)


## 2011-01-30 - version number : 0.1.2


### Support for *Finite Area*-stuff

Now there is support for the `finiteArea`-library found in
1.6-dev. The support is found in a separate library
`swakFiniteArea`. It has

-   a parser `faField` for `areaFields`
-   a parser `faPatch` for patches of `areaFields`
-   a variant of `groovyBC` for these patches
-   a computed source `faExpressionSource`
-   Function-object-variants for `areaFields`: `clearExpression`,
    `expressionField` and `manipulateField`. These work the same as
    their `volField`-counterparts


### Bugfix for compiling in single precision

See <https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=49>


### New function `nearDist`

See <https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=44>


## 2011-04-20 - version number : 0.1.3

New features and changes are (this list is not complete):


### New utility `funkySetAreaField`

Like `funkySetFields` for *finiteArea*. Also writes out a volume
field for easier post-processing


### Refactoring of `groovyBC` and groovified boundary conditions

Makes it easier to use the `groovyBC`-machinery for other boundary
conditions. Two standard boundary conditions were
*groovified*. Others may follow


### Easier deployment

If no `simpleFunctionObjects` are present they can be downloaded
by a script.
Also scripts to handle global installations of `swak4Foam`


### Force equations

A class to force equations to certain values according to
expressions


### New utility `funkyDoCalc`

Utility does calculations on saved results and outputs single
numbers (min, max, sum, average) to the terminal. Can be used for
reporting or validity checks


### Debian packaging

Crude packaging for Debian


### Lookup-tables

A single-argument function can be specified as a piecewise linear
function. Basically works like timelines but the argument can be
something else (not only the time)


### Stored variables

Variables that store their values between time-steps. Applications
are statistics or switches


### Sampled sets

Sampled sets can now also be used as en entity on which
calculation is possible.


## 2011-07-26 - version number : 0.1.4


### Port to OpenFOAM 2.0

This is the first release that officially supports OpenFOAM 2.0

Also it is the first release that incorporates the
`simpleFunctionObjects`-library


### New features:


#### Rewrite of `rand` and `randNormal`

-   These two functions now can receive an integer seed that
    determines the pseudo-randooom sequence generated by  these functions
-   Two functions `randFixed` and `randNormalFixed` were
    added. While the usual `rand` functions generate a different
    result at every time-steps for these functions the pseudo-random
    sequence is determined **only** by the seed (not by the timestep)


#### Binary `min` and `max`

Take the bigger/smaller of two fields. Helps avoid `?:`-operations


#### Allow writing of only volume-fields in `funkySetAreaFields`

Application: If the results of the calculation are only needed in
ParaView


#### Internal changes

-   Use `autoPtr` for sets
-   Update sets that change in memory or on disc


### Bug-fixes


#### `funkySetAreaFields` did not check for the correct fields

Fixed by Petr Vita


#### `surfaceProxy` uses the wrong geometric data


#### Avoid floating point exceptions with division of fields

Calculated boundaries were \(0\) and caused a division by zero


### Packaging


#### Update Debian packaging

-   Packaging information for the currently used OF-version is
    generated (allows a separate `swak`-package for every OF-version
-   Submission to launchpad added


#### Deployment scripts

Now install to `FOAM_SITE_APPBIN/LIBBIN`


## 2011-10-03 - version number : 0.1.5


### New features


#### `replayTransientBC` now supports multiple regions

Uses the usual `-region`-option. Looks for a separate dictionary
in the `system`-directory of that region


#### `replayTransientBC` allows execution of functionObjects

This can be switched on using the `allowFunctionObjects`-option


#### `Python`-embedding

Allows the execution of Python-Code in a functionObject

This feature is still experimental and the organization of the
libraries is subject to change


#### Global variables

It is now possible to define variables that are 'global': They
can be read in any entity.

Currently these variables can only be uniform.

To access global variables the specification-dictionary has to
have a `wordList` named `globalScopes`. The scopes are searched
in that order for the names of global variables. Having scopes
allows some kind of separation of the variables


#### Using OF 2.0 codeStreams

Adds a functionObject `swakCoded` that extends the
`coded`-functionObject to read and write global variables


#### Simplified boundary condition `groovyBCFixedValue`

Added a boundary condition than allows to only fix the
values. This should help to avoid problems with cases that don't
like `mixed` (on which the regular `groovyBC` is based)


#### Function objects to solve PDEs

Two function objects that solve *Partial Differential Equations*
during a run have been added:

-   one that solves a laplacian (Poisson) equation
-   one that solves the transport equation for a scalar

The relevant coefficients (including explicit and implicit source
terms) can be specified using expressions


### Administrative and packaging


#### Inject `swak4Foam` into a distro

Added a script that takes the current sources, copies them into
the appropriate places of a `OpenFOAM`-installation and slightly
rewrites them to compile in this place. What happens then
(committing them into the repository or just plain compilation)
is up to the maintainer


#### Absorb `simpleFunctionObjects`

As many parts of `swak4Foam` depend on it the
`simpleFunctionObjects` have now been absorbed into
`swak4Foam`. They can still be compiled on their own


### Bugfixes


#### Variables not working for parallel computations

If a variable is defined and the patch which it is defined on
doesn't have any faces the variable is reported on that processor
as *not existing* and the whole run fails


## 2012-01-04 - version number : 0.1.6


### Cases changed


#### `circulatingSplash`

Fixed according to a suggestion by Andreas Otto. Now runs again
(used to crash some time-steps into the beginning)


### Infrastructure


#### Check for correct `bison`-version

The `Allwmake`-script now checks for the correct bison-version
(and the existence of `bison`) and fails if it doesn't seem to be
the right one


#### Supply a header with preprocessor-symbols about the used OF-version

To allow distinguishing different OF-versions as discussed in the
bug report
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=114>
the `Allwmake`-script now generates a header file
`foamVersion4swak.H` that defines the symbols
`FOAM_VERSION4SWAK_MAJOR`, `FOAM_VERSION4SWAK_MINOR` and
`FOAM_VERSION4SWAK_PATCH`


### Technical


#### Refactoring of the FieldDriver

-   now can also produce surfaceFields
-   full support of `tensor`, `symmTensor` and `sphericalTensor`


#### Refactoring of the FaFieldDriver

-   now can also produce edgeFields

No support for tensors yet


#### Writing of `storedVariables`

If necessary (for instance `swakExpression`-functionObject) the
`storedVariables` are written to disc (into a subdirectory
`swak4Foam` of the timestep) and are reread at the start. This
allows consistent restarts (for instance if a flow was summed
using the variable) **if the expressions stay the same**.


#### `simpleFunctionObjects` now write vectors and tensors without brackets

Data files can now be written without brackets but each component on
its own. The number of entries in the header is not adjusted


#### A *default mesh* for the drivers exists

For drivers that don't have access to a `fvMesh` a default mesh
exists. This default mesh is defined by the first `fvMesh` that
is used during the construction of **any** driver.

Definition of the default mesh can be forced using the
`initSwakFunctionObject` (see the test case `flowRateAngledDuct`)


### New features


#### General `phi` in `solveTransportPDE`

Due to the refactoring of the `FieldDriver` now `phi` can be
specified by a general expression (instead of 'only' a
field-name)


#### `funkySetFields` now also writes surfaceFields

Can write `surfaceVector` and `surfaceScalar`-Fields. Condition
has to be consistent


#### Function objects now `surfaceField`-aware

`expressionField` and `manipulateField` now can create or modify
`surfaceFields`


#### `funkySetFields` and function objects support tensors

`funkySetFields` and the function objects `expressionField` and
`manipulateField`  now also work with the three tensor-types


#### Extension of the `expressionToFace` `topoSet`

If the expression evaluates to a `surfaceField` then this is used
as a flag whether or not the face is in the `faceSet`. If the
expression evaluates to a `volScalarField` then the old semantic
applies (faces are in the set if one cell is `true` and the other
is `false`).

This only works for internal faces


#### `addGlobalVariable` allows setting more than one value

If there is an entry `globalVariables` then this dictionary is
used to set the variables


#### Function object `calculateGlobalVariables`

Calculates variables and then pushes them to a global namespace


#### Generate a dummy `phi` in `replayTransientBC`

New option added that generates a `phi` field with value \(0\) to
keep boundary conditions like `inletOutlet` happy


#### Function object to dump expression results

The functionObject `dumpSwakExpression` dumps the complete
results of a swakExpression to file at each timestep. This
produces huge files and is therefor not endorsed


#### Additional options for `funkySetFields`

Add the options `allowFunctionObjects` and `addDummyPhi` to
execute functionObjects and add a `phi`-field (for fields that
require these)


#### Boundary condition `groovcBCDirection`

Based on the `directionMixed` boundary condition this allows to
set a boundary condition as a Dirichlet-condition only in certain
directions while in the other directions it is a
gradient-condition


#### Boundary condition `groovyBCJump`

Boundary condition that imposes a jump in the value on a cyclic
boundary condition pair (based on `jumpCyclic`). Only works for
scalar values


#### `simpleFunctionObjects` write CSV-files

Setting the option `outputFileMode` to `csv` writes
CSV-files. The option-value `foam` is the default (old style). The
option-value `raw` writes the values delimited by spaces (no
brackets for vectors and tensors)


#### Submeshes automatically read if `searchOnDisc` specified

If a submesh is not yet in memory and the option `searchOnDisc`
is set, the mesh is automatically read into memory and kept there


#### Conditional `functionObjects`

The `simpleFunctionObjects`-library now has a number of
functionObjects that allow the conditional execution of a list of
function objects.

These are

-   **executeIfExecutableFits:** if the name of the executable fits a
    regular expression the function objects are executed
-   **executeIfObjectExists:** if a named object exists (or
    alternatively: doesn't exist) in the registry execute the
    function objects. Type checking also implemented
-   **executeIfEnvironmentVariable:** execute if an environment
    variable satisfies a certain condition (exists, doesn't
    exist, fits a regular expression)
-   **executeIfFunctionObjectPresent:** execute if a specific
    functionObject is present. This can help prevent failures if
    a functionObject is missing for technical reasons

In addition the `simpleSwakFunctionObjects`-library has

-   **executeIfSwakObject:** Evaluates a logical swak-expression. The
    results are either accumulated using logical *or* (if **one**
    value is true the result will be true) or logical *and* (all
    values have to be true)

The `pythonIntegration`-library has

-   **executeIfPython:** Evaluates a Python-code-snipplet that
    returns a value. If this value is "true" in Pythons
    standards then the functionObjects are executed


#### `functionObject` that reads gravitation

`simpleFunctionObjects` has an additional function object that
reads the direction of gravitation. The purpose is to assist
boundary conditions like `buoyantPressure` that rely on it to
work. Best used together with conditional function objects ("If
`g` is missing &#x2026;.")


#### PDE-functionObjects for `finiteArea`

Solve transport and laplacian equation


#### Subclass to `DataEntry` that uses *swak*-expressions

This is defined in the `swak4FoamParsers`-library. The class
needs a default mesh defined to construct the driver. Definition
of the default mesh (if no other driver was constructed in some
function-object or by a `groovyBC`) can be forced using the
`initSwakFunctionObject` (see the test case `flowRateAngledDuct`)


#### `funkySetAreaField` now also writes edgeFields

Similar to the `surfaceFields` in `funkySetFields`


### Bug fixes


#### Compilation with Intel-Compiler possible

The `Utilities` failed with the Intel-compiler. Compilation now
falls back to good old `g++`


#### Access to tensor-components not working

Because the tokens were not defined in the `flex`-files getting
tensor components with `tensor.xx` did not work. Fixed


#### Constants for surfaceFields not working

Because surfaceFields know no `zeroGradient` the template
`makeConstant` did not work


#### `snGrad` does not work for patches if the file is on disc

Change so that the field gets temporarily loaded to calculate the
gradient on the patch. Same for `internalField` and
`neighbourField`


#### `potentialFoam` does not correctly use a `groovyBC`

The reason is that `groovyBC` usually doesn't get evaluated
during construction. The reason is that it is hard to tell
whether all required fields are already in memory. The current
fix is a workaround: setting `evaluateDuringConstruction` to
`true` forces the BC to be evaluated during construction


#### Extra evaluation of boundary conditions causes failure

Extra evaluation of boundary condition that should fix the
problem with `calculated` patches causes `funkySetFields` to fail
with stock boundary conditions if not all fields are present in
memory


### Discontinued features


#### `groovyFlowRateInletVelocity`

This boundary condition will be removed in future releases
because the base class now supports the more general
`DataEntry`-class for which a `swak`-subclass exists


## 2012-04-13 - version number : 0.2.0 Friday the 13th


### New features


#### Region name in `simpleFunctionObject`.outputs

To distinguish the output of various instances of
`functionObjects` from the `simpleFunctionObjects`-library in
multi-region cases the screen-output is prefixed with the region
name. For the default region nothing changes. Directory names
stay the same as they are unambiguous anyway (they contain the
name of the functionObject)


#### Temporal operators `ddt` and `oldTime`

For fields (not expressions!) the value at a previous timestep
can be gotten via `oldTime(field)` if that information exists
(also for `funkySetFields` if the corresponding file `field_0`
exists.

For fields that support it (basically volume-fields) there is
also a `ddt`-operator that calculates the explicit
time-derivative (if information about the last timestep exists)

Currently implemented for

-   **internalFields:** `oldTime` and `ddt`
-   **patch:** only `oldTime`
-   **cellSet,cellZone:** only `oldTime`
-   **sampledSurface,sampledSet:** only `oldTime`
-   **faceSet,faceZone:** `oldTime`
-   **internalFaFields:** `oldTime` and `ddt`
-   **faPatch:** only `oldTime`

If there is no old time value stored and in the parser dictionary
the parameter `prevIterIsOldTime` is set, then the previous
iteration value is used as the old time.


#### Boundary condition `groovcBCDirection`

Based on the `directionMixed` boundary condition this allows to
set a boundary condition as a Dirichlet-condition only in certain
directions while in the other directions it is a
gradient-condition

**Note**: this should have been in the last release but was
 forgotten to merge into the default branch


#### Boundary condition `groovyBCJump`

Boundary condition that imposes a jump in the value on a cyclic
boundary condition pair (based on `jumpCyclic`). Only works for
scalar values

**Note**: this should have been in the last release but was
 forgotten to merge into the default branch


#### Function to return the position of minimum and maximum

The functions `minPosition` and `maxPosition` return the position
of the minimum or the maximum of a scalar field

This is implemented for all field types


#### Support for `pointFields` in the field-parsers

Now can read and generate pointFields.

Detailed features (apart from the standard symbols) are:

-   Function `point` generates a constant `pointScalarField`
-   Function `pts()` returns a `pointVectorField` with the point
    positions
-   Functions `pzone` and `pset` generate logical fields according
    to existing pointZones or pointSets
-   Functions `interpolateToCell` and `interpolateToPoint`
    interpolate from pointFields to volFields and from volFields to
    pointFields

Utilities and functionObjects affected by this are

-   `funkySetFields`
-   new topoSource `expressionToPoint`
-   `expressionField` and `manipulateField` now can deal with pointFields


#### Support for tensors in the `finiteArea`-field parser

The `faField`-parser now supports tensors, symmetric tensors and
spherical tensors.

Not all operators are supported because the are not instantiated
in 1.6-ext


#### New convenience-variables for the Python-Integration

These variables are added in the namespace to ease the writing of
Python-code whose output is consistent with OF

-   **timeName:** Name of the current time as a string. Allows the
    construction of directory names
-   **outputTime:** Boolean that indicates whether this is a timestep
    where OpenFOAM will write output


#### Additional operators from the `fvc`-namespace

The missing operators from the `fvc`-namespace have been added to
the Field-parser. These are

-   **`d2dt2`:** for all volumeFields
-   **`flux`:** for all volumFields. Needs a surfaceField as a first
    argument
-   **`meshPhi`:** for volume-vector-fields. Optional with a
    scalar-field that acts as the density as the first
    argument. Only works in the context of a solver
    with a dynamic mesh and hasn't been tested yet

The only missing operators from the `fvc`-namespace are
`volumeIntegrate=/=domainIntegrate`. These have been omitted as
they are trivial to implement using other functions


### Infrastructure


#### Full parallel compilation

Thanks to patches supplied by Bruno Santos (see
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=105>)
compilation of the libraries is now possible in parallel


#### Version numbers and version number reporting

Releases up to now got a version number. Utilities now report the
version number. This should make it easier to find out whether
problems are due to an old version

Still looking for a way to do that for the libraries (so that
they will report it if dynamically loaded)


### Packaging


#### Update of the Debian-Packaging

`make dpkg` now genrates also a valid package if the current
OpenFOAM-installation is **not** installed using the `dpkg`.


### Changes in the behavior


#### Directory variables in Python-integration

For parallel runs the content of the `caseDir`-variable changed
and a few variables have been added

-   **caseDir:** in parallel runs now points to the `FOAM_CASE`
    instead of the processor subdirectory
-   **systemDir:** points to the global `system`-directory
-   **constantDir:** points to the global `constant`-directory
-   **procDir:** in parallel runs points to the
    processor-subdirectory of the current CPU
-   **meshDir:** The mesh data (of the current processor in parallel
    runs)
-   **timeDir:** Directory where data would be written to at the
    current time (processor dependent)


#### User must acknowledge parallelization in Python-integration

In parallel runs the user must set the `isParallelized` to `true`
if the `parallelMasterOnly` is set to `false`.

With that he indicates that in his opinion the Python-code has no
bad side-effects in parallel runs and that he doesn't blame
swak4Foam if anyting bad happens


### Bug fixes


#### `interFoam`-based example solvers do not compile on 2.1

As reported in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=119>
due to a change the way the PISO-loop is treated the
`interFoamWithSources` and `interFoamWithFixed` don't compile
with 2.1 anymore.

To avoid `#ifdef` in the solver sources there is now a separate
set of sources (labeled `pre2.1`) for older versions. The regular
sources work with 2.1 (and hopefully the following)


#### `-allowFunctionObjects`-option not working for `replayTransientBC`

Function-objects only work with the
`while(runTime.loop())`-construct in 2.1. The utility now uses
this.


#### Field itself can not be used in `funkySetBoundaryField`

Bug reported:
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=124>

An expression like `2*U` did not work for the field `U`. Reason
was that the registry already held an object called `U` (the
dictionary representation of the field) and therefor refused to
load/register another `U`.

Has been fixed by de-registering the dictionary `U` immediately
after loading.


#### No gradient for vectors in FieldParser

The gradient for a vector field (result: a tensor field) was not
calculated. It is now part of the grammar


#### Some operators for tensors not working in 1.6-ext

`tr`, `inv` and `det` were not working for some tensort types in
1.6-ext. The parser now fails if such a combination is
used. Works OK for other OF-versions

Also introduced a workaround for certain operators not being
properly defined for pointFields (by using the internal fields)


#### `x`, `y`, `z` and `xy` etc not available as field names

These symbols could not be used as field names because they were
used for the components of vectors and tensors

Now these names are only used if the `.`-operator asks for a
component. This is implemented for these parsers

-   FieldValues
-   PatchValues
-   SubsetValues (all Zones, sets and samples)
-   finiteArea-Parsers: faPatch and faField


#### Missing tensor components for point-Fields in some parsers

All parsers except the field-parser were missing the access to
tensor components in the grammar


#### No vector construction possible for point-vectors (and tensors) in PatchField

As mentioned in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=130>
it was not possible to construct a point-vector field using
`vector(toPoint(1),toPoint(1),toPoint(1))`. Same for tensors


#### Incomprehensible error message in `funkySetFields` if the field is missing

The error message in `funkySetFields` that was issued when a
field is supposed to be created was not very helpful (something
about the field currently being an `IOobject`)


#### Missing `magSqr` in parsers

This function was reported missing on the message board


#### Wrong size when composing vectors and tensors for point-fields

The composed objects got their size from the current parser, not
the components. This resulted in a segmentation-fault for
pointFields


#### `icc` does not compile `executeIfExecutableFitsFunctionObject` on Linux

Preprocessor symbol `linux` unknown. Replaced with `__linux__`


#### Enhancement to the `trackDictionary`-functionObject

Now handles bad or non-existent filenames for dictionaries to
track

Fix provided by Martin Beaudoin


## 2012-10-18 - version number : 0.2.1


### Requirements


#### `flex 2.5.35`

This version is needed for the reentrant parsers. `2.5.33` **may**
work but is untested. Version `2.5.4` which is on some old
systems definitely does not work


#### `bison 2.4`

Version 2.3 compiles but there is an offset-problem with the
locations that breaks the Plugin-functionality

Mac-users will have to install `bison` from another source (for
instance `MacPorts`)


### Bug fixes


#### Make sure that `Allwmake` always uses the `bash`

On Ubuntu `/bin/sh` is something else and the scripts fail.
Hardcode to `/bin/bash`


#### `downloadSimpleFunctionObjects.sh` still in Makefile

This script (which was removed long ago) was still referenced in
the `Makefile`.


#### `grad` in fields added dimensions

`grad` and other operators from `fvc` added dimensions to values
that were supposed to be dimensionless. This has been fixed


#### Default `condition` for surface fields wrong size in `funkySetFields`

Due to a typo the constructed condition field was too short for
surface-fields (too long for volume-fields, but that didn't matter)


#### `mappedFvPatch` not treated like regular patches

The field-driver created patch fields there as `calcuated` when
`zeroGradient` would have been more appropriate


#### `flip()` for `faceSet` not correctly calculated

A `SortableList` was used which meant that the vector with the
flip values was not in the correct order


#### `fset()` crashes when `faceSet` has boundary faces

This problem was due to a non-special treatment of faces on the
boundary. Fixed (for `faceZone` as well).

Also boundary faces are now honored in `expressionToFace` if the
expression is a surface-field (for the volume-field logic
boundary faces will never work)


#### `groovyBC` produced incorrect results with `wallHeatFlux` etc

The reason was that during construction `refGradient`, `refValue`
etc were not read correctly (if present).

This is now fixed in `groovyBC` and the other BCs (`groovyBC` for
pointFields and `groovyBCDirection`)


#### Global variables not found in `faField`

The Lexer correctly identified the variable but the `getField`
method did not know how to get it.

Fixed


#### Wrong type of condition field in `funkySetAreaFields`

If no condition was evaluated the utility generated a
pseudo-field of the wrong length


#### `calculated`-patches \(0\) for some operations

For some operations in the `Field`-driver the
`calculated`-patches had the value \(0\) instead of the number of
the next cell. This caused certain calculations to fail (give
\(0\))

The `FaField`-driver did no fixing of the `calculated`-patches at
all.

This is fixed


#### `sqr` of a `vector` should produce a `symmTensor`

Reported in
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=150>

Added the `sqr` at the right place to the grammars. Also some
other missing tensor operations (`dev` and `symm`).


#### `funkySetFields` produced wrong values on processor patches

Patch fields were not copied any no `correctBoundaryField` was
called for technical reasons.

Fix: values copied by hand


#### `sortedToc` does not exist for `1.6-ext`

Introduced a preprocessor symbol that allows using sortedToc on
newer versions


#### Wrong `size()` and `pointSize()` reported for `FaField`-driver

This was due to a strange () (still don't know what happened
there)


#### Memory leak in the field drivers

The strings of parsed IDs were not properly deleted. Funnily this
was done correctly in the Patch and the Subset-driver. Also for
timelines-

Also fixed a leak with the labels of plugin-functions that was
present with all drivers


#### Maintenance scripts not working with non-`bash` `/bin/sh`

Reported by Oliver Krueger that on systems where `/bin/sh` is not
a `bash` anymore (newer Ubuntu and SuSE) the sourcing of
`theFiles.sh` doesn't work anymore.

Fixed and moved all the files to the `maintainanceScripts`-folder


#### `cof` and `diag` undefined

Added. Diag had to be reprogrammed as it is not implemented for
fields (probably for performance reasons).

Also some tensor operators were missing (probably lost during copy/paste)


#### No new file created if number of patches for patchAverage changes

Reported in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=153>

Fixed by removing all file pointers if the number of patches changes


#### `variables` intolerant on spaces

Spaces in the list of variables made the reading
fail because words can't contain spaces. For instance

    "var =T*2;"

Now all the spaces are removed before assigning to
variables. This will also remove spaces on the "inside" thus
making

    "v ar =T*2;"

the same as the above expression. But it is unlikely that the
call will be used in this way


#### Missing `div`-operations

These valid `div`-operations were missing from the grammar:

-   Divergence of a volume-tensor (all three kinds) producing a
    vector
-   Divergence of a surface-tensor (all three kinds) producing a
    volume-tensor


#### Fields created by `expressionField` written too late

Fields created by that function object were written with the
value from the timestep before because the regular write occurs
before the execution of the function objects.

Fixed


#### `storedVariables` did not require a `initialValue`

Now an initial value is required (instead of the default empty
string which caused parser failure)


#### Dimension checking makes `expressionField` fail

Reason is that during the calculation of the variables dimensions
are checked.

Now the functionObject switches the checking off. But a more
general solution is desirable


#### `expressionField` at last timestep not written

The functionObject does not write (and calculate) the field at
the last timestep.

Fixed with an one-liner


#### `groovyBC` makes `interFoam`-runs fail unpredictably

Reason was an uninitialized `valueFraction` which sometimes has
values that cause a floating point exception. Fixed


#### Global variables of different sizes break parallel runs

Because `size()` was equal to the expected size on some
processors. Not on all. Now the branch is taken if the size is
equal on **all** processors


#### Fields treated with `readAndUpdateFields` were written one timestep too late

Fields were not written after the boundary condition was
updated. Now they are


### Enhancements


#### Topology-operators now support `variables` etc

The topology operators `expressionToCell`, `expressionToFace` and
`expressionToPoint` now support `variables` and the other
supporting keywords if being constructed with a dictionary (for
instance from the `topoSet`-utility)


#### Fields touched by `manipulateField` being written

Usually the manipulated version of the fields is not written as
the manipulation happens **after** writing. The option
`writeManipulated` enforces writing.

Writing is not the default behavior to avoid side-effects


#### Indicator functions `onPatch` and `internalFace` added to field-expressions

The function `onPatch(name)` returns a surface-field that is \(1\)
on all faces that belong to patch `name`.

The function `internalFace()` is \(1\) on all internal faces and
\(0\) on all patches


#### Non-uniform second argument for `pow`

Now the second argument to the `pow`-function can be a
non-constant


#### Added transpose to the tensors

The expression `A.T()` transposes the tensor `A` (for symmetrical
and spherical tensors it leaves them untouched)


#### Added unit tensor `I` to parsers

If no field `I` is defined then this is used as the unit-tensor


#### Added the *Hodge dual* operator

The unary operator `*` calculates for tensors and symmetrical
tensors the hodge dual


#### `replayTransientBC` can now switch on function-objects via dictionary

The optional entry `useFunctionObjects` switches on the execution
of function objects during the calculation


#### `replayTransientBC` can now create a `phi`-field via dictionary

The optional entry `addDummyPhi` creates a `phi`-field


#### `expressionField` now allows the specification of a dimension

The `dimensions`-entry is now read at the same time the variables
are read (this should work for all programs/functionObjects where
the parser is not constructed using a dictionary but the
dictionary is later searched for the `variables`-entry)


### New features


#### Allow dynamically loaded plugins that add new functions to parsers

This allows easy extension of the functionality of `swak4Foam`
without modifying the grammar files.

The way it works is that new functions are added to a
runtime-selection table. If the grammar can not resolve a symbol
as a built-in function or a field (but only then) it looks up the
name in this table and evaluates the function. Parameters are
parsed separately and can be:

-   **primitive data types:** integer, float, string and word
-   **swak-expression:** an expression parsed by one of the
    swak-parsers. The type of this expression does not
    necessarily have to be the same as the one of the 'main'
    expression.

The first time a plugin function is searched `swak4Foam` prints a
list of all the available functions of this type. Information
included is the return type and the types of the parameters
(these include the parser used, the expected type and a parameter
name).

Libraries with plugin-functions are added via the `libs`-entry in
the `system/controlDict`

A number of plugin-libraries are already included covering these
topics:

-   Evaluation of functions of the turbulence, transport or thermo
    model
-   Different random number distributions
-   Functions to "project" `sampledSets` and `sampledSurfaces` onto
    a volume-field
-   Execute explicit discretization functions (like `grad`) but
    select the used scheme in the function instead of using the
    value from `fvSchemes`
-   Calculations of the mesh quality (same way `checkMesh` does)
    and return as fields
-   Do calculations locally on a cell (like the maximum on its
    faces)
-   Get the source fields and other properties from lagrangian
    clouds based on the basic intermediate cloud classes
    (Kinematic, Thermo, Reacting, ReactingMultiphase)

It has been tried to make the names unique instead of
short. Usually function names from one library are prefixed with
the same short string.


#### Dynamically generated lists of `functionObjects`

The new `dynamicFunctionObjectListProxy` in the
`simpleFunctionObjects` can generate a `functionObjectList` from
a string and execute them during the run like regular
function-objects.

The string is provided by a special class (the so called
`dictionaryProvider`). Current implementations for the provider are:

-   **fromFileDictionaryProvider:** reads the text from a dictionary
    file
-   **stdoutFromCommandProvider:** executes a program and takes the
    standard output as the dictionary text
-   **stdoutFromPythonScriptProvider:** executes a python-script and
    takes the stdout as the dictionary text

The string must be in the format o a regular OpenFOAM-dictionary
with an entry `functions` from which the functionObjects are
generated


#### Function object `readAndUpdateFields`

This FO in the `simpleFunctionObjects` reads a number of fields
and updates their boundary conditions at every timestep.

Main purpose is to let `groovyBC` do calculations and use the
results for post-processing purposes

Does not support surface-fields as these don't have a
`correctBoundaryConditions`-method.

Example of the usage in the `angledDuctImplicit`-case (the
results are of limited value because of the temperature boundary
condition)


#### Source terms based on `basicSource`

Three source terms were added. These source terms are in the
`swakSourceFields`-library and can be used with solvers that use
the `sourcesProperties`-dictionary. The sources are

-   **SwakSetValue:** sets values according to a mask or the
    mechanism provided by `basicSource`
-   **SwakExplicitSource:** Uses the calculated source term on the
    right hand side of the equation
-   **SwakImplicitSource:** Uses a calculated scalar-field to add an
    implicit source term (source is **without** the actual field)

These fields are only implemented in the 2.x-version of `swak`
because the interface of `basicSource` is very different in 1.7
and a backport was unnecessary


#### Function objects that stop a run gracefully

`simpleFunctionObjects` now has a function object
`writeAndEndFieldRange` that stops a run (and writes the last
time) if a field is outside a specified range.

A similar function object `writeAndEndSwakExpression` is in the
`simpleSwakFunctionObjects` that stops if a swak-expression
evaluates to true. `writeAndEndPython` does the same in
`pythonIntegration`.

Note: after the run is written one more timestep is calculated
(this seems to be due to the fact that FOs are calculated at the
start of a timestep). Also there are issues if the next timestep
is a scheduled write-time (this only seem to be an issue with
1.7.x. It all works fine on 2.1.x)


#### Function-objects to load thermophysical and turbulence models

New function objects in the `simpleFunctionObjects` allow the
loading of such models for solvers/utilities that don't have such
models but where some functionObject (for instance) needs such a
model to be in memory


#### Function-objects that create and evolve clouds of lagrangian particles

Added as part of the `simpleFunctionObjects` some functionObjects
that create a cloud of particles and evolve them at every
timestep.

The appropriate fields needed by every cloud have to be present
(either supplied by the solver or via a `functionObject`)


#### Function-object `manipulatePatchField` to manipulate the field on patches

This function objects allows the manipulation of patch fields
like `manipulateField` allows the manipulation of the internal
field. Only use if desperate


#### Delayed variables to simulate responses

If a variable is declared in the `delayedVariables`-list then its
behavior changes: when a value is assigned to that variable then
the value is not immediately used but after the time specified in
`delay`. Values are stored at intervals `storeInterval` and
interpolated linearly. If no stored data is available then the
value of the expression `startupValue` is used.

This feature allows the modeling of boundary conditions that
react with a delay to flow conditions


#### Allow preloading of fields in `funkySetFields`

To satisfy the requirements of certain boundary conditions
`funkySetFields` now allows the preloading of fields. This is
only available in dictionary mode with the `preloadFields`-entry
(for each entry in the `expressions`-list separately)


### Infrastructure


#### Compilation script checks `SWAK4FOAM_SRC`

The environment variable `SWAK4FOAM_SRC` is needed for the
`swakCoded`-functionObject. The `Allwmake`-script now checks
whether this variable exists and prints a message with the
correct value if it doesn't. It also checks whether the value is
correct and warns if it isn't


#### `Allwmake` creates symbolic links in `swakFiniteArea`

The links to `mybison` and `myflex` are missing when the sources
are downloaded as a tarball. The `Allwmake`-script now creates
these links if they are missing


#### Reformatting of the parser sources

Sources of the parsers have been completely reformatted to make
them more readable and maintainable


#### Move non-parser sources in `swak4FoamParsers` into sub-directories

Make the directory a little bit cleaner


## 2013-02-28 - version number : 0.2.2


### Incompatibilities to previous versions


#### Python-integration library renamed

To avoid clashes with other similar efforts (not that I know of
any) the library `libpythonIntegration.so` has been renamed to
`libswakPythonIntegration.so`.

Usually only the `libs`-entry in the controlDict has to be
changed (if the library is used at all). Names of the function
objects stay the same


#### Change in write behaviour of `expressionField` and `manipulateField`

Now the `outputControl`-entry is honored. If set wrong the field
is **no longer** calculated/manipulated at every time-step


### Bug fixes


#### 2.x-branch did not compile on 2.0

Branch only compiled on 2.1, but not on 2.0 due to changes in the
OpenFOAM-API

Fix provided by Bruno Santos


#### `groovyBC`: `value` reset to \(0\) during construction

If no `refValue` is given during construction then a value of \(0\)
is assumed for this and during a preliminary update the value is
reset to this value.

Fixed by using the `value` as the `refValue` if no `refValue` is
specified


#### Unspecified `value` in `addGlobalVariables` produces obscure error message

The error message occured much later when a `tmp` tried to copy a
`NULL`-pointer and was not obvious at all for the general user. Fixed


#### Did not compile on `1.6-ext`

1.6-ext does not consider the product of two symmetrical tensors
to be symmetrical. Fixed


#### Python integration did not get global variables

Change in the interface of the `ExpressionResult`-class
broke the access of global variables from a
Python-functionObject. Fixed


#### `maintainanceScripts/makeSwakVersionFile.py` failed when no Mercurial is installed

This didn't influence the compilation (as there is already a
current version file there) but confused people.


#### Non-uniform variables for `internalField` only worked correctly for volume fields

Face and point fields did not propagate the information correctly
and were treated as non-uniform volume fields of the wrong
size. This should now be fixed


#### Division of `faceFields` causes division by zero

The reason was that the patches of the divisor had a value of
zero. This has been fixed by only dividing the
`internalFields()`. Same for `pointFields`.


#### `valuePatches` did not work for mapped patches

This kind of patch was not identified as something that could
have the value fixed


#### Recent versions of 2.1.x break compilation of `CommonValueExpressionDriver.C`

The definition of the operator `lessOp` clashed with another
definition. Renamed.

Fix provided by Bruno Santos


#### `expressionField` and `manipulateField` did not honor the `outputControl`-entry

`expressionFields` were calculated at every time-step, even if
the computational cost was high. Now if `outputControl` is
`outputTime` the field is only calculated if it is going to be
written


#### Double `namespace Foam` makes compilation of `groovyJump` fail with `icc`

Seems like this one was overlooked because `gcc` is more
tolerant towards stupidity.

Reported by Edo Frederix


### New Examples


#### Cases from the *OSCFD12* Conference in London

On the slides the case files were promised


### New features


#### Access fields from different cases

It is now possible for expression on `internalFields` to access
fields from different cases. These fields are always read from
disk when being accessed for the first time and stay cached in
memory until the time is changed. The data is interpolated to the
current mesh with the `meshToMesh`-class (which is the workhorse
of the `mapFields`-utility)

The meshes are stored in a repository and are accessed via a
name. If for instance the mesh is known under the name
`otherMesh` then in an expression the term `otherMesh(T)` is the
field `T` from that other mesh at the specified time interpolated
to the current mesh.

Certain function objects and parser instances allow specifying
foreign meshes in a subdictionary `foreignMeshes`.


#### Patch-expressions now support mapped-patches

This is the long asked for feature to "get non-uniform values
from other patches". It only works if the patches are specified
as `mapped` (or `directMapped` for 1.x). Therefor a patch can
only access one other patch with this.

On a mapped patch `mapped(T)` gets field `T` from whatever this
patch maps from (other patch, cells, faces). `mappedInternal(T)`
gets the values from the internal cells on the other patch (this
only works for patches).

On a mapped patch an external variable expression for patches now
changes its meaning: if `other` is the patch that the current
patch maps from (and only then) then `var{patch'other}=expr`
evaluates `expr` on the other patch and then maps the values to
the current patch (`var` may be non-uniform). In all other
circumstances `var` will get a uniform value which is derived
from whatever `expr` evaluates to.

To allow non-uniform `offsets` in the specification of mapped
patches in `polyMesh/boundary` a utility
`calcNonUniformOffsetsForMapped` was developed.


#### Function objects that manipulate time

The framework for function objects that manipulate the `runTime`
were added. The concrete implementations are

-   **setDeltaTByTimeline:** set the timestep from a data file
-   **setDeltaTWithPython:** set the timestep by evaluating a
    Python-snipplet that should return a floating point number
-   **setEndTimeWithPython:** set the end time from the evaluation of
    a Python-snipplet


#### Plugin-functions for chemical model

The library `libswakChemistryModelFunctionPlugin` adds the
possibility to calculate properties of the chemistry like
reaction rates, chemical timescale and released energy.

Currently only implemented for `psiChemistryModel`

If there is no chemistry model in memory it will be loaded and
the reaction rates are calculated.

There is a function `psiChem_updateChemistry` that forces the
chemistry to be recalculated with a specified timestep. For
technical reasons this returns a scalar field with the value
\(0\). So an expression like
`psiChem_updateChemistry(0.0001)+psiChem_RR(CH4)` calculates the
reaction rate assuming that the timestep is \(0.0001\).

Two additional functions that sum up all the reaction rates (this
**should** be \(0\)) and sum up only positive reaction rates (giving
an impression on what is going on)


#### Plugin-functions for radiation model

The library `libswakRadiationModelFunctionPlugin` adds the
possibility to calculate properties of the radiation like
parts of the source terms `Rp` and `Ru` and the current
radiation source

If there is no radiation model in memory it will be loaded and
the reaction rates are calculated. Assumes the presence of a
temperature field called `T`.


### Enhancements


#### Python-Integration now supports numpy-arrays

If the `numpy`-library is found then global variables which are
fields are being transformed to `numpy`-arrays. These arrays can
be accessed with the usual `numpy`-array access like `a[i,j]` or
`a[i,:]`. Global variables are made accessible **by
reference**. This means that writing a value changes the global
variable. Setting the whole variable has to be done by slicing
`a[:]=3` (`a=3` removes it from the workspace). Vectors and
tensors are two-dimensional arrays. They have
convenience-attributes that return the whole vector of a
component (like `a.x` for vectors or `a.xx` for tensors). To
overwrite these they have to be sliced: `a.x[:]=0` (`a.x=0` only
changes the attribute)

If a variable that is going to a global namespace is a
`numpy`-array then it is translated by the following rules:
vectors are transformed to `scalarField`. Arrays with 3 columns
to `vectorField`, 9 columns to `tensorField` and 6 columns to
`symmTensorField`. Different column-numbers produce errors


#### Optional parameter `writeCode=/=writeFile` for `pythonIntegrationFunctionObject`

If this is set then this Python-code is executed every time a
time-step was written to disk


#### Python-integration has convenience-functions to create data files

Python-integration now has two convenience-functions that return
a filename with the full path and create the directories if
necessary. The file is **not** created (that is the responsibility
of the Python-code).

The functions are (`name` is the name of the function object)

-   **dataFile(fname):** creates a directory
    `<case>/<name>_data/<time>`. To be used for data that is
    written at times that differ from write-time
-   **timeDataFile(fname):** creates a directory
    `<case>/<time>/<name>_data`. Should only be used for data
    that is written only at write-time


#### Switch off dimension checking in `funkyDoCalc`

A command lione switch `-noDimensionChecking` is added that
allows switching off the checking of dimensions if this makes
expressions fail


#### `funkySetFields` allows accessing fields from another mesh

If another case (with time and region) is specified via command
line options (no dictionary) then in the expressions the other
mesh can be accessed under the name `other`

Main application for this is comparing to other cases (but also
comparing different timesteps of the same case with each other).

Used time is either fixed or set to the currently used (this can
be selected via the command line)


#### `funkyDoCalc` allows specifying foreign meshes

Each expression can specify foreign meshes with a
`foreignMeshes`-subdict


#### Function objects to load and update foreign meshes

There are two new function objects:

-   **addForeignMeshes:** this allows specifying new foreign
    meshes. These can be used in subsequent function-objects or
    other swak-expressions on `internalFields`
-   **foreignMeshesFollowTime:** this allows specifying a list of
    meshes whose time is set to the current time of the running
    case


#### `replaxTransientBC` outputs timestep

This helps debugging timestepping-strategies


#### New accumulation `weightedAverage`

A new possible entry for the `accumulations` calculates an
average that is weighted by a facetor that depends on the entity:

-   cell volume if applicable
-   face area for surfaces etc
-   \(1\) if there is no appropriate weight (usually values defined
    on points). The result is equivalent to `average`.


#### `expressionField` reports name and type of the created field

The title says it all. Should give the user additional feedback
about what is happening


#### New boundary condition `groovyPartialSlip` in `groovyStandardBCs`

This boundary allows specifying the partial slip fraction of the
regular `partialSlip`-BC with an expression.


#### `funkySetFields` allows switching off the correction of BCs

This is sometimes necessary to allow calculations with fields
where the boundary conditions have a lot of dependencies
(turbulence model, other mesh etc)


#### Allow selection of additional regions in `funkySetFields`

The option `-additionalRegions` allows selecting additional mesh
regions for boundary conditions that require it (usually used
together with `-preloadFields`).

For the other case there is an additional option
`-otherAdditionalRegions`.


#### Adding a `-preloadFields`-option to `funkySetFields`

This option allows specifying additional field names to be
preloaded in command line mode. If multiple mesh regions are
specified (including another mesh)it will try to preload the
field there too


#### `replayTransientBC` now supports all volume types

Support for tensor-types has been added


#### `replayTransientBC` preloads fields

For boundary conditions that require other fields these can be
loaded into memory


#### `replayTransientBC` allows selection of time-steps

The utility now allows the selection of timesteps. In that case
it acts fundamentally different: instead of doing a timeloop it
jumps to those times, calculates the boundary conditions and
writes. The purpose of this mode is generating boundary fields
for postprocessing.


### Infrastructure


#### `Allwmake` stops if building of `swak4FoamParsers` fails

As the majority of installation problems were actually problems
with the compilation of the library `swak4FoamParsers` and as
this library is central for all other things in `swak4Foam` the
compilation terminates if this library is not built


#### Header `swak.H` with version information and macros

This new header includes information about the
`swak4Foam`-version, the `OpenFOAM`-version and macros for
workarounds that depend on the OpenFOAM-version


#### `accumulations`-entries converted to `NamedEnums`

This forces the entries to be checked while being read. Allowing
more consistent treatment


#### Additional maintenance script for special python-versions

This script allows compiling additional versions of
`swakPythonIntegration` for cases where the version on the
development machine is different from the version on the
simulation machine (see installation chapter)


### Cosmetics

Changes without consequences for the user


#### Change type of `name` in parsers to `word`

The valid characters for an identifier were the same as in `word`
(was `string` before)


#### Updated contributor lists in `README` and sources

The list of contributors to each file have been added (mostly by
script with the information from the repository)


#### Rewritten methods that return pointers

This abomination from the early days of `funkySetFields` has been
cleaned


### Documentation

Changes to the documentation


#### Start of the reference-manual

A first version of a reference manual is added. Only the first
part about expressions and parameters is written.


## 2013-03-18 - version number : 0.2.3


### Supports OpenFOAM 2.2

This is the first version to compile with OpenFOAM 2.2

Due to changes in OpenFOAM it requires several `#ifdef` (something
that is usually avoided in the OpenFOAM-world) and other
prepocessor definitions)


### Incompatibilities to previous versions


#### `simpleFunctionObjects` and `simpleLagrangianFunctionObjects` no longer independent from rest

Due to incompatibilities between OpenFOAM 2.2 and previous
versions there are compatibility headers included from the rest
of swak4Foam.

Theoretically both libraries can be easily made independent again.


### Bug fixes


#### Compiles on `1.6-ext` again

The last release (0.2.2) did not compile on `1.6-ext`. This is
fixed


#### Missing field files for the OSCFD2012-cases

Due to a stupid `.hgignore` the `0.orig`-directories were
missing. Nobody complained though


#### Did not compile on `2.0.x`

This has been fixed


### New features


#### Writing of sampled surfaces and sampled sets

With the options `autoWriteSurface` and `autoWriteSets` sampled
surfaces and sets are automatically written at each write
time. With `writeSurfaceOnConstruction` and
`writeSetOnConstruction` both are written when they are
created. A format has to be specified. No values are written.


### Enhancements


#### Parsers now support `eigenValues` and `eigenVectors`

These function and some other minor ones (`sph`, `dev2`,
`twoSymm`) are now supported by the parsers


#### Extract rows from tensors with component operator

The component operators `.x`, `.y` and `.z` now extract the rows
from a tensor


#### Variable names that are identical to fields trigger a Warning

As this situation (like a variable named `p` for the position
that shadows the pressure field `p`) leads to weird errors a
warning is given. The warning can be switched of with the option
`variableNameIdenticalToField`.


## 2013-05-24 - version number : 0.2.4


### Incompatibilities to previous versions


#### Data directories now generated in folder `postProcessing` instead of case folder

According to the convention introduced in OpenFOAM 2.2 now the
data directories generated by functionObjects based on
`simpleFunctionObjects` (that is almost all) and
`swakPythonIntegration` (if the Python-code uses the
`dataFile`-function) are generated as sub-directories of
`postProcessing` in the case directory.

Scripts that rely on the location of these files will have to be adapted


#### `ExpressionResult` from old versions are not correctly read

This is due to a fix that now allows reading/writing these in
binary mode. Only occurs if reading old data that either

-   uses a delayed variable
-   has global variables written in timesteps


#### Adaption of cases to 2.2

This may break them for previous versions of OpenFOAM


### Infrastructure


#### Additional flags for the compilation of the parsers

The C-files generated by `bison` become very big and some
compilers need quite long when aggressive optimization is
used. An additional environment variable
`SWAK_COMPILE_GRAMMAR_OPTION` allows the specification of
additional compiler switches (like `-O1`) that will appear
**after** the regular switches and should alter the behavior of
the compiler. Set in `swakConfiguration` if you want to set this
permanently


#### Use `bash` instead of `sh` in maintainance scripts

Some distributions don't use good old `bash` as the default
shell. This breaks some scripts in subtle and not so subtle ways


#### Build-script prints swak-version in the beginning

To filter bug-reports with outdated versions


### Cosmetics

Changes without consequences for the user


#### Methods use `word` for names instead of `string`

Methods that look for fields now use the appropriate data type


### Bug fixes


#### Compilation on Fujitsu (and probably Intel) compilers

There were template specializations for `pTraits` in
`ExpressionResult.C` that had to be moved to `ExpressionResult.H`
to allow compilation on these non-`gcc`-compilers


#### `executeIf`-function objects are executed if `readDuringConstruction` is set to `true`

Even if the condition evaluates to `false` the function objects
in the list are executed. This makes the run fail if the purpose
of the condition was to guard from such a situation. Fixed


#### Expression results are not correctly read from binary file

This was fixed by writing the fields properly with
`writeValue`. This may introduce a little incompatibility when
reading files written by old versions


#### Generated fields like `vol()` were registered

These fields were registered with the mesh which might have
disturbed similar named *real* fields. Also did the fields
generated for variables get registered


#### Missing files in `potentialPitzDaily`-case

Missing initial conditions made this case a bit pointless


#### `log(vol())` gives floating point exception

This was due to the boundaries being set to 0. Fixed. Also for
the `dist()` function


### New features


#### Library with additional `searchableSurfaces`

The library `simpleSearchableSurfaces` adds more searchable
surfaces to be used in `snappyHexMesh`. Types of surfaces in
that library are

-   some objects that duplicate existing surfaces and are only
    there for historical reasons (except the cylinder which
    distinguishes between different sides). Others wrap existing
    surfaces and expose the different sides
-   wrappers for other surfaces that do coordinate transformations
    with it: rotation, translation, scaling
-   wrappers that take two surfaces and do boolean operations with
    them: union, intersection, difference, exclusive difference
-   Some wrappers that manipulate the regions of a surface: rename
    them, collect them into one region


#### New variable type `StackedExpressionResult`

This type is used to collect multiple values. If it is assigned
a value then the **uniform** value of the right hand side is
appended to the variable. Useful for collecting multiple values
for output.

Variant `StoredStackExpresionResult` keeps the values between
timesteps and allows the collection of timeline data


#### Function object `dumpGlobalVariable`

Dumps the value of a global variable into a line of a file


#### Function object `removeGlobalVariable`

Removes a global variable (mainly used to preserve memory)


#### Optional entry `aliases` for parser specifications

This dictionary allows the specification of alias names and
their real names. If an alias name is found in an expression
then it is replaced by the real name when looking for
fields. The application for this is that OpenFOAM allows
characters in field names that are not valid in swak-names and
would break the grammar if they were (for instance `-` or
`:`). By setting an alias such fields can still be accessed.


#### Function object to dump registered objects

The function object `listRegisteredObjects` in the
`simpleFunctionObjects` lists all the objects currently
registered with the mesh. Mainly used for debugging ("Which
fields are currently available?")


#### Function object that only executes its children if the run is serial/parallel

The new functionObject `executeIfParallelSerial` allows
specifying if a list of function objects should only be run if
the simulation is running in serial or parallel.


#### Function objects to calculate distributions

There are now four function objects that calculate distributions
of fields. Distributions are calculated based on the
`Distribution`-class that is part of OpenFOAM starting from
version 2.0 and uses a weighting for added values. There are two
kinds of output

-   a timeline with the key characteristics of the distribution
-   directories with the distributions at specific times. These
    are written "raw" and accumulated

For tensors and vectors these are output separately for every
component.  These outputs can be switched on and off
separately. The user has to specify the bin-width for the
classification. If the bin-width is too small and the
distribution becomes too small it is scaled down.

The two function-objects in the `simpleFunctionObjects` are:

-   **fieldDistribution:** distribution of field values weighted by
    the cell volume
-   **patchFieldDistribution:** distribution of a field on a number
    of patches weighted by the face area

In `swakSimpleFunctionObjects` the two functionObjects are:

-   **swakExpressionDistribution:** distribution of an `expression`
    on some entity. The user also has to specify and expression
    `weight` with the weight of each value and a logical
    expression `mask`. Values are only used if `mask` is `true`
    at the place
-   **patchExpressionDistribution:** distribution of `expression` on
    a number of patches. As the weight the face-area is used


### Enhancements


#### Global variables now can be subclasses of `ExpressionResult`

This allows other variable types like stored variables to be
inserted there. They are also now saved for restarting


#### `endTime` now added to Python-wrapper

All classes using the python interpreter now have a variable
`endTime` with the current end-time value


#### Force write if `endTime` set by time-manipulation is reached

If one of the functionObjects for time-manipulation set the
end-time and it is reached and this is no time for output a
complete output of the fields is forced


#### Function-objects based on `patch` from the `simpleFunctionObjects` now accept regular expression

This includes the `patchExpression`-functionObject. The supplied
list is now interpreted as a list of regular expressions and all
patches that match are included and processed


#### More flexible output control for functionObjects based on `simpleFunctionObjects`

An optional parameter `outputControlMode` now allows control on
when the function object will be executed. Possible values are:

-   **timestep:** Every `outputInterval` timesteps the object will
    be executed. This is the default behaviour with an
    interval \(1\)
-   **deltaT:** Approximately every `outputDeltaT` times it will be
    executed. The nearest possible timestep is used
-   **outputTime:** Executed whenever a regular output is scheduled


#### `swakDataEntry` improved

Two enhancements

-   the name of the independent variable  no can be
    specified. This variable holds the value that is passed to the
    data entry as a uniform value
-   data entry can now be integrated. This allows using it for
    instance for the injection rate in lagrangian models


## 2014-01-24 - version number : 0.3.0

The reason for the jump in the minor revision number is that with
the introduction of the parser for lagrangian particles (`cloud`)
the last white spot as far as major data structures in OpenFOAM is
"explored"


### Incompatibilities to previous versions


#### Support of *old* `1.6-ext` lost

Due to certain differences this version only compiles with the
`nextRelease`-branch of 1.6-ext (from the `git`).Usually the
failing parts can be fixed b commenting out the appropriate
`#define` in `Libraries/swak4FoamParsers/include/swak.H`.


### New supported versions


#### Added support for Foam 3.0

As this version is quite similar to 1.7 a lot of `#ifdefs` were
introduced by this port leading to a possible unification with
the 1.x-branch


### Infrastructure


#### Make error messages in `Allwmake` more verbose

The error messages where already quite verbose. But people
didn't understand them and asked the same questions over and
over &#x2026;


#### `simpleFunctionObjects` no longer considered an independent project

As there are going to be more cross-dependencies the
`simpleFunctionObjects` now have to be part of swak. Changes to
the compile-scripts reflect this.


#### `Allwmake` makes sure that swak is compiled for the same installation

The script writes the version it is used with to disk and at
later compiles that this is the same (this makes sure that not a
wrong version is used inadvertently to compile)


#### Additional macros for Debugging output

There are two macros defined that inside an object write the name
of the class and the address of the object (if debug is
enabled). This makes the output easier distinguishable from the
output from other classes/objects. The two macros are:

-   **Dbug:** Writes the debug information **only** on the master processor
-   **Pbug:** Writes the output on all processors and for parallel
    runs prefixes the processor number

Both macros are to be used like regular streams and don't have
to be enclosed in `if(debug){}` (this is part of the macro)


#### Separate option for compiling the output of `flex`

With some compilers (Intel) and aggressive optimization the
lexers (whose code is generated by `flex`) have uninitialized
memory and segmentation faults. In these cases now an environment
variable `SWAK_COMPILE_LEXER_OPTION` to set lower optimization
for these compilers. For example:

    export SWAK_COMPILE_LEXER_OPTION=-O0

After that recompile


#### Fixing race condition with big `WM_NCOMPPROCS`

For some machines race conditions with compiling the main library
has been reported. The reason seems to be that some source files
need headers generated while compiling the grammar. If due to the
large number of simultaneous compilation processes the headers
have not been generated when these files are compiled the
compilation fails.

This effect could not be reproduced on the development machine
(but that doesn't mean that this doesn't happen on some
machines). Nevertheless for the libraries where this might be the
case the number of compilation jobs is limited to the number
grammars in the library.

Better solution to let the `Allwmake`-script generate the headers
before the regular `wmake` starts.


#### Updates to Debian-packaging

Several minor additions that should make packaging easier

-   Package version now gets the correct distro name (at least on Ubuntu)
-   Packaging picks up the correct python-version (2.7 or 2.6
    &#x2026; does anyone use older?)
-   Packaging process leaves the `swakVersion.H` alone


### Documentation

Important enhancements of the documentation in the
`Documentations`-folder


#### Documentation of `accumulations`

The possible values of the common `accumulations`-lists are
documented


#### General documentation of the Python-embedding

The general options and the behavior of the Python-embedding are
described


### Incompatibilities to previous versions


#### `outputControlMode` `timestep` renamed to `timeStep`

Because of report
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=177>
to be consistent with the nomenclature in the 'regular'
function-objects.


### Bug fixes


#### Missing `timeSet` in function-objects

This method has been added in 2.2.x and breaks the compilation
of several function-objects

Fix developed by Bruno Santos


#### `sourceImplicit` unstable

For some reason using `SuSp` gave unstable results for the
PDE-functionObjects. Changed to `Sp`


#### Fixed bug were only one `swakCoded` worked at a time

Bug originally reported
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=179>
in. Reason was that the variable describing the type was not
correctly set.


#### Incorrectly read entries in `swakCoded`

The entries `codeEnd` and `codeExecute` were not correctly read
but instead the entry `codeRead` was read. Fixed


#### No logical variables found by most parsers

Reported as
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=172>

Only the field parser correctly found logical variables (although
they were stored). Added a method to pick up the variables (the
reason why this part was commented out is that there is no such
thing as a `volBoolField` and that is what the regular mechanism
expected)


#### `sampledSurface` not correctly updated at time of write

Reported as
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=176>

In that case the driver was written before the surface was
updated thus generating fields of size \(0\). Now `update` is
called at various places (to make sure it is called in any instance)


#### `sumMag`-accumulation now working

This accumulation was available but not implemented. Now implemented.

For non-scalar types it is calculated separately for each component


#### Calculation of weight fields failed if size on one processor was \(0\)

This was due to a logical error that was propagated through
mindless copy/paste (only the Field-driver got it right). Fixed


#### `groovyTotalPressure` does not read `value`

Because it is not initialized from the superclass when the
dictionary constructor is used. Fixed


#### For multiple times the option `addDummyPhi` makes `funkySetFields` crash

Because the pointer is already set. Fixed


#### `aliases` not constructed from dictionary

If the dictionary was read after the construction the aliases
are not read. Fixed by moving this reading to the tables reading
which is used in every time a dictionary is involved


#### Gravity not correctly passed in `evolveXXCloud`

Passed a value where a reference would have been needed. Fixed


#### `writeOften` writes all the time

Reason for this was a change of the interface of `outputTime` not
being propagated to this function-object. Fixed


#### Python-integration does not return single scalars as uniform

The Python-integration returned single scalars (and vectors) not
as a single value but as a field of length \(1\). This caused
warnings that messed up the output. Fixed


#### `basicSource` working again for OF 2.1

These were not compiled by accident. Fixed


#### `faceZone` and `faceSet` do not correctly work on parallel runs

As reported in
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=190>
if a face-set or zone has faces on a processor boundary incorrect
values were produced.

The reason was that boundary faces are treated like internal
faces. Also do these faces exist on both processors. This is now
fixed. If the face is on a regular boundary patch the value is
used. If on a processor patch the value is only used if it is on
the owner processor.


#### Allow user to override `notImplemented` in `movePoints` and `moveMesh` of function objects

This addresses
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=190>

Subclasses of `simpleFunction`-objects are not guaranteed to be
parallel-aware (for instance `swakExpression` can have problems
with stored variables if the number of cells/faces changes) so
the `notImplemented` can't be removed for good.

The workaround is a rather verbose error message and the
possibility to override `notImplemented` with a dictionary-switch


### New features


#### Function object that executes if the OpenFOAM-version is right

The functionObject `executeIfOpenFOAMVersionBiggerEqual` only
executes if the OpenFOAM-version is bigger or equal to a
specified version. The arguments `majorVersion` and
`minorVersion` are required. If `patchVersion` is not specified
it will match any version. A `git` version (`.x`) will match any
patch-version


#### Function object that is only executed if the current time is the `startTime`

The function object `executeIfStartTime` executes its children
if the current time is the `startTime` from the `controlDict`
(which isn't necessarily the first time step in a restarted case)


#### Function object that calculates the potential flow

To be used on an existing field to set reasonable initial
conditions


#### Function object that forces a recalculation of `phi`

Named `recalcPhi` in the simpleFunctionObjects. Usually needed
when some other functionObject manipulates velocity or density


#### Function object that corrects thermophysical model

Name `correctThermo`. Recalculates the thermophysical
model. To be used after manipulating temperature etc


#### Function objects that recalculate the energy of a thermophysical model

Recalculate enthalpy, sensible enthalpy or internal energy
according to the current temperature. This allows making this
consistent if the temperature has been manipulated.

For OpenFOAM after 2.2 these are replaced by one that
recalculates the energy or enthalpy


#### Function object that calculates the average of one variable as a function of another

The function object `swakExpressionAverageDistribution`
calculates the average of one function (the `expression`) as a
distribution over another (the `abscissa`). An example would be
the average pressure in x-direction with `abscissa` being
`pos().x`, `expression` being just `p` and the `weight`
being `vol()`. The weight has to be a scalar. All other
expressions can be any data-type


#### New utility `fieldReport`

This utility prints some quantitative statistics about a
specified field. Optionally these statistics can also be printed
for patches, sets and zones. The data can be written to a
CSV-file. Also the distributions of the field can be written.


#### New utility `funkyPythonPostproc`

This utility needs a dictionary and a specification of
times. For each time it

-   loads a list of fields specified in the dictionary
-   executes a list of function objects specified in the dictionary
-   executes python-scripts

The idea is that the function objects pass data to the
python-scripts via global variables and the python-scripts do
whatever they like


#### New utility `funkySetLagrangianParticle`

This utility allows setting new fields of a lagrangian
cloud. Like `funkySetFields` it has two modes:

-   Setting one field over the command line. This is triggered by
    the `-field`-option
-   Using a dictionary to specify what is being set. In this mode
    more than one field and more than one cloud can be set. In this
    mode there is also the possibility to specify a **new**
    cloud. This mode expects variables in global namespaces. On of
    these variables is the position of the particles, for the other
    variables fields of the same name will be created. All the
    variables have to be of the same size. Data where corresponding
    particle position is outside the mesh is discarded


### Enhancements


#### Additional parser for lagrangian particles

There is now an additional parser to calculate expressions on
particles.

This parser is organized in a separate library
`libswakLagrangianParser` that has to be loaded to use this
parser.

Depending on whether the cloud in question is already in memory
or on disk the data is handled differently:

-   if the data is on disc then the basic properties of the cloud
    (particle positions etc) are used to initialize the class. Then
    all files in the folder of the cloud are read and made
    available as field names that are the same as the
    file-names. Characters in the file names that are not
    compatible with the parser are replaced and created as
    aliases. In this mode `swak` has no idea about the data except
    the name and the type
-   if the cloud is in memory then a corresponding proxy object is
    sought. This proxy object knows which data the cloud has, what
    the type is and a short description. It makes the data
    available as fields.

    `swak` has by default proxy objects for most particle classes
    that come with `OpenFOAM`. For unsupported classes and adaptor
    library has to be written.

When a parser for a cloud is used for the first time a table of
all the available fields is printed to the screen with type and
description (if available)


#### Function objects that calculate PDEs can use fields from memory

If the field is found in memory this is used instead of a file
read from disc.


#### Function objects that calculate PDEs can write fields before solving

An additional switch `writeBeforeAfter` allows writing the field
before and after solving


#### simpleFunctionObjects have addition `outputControlMode`

Mode `startup` executes the FO only during construction


#### Conditional functionObjects now have optional `else`

It is now possible to add a sub-dictionary `else` that is used
to initialize a `functionObjectProxy` that is executed if the
condition is **not** fulfilled. The sub-dictionary inherits all
settings that are not set from the parent-dictionary


#### `swakCoded` now allows addition of data to functionObject

The entry `codeData` is now read and inserted into the functionObject


#### Parsers in `swakFiniteArea` no also have complete tensor-operations

The two parsers in that library now also support the complete
set of tensor operations (like `eigenValues` etc)


#### `swakExpressionDistribution` now allows non-scalar weights

For expressions whose results is not a scalar now the weight
function can either be a scalar or of the same type as the
expression (so every component can have a separate weight)


#### More options for `accumulations`

A number of possible accumulations have been added. Most of
these are based on distributions. If the `weighted` variant is
chosen then the meaning is the more physical one (for `weighted`
the 'natural' weight of the quantity is used. For instance for
cells the cell volume. Otherwise the weight \(1\) is used). Some
of these accumulations need a single floating point number as a
parameter. This is simply added to the name. The added
accumulations are:

-   **weightedSum:** sum of the quantity times the weight. There is
    an alias `integrate` for this
-   **median:** The value for which 50% of the distribution are
    smaller than this. More robust alternative to
    `average`
-   **quantile:** `quantile0.25` for instance is the value for which
    25% of the distribution are smaller than it
-   **range:** The difference of the quantile of \(\frac{1+f}{2}\) and
    \(\frac{1-f}{2}\). For instance `range0.9` gives the
    range in which 90% of the values are (from the quantile
    5% to 95%)
-   **smaller:** The fraction of the distribution that is smaller
    than a given value
-   **bigger:** The inverse of `smaller`
-   **size:** The size of the underlying entity (usually number of
    cells, faces, points). For types with more than one
    components all the components have the same value
-   **weightSum:** Sum of the weights of the underlying
    entity. Usually the volume oder the area of it.


#### Python code files are now searched more flexible

If a file specified with an option like `startFile` in a
Python-functionObject (or similar) is not found in the current
directory the path of the dictionary it is specified in is
prepended and the file is searched there


#### Python integration now uses `IPython` if possible

The interactive shell of the python integration now uses
`IPython` if it is installed. This improves tab-completion etc


#### Preload libraries in the Python integration

As problematic libraries could hand during importing these
libraries can be imported in a safe way using the optional
`importLibs`-entry which is a dictionary. The keys are the names
under which the imports will appear in the Python-namespace. The
value is optional and the name of the actual library


#### Added standard function `weight()`

All parsers now implement a function `weight()` that returns the
"natural" weight that is used for instance in the weighted
average (for internal fields that would be for instance the cell
volume)


#### `funkyDoCalc` now writes files

Two kinds of files are optionally written

-   CSV-files with the values of an evaluation (each evaluation
    gets its own file)
-   Distributions of the evaluations (in a separate directory for
    each time)

All the files go into a directory whose name is derived from the
name of the evaluation. The outputs can be switched on with the
options `writeDistributions` and `writeCsv`. Either

-   from the command line: this switches it on for **all** evaluations
-   one "per dictionary"-basis for each evaluation separately


#### PDE-functionObjects now relax their equations

The PDE-functionObjects now honor the `relaxationFactors. If
     =steady` is `false` then relaxation has to be switched on using
`relaxUnsteady`. For the last corrector iteration the equation
is not relaxed unless the parameter `relaxLastIteration` is set.


#### Full set of `laplacian`-operations in `internalField`-parser

The support of `laplacian` operations (especially with a
coefficient that is different from a scalar) was incomplete. Now
all possible coefficient types are supported.

Also in the `fvcSchemes`-plugin functions the set of
`laplacian`-operators was completed


#### Function object `swakExpression` now has optional `mask`

If the logical expression `mask` is set then only the results
from `expression` for which `mask` is `true` are used for
accumulations


### Examples


#### Moved the OSCFD-examples to a different directory

Started one new directory for all cases from presentations


#### Added examples from the swak-training at the 8th Workshop

Two new examples

-   **sandPitsOfCarcoon:** Reenacting a scene from "Return of the
    Jedi" with `twoPhaseEulerFoam`
-   **landspeedersInCanyon:** Simulating two landspeeders from "A new
    hope" with `simpleFoam`


## 2014-07-11 - version number : 0.3.1


### Incompatibilities to previous versions


#### Additional required code snipplet for some Python-functionObjects

There is now a new code-snipplet required for a number of these
function objects. It can be left empty but has to be specified


#### Dimension required for stuff in `swakSourceFields`

The expressions used with `swakExplicitSource`,
`swakImplicitSource` and `swakSetValue` now also need a
dimension. This dimension is used to make sure that the user
"knows" the actual dimensions of his source terms


### New supported versions


#### Added support for OpenFOAM 2.3

This version adds support for OpenFOAM 2.3. The major changes
were due to changes in the API to

-   `searchableSurface`
-   `meshToMesh`

And several minor adaptions


#### Added support for Foam-Extend 3.1

No major changes required. Runtime-selection tables are now
sorted too


### Infrastructure


#### Improved macros for Foam versions

This should make `swak.H` more readable


#### Support for private installation of bison

If a directory `privateRequirements/bin` is private in the
sources directory then the `bison` found there is used. A script
`maintainanceScripts/compileRequirements.sh` is provided that
fetches the bison sources, compiles them and installs them there


### Documentation


#### Information about macro-expansion added

A section about thus new feature is added


### Bug fixes


#### Invalid entries in distributions from `swakExpressionAverageDistribution`

The distribution data was not clipped to the range where actually
data was collected. This is now fixed by adding the notion of an
invalid value to `SimpleDistribution`


#### `swakExpressionAverageDistribution` not correctly working in parallel

This should now be fixed


#### `fvOptions` in `swakSourceFields` now actually working

Because of the missing dimensions until now `swakExplicitSource`
and `swakImplicitSource` were not actually working


#### `fvOptions` did not honor the `selectionMode`-entry

Always set source etc for whole region. Now the `selectionMode`
is honored (for instance only in `cellZone` the source term will
be applied.)


#### `patchFunctionObject` fails for some parallel runs

Problem is that when the processor boundaries are used too that
the indexing might be inconsistent on the different
processors. Fix: coupled boundaries are ignored by default (have
to be switched on)


#### `funkyPythonPostproc` not executing function objects

In newer version the `execute()`-method of function objects
needs a parameter `force`. Fixed


#### Temporary fields shadow 'real' fields in `foam-extend-3.1`

Due to a small semantic difference in the copy constructor of
`GeometricField` a temporary object of the same name shadows the
real field and prohibits that it is written. Fixed by giving the
copy a unique name


### New features


#### Boundary condition `groovyFixedNormalSlip` in `groovyStandardBCs`

These boundary conditions allow specifying a value in normal
direction to the patch while in tangential direction the regular
slip condition is applied. This is implemented for `fvPatch` and
`pointPatch`. For the `pointPatch` the normal direction has to be
specified by the user


#### New utility `funkyWarpMesh`

Utility that calculates new point values for the mesh. In the
simplest form the expression is specified on the command line. If
the option `-expression` is missing the program looks for a
dictionary which allows more control. If `-relative` is specified
points are moved by the result of the expression. The result of
the expression must be a `pointVectorField`


#### Function objects that write additional writes depending on the flow conditions

A hierarchy of function objects that trigger additional
writes. Once a condition is fulfilled data is either written for
a specified interval or as long as the condition is
fulfilled. After that a cooldown period is possible. During this
period nothing is written.

The concrete function objects are

-   **writeIfFieldOutside:** writing is triggered if the minimum or
    the maximum of a field is outside a specified range. Part of
    the `simpleFunctionObjects`
-   **writeIfSwakExpression:** writing is triggered if a logical
    expression evaluates to `true`. Stopping and retriggering is
    also possible with logical expressions. Part of
    the `simpleSwakFunctionObjects`
-   **writeIfPython:** writing is triggered by a Python-snipplet that
    either returns `True` or `False`. Stopping and
    retriggering are also possible. Part of
    `swakPythonIntegration`

Allows saving and writing a specifiable number of timesteps
**before** the condition is met (this allows finding out what lead
to this)


#### Function object `writeOldTimesOnSignal` with new signal handlers

This function object installs a new signal handler for specified
signals.

The function object itself saves the last $N$-timesteps, the
signal handler (when called) writes these steps to disk and then
calls the regular OpenFOAM signal handler


#### New library with `fvOptions`

Starting with OpenFOAM 2.2 (where the concept of `fvOptions` was
introduced) there is an additional library `swakFvOptions` with
special `fvOptions` (in addition to those in `swakSourceFields`):

-   **executeFunctionObjectsFvOption:** this one does not manipulate
    the fields (as expected) but "only" runs a list of function
    objects. Application is for instance to get information
    during the time-step for algorithms that have multiple
    iterations in a time-step
-   **reportAvailableFvOptions:** Does no manipulation of the
    solution/equations. Simply reports that a callback has been
    called. Application: find out which `fvOption`-calls are
    used by a solver in which order without going through the
    sources
-   **reportMatrix:** Does not manipulate the matrix but prints a
    number of metrics


#### Macro expansion

For expressions and variable lists there is now the possibility
to expand values from the dictionary during reading. The two
characters that trigger this are

-   **$:** like the regular variable lookup in OpenFOAM.In its most
    complex form it looks like this `$[(type)spec]` where
    `spec` specifies where to look for the value (including
    scoping if the OpenFOAM-version supports it). The optional
    `type` specifies how the entry should be interpreted
-   **#:** in variable lists: `#otherList;` includes the variable
    list `otherList` instead of this entry

These expansions allow the construction of reusable snipplets
that include information from other parts of the case. For
instance (this is only part of the specification)

    #include "$FOAM_CASE/constant/g"
    vecName U;
    downComponent (
        "grav=$[(dimensionedVector)g];"
        "down=($vecName & grav)/mag(grav);"
    );
    variables (
        "#downComponent;"
        "maxDown=max(mag(down));"
    );

would calculate the component of a vector field that points in
the direction of the gravity (as specified in the `g`-file).

This expansion is done during the expression.


### Enhancements


#### `expressionField` now allows setting of dimensions

An optional entry `dimension` for the function object now allows
setting the dimension of the resulting field


#### New function `distToPatch` in Field-parser

This function gives the distance in the mesh to a specified face.

Not yet working correctly


#### `panicDump` now allows storing old values

With the switch `storeAndWritePreviousState` this functionObject
now can cache a number of timesteps and write these results in
addition to the current timestep


#### `swakExpressionAverageDistribution` records extremes and number of samples

In addition to the average now this Function object also records
the minimum and maximum value for each bin. Also the number of
samples used in the bin


#### Python-function objects that return a boolean now have a initialization-snipplet

The function objects `executeIfPython`, `setDeltaTWithPython`,
`setEntTimeWithPython` and `writeAndEndWithPython` now expect
code snipplet named `init`. This is executed in the beginning and
can be used to set up global variables if these snipplets need
some kind of state. The variables have to be declared `global` in
the snipplets that do the actual decision (for technical reason)


#### `fvOptions`-support in PDE-function objects

The function objects `solveTransportPDE` and `solveLaplacianPDE`
now call the regular `fvOptions`-callbacks (this allows modifying
the solution in the same way it can be modified for solvers)


### Examples


#### `other/simpleBendedPipe`

Example to demonstrate `funkyWarpMesh` and post-processing on
bended geometries


## 2015-05-31 - version number : 0.3.2

This is the last release to support OpenFOAM 1.7.x and older


### Future changes

These are things that will happen in the **next** release and will
break backwards-compatibility


#### Discontinuation of the `1.x`-branch

As version `1.7` is several years old this release will be the
last release that supports it.

Starting with this release the `port_2.x`-branch will become the
`default` branch and the `1.x` will become `legacy`


#### Fixing examples

Due to the umber of different supported Foam-versions not all the
examples run on all versions and it is not possible to fix them
in such a way that they work on all versions. In the next version
they will by adapted to use the `pyFoamPrepareCase.py` utility as
this supports having different Foam versions in one case. As a
consequence most cases won't run without an installed `PyFoam`


### Incompatibilities


#### Changed behavior of `groovyBC` during initialization

If `value` is unset then during initialization the value of the
internal field next to the patch is used. Previous behavior was
to set the patch to \(0\).

In most cases the new behavior should be better.


#### Changed semantics for boolean values in `internalField`

As OpenFOAM does not support geometric fields of type boolean
swak uses scalar fields for storing truth values (only in the
`internalField`-parser). The semantics was that a value of `0`
means `false`. Everything else `true`. Logical operators set
`true` to \(1\) and `false` to \(0\). As the test for equality to \(0\)
may sometimes fail this semantics is changed slightly: values
\(>\frac{1}{2}\) are interpreted as `true`. All others as `false`.

If operators were used properly this should not matter, but
expressions that assume for instance that `0.1` is `true` will
now fail. Overall this change improves the stability of logival
expressions


#### Better detection of single values

The improved detection of single values might break some
expressions. Especially for the Python-integration as values that
were previously passed as arrays are now being passed as single
values


#### Parser for kinematic parcels replaces `minParticleMass` with `minParcelMass`

This reflects a change in the API of the kinematic lagrangian
parcels. The old function has been discontinued to avoid
confusions with changed semantics


#### `SIGTERM` automatically switched on for parallel runs in `writeOldTimesOnSignal`

For parallel runs this signal is now automatically switched in so
that other signals are propagated to other processors


#### Changed bin-with in `swakExpressionAverageDistribution`

Due to a different calculation automatically calculated
bin-widths in this function object might slightly differ from
previous calculations


#### `funkyPythonPostproc` writes function object data to a different directory

For the swak-function objects the data is now written to a
directory whose name is based on the input dictionary instead of
`postProcessing`.

This might break scripts and other post-processing procedures
that rely on the data being in `postProcessing`.

Data generated during the run-time of the solver is unaffected


### New supported versions


#### Added support for Foam-Extend 3.2

Only minor adaptions. Mainly to accommodate the new debug switches


#### Added support for OpenFOAM 2.4.0

Compiles with OpenFOAM 2.4.0 (and 2.4.x)


### Internals (for developers)


#### Type of booleans changed for `internalField`

Results that were booleans used to report `bool` as their type
for the `internalField`-parser. Now they report
`volLogicalField`, `surfaceLogicalField` or
`pointLogicalField`. This makes them consistent with the
possibility of requesting these start symbols and specifying
logical fields in the parameters of a plugin function


### Bug fixes


#### `swakExpressionAverageDistribution` fails for small spans

If the span of the `abscissa` is too small (almost \(0\)) there
were numeric problems. In this case the bin-width is now extended
to \(1\)


#### Quantile calculations (including `median`) not correctly working in parallel

Reason was that `SimpleDistribution.calcScalarWeight` did not
`reduce`. This is now fixed (and a special switch is introduced
for one situation where this isn't desirable)


#### `PDE`-function objects broken because of `fvOptions`

On OpenFOAM-versions with `fvOptions` these were broken because
of inconsistent dimensions.

This is fixed: now if a `fvOption` is defined the
function-objects also needs `rho` and this is passed to the call
of `fvOption` when generating the source term


#### Restart in `binary` not working (with global variables)

As reported in
<https://sourceforge.net/p/openfoam-extend/ticketsswak4foam/225/>

Problem seems to be that the `HashPtrTable` is not correctly read
in `binary`-mode. Workaround is to switch to `ascii` before
writing and reading that table and then switch back to the
original mode


#### Single values were passed as arrays

Single values like `min(expr)` were passed as arrays instead of a
single value. This is now fixed. The problem is that now
expressions that rely on the old behavior might break


#### Distributions not written if there is only a single bin

If all data falls into one bin of the distribution then the
distribution was not written. This is now fixed


#### `weightedAverage` for `AccumulationCalculation` fails for zero-size fields

Fixed


#### `writeOldTimesOnSignal` fails with `writeCurrent`

If the current time was already written (because it is part of
the stored times) a segmentation fault occurred. This has been
fixed by checking whether the current time is already stored


#### Turbulence plugin-functions did not correct the boundary field

This has been fixed


#### Some `maintainanceScripts` do not work under Python 3

The scripts

-   `makeSwakVersionFile.py`
-   `makeCopyrightEntries.py`

have been adapted so that they now work with Python 2.x as well
as Python 3


#### Function `distToPatch` now working

This function for the field parser gives the distance in the mesh
to a specified patch. *Distance* is defined as minimial sum of
cell-to-face distances needed to reach this cell from the named
patch


#### Not all `processorX` written in `writeTimesOnSignal`

If a `SIGSEGV` or `SIGFPE` were encountered only on some
processors then on the others no data was written because these
signals do not propagate to other processors

Now the signal handler raises a `SIGTERM` which is propagated to
the other processors. For parallel runs a signal handler for
`SIGTERM` is automatically set up to make sure that on all
remaining processors data is written


### Infrastructure


#### Script for generating releases

Provided by Alexey Matveichev

The script `maintainanceScripts/make-releases.sh` rolls a release
and puts it into the directory `releases`


#### Automatic `swakConfiguration`

Provided by Alexey Matveichev

Automatically finds the linking options for Python with
`python-config`


### New features


#### Additional distance-functions in field-parser

Similar to `distToPatch` there are now two additional built-in
functions:

-   **distToCells:** A logical field is provided. The distance to the
    cells for which this field is `true` is
    calculated
-   **distToFaces:** Same as `distToCells` but with a logical
    function for the faces

Both functions return cell-values


#### New function-plugin `swakMeshWaveFunctionPlugin` for mesh related distances etc

This library collects functions that are similar to
`distFromPath`: they use the `MeshWave`-algorithm to calculate
distances and connectivity of the mesh.

Functions in this plugin are:

-   **meshLayersFromPatch:** Calculates for each cell how many mesh
    layers it is away from a patch
-   **meshLayersFromCells:** Calculates the number of layers from a
    set of cells selected by a logical expression
-   **meshLayersFromFaces:** Calculates the number of layers from a
    set of faces selected by a logical expression
-   **floodFillFromCells:** Number connected regions. Cells selected
    by a logical expression separate these regions
-   **floodFillFromFaces:** Number connected regions. Faces selected
    by a logical expression separate these regions
-   **cellColouring:** "Colors" the cells in such a way that no two
    neighboring cells have the same number. Mainly
    for visualizing the mesh
    structure. "Neighboring" means "cells sharing
    a face" not an edge. Therefor it may look at
    the surface like there are cells with the same
    color


#### Additional function in `swakMeshQualityFunctionPlugin`

Additional functions:

-   **mqCellFaceNr:** Number of faces for each cell
-   **mqCellShape:** Field with numbers describing the cell
    shape. Values are:
    -   **1:** Hex
    -   **2:** Prism
    -   **3:** Wedge
    -   **4:** Pyramid
    -   **5:** Tetraeder wedge
    -   **6:** Tetraeder
    -   **0:** no classification. Polygon


#### Function object for developers that raises a signal at the defined time

The function object `provokeSignal` can be used to debug signal
handling. No other possible use is known.

The function object allows defining the signal, the time and - in
case of parallel runs - the processors on which the signal should
be raised


#### New function-object `executeIfSetOrZone` that only executes if sets or zones are present

This function object executes a lost of function objects if
face/cell/point sets or zones are present. The required zones and
sets are specified in a dictionary `setsAndZones`: keys are the
names of the entities the values are the types (like
`faceZone`). Only if **all** objects specified there are present
the function objects are executed.

As a side effect if `loadAndCacheMissingSets` is `true` than sets
that are not yet found in memory are loaded from
disk. **Attention**: if no set of that name is found on disk then
it is searched at every time-step. This is a potential
performance problem (it is not a problem if the set is there)


#### New function object `executeIfPatch` that only executes if patches are present

This function object executes a lost of function objects if
a list of patches is present.


#### Function object `listMeshData` displays data stored in mesh

This function object shows data that is stored in the
mesh. Mostly `solverPerformance` with the residuals of the linear
solvers


#### Function object `solverPerformanceToGlobalVariables`

This function object gets a list of `fieldNames` and gets the
information about their solver performances. The three quantities
that it gets are

-   **`nIterations`:** the number of iterations
-   **`initialResidual`:** residual before iterating
-   **`finalResidual`:** residual after iterating

For each value three variables are created (assuming the field is
named `foo`):

-   **`foo_nIterations`:** array with all the values. This is usually
    only usable in the Python-integration
-   **`foo_nIterations_first`:** value of the first solver
    performance
-   **`foo_nIterations_last`:** value of the first solver performance


#### Function objects for manipulating `fvSchemes` and `fvSolution`

`simpleFunctionObjects` now has a base class that allows
manipulating `fvSolution` and `fvSchemes` in-memory but the
solver behaves as if the file changed on disk. Application for
this is changing parameters like relaxation or discretization
schemes during the simulation. This can be used for instance to
start the simulation with 'safe' settings and switch to faster
convergence later.

There is currently only one function object
`timeDependentFvSolutionFvSchemes` that implements this (more are
planned). It allows switching to different subdictionaries at
specified time. An example can be found at
`Examples/manipulateFvSolutionFvSchemes/pitzDailyTimeSwitched`.
This function object gets a list of times and
sub-dictionaries. At the specified time the values from the
sub-dictionary overrides the values in the original dictionary
(Values not present in the sub-dictionary are left untouched). If
`reset` is specified as the name for the sub-dictionary then the
dictionary is set back to the original state.


### Enhancements


#### `groovyBC` now has a better initialization if `value` is unset

Until no if `value` was not set `groovyBC` automatically
initialized the boundary to \(0\) until the actual expressions were
evaluated for the first time. This raised problems when people
for instance didn't initialize `T` to \(0\) and the density
calculation produced a *division by zero*.

Now if `value` is unset then the value of the patch is
initialized from the value of the internal field next to the
patch. This doesn't protect from the internal field being


#### Function objects `writeFieldsOften` and `writeAditionalFields` now write Lagrangian clouds

With an additional optional argument `cloudNames` a list of
clouds to be written can be specified like the fields.

This feature was implemented by E. David Huckaby


#### Added option `-functionPlugins` to `funkySetFields` and other utilities

For utilities for which this is appropriate this allows loading
function plugins from the command line (without adding them to
the `libs`-entry in the `controlDict`). It is assumed that the
name of the library starts with `libswak` and ends with
`FunctionPlugin.so`.


#### Optional parameter `functionPlugins` for parsers created with a dictionary

If a list of words named `functionPlugins` is found then the
string `libswak` is added in front and `FunctionPlugin.so` is
added at the end and a library of that name is loaded to add the
functions provided there


#### Additional signals added to `writeTimesOnSignal`

Now it is possible to handle `SIGUSR1`, `SIGUSR2` and
`SIGTERM`. Only needed for testing


#### `pythonIntegration` does nothing if no code is present

If a code is a string of the length \(0\) nothing is done. This is
especially important as no global namespaces are read (which
**may** not be created at the time. Especially during initialization)


#### `funkyDoCalc` uses `weight` and `mask`

If a dictionary specifies the entries `mask` and `weight` then
these are evaluated and used to mask and weight the data
specified in `expression`


#### `funkyDoCalc` allows setting bin width and number

If specified in the sub-dictionary the entries
`distributionMaxBinNr` and `distributionBinWidth` allow setting
the number of bins. These values are not used exactly but give
the distribution class a hint. `distributionMaxBinNr` takes
precedence


#### Timeout in `writeOldTimesOnSignal` to allow other processes to finish writing

For parallel runs the process that initiates the writing of data
waits for the other processes to write their data. This time can
be specified with a parameter `sleepSecondsBeforeReraising` that
has a default value 60


#### `writeAndEnd`-function objects now allow storing and writing previous time-steps

The function objects based on `writeAndEnd` now have an optional
switch `storeAndWritePreviousState`. If this is set then a number
of old time-steps can be stored and will be written when the run
is terminated.


#### `fvOption`-subclasses now support additional forms of `addSup`

In 2.3.1 two new forms of `addSup` where added to `fvOption` (one
for compressible equations. The other one for multi-phase
solvers). Both new forms are supported by all
`fvOption`-subclasses in `swakSourceFields` and `swakFvOptions`


#### `swakExpressionAverageDistribution` allows specifying the limits

When the switch `dynamicExtremesAbscissa` is `false` then the
limits of the distribution can be set with `minAbscissa` and
`maxAbscissa`. This mainly influences the bin-width. If values
fall outside this range then the distribution is extended to
accommodate these values


#### Function objects derived from `simpleDataFunctionObject` now can write to a different directory than `postProcessing`

This involves almost all function objects in swak that write data
to `postProcessing`. The library now allows setting a different
default. This value can also be set with an optional entry
`postProcDir` on a per-function-object basis.

This change is used in the `funkyPythonPostProc`-utility to set a
directory name that is based on the name of the input dictionary
(or set with an option). Now if there are function objects used
they will write their data to this directory instead of
`postProcessing` (leaving data generated during the running of
the solver alone)

**Note:** This only works for function objects from swak. "Regular"
 function objects will still write to `postProcessing`


#### `funkySetFields` allows correcting the boundary conditions afterwards

An additional option `-correctResultBoundaryFields` corrects the
boundary conditions before writing to disk. In dictionary mode
there is also an option `correctResultBoundaryFields`.

This is only needed in rare cases.


### Examples


#### `icoFoamCavityWithParcel` as a minimal example for adding lagrangian particles

This case demonstrates how to add lagrangian particles to the
simplest tutorial-case


## 2016-07-17 - version number : 0.4.0

This is the first release that does not support OpenFOAM 1.7.x,
OpenFOAM 1.6-ext and older versions anymore


### New supported versions


#### Rolling support for `OpenFOAM-dev`

This version adds support for `OpenFOAM-dev` published at
`git://github.com/OpenFOAM/OpenFOAM-dev.git`

As this version is constantly changing it might or might not work
with the most up-to-date version of the repository.

The port is maintained in a branch `feature/port_of-dev` that is
never closed and is periodically merged to the `develop`-branch.

Internally the `dev`-version is maintained as version `9.9.99`


#### Support for OpenFOAM 3.0

Support for this version has been added (basically all the fixes
from the `dev`-branch were guarded with the correct version


#### Support for `foam-extend` 4.0

This version is supported by `swak4Foam` (compiles and runs)


#### Support for the OpenFOAM v3.0+ version

This version is supported. For the time being it can be assumed
that this version is similar to the regular OpenFOAM 3.0


### Bug fixes


#### `fieldReport` doesn't work with all field names

Fields that have names that are not considered proper variable
names by swak (because for instance the have a dot in the name
like `alpha.water`) were not handled by `fieldReport`. This has
been fixed


#### Construction of `SimpleDistribution` may fail for zero-size fields

The reason is the initialization with `pTraits::max` and that
during the calculation of `mag` this causes an overlflow. Fixed
by scaling down to the square root of the maximum


#### Wrong type for cloud in `writeFieldsGeneral`

Because of this compilation in 2.2 failed. Fix provided by Bruno Santos


#### `solverPerformanceToGlobalVariables.C` did not compile with OF 2.2

The reason was that the `solverPerformance`-class is hidden in
`lduMatrix`. Fix provided by Bruno Santos


#### Solid thermophysics not available in the `ThermoTurb` function plugin

For OF-versions where `fluidThermo` and `solidThermo` have the
same base functions like `thermo_Cp` can be used for solids too


#### `lookup`-tables no working in `faField`-parser

Because of a bug in the lexer lookup tables did not work. Fixed


#### `swakDataEntry` not correctly written for some versions of OpenFOAM

The macro that guarded the code needed for some OpenFOAM-versions
did not include OpenFOAM 3.0. Fixed


#### `executeFunctionObjectsFvOption` always executed for `makeRelative` and `makeAbsolute`

The wrong flags were checked and the function objects were always
executed when even if `doMakeAbsolute` and `doMakeRelative` were
set to `false`. Fixed


#### Does not compile with `WM_LABEL_SIZE=64`

A reference to a `label` that was passed to a `flex`-function
broke compilation because that expects a 32-bit integer. Fixed


#### Tensor-fields not correctly handled by `replayTransientBC`

Due to a copy/past-problem all tensor-fields (including
symmtrical and spherical) would have caused the utility to
fail. Fixed


#### Surfaces created by `createSampledSurface` not updated during mesh movement

The function object ignored the callbacks for moving meshes. It
now implements them and forces a regeneration of the surface if
the mesh moves


#### `funkySetFields` does not set dimensions

The `dimension`-entries were read but not set. This is now fixed


#### `funkySetBoundaryField` does not compress files

The utility now reads the compression state of the original and
sets the resulting file accordingly


#### `swakExpression` not working for point fields

Point fields were not correctly returned. This is fixed


### Infrastructure


### Documentation


#### Adaption of the reference guide for new `org-mode`

New version of `org-mode` makes changes in the UML-code necessary


### New features


#### 2D-lookup tables supported in grammars

Like the optional list `lookuptables` it is now possible to have
a list `lookuptables2D` with 2-dimensional lookup tables. A table
with the name `tbl` can be used like this in expressions:
`tbl(expr1,expr2)`. `expr1` and `expr2` must evaluate to scalar
values of the same entity (cells, faces, points &#x2026;) and the
expression will evaluate to a scalar.

This feature is based on the `interpolation2DTable` that is part
of OpenFOAM since 2.2 but there is a modified version of this
class included in `swak4Foam` as there is a problem with the
output in the original version. Currently `foam-extend` is not
supported as this is missing some base classes as well
(`tableReader`)


#### Added a function plugin for dynamic meshes

The Plugin `DynamicMesh` adds functions to visualize the mesh
movement. The functions are

-   **dyM<sub>meshPhi</sub>:** the mesh `phi` as stored with the mesh. Some
    distributions write this field anyway
-   **dyM<sub>meshCourant</sub>:** Mesh courant number. Replicates the number
    some solvers print automatically
-   **dyM<sub>meshVelocity</sub>:** Velocity with which the cells are
    moved. Reconstructed from the mesh `phi`

These functions might failed if the mesh has not been moved yet
so their calls might have to be encapsulated in a
`executeIfStartTime` function Object


#### Function object to load `cell`, `face` and `pointSet`

The function object `loadTopoSets` loads all the available sets
in the mesh into memory for expressions that need them. Also
allows automatic writing of these sets (switched by `writeSets`)


#### Function object to create and manipulate `cell`, `face` and `pointSets`

The function objects `manipulateCellSet`, `manipulateFaceSet` and
`manipulatePointSet` allow the manipulation of these
`topoSet`. They do so by evaluating an expression `mask` and all
cells (faces, points) for which the expression is `true` will be
part of the topoSet. If the topoSet is not present in memory the
function object fails unless the parameter `createMissing` is set
to `true`. The entry `writeManipulated` (default: `true`) says
whether the topoSet should be written at write times.

An example of this functionality is found in `Examples/other/topoSetDamBreak`


#### Function object that executes other function objects only at write time

The function object `executeIfWriteTime` in the
`simpleFunctionObjects` executes a list of other function objects
only if the current time is a scheduled write time. The purpose
of this function object is to only execute function objects at
write times that don't honor the
`outputControlMode`-setting. Also does the `else`-branch allow
executing only when **nothing** is written


#### Added a function plugin for MRF cases

This function plugin makes a number of function from the
`MRF`-model available:

-   functions to make fields absolute or relative in the `MRF` region
-   a function to calculate the Coriolis force

All these functions call the MRF-model


#### Added library `simpleCloudFunctionObjects` with function objects for particle clouds

These function objects can be added to particle clouds. In this
first version these objects can be added:

-   **cloudMoveStatistics:** Records simple statistics about the
    solution of the cloud:

    -   How often were faces hit by particles (total, per particle:
        minimum, mean and average)
    -   How often were particles moved (same data as face hits)
    -   How often was each patch hit

    Only minimum effort has been spent to consolidate this data
    across processors (particles may be counted on multiple
    processors).

    The main purpose of this function object is to spot problematic
    behavior (particles that hit patches repeatedly and don't
    move)

    The optional parameter `reportHitNr` and `reportMoveNr` print
    out the ids of particles that exceed these thresholds

-   **eliminateCaughtParticles:** Sometimes particles get caught in
    "endless loops" (hitting the same patch over and over again
    without significant movement). This may slow down
    simulations significantly as basically the same impact is
    calculated over and over again without any progress. Two
    parameters allow eliminating such parcels:

    -   **maximumNumberOfHits:** if the same boundary face is hit more often
        than this by the particle (without hitting any other
        faces) then the particle is eliminated. Recommended value: 1000
    -   **minDistanceMove:** Minimum distance a particle has to move
        when hitting a boundary face. If it moved less the
        particle is removed. Recommended: \(10^{-9}\)

    If one of the parameters is set to a negative value the
    criterion is "switched off"
-   **eliminateOutsideParticles:** Removes particles that are outside
    the mesh (are not inside any cell). The purpose of this is
    to eliminate particles that are for some reason not
    correctly tracked
-   **correctParticleCell:** Correct the cell of the particle before
    the evolution begins.

    This should fix problems with moving meshes but is currently
    not working correctly.

    An optional parameter `logCorrected` logs the data of the
    corrected particles to a file. Only use for debugging or if you don't
    expect many incidents
-   **traceParticles:** This function object gets a list of particle
    ids. For these particles all data is written whenever a
    function object is called.

    This function object should **only** be used for debugging if the
    behavior of a special particle should be tracked


#### Add library `swakCloudFunctionObjects` with function objects that use expressions for clouds

These function objects can be added to particle clouds and use
expressions. The function objects are

-   **eliminateBySwakExpression:** Evaluates an expression for each
    particle. If the expression is `true` then the particle is
    eliminated. Expression can be evaluated before or after the
    moving of the particles


#### `streamFunction` added to `VelocityField` function-plugin

A function that calculates the stream function in a 2D simulation has been added


### Enhancements


#### Added function for `smooth`, `sweep`, `spread` to `FvcSchemes`-function plugin

These functions from `fvc` were implemented as
plugin-functions. Mainly to be able to test their behavior. To
check what they do see `fvcSmooth.H` in `finiteVolume`


#### Additional functions in plugin `ThermoTurb`

Functions from the public interface of `basicThermo` and
`turbulenceModel` have been added (like `kappa`, `kappaEff` etc)


#### Conditional function objects now allow construction of their `functions` at start

With the parameter

    constructUnconditionally yes;

in function objects that start with `executeIf` it is possible to
construct the lists unconditionally at start instead of waiting
for the condition to be true. If unset then the function objects
default to the old behaviour


#### `manipulateField`-function object can switch **off** manipulating at start

The regular behavior for this function object is that it does
the first manipulation during construction. This can be switched
off with the setting `manipulateAtStartup` in cases where not all
the requirements for correctly evaluating the expressions are
fulfilled during construction


#### `replayTransientBC` now handles surface and point fields

The utility can now read surface and point fields. Surface-fields
are not corrected because this is not supported by Foam


#### `replayTransientBC` reads `fvOptions`

The utility now reads `fvOptions` for FOAM-versions that support
it. They have to be switched on with 'useFvOptions' as most of
the tine they are not needed (only with function objects that
solve equations)


#### PDE-function objects now have additional `solveAt`-option `never`

When this option is set then the function-objects are read
(including the field) but never solved


#### `replayTransientBC` reports execution time

The utility now reports the time. This is useful when using the
utility to test function objects


#### Added parameter `sourceImplicitUseSuSp` to `PDE`-function objects

The function objects `solveLaplacianPDE` and `solveTransportPDE`
now have a new parameter `sourceImplicitUseSuSp` that allows
switching from using `Sp()` (the default) to `SuSp()` for the
addition of `sourceImplicitExpression`


#### `replayTransientBC` can read and update dynamic meshes

The command line option `-useDynamicMesh` tells the utility to
read the mesh as a dynamic mesh and update it at every time-step


#### `solveTransportPDE` function object now aware of mesh movement

If the function object detects that it is created with a moving
mesh then it asks for a parameter `makePhiRelative` to specify
whether `phi` should be made relative to the mesh movement (this
is usually necessary). Then either a `velocityExpression` or a
`velocityName` has to be specified as well and `phi` is corrected
according to the mesh movement

If `steady` is `true` then `phi` is **not** made relative


#### `funkySetLagrangianFields` allows creating fields of arbitrary types

If a cloud is created and in en entry in `expressions` the entry
`class` is found then it is assumed that a field of that type
should be created. The `expression` is not evaluated but added to
the cloud for each particle.

**Note:** this does not do **any** semantic checking and is only to
be used to set constant values for required fields for which
`swak` can not evaluate the type


#### Unconverged steady solution of PDE-function objects are reset

For some reason the linear solver does not converge on the
`solveTransportPDE` and `solveLaplacianPDE` function
objects. This messes up the solution field. In these cases the
solution field is reset to the last known good solution. If this
behavior is undesired it can be switched off with
`restoreNonConvergedSteady`


#### Additional output options for function obkjects based on `simpleFunctionObjects`

There are now three more options for `outputControlMode`:
`outputTimeAndStartup`, `timeStepAndStartup` and
`deltaTAndStartup`. These work like the other options but write
data at the startup-time as well


#### `dumpSwakExpression` now works in parallel as well

This function object used to only dump values from the master
processor if used in parallel. Now the values from all processors
are dumped in the order of the processors


#### Missing properties `age` and `dTarget` added to KinematicCloud parser

Two properties that were missing for that cloud type are now
available in the parser


### Examples


#### `solverPDE/pitzDailyWithPDE` extended

This case has been modified to a version that was presented at
the PFAU (Austrian User Group Meeting) in 2015. Now it compares
with the `scalarTransport` function object that was added to
OpenFOAM.

The case now also calculates the time the fluid needs from the
inlet to the outlet (`residenceTime`). Steady as well as
transient


## 2017-05-31 - version number : 0.4.1


### New supported versions


#### Support for OpenFOAM v1606+

This version is supported in the this version of
`swak4Foam`. There is only one problem: there were changes to the
`tmp`-template. These changes have been taken care of in the
`swak4Foam`-sources. **But** there are two source files in the
distro that need to be modified (basically adding `.ref()`
instead of `()` in three places:

    src/finiteVolume/finiteVolume/fvc/fvcD2dt2.C
    src/finiteVolume/fvMatrices/fvMatrix/fvMatrixSolve.C

Therefor the officially released version will **never** compile
with `swak4Foam` but this is nothing that can be fixed in
`swak4Foam` (the patch is found in the directory
`distroPatches/OpenFOAM-v1606+`)


#### Support for OpenFOAM 4.0

This version is supported in this version of `swak4Foam`. There is
one problem: the released version has two problems that mean that
`swak4foam` will **never** compile on the released version.

Patches fixing these issues are found in the directory
`distroPatches/OpenFOAM-4.0`


#### Support for OpenFOAM 4.1

This version is supported in this version of `swak4Foam`. No
modifications are required


#### Support for OpenFOAM v1612+

This version supports OpenFOAM v1612+. The modifications were
done by Mark Olesen


### Incompatibilities


#### Changed location of files by `simpleFunctionObject` and subclasses for multi-region-cases

The location of files written by these function objects (which
are almost all function objects writing data in the
`postProcessing`-directory) has changed: now if the function
object belongs to a region that is not the default mesh the
output is written to a sub-directory with the name of that mesh.

This might break scripts or other tools that people use to
process that data


### Bug fixes


#### Destruction of `dummyPhi` causes error message

On some distros the destruction of a field stored in an `autoPtr`
**after** the destruction of the mesh causes an error message. This
has no effect but is annoying. Fixed


#### `functionObjectListProxy` does not honor `outputTime`

The function object was executed every time not matter what
`outputControlMode` was set. Fixed


#### Plugin functions with `bool` results not supported by all drivers

Until now only the `internalField` and the `faInternalField`
supported boolean results. Now all drivers support them


#### No plugin function for `cloud` possible

Because the static variable/method `driverName` was not specified
no plugin functions for clouds could get specified


#### Wrong syntax in primitive types of plugin-function causes C++-error

If a primitive function for instance expects a label and finds an
unscanable string like `x23` then this used to generate a failure
by the C++-library. This gave no indication on the real
problem. Now there is a OpenFOAM-error message with a somewhat
better explanation


#### Initialization function objects in `simpleFunctionObjects` fail in parallel

Because of the wrong order of initialization of these function
objects they failed in parallel. Fixed


#### Writing `storedVariables` fails for single values in parallel

This war reported by Philippose Rajan (who also provided a patch) at
<https://sourceforge.net/p/openfoam-extend/ticketsswak4foam/237/>

In parallel if the values was marked as a single value but no
part of the underlying entity was on the processor it was tried
to access the first element of an empty field. The fix was to
write the value \(0\) instead. This fix has been adapted so that
the actual uniform value is written to the file (and read as well)


#### `executeIfOpenFOAMVersionBiggerEqual` not correctly working with alphabetic patch versions

If the patch version of the current OpenFOAM-version is not a
number and not `x` then this function object failed


#### `mqFaceMaximum` and `mqFaceMinimum` in the `LocalCalculations`-plugin fail for patch types without values

As patch types like `empty` have no values stored this plugin
failed. Now it checks if there is data


#### `simpleFunctionObject` did not pass parent directory correctly

When storing the data for the function object no reference to the
parent directory was stored and therefor scoped macro-expansion
did not work anymore


#### `ThermoTurb` functionPlugin not working with new `thermoType`-format

The thermophysical functions in this plugin did not work if
`thermoType` was a dictionary. Fixed

Also: construction of turbulence now gets the rho-field directly
from the thermophysics


#### `solidThermo` instances could not use `basicThermo`-methods in `ThermoTurb`-plugin

Reason was that only two specific thermo-subtypes of
`fluidThermo` were searched if the method was not part of
`solidThermo`. Now also the basic type is searched


#### Global variables recognized but not correctly used by some drivers

The drivers for sampled entities, sets and zones found global
variables but were not able to use them correctly. Fixed


#### Cloud function-objects that need a parser not working

The reason is that when constructing the `CloudProxy` a cast is
made to the *assumed* cloud type which in the case of cloud
function-objects is not the *actual* type. To avoid this separate
instantiations of the `CloudProxy` were added


### Internals (for developers)


#### `ExpressionResult` now stores single values separately

To fix a problem with writing of single values in parallel (see
above) a `union` was added to store the single values. The reason
for this is to avoid a second parallel communication when writing
the value. This adds memory to `ExpressionResult` (in the order
of a `tenor` object). The memory is not the problem but that the
single value is saved twice (the other time is in the regular
value field)

For compilers that do not support the C++0x-standard instead of a
`union` a `struct` will be used because non-primitive members for
`unions` are only supported for that standard. This will lead to
a slightly higher memory usage (but still small compared to full
fields)


#### Rewrite of `functionObject` removes the `start()`-method

OpenFOAM-versions based on the foundation release 4.0 and later
don't have a `start()`-method in the function objects
anymore. Lot of functionality of swak relied on this method. To
make swak compatible across versions `start()` is now called in
the constructors of the function-objects (if the OpenFOAM-version
requires it)


### Infrastructure


#### Add the development branch to the version string in utilities

If the compiled version is not a released one then the info of
`hg branch` is added to the version output that utilities like
`funkySetFields` print


### Documentation


#### Added *Compatibility matrix* for examples

Different OpenFOAM-versions have different formats for the input
files. This means that examples that work for one version will
break with another version (but is still useful to see how things
are done). The file
`Documentation/examplesCompatibilityMatrix.org` gives an
(incomplete) overview on which versions run which examples (some
examples will never run because a certain feature might be
missing in that Foam-version)

Currently 4 Foam-versions are tested

-   OpenFOAM 5.0 (the Foundation release)
-   Foam-extend 4.0
-   OpenFOAM+ 1706
-   OpenFOAM 2.3 (to check compatibility with old versions)

Other versions might or might not work. The first three will be
replaced with the latest version of that fork once it becomes
available


### New features


#### Function plugin `ShiftField` that allows shifting volume fields

When adding the library `libswakShiftFieldFunctionPlugin.so` to
`libs` in the `controlDict` functions are added that allow
shifting volume fields by a specified distance. One example
would be an expression

    "shiftConstantScalarField(T,(1 1 0))"

which generates a new `volScalarField` where the value at `(0 0
     0)` is the value of `T` at the location `(1 0 0)`.

The scheme for naming the functions is
`scalar<ShiftType><ValueType>Field` with

-   **ValueType:** one of the base types `Scalar`, `Vector`,
    `Tensor`, `SymmTensor`, `SphericalTensor`
-   **ShiftType:** how the shift is specified. Possible values are either
    -   **Constant:** A constant vector (like the example here)
    -   **Expression:** An expression that gives a `pointVectorField`
        (for instance `interpolateToPoint(time()*vector(0,0,-1))`)

These functions work by generating a new mesh that is shifted as
specified (in the inverse direction) and interpolating between
this and the original mesh. This is computationally **very**
expensive and therefor not recommended in evaluations that occur
very often.

Cells that have no shifted equivalent value are set to
zero. Boundary patches are not interpolated. Both of these
behaviors can cause weird results.

There are also variants with `Default` in the end. These allow
setting different default values than \(0\)

The functions with the names `map<ShiftType><ValueType>Field` map
the value of the nearest cell. This is more stable but might
coarsen the field

The additional `findShifted<ValueType>Field`-functions do the
mapping by avoiding the `meshToMesh`-functions of OpenFOAM: the
shifted value is found by "looking" into the original cell that
contains the point that is the cell center plus the displacement
value. If that point is not in the mesh a default value is used


#### Library `swakStateMachine` to easily implement switching in cases

This library implements *State machines*. This is a convenient
way to store a state in a simulation and switch between them by
predefined rules. An example for states in a *State machine*
would be: `closed`, `valveOpening`, `open`, `valveClosing`

The library has the function objects and plugin functions to use
*State machines*.

The chapter *State machines* in the *Incomplete swak4Foam
reference* describes it in more detail


#### Function object `stateMachineFvSolutionFvSchemes` to switch numerics accorrding to a state machine

This function object allows modifying `fvSchemes` and
`fvSolution` according to the states of two state machines.


#### Function plugin `Quantile` for calculations with the distribution of a function

All the functions in this plugin first calculate a
distribution. This distribution is calculated from the first parameters:

1.  the field for which the distribution is calculated
2.  the weight with which the cell values enter the
    distribution. Usually `vol()` (volume weighted) or `1` (equal
    weight for all cells)
3.  a logical expression that specifies which cells are used
    (`true` for all cells)
4.  the number of bins to use for the distribution. This specifies
    how fine the distribution is calculated. Should be small
    compared to the number of cells. Usually the values `100` or
    `1000` should be sufficient

The implemented functions are

-   **fracSmallerThan:** Writes a fraction in the range from \(0\) to
    \(1\) with for instance \(0.5\) meaning "50 % of the
    distribution is smaller than the value in this cell". The
    cells which are masked out get the value \(-1\).
-   **fracSmallerThanFromOther:** Like `fracSmallerThan` but there is
    an additional parameter: the field that is used for
    calculating the fraction. Here the mask is not used for
    calculating the fraction (but still used for calculating the
    distribution)
-   **quantileFromOther:** Basically the inverse of
    `fracSmallerThanFromOther`. The last parameter is a fraction
    (from \(0\) to \(1\)) and the result is the corresponding
    quantile: if a fraction \(0.5\) is specified then the value
    from the distribution for which 50% are smaller is written
    to the cell

Note: values may slightly vary according to the number of
bins. But the bin number also increases the calculation time.


#### Function `outputTime()` to indicate output time

A function `outputTime()` has been added to the parsers. This
function is `true` if Foam is writing at the current time and
`false` else


#### Utility `writeBoundarySubfields` to write subfields of boundary conditions

Sometimes to debug problems with boundary conditions it is good
to **look** at fields like `refValue` in Paraview (for instance
because `groovyBC` might not have set that value the way we
expected it). For a field `foo` the utility would write a field
`foo_bar` if the user specified `bar` to be written. The interior
of the field will be \(0\), patches where `bar` is defined will be
`fixedValue` with the value of `bar`. All others are
`zeroGradient`

A string with the specificion of the subfields to create fields
for is set with the option `-subfields`. The specifications are
separated by `,`. Each specification is of the form
`<name>:<type>` (with type being `scalar`, `vector`, `symmTensor`
or `sphericalTensor`. A type `native` assumes that the same type
as the field should be used). If unset the default value is used:
the specification to write all subfields usually associated with
the `mixed` boundary condition

It is assumed that the length of the sub-field is equal to the
number of faces in the patch


#### Library `swakDynamicMesh` with "swakified" dynamic mesh subclasses

This libraries collects specializations of dynamic mesh classes
that work together with OpenFOAM. Currently these classes are in
the library:

-   **swakMotion:** this is a solid body motion solver for the
    `solidBodyMotionFvMesh` where the translation and the
    rotation of the solid body can be specified by `swak`-expressions
-   **groovyCyclicACMI:** this patch type is a subclass of
    `cyclicACMI` where the faces that cyclic can be switched on
    and off with an additional field that is specified with the
    parameter `openField` in the specification in the
    `boundary`-file. This field has to be present in memory at
    the first time-step (usually a
    `readAndUpdateFields`-functionObject is a good way to
    achieve this). The values on the corresponding
    `nonOverlapPatch` of the first boundary in the coupled pair
    is used to switch faces on and off: if the value of the face
    is \(1\) then it is 'open' if it is \(0\) it is 'closed'
-   **nonMovingFvMesh:** A non-moving mesh that pretends to
    move. This triggers the recalculation of AMI-interfaces
-   **groovyBCJumpAMI:** allows implementing a jump condition on an
    AMI-interface that depends on an expression evaluated on the
    master patch of the interface

The folder `Examples/DynamicMesh` has examples for this library


#### Record current residual of the equations with `fvOptions`

The two `fvOptions` `matrixChangeBefore` and `matrixChangeAfter`
can be used to record the residual or the change of it.

The residual \(r\) for a field \(\phi\) which is supposed to be the
solution of a linear equation \(A \phi = b\) is \(r=A \phi-b\)

`matrixChangeBefore` calculates this for a \(\phi\) specified in
`fieldName` and uses for \(A\) and \(b\) the current `fvMatrix` of
that field. The result is stored in a field whose name is
composed of the value of `namePrefix` and `fieldName`. This field
can be useful to see where the current solution does not fit the
linearized system

`matrixChangeAfter` does the same calculation but assumes that
the field already exists and subtracts the result from the
previous value. So if another `fvOption` was applied to the
`fvMatrix` between these calls then the field has the change of
this matrix. If "only" a source term was added then this field
corresponds to the source term.

In principle `fvOptions` are executed in the order in which they
are specified. If the setting `doAtAddSup` is set to `true` then
it is executed in an early phase (when source terms are set). If
set to `false` then the `fvOption` is executed during the
`setValues=/=constrain` phase.


#### Setting time-step with an expression: `setDeltaTBySwakExpression`

This new function object in `simpleSwakFunctionObjects` evaluates
a `scalar` expression and sets `deltaT` accordingly. Only works
on OpenFOAM-versions that support `DataEntry` or its successor
`Function1Types`.


### Enhancements


#### Primitive parser for Plugin-functions now supports tensor types

The parser for `primitve` types now supports `tensor`,
`symmTensor` and `sphericalTensor` as well.


#### Plugin-functions now can use `variables` (under certain circumstances)

For an expression `fooFunmction(var+2)` where `fooFunction` is a
Plugin-function the symbol `var` can now be a variable: if in the
parent dictionary there is a sub-dictionary `fooFunctionData`
then this dictionary is searched for a `variables`-entry and
these variables are then evaluated (it is also possible to have
`lookuptables` and similar in `fooFunctionData`)


#### Timeline function objects now optionally write value at start time

Function objects that inherit from `timelineFunctionObject` (like
`swakExpression`) now have an optional parameter
`writeStartTime`. If set to `yes` then the value at start-time
will be written. The default setting is `no`. This is similar to
the old behavior and avoids crashes at startup. Some classes
change this default to make it `yes`


#### Additional functions in the radiation function plugin

Additional functions for the `absorptionEmissionModel` and the
`sootModel` are added to to library with the radiation-functions


#### Improvement of `readAndUpdateFields`: surface fields, output, switching

The function object `readAndUpdateFields` got three improvements:

-   an optional switch `correctBoundary` allows switching the
    correction of boundary conditions off
-   surface-fields are now read as well. But no boundary correction
    is performed as the these fields do not support this
-   better output. Instead of silently reading fields or using
    those in memory the name of the field and the type are written
    out


#### `executeIfStartTime` now also allows execution if it is "only" a restart

Normally the function object only executes only if this is the
real start time (not a restart). If the option `executeOnRestart`
is set then the function object also executes if this is a restart


#### `reportAvailableFvOptions` now reports `constrain`-methods

On OpenFOAM-versions that support it this now reports if
`constrain` is called for an equation


#### `constrain` method added to `fvOptions`

In earlier OpenFOAM-versions the name of this method was
`setValue`. To keep source compatibility this method is kept and
the method `constrain` patches through to it. If the `fvOption`
has `SetValue` in the name then this name is kept


#### `simpleFunctionObjects` in multi-region cases now write to sub-directories of `postProcessing`

To conform with the way that regular OpenFOAM function objects do
it now function objects that belong to the non-default mesh in
multi-region cases write their data to a sub-directory of
`postProcessing` named after the mesh


#### `funkyWarpMesh` allows reading function objects

Function objects can now be loaded with an option
`-allowFunctionObjects`


#### `executeIfStartTime` now also allows execution if it is "only" a restart

Normally the function object only executes only if this is the
real start time (not a restart). If the option `executeOnRestart`
is set then the function object also executes if this is a restart


#### Scoped macro expansion now works for `storedVariables` and `swakDataEntry`

When reading these data structures the parent directory is
correctly passed and therefor scoped macro expansions
(`$[:deltaT]` for instance) works


### Examples


#### `FvOptions/heatExchangerSources`

Demonstrates getting changes to the solution (residuals) with
`matrixChangeBefore` and `matrixChangeAfter`


#### State machine examples

-   **Examples/StateMachine/stateCavity:** demonstrates the basic
    functionality of state machines
-   **Examples/manipulateFvSolutionFvSchemes/pitzDailyStateMachineSwitched:** demonstrates
    using state machines to manipulate the discretization during
    a simulation


#### Dynamic mesh examples

The folder `Examples/DynamicMesh` has examples that demonstrate
capabilities of the `swakDynamicMesh`-library.

-   **rotatingTank2D:** is a demonstration of a `solidBody` mesh
    movement where translation and rotation are
    described by swak-expressions
-   **hecticInletACMI2D,switchedTJunction,switchedInletACMI2D:** demonstrate
    a `ACMI` boundary condition where switching of faces is done
    by a swak-expression. Won't work with `Foam-extend` as this
    has no `ACMI`


## 2018-12-29 - version number : 0.4.2

The Foam-versions that this release has been tested with are

-   OpenFOAM 2.3
-   OpenFOAM 6.0
-   OpenFOAM+ v1806
-   Foam-Extend 4.0


### New supported versions


#### OpenFOAM+ v1706

Minor adaptions were required to make this compile


#### OpenFOAM 5.0

Some adaptions were required to make this compile


#### OpenFOAM+ v1712

Minor adaptions were required to make this compile by Mark Olesen


#### OpenFOAM+ v1806

Adaptions supplied by Mark Olesen. Adaptions needed after the release

This is one of the versions that this release is tested with


#### foam-extend 4.1

This is a work in progress based on the `nextRelease`-branch as
there is no release yet

This is one of the versions that this release is tested with


#### OpenFOAM 6

Compiles.

This is one of the versions that this release is tested with


#### OpenFOAM+ v1812

Adaptions supplied by Mark Olesen.

This will be the officially tested version after this release


### Incompatibilities


#### Solved field in `initPotentialFlow` now named differently

The field name is now the name of the original field plus
`Potential`. This means that in `fvSchemes` discretizations for
that field have to be provided


#### Environment variables for Python 2 integration renamed

These environment variables are now renamed from `_PYTHON_` to
`_PYTHON2_`. Scripts are adapted


#### `funkyDoCalc`-files with an entry `expressions` assumed to be new format

If one of the dictionaries in the specification file for
`funkyDoCalc` is named `expressions` it is assumed that the file
is in the new format and calculations will probably fail


### Bug fixes


#### `initPotentialFlow` not working in `foam-extend`

Here the function object failed with a "copy to
myself"-error. This has been fixed by changing the name of the
temporary field.


#### Floating point exception in `unitCylinder` in `simpleSearchableSurfaces`

When finding a cutting point in "infinity" then `magSqr`
failed. Fixed


#### `ReaderParticleCloud` could not read data if particle number changed

These particles could not be read data from different time-steps
if the number of particles differed between them. This has been fixed


#### `funkyDoCalc` does not write data files correctly for parallel cases

In parallel cases the same data was written to each
`processor`-directory when writing CSV-files or
distributions. This has been fixed so that the data is written
similarly to single-CPU-cases


#### Horrible spelling mistake in the `Allwmake`

Reported by Matti Rauter at
<https://twitter.com/igt_matti/status/989870314241880067>
Fixed


#### `groovyBCJumpAMI` not working correctly

Because the reference was returned instead of the `tmp` some
values were overwritten


#### `patchFunctionObject` hangs with `processorCyclic` boundaries and regular expressions

To enable `cyclic` boundary conditions on decomposed cases
`processorCyclic` boundaries are created on **some**
processors. Sometimes these boundaries are picked up by regular
expressions in function objects derived from
`patchFunctionObject` (for instance `patchExpression`). This
makes the run hang

This has been fixed by checking whether all processors have the
same boundaries in the list and stopping if they haven't
requesting the user to be more specific in the regular
expressions


#### Problem compiling on OF 5.0

Fixed a compilation problem on 5.0 that was due to a wrong
assumption about the change of an API in the `Random`-class (it
happens one version later). Thanks for reporting it at
<https://sourceforge.net/p/openfoam-extend/ticketsswak4foam/244/>
to Daniel Pielmeier


#### Failing `Python2` and `Python3` integration if Floating Point Exception is enabled

On some platforms if floating point exceptions are enabled then
importing `numpy` fails because it seems to use FPEs to detect
properties of the floating point implementation. This makes the
whole program fail.

Only known workaround is switching FPE-trapping off by

    export FOAM_SIGFPE=false

Currently there is a warning issued the first time `numpy` is
imported to guide the user to the fix


### Internals (for developers)


#### Refactoring of the python integration

The Python integration has now been split into the actual Python
2 integration and a general part to allow the integration of
other interpreter languages


### Infrastructure


#### Automatic detection of Python in `swakConfiguation.automatic`

If `swakConfiguation.automatic` is used as `swakConfiguation`
then the highest available version of Python 2 and Python 3 is
used for the integration of these two languages


#### Change banner in Sources

The regular OpenFOAM and the obsolete ICE-banner are removed and
a new swak4Foam-banner are added to the source files. This
modifies almost every file without changing any functionality


#### Renaming of the Forks

The three main supported forks are now renamed in the `#ifdef`

-   **EXTEND:** stays the same
-   **ORG:** formerly known as `OPENFOAM`. The CFDDirect/Foundation fork
-   **COM:** formerly `PLUS`. The Version maintained by ESI/OpenCFD


#### Additional output of `Allwmake`

The current OpenFOAM-version and the version of the sources is
printed in the beginning. This should help diagnosing problems


#### Script `AllwmakeAll` that compiles requirements automatically

The script compiles `bison` and `lua` before compiling
`swak4Foam` itself. It also automatically sets
`swakConfiguration` to a version that tries to find the scripting
languages automatically


### Documentation


### New features


#### Python 3 integration

In addition to the "regular" Python-integration a separate
library `swakPython3Integration` has been added which integrates
with Python 3. The "old" Python integration will always be
Python 2. The two integrations have the same features and the
same function objects (in the names the `python` has to be
replaced with `python3`)


#### Lua integration

The library `swakLuaIntegration` integrates with the [Lua
Scripting language](https://www.lua.org/) the same way that the Python 2 and Python 3
integrations do. The same function objects exist except that in
the names `python` has to be replaced with `lua`. If fields are
transferred to Lua then they are converted into
Lua-tables. Therefor the performance should be much slower than
the Python-integration where the native data is mapped to
`numpy`-arrays

The script `./maintainanceScripts/compileRequirements.sh`
compiles a private version of Lua that is automatically used when
the `swakConfiguation.automatic` is used


#### Function object to read and write dictionaries

The function object `readAndWriteDictionary` in the
`simpleFunctionObjects` reads a dictionary from disk and writes
it with the other data. The main purpose of this function object
is to allow scripting languages to write their data


#### `atan2` added to parsers

The `atan2(x,y)` function has been added to all the parsers. This
takes 2 values of which one must be non-zero (they represent a
point in the $(x,y)$-plane) and calculates the angle in radiants
between this vector and the x-axis


#### Additional logical accumulators

Three new logical accumulators have been added. They are all
aliases or trivial extensions of the existing `or` and `and` but
sometimes should make the intention clearer

-   **all:** alias for `and`
-   **any:** alias for `or`
-   **none:** only `true` if everything is `false` (basically `not or`)


#### `neighbourPatch`-function for expressions on `cyclic` patches

The expression `neighbourPatch(foo)` on a cyclic patch gets the
value of `foo` on the "other" side of the cyclic. For this the
field has to be a `cyclic` or a subclass


### Enhancements


#### `solverPerformanceToGlobalVariables` now supports vector fields

With an additional (optional) parameter `vectorFieldNames` this
function object now collects information about the performance of
the linear solver for vector fields as well


#### New file format for `funkyDoCalc`

`funkyDoCalc` now has a new file format. If a dictionary
`expressions` is found in the specification file then it is
assumed that the file is in the new format. `expressions` are the
contents of the previous format. Additional keys in the file now
allow specifying things that previously only specified on the command line:

-   noDimensionChecking
-   foreignMeshesThatFollowTime

A list `libs` also allows specifying a list of libraries to
load. This allows adding boundary conditions that would otherwise
make the execution fail


#### `funkyDoCalc` allows writing data as a dictionary

The option `-writeDict` writes all the results as a
dictionary. It also includes a sub-dictionary with a copy of the
specification file


#### `funkyDoCalc` executes function objects

With the `-allowFunctionObjects` the regular function objects
from the `constrolDict` are now executed.

If the new file format is used and there is an entry `functions`
then the function objects specified there are executed (this
allows setting up additional fields etc)

A list `preloadFields` allows preloading fields that might be
needed by the function objects

**BUG**: using separate function
objects causes a segmentation fault after the run
finished. Probably because the function objects are inconsitently
destroyed


#### `groovyBCJump` now works for non-scalar boundary conditions

Now the boundary condition works for non-scalar fields as well


#### `groovyBC` now supports `neighbourField(foo)`

If used on a cyclic patch then this will get the value of `foo`
on the other side. Before this raise a `Not Implemented`-Error

**Attention**: `foo` on the patch has to be a `groovyBC` for this
 work (or any other BC that implements `patchNeighbourField()`


#### `groovyBC` supports immersed boundary conditions in `Foam-extend 4.1`

Immersed boundaries need special treatment because it is not a
"real" boundary that should have a written value


### Examples


#### `groovyBC/jumpChannel` to demonstrate `groovyBC` on cyclic boundaries

This old test-example has been upgraded to a full example to
demonstrate the use of `groovyBC` with the `cyclicSlave`-option
and the `groovyBCJump`-condition


#### `ImmersedBC/pitzDaily` to demonstrate *immersed boundary conditions* in `foam-extend`

This demonstrates that swak-evaluations can be used for the
immersed boundary conditions in `foam-extend`


## Next release - version number : 0.4.3


### New supported versions


#### OpenFOAM+ v1906

Adaptions supplied by Mark Olesen.

This is currently the tested version in the ESI/OpenFOAM+ family


#### OpenFOAM 7

Compiles. Not fully tested


#### OpenFOAM+ v1912

Adaptions supplied by Mark Olesen.


### Incompatibilities


#### Bison older than version 3.3 no longer supported

The removal of warnings in biosn 3.4 meant that bison 3.2 and
older are no longr able to generate C++ sources from the grammar
files


### Bug fixes


#### Compilation with `WM_LABEL_SIZE=64` for `foam-extend-4.1`

Fix supplied by Danial Khazaei because compilation of `fe 4.1`
broke when the `label` size was set to 64 bit. Now works with
other distributions as well


#### `writeOldTimesOnSignal` not working for newer Foam-versions

This function object did not work for newer Foam-version where
`start()` was removed from the API of `functionObject`

This has been fixed and the storing of times has been fixed


### Internals (for developers)


### Infrastructure


#### Support for `bison` 3.4

The grammars have been adapted to generate C++-sources without
warnings with bison 3.4. The script to compile requirements has
been modified to download and compile this version.

This breaks compatibility with bison 3.2 and older
This is the latest stable release and only a bug-fix
release. `luarocks` was upgraded as well
Now if before compiling a command like

    export SWAK_PYTHON3VERSION=6

is executed then Python 3.6 is used even if a "better" Python
like 3.7 is found. THe same is true for

    export SWAK_PYTHON2VERSION=5

(or similar).

If these variables are not set the highest possible Python is
used (like it was before


### Documentation


#### Adapted contributors section in the README

Removed outdated information, updated links


### New features


#### Expression run-time trigger for runtime-control in ESI-fork

There now is a run-time trigger `swakExpression` that allows
switching the runtime-control (for instance stopping the run)
depending on a swak-expression.

This works only for the ESI-fork. It **should** work with versions
starting at 3.0+ but is currently only available starting with
v1906 (modify `swak.H` if you want to enable/test it for older
versions)

The trigger is found in the `swakFunctionObjects`-library


#### Library for solving physical systems in separate regions

The library `simpleRegionSolvers` holds some function objects
which solve a physical system in a separate region. This region
can be coupled with the main region to get more realistic
boundary conditions (for instance heat conduction in the
boundary)

Currently implemented function objects are

-   **`heatConductionRegionSolver`:** Solves the heat conduction in a
    solid (modeled on the solid regions in `chtMultiRegionFoam`)


#### A `fvOption` to set the energy according to a temperature expression

In the `swakSourceFields`-library there is a `fvOption` named
`swakSetTemperature` that specifies the temperature in a region
which is then converted to the correct energy in the specified
cells.

Note: in the `expressions` dictionary the name of the emergy
field that is solved for (usually `h` or `he`) has to be
specified. **Not** the temperature

An example can be found in
`Examples/RegionSolvers/cavityWithHeater`


### Enhancements


#### `writeOldTimeStepsOnSignal` now also writes when a `FatalError` happens

By intercepting the `ABRT` signal the function object now writes
all stored timesteps when a `FatalError` is encountered in the code


#### `provokeSignal` now also simulates a FatalError

The function object now has a special 'signal' `FoamFatal` that
instead of raising a signal does a `FatalError`


### Examples


#### `groovyBC/nonBreakingDam` to demonstrate macro expansion

This example demonstrates OpenFOAM macro-expansion inside of
expression strings


#### `runTimeCondition/simpleSwakCar` to demonstrate runtime-control in the ESI fork

This example demonstrates the runtime-control for function
objects that exists in the ESI-fork
