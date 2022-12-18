#!/usr/bin/python
from mpi4py            import MPI
from mpi_utils         import *

from scipy.integrate   import quad, cumulative_trapezoid
from scipy.interpolate import interp1d
from numpy             import sqrt, load, save

import matplotlib.pyplot as plt

dx = 0.05     # , c/wp - Grid spacing
mi_me = 1.0   # , me - Mass of ions

raw_integral_of_Jyg = load('integral_of_Jyg.npy', allow_pickle=True)
integral_of_Jyg = interp1d(raw_integral_of_Jyg[0], raw_integral_of_Jyg[1])

evaluate_to = 'integral_of_xg'


def Bz_value(g):
  return sqrt(- 2 * integral_of_Jyg(g))


if __name__ == '__main__':
  comm = MPI.COMM_WORLD
  rank = comm.Get_rank()
  proc = comm.Get_size()

  gs = reduce_array(raw_integral_of_Jyg[0], rank, proc)
  dxs = []

  for g in gs:
    if rank == 0:
      print(f'g={g:.3f}')
    dxs.append(1 / Bz_value(g))

  # Calculating non-evenly spaced x value.
  dxs_all = comm.allgather(dxs); dxs_all = aggregate_list(dxs_all)
  gs_all = comm.allgather(gs); gs_all = aggregate_list(gs_all)


  if rank == 0:
    xs_all = cumulative_trapezoid(dxs_all, gs_all, initial=0)
    plt.plot(gs_all, xs_all, label='x(g)')
    plt.legend()
    plt.show()

    # Calculating evenly-spaced values.
    save(f'{evaluate_to}.npy', [gs_all, xs_all])