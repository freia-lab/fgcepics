import sys
import csv
import time

from tkinter import Tk, Button, Label, Frame, filedialog, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

import epics

######## Configure property names - CAN BE EDITED ########

# Triggers
TRIGGER_GET = "REF:TABLE:FUNC:VALUE:G" 
TRIGGER_SET = "REF:TABLE:FUNC:VALUE:S"

# Arrays
ARRAY_TIM = "REF:TABLE:FUNC:VALUE:TIME"
ARRAY_REF = "REF:TABLE:FUNC:VALUE:REF"

###########################################################


#### Calculate full record names ####

fgc = sys.argv[1]
TRIG_GET = fgc + ':' + TRIGGER_GET
TRIG_SET = fgc + ':' + TRIGGER_SET
TBL_T = fgc + ':' + ARRAY_TIM
TBL_R = fgc + ':' + ARRAY_REF


#### Matplotlib plot formatting ####

window = Tk()
window.title("{} reference table".format(fgc))
window.geometry("600x500")

plt.style.use('classic')

fig = Figure(figsize=(5, 5))
fig.subplots_adjust(left=0.15, right=0.85)
canvas = FigureCanvasTkAgg(fig, master=window)

ax = fig.add_subplot(111)

# To be called to re-draw ax plot
def redrawplot(t_values, r_values):
    ax.clear()
    ax.set_xlabel("Time")
    ax.set_ylabel("Ref")
    ax.grid()
    ax.plot(t_values, r_values, "b-")
    canvas.draw()

redrawplot([], [])


#### Callbacks for button clicking ####

time_values = []
ref_values = []


def read_fgc():
    """
    Read the reference table from a FGC.
    Update 'time_values' and 'ref_values' with new data collected, and display it on screen.
    """

    epics.caput(TRIG_GET+'.PROC', 1)
    
    time.sleep(1)

    global time_values
    global ref_values

    time_values = epics.caget(TBL_T)
    ref_values = epics.caget(TBL_R)

    redrawplot(time_values, ref_values)

def write_fgc():
    """
    Send data from 'time_values' and 'ref_values' (data on screen) to FGC.
    """

    msgbox = messagebox.askquestion("Send to FGC", "Send to FGC?", icon="warning")

    if msgbox == "no":
        return

    epics.caput(TBL_T, time_values)
    epics.caput(TBL_R, ref_values)

    epics.caput(TRIG_SET+'.PROC', 1)


def read_file():
    """
    Read reference table data from a CSV file.
    Update 'time_values' and 'ref_values' with new data collected, and display it on screen.
    """
    
    t_values = []
    r_values = []

    filename = filedialog.askopenfilename(initialdir=".", title="Select file to read...", filetypes=(("csv files","*.csv"),("all files","*.*")))

    with open(filename, 'r', newline='') as csvfile:
        csvreader = csv.reader(csvfile)

        for row in csvreader:

            if len(row) > 2:
                return
                # TODO: Print error
            try:
                t_var = float(row[0])
                r_var = float(row[1])
            except ValueError:
                return
                # TODO: Print error

            t_values.append(t_var)
            r_values.append(r_var)

    global time_values
    global ref_values

    time_values = t_values
    ref_values = r_values

    redrawplot(time_values, ref_values)

def write_file():
    """
    Send data from 'time_values' and 'ref_values' (data on screen) to a new CSV file.
    """

    filename = filedialog.asksaveasfilename(initialdir=".", title="Select file", filetypes=(("csv files","*.csv"),("all files","*.*")))

    tr_pairs = list(zip(time_values, ref_values))

    with open(filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=",")

        for pair in tr_pairs:
            csvwriter.writerow([pair[0], pair[1]])


#### Setup tkinter GUI buttons, and run main loop ####

canvas_plot = canvas.get_tk_widget()
canvas_plot.pack(side="top",fill="both",expand=True)

aux_frame = Frame(window)
aux_frame.pack(side="top", pady=4, fill="x", expand=True)

btn_read = Button(aux_frame, text="Read from FGC", command=read_fgc)
btn_read.grid(column=1, row=0, pady=4, sticky="NSEW")
btn_send = Button(aux_frame, text="Send to FGC", command=write_fgc)
btn_send.grid(column=2, row=0, pady=4, sticky="NSEW")
btn_read_file = Button(aux_frame, text="Read from CSV file", command=read_file)
btn_read_file.grid(column=1, row=1, pady=4, sticky="NSEW")
btn_save_file = Button(aux_frame, text="Save to CSV file", command=write_file)
btn_save_file.grid(column=2, row=1, pady=4, sticky="NSEW")
aux_frame.grid_columnconfigure((0, 3), weight=1, uniform="borders")
aux_frame.grid_columnconfigure((1, 2), weight=1, uniform="buttons")


window.mainloop()

# EOF
