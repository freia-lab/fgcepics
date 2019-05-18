import sys
import time
from collections import deque

from tkinter import Tk, Button, Label, Frame 
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
import matplotlib.dates as plt_dates

import epics

######## Configure property names - CAN BE EDITED ########

# Published analog property - to be displayed
PUB_PROP = "PUB:I_MEAS"
PUB_PROP_NAME = "I Measure"

# Units
PROP_UNITS = "Current, I"

# Limits
PROP_POS = "LIMITS:OP:I:POS:G" 
PROP_NEG = "LIMITS:OP:I:NEG:G" 

# Time axis size (seconds)
TIME_AXIS_SIZE = 60

# Refresh rate (milliseconds)
REFRESH_RATE_MS = 250

###########################################################



#### Calculate full record names ####

fgc = sys.argv[1]
PUB_PROP_REC = fgc + ':' + PUB_PROP
PROP_POS_REC = fgc + ':' + PROP_POS
PROP_NEG_REC = fgc + ':' + PROP_NEG


#### Obtain plot limits, from FGC properties ####

epics.caput(PROP_POS_REC + '.PROC', 1)
epics.caput(PROP_NEG_REC + '.PROC', 1)

time.sleep(.2)

prop_max = epics.caget(PROP_POS_REC)
prop_min = epics.caget(PROP_NEG_REC)

#### PyEpics PV - I/O Intr record with periodically published data ####

pub_pv = epics.PV(PUB_PROP_REC)


#### Configure matplotlib display format ####

plt.style.use('classic')

fig = Figure()
fig.subplots_adjust(left=0.15, right=0.85)

x_lim_window = TIME_AXIS_SIZE
y_size = prop_max - prop_min
y_lim_min = prop_min - (y_size * 0.025)
y_lim_max = prop_max + (y_size * 0.025)

ax = fig.add_subplot(111)
ax.set_xlabel("Time")
ax.set_ylabel(PROP_UNITS)
ax.set_ylim(y_lim_min, y_lim_max)
ax.grid()

line_meas, = ax.plot_date([], [], "b-")

ax.xaxis.set_major_formatter(plt_dates.DateFormatter("%H:%M:%S"))
fig.autofmt_xdate()


#### Callback for button clicking ####

running = False

def button_clicked():
    global running
    if running:
        running = False
        status_btn.configure(text="Start Reading")
    else:
        running = True
        status_btn.configure(text="Stop Reading")


#### Open Tkinter GUI ####

window = Tk()
window.title(fgc + ' ' + PUB_PROP_NAME)
window.geometry("600x550")

canvas = FigureCanvasTkAgg(fig, master=window)
canvas_plot = canvas.get_tk_widget()
canvas_plot.pack(side="top",fill="x",expand=True)

aux_frame = Frame(window)
aux_frame.pack(side="top", pady=5, expand=True)

status_btn = Button(aux_frame, text="Start reading", command=button_clicked)
status_btn.grid(column=0, row=0)

status_lbl = Label(aux_frame, text="Stopped", bg="yellow")
status_lbl.grid(column=1, row=0)


#### Lists for collected samples ####

time_list = deque([], 500)
ref_list = deque([], 500)


#### Callback for adding new data to list ####

prev_time = None

def add_data(timestamp=None, value=None, status=None, **kwargs):

    if not status and running:
        status_lbl.configure(text="Running", bg="green")
        time_f = float(timestamp) - time.timezone
    elif not status:
        status_lbl.configure(text="Stopped", bg="yellow")
        time_f = float("NaN")
    else:
        status_lbl.configure(text="Error", bg="red")
        time_f = float('NaN')

    # Update data lists only if there is a new timestamp
    global prev_time
    if prev_time == time_f:
        return

    prev_time = time_f
    time_list.append(time_f)
    ref_list.append(value)


#### Callback to draw plot data, called periodically

def plotter(**kwargs):

    # Re-check if there is a new sample with a new timestamp
    # Required, because EPICS only runs callbacks when value is modified (regardless of the new timestamp)
    pub_pv.get_timevars()
    add_data(timestamp=pub_pv.timestamp, value=pub_pv.value, status=pub_pv.status)

    curr_timestamp = time.time() - time.timezone
    time_list_form = plt_dates.epoch2num(time_list)
    time_list_min = plt_dates.epoch2num(curr_timestamp - (TIME_AXIS_SIZE - 1))
    time_list_max = plt_dates.epoch2num(curr_timestamp + 1)

    line_meas.set_data(time_list_form, ref_list)
    ax.relim()
    ax.set_xlim(time_list_min, time_list_max)
    canvas.draw()

    # Setup next plot time event
    canvas_plot.after(REFRESH_RATE_MS, plotter)


#### Configure new data and plotting callbacks, and run main loop ####

pub_pv.add_callback(add_data)
canvas_plot.after(0, plotter)

window.mainloop()


# EOF
