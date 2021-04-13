#!/usr/bin/env python
# coding: utf-8

# In[1]:


def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        TIME_dt_DTS = f.readline().split(' ')
        TIME = (int)(TIME_dt_DTS[0])
        dt = (float)(TIME_dt_DTS[1])
        DTS = (int)(TIME_dt_DTS[2])
        SIZE = (int)(f.readline())
        return TIME, dt, DTS, SIZE


# In[2]:


def set_field_frame(t : int, field : list, field_to_read : str):
    field.clear()
    with open(field_to_read, 'r') as f:
        next(f)
        next(f)
        for n in range(t):
            next(f) 
            continue
        
        temp = f.readline()
        temp = temp.split(' ')[:-1]
        field.append(list(map(float, temp)))


# In[3]:


import matplotlib.pyplot as plt

def set_whole_ax(axes, ddata_name : list, ddata_enum : dict, SIZE):
    ax = axes[ ddata_name[ddata_enum['axes_position']] ]
    ax.set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=15)
    ax.set_xlim( (0, SIZE) )
    ax.set_ylim( ddata_name[ ddata_enum['vmin_vmax'] ] )
    pl_ = ax.plot( range(SIZE), (ddata_name[ ddata_enum['frame_data'] ])[0] )
    
    # additive information about the current density
    dr_v   = ax.plot((SIZE/2 - r_larm/d - dr/d, SIZE/2 - r_larm/d - dr/d, SIZE/2 + r_larm/d + dr/d, SIZE/2 + r_larm/d + dr/d), (-10, 10, 10, -10))
    dr_vv  = ax.plot((SIZE/2 - r_larm/d + dr/d, SIZE/2 - r_larm/d + dr/d, SIZE/2 + r_larm/d - dr/d, SIZE/2 + r_larm/d - dr/d), (-10, 10, 10, -10))
    dr1_v  = ax.plot((SIZE/2 - r_larm/d - dr1/d, SIZE/2 - r_larm/d - dr1/d, SIZE/2 + r_larm/d + dr1/d, SIZE/2 + r_larm/d + dr1/d), (-10, 10, 10, -10))
    dr1_vv = ax.plot((SIZE/2 - r_larm/d + dr1/d, SIZE/2 - r_larm/d + dr1/d, SIZE/2 + r_larm/d - dr1/d, SIZE/2 + r_larm/d - dr1/d), (-10, 10, 10, -10))
    #r_larm = ax.plot((SIZE/2 - r_larm/d, SIZE/2 - r_larm/d, SIZE/2 + r_larm/d, SIZE/2 + r_larm/d), (-10, 10, 10, -10))
    


# In[4]:


def clear_whole_figure(axes, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes[i,j].cla()


# In[8]:

import os

parameters_file = "jx.txt"
TIME, dt, DTS, SIZE = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.txt", [frame_data], (axes_position), "axes_name", (vmin, vmax)]
    'jx': [ "jx.txt", [], (0,0), "$j_x$", (-0.05, 0.05) ],
    'jy': [ "jy.txt", [], (0,1), "$j_y$", (-1, 1) ],
    'Ex': [ "Ex.txt", [], (1,0), "$E_x$", (-0.05, 0.05) ],
    'Ey': [ "Ey.txt", [], (1,1), "$E_y$", (-0.05, 0.05) ],
    'Bz': [ "Bz.txt", [], (1,2), "$B_z$", (0, 0.3) ],
    
    'off0' : [ "", [], (0,2), "", (), "" ]
    
}

ddata_enum = {
    
    'file_to_read': 0,
    'frame_data': 1,
    'axes_position': 2,
    'axes_name': 3,
    'vmin_vmax': 4,
    
}

nrows = 2
ncols = 3

dr1 = 0.20
dr  = 0.36
d   = 0.04
r_larm = 0.6


fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(25,10))

from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_num = comm.Get_size()

for t in range(rank, int(TIME/DTS), proc_num):
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']] )

            set_whole_ax(axes, ddata[name], ddata_enum, SIZE)
            
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
        
    axes[0, 1].text(0.25, 1.2, "$(\ \hatx,\ \haty\ =\ Ly/2,\ \hatt\ =\ %.2f\ )$" %(DTS*t*dt), transform=axes[0, 1].transAxes, fontsize=13)

    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("./animation/" + name  + ".png")
    
    clear_whole_figure(axes, nrows, ncols) 

os.system("cd ./animation/ && (ffmpeg -f image2 -i %03d.png -r 15 ../../../fields_along_X.mp4)")