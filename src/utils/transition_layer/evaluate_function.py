#!/usr/bin/python
from scipy.integrate   import quad
from scipy.interpolate import interp1d
from cmath             import isnan
from mpi4py            import MPI

import struct
import numpy as np


x0 = 20.0   # , c/wp - Start coordinate 
dx = 0.05   # , c/wp - Grid spacing
mi = 400.0  # , me - Mass of ions
me = 1.0    # , me - Mass of electrons

eps = 1e-6  # Calculation tolerance

# Number of steps for preparation step. Greater
# number leads to better f(x) resolution.
f_first_even_steps  = 10000   # for f in [0.99, 1.0 - eps)
f_second_even_steps = 100000  # for f in [-1.0, 0.99)

mi_me = mi / me

evaluate_to = f'evaluated_function_{x0}X0_{dx}DX_{mi_me}mi_me'


# This function calculates an integral and returns the
# parameter-function value with given integration error.
def x_value(f: float):
    def integrand(z): return 1.0 / \
        np.sqrt(np.pi / 2 - np.arcsin(z) - z * np.sqrt(1.0 - z * z))

    result, err = quad(integrand, f, 1.0, epsabs=eps)
    result *= np.sqrt(np.pi * mi_me)
    result += x0

    return result, err


comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc = comm.Get_size()


# Reduces an array for a concrete rank.
def reduce_array(a) -> list:
    length = len(a)
    chunk = length // proc
    remain = length % proc
    prev = 0

    if rank < remain:
        prev = (chunk + 1) * rank
        chunk += 1
    else:
        prev = chunk * rank + remain

    return a[prev: prev + chunk]

def filter_NaNs(d: dict) -> dict:
    return {k: v for k, v in d.items() if not isnan(v)}

# Merges rank-separated dictionaries.
def aggregate_dictionary(d: dict) -> dict:
    for ds in d:
        d[0].update(ds)

    return d[0]


# Calculating non-evenly spaced x value.
xf_guess = {1.0: x0}

for f in reduce_array(np.linspace(0.99, 1.0 - eps, f_first_even_steps)):
    xf_guess[f] = x_value(f)[0]

for f in reduce_array(np.linspace(-1.0, 0.99, f_second_even_steps)):
    xf_guess[f] = x_value(f)[0]

xf_guess = comm.allgather(xf_guess)

xf_guess = aggregate_dictionary(xf_guess)

xf_guess = filter_NaNs(xf_guess)


if rank == 0:
    # Calculating evenly-spaced values.
    interpolated_function = interp1d(list(xf_guess.values()), list(xf_guess.keys()))

    x_range = np.arange(x0, x_value(-1.0)[0], dx)

    f_values = {x: float(interpolated_function(x)) for x in x_range}

    # Writing table function to the binary file, not mpi-parallel.
    parameters = [x0, x_range[-1], dx, mi, me]

    with open(f'{evaluate_to}.bin', 'wb') as file:
        plain_dict = []
        for x, f in f_values.items():
            plain_dict.append(x)
            plain_dict.append(f)

        result = parameters + plain_dict 
        s = struct.pack('d' * len(result), *result)
        file.write(s)
