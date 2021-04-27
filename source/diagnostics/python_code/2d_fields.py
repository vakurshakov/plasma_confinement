#!/usr/bin/env python
# coding: utf-8

# #### Animation of fields

# In[1]:


def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        TIME_dt_DTS = f.readline().split(' ')
        TIME = (int)(TIME_dt_DTS[0])
        dt = (float)(TIME_dt_DTS[1])
        DTS = (int)(TIME_dt_DTS[2])
        SIZE = f.readline().split(' ')
        SIZE_X = (int)(SIZE[0])
        SIZE_Y = (int)(SIZE[1])
        return TIME, dt, DTS, SIZE_X, SIZE_Y


# In[2]:


#TODO: itertools.islice(...)

def set_field_frame(t : int, field : list, field_to_read : str):
    field.clear()
    with open(field_to_read, 'r') as f:
        next(f)
        next(f)
        for n in range(t):
            next(f) 
            continue
        
        temp = f.readline() 
        temp = temp.split('\t')[:-1]
        for X in range(len(temp)) :
            field.append(list(map(float, temp[X].split(' ')[:-1])))


# In[3]:


import matplotlib.pyplot as plt


def set_imshow(subplot, Field: list, cmap_: str, v : tuple, SIZE_X : int, SIZE_Y : int):
    return subplot.imshow(
        Field[:][:],
        cmap = plt.get_cmap(cmap_),
        interpolation="gaussian",
        animated=True,
        origin='lower',
        extent=(0,SIZE_X,0,SIZE_Y),
        vmin=v[0], vmax=v[1],
    )


# In[4]:


import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np


def set_colorbar(mappable):
    last_axes = plt.gca()
    ax = mappable.axes
    fig = ax.figure
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="30%", pad=0.05)
    cbar = fig.colorbar(mappable,
                       orientation="vertical",
                       cax=cax)
    plt.sca(last_axes)
    return cbar


# In[5]:


import matplotlib.pyplot as plt
import numpy as np


def set_whole_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict, SIZE_X, SIZE_Y,
                 current_shape_is_shown = False):
    
    ax = axes[ ddata_name[ddata_enum['axes_position']] ]

    ax.set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=20)
    ax.set_xlabel( ddata_name[ ddata_enum['xlabel'] ], fontsize=15 )
    ax.set_ylabel( ddata_name[ ddata_enum['ylabel'] ], fontsize=15 )

    im_ = set_imshow(ax,
                     ddata_name[ddata_enum['frame_data']],
                     ddata_name[ddata_enum['colormap']],
                     ddata_name[ddata_enum['vmin_vmax']],
                     SIZE_X, SIZE_Y)
    
    cbars.append(set_colorbar(im_))


# In[6]:


def clear_whole_figure(axes, cbars, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes[j].cla()
    
    for i in range(len(cbars)):
        cbars[i].remove()


# In[7]:



parameters_file = "jx.txt"
TIME, dt, DTS, SIZE_X, SIZE_Y = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.txt", [frame_data], (axes_position), "axes_name", "xlabel", "ylabel" (vmin, vmax), "colormap"]
    'jx': [ "jx.txt", [], (0), "ток $\hatj_x$", "$\hatx$", "$\haty$", (-5e-2, 5e-2), "plasma" ],
    'jy': [ "jy.txt", [], (1), "ток $\hatj_y$", "$\hatx$", "$\haty$", (-5e-2, 5e-2), "plasma" ],
    'jz': [ "jz.txt", [], (2), "ток $\hatj_z$", "$\hatx$", "$\haty$", (-5e-2, 5e-2), "plasma" ],
    
    #'Ex': [ "Ex.txt", [], (1,1), "$E_x$", (-5e-2, 5e-2), "plasma" ],
    #'Ey': [ "Ey.txt", [], (1,2), "$E_y$", (-5e-2, 5e-2), "plasma" ],
    #'Ez': [ "Ez.txt", [], (1,3), "$E_z$", (-5e-2, 5e-2), "plasma" ],
    #
    #'Bx': [ "Bx.txt", [], (2,1), "$B_x$", (0, 0.2), "plasma"],
    #'By': [ "By.txt", [], (2,2), "$B_y$", (-5e-2, 5e-2), "plasma"],
    #'Bz': [ "Bz.txt", [], (2,3), "$B_z$", (-5e-2, 5e-2),  "plasma"],
    #
    #'ne': [ "../../Electrons/density/density.txt", [], (0,0), "$n_e$", (0, 5), "Greys"], 
    #
    #'off1': [ "", [], (1,0), "", (), "" ],
    #'off2': [ "", [], (2,0), "", (), "" ],

}

ddata_enum = {
    
    'file_to_read': 0,
    'frame_data': 1,
    'axes_position': 2,
    'axes_name': 3,
    'xlabel': 4,
    'ylabel': 5,
    'vmin_vmax': 6,
    'colormap': 7,

}

nrows = 1
ncols = 3


fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(18,12))

'''
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_num = comm.Get_size()

comm.Barrier()
'''

for t in range(0, int(TIME/DTS)):
    imshows = []
    cbars = []
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']])
            
        else:
            continue
             
    for name in ddata.keys():
        if (name[:3] != 'off'):    
            set_whole_ax(axes, cbars, ddata[name], ddata_enum, SIZE_X, SIZE_Y)
        
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
    
    axes[1].text(-1.5, 1.1, "%.2f $t\ {\cdot}\ w_p$" %(DTS*t*dt), transform=axes[1].transAxes, fontsize=20)

    
    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("animation/" + name + ".png")
    
    clear_whole_figure(axes, cbars, nrows, ncols)

from os import system 

os.system("cd ./animation/ && (ffmpeg -f image2 %03d.png -r 15 ../../2d_fields.mp4)")


# In[ ]:




