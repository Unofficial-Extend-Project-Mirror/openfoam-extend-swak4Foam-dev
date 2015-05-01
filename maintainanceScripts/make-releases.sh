#!/bin/sh

# Generate release archives (tbz2) using `hg archive command'
#
# Options:
#  -o, --output: path to store archives (default: current folder)
#  -r, --regex:  regular expression to select release tags (default: version_)

readonly EXCLUDE=".issues .hgchurn .hgflow .hgignore .hgsigs .hgtags .hg_archival.txt"

# Generates exclude flag line with the files from EXCLUDE variable
# Example:
#   EXCLUDE="a b c" -> "--exclude a --exclude b --exclude c"
make_exclude_flag () {
    local res=""
    for f in $(echo "$EXCLUDE" | tr " " "\n"); do
        res="$res --exclude $f"
    done

    echo "$res"
}

# Generate archives
main () {
    local output_path="./releases"
    local tag_regex="version_"

    # Parse command line options
    if [ "$#" -gt 0 ]; then
        while [ $# -gt 0 ]; do
            local t="$1"
            case "$1" in
            -o | --output)
                shift
                output_path="$1"
                shift
                ;;
            -r | --reg-exp)
                shift
                tag_regex="$1"
                shift
                ;;
            esac
        done
    fi

    local tags=$(hg -y --color=never tags | grep -E "$tag_regex" | \
        awk '{print $1}')
    local exclude=$(make_exclude_flag)
    echo "Creating"
    for t in $tags; do
        echo -n "  release with tag $t ."
        outFile="$output_path/swak4Foam-${t}.tbz2"
        hg archive -r "$t" $exclude "$outFile" > /dev/null 2>&1
        echo -n ". "
        openssl dgst -md5 -out "$outFile.MD5" "$outFile"
        [ $? ] && echo "OK" || echo "Fail"
    done

    # Restore tip
    hg checkout tip

    return 0
}

main "$@"

# vim:set ft=sh et sw=4 ts=4 sts=4:
