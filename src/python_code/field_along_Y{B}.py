#!/usr/bin/env python
# coding: utf-8

# In[1]:


def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        f.readline()
        sizeof_float = (int)(f.readline())
        
        f.readline()
        TIME_dt_DTS = f.readline().split(' ')
        TIME = (int)(TIME_dt_DTS[0])
        dt = (float)(TIME_dt_DTS[1])
        DTS = (int)(TIME_dt_DTS[2])
        
        f.readline()
        SIZE = (int)(f.readline())
        
        return sizeof_float, TIME, dt, DTS, SIZE


# In[2]:


import os
import sys
import numpy as np

def set_field_frame(t : int, field, field_to_read : str, sizeof_float : int, SIZE : int):
    dtype = np.float32
    
    with open(field_to_read, 'br') as f:
        
        f.seek((t*SIZE)*sizeof_float, os.SEEK_SET)

        temp = np.fromfile(f, dtype=dtype, count=SIZE)

        np.copyto(field, temp)


# In[15]:


import matplotlib.pyplot as plt

def set_whole_ax(len_ddata, axes, ddata_name : list, ddata_enum : dict, SIZE):
    if (len_ddata == 1): 
        ax = axes
    else:
        ax = axes[ ddata_name[ddata_enum['axes_position']] ]
    
    ax.set_title ( ddata_name[ ddata_enum[ 'title' ] ], fontsize=12 )
    ax.set_xlabel( ddata_name[ ddata_enum['x_label'] ], fontsize=12 )
    ax.set_xlim  ( (0,SIZE) )
    ax.set_ylim  ( ddata_name[ ddata_enum['vmin_vmax'] ] )
    pl_ = ax.plot( range(SIZE)[:], (ddata_name[ ddata_enum['frame_data'] ])[:] )
    


# In[17]:


def clear_whole_figure(len_ddata, axes, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            if (len_ddata == 1):
                axes.cla()
            elif (nrows == 1):
                axes[j].cla()
            else:
                axes[i,j].cla()


# In[19]:


parameters_file = "parameters.txt"
sizeof_float, TIME, dt, DTS, SIZE = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.txt", [frame_data], (axes_position), "title", (vmin, vmax)]
    'jx': [ "jx.bin", np.zeros(SIZE), (0,0), "$j_x$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-5e-5, 5e-5) ],
    #'jy': [ "jy.txt", [], (0,1), "$j_y$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-3, 7e-3) ],
    #'jz': [ "jz.txt", [], (0,2), "$j_z$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-1, 7e-1) ],
    #'Ex': [ "Ex.txt", [], (1,0), "$E_x$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-3, 7e-3) ],
    #'Ey': [ "Ey.txt", [], (1,1), "$E_y$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-3, 7e-3) ],
    #'Ez': [ "Ez.txt", [], (1,2), "$E_z$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-3, 7e-3) ],
    #'Bx': [ "Bx.txt", [], (2,0), "$B_x$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-2, 7e-2) ],
    #'By': [ "By.txt", [], (2,1), "$B_y$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-4, 7e-4) ],
    #'Bz': [ "Bz.txt", [], (2,2), "$B_z$", "$(\ \hatx/h,\ \haty/h\ =\ Ny/2\ )$", (-7e-4, 7e-4) ],
    
}

ddata_enum = {
    
    'file_to_read':     0,
    'frame_data':       1,
    'axes_position':    2,
    'title':            3,
    'x_label':          4,
    'vmin_vmax':        5,
    
}

nrows = 1
ncols = 1


fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(20,20))


for t in range(0, int(TIME/DTS), 1):
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']],
                               sizeof_float, SIZE)

            set_whole_ax(len(ddata), axes, ddata[name], ddata_enum, SIZE)
            
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
        
    axes.text(0.5, 1.1, "$\hatt\ =\ %.2f\ $" %(DTS*t*dt), transform=axes.transAxes, fontsize=13)

    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("./animation/" + name  + ".png")
    
    clear_whole_figure(len(ddata), axes, nrows, ncols) 

import os

os.system("cd ./animation/ && (ffmpeg -f image2 -i %03d.png -r 15 ../../fields_along_Y.mp4)")

