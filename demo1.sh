#!/bin/bash

function main_function {
    testmode=$1
    check_compiled;
    if [ "$testmode" == "B" ]; then
        do_run_firetrack "waveforms_first_5e5_points.mda";
    else
        do_run_firetrack "first_1e3_points_filtered.mda";
    fi
}

function do_run_firetrack {
    fname=$1
    echo "Loading $fname..."
    if [ ! -f testdata/$fname ] || [ $(wc -c <"$file") lt 1000000 ] ; then
        read -p "We will need to download some data. Proceed? (y/n): " yn
        case $yn in
            [Yy]* )
   		#read -p "Please enter the passcode: " passcode
		do_download $fname;;
            [Nn]* ) exit;;
            * ) echo "Please answer yes or no next time!"; exit;;
        esac
    fi
    
    echo "Now I am trying to launch firetrack. If this doesn't work, ask Jeremy."
    bin/firetrack --waveforms testdata/$fname
}

function check_compiled {
    if [ ! -f bin/firetrack ]; then
        echo ""
        echo "Binary bin/firetrack not found."
        echo "Maybe it hasn't been compiled yet."
        echo "I will tell you how to do that, but first you need to install Qt4 or Qt5 development environment. Then..."
        echo "> cd src"
        echo "> qmake"
        echo "> make"
        echo "> cd .."
        echo "Then try running this script again."
        echo ""
        exit;
    fi;
}

function do_download {
    fname=$1
    #passcode=$2
    curl -f http://97.107.129.125/$fname -o testdata/$fname
}

testmode=$1
main_function $testmode

