#!/bin/sh
#SBATCH --partition=debug
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name="dtriangulation"
#SBATCH --output=dt.log
#SBATCH --mail-user=adarshpr@buffalo.edu
#SBATCH --mail-type=ALL

export I_MPI_DEBUG=4
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi.so


echo "SLURMTMPDIR="$SLURMTMPDIR
echo "SLURM_JOBID="$SLURM_JOBID
echo "SLURM_JOB_NODELIST"=$SLURM_JOB_NODELIST
echo "SLURM_NODENAME"=$SLURM_NODENAME
echo "SLURM_NNODES"=$SLURM_NNODES

MYNAME=`hostname`

module load intel-mpi/2017.0.1

sbcast slurm-check $SLURMTMPDIR/slurm-check
sbcast points.csv $SLURMTMPDIR/points.csv

cd $SLURMTMPDIR
pwd

echo "SBATCH:: Starting 'main'"
echo "================================================"

srun ./slurm-check points.csv dt-edges.txt

echo "================================================"
ls -ltr | grep dt-edges.txt

mkdir -p /gpfs/scratch/adarshpr/$SLURM_JOBID/$MYNAME/ && mv ./* /gpfs/scratch/adarshpr/$SLURM_JOBID/$MYNAME/ && echo "Files cleanedup!"

echo "Finished!"
