#!/bin/bash

for test in game gnuchess fics localparser position
do
    echo "Running ${test} ..."
    if [ ${VERBOSE+1} ]
    then
        LD_LIBRARY_PATH=game tests/${test}/${test}
    else
        LD_LIBRARY_PATH=game tests/${test}/${test} 2>&1 | grep 'Totals:'
    fi
done
