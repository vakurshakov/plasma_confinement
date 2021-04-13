#!/usr/bin/env python
# coding: utf-8

# #### Animation of fields
# 
# I've added an appropriate colorbar, you can read the explanation at this link: https://joseph-long.com/writing/colorbars/, I also added saving option. Now, if you want to save it as a .gif file just delete '#' symol in corresponding line.

# In[21]:


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


# In[22]:


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


# In[23]:


from math import sqrt
import numpy as np


def set_cylindrical_components(field : str, ddata : dict, ddata_enum : dict, SIZE_X : int, SIZE_Y : int):
    
    fr   = [list(np.zeros(SIZE_X)) for i in range(SIZE_Y)]
    fphi = [list(np.zeros(SIZE_X)) for i in range(SIZE_Y)]
    
    fx_ = field + "x"
    fy_ = field + "y"
    fd_ = ddata_enum['frame_data']
    
    fx = ((ddata[fx_])[fd_])
    fy = ((ddata[fy_])[fd_])
    
    an_ = ddata_enum['axes_name']
    
    (ddata[fx_])[an_] = "$" + field + "_r$"
    (ddata[fy_])[an_] = "$" + field + "_{\phi}$"
    
    for i in range(0, SIZE_Y):
        for j in range(0, SIZE_X):
            
            y = (i) - SIZE_Y/2
            x = j - SIZE_X/2    
            r   = sqrt(x*x + y*y)
            
            if (r != 0):
                fr[i][j]   = +fx[i][j]*(x/r) + fy[i][j]*(y/r)
                fphi[i][j] = -fx[i][j]*(y/r) + fy[i][j]*(x/r)
            else: 
                fr[i][j]   = ( fx[i][j] + fy[i][j] )/2
                fphi[i][j] = 0
    
            fx[i][j] = fr[i][j]   
            fy[i][j] = fphi[i][j]


# In[24]:


import matplotlib.pyplot as plt


def set_imshow(subplot, Field: list, cmap_: str, v : tuple, SIZE_X : int, SIZE_Y : int):
    return subplot.imshow(
        Field,
        cmap = plt.get_cmap(cmap_),
        interpolation="gaussian",
        animated=True,
        origin='lower',
        extent=(0,SIZE_X,0,SIZE_Y),
        vmin=v[0], vmax=v[1],
    )


# In[25]:


import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np

def set_colorbar(mappable):
    last_axes = plt.gca()
    ax = mappable.axes
    fig = ax.figure
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="3%", pad=0.05)
    cbar = fig.colorbar(mappable,
                       orientation="vertical",
                       cax=cax)
    plt.sca(last_axes)
    return cbar


# In[30]:


import matplotlib.pyplot as plt
import numpy as np

dr1 = 0.24
dr  = 0.40
d   = 0.04
r_larm = 0.6

def set_whole_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict, SIZE_X, SIZE_Y,
                 current_shape_is_shown = False):
    
    axes[ ddata_name[ddata_enum['axes_position']] ].set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=20)
    
    im_ = set_imshow(axes[ ddata_name[ddata_enum['axes_position']] ],
                     ddata_name[ddata_enum['frame_data']],
                     ddata_name[ddata_enum['colormap']],
                     ddata_name[ddata_enum['vmin_vmax']],
                     SIZE_X, SIZE_Y)
    
    cbars.append(set_colorbar(im_))
    
    if (current_shape_is_shown):
        ax = axes[ ddata_name[ddata_enum['axes_position']] ]
        
        ax.plot([SIZE_X/2 + r_larm/d*np.cos(i) for i in np.arange(0, 2*np.pi, 0.01)],
                [SIZE_Y/2 + r_larm/d*np.sin(i) for i in np.arange(0, 2*np.pi, 0.01)])
        
        ax.plot([SIZE_X/2 + (r_larm + dr1)/d*np.cos(i) for i in np.arange(0, 2*np.pi, 0.01)],
                [SIZE_Y/2 + (r_larm + dr1)/d*np.sin(i) for i in np.arange(0, 2*np.pi, 0.01)])
        
        ax.plot([SIZE_X/2 + (r_larm - dr1)/d*np.cos(i) for i in np.arange(0, 2*np.pi, 0.01)],
                [SIZE_Y/2 + (r_larm - dr1)/d*np.sin(i) for i in np.arange(0, 2*np.pi, 0.01)])
        
        ax.plot([SIZE_X/2 + (r_larm + dr)/d*np.cos(i) for i in np.arange(0, 2*np.pi, 0.01)],
                [SIZE_Y/2 + (r_larm + dr)/d*np.sin(i) for i in np.arange(0, 2*np.pi, 0.01)])
        
        ax.plot([SIZE_X/2 + (r_larm - dr)/d*np.cos(i) for i in np.arange(0, 2*np.pi, 0.01)],
                [SIZE_Y/2 + (r_larm - dr)/d*np.sin(i) for i in np.arange(0, 2*np.pi, 0.01)])


