import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable


def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        f.readline()
        TIME_dt_DTS = f.readline().split(' ')
        TIME = int(TIME_dt_DTS[0])
        dt = float(TIME_dt_DTS[1])
        DTS = int(TIME_dt_DTS[2])
        
        f.readline()
        nvmin_nvmax_dv = f.readline().split(' ')
        dv = float(nvmin_nvmax_dv[2])
        nvmin = int(float(nvmin_nvmax_dv[0]))
        nvmax = int(float(nvmin_nvmax_dv[1]))

        f.readline()
        nymin_nymax_dy = f.readline().split(' ')
        dy = float(nymin_nymax_dy[2])
        nymin = int(float(nymin_nymax_dy[0]))
        nymax = int(float(nymin_nymax_dy[1]))

        f.readline()
        sizeof_float = (int)(f.readline())
        
        return sizeof_float, TIME, dt, DTS, nvmin, nvmax, dv, nymin, nymax, dy


def set_field_frame(t : int, field, field_to_read : str,
    nymin : int, nymax : int, nvmin : int, nvmax : int):
        dtype = np.float32

        with open(field_to_read, 'br') as f:

            f.seek((t*(nymax - nymin)*(nvmax - nvmin))*sizeof_float, os.SEEK_SET)

            temp = np.fromfile(f, dtype=dtype, count=(nymax - nymin)*(nvmax - nvmin))

            np.copyto(field, temp.reshape(((nvmax - nvmin), (nymax - nymin))))


def set_imshow(subplot, Field: list, cmap_: str, v : tuple,
    nymin : int, nymax : int, dy : float, nvmin : int, nvmax : int, dv : float):
    
    return subplot.imshow(
        Field[:,:],
        cmap = plt.get_cmap(cmap_),
        interpolation="gaussian",
        origin='lower',
        extent=(nymin*dy, nymax*dy, 5*nvmin*dv, 5*nvmax*dv),
        vmin=v[0], vmax=v[1],
    )


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


def set_whole_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict, nymin, nymax, dy, nvmin, nvmax, dv):
    
    ax = axes # [ ddata_name[ddata_enum['axes_position']] ]

    ax.set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=20)
    ax.set_xlabel( ddata_name[ ddata_enum['xlabel'] ], fontsize=15 )
    ax.set_ylabel( ddata_name[ ddata_enum['ylabel'] ], fontsize=15 )

    im_ = set_imshow(ax,
        ddata_name[ddata_enum['frame_data']],
        ddata_name[ddata_enum['colormap']],
        ddata_name[ddata_enum['vmin_vmax']],
        nymin, nymax, dy, nvmin, nvmax, dv)
        
    cbars.append(set_colorbar(im_))


def clear_whole_figure(axes, cbars, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes.cla()
    
    for i in range(len(cbars)):
        cbars[i].remove()


parameters_file = "./parameters.txt"
sizeof_float, TIME, dt, DTS, nvmin, nvmax, dv, nymin, nymax, dy = return_parameters(parameters_file)

ddata = {
    
    #'name': ["file.bin", [frame_data], (axes_position), "axes_name", "xlabel", "ylabel" (vmin, vmax), "colormap"]
    'vx(y)': [ "./diagram_vx_on_y.bin",
               np.zeros(((nvmax-nvmin), (nymax-nymin))), (0), "$\hat{v}_x(\hat{y})$", "$\hat{y}$", "$\hat{v}_x$", (0, 1), "Blues" ]
}

ddata_enum = {
    
    'file_to_read':     0,
    'frame_data':       1,
    'axes_position':    2,
    'axes_name':        3,
    'xlabel':           4,
    'ylabel':           5,
    'vmin_vmax':        6,
    'colormap':         7,
}


nrows = 1
ncols = 1

# разобраться с тем, как расширить границы картинки
fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(6, 12))

'''
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_num = comm.Get_size()

comm.Barrier()
'''

for t in range(0, 1):
    imshows = []
    cbars = []
    
    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']],
                               nymin, nymax, nvmin, nvmax)
            
        else:
            continue
    

    for name in ddata.keys():
        if (name[:3] != 'off'):
            set_whole_ax(axes, cbars, ddata[name], ddata_enum, nymin, nymax, dy, nvmin, nvmax, dv)
        
        else:
            axes[ ddata[name][ddata_enum['axes_position']] ].axis("off")
    
    axes.text(0.35, 1.7, "%.2f $t\ {\cdot}\ w_p$" %(DTS*t*dt), transform=axes.transAxes, fontsize=20)

    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("animation/" + name + ".png")
    
    clear_whole_figure(axes, cbars, nrows, ncols)


