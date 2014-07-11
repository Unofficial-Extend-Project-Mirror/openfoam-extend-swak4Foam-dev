<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. Description</a></li>
<li><a href="#sec-2">2. About this document</a>
<ul>
<li><a href="#sec-2-1">2.1. Scope</a></li>
<li><a href="#sec-2-2">2.2. Technical</a></li>
</ul>
</li>
<li><a href="#sec-3">3. Contributors etc</a>
<ul>
<li><a href="#sec-3-1">3.1. Original Author</a></li>
<li><a href="#sec-3-2">3.2. Current Maintainer</a></li>
<li><a href="#sec-3-3">3.3. Contributors</a></li>
<li><a href="#sec-3-4">3.4. Documentation</a></li>
</ul>
</li>
<li><a href="#sec-4">4. Installation/Compilation</a>
<ul>
<li><a href="#sec-4-1">4.1. Requirements</a></li>
<li><a href="#sec-4-2">4.2. Building</a>
<ul>
<li><a href="#sec-4-2-1">4.2.1. Additional configuration</a></li>
<li><a href="#sec-4-2-2">4.2.2. Possible compilation failure with old 2.0.x-versions</a></li>
<li><a href="#sec-4-2-3">4.2.3. Special versions of the python integration</a></li>
</ul>
</li>
<li><a href="#sec-4-3">4.3. Global installation</a></li>
<li><a href="#sec-4-4">4.4. Packaging</a>
<ul>
<li><a href="#sec-4-4-1">4.4.1. Debian</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-5">5. Contents</a>
<ul>
<li><a href="#sec-5-1">5.1. Libraries</a>
<ul>
<li><a href="#sec-5-1-1">5.1.1. <code>swak4FoamParsers</code></a></li>
<li><a href="#sec-5-1-2">5.1.2. <code>simpleFunctionObjects</code></a></li>
<li><a href="#sec-5-1-3">5.1.3. <code>groovyBC</code></a></li>
<li><a href="#sec-5-1-4">5.1.4. <code>swakFunctionObjects</code></a></li>
<li><a href="#sec-5-1-5">5.1.5. <code>simpleSwakFunctionObjects</code></a></li>
<li><a href="#sec-5-1-6">5.1.6. <code>swakSourceFields</code></a></li>
<li><a href="#sec-5-1-7">5.1.7. <code>swakFvOptions</code></a></li>
<li><a href="#sec-5-1-8">5.1.8. <code>swakTopoSources</code></a></li>
<li><a href="#sec-5-1-9">5.1.9. <code>swakFiniteArea</code></a></li>
<li><a href="#sec-5-1-10">5.1.10. <code>groovyStandardBCs</code></a></li>
<li><a href="#sec-5-1-11">5.1.11. <code>pythonIntegration</code></a></li>
<li><a href="#sec-5-1-12">5.1.12. <code>simpleLagrangianFunctionObjects</code></a></li>
<li><a href="#sec-5-1-13">5.1.13. <code>simpleSearchableSurfaces</code></a></li>
<li><a href="#sec-5-1-14">5.1.14. <code>fluFunctionObjectDriver</code></a></li>
<li><a href="#sec-5-1-15">5.1.15. <code>swakLagrangianParser</code></a></li>
<li><a href="#sec-5-1-16">5.1.16. <code>functionPlugins</code></a>
<ul>
<li><a href="#sec-5-1-16-1">5.1.16.1. lagrangianCloudAdaptors-directory</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-5-2">5.2. Utilities</a>
<ul>
<li><a href="#sec-5-2-1">5.2.1. <code>funkySetFields</code></a></li>
<li><a href="#sec-5-2-2">5.2.2. <code>funkySetAreaFields</code></a></li>
<li><a href="#sec-5-2-3">5.2.3. <code>funkySetBoundaryField</code></a></li>
<li><a href="#sec-5-2-4">5.2.4. <code>replayTransientBC</code></a></li>
<li><a href="#sec-5-2-5">5.2.5. <code>funkyDoCalc</code></a></li>
<li><a href="#sec-5-2-6">5.2.6. <code>calcNonUniformOffsetsForMapped</code></a></li>
<li><a href="#sec-5-2-7">5.2.7. <code>fieldReport</code></a></li>
<li><a href="#sec-5-2-8">5.2.8. <code>funkyPythonPostproc</code></a></li>
<li><a href="#sec-5-2-9">5.2.9. <code>funkySetLagrangianField</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3">5.3. Examples</a>
<ul>
<li><a href="#sec-5-3-1">5.3.1. groovyBC</a>
<ul>
<li><a href="#sec-5-3-1-1">5.3.1.1. pulsedPitzDaily</a></li>
<li><a href="#sec-5-3-1-2">5.3.1.2. wobbler</a></li>
<li><a href="#sec-5-3-1-3">5.3.1.3. circulatingSplash</a></li>
<li><a href="#sec-5-3-1-4">5.3.1.4. movingConeDistorted</a></li>
<li><a href="#sec-5-3-1-5">5.3.1.5. average-t-junction</a></li>
<li><a href="#sec-5-3-1-6">5.3.1.6. delayed-t-junction</a></li>
<li><a href="#sec-5-3-1-7">5.3.1.7. multiRegionHeaterFeedback</a></li>
<li><a href="#sec-5-3-1-8">5.3.1.8. fillingTheDam</a></li>
<li><a href="#sec-5-3-1-9">5.3.1.9. sprinklingInlet</a></li>
<li><a href="#sec-5-3-1-10">5.3.1.10. potentialPitzDaily</a></li>
</ul>
</li>
<li><a href="#sec-5-3-2">5.3.2. FunkyDoCalc</a></li>
<li><a href="#sec-5-3-3">5.3.3. FunkySetFields</a></li>
<li><a href="#sec-5-3-4">5.3.4. FunkySetBoundaryFields</a></li>
<li><a href="#sec-5-3-5">5.3.5. InterFoamWithSources</a></li>
<li><a href="#sec-5-3-6">5.3.6. InterFoamWithFixed</a>
<ul>
<li><a href="#sec-5-3-6-1">5.3.6.1. <code>interFoamWithSources</code></a></li>
<li><a href="#sec-5-3-6-2">5.3.6.2. <code>mixingThing</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3-7">5.3.7. FiniteArea</a>
<ul>
<li><a href="#sec-5-3-7-1">5.3.7.1. <code>swakSurfactantFoam</code></a></li>
<li><a href="#sec-5-3-7-2">5.3.7.2. <code>planeTransport</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3-8">5.3.8. FvOptions</a>
<ul>
<li><a href="#sec-5-3-8-1">5.3.8.1. angleDuctWithSources</a></li>
</ul>
</li>
<li><a href="#sec-5-3-9">5.3.9. other</a>
<ul>
<li><a href="#sec-5-3-9-1">5.3.9.1. <code>angledDuctImplicit</code></a></li>
<li><a href="#sec-5-3-9-2">5.3.9.2. <code>angledDuctImplicitTransient</code></a></li>
<li><a href="#sec-5-3-9-3">5.3.9.3. <code>capillaryRise</code></a></li>
<li><a href="#sec-5-3-9-4">5.3.9.4. <code>mixingDam</code></a></li>
<li><a href="#sec-5-3-9-5">5.3.9.5. <code>counterFlowFlame2DInitialized</code></a></li>
<li><a href="#sec-5-3-9-6">5.3.9.6. <code>simpleBendedPipe</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3-10">5.3.10. FromPresentations</a>
<ul>
<li><a href="#sec-5-3-10-1">5.3.10.1. OSCFD<sub>cleaningTank3D</sub></a></li>
<li><a href="#sec-5-3-10-2">5.3.10.2. OSCFD<sub>cleaningTank2D</sub></a></li>
<li><a href="#sec-5-3-10-3">5.3.10.3. OFW8<sub>sandPitOfCarcoon</sub></a></li>
<li><a href="#sec-5-3-10-4">5.3.10.4. OFW8<sub>landspeedersInCanyon</sub></a></li>
</ul>
</li>
<li><a href="#sec-5-3-11">5.3.11. PythonIntegration</a>
<ul>
<li><a href="#sec-5-3-11-1">5.3.11.1. <code>manipulatedPitzDaily</code></a></li>
<li><a href="#sec-5-3-11-2">5.3.11.2. <code>findPointPitzDaily</code></a></li>
<li><a href="#sec-5-3-11-3">5.3.11.3. <code>bed2OfPisa</code></a></li>
<li><a href="#sec-5-3-11-4">5.3.11.4. <code>multiRegionHeaterBuildFunctionObjects</code></a></li>
<li><a href="#sec-5-3-11-5">5.3.11.5. <code>slicedPitzDaily</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3-12">5.3.12. <code>CodeStream</code></a></li>
<li><a href="#sec-5-3-13">5.3.13. solvePDE</a>
<ul>
<li><a href="#sec-5-3-13-1">5.3.13.1. flangeWithPDE</a></li>
<li><a href="#sec-5-3-13-2">5.3.13.2. pitzDailyWithPDE</a></li>
</ul>
</li>
<li><a href="#sec-5-3-14">5.3.14. BasicSourceSubclasses</a>
<ul>
<li><a href="#sec-5-3-14-1">5.3.14.1. pitzDailyWithSwirl</a></li>
<li><a href="#sec-5-3-14-2">5.3.14.2. pitzDailyWithExplicitPoroPlug</a></li>
<li><a href="#sec-5-3-14-3">5.3.14.3. pitzDailyWithImplicitPoroPlug</a></li>
</ul>
</li>
<li><a href="#sec-5-3-15">5.3.15. Lagrangian</a>
<ul>
<li><a href="#sec-5-3-15-1">5.3.15.1. functionObjects</a></li>
<li><a href="#sec-5-3-15-2">5.3.15.2. parser</a></li>
</ul>
</li>
<li><a href="#sec-5-3-16">5.3.16. SimpleSurface</a>
<ul>
<li><a href="#sec-5-3-16-1">5.3.16.1. twoMotorbikesAndAWall</a></li>
<li><a href="#sec-5-3-16-2">5.3.16.2. littleVolcano</a></li>
<li><a href="#sec-5-3-16-3">5.3.16.3. chargedPlates</a></li>
</ul>
</li>
<li><a href="#sec-5-3-17">5.3.17. tests</a>
<ul>
<li><a href="#sec-5-3-17-1">5.3.17.1. <code>randomCavity</code></a></li>
</ul>
</li>
<li><a href="#sec-5-3-18">5.3.18. BugCases</a></li>
</ul>
</li>
<li><a href="#sec-5-4">5.4. <code>maintainanceScripts</code></a></li>
<li><a href="#sec-5-5">5.5. Documentation</a></li>
<li><a href="#sec-5-6">5.6. releaseTesting</a></li>
</ul>
</li>
<li><a href="#sec-6">6. Bug reporting and Development</a>
<ul>
<li><a href="#sec-6-1">6.1. Bug reports</a>
<ul>
<li><a href="#sec-6-1-1">6.1.1. Things to do before reporting bug</a></li>
</ul>
</li>
<li><a href="#sec-6-2">6.2. Development</a>
<ul>
<li><a href="#sec-6-2-1">6.2.1. Suggest reading</a></li>
<li><a href="#sec-6-2-2">6.2.2. Special branches</a></li>
<li><a href="#sec-6-2-3">6.2.3. Distributed bug-tracking</a></li>
<li><a href="#sec-6-2-4">6.2.4. Maintaining feature and hotfix-branches</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-7">7. Copyright</a></li>
<li><a href="#sec-8">8. Known bugs</a>
<ul>
<li><a href="#sec-8-1">8.1. Moving meshes and <code>sampledSurfaces</code></a></li>
<li><a href="#sec-8-2">8.2. Missing support for interpolation and point-Fields</a></li>
<li><a href="#sec-8-3">8.3. Caching of loaded fields not working</a></li>
<li><a href="#sec-8-4">8.4. Possible enhancements of the code</a>
<ul>
<li><a href="#sec-8-4-1">8.4.1. Pointers in the driver code</a></li>
</ul>
</li>
<li><a href="#sec-8-5">8.5. Possible memory loss</a></li>
<li><a href="#sec-8-6">8.6. Non-treatment of the inner product <code>&amp;</code> of symmetric tensors</a></li>
<li><a href="#sec-8-7">8.7. No point-vector construction for Subsets</a></li>
<li><a href="#sec-8-8">8.8. No tab-completion for regular Python-shell and old IPython-versions</a></li>
<li><a href="#sec-8-9">8.9. Failure of <code>groovyBC</code> for <code>commsType</code> <code>blocking</code></a></li>
</ul>
</li>
<li><a href="#sec-9">9. History</a>
<ul>
<li><a href="#sec-9-1">9.1. 2010-09-13 - version number : 0.1</a></li>
<li><a href="#sec-9-2">9.2. 2010-12-18 - version number : 0.1.1</a>
<ul>
<li><a href="#sec-9-2-1">9.2.1. Parser for <code>sampledSurfaces</code></a></li>
<li><a href="#sec-9-2-2">9.2.2. Multiline <code>variables</code></a></li>
<li><a href="#sec-9-2-3">9.2.3. Two maintainance-scripts were added</a></li>
<li><a href="#sec-9-2-4">9.2.4. Parsers using 'external variables' are now run-time selectable</a></li>
</ul>
</li>
<li><a href="#sec-9-3">9.3. 2011-01-30 - version number : 0.1.2</a>
<ul>
<li><a href="#sec-9-3-1">9.3.1. Support for <i>Finite Area</i>-stuff</a></li>
<li><a href="#sec-9-3-2">9.3.2. Bugfix for compiling in single precision</a></li>
<li><a href="#sec-9-3-3">9.3.3. New function <code>nearDist</code></a></li>
</ul>
</li>
<li><a href="#sec-9-4">9.4. 2011-04-20 - version number : 0.1.3</a>
<ul>
<li><a href="#sec-9-4-1">9.4.1. New utility <code>funkySetAreaField</code></a></li>
<li><a href="#sec-9-4-2">9.4.2. Refactoring of <code>groovyBC</code> and groovified boundary conditions</a></li>
<li><a href="#sec-9-4-3">9.4.3. Easier deployment</a></li>
<li><a href="#sec-9-4-4">9.4.4. Force equations</a></li>
<li><a href="#sec-9-4-5">9.4.5. New utility <code>funkyDoCalc</code></a></li>
<li><a href="#sec-9-4-6">9.4.6. Debian packaging</a></li>
<li><a href="#sec-9-4-7">9.4.7. Lookup-tables</a></li>
<li><a href="#sec-9-4-8">9.4.8. Stored variables</a></li>
<li><a href="#sec-9-4-9">9.4.9. Sampled sets</a></li>
</ul>
</li>
<li><a href="#sec-9-5">9.5. 2011-07-26 - version number : 0.1.4</a>
<ul>
<li><a href="#sec-9-5-1">9.5.1. Port to OpenFOAM 2.0</a></li>
<li><a href="#sec-9-5-2">9.5.2. New features:</a>
<ul>
<li><a href="#sec-9-5-2-1">9.5.2.1. Rewrite of <code>rand</code> and <code>randNormal</code></a></li>
<li><a href="#sec-9-5-2-2">9.5.2.2. Binary <code>min</code> and <code>max</code></a></li>
<li><a href="#sec-9-5-2-3">9.5.2.3. Allow writing of only volume-fields in <code>funkySetAreaFields</code></a></li>
<li><a href="#sec-9-5-2-4">9.5.2.4. Internal changes</a></li>
</ul>
</li>
<li><a href="#sec-9-5-3">9.5.3. Bug-fixes</a>
<ul>
<li><a href="#sec-9-5-3-1">9.5.3.1. <code>funkySetAreaFields</code> did not check for the correct fields</a></li>
<li><a href="#sec-9-5-3-2">9.5.3.2. <code>surfaceProxy</code> uses the wrong geometric data</a></li>
<li><a href="#sec-9-5-3-3">9.5.3.3. Avoid floating point exceptions with division of fields</a></li>
</ul>
</li>
<li><a href="#sec-9-5-4">9.5.4. Packaging</a>
<ul>
<li><a href="#sec-9-5-4-1">9.5.4.1. Update Debian packaging</a></li>
<li><a href="#sec-9-5-4-2">9.5.4.2. Deployment scripts</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-6">9.6. 2011-10-03 - version number : 0.1.5</a>
<ul>
<li><a href="#sec-9-6-1">9.6.1. New features</a>
<ul>
<li><a href="#sec-9-6-1-1">9.6.1.1. <code>replayTransientBC</code> now supports multiple regions</a></li>
<li><a href="#sec-9-6-1-2">9.6.1.2. <code>replayTransientBC</code> allows execution of functionObjects</a></li>
<li><a href="#sec-9-6-1-3">9.6.1.3. <code>Python</code>-embedding</a></li>
<li><a href="#sec-9-6-1-4">9.6.1.4. Global variables</a></li>
<li><a href="#sec-9-6-1-5">9.6.1.5. Using OF 2.0 codeStreams</a></li>
<li><a href="#sec-9-6-1-6">9.6.1.6. Simplified boundary condition <code>groovyBCFixedValue</code></a></li>
<li><a href="#sec-9-6-1-7">9.6.1.7. Function objects to solve PDEs</a></li>
</ul>
</li>
<li><a href="#sec-9-6-2">9.6.2. Administrative and packaging</a>
<ul>
<li><a href="#sec-9-6-2-1">9.6.2.1. Inject <code>swak4Foam</code> into a distro</a></li>
<li><a href="#sec-9-6-2-2">9.6.2.2. Absorb <code>simpleFunctionObjects</code></a></li>
</ul>
</li>
<li><a href="#sec-9-6-3">9.6.3. Bugfixes</a>
<ul>
<li><a href="#sec-9-6-3-1">9.6.3.1. Variables not working for parallel computations</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-7">9.7. 2012-01-04 - version number : 0.1.6</a>
<ul>
<li><a href="#sec-9-7-1">9.7.1. Cases changed</a>
<ul>
<li><a href="#sec-9-7-1-1">9.7.1.1. <code>circulatingSplash</code></a></li>
</ul>
</li>
<li><a href="#sec-9-7-2">9.7.2. Infrastructure</a>
<ul>
<li><a href="#sec-9-7-2-1">9.7.2.1. Check for correct <code>bison</code>-version</a></li>
<li><a href="#sec-9-7-2-2">9.7.2.2. Supply a header with preprocessor-symbols about the used OF-version</a></li>
</ul>
</li>
<li><a href="#sec-9-7-3">9.7.3. Technical</a>
<ul>
<li><a href="#sec-9-7-3-1">9.7.3.1. Refactoring of the FieldDriver</a></li>
<li><a href="#sec-9-7-3-2">9.7.3.2. Refactoring of the FaFieldDriver</a></li>
<li><a href="#sec-9-7-3-3">9.7.3.3. Writing of <code>storedVariables</code></a></li>
<li><a href="#sec-9-7-3-4">9.7.3.4. <code>simpleFunctionObjects</code> now write vectors and tensors without brackets</a></li>
<li><a href="#sec-9-7-3-5">9.7.3.5. A <i>default mesh</i> for the drivers exists</a></li>
</ul>
</li>
<li><a href="#sec-9-7-4">9.7.4. New features</a>
<ul>
<li><a href="#sec-9-7-4-1">9.7.4.1. General <code>phi</code> in <code>solveTransportPDE</code></a></li>
<li><a href="#sec-9-7-4-2">9.7.4.2. <code>funkySetFields</code> now also writes surfaceFields</a></li>
<li><a href="#sec-9-7-4-3">9.7.4.3. Function objects now <code>surfaceField</code>-aware</a></li>
<li><a href="#sec-9-7-4-4">9.7.4.4. <code>funkySetFields</code> and function objects support tensors</a></li>
<li><a href="#sec-9-7-4-5">9.7.4.5. Extension of the <code>expressionToFace</code> <code>topoSet</code></a></li>
<li><a href="#sec-9-7-4-6">9.7.4.6. <code>addGlobalVariable</code> allows setting more than one value</a></li>
<li><a href="#sec-9-7-4-7">9.7.4.7. Function object <code>calculateGlobalVariables</code></a></li>
<li><a href="#sec-9-7-4-8">9.7.4.8. Generate a dummy <code>phi</code> in <code>replayTransientBC</code></a></li>
<li><a href="#sec-9-7-4-9">9.7.4.9. Function object to dump expression results</a></li>
<li><a href="#sec-9-7-4-10">9.7.4.10. Additional options for <code>funkySetFields</code></a></li>
<li><a href="#sec-9-7-4-11">9.7.4.11. Boundary condition <code>groovcBCDirection</code></a></li>
<li><a href="#sec-9-7-4-12">9.7.4.12. Boundary condition <code>groovyBCJump</code></a></li>
<li><a href="#sec-9-7-4-13">9.7.4.13. <code>simpleFunctionObjects</code> write CSV-files</a></li>
<li><a href="#sec-9-7-4-14">9.7.4.14. Submeshes automatically read if <code>searchOnDisc</code> specified</a></li>
<li><a href="#sec-9-7-4-15">9.7.4.15. Conditional <code>functionObjects</code></a></li>
<li><a href="#sec-9-7-4-16">9.7.4.16. <code>functionObject</code> that reads gravitation</a></li>
<li><a href="#sec-9-7-4-17">9.7.4.17. PDE-functionObjects for <code>finiteArea</code></a></li>
<li><a href="#sec-9-7-4-18">9.7.4.18. Subclass to <code>DataEntry</code> that uses <i>swak</i>-expressions</a></li>
<li><a href="#sec-9-7-4-19">9.7.4.19. <code>funkySetAreaField</code> now also writes edgeFields</a></li>
</ul>
</li>
<li><a href="#sec-9-7-5">9.7.5. Bug fixes</a>
<ul>
<li><a href="#sec-9-7-5-1">9.7.5.1. Compilation with Intel-Compiler possible</a></li>
<li><a href="#sec-9-7-5-2">9.7.5.2. Access to tensor-components not working</a></li>
<li><a href="#sec-9-7-5-3">9.7.5.3. Constants for surfaceFields not working</a></li>
<li><a href="#sec-9-7-5-4">9.7.5.4. <code>snGrad</code> does not work for patches if the file is on disc</a></li>
<li><a href="#sec-9-7-5-5">9.7.5.5. <code>potentialFoam</code> does not correctly use a <code>groovyBC</code></a></li>
<li><a href="#sec-9-7-5-6">9.7.5.6. Extra evaluation of boundary conditions causes failure</a></li>
</ul>
</li>
<li><a href="#sec-9-7-6">9.7.6. Discontinued features</a>
<ul>
<li><a href="#sec-9-7-6-1">9.7.6.1. <code>groovyFlowRateInletVelocity</code></a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-8">9.8. 2012-04-13 - version number : 0.2.0 Friday the 13th</a>
<ul>
<li><a href="#sec-9-8-1">9.8.1. New features</a>
<ul>
<li><a href="#sec-9-8-1-1">9.8.1.1. Region name in <code>simpleFunctionObject</code>.outputs</a></li>
<li><a href="#sec-9-8-1-2">9.8.1.2. Temporal operators <code>ddt</code> and <code>oldTime</code></a></li>
<li><a href="#sec-9-8-1-3">9.8.1.3. Boundary condition <code>groovcBCDirection</code></a></li>
<li><a href="#sec-9-8-1-4">9.8.1.4. Boundary condition <code>groovyBCJump</code></a></li>
<li><a href="#sec-9-8-1-5">9.8.1.5. Function to return the position of minimum and maximum</a></li>
<li><a href="#sec-9-8-1-6">9.8.1.6. Support for <code>pointFields</code> in the field-parsers</a></li>
<li><a href="#sec-9-8-1-7">9.8.1.7. Support for tensors in the <code>finiteArea</code>-field parser</a></li>
<li><a href="#sec-9-8-1-8">9.8.1.8. New convenience-variables for the Python-Integration</a></li>
<li><a href="#sec-9-8-1-9">9.8.1.9. Additional operators from the <code>fvc</code>-namespace</a></li>
</ul>
</li>
<li><a href="#sec-9-8-2">9.8.2. Infrastructure</a>
<ul>
<li><a href="#sec-9-8-2-1">9.8.2.1. Full parallel compilation</a></li>
<li><a href="#sec-9-8-2-2">9.8.2.2. Version numbers and version number reporting</a></li>
</ul>
</li>
<li><a href="#sec-9-8-3">9.8.3. Packaging</a>
<ul>
<li><a href="#sec-9-8-3-1">9.8.3.1. Update of the Debian-Packaging</a></li>
</ul>
</li>
<li><a href="#sec-9-8-4">9.8.4. Changes in the behavior</a>
<ul>
<li><a href="#sec-9-8-4-1">9.8.4.1. Directory variables in Python-integration</a></li>
<li><a href="#sec-9-8-4-2">9.8.4.2. User must acknowledge parallelization in Python-integration</a></li>
</ul>
</li>
<li><a href="#sec-9-8-5">9.8.5. Bug fixes</a>
<ul>
<li><a href="#sec-9-8-5-1">9.8.5.1. <code>interFoam</code>-based example solvers do not compile on 2.1</a></li>
<li><a href="#sec-9-8-5-2">9.8.5.2. <code>-allowFunctionObjects</code>-option not working for <code>replayTransientBC</code></a></li>
<li><a href="#sec-9-8-5-3">9.8.5.3. Field itself can not be used in <code>funkySetBoundaryField</code></a></li>
<li><a href="#sec-9-8-5-4">9.8.5.4. No gradient for vectors in FieldParser</a></li>
<li><a href="#sec-9-8-5-5">9.8.5.5. Some operators for tensors not working in 1.6-ext</a></li>
<li><a href="#sec-9-8-5-6">9.8.5.6. <code>x</code>, <code>y</code>, <code>z</code> and <code>xy</code> etc not available as field names</a></li>
<li><a href="#sec-9-8-5-7">9.8.5.7. Missing tensor components for point-Fields in some parsers</a></li>
<li><a href="#sec-9-8-5-8">9.8.5.8. No vector construction possible for point-vectors (and tensors) in PatchField</a></li>
<li><a href="#sec-9-8-5-9">9.8.5.9. Incomprehensible error message in <code>funkySetFields</code> if the field is missing</a></li>
<li><a href="#sec-9-8-5-10">9.8.5.10. Missing <code>magSqr</code> in parsers</a></li>
<li><a href="#sec-9-8-5-11">9.8.5.11. Wrong size when composing vectors and tensors for point-fields</a></li>
<li><a href="#sec-9-8-5-12">9.8.5.12. <code>icc</code> does not compile <code>executeIfExecutableFitsFunctionObject</code> on Linux</a></li>
<li><a href="#sec-9-8-5-13">9.8.5.13. Enhancement to the <code>trackDictionary</code>-functionObject</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-9">9.9. 2012-10-18 - version number : 0.2.1</a>
<ul>
<li><a href="#sec-9-9-1">9.9.1. Requirements</a>
<ul>
<li><a href="#sec-9-9-1-1">9.9.1.1. <code>flex 2.5.35</code></a></li>
<li><a href="#sec-9-9-1-2">9.9.1.2. <code>bison 2.4</code></a></li>
</ul>
</li>
<li><a href="#sec-9-9-2">9.9.2. Bug fixes</a>
<ul>
<li><a href="#sec-9-9-2-1">9.9.2.1. Make sure that <code>Allwmake</code> always uses the <code>bash</code></a></li>
<li><a href="#sec-9-9-2-2">9.9.2.2. <code>downloadSimpleFunctionObjects.sh</code> still in Makefile</a></li>
<li><a href="#sec-9-9-2-3">9.9.2.3. <code>grad</code> in fields added dimensions</a></li>
<li><a href="#sec-9-9-2-4">9.9.2.4. Default <code>condition</code> for surface fields wrong size in <code>funkySetFields</code></a></li>
<li><a href="#sec-9-9-2-5">9.9.2.5. <code>mappedFvPatch</code> not treated like regular patches</a></li>
<li><a href="#sec-9-9-2-6">9.9.2.6. <code>flip()</code> for <code>faceSet</code> not correctly calculated</a></li>
<li><a href="#sec-9-9-2-7">9.9.2.7. <code>fset()</code> crashes when <code>faceSet</code> has boundary faces</a></li>
<li><a href="#sec-9-9-2-8">9.9.2.8. <code>groovyBC</code> produced incorrect results with <code>wallHeatFlux</code> etc</a></li>
<li><a href="#sec-9-9-2-9">9.9.2.9. Global variables not found in <code>faField</code></a></li>
<li><a href="#sec-9-9-2-10">9.9.2.10. Wrong type of condition field in <code>funkySetAreaFields</code></a></li>
<li><a href="#sec-9-9-2-11">9.9.2.11. <code>calculated</code>-patches \(0\) for some operations</a></li>
<li><a href="#sec-9-9-2-12">9.9.2.12. <code>sqr</code> of a <code>vector</code> should produce a <code>symmTensor</code></a></li>
<li><a href="#sec-9-9-2-13">9.9.2.13. <code>funkySetFields</code> produced wrong values on processor patches</a></li>
<li><a href="#sec-9-9-2-14">9.9.2.14. <code>sortedToc</code> does not exist for <code>1.6-ext</code></a></li>
<li><a href="#sec-9-9-2-15">9.9.2.15. Wrong <code>size()</code> and <code>pointSize()</code> reported for <code>FaField</code>-driver</a></li>
<li><a href="#sec-9-9-2-16">9.9.2.16. Memory leak in the field drivers</a></li>
<li><a href="#sec-9-9-2-17">9.9.2.17. Maintenance scripts not working with non-=bash= <code>/bin/sh</code></a></li>
<li><a href="#sec-9-9-2-18">9.9.2.18. <code>cof</code> and <code>diag</code> undefined</a></li>
<li><a href="#sec-9-9-2-19">9.9.2.19. No new file created if number of patches for patchAverage changes</a></li>
<li><a href="#sec-9-9-2-20">9.9.2.20. <code>variables</code> intolerant on spaces</a></li>
<li><a href="#sec-9-9-2-21">9.9.2.21. Missing <code>div</code>-operations</a></li>
<li><a href="#sec-9-9-2-22">9.9.2.22. Fields created by <code>expressionField</code> written too late</a></li>
<li><a href="#sec-9-9-2-23">9.9.2.23. <code>storedVariables</code> did not require a <code>initialValue</code></a></li>
<li><a href="#sec-9-9-2-24">9.9.2.24. Dimension checking makes <code>expressionField</code> fail</a></li>
<li><a href="#sec-9-9-2-25">9.9.2.25. <code>expressionField</code> at last timestep not written</a></li>
<li><a href="#sec-9-9-2-26">9.9.2.26. <code>groovyBC</code> makes <code>interFoam</code>-runs fail unpredictably</a></li>
<li><a href="#sec-9-9-2-27">9.9.2.27. Global variables of different sizes break parallel runs</a></li>
<li><a href="#sec-9-9-2-28">9.9.2.28. Fields treated with <code>readAndUpdateFields</code> were written one timestep too late</a></li>
</ul>
</li>
<li><a href="#sec-9-9-3">9.9.3. Enhancements</a>
<ul>
<li><a href="#sec-9-9-3-1">9.9.3.1. Topology-operators now support <code>variables</code> etc</a></li>
<li><a href="#sec-9-9-3-2">9.9.3.2. Fields touched by <code>manipulateField</code> being written</a></li>
<li><a href="#sec-9-9-3-3">9.9.3.3. Indicator functions <code>onPatch</code> and <code>internalFace</code> added to field-expressions</a></li>
<li><a href="#sec-9-9-3-4">9.9.3.4. Non-uniform second argument for <code>pow</code></a></li>
<li><a href="#sec-9-9-3-5">9.9.3.5. Added transpose to the tensors</a></li>
<li><a href="#sec-9-9-3-6">9.9.3.6. Added unit tensor <code>I</code> to parsers</a></li>
<li><a href="#sec-9-9-3-7">9.9.3.7. Added the <i>Hodge dual</i> operator</a></li>
<li><a href="#sec-9-9-3-8">9.9.3.8. <code>replayTransientBC</code> can now switch on function-objects via dictionary</a></li>
<li><a href="#sec-9-9-3-9">9.9.3.9. <code>replayTransientBC</code> can now create a <code>phi</code>-field via dictionary</a></li>
<li><a href="#sec-9-9-3-10">9.9.3.10. <code>expressionField</code> now allows the specification of a dimension</a></li>
</ul>
</li>
<li><a href="#sec-9-9-4">9.9.4. New features</a>
<ul>
<li><a href="#sec-9-9-4-1">9.9.4.1. Allow dynamically loaded plugins that add new functions to parsers</a></li>
<li><a href="#sec-9-9-4-2">9.9.4.2. Dynamically generated lists of <code>functionObjects</code></a></li>
<li><a href="#sec-9-9-4-3">9.9.4.3. Function object <code>readAndUpdateFields</code></a></li>
<li><a href="#sec-9-9-4-4">9.9.4.4. Source terms based on <code>basicSource</code></a></li>
<li><a href="#sec-9-9-4-5">9.9.4.5. Function objects that stop a run gracefully</a></li>
<li><a href="#sec-9-9-4-6">9.9.4.6. Function-objects to load thermophysical and turbulence models</a></li>
<li><a href="#sec-9-9-4-7">9.9.4.7. Function-objects that create and evolve clouds of lagrangian particles</a></li>
<li><a href="#sec-9-9-4-8">9.9.4.8. Function-object <code>manipulatePatchField</code> to manipulate the field on patches</a></li>
<li><a href="#sec-9-9-4-9">9.9.4.9. Delayed variables to simulate responses</a></li>
<li><a href="#sec-9-9-4-10">9.9.4.10. Allow preloading of fields in <code>funkySetFields</code></a></li>
</ul>
</li>
<li><a href="#sec-9-9-5">9.9.5. Infrastructure</a>
<ul>
<li><a href="#sec-9-9-5-1">9.9.5.1. Compilation script checks <code>SWAK4FOAM_SRC</code></a></li>
<li><a href="#sec-9-9-5-2">9.9.5.2. <code>Allwmake</code> creates symbolic links in <code>swakFiniteArea</code></a></li>
<li><a href="#sec-9-9-5-3">9.9.5.3. Reformatting of the parser sources</a></li>
<li><a href="#sec-9-9-5-4">9.9.5.4. Move non-parser sources in <code>swak4FoamParsers</code> into sub-directories</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-10">9.10. 2013-02-28 - version number : 0.2.2</a>
<ul>
<li><a href="#sec-9-10-1">9.10.1. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-10-1-1">9.10.1.1. Python-integration library renamed</a></li>
<li><a href="#sec-9-10-1-2">9.10.1.2. Change in write behaviour of <code>expressionField</code> and <code>manipulateField</code></a></li>
</ul>
</li>
<li><a href="#sec-9-10-2">9.10.2. Bug fixes</a>
<ul>
<li><a href="#sec-9-10-2-1">9.10.2.1. 2.x-branch did not compile on 2.0</a></li>
<li><a href="#sec-9-10-2-2">9.10.2.2. <code>groovyBC</code>: <code>value</code> reset to \(0\) during construction</a></li>
<li><a href="#sec-9-10-2-3">9.10.2.3. Unspecified <code>value</code> in <code>addGlobalVariables</code> produces obscure error message</a></li>
<li><a href="#sec-9-10-2-4">9.10.2.4. Did not compile on <code>1.6-ext</code></a></li>
<li><a href="#sec-9-10-2-5">9.10.2.5. Python integration did not get global variables</a></li>
<li><a href="#sec-9-10-2-6">9.10.2.6. <code>maintainanceScripts/makeSwakVersionFile.py</code> failed when no Mercurial is installed</a></li>
<li><a href="#sec-9-10-2-7">9.10.2.7. Non-uniform variables for <code>internalField</code> only worked correctly for volume fields</a></li>
<li><a href="#sec-9-10-2-8">9.10.2.8. Division of <code>faceFields</code> causes division by zero</a></li>
<li><a href="#sec-9-10-2-9">9.10.2.9. <code>valuePatches</code> did not work for mapped patches</a></li>
<li><a href="#sec-9-10-2-10">9.10.2.10. Recent versions of 2.1.x break compilation of <code>CommonValueExpressionDriver.C</code></a></li>
<li><a href="#sec-9-10-2-11">9.10.2.11. <code>expressionField</code> and <code>manipulateField</code> did not honor the <code>outputControl</code>-entry</a></li>
<li><a href="#sec-9-10-2-12">9.10.2.12. Double <code>namespace Foam</code> makes compilation of <code>groovyJump</code> fail with <code>icc</code></a></li>
</ul>
</li>
<li><a href="#sec-9-10-3">9.10.3. New Examples</a>
<ul>
<li><a href="#sec-9-10-3-1">9.10.3.1. Cases from the <i>OSCFD12</i> Conference in London</a></li>
</ul>
</li>
<li><a href="#sec-9-10-4">9.10.4. New features</a>
<ul>
<li><a href="#sec-9-10-4-1">9.10.4.1. Access fields from different cases</a></li>
<li><a href="#sec-9-10-4-2">9.10.4.2. Patch-expressions now support mapped-patches</a></li>
<li><a href="#sec-9-10-4-3">9.10.4.3. Function objects that manipulate time</a></li>
<li><a href="#sec-9-10-4-4">9.10.4.4. Plugin-functions for chemical model</a></li>
<li><a href="#sec-9-10-4-5">9.10.4.5. Plugin-functions for radiation model</a></li>
</ul>
</li>
<li><a href="#sec-9-10-5">9.10.5. Enhancements</a>
<ul>
<li><a href="#sec-9-10-5-1">9.10.5.1. Python-Integration now supports numpy-arrays</a></li>
<li><a href="#sec-9-10-5-2">9.10.5.2. Optional parameter <code>writeCode=/=writeFile</code> for <code>pythonIntegrationFunctionObject</code></a></li>
<li><a href="#sec-9-10-5-3">9.10.5.3. Python-integration has convenience-functions to create data files</a></li>
<li><a href="#sec-9-10-5-4">9.10.5.4. Switch off dimension checking in <code>funkyDoCalc</code></a></li>
<li><a href="#sec-9-10-5-5">9.10.5.5. <code>funkySetFields</code> allows accessing fields from another mesh</a></li>
<li><a href="#sec-9-10-5-6">9.10.5.6. <code>funkyDoCalc</code> allows specifying foreign meshes</a></li>
<li><a href="#sec-9-10-5-7">9.10.5.7. Function objects to load and update foreign meshes</a></li>
<li><a href="#sec-9-10-5-8">9.10.5.8. <code>replaxTransientBC</code> outputs timestep</a></li>
<li><a href="#sec-9-10-5-9">9.10.5.9. New accumulation <code>weightedAverage</code></a></li>
<li><a href="#sec-9-10-5-10">9.10.5.10. <code>expressionField</code> reports name and type of the created field</a></li>
<li><a href="#sec-9-10-5-11">9.10.5.11. New boundary condition <code>groovyPartialSlip</code> in <code>groovyStandardBCs</code></a></li>
<li><a href="#sec-9-10-5-12">9.10.5.12. <code>funkySetFields</code> allows switching off the correction of BCs</a></li>
<li><a href="#sec-9-10-5-13">9.10.5.13. Allow selection of additional regions in <code>funkySetFields</code></a></li>
<li><a href="#sec-9-10-5-14">9.10.5.14. Adding a <code>-preloadFields</code>-option to <code>funkySetFields</code></a></li>
<li><a href="#sec-9-10-5-15">9.10.5.15. <code>replayTransientBC</code> now supports all volume types</a></li>
<li><a href="#sec-9-10-5-16">9.10.5.16. <code>replayTransientBC</code> preloads fields</a></li>
<li><a href="#sec-9-10-5-17">9.10.5.17. <code>replayTransientBC</code> allows selection of time-steps</a></li>
</ul>
</li>
<li><a href="#sec-9-10-6">9.10.6. Infrastructure</a>
<ul>
<li><a href="#sec-9-10-6-1">9.10.6.1. <code>Allwmake</code> stops if building of <code>swak4FoamParsers</code> fails</a></li>
<li><a href="#sec-9-10-6-2">9.10.6.2. Header <code>swak.H</code> with version information and macros</a></li>
<li><a href="#sec-9-10-6-3">9.10.6.3. <code>accumulations</code>-entries converted to <code>NamedEnums</code></a></li>
<li><a href="#sec-9-10-6-4">9.10.6.4. Additional maintenance script for special python-versions</a></li>
</ul>
</li>
<li><a href="#sec-9-10-7">9.10.7. Cosmetics</a>
<ul>
<li><a href="#sec-9-10-7-1">9.10.7.1. Change type of <code>name</code> in parsers to <code>word</code></a></li>
<li><a href="#sec-9-10-7-2">9.10.7.2. Updated contributor lists in <code>README</code> and sources</a></li>
<li><a href="#sec-9-10-7-3">9.10.7.3. Rewritten methods that return pointers</a></li>
</ul>
</li>
<li><a href="#sec-9-10-8">9.10.8. Documentation</a>
<ul>
<li><a href="#sec-9-10-8-1">9.10.8.1. Start of the reference-manual</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-11">9.11. 2013-03-18 - version number : 0.2.3</a>
<ul>
<li><a href="#sec-9-11-1">9.11.1. Supports OpenFOAM 2.2</a></li>
<li><a href="#sec-9-11-2">9.11.2. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-11-2-1">9.11.2.1. <code>simpleFunctionObjects</code> and <code>simpleLagrangianFunctionObjects</code> no longer independent from rest</a></li>
</ul>
</li>
<li><a href="#sec-9-11-3">9.11.3. Bug fixes</a>
<ul>
<li><a href="#sec-9-11-3-1">9.11.3.1. Compiles on <code>1.6-ext</code> again</a></li>
<li><a href="#sec-9-11-3-2">9.11.3.2. Missing field files for the OSCFD2012-cases</a></li>
<li><a href="#sec-9-11-3-3">9.11.3.3. Did not compile on <code>2.0.x</code></a></li>
</ul>
</li>
<li><a href="#sec-9-11-4">9.11.4. New features</a>
<ul>
<li><a href="#sec-9-11-4-1">9.11.4.1. Writing of sampled surfaces and sampled sets</a></li>
</ul>
</li>
<li><a href="#sec-9-11-5">9.11.5. Enhancements</a>
<ul>
<li><a href="#sec-9-11-5-1">9.11.5.1. Parsers now support <code>eigenValues</code> and <code>eigenVectors</code></a></li>
<li><a href="#sec-9-11-5-2">9.11.5.2. Extract rows from tensors with component operator</a></li>
<li><a href="#sec-9-11-5-3">9.11.5.3. Variable names that are identical to fields trigger a Warning</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-12">9.12. 2013-05-24 - version number : 0.2.4</a>
<ul>
<li><a href="#sec-9-12-1">9.12.1. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-12-1-1">9.12.1.1. Data directories now generated in folder <code>postProcessing</code> instead of case folder</a></li>
<li><a href="#sec-9-12-1-2">9.12.1.2. <code>ExpressionResult</code> from old versions are not correctly read</a></li>
<li><a href="#sec-9-12-1-3">9.12.1.3. Adaption of cases to 2.2</a></li>
</ul>
</li>
<li><a href="#sec-9-12-2">9.12.2. Infrastructure</a>
<ul>
<li><a href="#sec-9-12-2-1">9.12.2.1. Additional flags for the compilation of the parsers</a></li>
<li><a href="#sec-9-12-2-2">9.12.2.2. Use <code>bash</code> instead of <code>sh</code> in maintainance scripts</a></li>
<li><a href="#sec-9-12-2-3">9.12.2.3. Build-script prints swak-version in the beginning</a></li>
</ul>
</li>
<li><a href="#sec-9-12-3">9.12.3. Cosmetics</a>
<ul>
<li><a href="#sec-9-12-3-1">9.12.3.1. Methods use <code>word</code> for names instead of <code>string</code></a></li>
</ul>
</li>
<li><a href="#sec-9-12-4">9.12.4. Bug fixes</a>
<ul>
<li><a href="#sec-9-12-4-1">9.12.4.1. Compilation on Fujitsu (and probably Intel) compilers</a></li>
<li><a href="#sec-9-12-4-2">9.12.4.2. <code>executeIf</code>-function objects are executed if <code>readDuringConstruction</code> is set to <code>true</code></a></li>
<li><a href="#sec-9-12-4-3">9.12.4.3. Expression results are not correctly read from binary file</a></li>
<li><a href="#sec-9-12-4-4">9.12.4.4. Generated fields like <code>vol()</code> were registered</a></li>
<li><a href="#sec-9-12-4-5">9.12.4.5. Missing files in <code>potentialPitzDaily</code>-case</a></li>
<li><a href="#sec-9-12-4-6">9.12.4.6. <code>log(vol())</code> gives floating point exception</a></li>
</ul>
</li>
<li><a href="#sec-9-12-5">9.12.5. New features</a>
<ul>
<li><a href="#sec-9-12-5-1">9.12.5.1. Library with additional <code>searchableSurfaces</code></a></li>
<li><a href="#sec-9-12-5-2">9.12.5.2. New variable type <code>StackedExpressionResult</code></a></li>
<li><a href="#sec-9-12-5-3">9.12.5.3. Function object <code>dumpGlobalVariable</code></a></li>
<li><a href="#sec-9-12-5-4">9.12.5.4. Function object <code>removeGlobalVariable</code></a></li>
<li><a href="#sec-9-12-5-5">9.12.5.5. Optional entry <code>aliases</code> for parser specifications</a></li>
<li><a href="#sec-9-12-5-6">9.12.5.6. Function object to dump registered objects</a></li>
<li><a href="#sec-9-12-5-7">9.12.5.7. Function object that only executes its children if the run is serial/parallel</a></li>
<li><a href="#sec-9-12-5-8">9.12.5.8. Function objects to calculate distributions</a></li>
</ul>
</li>
<li><a href="#sec-9-12-6">9.12.6. Enhancements</a>
<ul>
<li><a href="#sec-9-12-6-1">9.12.6.1. Global variables now can be subclasses of <code>ExpressionResult</code></a></li>
<li><a href="#sec-9-12-6-2">9.12.6.2. <code>endTime</code> now added to Python-wrapper</a></li>
<li><a href="#sec-9-12-6-3">9.12.6.3. Force write if <code>endTime</code> set by time-manipulation is reached</a></li>
<li><a href="#sec-9-12-6-4">9.12.6.4. Function-objects based on <code>patch</code> from the <code>simpleFunctionObjects</code> now accept regular expression</a></li>
<li><a href="#sec-9-12-6-5">9.12.6.5. More flexible output control for functionObjects based on <code>simpleFunctionObjects</code></a></li>
<li><a href="#sec-9-12-6-6">9.12.6.6. <code>swakDataEntry</code> improved</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-13">9.13. 2014-01-24 - version number : 0.3.0</a>
<ul>
<li><a href="#sec-9-13-1">9.13.1. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-13-1-1">9.13.1.1. Support of <i>old</i> <code>1.6-ext</code> lost</a></li>
</ul>
</li>
<li><a href="#sec-9-13-2">9.13.2. New supported versions</a>
<ul>
<li><a href="#sec-9-13-2-1">9.13.2.1. Added support for Foam 3.0</a></li>
</ul>
</li>
<li><a href="#sec-9-13-3">9.13.3. Infrastructure</a>
<ul>
<li><a href="#sec-9-13-3-1">9.13.3.1. Make error messages in <code>Allwmake</code> more verbose</a></li>
<li><a href="#sec-9-13-3-2">9.13.3.2. <code>simpleFunctionObjects</code> no longer considered an independent project</a></li>
<li><a href="#sec-9-13-3-3">9.13.3.3. <code>Allwmake</code> makes sure that swak is compiled for the same installation</a></li>
<li><a href="#sec-9-13-3-4">9.13.3.4. Additional macros for Debugging output</a></li>
<li><a href="#sec-9-13-3-5">9.13.3.5. Separate option for compiling the output of <code>flex</code></a></li>
<li><a href="#sec-9-13-3-6">9.13.3.6. Fixing race condition with big <code>WM_NCOMPPROCS</code></a></li>
<li><a href="#sec-9-13-3-7">9.13.3.7. Updates to Debian-packaging</a></li>
</ul>
</li>
<li><a href="#sec-9-13-4">9.13.4. Documentation</a>
<ul>
<li><a href="#sec-9-13-4-1">9.13.4.1. Documentation of <code>accumulations</code></a></li>
<li><a href="#sec-9-13-4-2">9.13.4.2. General documentation of the Python-embedding</a></li>
</ul>
</li>
<li><a href="#sec-9-13-5">9.13.5. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-13-5-1">9.13.5.1. <code>outputControlMode</code> <code>timestep</code> renamed to <code>timeStep</code></a></li>
</ul>
</li>
<li><a href="#sec-9-13-6">9.13.6. Bug fixes</a>
<ul>
<li><a href="#sec-9-13-6-1">9.13.6.1. Missing <code>timeSet</code> in function-objects</a></li>
<li><a href="#sec-9-13-6-2">9.13.6.2. <code>sourceImplicit</code> unstable</a></li>
<li><a href="#sec-9-13-6-3">9.13.6.3. Fixed bug were only one <code>swakCoded</code> worked at a time</a></li>
<li><a href="#sec-9-13-6-4">9.13.6.4. Incorrectly read entries in <code>swakCoded</code></a></li>
<li><a href="#sec-9-13-6-5">9.13.6.5. No logical variables found by most parsers</a></li>
<li><a href="#sec-9-13-6-6">9.13.6.6. <code>sampledSurface</code> not correctly updated at time of write</a></li>
<li><a href="#sec-9-13-6-7">9.13.6.7. <code>sumMag</code>-accumulation now working</a></li>
<li><a href="#sec-9-13-6-8">9.13.6.8. Calculation of weight fields failed if size on one processor was \(0\)</a></li>
<li><a href="#sec-9-13-6-9">9.13.6.9. <code>groovyTotalPressure</code> does not read <code>value</code></a></li>
<li><a href="#sec-9-13-6-10">9.13.6.10. For multiple times the option <code>addDummyPhi</code> makes <code>funkySetFields</code> crash</a></li>
<li><a href="#sec-9-13-6-11">9.13.6.11. <code>aliases</code> not constructed from dictionary</a></li>
<li><a href="#sec-9-13-6-12">9.13.6.12. Gravity not correctly passed in <code>evolveXXCloud</code></a></li>
<li><a href="#sec-9-13-6-13">9.13.6.13. <code>writeOften</code> writes all the time</a></li>
<li><a href="#sec-9-13-6-14">9.13.6.14. Python-integration does not return single scalars as uniform</a></li>
<li><a href="#sec-9-13-6-15">9.13.6.15. <code>basicSource</code> working again for OF 2.1</a></li>
<li><a href="#sec-9-13-6-16">9.13.6.16. <code>faceZone</code> and <code>faceSet</code> do not correctly work on parallel runs</a></li>
<li><a href="#sec-9-13-6-17">9.13.6.17. Allow user to override <code>notImplemented</code> in <code>movePoints</code> and <code>moveMesh</code> of function objects</a></li>
</ul>
</li>
<li><a href="#sec-9-13-7">9.13.7. New features</a>
<ul>
<li><a href="#sec-9-13-7-1">9.13.7.1. Function object that executes if the OpenFOAM-version is right</a></li>
<li><a href="#sec-9-13-7-2">9.13.7.2. Function object that is only executed if the current time is the <code>startTime</code></a></li>
<li><a href="#sec-9-13-7-3">9.13.7.3. Function object that calculates the potential flow</a></li>
<li><a href="#sec-9-13-7-4">9.13.7.4. Function object that forces a recalculation of <code>phi</code></a></li>
<li><a href="#sec-9-13-7-5">9.13.7.5. Function object that corrects thermophysical model</a></li>
<li><a href="#sec-9-13-7-6">9.13.7.6. Function objects that recalculate the energy of a thermophysical model</a></li>
<li><a href="#sec-9-13-7-7">9.13.7.7. Function object that calculates the average of one variable as a function of another</a></li>
<li><a href="#sec-9-13-7-8">9.13.7.8. New utility <code>fieldReport</code></a></li>
<li><a href="#sec-9-13-7-9">9.13.7.9. New utility <code>funkyPythonPostproc</code></a></li>
<li><a href="#sec-9-13-7-10">9.13.7.10. New utility <code>funkySetLagrangianParticle</code></a></li>
</ul>
</li>
<li><a href="#sec-9-13-8">9.13.8. Enhancements</a>
<ul>
<li><a href="#sec-9-13-8-1">9.13.8.1. Additional parser for lagrangian particles</a></li>
<li><a href="#sec-9-13-8-2">9.13.8.2. Function objects that calculate PDEs can use fields from memory</a></li>
<li><a href="#sec-9-13-8-3">9.13.8.3. Function objects that calculate PDEs can write fields before solving</a></li>
<li><a href="#sec-9-13-8-4">9.13.8.4. simpleFunctionObjects have addition <code>outputControlMode</code></a></li>
<li><a href="#sec-9-13-8-5">9.13.8.5. Conditional functionObjects now have optional <code>else</code></a></li>
<li><a href="#sec-9-13-8-6">9.13.8.6. <code>swakCoded</code> now allows addition of data to functionObject</a></li>
<li><a href="#sec-9-13-8-7">9.13.8.7. Parsers in <code>swakFiniteArea</code> no also have complete tensor-operations</a></li>
<li><a href="#sec-9-13-8-8">9.13.8.8. <code>swakExpressionDistribution</code> now allows non-scalar weights</a></li>
<li><a href="#sec-9-13-8-9">9.13.8.9. More options for <code>accumulations</code></a></li>
<li><a href="#sec-9-13-8-10">9.13.8.10. Python code files are now searched more flexible</a></li>
<li><a href="#sec-9-13-8-11">9.13.8.11. Python integration now uses <code>IPython</code> if possible</a></li>
<li><a href="#sec-9-13-8-12">9.13.8.12. Preload libraries in the Python integration</a></li>
<li><a href="#sec-9-13-8-13">9.13.8.13. Added standard function <code>weight()</code></a></li>
<li><a href="#sec-9-13-8-14">9.13.8.14. <code>funkyDoCalc</code> now writes files</a></li>
<li><a href="#sec-9-13-8-15">9.13.8.15. PDE-functionObjects now relax their equations</a></li>
<li><a href="#sec-9-13-8-16">9.13.8.16. Full set of <code>laplacian</code>-operations in <code>internalField</code>-parser</a></li>
<li><a href="#sec-9-13-8-17">9.13.8.17. Function object <code>swakExpression</code> now has optional <code>mask</code></a></li>
</ul>
</li>
<li><a href="#sec-9-13-9">9.13.9. Examples</a>
<ul>
<li><a href="#sec-9-13-9-1">9.13.9.1. Moved the OSCFD-examples to a different directory</a></li>
<li><a href="#sec-9-13-9-2">9.13.9.2. Added examples from the swak-training at the 8th Workshop</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-9-14">9.14. 2014-07-11 - version number : 0.3.1</a>
<ul>
<li><a href="#sec-9-14-1">9.14.1. Incompatibilities to previous versions</a>
<ul>
<li><a href="#sec-9-14-1-1">9.14.1.1. Additional required code snipplet for some Python-functionObjects</a></li>
<li><a href="#sec-9-14-1-2">9.14.1.2. Dimension required for stuff in <code>swakSourceFields</code></a></li>
</ul>
</li>
<li><a href="#sec-9-14-2">9.14.2. New supported versions</a>
<ul>
<li><a href="#sec-9-14-2-1">9.14.2.1. Added support for OpenFOAM 2.3</a></li>
<li><a href="#sec-9-14-2-2">9.14.2.2. Added support for Foam-Extend 3.1</a></li>
</ul>
</li>
<li><a href="#sec-9-14-3">9.14.3. Infrastructure</a>
<ul>
<li><a href="#sec-9-14-3-1">9.14.3.1. Improved macros for Foam versions</a></li>
<li><a href="#sec-9-14-3-2">9.14.3.2. Support for private installation of bison</a></li>
</ul>
</li>
<li><a href="#sec-9-14-4">9.14.4. Documentation</a>
<ul>
<li><a href="#sec-9-14-4-1">9.14.4.1. Information about macro-expansion added</a></li>
</ul>
</li>
<li><a href="#sec-9-14-5">9.14.5. Bug fixes</a>
<ul>
<li><a href="#sec-9-14-5-1">9.14.5.1. Invalid entries in distributions from <code>swakExpressionAverageDistribution</code></a></li>
<li><a href="#sec-9-14-5-2">9.14.5.2. <code>swakExpressionAverageDistribution</code> not correctly working in parallel</a></li>
<li><a href="#sec-9-14-5-3">9.14.5.3. <code>fvOptions</code> in <code>swakSourceFields</code> now actually working</a></li>
<li><a href="#sec-9-14-5-4">9.14.5.4. <code>fvOptions</code> did not honor the <code>selectionMode</code>-entry</a></li>
<li><a href="#sec-9-14-5-5">9.14.5.5. <code>patchFunctionObject</code> fails for some parallel runs</a></li>
<li><a href="#sec-9-14-5-6">9.14.5.6. <code>funkyPythonPostproc</code> not executing function objects</a></li>
<li><a href="#sec-9-14-5-7">9.14.5.7. Temporary fields shadow 'real' fields in <code>foam-extend-3.1</code></a></li>
</ul>
</li>
<li><a href="#sec-9-14-6">9.14.6. New features</a>
<ul>
<li><a href="#sec-9-14-6-1">9.14.6.1. Boundary condition <code>groovyFixedNormalSlip</code> in <code>groovyStandardBCs</code></a></li>
<li><a href="#sec-9-14-6-2">9.14.6.2. New utility <code>funkyWarpMesh</code></a></li>
<li><a href="#sec-9-14-6-3">9.14.6.3. Function objects that write additional writes depending on the flow conditions</a></li>
<li><a href="#sec-9-14-6-4">9.14.6.4. Function object <code>writeOldTimesOnSignal</code> with new signal handlers</a></li>
<li><a href="#sec-9-14-6-5">9.14.6.5. New library with <code>fvOptions</code></a></li>
<li><a href="#sec-9-14-6-6">9.14.6.6. Macro expansion</a></li>
</ul>
</li>
<li><a href="#sec-9-14-7">9.14.7. Enhancements</a>
<ul>
<li><a href="#sec-9-14-7-1">9.14.7.1. <code>expressionField</code> now allows setting of dimensions</a></li>
<li><a href="#sec-9-14-7-2">9.14.7.2. New function <code>distToPatch</code> in Field-parser</a></li>
<li><a href="#sec-9-14-7-3">9.14.7.3. <code>panicDump</code> now allows storing old values</a></li>
<li><a href="#sec-9-14-7-4">9.14.7.4. <code>swakExpressionAverageDistribution</code> records extremes and number of samples</a></li>
<li><a href="#sec-9-14-7-5">9.14.7.5. Python-function objects that return a boolean now have a initialization-snipplet</a></li>
<li><a href="#sec-9-14-7-6">9.14.7.6. <code>fvOptions</code>-support in PDE-function objects</a></li>
</ul>
</li>
<li><a href="#sec-9-14-8">9.14.8. Examples</a>
<ul>
<li><a href="#sec-9-14-8-1">9.14.8.1. <code>other/simpleBendedPipe</code></a></li>
</ul>
</li>
</ul>
</li>
</ul>
</li>
</ul>
</div>
</div>



