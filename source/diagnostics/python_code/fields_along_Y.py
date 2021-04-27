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
    ax.set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=15 )
    ax.set_xlim( (0, SIZE) )
    ax.set_xlabel( ddata_name[ ddata_enum['xlabel'] ], fontsize=15 )
    ax.set_ylim( ddata_name[ ddata_enum['vmin_vmax'] ] )
    ax.set_ylabel( ddata_name[ ddata_enum['ylabel'] ], fontsize=15 )
    pl_ = ax.plot( range(SIZE), (ddata_name[ ddata_enum['frame_data'] ])[0] )
    
    

# In[4]:


def clear_whole_figure(axes, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes[j].cla()


# In[8]:

import os

parameters_file = "jx.txt"
TIME, dt, DTS, SIZE = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.txt", [frame_data], (axes_position), "axes_name", "xlabel", "ylabel", (vmin, vmax)]
    'jx': [ "jx.txt", [], (0), "ток$\ \hatj_x\ $в центре", "$(\hatx\ =\ Lx/2,\ \haty,\hatt)$", "$\hatj_x$", (-1, 1) ],
    'jy': [ "jy.txt", [], (1), "ток$\ \hatj_y\ $в центре", "$(\hatx\ =\ Lx/2,\ \haty,\hatt)$", "$\hatj_y$", (-1, 1) ],
    'jz': [ "jz.txt", [], (2), "ток$\ \hatj_z\ $в центре", "$(\hatx\ =\ Lx/2,\ \haty,\hatt)$", "$\hatj_z$", (-1, 1) ],
    #'Ex': [ "Ex.txt", [], (1,0), "$E_x\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (-1e-2, 1e-2) ],
    #'Ey': [ "Ey.txt", [], (1,1), "$E_y\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (-1e-2, 1e-2) ],
    #'Ez': [ "Ez.txt", [], (1,2), "$E_z\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (-1e-2, 1e-2) ],
    #'Bx': [ "Bx.txt", [], (2,0), "$B_x\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (0,     0.3)  ],
    #'By': [ "By.txt", [], (2,1), "$B_y\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (-1e-3, 1e-3) ],
    #'Bz': [ "Bz.txt", [], (2,2), "$B_z\ (\ \hatx\ =\ Lx/2,\ \haty\ )$", (-1e-3, 1e-3) ],
    
}

ddata_enum = {
    
    'file_to_read': 0,
    'frame_data': 1,
    'axes_position': 2,
    'axes_name': 3,
    'xlabel': 4,
    'ylabel': 5,
    'vmin_vmax': 6,
    
}

nrows = 1
ncols = 3


fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(21,15))


for t in range(0, int(TIME/DTS), 1):
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']] )

            set_whole_ax(axes, ddata[name], ddata_enum, SIZE)
            
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
        
    axes[1].text(0.4, 1.05, "$\hatt\ =\ %.2f\ $" %(DTS*t*dt), transform=axes[1].transAxes, fontsize=15)

    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("./animation/" + name  + ".png")
    
    clear_whole_figure(axes, nrows, ncols) 

import os

os.system("cd ./animation/ && (ffmpeg -f image2 -i %03d.png -r 15 ../../fields_along_Y.mp4)")