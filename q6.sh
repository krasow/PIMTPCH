#!/bin/bash
printhelp()
{
    echo "another custom program smh";
    echo "-t :: test to run 
      [[  cpu upmem gpu threadmp data ]]";
    echo "-i :: number of iterations";
    echo "-o :: stdout output file";
    exit 0;
}


while getopts t:i:o:hd flag
do
    case "${flag}" in
        t) TEST=${OPTARG} ;;
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


# -t :: data
if [ "$TEST" = data ]; then 
    ./build/$TEST/q6
# -t :: threadmp
elif [ "$TEST" = threadmp ]; then 
    for thread in 1 2 4 8 16 32 64 128 172
    do
        if [ -z ${__STDOUTFILE} ]; then
            OMP_NUM_THREADS=$thread ./build/cpu/q6 $ITERS 
        else
            OMP_NUM_THREADS=$thread ./build/cpu/q6 $ITERS >> $__STDOUTFILE
        fi
    done
# -t :: cpu upmem gpu
else 
    if [ -z ${__STDOUTFILE} ]; then
        ./build/$TEST/q6 $ITERS 
    else
        ./build/$TEST/q6 $ITERS >> $__STDOUTFILE
    fi
fi 