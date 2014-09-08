#!/bin/sh

# Note: This script is meant to be used directly from the library folder that
# has Bison based parsers.
# In addition, the file "Make/options" for that library should include this line:
#   sinclude $(OBJECTS_DIR)/../mydependencies
#
# The "sinclude" means that it will only include if found.

# Clean up the library
wclean libso

# Forcefully generate the lnInclude folder and then create dummy files for the
# expected ".tab.hh" files
wmakeLnInclude .
grep --no-filename tab.hh *.[CHyl]* | sed -e 's=#include\ *"==' -e 's="\ *$==' | \
  sort -u | xargs -I {} touch lnInclude/{}

# Take out the current version of Make/mydependencies and make a dummy
[ -e Make/mydependencies ] && rm Make/mydependencies
touch Make/mydependencies

# Generate the *.dep files, which will tag for us which files need the
# ".tab.hh" files
wmake Make/$WM_OPTIONS/dependencies

# Generate the necessary dependencies, by populating the file Make/mydependencies
# WARNING: This assumes that the file names ending in "Parser" are the ones that
# generate the tab.hh files!

find . -name "*.dep" | sort -u | xargs grep --no-filename "tab.hh"  | grep -v "Parser.dep" | \
  while read line; do

    targetName="${line%%.dep*}.o"
    targetName="${targetName##*/}"
    requirement="${line##*/}"
    requirement="${requirement%%.tab.hh}.o"

    echo "\$(OBJECTS_DIR)/${targetName}: \$(OBJECTS_DIR)/${requirement}" >> Make/mydependencies

done

# Final clean up, as our dependencies file has already been generated
wclean libso
