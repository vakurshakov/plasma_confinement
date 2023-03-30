#!/usr/bin/python
from cmath import isnan
from numpy import append

# Reduces an array for a concrete rank.
def reduce_array(a, rank, proc) -> list:
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


# Merges rank-separated lists.
def aggregate_list(ls: list) -> list:
  for l in ls[1:]:
    ls[0] = append(ls[0], l, axis=0)

  return ls[0]