#!/bin/bash

# testset_generator.sh [judge | test | full] [ options ]

# options:
#  judge generates 30 cases
#  test  generates 300 cases
#  full  generates 1000 cases
#  -o    output directory
#  -f    parameter file
#  -p    output file prefix
#  -g    command to execute testcase

while [ "$#" -gt 0 ]; do
    case $1 in
        judge|test|full)
            testsetType=$1; shift;;
        -o)
            outputDir=$2; shift 2;;
        -f)
            parameterFile=$2; shift 2;;
        -p)
            outputPrefix=$2; shift 2;;
        -g)
            generatorCommand=$2; shift 2;;
        *)
            echo "Unknown option: $1"; shift;;
    esac
done

if [ -z $outputDir ]; then
    echo '出力先ディレクトリ (default: test)'
#    echo 'output directory (default: test)'
    read line
    if [ -z $line ]; then
        outputDir='test'
    else
        outputDir=$line
    fi
fi

if [ -z $parameterFile ]; then
    echo 'パラメタ記述ファイル (default: testset_parameters.txt)'
#    echo 'parameter file (default: testset_parameters.txt)'
    read line
    if [ -z $line ]; then
        parameterFile='testset_parameters.txt'
    else
        parameterFile=$line
    fi
fi

if [ -z $outputPrefix ]; then
    echo '出力ファイルの接頭辞 (default: test)'
#    echo 'output file prefix (default: test)'
    read line
    if [ -z $line ]; then
        outputPrefix='test'
    else
        outputPrefix=$line
    fi
fi

if [ -z $generatorCommand ]; then
    echo 'テストケース生成コマンド (default: ./testcase_generator)'
#    echo 'command to execute testcase generator (default: ./testcase_generator)'
    read line
    if [ -z $line ]; then
        generatorCommand='./testcase_generator'
    else
        generatorCommand=$line
    fi
fi

mkdir -p $outputDir

cnt=1
grep -e '^[0-9]\+,[^,]*$' $parameterFile | \
    while read line; do
        case "$testsetType" in
            judge)
                num=1;;
            test)
                num=10;;
            full)
                if [ $cnt -lt 250 ]; then
                    num=50
                else
                    num=30
                fi;;
            *)
                num=${line%,*};;
        esac
        if [ $num -lt 1 ]; then
            continue
        fi
        opt=${line#*,}
        first=$cnt
        last=$(expr $cnt + $num - 1)
        outputPaths=`seq -f "$outputDir/$outputPrefix%04g.txt" $first $last`
        $generatorCommand $opt $outputPaths
        cnt=$(expr $cnt + $num)
    done
