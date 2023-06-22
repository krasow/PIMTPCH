#!/bin/bash
printhelp()
{
    echo "another custom program smh";
    echo "-t :: test to run 
      [[  cpu upmem gpu threadmp ]]";
    echo "-q :: TPCH query to run 
      [[ 6 ]]";
    echo "-i :: number of iterations";
    echo "-o :: stdout output file";
    exit 0;
}


while getopts t:q:i:o:h flag
do
    case "${flag}" in
        t) TEST=${OPTARG} ;;
        q) QUERY=${OPTARG} ;;
        i) ITERS=${OPTARG} ;;
        o) __STDOUTFILE=${OPTARG} ;;
        h) printhelp ;;
    esac
done

# check if a test is set
if [ -z ${TEST+x} ]; then 
    echo "Must set -t to run a test"
    echo " "
    printhelp
fi

# check if a query is set
if [ -z ${QUERY+x} ]; then 
    echo "Must set -q to run a test"
    echo " "
    printhelp
fi



# -t :: threadmp
if [ "$TEST" = threadmp ]; then 
    for thread in 1 2 4 8 16 32 64 128 172
    do
        echo "$thread threads"
        if [ -z ${__STDOUTFILE} ]; then
            OMP_NUM_THREADS=$thread ./build/cpu/q$QUERY $ITERS 
        else
            OMP_NUM_THREADS=$thread ./build/cpu/q$QUERY $ITERS >> $__STDOUTFILE
        fi
    done
# -t :: cpu upmem gpu
else 
    if [ -z ${__STDOUTFILE} ]; then
        ./build/$TEST/q$QUERY $ITERS 
    else
        ./build/$TEST/q$QUERY $ITERS >> $__STDOUTFILE
    fi
fi 