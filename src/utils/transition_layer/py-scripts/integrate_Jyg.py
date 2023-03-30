#!/usr/bin/python
from mpi4py            import MPI
from mpi_utils         import *

from scipy.special     import kv
from scipy.integrate   import quad
from numpy             import pi, exp, sqrt, linspace, save

import matplotlib.pyplot as plt

eps = 1e-8  # Calculation tolerance

# Number of steps for preparation even-spaced values
g_even_steps = 100000  # for f in [-1.0, 1.0 - eps)

evaluate_to = 'integral_of_Jyg'

def Jy_value(g):
  gg = g * g / 16
  
  # integrating from g/2 to infinity
  return - exp(-gg) * g * kv(-0.25, gg) / (pi * 2 * sqrt(2))

def IJy_value(g):
  return quad(Jy_value, 0, g, full_output=1, epsabs=eps)[0]


if __name__ == '__main__':
  comm = MPI.COMM_WORLD
  rank = comm.Get_rank()
  proc = comm.Get_size()

  gs = reduce_array(linspace(0.0 + 1e-5, +10.0, g_even_steps), rank, proc)
  Js = []

  for g in gs:
    if rank == 0:
      print(f'g={g:.3f}')
    Js.append(IJy_value(g))

  # Calculating non-evenly spaced x value.
  gs_all = comm.allgather(gs); gs_all = aggregate_list(gs_all)
  Js_all = comm.allgather(Js); Js_all = aggregate_list(Js_all)

  if rank == 0:
    plt.plot(gs_all, Js_all, label='integral of Jy(g)')
    plt.legend()
    plt.show()

    save(f'{evaluate_to}.npy', [gs_all, Js_all])