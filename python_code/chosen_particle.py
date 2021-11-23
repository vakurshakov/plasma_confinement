import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
from cycler import cycler


def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        f.readline()
        TIME_dt_DTS = f.readline().split(' ')
        TIME = (int)(TIME_dt_DTS[0])
        dt = (float)(TIME_dt_DTS[1])
        DTS = (int)(TIME_dt_DTS[2])
        
        f.readline()
        SIZE = f.readline().split(' ')
        SIZE_X = (int)(SIZE[0])
        SIZE_Y = (int)(SIZE[1])

        f.readline()
        sizeof_float = (int)(f.readline())
        
        return sizeof_float, TIME, dt, DTS, SIZE_X, SIZE_Y


def return_chosen_particles_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        f.readline()
        TIME_dt_DTS = f.readline().split(' ')
        TIME = int(TIME_dt_DTS[0])
        dt = float(TIME_dt_DTS[1])
        DTS = int(TIME_dt_DTS[2])
        
        f.readline()
        Number_of_particles = int(f.readline())

        f.readline()
        sizeof_float = int(f.readline())
        
        return sizeof_float, TIME, dt, DTS, Number_of_particles


def set_field_frame(t : int, field, field_to_read : str, sizeof_float : int, SIZE_X : int, SIZE_Y : int):
    dtype = np.float32
    
    with open(field_to_read, 'br') as f:
        
        f.seek((t*SIZE_X*SIZE_Y)*sizeof_float, os.SEEK_SET)

        temp = np.fromfile(f, dtype=dtype, count=SIZE_X*SIZE_Y)

        np.copyto(field, temp.reshape(SIZE_Y, SIZE_X))


def set_chosen_particles(t : int, field, field_to_read : str, sizeof_float : int, number_of_chosen_particles : int):
    dtype = np.float32
    
    with open(field_to_read, 'br') as f:
        
        f.seek((t*number_of_chosen_particles*6)*sizeof_float, os.SEEK_SET)

        temp = np.fromfile(f, dtype=dtype, count=number_of_chosen_particles*6)

        np.copyto(field, temp.reshape(number_of_chosen_particles, 6))


def set_imshow(subplot, Field: list, cmap_: str, v : tuple, SIZE_X : int, SIZE_Y : int):
    return subplot.imshow(
        Field[250-150:250+150,250-150:250+150],
        cmap=plt.get_cmap(cmap_),
        interpolation="gaussian",
        animated=True,
        origin='lower',
        extent=(250-150,250+150,250-150,250+150),
        vmin=v[0], vmax=v[1],
    )


def set_scatter(subplot, Field: list, cmap_: str, v : tuple):
    return subplot.scatter(
        x=Field[::4, 0],
        y=Field[::4, 1],
        c=Field[::4,-1],
        alpha=0.7,
        cmap=plt.get_cmap(cmap_),
        vmin=v[0], vmax=v[1],
    )    


def set_trajectory_plot(subplot, Field: list):
    return subplot.plot(
        Field[:, 0],
        Field[:, 1],
        alpha=0.9,
        linestyle='-',
    )


def set_colorbar(mappable, pos : str, pad):
    last_axes = plt.gca()
    ax = mappable.axes
    fig = ax.figure
    divider = make_axes_locatable(ax)
    cax = divider.append_axes(pos, size="3%", pad=pad)
    cbar = fig.colorbar(mappable,
                       orientation="horizontal",
                       cax=cax)
    plt.sca(last_axes)
    return cbar


def set_field_on_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict, SIZE_X, SIZE_Y,
                 current_shape_is_shown = False):
    
    ax = axes # [ ddata_name[ddata_enum['axes_position']] ]

    ax.set_title( ddata_name[ ddata_enum['axes_name'] ], fontsize=20)
    ax.set_xlabel( ddata_name[ ddata_enum['xlabel'] ], fontsize=15 )
    ax.set_ylabel( ddata_name[ ddata_enum['ylabel'] ], fontsize=15 )

    im_ = set_imshow(ax,
        ddata_name[ddata_enum['frame_data']],
        ddata_name[ddata_enum['colormap']],
        ddata_name[ddata_enum['vmin_vmax']],
        SIZE_X, SIZE_Y)

    cbars.append(set_colorbar(im_, "bottom", 0.15))


