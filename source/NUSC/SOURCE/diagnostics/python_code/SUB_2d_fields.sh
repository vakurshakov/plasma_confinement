#!/bin/sh

#PBS -l walltime=00:10:00
#PBS -l select=1:ncpus=12:mpiprocs=24:mem=1000m,place=free

cd $PBS_O_WORKDIR

mpi-selector --unset

source /opt/shared/anaconda/anaconda3-2020/bin/activate

echo -n 'mpiexec = '
which mpiexec
echo

MPI_NP=$(wc -l $PBS_NODEFILE | awk '{ print $1 }')

time mpiexec -hostfile $PBS_NODEFILE -np $MPI_NP python3 $PBS_O_WORKDIR/2d_fields.py