# Description<a id="sec-1" name="sec-1"></a>

A collection of libraries and tools that let the user handle
OpenFOAM-data based on expressions

# About this document<a id="sec-2" name="sec-2"></a>

## Scope<a id="sec-2-1" name="sec-2-1"></a>

This file gives an overview of `swak4Foam` and a history of the
features. It is not a canonical documentation.

## Technical<a id="sec-2-2" name="sec-2-2"></a>

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

# Contributors etc<a id="sec-3" name="sec-3"></a>

## Original Author<a id="sec-3-1" name="sec-3-1"></a>

Bernhard Gschaider (bgschaid@ice-sf.at)

## Current Maintainer<a id="sec-3-2" name="sec-3-2"></a>

Bernhard Gschaider (bgschaid@ice-sf.at)

## Contributors<a id="sec-3-3" name="sec-3-3"></a>

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

If anyone is forgotten: let me know

According to the commits in the `mercurial`-repository (and
the repositories of the projects from which swak emerged)
contributors are (ordered by the year of their first contribution):

-   2006-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
-   2008 Hannes Kroeger (hannes@kroegeronline.net)
-   2008-2009, 2012 Martin Beaudoin, Hydro-Quebec (beaudoin.martin@ireq.ca)
-   2010 Marianne Mataln <mmataln@ice-sf>
-   2010 Oliver Borm (oli.borm@web.de)
-   2011 Alexey Petrov <alexey.petrov.nnov@gmail.com>
-   2011 Petr Vita <petr.vita@unileoben.ac.at>
-   2012-2014 Bruno Santos <wyldckat@gmail.com>
-   2013 Georg Reiss <georg.reiss@ice-sf.at>
-   2014 Hrvoje Jasak <h.jasak@wikki.co.uk>