def set_scatter_on_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict):
    
    ax = axes # [ ddata_name[ddata_enum['axes_position']] ]
    
    sc_ = set_scatter(ax,
        ddata_name[ddata_enum['frame_data']],
        ddata_name[ddata_enum['colormap']],
        ddata_name[ddata_enum['vmin_vmax']])

    cbars.append(set_colorbar(sc_, "bottom", 1.3))


def set_trajectory_on_ax(axes, cbars : list, ddata_name : list, ddata_enum : dict):

    
    ax = axes # [ ddata_name[ddata_enum['axes_position']] ]
    
    trajectories = ddata_name[ddata_enum['frame_data']]

    for i in range(number_of_particles_to_track):
        set_trajectory_plot(ax,
            trajectories[:, i, [0,1,-1]])

    # cbars.append(set_colorbar(sc_, "bottom", 1.3))


def clear_whole_figure(axes, cbars, nrows, ncols):
    for i in range(nrows):
        for j in range(ncols):
            axes.cla()
    
    for i in range(len(cbars)):
        cbars[i].remove()


parameters_file = "parameters.txt"
sizeof_float, TIME, dt, DTS, SIZE_X, SIZE_Y = return_parameters(parameters_file)
_, _, _, _, number_of_particles = return_chosen_particles_parameters("../chosen_particles/" + parameters_file)

number_of_particles_to_track = 5


ddata = {
    
    #'name': ["file.bin", [frame_data], (axes_position), "axes_name", "xlabel", "ylabel" (vmin, vmax), "colormap"]
    'field_ne'     : [ "./density.bin", np.zeros((SIZE_Y, SIZE_X)), (0), "$\hat{n}_e$", "$\hat{x}/h$", "$\hat{y}/h$", (0, 2), "Greys"  ],
    'scatter_ne'   : [ "../chosen_particles/chosen_particles.bin", np.zeros((number_of_particles, 6)), (0), "", "", "", (1e-2-5e-7, 1e-2+5e-7), "RdYlBu"  ],
    'trajectory_ne': [ "", np.array([np.zeros((number_of_particles_to_track, 6))]), (0), "", "", "", (), "RdYlBu" ]
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

fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(7,10))

custom_cycler = (cycler(color=['#FFFF73', '#AD66D5', '#FF0D00', '#F13C73', '#D6FA3F']))

if ('trajectoty_ne' in ddata.keys()):
    axes(ddata['trajectoty_ne'][ddata_enum['axes_position']]).set_prop_cycle(custom_cycler)


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
        if ('field' in name):
            set_field_frame(t, ddata[name][ddata_enum['frame_data']],
                               ddata[name][ddata_enum['file_to_read']],
                               sizeof_float, SIZE_X, SIZE_Y)
            
        elif ('scatter' in name):
            set_chosen_particles(t, ddata[name][ddata_enum['frame_data']],
                                    ddata[name][ddata_enum['file_to_read']],
                                    sizeof_float, number_of_particles)

        elif ('trajectory' in name):
            if (t == 0):
                np.copyto(((ddata['trajectory_ne'])[ddata_enum['frame_data']])[0,:,:], ((ddata['scatter_ne'])[ddata_enum['frame_data']])[[0, -1, 5, 10, 20]])
                ((ddata['trajectory_ne'])[ddata_enum['frame_data']]) = np.append(((ddata['trajectory_ne'])[ddata_enum['frame_data']]),
                    [((ddata['scatter_ne'])[ddata_enum['frame_data']])[[0, -1, 5, 10, 20]]], axis=0)
            else:
                ((ddata['trajectory_ne'])[ddata_enum['frame_data']]) = np.append(((ddata['trajectory_ne'])[ddata_enum['frame_data']]),
                    [((ddata['scatter_ne'])[ddata_enum['frame_data']])[[0, -1, 5, 10, 20]]], axis=0)

        elif ('off' in name):
            continue
    

    set_field_on_ax(axes, cbars, ddata['field_ne'], ddata_enum, SIZE_X, SIZE_Y)
    set_scatter_on_ax(axes, cbars, ddata['scatter_ne'], ddata_enum)
    set_trajectory_on_ax(axes, cbars, ddata['trajectory_ne'], ddata_enum)
        
    axes.text(0.4, 1.2, "%.2f $t\ {\cdot}\ w_p$" %(DTS*t*dt), transform=axes.transAxes, fontsize=20)

    name = str(t).zfill(len(str(int(TIME/DTS)-1)))
    fig.savefig("animation/" + name + ".png", dpi=200)
    
    clear_whole_figure(axes, cbars, nrows, ncols)