# In[31]:


def clear_whole_figure(axes, cbars, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes[i,j].cla()
    
    for i in range(len(cbars)):
        cbars[i].remove()


# In[ ]:

parameters_file = "jx.txt"
TIME, dt, DTS, SIZE_X, SIZE_Y = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.txt", [frame_data], (axes_position), "axes_name", (vmin, vmax), "colormap"]
    'jx': [ "jx.txt", [], (0,0), "$j_x$", (-0.05, 0.05), "plasma" ],
    'jy': [ "jy.txt", [], (0,1), "$j_y$", (-0.5, 0.5), "plasma" ],
    'Ex': [ "Ex.txt", [], (1,0), "$E_x$", (-0.01, 0.01), "plasma" ],
    'Ey': [ "Ey.txt", [], (1,1), "$E_y$", (-0.01, 0.01), "plasma" ],
    #'off_Ez': [ "Ez.txt", [], (0,4), "$E_z$", (0, 0), "plasma" ],
    #'off_Bx': [ "Bx.txt", [], (1,2), "$B_x$", (0, 0), "plasma"],
    #'off_By': [ "By.txt", [], (1,3), "$B_y$", (0, 0), "plasma"],
    'Bz': [ "Bz.txt", [], (1,2), "$B_z$", (-0.2, 0), "plasma"],
    'off_ne': [ "../../Electrons/density/density.txt", [], (0,2), "$n_e$", (0, 10), "Greys"], 
    
    #'off0': [ "", [], (0,2), "", (0,0), "" ],
}

ddata_enum = {
    
    'file_to_read': 0,
    'frame_data': 1,
    'axes_position': 2,
    'axes_name': 3,
    'vmin_vmax': 4,
    'colormap': 5,

}

nrows = 2
ncols = 3

geometry_is_cylindrical = False

fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(23,12))

from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_num = comm.Get_size()

comm.Barrier()

for t in range(rank, int(TIME/DTS), proc_num):
    imshows = []
    cbars = []
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']])
            
        else:
            continue
    
    if (geometry_is_cylindrical):
        set_cylindrical_components('j', ddata, ddata_enum, SIZE_X, SIZE_Y)
        set_cylindrical_components('E', ddata, ddata_enum, SIZE_X, SIZE_Y)
            
    for name in ddata.keys():
        if (name[:3] != 'off'):    
            set_whole_ax(axes, cbars, ddata[name], ddata_enum, SIZE_X, SIZE_Y,
                         current_shape_is_shown = True)
        
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
    
    axes[0,1].text(0.35, 1.2, "%.2f $t\ {\cdot}\ w_p$" %(DTS*t*dt), transform=axes[0,1].transAxes, fontsize=20)

    
    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("animation/" + name + ".png")
    
    clear_whole_figure(axes, cbars, nrows, ncols)

from os import system 

os.system("cd ./animation/ && (ffmpeg -f image2 %03d.png -r 15 ../../../2d_fields.mp4)")




