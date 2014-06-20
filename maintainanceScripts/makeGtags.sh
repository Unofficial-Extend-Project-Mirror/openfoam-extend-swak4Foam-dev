#! /usr/bin/env bash

GTAGSPATH=`which gtags`
NOGTAGS=$?

if [[ $NOGTAGS != 0 ]]; then
    echo "No gtags found. No source database built (this is only for developers anyway)"
else
    echo "Building gtags-database"

    rm .files2gtag

    for f in Libraries Utilities
    do
	find  -E Libraries \( ! -path "*Make*" -a ! -path "*lnInclude*" -a -regex ".*.(yy|ll|C|H)" -print \) >>.files2gtag
    done

    gtags -v -f .files2gtag
fi
