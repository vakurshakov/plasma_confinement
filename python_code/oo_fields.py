#!/opt/shared/anaconda/anaconda3-2020/bin/python

from abc import ABC, ABCMeta, abstractmethod
import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
from cycler import cycler
from mpi4py import MPI


class Drawable_object(metaclass=ABCMeta):
    '''
        This is an interface of class that collects data\\
        from file and than can be drawn on a matplotlib\\
        plots via method self.draw().
    '''

    def __init__(self, path_to_file, axes_position):
        ''' 
            ## Parameters
            axes_position
                an axes where the data will be drawn.
        '''
        self.path_to_file = path_to_file
        self.axes_position = axes_position   

    ''' Methods to be overloaded by the child-classes. '''
    @abstractmethod
    def draw(self):
        pass
    
    @abstractmethod
    def clear(self):
        pass

    @abstractmethod
    def draw_info(self, **kwargs):
        pass

    @abstractmethod
    def set_parameters(self, **parameters):
        pass

    @abstractmethod
    def read_parameters(self, t : int, path_to_parameters : str, **kwargs):
        pass

    @abstractmethod
    def set_data(self, data):
        pass

    @abstractmethod
    def read_data(self, path_to_file_containing_data : str, **kwargs):
        pass

    @abstractmethod
    def set_axes_args(self, **kwargs):
        pass


class Field(Drawable_object, metaclass=ABCMeta):
    '''
        This is a class that can be drawn on a matplotlib imshow\\
        via method self.draw().
    '''

    def __init__(self, path_to_file, axes_position, boundaries=(0,1,0,1), cmap='plasma', vmin_vmax=(0,1)):
        ''' Constructor.
            ## Parameters

            For self@Drawable_object: `path_to_file`, `axes_position`;
            
            data_shape: tuple
                shape of the data stored in a file_containing_data,\\
                should take the form `(number_of_elements_x, number_of_elements_y)`.
            
            boundaries: tuple
                boundaries for a imshow's extent,\\
                should take the form `(begin_x, end_x, begin_y, end_y)`.
            
            cmap: str, plt.Colormap
                color map of the imshow.
            
            vmin_vmax: tuple
                minimum and maximum value on a colorbar,\\
                should take the form `(vmin, vmax)`.
        '''
        super().__init__(path_to_file, axes_position)
        self.data       = None
        self.boundaries = boundaries
        self.cmap       = cmap
        self.vmin_vmax  = vmin_vmax
        self.cbars      = []
        self.axes_args  = {}

    def draw(self):
        ''' Draws a plot on a self.axes_position '''
        if self.data.all() != None:
            im_ = self.axes_position.imshow(
                    self.data[:,:],
                    cmap=self.cmap,
                    interpolation="gaussian",
                    animated=True,
                    origin='lower',
                    extent=self.boundaries,
                    vmin=self.vmin_vmax[0],
                    vmax=self.vmin_vmax[1],
                )

            fig = self.axes_position.figure
            divider = make_axes_locatable(self.axes_position)
            cax = divider.append_axes("right", size="3%", pad=0.05)
            self.cbars.append(
                fig.colorbar(
                    im_,
                    orientation="vertical",
                    cax=cax
                )
            )
            
        else:
            raise RuntimeError('No data field')
    
    def clear(self):
        self.axes_position.cla()
        #for cbar in self.cbars:
        #    cbar.remove()

    def set_parameters(self, **parameters):
        for parameter, arg in parameters.items():
            if parameter == 'boundaries':
                self.boundaries = arg
            elif parameter == 'cmap':
                self.cmap = arg
            elif parameter == 'vmin_vmax':
                self.vmin_vmax = arg
            else:
                raise RuntimeError('No parameter named ' + name)
            
    def read_parameters(self, path_to_parameters: str):
        with open(path_to_parameters, 'r') as f:
            f.readline()
            f.readline()
        
            f.readline()
            SIZE = f.readline().split(' ')
            self.boundaries = tuple(map(int, SIZE))

    def set_data(self, data : np.ndarray):
        self.data = data

    def read_data(self, t : int, **kwargs):
        data_shape = (int(abs(self.boundaries[1] - self.boundaries[0])),
                      int(abs(self.boundaries[3] - self.boundaries[2])))

        for name, arg in kwargs.items():
            if name == 'data_shape':
                data_shape = arg
            else:
                raise RuntimeError('No argument named ' + name)
                
        self.data = np.zeros(data_shape)
        
        with open(self.path_to_file + str(t) + ".bin", "rb") as f:
            temp = np.fromfile(
                f,
                dtype  = np.float32,
                count  = int(data_shape[0]*data_shape[1]),
                offset = 0 # int(t*data_shape[0]*data_shape[1]*sizeof_float)
            )
            
            np.copyto(self.data, temp.reshape(data_shape))

    def set_axes_args(self, **kwargs):
        for name, arg in kwargs.items():
            if name in {'title', 'xlabel', 'ylabel', 'xticks', 'yticks'}:
                self.axes_args[name] = arg
            else:
                raise RuntimeError('No parameter named ' + name)

    def draw_info(self, **kwargs):
        for name, arg in {**self.axes_args, **kwargs}.items():
            if name == 'title':
                self.axes_position.set_title( arg, fontsize=20)
            elif name == 'xlabel':
                self.axes_position.set_xlabel(arg, fontsize=15)
            elif name == 'ylabel':
                self.axes_position.set_ylabel(arg, fontsize=15)
            elif name == 'xticks':
                self.axes_position.set_xticks(arg)
            elif name == 'yticks':
                self.axes_position.set_yticks(arg)
            else:
                raise RuntimeError('No parameter named ' + name)
        