## Documentation<a id="sec-3-4" name="sec-3-4"></a>

See: <http://openfoamwiki.net/index.php/contrib/swak4Foam>

# Installation/Compilation<a id="sec-4" name="sec-4"></a>

## Requirements<a id="sec-4-1" name="sec-4-1"></a>

-   Version 2.0, 2.1 or 2.2 of OpenFOAM (a version that works with 1.6,
    1.6-ext and 1.7 is available separately) and version 3.0 of Foam
-   the compiler generators `bison` and `flex`
    -   **bison:** `swak4Foam` is known to work with `bison` version 2.4 and
        higher. Version 2.3 compiles but the plugin-functionality does
                 not work correctly
    -   **flex:** since the introduction of the plugin functions at least
        a flex version of `2.5.33` is required (`2.5.35` is the
        lowest **confirmed** version)

Both of these are mainstream packages (they are for instance needed
to compile `gcc`) and should exist on every Linux distribution. Use
the package manager of your distribution to install them and **only**
if the compilation process of `swak4Foam` complains about too low
versions try to install them from other sources.

`swak4Foam` tries to keep the requirements on these as low as
possible and sometimes lower versions than the ones reported **may**
work. If they do please report so.

The version of bison can be checked with

    bison -V

The version of flex with

    flex -V

## Building<a id="sec-4-2" name="sec-4-2"></a>

    wmake all

at the base directory should build all the libraries and
tools.

Rerun the command to make sure that there was no problem with the
compilation (this should be quite fast and only report libraries
being created and some administrative stuff)

### Additional configuration<a id="sec-4-2-1" name="sec-4-2-1"></a>

