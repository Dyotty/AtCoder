#!/bin/bash

# local_tester.sh [ options ]

# options:
#  -i    directory of the test cases; defaults to test
#  -o    output directory; defaults to the current time in YYYYMMDDhhmmss
#  -e    command to run your program; defaults to ./a.out
#  -c    command to run output checker; defaults to ./output_checker

while [ "$#" -gt 0 ]; do
    case $1 in
        -i)
            inputDir=$2; shift 2;;
        -o)
            outputDir=$2; shift 2;;
        -e)
            execCommand=$2; shift 2;;
        -c)
            outputChecker=$2; shift 2;;
        -r)
            resultFile=$2; shift 2;;
        *)
            echo "Unknown option: $1"; shift;;
    esac
done

if [ -z $inputDir ]; then
    echo 'テストケースの場所 (default: test)'
#    echo 'testcase directory (default: test)'
    read line
    if [ -z $line ]; then
        inputDir='test'
    else
        inputDir=$line
    fi
fi

if [ -z $outputDir ]; then
    curTime=`date +%Y%m%d%H%M%S`
    echo "結果の出力先ディレクトリ (default: $curTime)"
#    echo "output directory (default: $curTime)"
    read line
    if [ -z $line ]; then
        outputDir=$curTime
    else
        outputDir=$line
    fi
fi

if uname -a | grep MINGW > /dev/null; then
    defaultExecCommand=./a.exe
else
    defaultExecCommand=./a.out
fi

if [ -z "$execCommand" ]; then
    echo "解答プログラムを実行するコマンド (default: $defaultExecCommand)"
#    echo "command to execute your program (default: $defaultExecCommand)"
    read line
    if [ -z $line ]; then
        execCommand=$defaultExecCommand
    else
        execCommand=$line
    fi
fi

if [ -z "$outputChecker" ]; then
    echo 'output checker の場所 (default: ./output_checker)'
#    echo 'command to execute output checker (default: ./output_checker)'
    read line
    if [ -z $line ]; then
        outputChecker='./output_checker'
    else
        outputChecker=$line
    fi
fi


resultFile=summary.csv
resultPath=$outputDir/$resultFile

mkdir -p $outputDir

echo 'テストケース名,粗利(pr),納期遅れペナルティ(pe1),電力コスト(ec),段取りコスト(cc),得点(S)' >> $resultPath
#echo 'testcase,gross profit(pr),late delivery penalty cost(pe1),power cost(ec),setup cost(cc),score(S)' >> $resultPath

for inputPath in $inputDir/*.txt; do
    outputPath=$outputDir/${inputPath##*/}
    echo running test on $inputPath...
    $execCommand < $inputPath > $outputPath
    if [ $? -eq 0 ]; then
        echo -n ${inputPath##*/}',' >> $resultPath
        $outputChecker $inputPath $outputPath 0 >> $resultPath
        if [ $? -eq 0 ]; then
            echo 'Accepted'
        else
            echo 'Wrong Answer'
            echo '0,0,0,0,0,WA' >> $resultPath
        fi
    else
        echo "Runtime Error"
        echo ${inputPath##*/}',0,0,0,0,0,RE' >> $resultPath
    fi
done
