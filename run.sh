#!/bin/bash

export OMP_PLACES=cores
export OMP_PROC_BIND=spread
export OMP_NUM_THREAD=16

if [[ $1 == "-v" || $1 == "--verbose" ]]
then
  export OMP_DISPLAY_ENV=verbose
  export OMP_DISPLAY_AFFINITY=true  # to measure thread migration
elif [[ $1 != "" ]]
then
  echo "Usage $0 [-v | --verbose]" 1>&2
  exit 1
fi

./bin/simulation.out