Some features (currently only the `Python`-integration may need
third party software. The paths to these packages can be configured
in a file `swakConfiguration` (an example file
`swakConfiguration.example` is provided). If that file is not
present these unconfigured features will not be compiled.

Environment variables that can be set in this file are:

-   **SWAK<sub>PYTHON</sub><sub>INCLUDE</sub>:** Path to the `Python.h` file of the used
    `python`-installation
-   **SWAK<sub>PYTHON</sub><sub>LINK</sub>:** Options to link the `python`-library to the
    library for the `python`-integration
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

### Possible compilation failure with old 2.0.x-versions<a id="sec-4-2-2" name="sec-4-2-2"></a>

With older versions of 2.0.x (or 2.0 or 2.0.1) it is possible that
the compilation of  `swakCodedFunctionObject` will fail. In that
case remove the last parameter to the
`codedFunctionObject`-constructor in `swakCodedFunctionObject.C`
(it is clearly marked by a comment)

### Special versions of the python integration<a id="sec-4-2-3" name="sec-4-2-3"></a>

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

1.  `SWAK_PYTHON_INCLUDE`
2.  `SWAK_PYTHON_LINK`
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

## Global installation<a id="sec-4-3" name="sec-4-3"></a>

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

## Packaging<a id="sec-4-4" name="sec-4-4"></a>

### Debian<a id="sec-4-4-1" name="sec-4-4-1"></a>

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

# Contents<a id="sec-5" name="sec-5"></a>

## Libraries<a id="sec-5-1" name="sec-5-1"></a>

Collection of Libraries

### `swak4FoamParsers`<a id="sec-5-1-1" name="sec-5-1-1"></a>

The basis of `swak4Foam`: the expression parsers with the logic to
access the *OpenFOAM* data-structures.

None of the other software pieces compile without it.

Also defines a subclass to `DataEntry` that uses
`swak`-expressions and a function object `initSwakFunctionObject`
that might be used if this fails

### `simpleFunctionObjects`<a id="sec-5-1-2" name="sec-5-1-2"></a>

A collection of function objects that was previously separately
available at
<http://openfoamwiki.net/index.php/Contrib_simpleFunctionObjects>.

Provides consistent output of values (on patches and fields) and
more.

### `groovyBC`<a id="sec-5-1-3" name="sec-5-1-3"></a>

Implements the infamous `groovyBC`. A boundary condition that
allows arbitrary expressions in the field-file

### `swakFunctionObjects`<a id="sec-5-1-4" name="sec-5-1-4"></a>

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

### `simpleSwakFunctionObjects`<a id="sec-5-1-5" name="sec-5-1-5"></a>

Function objects based on the `simpleFunctionObjects`-library
(which is a prerequisite for compiling it).

Evaluate expressions and output the results

### `swakSourceFields`<a id="sec-5-1-6" name="sec-5-1-6"></a>

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

### `swakFvOptions`<a id="sec-5-1-7" name="sec-5-1-7"></a>

Starting from OpenFOAM 2.2 this library has additional `fvOptions`

### `swakTopoSources`<a id="sec-5-1-8" name="sec-5-1-8"></a>

`topoSources` for `cellSet` and `faceSet`. Can be used with the
`cellSet` and `faceSet`-utilities

### `swakFiniteArea`<a id="sec-5-1-9" name="sec-5-1-9"></a>

Implements parsers for the `finiteArea`-stuff in 1.6-ext. Also
implements `groovyBC` for `areaField` and `expressionField` and
other function objects

### `groovyStandardBCs`<a id="sec-5-1-10" name="sec-5-1-10"></a>

Collection of boundary conditions that give standard boundary
conditions the possibility to use expression for the coefficients

Contributions to this library are explicitly encouraged. Please
use the Mercurial-branch `groovyStandardBCs` to *groovyify*
standard boundary conditions.

### `pythonIntegration`<a id="sec-5-1-11" name="sec-5-1-11"></a>

Embeds a `Python`-interpreter.
-   **`pythonIntegrationFunctionObject`:** Executes `Python`-code
    at the usual execution times of functionObjects. The
    interpreter keeps its state

This library is only compiled if the paths to the Python-Headers
are configured in the `swakConfiguration`-file (see above)

### `simpleLagrangianFunctionObjects`<a id="sec-5-1-12" name="sec-5-1-12"></a>

Function objects that allow the easy addition of lagrangian
clouds to a case. No other libraries from `swak4Foam` are needed
for this

### `simpleSearchableSurfaces`<a id="sec-5-1-13" name="sec-5-1-13"></a>

Additional `searchableSurfaces` (for use in `snappyHexMesh`)
which include boolean operations for other surfaces and
coordinate transformations

### `fluFunctionObjectDriver`<a id="sec-5-1-14" name="sec-5-1-14"></a>

Driver for `functionObjects` that implemented entirely in `Python`
using the `pythonFlu`-library

### `swakLagrangianParser`<a id="sec-5-1-15" name="sec-5-1-15"></a>

Parser for calculating expressions on clouds of lagrangian particles

### `functionPlugins`<a id="sec-5-1-16" name="sec-5-1-16"></a>

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

#### lagrangianCloudAdaptors-directory<a id="sec-5-1-16-1" name="sec-5-1-16-1"></a>

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

## Utilities<a id="sec-5-2" name="sec-5-2"></a>

### `funkySetFields`<a id="sec-5-2-1" name="sec-5-2-1"></a>

Utility that allows creation and manipulation of files with
expressions

### `funkySetAreaFields`<a id="sec-5-2-2" name="sec-5-2-2"></a>

Utility like `funkySetFields` for `areaFields` (only works with
1.6-ext)

### `funkySetBoundaryField`<a id="sec-5-2-3" name="sec-5-2-3"></a>

Sets any field on a boundary to a non-uniform value based on an
expression.

Acts without deeper understanding of the underlying boundary condition

### `replayTransientBC`<a id="sec-5-2-4" name="sec-5-2-4"></a>

Utility to quickly test whether a groovyBC gives the expected
results. Writes the specified fields with the applied boundary
condition but doesn't do anything else.

Can be used for other BCs as well

### `funkyDoCalc`<a id="sec-5-2-5" name="sec-5-2-5"></a>

Evaluates expressions that are listed in a dictionary using data
that is found on the disc and prints summarized data (min, max,
average, sum) to the screen

### `calcNonUniformOffsetsForMapped`<a id="sec-5-2-6" name="sec-5-2-6"></a>

Calculates the `offsets`-entry in the `polyMesh/boundary`-file
according to the specification in a dictionary. Only needed if you
have mapped patches and the regular uniform offset is not enough
for your purposes

### `fieldReport`<a id="sec-5-2-7" name="sec-5-2-7"></a>

Utility that quickly does some quantitative analysis (minimum,
maximum, average etc ) on a field on the disc (internal field but
also patches, sets, zones, &#x2026;)

### `funkyPythonPostproc`<a id="sec-5-2-8" name="sec-5-2-8"></a>

This utility loads specified fields into memory, executes a list
of user-specified function objects whose data is then passed to a
python script which does the user-specified analysis.

### `funkySetLagrangianField`<a id="sec-5-2-9" name="sec-5-2-9"></a>

Utility to calculate fields for a lagrangian cloud (or setting it
up from scratch)

## Examples<a id="sec-5-3" name="sec-5-3"></a>

If not otherwise noted cases are prepared by a simple
`blockMesh`-call.

**Note**: All the cases here are strictly for demonstration purposes and
resemble nothing from the 'real world'

**Note**: Due to various changes in the case syntax between OpenFOAM
 2.0 and 2.2 (for instance `thermophysicalProperties`, wall
 functions etc) not all of the examples work with all
 OpenFOAM-installations "out of the box". Slight adaptions may be
 necessary

### groovyBC<a id="sec-5-3-1" name="sec-5-3-1"></a>

The old `groovyBC`-Demos

#### pulsedPitzDaily<a id="sec-5-3-1-1" name="sec-5-3-1-1"></a>

-   **Solver:** pisoFoam
-   **Also demonstrates:** `manipulateField`, `expressionField` and
    `clearField` from the
    `swakFunctionObjects`. `patchExpression` from
    `simpleSwakFunctionObjects`. `solveLaplacianPDE` and
    `solveTransportPDE` for solving equations,
    `pythonIntegration` with calculations using `numpy`

#### wobbler<a id="sec-5-3-1-2" name="sec-5-3-1-2"></a>

-   **Solver:** solidDisplacementFoam

#### circulatingSplash<a id="sec-5-3-1-3" name="sec-5-3-1-3"></a>

-   **Solver:** interDyMFoam

#### movingConeDistorted<a id="sec-5-3-1-4" name="sec-5-3-1-4"></a>

-   **Solver:** pimpleDyMFoam
-   **Also demonstrates:** `swakExpression` with `surface`. Due to a
    problem described below this currently doesn't work

#### average-t-junction<a id="sec-5-3-1-5" name="sec-5-3-1-5"></a>

-   **Solver:** pimpleFoam
-   **Mesh preparation:** Execute the script `prepare.sh` in that
    directory (requires PyFoam: if not installed change in the
    `boundary`-file the type of the `defaultFaces` to `wall`)

#### delayed-t-junction<a id="sec-5-3-1-6" name="sec-5-3-1-6"></a>

-   **Solver:** pimpleFoam
-   **Demonstrates:** Delayed variables to simulate an inflow that
    depends on the value of the outflow

#### multiRegionHeaterFeedback<a id="sec-5-3-1-7" name="sec-5-3-1-7"></a>

-   **Solver:** chtMultiRegionFoam
-   **Mesh preparation:** Execute the script `prepare.sh` in that
    directory
-   **Also demonstrated:** `patchExpression` and `swakExpression` from
    `simpleSwakFunctionObjects`.

#### fillingTheDam<a id="sec-5-3-1-8" name="sec-5-3-1-8"></a>

-   **Solver:** interFoam
-   **Also demonstrates:** Usage of a `sampledSet` defined in the
    `controlDict` do determine the average filling height. Also
    stored variables for not switching back once the criterion
    is reached. Global variables defined by a function object

#### sprinklingInlet<a id="sec-5-3-1-9" name="sec-5-3-1-9"></a>

-   **Solver:** interFoam
-   **Description:** Winner of the `swak4Foam`-competition at the 6th
    OpenFOAM-Workshop (2011). By Peter Keller

#### potentialPitzDaily<a id="sec-5-3-1-10" name="sec-5-3-1-10"></a>

-   **Solver:** potentialFoam
-   **Description:** Demonstrates the use of `groovyB` with
    `potentialFoam` (also a problem connected with
    that). Provided by Martin Backer

### FunkyDoCalc<a id="sec-5-3-2" name="sec-5-3-2"></a>

Example dictionaries for `funkyDoCalc`

### FunkySetFields<a id="sec-5-3-3" name="sec-5-3-3"></a>

Example dictionary for `funkySetFields`

### FunkySetBoundaryFields<a id="sec-5-3-4" name="sec-5-3-4"></a>

Example dictionary for `funkySetBoundaryFields`. Sets nonsense
boundary conditions for the world famous `damBreak`-case

### InterFoamWithSources<a id="sec-5-3-5" name="sec-5-3-5"></a>

Demonstrates usage of `expressionSource`

Due to differences in the original `interFoam`-solver this doesn't
work on certain OpenFOAM-versions (most specifically
`1.6-ext`). The current solver works with 2.1. For older
OF-versions use the sources labeled `_pre2.1`.

The only modifications to the original solver are found at the end
of `createFields.H` and in `UEqn.H` (the added source terms).

### InterFoamWithFixed<a id="sec-5-3-6" name="sec-5-3-6"></a>

Demonstrates usage of `forceEquation`

Due to differences in the original `interFoam`-solver this doesn't
work on certain OpenFOAM-versions (most specifically
`1.6-ext`). The current solver works with 2.1. For older
OF-versions use the sources labeled `_pre2.1`.

The only modifications to the original solver are found at the end
of `createFields.H` and in `UEqn.H` (the fixing of the velocities).

#### `interFoamWithSources`<a id="sec-5-3-6-1" name="sec-5-3-6-1"></a>

Slightly modified version of `interFoam`. Adds a source term to
the momentum equation. The source term is an expression that is
defined at run-time

#### `mixingThing`<a id="sec-5-3-6-2" name="sec-5-3-6-2"></a>

Demonstration case for it.
-   **Preparation:** Run the script `prepare.sh` to prepare the case

### FiniteArea<a id="sec-5-3-7" name="sec-5-3-7"></a>

Demonstration of the `finiteArea`-stuff that works with 1.6-ext

#### `swakSurfactantFoam`<a id="sec-5-3-7-1" name="sec-5-3-7-1"></a>

Variation of `surfactantFoam` that adds an `expressionSource`

#### `planeTransport`<a id="sec-5-3-7-2" name="sec-5-3-7-2"></a>

Demonstration case
-   **Preparation:** Use `blockMesh` and `makeFaMesh`
-   **Solver:** `surfactantFoam` (without source term) or
    `swakSurfactantFoam`
-   **Demonstrates:** FAM-specific `swakExpressions` and `groovyBC`
                           (as well as the `expressionSource`)

### FvOptions<a id="sec-5-3-8" name="sec-5-3-8"></a>

Cases that demonstrate `swakSourceFields`

#### angleDuctWithSources<a id="sec-5-3-8-1" name="sec-5-3-8-1"></a>

-   **Solver:** rhoPimpleFoam
-   **Mesh preparation:** Execute `prepareCase.sh`
-   **Demonstrates:** Simple expression sources (adds a *heat source*
                           to the solid)

### other<a id="sec-5-3-9" name="sec-5-3-9"></a>

Cases that don't have a `groovyBC`

#### `angledDuctImplicit`<a id="sec-5-3-9-1" name="sec-5-3-9-1"></a>

-   **Solver:** rhoPorousMRFSimpleFoam
-   **Mesh preparation:** Execute the `makeMesh.sh`-script in that
    directory. If you want to run in parallel call the
    `decomposeMesh.sh`-script with the number of processors as
    an argument
-   **Demonstrates:** Usage of the `swakTopoSources`. Compares
    different approaches to evaluating with the
    `swakExpression`-functionObject. Also an example dictionary
    that demonstrates the use of `funkyDoCalc`. Demonstrates
    "live" comparing to another case using *foreign meshes*

#### `angledDuctImplicitTransient`<a id="sec-5-3-9-2" name="sec-5-3-9-2"></a>

-   **Solver:** rhoPorousMRFPimpleFoam
-   **Mesh preparation:** Execute the `makeMesh.sh`-script in that
    directory. If you want to run in parallel call the
    `decomposeMesh.sh`-script with the number of processors as
    an argument
-   **Demonstrates:** The same as `angledDuctImplicit` but also the
    output of temporal changes

#### `capillaryRise`<a id="sec-5-3-9-3" name="sec-5-3-9-3"></a>

-   **Solver:** interFoam
-   **Case preparation:** run the supplied script `prepareCase.sh`
-   **Demonstrates:** Usage of a sampled surface to track the
    interface in a VOF-simulation

#### `mixingDam`<a id="sec-5-3-9-4" name="sec-5-3-9-4"></a>

-   **Solver:** interFoam
-   **Case preparation:** run the supplied script `prepareCase.sh`
-   **Demonstrates:** Emulate a "moving gravitation" by using the
    `manipulateField`-functionObject to recalculate
    `gh` and `ghf`

#### `counterFlowFlame2DInitialized`<a id="sec-5-3-9-5" name="sec-5-3-9-5"></a>

Demonstrates initializing a case with initial conditions that
approximate the real flow.
-   **Solver:** reactingFoam
-   **Case preparation:** run the supplied script
    `prepareCase.sh`. **Caution:** this has to be done **every**
    time before running the case
-   **Demonstrates:** Demonstrates setting calculated initial
    conditions with functionObjects

#### `simpleBendedPipe`<a id="sec-5-3-9-6" name="sec-5-3-9-6"></a>

Demonstrates the use of `funkyWarpMesh` and post-processing on
bended pip geometries
-   **Solver:** simpleFoam
-   **Case preparation:** Run `prepare.sh`

### FromPresentations<a id="sec-5-3-10" name="sec-5-3-10"></a>

Cases that were shown in some presentations

#### OSCFD<sub>cleaningTank3D</sub><a id="sec-5-3-10-1" name="sec-5-3-10-1"></a>

-   **Solver:** interFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** The case described on the slides of the
    talk about `swak4Foam` at the  OSCFD-conference
    2012 in London
-   **Demonstrates:** Boundary conditions, function objects, global
    variables and delayed variables

#### OSCFD<sub>cleaningTank2D</sub><a id="sec-5-3-10-2" name="sec-5-3-10-2"></a>

A 2D-variant of the above case

#### OFW8<sub>sandPitOfCarcoon</sub><a id="sec-5-3-10-3" name="sec-5-3-10-3"></a>

-   **Solver:** twoPhaseEulerFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** Simulate a sand-monster from the StarWars-movie
    "Return of the Jedi"
-   **Demonstrates:** Use of `funkySetFields`, `groovyBC` and
    functionObjects for lagrangian particles

#### OFW8<sub>landspeedersInCanyon</sub><a id="sec-5-3-10-4" name="sec-5-3-10-4"></a>

-   **solver:** simpleFoam
-   **Case preparation:** run the `prepareCase.sh`-script
-   **Description:** Simulates two landSpeeders (as seen in the
    StarWars-movie "A New Hope")
-   **Demonstrates:** Advanced searchableSurfaces (for
    `snappyHexMesh`), functionObject for passive
    scalar, functionObject to calculate
    distributions

### PythonIntegration<a id="sec-5-3-11" name="sec-5-3-11"></a>

Demonstrate the integration of `Python`. Mostly using `PyFoam` but
also with `pythonFlu`

#### `manipulatedPitzDaily`<a id="sec-5-3-11-1" name="sec-5-3-11-1"></a>

-   **Solver:** simpleFoam
-   **Demonstrates:** Usage of `PyFoam` to manipulate the
    `fvSolution`-file during the run (possible
    application: unphysical initial conditions
    cause the run to fail during startup with
    "normal" relaxation values)

#### `findPointPitzDaily`<a id="sec-5-3-11-2" name="sec-5-3-11-2"></a>

-   **Solver:** simpleFoam
-   **Demonstrates:** Usage of the `pythonFlu`-integration to find
    the point where the recirculation behind the
    step ends. Also tries to plot the result using
    the `matplotlib`-library

#### `bed2OfPisa`<a id="sec-5-3-11-3" name="sec-5-3-11-3"></a>

-   **Solver:** twoPhaseEulerFoam
-   **Demonstrates:** Usage of `PyFoam` to read the direction of
    gravity and feeding it into a `goovyBC` via
    global variables
-   **Case preparation:** Just call `funkySetFields -time 0`

#### `multiRegionHeaterBuildFunctionObjects`<a id="sec-5-3-11-4" name="sec-5-3-11-4"></a>

-   **Solver:** chtMultiRegionFoam
-   **Demonstrates:** Building the specification of function objects
    at run-time via a Python-script

#### `slicedPitzDaily`<a id="sec-5-3-11-5" name="sec-5-3-11-5"></a>

-   **Solver:** simpleFoam
-   **Demonstrates:** Using a python-script to dynamically generate
    multiple function objects (sampled
    surfaces). Using stack-variables to calculate
    the results and write them. Using a stored
    stack-variable to monitor the pressure at a
    point and stop the run if the pressure didn't
    change there for the last 50 iterations

### `CodeStream`<a id="sec-5-3-12" name="sec-5-3-12"></a>

Demonstrates working together with the `coded`-stuff in OpenFOAM 2.0

### solvePDE<a id="sec-5-3-13" name="sec-5-3-13"></a>

Examples for the `functionObjects` that can solve *Partial
Differential equations*

#### flangeWithPDE<a id="sec-5-3-13-1" name="sec-5-3-13-1"></a>

-   **Solver:** laplacianFoam
-   **Demonstrates:** The usage of the `functionObject` that solves
    the laplacian (Poisson) equation and
    (hopefully) that it gets the same result as the
    native solver
-   **Case preparation:** `Allrun`-script is provided

#### pitzDailyWithPDE<a id="sec-5-3-13-2" name="sec-5-3-13-2"></a>

-   **Solver:** scalarTransportFoam
-   **Demonstrates:** Solving additional transport equations

### BasicSourceSubclasses<a id="sec-5-3-14" name="sec-5-3-14"></a>

These examples test the source terms based on `basicSource`. They
only work with OpenFOAM 2.x and all use the `simpleFoam`-solver

#### pitzDailyWithSwirl<a id="sec-5-3-14-1" name="sec-5-3-14-1"></a>

-   **Demonstrates:** Fixing the values of the velocity in a region
    with `SwakSetValues`

#### pitzDailyWithExplicitPoroPlug<a id="sec-5-3-14-2" name="sec-5-3-14-2"></a>

-   **Demonstrates:** Implementing a simple porous plug by adding the
    Darcy-term as a source term with
    `SwakExplicitSource`

#### pitzDailyWithImplicitPoroPlug<a id="sec-5-3-14-3" name="sec-5-3-14-3"></a>

-   **Demonstrates:** Same as `pitzDailyWithExplicitPoroPlug` but
    with an implicit source term with
    `SwakImplicitSource`

### Lagrangian<a id="sec-5-3-15" name="sec-5-3-15"></a>

Stuff that has to do with lagrangian particles

#### functionObjects<a id="sec-5-3-15-1" name="sec-5-3-15-1"></a>

Tests for the functionObjects that create and evolve a cloud of
particles (library `simpleLagrangianFunctionObjects`)

1.  hotStream

    -   **Solver:** replayTransientBC
    -   **Mesh preparation:** `prepareCase.sh`-script
    -   **Demonstrates:** 3 clouds (kinematic, reacting,
        solidParticle). Loading of a thermophysical
        model with a functionObject. Plugin functions
        for information about the clouds

2.  angledDuctWithBalls

    -   **Solver:** rhoPimpleFoam
    -   **Demonstrates:** Thermo-cloud. Functions for lagrangian
        particles

#### parser<a id="sec-5-3-15-2" name="sec-5-3-15-2"></a>

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
    -   **Mesh preparation:** run the `prepareCase.sh` script to set up
        the mesh and the fields

### SimpleSurface<a id="sec-5-3-16" name="sec-5-3-16"></a>

Examples for the use of the `simpleSearchableSurfaces`-library.

#### twoMotorbikesAndAWall<a id="sec-5-3-16-1" name="sec-5-3-16-1"></a>

-   **Solver:** simpleFoam (alternatively use the
    `caseRun.sh`-script)
-   **Case preparation:** Script `caseSetup.sh` is provided
-   **Demonstrates:** Using the same STL-file more than once in a
    case by applying coordinate
    transformations. **Note:** the physics of this
    case are more than questionable as the frame
    of reference for both motorbikes is wrong

#### littleVolcano<a id="sec-5-3-16-2" name="sec-5-3-16-2"></a>

-   **Solver:** interFoam
-   **Case preparation:** Script `caseSetup.sh` is provided
-   **Demonstrates:** Boolean operations on STL-files

#### chargedPlates<a id="sec-5-3-16-3" name="sec-5-3-16-3"></a>

-   **Solver:** electrostaticFoam
-   **Case preparation:** Script `caseSetup.sh` is provided
-   **Demonstrates:** Boolean operations with regular surfaces

Physics of the case not as expected (charge-distribution)

### tests<a id="sec-5-3-17" name="sec-5-3-17"></a>

Simple test cases for specific features. The names of the
directories should give you a hint what is tested there. Most of
them don't need a real solver but `replayTransientBC` or
`scalarTransportFoam`.

**Note**: for some features these cases are the only example there
 is. Sorry

#### `randomCavity`<a id="sec-5-3-17-1" name="sec-5-3-17-1"></a>

Testing of different seeds for the `rand`-function. Also tests
the `randFixed`-function

### BugCases<a id="sec-5-3-18" name="sec-5-3-18"></a>

These are cases provided by users to demonstrate bugs. Not
maintained nor documented and may be removed at any time

## `maintainanceScripts`<a id="sec-5-4" name="sec-5-4"></a>

Undocumented scripts used for maintaining `swak4Foam`. **If you
don't understand them, don't use them**

## Documentation<a id="sec-5-5" name="sec-5-5"></a>

Additional documentation. Most written in `org-mode`

Current contents:

-   **swak4FoamReference:** Reference manual of swak4Foam. Especially
    concepts like expression syntax and parameters

## releaseTesting<a id="sec-5-6" name="sec-5-6"></a>

Scripts and configuration to test for a release in a virtual
machine using `vagrant`. Also to be used for packaging

# Bug reporting and Development<a id="sec-6" name="sec-6"></a>

## Bug reports<a id="sec-6-1" name="sec-6-1"></a>

The preferred place for bug reports is
<http://sourceforge.net/apps/mantisbt/openfoam-extend/search.php?project_id=10&sticky_issues=on&sortby=last_updated&dir=DESC&hide_status_id=90>

A sourceforge-account is required for reporting

### Things to do before reporting bug<a id="sec-6-1-1" name="sec-6-1-1"></a>

If you're reporting a bug about the compilation please run
`Allwmake` **twice** and only report the messages from the second
run. This makes analyzing the log easier as only the unsuccessful
commands will be reported.

If the problem seems to be a missing library rerun the compilation
to make sure that there wasn't a problem with that.

## Development<a id="sec-6-2" name="sec-6-2"></a>

Contributions to to `swak4Foam` are most welcome. If you want to
contribute clone the Mercurial archive of the sources

    hg clone http://openfoam-extend.hg.sourceforge.net:8000/hgroot/openfoam-extend/swak4Foam

Change to the branch that you want to improve (usually `default`)
and create a new branch

    hg branch <branchName>

where `<branchname>` is an easily identifiable name that makes the
purpose of the branch clear (for instance
`bugfixWrongRandomFunction` or `featureHyperbolicFunctions`). Don't
work on the `default` branch or any other branches that are not
"yours". Such contributions will not be merged

Once development on the branch is finished export the relevant
changesets with

    hg export <nodeID>

(`nodeID` being the ids of "your" changesets) and send them to the
maintainer (or attach them to a bug report on Manits). The changes
will be reviewed and merged into the `default` branch (do not
attempt to do this yourself). Patches generated with `hg export`
make sure that all changes are attributed to the original developer
(you).

An alternative would be the `bundle` command. Just do

    hg bundle <bundlefile>

and then send the `bundlefile`. This will include **all** commits
that are not in the upstream repository and will allow similar
inclusion in the upstream as `export`.

Once you have proven by successfully submitting changesets via `hg
   export` you can ask for write access to the mercurial repository.

Only if you got through Mercurial it can be ensured that your
contribution is recognized (if you want to stay anonymous send
patches).

### Suggest reading<a id="sec-6-2-1" name="sec-6-2-1"></a>

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

### Special branches<a id="sec-6-2-2" name="sec-6-2-2"></a>

Currently the main branches are:
-   **default:** The **main** branch. This is the brancht that the
    general public will receive. It compiles under 1.7
    and 1.6-ext
-   **`port_2.0.x`:** The branch that compiles under OpenFOAM 2.0. This
    will eventually become the `default`-branch
-   **debianPackaging:** Branch for generating new Debian-packages of
    `swak4Foam`. If somebody wants to "inherit" this: contact the
    maintainer
-   **finiteArea:** In this branch the things for the
    `finiteArea`-discretization (only present in
    `1.6-ext`) is developed. Usually gets merged back
    into the `default`-branch once a feature is completed

### Distributed bug-tracking<a id="sec-6-2-3" name="sec-6-2-3"></a>

As an experimental feature distributed bug-tracking was introduced
using the *Artemis*-extension for *Mercurial* (see
<http://hg.mrzv.org/Artemis/>). An up-to-date version can be
installed by

    hg clone http://hg.mrzv.org/Artemis/

somewhere and installing the plugin by editing `.hgrc`.

This is **not** the official bug-tracker for `swak4Foam`. It is used
for keeping track of new features that are to be introduced to
`swak4Foam` and may be discontinued if the experiment proves to be
unsuccessful.

### Maintaining feature and hotfix-branches<a id="sec-6-2-4" name="sec-6-2-4"></a>

The repository comes with a `.hgflow`-file that is set for the
`hgflow`-extension found at
<https://bitbucket.org/yujiewu/hgflow/wiki/Home> (there are multiple
branches of this extension. This **seems** to be the most up-to date
and still under active development)

The two main lines (`1.x` and `2.x`) have slightly different names
for the `master` and the `dvelopment`-branch. But apart from that
in the future this repository will try to stick to the model
described in <http://nvie.com/posts/a-successful-git-branching-model/>

# Copyright<a id="sec-7" name="sec-7"></a>

`swak4Foam` is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.  See the file COPYING in this directory,
for a description of the GNU General Public License terms under which
you can copy the files.

# Known bugs<a id="sec-8" name="sec-8"></a>

The following list is not complete. If the Artemis extension (see
above) is installed then

    hg ilist

gives a more up-to-date list

## Moving meshes and `sampledSurfaces`<a id="sec-8-1" name="sec-8-1"></a>

It seems that with moving meshes `sampledSurfaces` don't get
updated. This seems to be a problem with OpenFOAM itself (the
regular `surfaces`-functionObject doesn't get updated. This is
currently investigated

## Missing support for interpolation and point-Fields<a id="sec-8-2" name="sec-8-2"></a>

Apart from patches and internal fields the support for
interpolation from cells to faces (and vice versa) is incomplete as
well as point fields (although they are supported in the grammar)

## Caching of loaded fields not working<a id="sec-8-3" name="sec-8-3"></a>

This is especially evident for the `funkyDoCalc`-example

## Possible enhancements of the code<a id="sec-8-4" name="sec-8-4"></a>

Not really bugs, but stuff that bugs me

### Pointers in the driver code<a id="sec-8-4-1" name="sec-8-4-1"></a>

This is necessary because of `bison`. Investigate possibilities to
replace these by `tmp` and `autoPtr`

## Possible memory loss<a id="sec-8-5" name="sec-8-5"></a>

`valgrind` reports some lost memory for stuff that is not directly
allocated by `swak4Foam` (in OpenFOAM-sources)

Will investigate. Relevant places are marked by comments in the
code. Also the construction of `sampledSet` seems to loose memory

Currenly problematice parts seem to be:

-   update and `magSf`-calculation of `SampledSurface` (marked in
    `SampledSurfaceExpressionDriver.C`)
-   Python interpreter (but no calls in swak were found in the
    stack-trace that could be responsible)

## Non-treatment of the inner product `&` of symmetric tensors<a id="sec-8-6" name="sec-8-6"></a>

Before OpenFOAM 2.1 the inner product of two symmetric tensors was
a symmetric tensor. Since 2.1 it is a general tensor. As the
general treatment in the grammar would be confusing currently the
this product was removed from the grammar and therefor will not be
correctly parsed

## No point-vector construction for Subsets<a id="sec-8-7" name="sec-8-7"></a>

The same problem that was mentioned in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=130>
is also true for subsets. But as the interpolation is not
implemented for most subsets this will be postponed

## No tab-completion for regular Python-shell and old IPython-versions<a id="sec-8-8" name="sec-8-8"></a>

The tab-completion does not work except for up-to-date versions of
IPython. This seems to be a problem with the `readline`-library
inside an embedded Python. Low priority

## Failure of `groovyBC` for `commsType` `blocking`<a id="sec-8-9" name="sec-8-9"></a>

If the `commsType` is set to `blocking` then the MPI-communications
of `groovyBC` cause problems in MPI and a fatal error.

Currently the only workaround is to edit
`$WM_PROJECT_DIR/etc/controlDict` so that a different `commsType`
is used (`nonBlocking` or `scheduled`)

# History<a id="sec-9" name="sec-9"></a>

## 2010-09-13 - version number : 0.1<a id="sec-9-1" name="sec-9-1"></a>

First Release

## 2010-12-18 - version number : 0.1.1<a id="sec-9-2" name="sec-9-2"></a>

New release
Most important changes

### Parser for `sampledSurfaces`<a id="sec-9-2-1" name="sec-9-2-1"></a>

Now expressions for the field on a sampled surface can be
evaluated. All `sampledSurfaces` offered by OpenFOAM now can be
used

### Multiline `variables`<a id="sec-9-2-2" name="sec-9-2-2"></a>

The `variables` entry (most notably used in `groovyBC` and
`swakExpression`) now can be a list of strings. This allows some
kind of "formatting" (one expression per line) and should improve
readability

### Two maintainance-scripts were added<a id="sec-9-2-3" name="sec-9-2-3"></a>

These can copy the libraries and utilities to the global
installation (for those who think that the swak4Foam-stuff is
stable enough and want to 'bless' all users at their site with
it). Note that any local installation still takes precedence
(because `$FOAM_USER_APPBIN` is before `$FOAM_APPBIN` in the
`$PATH`

### Parsers using 'external variables' are now run-time selectable<a id="sec-9-2-4" name="sec-9-2-4"></a>

This allows the inclusion of other parsers with the regular
`swak4Foam` parsers and include them seamlessly with the
`variables`-mechanism for 'externals' (in other words: you can add
your own parser in a separate library without having to change
anything about the overall `swak4Foam`, but it behaves as if it
was part of it)

## 2011-01-30 - version number : 0.1.2<a id="sec-9-3" name="sec-9-3"></a>

### Support for *Finite Area*-stuff<a id="sec-9-3-1" name="sec-9-3-1"></a>

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

### Bugfix for compiling in single precision<a id="sec-9-3-2" name="sec-9-3-2"></a>

See <https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=49>

### New function `nearDist`<a id="sec-9-3-3" name="sec-9-3-3"></a>

See <https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=44>

## 2011-04-20 - version number : 0.1.3<a id="sec-9-4" name="sec-9-4"></a>

New features and changes are (this list is not complete):

### New utility `funkySetAreaField`<a id="sec-9-4-1" name="sec-9-4-1"></a>

Like `funkySetFields` for *finiteArea*. Also writes out a volume
field for easier post-processing

### Refactoring of `groovyBC` and groovified boundary conditions<a id="sec-9-4-2" name="sec-9-4-2"></a>

Makes it easier to use the `groovyBC`-machinery for other boundary
conditions. Two standard boundary conditions were
*groovified*. Others may follow

### Easier deployment<a id="sec-9-4-3" name="sec-9-4-3"></a>

If no `simpleFunctionObjects` are present they can be downloaded
by a script.
Also scripts to handle global installations of `swak4Foam`

### Force equations<a id="sec-9-4-4" name="sec-9-4-4"></a>

A class to force equations to certain values according to
expressions

### New utility `funkyDoCalc`<a id="sec-9-4-5" name="sec-9-4-5"></a>

Utility does calculations on saved results and outputs single
numbers (min, max, sum, average) to the terminal. Can be used for
reporting or validity checks

### Debian packaging<a id="sec-9-4-6" name="sec-9-4-6"></a>

Crude packaging for Debian

### Lookup-tables<a id="sec-9-4-7" name="sec-9-4-7"></a>

A single-argument function can be specified as a piecewise linear
function. Basically works like timelines but the argument can be
something else (not only the time)

### Stored variables<a id="sec-9-4-8" name="sec-9-4-8"></a>

Variables that store their values between time-steps. Applications
are statistics or switches

### Sampled sets<a id="sec-9-4-9" name="sec-9-4-9"></a>

Sampled sets can now also be used as en entity on which
calculation is possible.

## 2011-07-26 - version number : 0.1.4<a id="sec-9-5" name="sec-9-5"></a>

### Port to OpenFOAM 2.0<a id="sec-9-5-1" name="sec-9-5-1"></a>

This is the first release that officially supports OpenFOAM 2.0

Also it is the first release that incorporates the
`simpleFunctionObjects`-library

### New features:<a id="sec-9-5-2" name="sec-9-5-2"></a>

#### Rewrite of `rand` and `randNormal`<a id="sec-9-5-2-1" name="sec-9-5-2-1"></a>

-   These two functions now can receive an integer seed that
    determines the pseudo-randooom sequence generated by  these functions
-   Two functions `randFixed` and `randNormalFixed` were
    added. While the usual `rand` functions generate a different
    result at every time-steps for these functions the pseudo-random
    sequence is determined **only** by the seed (not by the timestep)

#### Binary `min` and `max`<a id="sec-9-5-2-2" name="sec-9-5-2-2"></a>

Take the bigger/smaller of two fields. Helps avoid `?:`-operations

#### Allow writing of only volume-fields in `funkySetAreaFields`<a id="sec-9-5-2-3" name="sec-9-5-2-3"></a>

Application: If the results of the calculation are only needed in
ParaView

#### Internal changes<a id="sec-9-5-2-4" name="sec-9-5-2-4"></a>

-   Use `autoPtr` for sets
-   Update sets that change in memory or on disc

### Bug-fixes<a id="sec-9-5-3" name="sec-9-5-3"></a>

#### `funkySetAreaFields` did not check for the correct fields<a id="sec-9-5-3-1" name="sec-9-5-3-1"></a>

Fixed by Petr Vita

#### `surfaceProxy` uses the wrong geometric data<a id="sec-9-5-3-2" name="sec-9-5-3-2"></a>

#### Avoid floating point exceptions with division of fields<a id="sec-9-5-3-3" name="sec-9-5-3-3"></a>

Calculated boundaries were \(0\) and caused a division by zero

### Packaging<a id="sec-9-5-4" name="sec-9-5-4"></a>

#### Update Debian packaging<a id="sec-9-5-4-1" name="sec-9-5-4-1"></a>

-   Packaging information for the currently used OF-version is
    generated (allows a separate `swak`-package for every OF-version
-   Submission to launchpad added

#### Deployment scripts<a id="sec-9-5-4-2" name="sec-9-5-4-2"></a>

Now install to `FOAM_SITE_APPBIN/LIBBIN`

## 2011-10-03 - version number : 0.1.5<a id="sec-9-6" name="sec-9-6"></a>

### New features<a id="sec-9-6-1" name="sec-9-6-1"></a>

#### `replayTransientBC` now supports multiple regions<a id="sec-9-6-1-1" name="sec-9-6-1-1"></a>

Uses the usual `-region`-option. Looks for a separate dictionary
in the `system`-directory of that region

#### `replayTransientBC` allows execution of functionObjects<a id="sec-9-6-1-2" name="sec-9-6-1-2"></a>

This can be switched on using the `allowFunctionObjects`-option

#### `Python`-embedding<a id="sec-9-6-1-3" name="sec-9-6-1-3"></a>

Allows the execution of Python-Code in a functionObject

This feature is still experimental and the organization of the
libraries is subject to change

#### Global variables<a id="sec-9-6-1-4" name="sec-9-6-1-4"></a>

It is now possible to define variables that are 'global': They
can be read in any entity.

Currently these variables can only be uniform.

To access global variables the specification-dictionary has to
have a `wordList` named `globalScopes`. The scopes are searched
in that order for the names of global variables. Having scopes
allows some kind of separation of the variables

#### Using OF 2.0 codeStreams<a id="sec-9-6-1-5" name="sec-9-6-1-5"></a>

Adds a functionObject `swakCoded` that extends the
`coded`-functionObject to read and write global variables

#### Simplified boundary condition `groovyBCFixedValue`<a id="sec-9-6-1-6" name="sec-9-6-1-6"></a>

Added a boundary condition than allows to only fix the
values. This should help to avoid problems with cases that don't
like `mixed` (on which the regular `groovyBC` is based)

#### Function objects to solve PDEs<a id="sec-9-6-1-7" name="sec-9-6-1-7"></a>

Two function objects that solve *Partial Differential Equations*
during a run have been added:
-   one that solves a laplacian (Poisson) equation
-   one that solves the transport equation for a scalar

The relevant coefficients (including explicit and implicit source
terms) can be specified using expressions

### Administrative and packaging<a id="sec-9-6-2" name="sec-9-6-2"></a>

#### Inject `swak4Foam` into a distro<a id="sec-9-6-2-1" name="sec-9-6-2-1"></a>

Added a script that takes the current sources, copies them into
the appropriate places of a `OpenFOAM`-installation and slightly
rewrites them to compile in this place. What happens then
(committing them into the repository or just plain compilation)
is up to the maintainer

#### Absorb `simpleFunctionObjects`<a id="sec-9-6-2-2" name="sec-9-6-2-2"></a>

As many parts of `swak4Foam` depend on it the
`simpleFunctionObjects` have now been absorbed into
`swak4Foam`. They can still be compiled on their own

### Bugfixes<a id="sec-9-6-3" name="sec-9-6-3"></a>

#### Variables not working for parallel computations<a id="sec-9-6-3-1" name="sec-9-6-3-1"></a>

If a variable is defined and the patch which it is defined on
doesn't have any faces the variable is reported on that processor
as *not existing* and the whole run fails

## 2012-01-04 - version number : 0.1.6<a id="sec-9-7" name="sec-9-7"></a>

### Cases changed<a id="sec-9-7-1" name="sec-9-7-1"></a>

#### `circulatingSplash`<a id="sec-9-7-1-1" name="sec-9-7-1-1"></a>

Fixed according to a suggestion by Andreas Otto. Now runs again
(used to crash some time-steps into the beginning)

### Infrastructure<a id="sec-9-7-2" name="sec-9-7-2"></a>

#### Check for correct `bison`-version<a id="sec-9-7-2-1" name="sec-9-7-2-1"></a>

The `Allwmake`-script now checks for the correct bison-version
(and the existence of `bison`) and fails if it doesn't seem to be
the right one

#### Supply a header with preprocessor-symbols about the used OF-version<a id="sec-9-7-2-2" name="sec-9-7-2-2"></a>

To allow distinguishing different OF-versions as discussed in the
bug report
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=114>
the `Allwmake`-script now generates a header file
`foamVersion4swak.H` that defines the symbols
`FOAM_VERSION4SWAK_MAJOR`, `FOAM_VERSION4SWAK_MINOR` and
`FOAM_VERSION4SWAK_PATCH`

### Technical<a id="sec-9-7-3" name="sec-9-7-3"></a>

#### Refactoring of the FieldDriver<a id="sec-9-7-3-1" name="sec-9-7-3-1"></a>

-   now can also produce surfaceFields
-   full support of `tensor`, `symmTensor` and `sphericalTensor`

#### Refactoring of the FaFieldDriver<a id="sec-9-7-3-2" name="sec-9-7-3-2"></a>

-   now can also produce edgeFields

No support for tensors yet

#### Writing of `storedVariables`<a id="sec-9-7-3-3" name="sec-9-7-3-3"></a>

If necessary (for instance `swakExpression`-functionObject) the
`storedVariables` are written to disc (into a subdirectory
`swak4Foam` of the timestep) and are reread at the start. This
allows consistent restarts (for instance if a flow was summed
using the variable) **if the expressions stay the same**.

#### `simpleFunctionObjects` now write vectors and tensors without brackets<a id="sec-9-7-3-4" name="sec-9-7-3-4"></a>

Data files can now be written without brackets but each component on
its own. The number of entries in the header is not adjusted

#### A *default mesh* for the drivers exists<a id="sec-9-7-3-5" name="sec-9-7-3-5"></a>

For drivers that don't have access to a `fvMesh` a default mesh
exists. This default mesh is defined by the first `fvMesh` that
is used during the construction of **any** driver.

Definition of the default mesh can be forced using the
`initSwakFunctionObject` (see the test case `flowRateAngledDuct`)

### New features<a id="sec-9-7-4" name="sec-9-7-4"></a>

#### General `phi` in `solveTransportPDE`<a id="sec-9-7-4-1" name="sec-9-7-4-1"></a>

Due to the refactoring of the `FieldDriver` now `phi` can be
specified by a general expression (instead of 'only' a
field-name)

#### `funkySetFields` now also writes surfaceFields<a id="sec-9-7-4-2" name="sec-9-7-4-2"></a>

Can write `surfaceVector` and `surfaceScalar`-Fields. Condition
has to be consistent

#### Function objects now `surfaceField`-aware<a id="sec-9-7-4-3" name="sec-9-7-4-3"></a>

`expressionField` and `manipulateField` now can create or modify
`surfaceFields`

#### `funkySetFields` and function objects support tensors<a id="sec-9-7-4-4" name="sec-9-7-4-4"></a>

`funkySetFields` and the function objects `expressionField` and
`manipulateField`  now also work with the three tensor-types

#### Extension of the `expressionToFace` `topoSet`<a id="sec-9-7-4-5" name="sec-9-7-4-5"></a>

If the expression evaluates to a `surfaceField` then this is used
as a flag whether or not the face is in the `faceSet`. If the
expression evaluates to a `volScalarField` then the old semantic
applies (faces are in the set if one cell is `true` and the other
is `false`).

This only works for internal faces

#### `addGlobalVariable` allows setting more than one value<a id="sec-9-7-4-6" name="sec-9-7-4-6"></a>

If there is an entry `globalVariables` then this dictionary is
used to set the variables

#### Function object `calculateGlobalVariables`<a id="sec-9-7-4-7" name="sec-9-7-4-7"></a>

Calculates variables and then pushes them to a global namespace

#### Generate a dummy `phi` in `replayTransientBC`<a id="sec-9-7-4-8" name="sec-9-7-4-8"></a>

New option added that generates a `phi` field with value \(0\) to
keep boundary conditions like `inletOutlet` happy

#### Function object to dump expression results<a id="sec-9-7-4-9" name="sec-9-7-4-9"></a>

The functionObject `dumpSwakExpression` dumps the complete
results of a swakExpression to file at each timestep. This
produces huge files and is therefor not endorsed

#### Additional options for `funkySetFields`<a id="sec-9-7-4-10" name="sec-9-7-4-10"></a>

Add the options `allowFunctionObjects` and `addDummyPhi` to
execute functionObjects and add a `phi`-field (for fields that
require these)

#### Boundary condition `groovcBCDirection`<a id="sec-9-7-4-11" name="sec-9-7-4-11"></a>

Based on the `directionMixed` boundary condition this allows to
set a boundary condition as a Dirichlet-condition only in certain
directions while in the other directions it is a
gradient-condition

#### Boundary condition `groovyBCJump`<a id="sec-9-7-4-12" name="sec-9-7-4-12"></a>

Boundary condition that imposes a jump in the value on a cyclic
boundary condition pair (based on `jumpCyclic`). Only works for
scalar values

#### `simpleFunctionObjects` write CSV-files<a id="sec-9-7-4-13" name="sec-9-7-4-13"></a>

Setting the option `outputFileMode` to `csv` writes
CSV-files. The option-value `foam` is the default (old style). The
option-value `raw` writes the values delimited by spaces (no
brackets for vectors and tensors)

#### Submeshes automatically read if `searchOnDisc` specified<a id="sec-9-7-4-14" name="sec-9-7-4-14"></a>

If a submesh is not yet in memory and the option `searchOnDisc`
is set, the mesh is automatically read into memory and kept there

#### Conditional `functionObjects`<a id="sec-9-7-4-15" name="sec-9-7-4-15"></a>

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

#### `functionObject` that reads gravitation<a id="sec-9-7-4-16" name="sec-9-7-4-16"></a>

`simpleFunctionObjects` has an additional function object that
reads the direction of gravitation. The purpose is to assist
boundary conditions like `buoyantPressure` that rely on it to
work. Best used together with conditional function objects ("If
`g` is missing &#x2026;.")

#### PDE-functionObjects for `finiteArea`<a id="sec-9-7-4-17" name="sec-9-7-4-17"></a>

Solve transport and laplacian equation

#### Subclass to `DataEntry` that uses *swak*-expressions<a id="sec-9-7-4-18" name="sec-9-7-4-18"></a>

This is defined in the `swak4FoamParsers`-library. The class
needs a default mesh defined to construct the driver. Definition
of the default mesh (if no other driver was constructed in some
function-object or by a `groovyBC`) can be forced using the
`initSwakFunctionObject` (see the test case `flowRateAngledDuct`)

#### `funkySetAreaField` now also writes edgeFields<a id="sec-9-7-4-19" name="sec-9-7-4-19"></a>

Similar to the `surfaceFields` in `funkySetFields`

### Bug fixes<a id="sec-9-7-5" name="sec-9-7-5"></a>

#### Compilation with Intel-Compiler possible<a id="sec-9-7-5-1" name="sec-9-7-5-1"></a>

The `Utilities` failed with the Intel-compiler. Compilation now
falls back to good old `g++`

#### Access to tensor-components not working<a id="sec-9-7-5-2" name="sec-9-7-5-2"></a>

Because the tokens were not defined in the `flex`-files getting
tensor components with `tensor.xx` did not work. Fixed

#### Constants for surfaceFields not working<a id="sec-9-7-5-3" name="sec-9-7-5-3"></a>

Because surfaceFields know no `zeroGradient` the template
`makeConstant` did not work

#### `snGrad` does not work for patches if the file is on disc<a id="sec-9-7-5-4" name="sec-9-7-5-4"></a>

Change so that the field gets temporarily loaded to calculate the
gradient on the patch. Same for `internalField` and
`neighbourField`

#### `potentialFoam` does not correctly use a `groovyBC`<a id="sec-9-7-5-5" name="sec-9-7-5-5"></a>

The reason is that `groovyBC` usually doesn't get evaluated
during construction. The reason is that it is hard to tell
whether all required fields are already in memory. The current
fix is a workaround: setting `evaluateDuringConstruction` to
`true` forces the BC to be evaluated during construction

#### Extra evaluation of boundary conditions causes failure<a id="sec-9-7-5-6" name="sec-9-7-5-6"></a>

Extra evaluation of boundary condition that should fix the
problem with `calculated` patches causes `funkySetFields` to fail
with stock boundary conditions if not all fields are present in
memory

### Discontinued features<a id="sec-9-7-6" name="sec-9-7-6"></a>

#### `groovyFlowRateInletVelocity`<a id="sec-9-7-6-1" name="sec-9-7-6-1"></a>

This boundary condition will be removed in future releases
because the base class now supports the more general
`DataEntry`-class for which a `swak`-subclass exists

## 2012-04-13 - version number : 0.2.0 Friday the 13th<a id="sec-9-8" name="sec-9-8"></a>

### New features<a id="sec-9-8-1" name="sec-9-8-1"></a>

#### Region name in `simpleFunctionObject`.outputs<a id="sec-9-8-1-1" name="sec-9-8-1-1"></a>

To distinguish the output of various instances of
`functionObjects` from the `simpleFunctionObjects`-library in
multi-region cases the screen-output is prefixed with the region
name. For the default region nothing changes. Directory names
stay the same as they are unambiguous anyway (they contain the
name of the functionObject)

#### Temporal operators `ddt` and `oldTime`<a id="sec-9-8-1-2" name="sec-9-8-1-2"></a>

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

#### Boundary condition `groovcBCDirection`<a id="sec-9-8-1-3" name="sec-9-8-1-3"></a>

Based on the `directionMixed` boundary condition this allows to
set a boundary condition as a Dirichlet-condition only in certain
directions while in the other directions it is a
gradient-condition

**Note**: this should have been in the last release but was
 forgotten to merge into the default branch

#### Boundary condition `groovyBCJump`<a id="sec-9-8-1-4" name="sec-9-8-1-4"></a>

Boundary condition that imposes a jump in the value on a cyclic
boundary condition pair (based on `jumpCyclic`). Only works for
scalar values

**Note**: this should have been in the last release but was
 forgotten to merge into the default branch

#### Function to return the position of minimum and maximum<a id="sec-9-8-1-5" name="sec-9-8-1-5"></a>

The functions `minPosition` and `maxPosition` return the position
of the minimum or the maximum of a scalar field

This is implemented for all field types

#### Support for `pointFields` in the field-parsers<a id="sec-9-8-1-6" name="sec-9-8-1-6"></a>

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

#### Support for tensors in the `finiteArea`-field parser<a id="sec-9-8-1-7" name="sec-9-8-1-7"></a>

The `faField`-parser now supports tensors, symmetric tensors and
spherical tensors.

Not all operators are supported because the are not instantiated
in 1.6-ext

#### New convenience-variables for the Python-Integration<a id="sec-9-8-1-8" name="sec-9-8-1-8"></a>

These variables are added in the namespace to ease the writing of
Python-code whose output is consistent with OF
-   **timeName:** Name of the current time as a string. Allows the
    construction of directory names
-   **outputTime:** Boolean that indicates whether this is a timestep
    where OpenFOAM will write output

#### Additional operators from the `fvc`-namespace<a id="sec-9-8-1-9" name="sec-9-8-1-9"></a>

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

### Infrastructure<a id="sec-9-8-2" name="sec-9-8-2"></a>

#### Full parallel compilation<a id="sec-9-8-2-1" name="sec-9-8-2-1"></a>

Thanks to patches supplied by Bruno Santos (see
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=105>)
compilation of the libraries is now possible in parallel

#### Version numbers and version number reporting<a id="sec-9-8-2-2" name="sec-9-8-2-2"></a>

Releases up to now got a version number. Utilities now report the
version number. This should make it easier to find out whether
problems are due to an old version

Still looking for a way to do that for the libraries (so that
they will report it if dynamically loaded)

### Packaging<a id="sec-9-8-3" name="sec-9-8-3"></a>

#### Update of the Debian-Packaging<a id="sec-9-8-3-1" name="sec-9-8-3-1"></a>

`make dpkg` now genrates also a valid package if the current
OpenFOAM-installation is **not** installed using the `dpkg`.

### Changes in the behavior<a id="sec-9-8-4" name="sec-9-8-4"></a>

#### Directory variables in Python-integration<a id="sec-9-8-4-1" name="sec-9-8-4-1"></a>

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

#### User must acknowledge parallelization in Python-integration<a id="sec-9-8-4-2" name="sec-9-8-4-2"></a>

In parallel runs the user must set the `isParallelized` to `true`
if the `parallelMasterOnly` is set to `false`.

With that he indicates that in his opinion the Python-code has no
bad side-effects in parallel runs and that he doesn't blame
swak4Foam if anyting bad happens

### Bug fixes<a id="sec-9-8-5" name="sec-9-8-5"></a>

#### `interFoam`-based example solvers do not compile on 2.1<a id="sec-9-8-5-1" name="sec-9-8-5-1"></a>

As reported in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=119>
due to a change the way the PISO-loop is treated the
`interFoamWithSources` and `interFoamWithFixed` don't compile
with 2.1 anymore.

To avoid `#ifdef` in the solver sources there is now a separate
set of sources (labeled `pre2.1`) for older versions. The regular
sources work with 2.1 (and hopefully the following)

#### `-allowFunctionObjects`-option not working for `replayTransientBC`<a id="sec-9-8-5-2" name="sec-9-8-5-2"></a>

Function-objects only work with the
`while(runTime.loop())`-construct in 2.1. The utility now uses
this.

#### Field itself can not be used in `funkySetBoundaryField`<a id="sec-9-8-5-3" name="sec-9-8-5-3"></a>

Bug reported:
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=124>

An expression like `2*U` did not work for the field `U`. Reason
was that the registry already held an object called `U` (the
dictionary representation of the field) and therefor refused to
load/register another `U`.

Has been fixed by de-registering the dictionary `U` immediately
after loading.

#### No gradient for vectors in FieldParser<a id="sec-9-8-5-4" name="sec-9-8-5-4"></a>

The gradient for a vector field (result: a tensor field) was not
calculated. It is now part of the grammar

#### Some operators for tensors not working in 1.6-ext<a id="sec-9-8-5-5" name="sec-9-8-5-5"></a>

`tr`, `inv` and `det` were not working for some tensort types in
1.6-ext. The parser now fails if such a combination is
used. Works OK for other OF-versions

Also introduced a workaround for certain operators not being
properly defined for pointFields (by using the internal fields)

#### `x`, `y`, `z` and `xy` etc not available as field names<a id="sec-9-8-5-6" name="sec-9-8-5-6"></a>

These symbols could not be used as field names because they were
used for the components of vectors and tensors

Now these names are only used if the `.`-operator asks for a
component. This is implemented for these parsers

-   FieldValues
-   PatchValues
-   SubsetValues (all Zones, sets and samples)
-   finiteArea-Parsers: faPatch and faField

#### Missing tensor components for point-Fields in some parsers<a id="sec-9-8-5-7" name="sec-9-8-5-7"></a>

All parsers except the field-parser were missing the access to
tensor components in the grammar

#### No vector construction possible for point-vectors (and tensors) in PatchField<a id="sec-9-8-5-8" name="sec-9-8-5-8"></a>

As mentioned in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=130>
it was not possible to construct a point-vector field using
`vector(toPoint(1),toPoint(1),toPoint(1))`. Same for tensors

#### Incomprehensible error message in `funkySetFields` if the field is missing<a id="sec-9-8-5-9" name="sec-9-8-5-9"></a>

The error message in `funkySetFields` that was issued when a
field is supposed to be created was not very helpful (something
about the field currently being an `IOobject`)

#### Missing `magSqr` in parsers<a id="sec-9-8-5-10" name="sec-9-8-5-10"></a>

This function was reported missing on the message board

#### Wrong size when composing vectors and tensors for point-fields<a id="sec-9-8-5-11" name="sec-9-8-5-11"></a>

The composed objects got their size from the current parser, not
the components. This resulted in a segmentation-fault for
pointFields

#### `icc` does not compile `executeIfExecutableFitsFunctionObject` on Linux<a id="sec-9-8-5-12" name="sec-9-8-5-12"></a>

Preprocessor symbol `linux` unknown. Replaced with `__linux__`

#### Enhancement to the `trackDictionary`-functionObject<a id="sec-9-8-5-13" name="sec-9-8-5-13"></a>

Now handles bad or non-existent filenames for dictionaries to
track

Fix provided by Martin Beaudoin

## 2012-10-18 - version number : 0.2.1<a id="sec-9-9" name="sec-9-9"></a>

### Requirements<a id="sec-9-9-1" name="sec-9-9-1"></a>

#### `flex 2.5.35`<a id="sec-9-9-1-1" name="sec-9-9-1-1"></a>

This version is needed for the reentrant parsers. `2.5.33` **may**
work but is untested. Version `2.5.4` which is on some old
systems definitely does not work

#### `bison 2.4`<a id="sec-9-9-1-2" name="sec-9-9-1-2"></a>

Version 2.3 compiles but there is an offset-problem with the
locations that breaks the Plugin-functionality

Mac-users will have to install `bison` from another source (for
instance `MacPorts`)

### Bug fixes<a id="sec-9-9-2" name="sec-9-9-2"></a>

#### Make sure that `Allwmake` always uses the `bash`<a id="sec-9-9-2-1" name="sec-9-9-2-1"></a>

On Ubuntu `/bin/sh` is something else and the scripts fail.
Hardcode to `/bin/bash`

#### `downloadSimpleFunctionObjects.sh` still in Makefile<a id="sec-9-9-2-2" name="sec-9-9-2-2"></a>

This script (which was removed long ago) was still referenced in
the `Makefile`.

#### `grad` in fields added dimensions<a id="sec-9-9-2-3" name="sec-9-9-2-3"></a>

`grad` and other operators from `fvc` added dimensions to values
that were supposed to be dimensionless. This has been fixed

#### Default `condition` for surface fields wrong size in `funkySetFields`<a id="sec-9-9-2-4" name="sec-9-9-2-4"></a>

Due to a typo the constructed condition field was too short for
surface-fields (too long for volume-fields, but that didn't matter)

#### `mappedFvPatch` not treated like regular patches<a id="sec-9-9-2-5" name="sec-9-9-2-5"></a>

The field-driver created patch fields there as `calcuated` when
`zeroGradient` would have been more appropriate

#### `flip()` for `faceSet` not correctly calculated<a id="sec-9-9-2-6" name="sec-9-9-2-6"></a>

A `SortableList` was used which meant that the vector with the
flip values was not in the correct order

#### `fset()` crashes when `faceSet` has boundary faces<a id="sec-9-9-2-7" name="sec-9-9-2-7"></a>

This problem was due to a non-special treatment of faces on the
boundary. Fixed (for `faceZone` as well).

Also boundary faces are now honored in `expressionToFace` if the
expression is a surface-field (for the volume-field logic
boundary faces will never work)

#### `groovyBC` produced incorrect results with `wallHeatFlux` etc<a id="sec-9-9-2-8" name="sec-9-9-2-8"></a>

The reason was that during construction `refGradient`, `refValue`
etc were not read correctly (if present).

This is now fixed in `groovyBC` and the other BCs (`groovyBC` for
pointFields and `groovyBCDirection`)

#### Global variables not found in `faField`<a id="sec-9-9-2-9" name="sec-9-9-2-9"></a>

The Lexer correctly identified the variable but the `getField`
method did not know how to get it.

Fixed

#### Wrong type of condition field in `funkySetAreaFields`<a id="sec-9-9-2-10" name="sec-9-9-2-10"></a>

If no condition was evaluated the utility generated a
pseudo-field of the wrong length

#### `calculated`-patches \(0\) for some operations<a id="sec-9-9-2-11" name="sec-9-9-2-11"></a>

For some operations in the `Field`-driver the
`calculated`-patches had the value \(0\) instead of the number of
the next cell. This caused certain calculations to fail (give
\(0\))

The `FaField`-driver did no fixing of the `calculated`-patches at
all.

This is fixed

#### `sqr` of a `vector` should produce a `symmTensor`<a id="sec-9-9-2-12" name="sec-9-9-2-12"></a>

Reported in
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=150>

Added the `sqr` at the right place to the grammars. Also some
other missing tensor operations (`dev` and `symm`).

#### `funkySetFields` produced wrong values on processor patches<a id="sec-9-9-2-13" name="sec-9-9-2-13"></a>

Patch fields were not copied any no `correctBoundaryField` was
called for technical reasons.

Fix: values copied by hand

#### `sortedToc` does not exist for `1.6-ext`<a id="sec-9-9-2-14" name="sec-9-9-2-14"></a>

Introduced a preprocessor symbol that allows using sortedToc on
newer versions

#### Wrong `size()` and `pointSize()` reported for `FaField`-driver<a id="sec-9-9-2-15" name="sec-9-9-2-15"></a>

This was due to a strange () (still don't know what happened
there)

#### Memory leak in the field drivers<a id="sec-9-9-2-16" name="sec-9-9-2-16"></a>

The strings of parsed IDs were not properly deleted. Funnily this
was done correctly in the Patch and the Subset-driver. Also for
timelines-

Also fixed a leak with the labels of plugin-functions that was
present with all drivers

#### Maintenance scripts not working with non-=bash= `/bin/sh`<a id="sec-9-9-2-17" name="sec-9-9-2-17"></a>

Reported by Oliver Krueger that on systems where `/bin/sh` is not
a `bash` anymore (newer Ubuntu and SuSE) the sourcing of
`theFiles.sh` doesn't work anymore.

Fixed and moved all the files to the `maintainanceScripts`-folder

#### `cof` and `diag` undefined<a id="sec-9-9-2-18" name="sec-9-9-2-18"></a>

Added. Diag had to be reprogrammed as it is not implemented for
fields (probably for performance reasons).

Also some tensor operators were missing (probably lost during copy/paste)

#### No new file created if number of patches for patchAverage changes<a id="sec-9-9-2-19" name="sec-9-9-2-19"></a>

Reported in
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=153>

Fixed by removing all file pointers if the number of patches changes

#### `variables` intolerant on spaces<a id="sec-9-9-2-20" name="sec-9-9-2-20"></a>

Spaces in the list of variables made the reading
fail because words can't contain spaces. For instance

    "var =T*2;"

Now all the spaces are removed before assigning to
variables. This will also remove spaces on the "inside" thus
making

    "v ar =T*2;"

the same as the above expression. But it is unlikely that the
call will be used in this way

#### Missing `div`-operations<a id="sec-9-9-2-21" name="sec-9-9-2-21"></a>

These valid `div`-operations were missing from the grammar:
-   Divergence of a volume-tensor (all three kinds) producing a
    vector
-   Divergence of a surface-tensor (all three kinds) producing a
    volume-tensor

#### Fields created by `expressionField` written too late<a id="sec-9-9-2-22" name="sec-9-9-2-22"></a>

Fields created by that function object were written with the
value from the timestep before because the regular write occurs
before the execution of the function objects.

Fixed

#### `storedVariables` did not require a `initialValue`<a id="sec-9-9-2-23" name="sec-9-9-2-23"></a>

Now an initial value is required (instead of the default empty
string which caused parser failure)

#### Dimension checking makes `expressionField` fail<a id="sec-9-9-2-24" name="sec-9-9-2-24"></a>

Reason is that during the calculation of the variables dimensions
are checked.

Now the functionObject switches the checking off. But a more
general solution is desirable

#### `expressionField` at last timestep not written<a id="sec-9-9-2-25" name="sec-9-9-2-25"></a>

The functionObject does not write (and calculate) the field at
the last timestep.

Fixed with an one-liner

#### `groovyBC` makes `interFoam`-runs fail unpredictably<a id="sec-9-9-2-26" name="sec-9-9-2-26"></a>

Reason was an uninitialized `valueFraction` which sometimes has
values that cause a floating point exception. Fixed

#### Global variables of different sizes break parallel runs<a id="sec-9-9-2-27" name="sec-9-9-2-27"></a>

Because `size()` was equal to the expected size on some
processors. Not on all. Now the branch is taken if the size is
equal on **all** processors

#### Fields treated with `readAndUpdateFields` were written one timestep too late<a id="sec-9-9-2-28" name="sec-9-9-2-28"></a>

Fields were not written after the boundary condition was
updated. Now they are

### Enhancements<a id="sec-9-9-3" name="sec-9-9-3"></a>

#### Topology-operators now support `variables` etc<a id="sec-9-9-3-1" name="sec-9-9-3-1"></a>

The topology operators `expressionToCell`, `expressionToFace` and
`expressionToPoint` now support `variables` and the other
supporting keywords if being constructed with a dictionary (for
instance from the `topoSet`-utility)

#### Fields touched by `manipulateField` being written<a id="sec-9-9-3-2" name="sec-9-9-3-2"></a>

Usually the manipulated version of the fields is not written as
the manipulation happens **after** writing. The option
`writeManipulated` enforces writing.

Writing is not the default behavior to avoid side-effects

#### Indicator functions `onPatch` and `internalFace` added to field-expressions<a id="sec-9-9-3-3" name="sec-9-9-3-3"></a>

The function `onPatch(name)` returns a surface-field that is \(1\)
on all faces that belong to patch `name`.

The function `internalFace()` is \(1\) on all internal faces and
\(0\) on all patches

#### Non-uniform second argument for `pow`<a id="sec-9-9-3-4" name="sec-9-9-3-4"></a>

Now the second argument to the `pow`-function can be a
non-constant

#### Added transpose to the tensors<a id="sec-9-9-3-5" name="sec-9-9-3-5"></a>

The expression `A.T()` transposes the tensor `A` (for symmetrical
and spherical tensors it leaves them untouched)

#### Added unit tensor `I` to parsers<a id="sec-9-9-3-6" name="sec-9-9-3-6"></a>

If no field `I` is defined then this is used as the unit-tensor

#### Added the *Hodge dual* operator<a id="sec-9-9-3-7" name="sec-9-9-3-7"></a>

The unary operator `*` calculates for tensors and symmetrical
tensors the hodge dual

#### `replayTransientBC` can now switch on function-objects via dictionary<a id="sec-9-9-3-8" name="sec-9-9-3-8"></a>

The optional entry `useFunctionObjects` switches on the execution
of function objects during the calculation

#### `replayTransientBC` can now create a `phi`-field via dictionary<a id="sec-9-9-3-9" name="sec-9-9-3-9"></a>

The optional entry `addDummyPhi` creates a `phi`-field

#### `expressionField` now allows the specification of a dimension<a id="sec-9-9-3-10" name="sec-9-9-3-10"></a>

The `dimensions`-entry is now read at the same time the variables
are read (this should work for all programs/functionObjects where
the parser is not constructed using a dictionary but the
dictionary is later searched for the `variables`-entry)

### New features<a id="sec-9-9-4" name="sec-9-9-4"></a>

#### Allow dynamically loaded plugins that add new functions to parsers<a id="sec-9-9-4-1" name="sec-9-9-4-1"></a>

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

#### Dynamically generated lists of `functionObjects`<a id="sec-9-9-4-2" name="sec-9-9-4-2"></a>

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

#### Function object `readAndUpdateFields`<a id="sec-9-9-4-3" name="sec-9-9-4-3"></a>

This FO in the `simpleFunctionObjects` reads a number of fields
and updates their boundary conditions at every timestep.

Main purpose is to let `groovyBC` do calculations and use the
results for post-processing purposes

Does not support surface-fields as these don't have a
`correctBoundaryConditions`-method.

Example of the usage in the `angledDuctImplicit`-case (the
results are of limited value because of the temperature boundary
condition)

#### Source terms based on `basicSource`<a id="sec-9-9-4-4" name="sec-9-9-4-4"></a>

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

#### Function objects that stop a run gracefully<a id="sec-9-9-4-5" name="sec-9-9-4-5"></a>

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

#### Function-objects to load thermophysical and turbulence models<a id="sec-9-9-4-6" name="sec-9-9-4-6"></a>

New function objects in the `simpleFunctionObjects` allow the
loading of such models for solvers/utilities that don't have such
models but where some functionObject (for instance) needs such a
model to be in memory

#### Function-objects that create and evolve clouds of lagrangian particles<a id="sec-9-9-4-7" name="sec-9-9-4-7"></a>

Added as part of the `simpleFunctionObjects` some functionObjects
that create a cloud of particles and evolve them at every
timestep.

The appropriate fields needed by every cloud have to be present
(either supplied by the solver or via a `functionObject`)

#### Function-object `manipulatePatchField` to manipulate the field on patches<a id="sec-9-9-4-8" name="sec-9-9-4-8"></a>

This function objects allows the manipulation of patch fields
like `manipulateField` allows the manipulation of the internal
field. Only use if desperate

#### Delayed variables to simulate responses<a id="sec-9-9-4-9" name="sec-9-9-4-9"></a>

If a variable is declared in the `delayedVariables`-list then its
behavior changes: when a value is assigned to that variable then
the value is not immediately used but after the time specified in
`delay`. Values are stored at intervals `storeInterval` and
interpolated linearly. If no stored data is available then the
value of the expression `startupValue` is used.

This feature allows the modeling of boundary conditions that
react with a delay to flow conditions

#### Allow preloading of fields in `funkySetFields`<a id="sec-9-9-4-10" name="sec-9-9-4-10"></a>

To satisfy the requirements of certain boundary conditions
`funkySetFields` now allows the preloading of fields. This is
only available in dictionary mode with the `preloadFields`-entry
(for each entry in the `expressions`-list separately)

### Infrastructure<a id="sec-9-9-5" name="sec-9-9-5"></a>

#### Compilation script checks `SWAK4FOAM_SRC`<a id="sec-9-9-5-1" name="sec-9-9-5-1"></a>

The environment variable `SWAK4FOAM_SRC` is needed for the
`swakCoded`-functionObject. The `Allwmake`-script now checks
whether this variable exists and prints a message with the
correct value if it doesn't. It also checks whether the value is
correct and warns if it isn't

#### `Allwmake` creates symbolic links in `swakFiniteArea`<a id="sec-9-9-5-2" name="sec-9-9-5-2"></a>

The links to `mybison` and `myflex` are missing when the sources
are downloaded as a tarball. The `Allwmake`-script now creates
these links if they are missing

#### Reformatting of the parser sources<a id="sec-9-9-5-3" name="sec-9-9-5-3"></a>

Sources of the parsers have been completely reformatted to make
them more readable and maintainable

#### Move non-parser sources in `swak4FoamParsers` into sub-directories<a id="sec-9-9-5-4" name="sec-9-9-5-4"></a>

Make the directory a little bit cleaner

## 2013-02-28 - version number : 0.2.2<a id="sec-9-10" name="sec-9-10"></a>

### Incompatibilities to previous versions<a id="sec-9-10-1" name="sec-9-10-1"></a>

#### Python-integration library renamed<a id="sec-9-10-1-1" name="sec-9-10-1-1"></a>

To avoid clashes with other similar efforts (not that I know of
any) the library `libpythonIntegration.so` has been renamed to
`libswakPythonIntegration.so`.

Usually only the `libs`-entry in the controlDict has to be
changed (if the library is used at all). Names of the function
objects stay the same

#### Change in write behaviour of `expressionField` and `manipulateField`<a id="sec-9-10-1-2" name="sec-9-10-1-2"></a>

Now the `outputControl`-entry is honored. If set wrong the field
is **no longer** calculated/manipulated at every time-step

### Bug fixes<a id="sec-9-10-2" name="sec-9-10-2"></a>

#### 2.x-branch did not compile on 2.0<a id="sec-9-10-2-1" name="sec-9-10-2-1"></a>

Branch only compiled on 2.1, but not on 2.0 due to changes in the
OpenFOAM-API

Fix provided by Bruno Santos

#### `groovyBC`: `value` reset to \(0\) during construction<a id="sec-9-10-2-2" name="sec-9-10-2-2"></a>

If no `refValue` is given during construction then a value of \(0\)
is assumed for this and during a preliminary update the value is
reset to this value.

Fixed by using the `value` as the `refValue` if no `refValue` is
specified

#### Unspecified `value` in `addGlobalVariables` produces obscure error message<a id="sec-9-10-2-3" name="sec-9-10-2-3"></a>

The error message occured much later when a `tmp` tried to copy a
`NULL`-pointer and was not obvious at all for the general user. Fixed

#### Did not compile on `1.6-ext`<a id="sec-9-10-2-4" name="sec-9-10-2-4"></a>

1.6-ext does not consider the product of two symmetrical tensors
to be symmetrical. Fixed

#### Python integration did not get global variables<a id="sec-9-10-2-5" name="sec-9-10-2-5"></a>

Change in the interface of the `ExpressionResult`-class
broke the access of global variables from a
Python-functionObject. Fixed

#### `maintainanceScripts/makeSwakVersionFile.py` failed when no Mercurial is installed<a id="sec-9-10-2-6" name="sec-9-10-2-6"></a>

This didn't influence the compilation (as there is already a
current version file there) but confused people.

#### Non-uniform variables for `internalField` only worked correctly for volume fields<a id="sec-9-10-2-7" name="sec-9-10-2-7"></a>

Face and point fields did not propagate the information correctly
and were treated as non-uniform volume fields of the wrong
size. This should now be fixed

#### Division of `faceFields` causes division by zero<a id="sec-9-10-2-8" name="sec-9-10-2-8"></a>

The reason was that the patches of the divisor had a value of
zero. This has been fixed by only dividing the
`internalFields()`. Same for `pointFields`.

#### `valuePatches` did not work for mapped patches<a id="sec-9-10-2-9" name="sec-9-10-2-9"></a>

This kind of patch was not identified as something that could
have the value fixed

#### Recent versions of 2.1.x break compilation of `CommonValueExpressionDriver.C`<a id="sec-9-10-2-10" name="sec-9-10-2-10"></a>

The definition of the operator `lessOp` clashed with another
definition. Renamed.

Fix provided by Bruno Santos

#### `expressionField` and `manipulateField` did not honor the `outputControl`-entry<a id="sec-9-10-2-11" name="sec-9-10-2-11"></a>

`expressionFields` were calculated at every time-step, even if
the computational cost was high. Now if `outputControl` is
`outputTime` the field is only calculated if it is going to be
written

#### Double `namespace Foam` makes compilation of `groovyJump` fail with `icc`<a id="sec-9-10-2-12" name="sec-9-10-2-12"></a>

Seems like this one was overlooked because `gcc` is more
tolerant towards stupidity.

Reported by Edo Frederix

### New Examples<a id="sec-9-10-3" name="sec-9-10-3"></a>

#### Cases from the *OSCFD12* Conference in London<a id="sec-9-10-3-1" name="sec-9-10-3-1"></a>

On the slides the case files were promised

### New features<a id="sec-9-10-4" name="sec-9-10-4"></a>

#### Access fields from different cases<a id="sec-9-10-4-1" name="sec-9-10-4-1"></a>

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

#### Patch-expressions now support mapped-patches<a id="sec-9-10-4-2" name="sec-9-10-4-2"></a>

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

#### Function objects that manipulate time<a id="sec-9-10-4-3" name="sec-9-10-4-3"></a>

The framework for function objects that manipulate the `runTime`
were added. The concrete implementations are
-   **setDeltaTByTimeline:** set the timestep from a data file
-   **setDeltaTWithPython:** set the timestep by evaluating a
    Python-snipplet that should return a floating point number
-   **setEndTimeWithPython:** set the end time from the evaluation of
    a Python-snipplet

#### Plugin-functions for chemical model<a id="sec-9-10-4-4" name="sec-9-10-4-4"></a>

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

#### Plugin-functions for radiation model<a id="sec-9-10-4-5" name="sec-9-10-4-5"></a>

The library `libswakRadiationModelFunctionPlugin` adds the
possibility to calculate properties of the radiation like
parts of the source terms `Rp` and `Ru` and the current
radiation source

If there is no radiation model in memory it will be loaded and
the reaction rates are calculated. Assumes the presence of a
temperature field called `T`.

### Enhancements<a id="sec-9-10-5" name="sec-9-10-5"></a>

#### Python-Integration now supports numpy-arrays<a id="sec-9-10-5-1" name="sec-9-10-5-1"></a>

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

#### Optional parameter `writeCode=/=writeFile` for `pythonIntegrationFunctionObject`<a id="sec-9-10-5-2" name="sec-9-10-5-2"></a>

If this is set then this Python-code is executed every time a
time-step was written to disk

#### Python-integration has convenience-functions to create data files<a id="sec-9-10-5-3" name="sec-9-10-5-3"></a>

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

#### Switch off dimension checking in `funkyDoCalc`<a id="sec-9-10-5-4" name="sec-9-10-5-4"></a>

A command lione switch `-noDimensionChecking` is added that
allows switching off the checking of dimensions if this makes
expressions fail

#### `funkySetFields` allows accessing fields from another mesh<a id="sec-9-10-5-5" name="sec-9-10-5-5"></a>

If another case (with time and region) is specified via command
line options (no dictionary) then in the expressions the other
mesh can be accessed under the name `other`

Main application for this is comparing to other cases (but also
comparing different timesteps of the same case with each other).

Used time is either fixed or set to the currently used (this can
be selected via the command line)

#### `funkyDoCalc` allows specifying foreign meshes<a id="sec-9-10-5-6" name="sec-9-10-5-6"></a>

Each expression can specify foreign meshes with a
`foreignMeshes`-subdict

#### Function objects to load and update foreign meshes<a id="sec-9-10-5-7" name="sec-9-10-5-7"></a>

There are two new function objects:
-   **addForeignMeshes:** this allows specifying new foreign
    meshes. These can be used in subsequent function-objects or
    other swak-expressions on `internalFields`
-   **foreignMeshesFollowTime:** this allows specifying a list of
    meshes whose time is set to the current time of the running
    case

#### `replaxTransientBC` outputs timestep<a id="sec-9-10-5-8" name="sec-9-10-5-8"></a>

This helps debugging timestepping-strategies

#### New accumulation `weightedAverage`<a id="sec-9-10-5-9" name="sec-9-10-5-9"></a>

A new possible entry for the `accumulations` calculates an
average that is weighted by a facetor that depends on the entity:
-   cell volume if applicable
-   face area for surfaces etc
-   \(1\) if there is no appropriate weight (usually values defined
    on points). The result is equivalent to `average`.

#### `expressionField` reports name and type of the created field<a id="sec-9-10-5-10" name="sec-9-10-5-10"></a>

The title says it all. Should give the user additional feedback
about what is happening

#### New boundary condition `groovyPartialSlip` in `groovyStandardBCs`<a id="sec-9-10-5-11" name="sec-9-10-5-11"></a>

This boundary allows specifying the partial slip fraction of the
regular `partialSlip`-BC with an expression.

#### `funkySetFields` allows switching off the correction of BCs<a id="sec-9-10-5-12" name="sec-9-10-5-12"></a>

This is sometimes necessary to allow calculations with fields
where the boundary conditions have a lot of dependencies
(turbulence model, other mesh etc)

#### Allow selection of additional regions in `funkySetFields`<a id="sec-9-10-5-13" name="sec-9-10-5-13"></a>

The option `-additionalRegions` allows selecting additional mesh
regions for boundary conditions that require it (usually used
together with `-preloadFields`).

For the other case there is an additional option
`-otherAdditionalRegions`.

#### Adding a `-preloadFields`-option to `funkySetFields`<a id="sec-9-10-5-14" name="sec-9-10-5-14"></a>

This option allows specifying additional field names to be
preloaded in command line mode. If multiple mesh regions are
specified (including another mesh)it will try to preload the
field there too

#### `replayTransientBC` now supports all volume types<a id="sec-9-10-5-15" name="sec-9-10-5-15"></a>

Support for tensor-types has been added

#### `replayTransientBC` preloads fields<a id="sec-9-10-5-16" name="sec-9-10-5-16"></a>

For boundary conditions that require other fields these can be
loaded into memory

#### `replayTransientBC` allows selection of time-steps<a id="sec-9-10-5-17" name="sec-9-10-5-17"></a>

The utility now allows the selection of timesteps. In that case
it acts fundamentally different: instead of doing a timeloop it
jumps to those times, calculates the boundary conditions and
writes. The purpose of this mode is generating boundary fields
for postprocessing.

### Infrastructure<a id="sec-9-10-6" name="sec-9-10-6"></a>

#### `Allwmake` stops if building of `swak4FoamParsers` fails<a id="sec-9-10-6-1" name="sec-9-10-6-1"></a>

As the majority of installation problems were actually problems
with the compilation of the library `swak4FoamParsers` and as
this library is central for all other things in `swak4Foam` the
compilation terminates if this library is not built

#### Header `swak.H` with version information and macros<a id="sec-9-10-6-2" name="sec-9-10-6-2"></a>

This new header includes information about the
`swak4Foam`-version, the `OpenFOAM`-version and macros for
workarounds that depend on the OpenFOAM-version

#### `accumulations`-entries converted to `NamedEnums`<a id="sec-9-10-6-3" name="sec-9-10-6-3"></a>

This forces the entries to be checked while being read. Allowing
more consistent treatment

#### Additional maintenance script for special python-versions<a id="sec-9-10-6-4" name="sec-9-10-6-4"></a>

This script allows compiling additional versions of
`swakPythonIntegration` for cases where the version on the
development machine is different from the version on the
simulation machine (see installation chapter)

### Cosmetics<a id="sec-9-10-7" name="sec-9-10-7"></a>

Changes without consequences for the user

#### Change type of `name` in parsers to `word`<a id="sec-9-10-7-1" name="sec-9-10-7-1"></a>

The valid characters for an identifier were the same as in `word`
(was `string` before)

#### Updated contributor lists in `README` and sources<a id="sec-9-10-7-2" name="sec-9-10-7-2"></a>

The list of contributors to each file have been added (mostly by
script with the information from the repository)

#### Rewritten methods that return pointers<a id="sec-9-10-7-3" name="sec-9-10-7-3"></a>

This abomination from the early days of `funkySetFields` has been
cleaned

### Documentation<a id="sec-9-10-8" name="sec-9-10-8"></a>

Changes to the documentation

#### Start of the reference-manual<a id="sec-9-10-8-1" name="sec-9-10-8-1"></a>

A first version of a reference manual is added. Only the first
part about expressions and parameters is written.

## 2013-03-18 - version number : 0.2.3<a id="sec-9-11" name="sec-9-11"></a>

### Supports OpenFOAM 2.2<a id="sec-9-11-1" name="sec-9-11-1"></a>

This is the first version to compile with OpenFOAM 2.2

Due to changes in OpenFOAM it requires several `#ifdef` (something
that is usually avoided in the OpenFOAM-world) and other
prepocessor definitions)

### Incompatibilities to previous versions<a id="sec-9-11-2" name="sec-9-11-2"></a>

#### `simpleFunctionObjects` and `simpleLagrangianFunctionObjects` no longer independent from rest<a id="sec-9-11-2-1" name="sec-9-11-2-1"></a>

Due to incompatibilities between OpenFOAM 2.2 and previous
versions there are compatibility headers included from the rest
of swak4Foam.

Theoretically both libraries can be easily made independent again.

### Bug fixes<a id="sec-9-11-3" name="sec-9-11-3"></a>

#### Compiles on `1.6-ext` again<a id="sec-9-11-3-1" name="sec-9-11-3-1"></a>

The last release (0.2.2) did not compile on `1.6-ext`. This is
fixed

#### Missing field files for the OSCFD2012-cases<a id="sec-9-11-3-2" name="sec-9-11-3-2"></a>

Due to a stupid `.hgignore` the `0.orig`-directories were
missing. Nobody complained though

#### Did not compile on `2.0.x`<a id="sec-9-11-3-3" name="sec-9-11-3-3"></a>

This has been fixed

### New features<a id="sec-9-11-4" name="sec-9-11-4"></a>

#### Writing of sampled surfaces and sampled sets<a id="sec-9-11-4-1" name="sec-9-11-4-1"></a>

With the options `autoWriteSurface` and `autoWriteSets` sampled
surfaces and sets are automatically written at each write
time. With `writeSurfaceOnConstruction` and
`writeSetOnConstruction` both are written when they are
created. A format has to be specified. No values are written.

### Enhancements<a id="sec-9-11-5" name="sec-9-11-5"></a>

#### Parsers now support `eigenValues` and `eigenVectors`<a id="sec-9-11-5-1" name="sec-9-11-5-1"></a>

These function and some other minor ones (`sph`, `dev2`,
`twoSymm`) are now supported by the parsers

#### Extract rows from tensors with component operator<a id="sec-9-11-5-2" name="sec-9-11-5-2"></a>

The component operators `.x`, `.y` and `.z` now extract the rows
from a tensor

#### Variable names that are identical to fields trigger a Warning<a id="sec-9-11-5-3" name="sec-9-11-5-3"></a>

As this situation (like a variable named `p` for the position
that shadows the pressure field `p`) leads to weird errors a
warning is given. The warning can be switched of with the option
`variableNameIdenticalToField`.

## 2013-05-24 - version number : 0.2.4<a id="sec-9-12" name="sec-9-12"></a>

### Incompatibilities to previous versions<a id="sec-9-12-1" name="sec-9-12-1"></a>

#### Data directories now generated in folder `postProcessing` instead of case folder<a id="sec-9-12-1-1" name="sec-9-12-1-1"></a>

According to the convention introduced in OpenFOAM 2.2 now the
data directories generated by functionObjects based on
`simpleFunctionObjects` (that is almost all) and
`swakPythonIntegration` (if the Python-code uses the
`dataFile`-function) are generated as sub-directories of
`postProcessing` in the case directory.

Scripts that rely on the location of these files will have to be adapted

#### `ExpressionResult` from old versions are not correctly read<a id="sec-9-12-1-2" name="sec-9-12-1-2"></a>

This is due to a fix that now allows reading/writing these in
binary mode. Only occurs if reading old data that either
-   uses a delayed variable
-   has global variables written in timesteps

#### Adaption of cases to 2.2<a id="sec-9-12-1-3" name="sec-9-12-1-3"></a>

This may break them for previous versions of OpenFOAM

### Infrastructure<a id="sec-9-12-2" name="sec-9-12-2"></a>

#### Additional flags for the compilation of the parsers<a id="sec-9-12-2-1" name="sec-9-12-2-1"></a>

The C-files generated by `bison` become very big and some
compilers need quite long when aggressive optimization is
used. An additional environment variable
`SWAK_COMPILE_GRAMMAR_OPTION` allows the specification of
additional compiler switches (like `-O1`) that will appear
**after** the regular switches and should alter the behavior of
the compiler. Set in `swakConfiguration` if you want to set this
permanently

#### Use `bash` instead of `sh` in maintainance scripts<a id="sec-9-12-2-2" name="sec-9-12-2-2"></a>

Some distributions don't use good old `bash` as the default
shell. This breaks some scripts in subtle and not so subtle ways

#### Build-script prints swak-version in the beginning<a id="sec-9-12-2-3" name="sec-9-12-2-3"></a>

To filter bug-reports with outdated versions

### Cosmetics<a id="sec-9-12-3" name="sec-9-12-3"></a>

Changes without consequences for the user

#### Methods use `word` for names instead of `string`<a id="sec-9-12-3-1" name="sec-9-12-3-1"></a>

Methods that look for fields now use the appropriate data type

### Bug fixes<a id="sec-9-12-4" name="sec-9-12-4"></a>

#### Compilation on Fujitsu (and probably Intel) compilers<a id="sec-9-12-4-1" name="sec-9-12-4-1"></a>

There were template specializations for `pTraits` in
`ExpressionResult.C` that had to be moved to `ExpressionResult.H`
to allow compilation on these non-=gcc=-compilers

#### `executeIf`-function objects are executed if `readDuringConstruction` is set to `true`<a id="sec-9-12-4-2" name="sec-9-12-4-2"></a>

Even if the condition evaluates to `false` the function objects
in the list are executed. This makes the run fail if the purpose
of the condition was to guard from such a situation. Fixed

#### Expression results are not correctly read from binary file<a id="sec-9-12-4-3" name="sec-9-12-4-3"></a>

This was fixed by writing the fields properly with
`writeValue`. This may introduce a little incompatibility when
reading files written by old versions

#### Generated fields like `vol()` were registered<a id="sec-9-12-4-4" name="sec-9-12-4-4"></a>

These fields were registered with the mesh which might have
disturbed similar named *real* fields. Also did the fields
generated for variables get registered

#### Missing files in `potentialPitzDaily`-case<a id="sec-9-12-4-5" name="sec-9-12-4-5"></a>

Missing initial conditions made this case a bit pointless

#### `log(vol())` gives floating point exception<a id="sec-9-12-4-6" name="sec-9-12-4-6"></a>

This was due to the boundaries being set to 0. Fixed. Also for
the `dist()` function

### New features<a id="sec-9-12-5" name="sec-9-12-5"></a>

#### Library with additional `searchableSurfaces`<a id="sec-9-12-5-1" name="sec-9-12-5-1"></a>

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

#### New variable type `StackedExpressionResult`<a id="sec-9-12-5-2" name="sec-9-12-5-2"></a>

This type is used to collect multiple values. If it is assigned
a value then the **uniform** value of the right hand side is
appended to the variable. Useful for collecting multiple values
for output.

Variant `StoredStackExpresionResult` keeps the values between
timesteps and allows the collection of timeline data

#### Function object `dumpGlobalVariable`<a id="sec-9-12-5-3" name="sec-9-12-5-3"></a>

Dumps the value of a global variable into a line of a file

#### Function object `removeGlobalVariable`<a id="sec-9-12-5-4" name="sec-9-12-5-4"></a>

Removes a global variable (mainly used to preserve memory)

#### Optional entry `aliases` for parser specifications<a id="sec-9-12-5-5" name="sec-9-12-5-5"></a>

This dictionary allows the specification of alias names and
their real names. If an alias name is found in an expression
then it is replaced by the real name when looking for
fields. The application for this is that OpenFOAM allows
characters in field names that are not valid in swak-names and
would break the grammar if they were (for instance `-` or
`:`). By setting an alias such fields can still be accessed.

#### Function object to dump registered objects<a id="sec-9-12-5-6" name="sec-9-12-5-6"></a>

The function object `listRegisteredObjects` in the
`simpleFunctionObjects` lists all the objects currently
registered with the mesh. Mainly used for debugging ("Which
fields are currently available?")

#### Function object that only executes its children if the run is serial/parallel<a id="sec-9-12-5-7" name="sec-9-12-5-7"></a>

The new functionObject `executeIfParallelSerial` allows
specifying if a list of function objects should only be run if
the simulation is running in serial or parallel.

#### Function objects to calculate distributions<a id="sec-9-12-5-8" name="sec-9-12-5-8"></a>

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

### Enhancements<a id="sec-9-12-6" name="sec-9-12-6"></a>

#### Global variables now can be subclasses of `ExpressionResult`<a id="sec-9-12-6-1" name="sec-9-12-6-1"></a>

This allows other variable types like stored variables to be
inserted there. They are also now saved for restarting

#### `endTime` now added to Python-wrapper<a id="sec-9-12-6-2" name="sec-9-12-6-2"></a>

All classes using the python interpreter now have a variable
`endTime` with the current end-time value

#### Force write if `endTime` set by time-manipulation is reached<a id="sec-9-12-6-3" name="sec-9-12-6-3"></a>

If one of the functionObjects for time-manipulation set the
end-time and it is reached and this is no time for output a
complete output of the fields is forced

#### Function-objects based on `patch` from the `simpleFunctionObjects` now accept regular expression<a id="sec-9-12-6-4" name="sec-9-12-6-4"></a>

This includes the `patchExpression`-functionObject. The supplied
list is now interpreted as a list of regular expressions and all
patches that match are included and processed

#### More flexible output control for functionObjects based on `simpleFunctionObjects`<a id="sec-9-12-6-5" name="sec-9-12-6-5"></a>

An optional parameter `outputControlMode` now allows control on
when the function object will be executed. Possible values are:
-   **timestep:** Every `outputInterval` timesteps the object will
    be executed. This is the default behaviour with an
    interval \(1\)
-   **deltaT:** Approximately every `outputDeltaT` times it will be
    executed. The nearest possible timestep is used
-   **outputTime:** Executed whenever a regular output is scheduled

#### `swakDataEntry` improved<a id="sec-9-12-6-6" name="sec-9-12-6-6"></a>

Two enhancements
-   the name of the independent variable  no can be
    specified. This variable holds the value that is passed to the
    data entry as a uniform value
-   data entry can now be integrated. This allows using it for
    instance for the injection rate in lagrangian models

## 2014-01-24 - version number : 0.3.0<a id="sec-9-13" name="sec-9-13"></a>

The reason for the jump in the minor revision number is that with
the introduction of the parser for lagrangian particles (`cloud`)
the last white spot as far as major data structures in OpenFOAM is
"explored"

### Incompatibilities to previous versions<a id="sec-9-13-1" name="sec-9-13-1"></a>

#### Support of *old* `1.6-ext` lost<a id="sec-9-13-1-1" name="sec-9-13-1-1"></a>

Due to certain differences this version only compiles with the
`nextRelease`-branch of 1.6-ext (from the `git`).Usually the
failing parts can be fixed b commenting out the appropriate
`#define` in `Libraries/swak4FoamParsers/include/swak.H`.

### New supported versions<a id="sec-9-13-2" name="sec-9-13-2"></a>

#### Added support for Foam 3.0<a id="sec-9-13-2-1" name="sec-9-13-2-1"></a>

As this version is quite similar to 1.7 a lot of `#ifdefs` were
introduced by this port leading to a possible unification with
the 1.x-branch

### Infrastructure<a id="sec-9-13-3" name="sec-9-13-3"></a>

#### Make error messages in `Allwmake` more verbose<a id="sec-9-13-3-1" name="sec-9-13-3-1"></a>

The error messages where already quite verbose. But people
didn't understand them and asked the same questions over and
over &#x2026;

#### `simpleFunctionObjects` no longer considered an independent project<a id="sec-9-13-3-2" name="sec-9-13-3-2"></a>

As there are going to be more cross-dependencies the
`simpleFunctionObjects` now have to be part of swak. Changes to
the compile-scripts reflect this.

#### `Allwmake` makes sure that swak is compiled for the same installation<a id="sec-9-13-3-3" name="sec-9-13-3-3"></a>

The script writes the version it is used with to disk and at
later compiles that this is the same (this makes sure that not a
wrong version is used inadvertently to compile)

#### Additional macros for Debugging output<a id="sec-9-13-3-4" name="sec-9-13-3-4"></a>

There are two macros defined that inside an object write the name
of the class and the address of the object (if debug is
enabled). This makes the output easier distinguishable from the
output from other classes/objects. The two macros are:
-   **Dbug:** Writes the debug information **only** on the master processor
-   **Pbug:** Writes the output on all processors and for parallel
    runs prefixes the processor number

Both macros are to be used like regular streams and don't have
to be enclosed in `if(debug){}` (this is part of the macro)

#### Separate option for compiling the output of `flex`<a id="sec-9-13-3-5" name="sec-9-13-3-5"></a>

With some compilers (Intel) and aggressive optimization the
lexers (whose code is generated by `flex`) have uninitialized
memory and segmentation faults. In these cases now an environment
variable `SWAK_COMPILE_LEXER_OPTION` to set lower optimization
for these compilers. For example:

    export SWAK_COMPILE_LEXER_OPTION=-O0

After that recompile

#### Fixing race condition with big `WM_NCOMPPROCS`<a id="sec-9-13-3-6" name="sec-9-13-3-6"></a>

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

#### Updates to Debian-packaging<a id="sec-9-13-3-7" name="sec-9-13-3-7"></a>

Several minor additions that should make packaging easier
-   Package version now gets the correct distro name (at least on Ubuntu)
-   Packaging picks up the correct python-version (2.7 or 2.6
    &#x2026; does anyone use older?)
-   Packaging process leaves the `swakVersion.H` alone

### Documentation<a id="sec-9-13-4" name="sec-9-13-4"></a>

Important enhancements of the documentation in the
`Documentations`-folder

#### Documentation of `accumulations`<a id="sec-9-13-4-1" name="sec-9-13-4-1"></a>

The possible values of the common `accumulations`-lists are
documented

#### General documentation of the Python-embedding<a id="sec-9-13-4-2" name="sec-9-13-4-2"></a>

The general options and the behavior of the Python-embedding are
described

### Incompatibilities to previous versions<a id="sec-9-13-5" name="sec-9-13-5"></a>

#### `outputControlMode` `timestep` renamed to `timeStep`<a id="sec-9-13-5-1" name="sec-9-13-5-1"></a>

Because of report
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=177>
to be consistent with the nomenclature in the 'regular'
function-objects.

### Bug fixes<a id="sec-9-13-6" name="sec-9-13-6"></a>

#### Missing `timeSet` in function-objects<a id="sec-9-13-6-1" name="sec-9-13-6-1"></a>

This method has been added in 2.2.x and breaks the compilation
of several function-objects

Fix developed by Bruno Santos

#### `sourceImplicit` unstable<a id="sec-9-13-6-2" name="sec-9-13-6-2"></a>

For some reason using `SuSp` gave unstable results for the
PDE-functionObjects. Changed to `Sp`

#### Fixed bug were only one `swakCoded` worked at a time<a id="sec-9-13-6-3" name="sec-9-13-6-3"></a>

Bug originally reported
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=179>
in. Reason was that the variable describing the type was not
correctly set.

#### Incorrectly read entries in `swakCoded`<a id="sec-9-13-6-4" name="sec-9-13-6-4"></a>

The entries `codeEnd` and `codeExecute` were not correctly read
but instead the entry `codeRead` was read. Fixed

#### No logical variables found by most parsers<a id="sec-9-13-6-5" name="sec-9-13-6-5"></a>

Reported as
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=172>

Only the field parser correctly found logical variables (although
they were stored). Added a method to pick up the variables (the
reason why this part was commented out is that there is no such
thing as a `volBoolField` and that is what the regular mechanism
expected)

#### `sampledSurface` not correctly updated at time of write<a id="sec-9-13-6-6" name="sec-9-13-6-6"></a>

Reported as
<https://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=176>

In that case the driver was written before the surface was
updated thus generating fields of size \(0\). Now `update` is
called at various places (to make sure it is called in any instance)

#### `sumMag`-accumulation now working<a id="sec-9-13-6-7" name="sec-9-13-6-7"></a>

This accumulation was available but not implemented. Now implemented.

For non-scalar types it is calculated separately for each component

#### Calculation of weight fields failed if size on one processor was \(0\)<a id="sec-9-13-6-8" name="sec-9-13-6-8"></a>

This was due to a logical error that was propagated through
mindless copy/paste (only the Field-driver got it right). Fixed

#### `groovyTotalPressure` does not read `value`<a id="sec-9-13-6-9" name="sec-9-13-6-9"></a>

Because it is not initialized from the superclass when the
dictionary constructor is used. Fixed

#### For multiple times the option `addDummyPhi` makes `funkySetFields` crash<a id="sec-9-13-6-10" name="sec-9-13-6-10"></a>

Because the pointer is already set. Fixed

#### `aliases` not constructed from dictionary<a id="sec-9-13-6-11" name="sec-9-13-6-11"></a>

If the dictionary was read after the construction the aliases
are not read. Fixed by moving this reading to the tables reading
which is used in every time a dictionary is involved

#### Gravity not correctly passed in `evolveXXCloud`<a id="sec-9-13-6-12" name="sec-9-13-6-12"></a>

Passed a value where a reference would have been needed. Fixed

#### `writeOften` writes all the time<a id="sec-9-13-6-13" name="sec-9-13-6-13"></a>

Reason for this was a change of the interface of `outputTime` not
being propagated to this function-object. Fixed

#### Python-integration does not return single scalars as uniform<a id="sec-9-13-6-14" name="sec-9-13-6-14"></a>

The Python-integration returned single scalars (and vectors) not
as a single value but as a field of length \(1\). This caused
warnings that messed up the output. Fixed

#### `basicSource` working again for OF 2.1<a id="sec-9-13-6-15" name="sec-9-13-6-15"></a>

These were not compiled by accident. Fixed

#### `faceZone` and `faceSet` do not correctly work on parallel runs<a id="sec-9-13-6-16" name="sec-9-13-6-16"></a>

As reported in
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=190>
if a face-set or zone has faces on a processor boundary incorrect
values were produced.

The reason was that boundary faces are treated like internal
faces. Also do these faces exist on both processors. This is now
fixed. If the face is on a regular boundary patch the value is
used. If on a processor patch the value is only used if it is on
the owner processor.

#### Allow user to override `notImplemented` in `movePoints` and `moveMesh` of function objects<a id="sec-9-13-6-17" name="sec-9-13-6-17"></a>

This addresses
<http://sourceforge.net/apps/mantisbt/openfoam-extend/view.php?id=190>

Subclasses of `simpleFunction`-objects are not guaranteed to be
parallel-aware (for instance `swakExpression` can have problems
with stored variables if the number of cells/faces changes) so
the `notImplemented` can't be removed for good.

The workaround is a rather verbose error message and the
possibility to override `notImplemented` with a dictionary-switch

### New features<a id="sec-9-13-7" name="sec-9-13-7"></a>

#### Function object that executes if the OpenFOAM-version is right<a id="sec-9-13-7-1" name="sec-9-13-7-1"></a>

The functionObject `executeIfOpenFOAMVersionBiggerEqual` only
executes if the OpenFOAM-version is bigger or equal to a
specified version. The arguments `majorVersion` and
`minorVersion` are required. If `patchVersion` is not specified
it will match any version. A `git` version (`.x`) will match any
patch-version

#### Function object that is only executed if the current time is the `startTime`<a id="sec-9-13-7-2" name="sec-9-13-7-2"></a>

The function object `executeIfStartTime` executes its children
if the current time is the `startTime` from the `controlDict`
(which isn't necessarily the first time step in a restarted case)

#### Function object that calculates the potential flow<a id="sec-9-13-7-3" name="sec-9-13-7-3"></a>

To be used on an existing field to set reasonable initial
conditions

#### Function object that forces a recalculation of `phi`<a id="sec-9-13-7-4" name="sec-9-13-7-4"></a>

Named `recalcPhi` in the simpleFunctionObjects. Usually needed
when some other functionObject manipulates velocity or density

#### Function object that corrects thermophysical model<a id="sec-9-13-7-5" name="sec-9-13-7-5"></a>

Name `correctThermo`. Recalculates the thermophysical
model. To be used after manipulating temperature etc

#### Function objects that recalculate the energy of a thermophysical model<a id="sec-9-13-7-6" name="sec-9-13-7-6"></a>

Recalculate enthalpy, sensible enthalpy or internal energy
according to the current temperature. This allows making this
consistent if the temperature has been manipulated.

For OpenFOAM after 2.2 these are replaced by one that
recalculates the energy or enthalpy

#### Function object that calculates the average of one variable as a function of another<a id="sec-9-13-7-7" name="sec-9-13-7-7"></a>

The function object `swakExpressionAverageDistribution`
calculates the average of one function (the `expression`) as a
distribution over another (the `abscissa`). An example would be
the average pressure in x-direction with `abscissa` being
`pos().x`, `expression` being just `p` and the `weight`
being `vol()`. The weight has to be a scalar. All other
expressions can be any data-type

#### New utility `fieldReport`<a id="sec-9-13-7-8" name="sec-9-13-7-8"></a>

This utility prints some quantitative statistics about a
specified field. Optionally these statistics can also be printed
for patches, sets and zones. The data can be written to a
CSV-file. Also the distributions of the field can be written.

#### New utility `funkyPythonPostproc`<a id="sec-9-13-7-9" name="sec-9-13-7-9"></a>

This utility needs a dictionary and a specification of
times. For each time it
-   loads a list of fields specified in the dictionary
-   executes a list of function objects specified in the dictionary
-   executes python-scripts

The idea is that the function objects pass data to the
python-scripts via global variables and the python-scripts do
whatever they like

#### New utility `funkySetLagrangianParticle`<a id="sec-9-13-7-10" name="sec-9-13-7-10"></a>

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

### Enhancements<a id="sec-9-13-8" name="sec-9-13-8"></a>

#### Additional parser for lagrangian particles<a id="sec-9-13-8-1" name="sec-9-13-8-1"></a>

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

#### Function objects that calculate PDEs can use fields from memory<a id="sec-9-13-8-2" name="sec-9-13-8-2"></a>

If the field is found in memory this is used instead of a file
read from disc.

#### Function objects that calculate PDEs can write fields before solving<a id="sec-9-13-8-3" name="sec-9-13-8-3"></a>

An additional switch `writeBeforeAfter` allows writing the field
before and after solving

#### simpleFunctionObjects have addition `outputControlMode`<a id="sec-9-13-8-4" name="sec-9-13-8-4"></a>

Mode `startup` executes the FO only during construction

#### Conditional functionObjects now have optional `else`<a id="sec-9-13-8-5" name="sec-9-13-8-5"></a>

It is now possible to add a sub-dictionary `else` that is used
to initialize a `functionObjectProxy` that is executed if the
condition is **not** fulfilled. The sub-dictionary inherits all
settings that are not set from the parent-dictionary

#### `swakCoded` now allows addition of data to functionObject<a id="sec-9-13-8-6" name="sec-9-13-8-6"></a>

The entry `codeData` is now read and inserted into the functionObject

#### Parsers in `swakFiniteArea` no also have complete tensor-operations<a id="sec-9-13-8-7" name="sec-9-13-8-7"></a>

The two parsers in that library now also support the complete
set of tensor operations (like `eigenValues` etc)

#### `swakExpressionDistribution` now allows non-scalar weights<a id="sec-9-13-8-8" name="sec-9-13-8-8"></a>

For expressions whose results is not a scalar now the weight
function can either be a scalar or of the same type as the
expression (so every component can have a separate weight)

#### More options for `accumulations`<a id="sec-9-13-8-9" name="sec-9-13-8-9"></a>

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

#### Python code files are now searched more flexible<a id="sec-9-13-8-10" name="sec-9-13-8-10"></a>

If a file specified with an option like `startFile` in a
Python-functionObject (or similar) is not found in the current
directory the path of the dictionary it is specified in is
prepended and the file is searched there

#### Python integration now uses `IPython` if possible<a id="sec-9-13-8-11" name="sec-9-13-8-11"></a>

The interactive shell of the python integration now uses
`IPython` if it is installed. This improves tab-completion etc

#### Preload libraries in the Python integration<a id="sec-9-13-8-12" name="sec-9-13-8-12"></a>

As problematic libraries could hand during importing these
libraries can be imported in a safe way using the optional
`importLibs`-entry which is a dictionary. The keys are the names
under which the imports will appear in the Python-namespace. The
value is optional and the name of the actual library

#### Added standard function `weight()`<a id="sec-9-13-8-13" name="sec-9-13-8-13"></a>

All parsers now implement a function `weight()` that returns the
"natural" weight that is used for instance in the weighted
average (for internal fields that would be for instance the cell
volume)

#### `funkyDoCalc` now writes files<a id="sec-9-13-8-14" name="sec-9-13-8-14"></a>

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

#### PDE-functionObjects now relax their equations<a id="sec-9-13-8-15" name="sec-9-13-8-15"></a>

The PDE-functionObjects now honor the `relaxationFactors. If
     =steady` is `false` then relaxation has to be switched on using
`relaxUnsteady`. For the last corrector iteration the equation
is not relaxed unless the parameter `relaxLastIteration` is set.

#### Full set of `laplacian`-operations in `internalField`-parser<a id="sec-9-13-8-16" name="sec-9-13-8-16"></a>

The support of `laplacian` operations (especially with a
coefficient that is different from a scalar) was incomplete. Now
all possible coefficient types are supported.

Also in the `fvcSchemes`-plugin functions the set of
`laplacian`-operators was completed

#### Function object `swakExpression` now has optional `mask`<a id="sec-9-13-8-17" name="sec-9-13-8-17"></a>

If the logical expression `mask` is set then only the results
from `expression` for which `mask` is `true` are used for
accumulations

### Examples<a id="sec-9-13-9" name="sec-9-13-9"></a>

#### Moved the OSCFD-examples to a different directory<a id="sec-9-13-9-1" name="sec-9-13-9-1"></a>

Started one new directory for all cases from presentations

#### Added examples from the swak-training at the 8th Workshop<a id="sec-9-13-9-2" name="sec-9-13-9-2"></a>

Two new examples
-   **sandPitsOfCarcoon:** Reenacting a scene from "Return of the
    Jedi" with `twoPhaseEulerFoam`
-   **landspeedersInCanyon:** Simulating two landspeeders from "A new
    hope" with `simpleFoam`

## 2014-07-11 - version number : 0.3.1<a id="sec-9-14" name="sec-9-14"></a>

### Incompatibilities to previous versions<a id="sec-9-14-1" name="sec-9-14-1"></a>

#### Additional required code snipplet for some Python-functionObjects<a id="sec-9-14-1-1" name="sec-9-14-1-1"></a>

There is now a new code-snipplet required for a number of these
function objects. It can be left empty but has to be specified

#### Dimension required for stuff in `swakSourceFields`<a id="sec-9-14-1-2" name="sec-9-14-1-2"></a>

The expressions used with `swakExplicitSource`,
`swakImplicitSource` and `swakSetValue` now also need a
dimension. This dimension is used to make sure that the user
"knows" the actual dimensions of his source terms

### New supported versions<a id="sec-9-14-2" name="sec-9-14-2"></a>

#### Added support for OpenFOAM 2.3<a id="sec-9-14-2-1" name="sec-9-14-2-1"></a>

This version adds support for OpenFOAM 2.3. The major changes
were due to changes in the API to
-   `searchableSurface`
-   `meshToMesh`

And several minor adaptions

#### Added support for Foam-Extend 3.1<a id="sec-9-14-2-2" name="sec-9-14-2-2"></a>

No major changes required. Runtime-selection tables are now
sorted too

### Infrastructure<a id="sec-9-14-3" name="sec-9-14-3"></a>

#### Improved macros for Foam versions<a id="sec-9-14-3-1" name="sec-9-14-3-1"></a>

This should make `swak.H` more readable

#### Support for private installation of bison<a id="sec-9-14-3-2" name="sec-9-14-3-2"></a>

If a directory `privateRequirements/bin` is private in the
sources directory then the `bison` found there is used. A script
`maintainanceScripts/compileRequirements.sh` is provided that
fetches the bison sources, compiles them and installs them there

### Documentation<a id="sec-9-14-4" name="sec-9-14-4"></a>

#### Information about macro-expansion added<a id="sec-9-14-4-1" name="sec-9-14-4-1"></a>

A section about thus new feature is added

### Bug fixes<a id="sec-9-14-5" name="sec-9-14-5"></a>

#### Invalid entries in distributions from `swakExpressionAverageDistribution`<a id="sec-9-14-5-1" name="sec-9-14-5-1"></a>

The distribution data was not clipped to the range where actually
data was collected. This is now fixed by adding the notion of an
invalid value to `SimpleDistribution`

#### `swakExpressionAverageDistribution` not correctly working in parallel<a id="sec-9-14-5-2" name="sec-9-14-5-2"></a>

This should now be fixed

#### `fvOptions` in `swakSourceFields` now actually working<a id="sec-9-14-5-3" name="sec-9-14-5-3"></a>

Because of the missing dimensions until now `swakExplicitSource`
and `swakImplicitSource` were not actually working

#### `fvOptions` did not honor the `selectionMode`-entry<a id="sec-9-14-5-4" name="sec-9-14-5-4"></a>

Always set source etc for whole region. Now the `selectionMode`
is honored (for instance only in `cellZone` the source term will
be applied.)

#### `patchFunctionObject` fails for some parallel runs<a id="sec-9-14-5-5" name="sec-9-14-5-5"></a>

Problem is that when the processor boundaries are used too that
the indexing might be inconsistent on the different
processors. Fix: coupled boundaries are ignored by default (have
to be switched on)

#### `funkyPythonPostproc` not executing function objects<a id="sec-9-14-5-6" name="sec-9-14-5-6"></a>

In newer version the `execute()`-method of function objects
needs a parameter `force`. Fixed

#### Temporary fields shadow 'real' fields in `foam-extend-3.1`<a id="sec-9-14-5-7" name="sec-9-14-5-7"></a>

Due to a small semantic difference in the copy constructor of
`GeometricField` a temporary object of the same name shadows the
real field and prohibits that it is written. Fixed by giving the
copy a unique name

### New features<a id="sec-9-14-6" name="sec-9-14-6"></a>

#### Boundary condition `groovyFixedNormalSlip` in `groovyStandardBCs`<a id="sec-9-14-6-1" name="sec-9-14-6-1"></a>

These boundary conditions allow specifying a value in normal
direction to the patch while in tangential direction the regular
slip condition is applied. This is implemented for `fvPatch` and
`pointPatch`. For the `pointPatch` the normal direction has to be
specified by the user

#### New utility `funkyWarpMesh`<a id="sec-9-14-6-2" name="sec-9-14-6-2"></a>

Utility that calculates new point values for the mesh. In the
simplest form the expression is specified on the command line. If
the option `-expression` is missing the program looks for a
dictionary which allows more control. If `-relative` is specified
points are moved by the result of the expression. The result of
the expression must be a `pointVectorField`

#### Function objects that write additional writes depending on the flow conditions<a id="sec-9-14-6-3" name="sec-9-14-6-3"></a>

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

#### Function object `writeOldTimesOnSignal` with new signal handlers<a id="sec-9-14-6-4" name="sec-9-14-6-4"></a>

This function object installs a new signal handler for specified
signals.

The function object itself saves the last \(N\)-timesteps, the
signal handler (when called) writes these steps to disk and then
calls the regular OpenFOAM signal handler

#### New library with `fvOptions`<a id="sec-9-14-6-5" name="sec-9-14-6-5"></a>

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

#### Macro expansion<a id="sec-9-14-6-6" name="sec-9-14-6-6"></a>

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

### Enhancements<a id="sec-9-14-7" name="sec-9-14-7"></a>

#### `expressionField` now allows setting of dimensions<a id="sec-9-14-7-1" name="sec-9-14-7-1"></a>

An optional entry `dimension` for the function object now allows
setting the dimension of the resulting field

#### New function `distToPatch` in Field-parser<a id="sec-9-14-7-2" name="sec-9-14-7-2"></a>

This function gives the distance in the mesh to a specified face.

Not yet working correctly

#### `panicDump` now allows storing old values<a id="sec-9-14-7-3" name="sec-9-14-7-3"></a>

With the switch `storeAndWritePreviousState` this functionObject
now can cache a number of timesteps and write these results in
addition to the current timestep

#### `swakExpressionAverageDistribution` records extremes and number of samples<a id="sec-9-14-7-4" name="sec-9-14-7-4"></a>

In addition to the average now this Function object also records
the minimum and maximum value for each bin. Also the number of
samples used in the bin

#### Python-function objects that return a boolean now have a initialization-snipplet<a id="sec-9-14-7-5" name="sec-9-14-7-5"></a>

The function objects `executeIfPython`, `setDeltaTWithPython`,
`setEntTimeWithPython` and `writeAndEndWithPython` now expect
code snipplet named `init`. This is executed in the beginning and
can be used to set up global variables if these snipplets need
some kind of state. The variables have to be declared `global` in
the snipplets that do the actual decision (for technical reason)

#### `fvOptions`-support in PDE-function objects<a id="sec-9-14-7-6" name="sec-9-14-7-6"></a>

The function objects `solveTransportPDE` and `solveLaplacianPDE`
now call the regular `fvOptions`-callbacks (this allows modifying
the solution in the same way it can be modified for solvers)

### Examples<a id="sec-9-14-8" name="sec-9-14-8"></a>

#### `other/simpleBendedPipe`<a id="sec-9-14-8-1" name="sec-9-14-8-1"></a>

Example to demonstrate `funkyWarpMesh` and post-processing on
bended geometries
