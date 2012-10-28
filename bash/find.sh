#!/bin/bash

shopt -s nullglob
shopt -s nocaseglob
shopt -s nocasematch
shopt -s globstar

function check {
    filename=${1##*/}
    if [[ $filename == $pattern ]] 
    then
        if [[ -z $filetype ]]
        then
            echo $1
        elif [[ $filetype == "b" && -b $1 ]]
        then
            echo $1
        elif [[ $filetype == "c" && -c $1 ]]
        then
            echo $1
        elif [[ $filetype == "d" && -d $1 ]]
        then
            echo $1
        elif [[ $filetype == "p" && -p $1 ]]
        then
            echo $1
        elif [[ $filetype == "f" && -f $1 ]]
        then
            echo $1
        elif [[ $filetype == "l" && -L $1 ]]
        then
            echo $1
        elif [[ $filetype == "s" && -S $1 ]]
        then
            echo $1
        fi
    fi
}

if [[ $2 == "-iname" ]]
then 
    pattern=$3
elif [[ $4 == "-iname" ]]
then
    pattern=$5
else
    pattern="*"
fi

if [[ $2 == "-type" ]]
then
    filetype=$3
elif [[ $4 == "-type" ]]
then
    filetype=$5
else
    filetype=""
fi

if [[ -z $1 ]]
then
    path="."
else
    path=$1
fi

for f in $path/**
do
    check $f
done
