while getopts t:i: flag
do
    case "${flag}" in
        t) TEST=${OPTARG} ;;
        i) ITERS=${OPTARG} ;;
    esac
done

./build/$TEST/q6 $ITERS