def return_parameters(parameters_file : str):
    with open(parameters_file, 'r') as f:
        f.readline()
        TIME_dt_DTS = f.readline().split(' ')
        TIME = int(TIME_dt_DTS[0])
        dt = float(TIME_dt_DTS[1])
        DTS = int(TIME_dt_DTS[2])
        
        f.readline()
        bx_ex_dx = f.readline().split(' ')
        bx = int(bx_ex_dx[0])
        ex = int(bx_ex_dx[1])
        dx = float(bx_ex_dx[2])

        f.readline()
        by_ey_dy = f.readline().split(' ')
        by = int(by_ey_dy[0])
        ey = int(by_ey_dy[1])
        dy = float(by_ey_dy[2])

        f.readline()
        sof_char = f.readline().split(' ')
        sizeof_float = int(sof_char[0])
        # char_bit = int(sof_char[1])

        return sizeof_float, TIME, dt, DTS, bx, ex, dx, by, ey, dy

### Figure parameters.

nrows = 1
ncols = 1

fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(10,10))

custom_cycler = (cycler(color=['#FFFF73', '#FEC47F', '#F2A9D3', '#FF0D00', '#F13C73', '#D6FA3F']))

plt.rc('axes', prop_cycle=custom_cycler)

parameters_file = "parameters.txt"
sizeof_float, TIME, dt, DTS, bx, ex, dx, by, ey, dy = return_parameters(parameters_file)

### Objects to be drawn.

Bz   = Field("fields/whole_field/Bz/", axes[0,2], (bx,ex,by,ey), "bwr", (-1e-2+0.02, +1e-2+0.02))

Bz.set_axes_args(
    title='$\hat{B}_z$',
    xlabel='$\hat{x}/h$',
    ylabel='$\hat{y}/h$',
    xticks=np.linspace(bx,ex,5),
    yticks=np.linspace(by,ey,5)
)

diagnostic_set = [ Bz ]

del Bz

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_num = comm.Get_size()

comm.Barrier()

if (not os.path.exists('./animation') and rank == 0): os.mkdir('./animation')

phi = np.linspace(0, 2*np.pi, 50)

### Drawing cycle.

for t in range(rank*DTS, TIME, proc_num*DTS): 
   for diagnostic in diagnostic_set:
      diagnostic.read_data(t)
      
      diagnostic.draw()
      diagnostic.axes_position.plot((1600 + (96*2 + 10)*np.cos(phi)), (1600 + (96*2 + 10)*np.sin(phi)), '--', color='red')
      diagnostic.axes_position.plot((1600 + (96*2 - 10)*np.cos(phi)), (1600 + (96*2 - 10)*np.sin(phi)), '--', color='red')

      diagnostic.draw_info()
      
   
   
   axes[0,1].text(0.32, 1.1, "$t\ {\cdot}\ {\\omega}_p$ = %.2f" %(t*dt), transform=axes[0,1].transAxes, fontsize=20)
   name = str(int(t/DTS)).zfill(len(str(int(TIME/DTS)-1)))
   fig.savefig("animation/" + name + ".png")

   for diagnostic in diagnostic_set:
      diagnostic.clear()