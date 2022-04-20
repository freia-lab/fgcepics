from tkinter import *
from tkinter.ttk import *

import epics
import sys
import time

states = [
    "OFF",
    "BLOCKING",
    "ON"
    ]

fgc = sys.argv[1]
state_get_rec = fgc + ":PUB:STATE_PC"
state_set_rec = fgc + ":MODE:PC_SIMPLIFIED:S"
ccv_set_rec = fgc + ":REF:CCV:VALUE:S"
meas_rec = fgc + ":PUB:I_MEAS"

def send_mode():
    new_state = combo_10.current() 
    # Send new state to FGC
    epics.caput(state_set_rec, states[new_state])

def send_ccv(): 
    new_ccv_value = entry_11.get() 
    # Send new CCV value to FGC
    try:
        ccv_value = float(new_ccv_value)
    except ValueError:
        entry_11.delete(0, END)
        entry_11.insert(0, "<Invalid format>")
        entry_11.configure(foreground="red")
        return;    
    
    epics.caput(ccv_set_rec, ccv_value)
    time.sleep(.1)

    if epics.caget(ccv_set_rec + ".STAT"):
        entry_11.delete(0, END)
        entry_11.insert(0, "<FGC error>")
        entry_11.configure(foreground="red")


def clear_ccv(event):
    entry_11.delete(0, END)
    entry_11.config(foreground="")

def curr_state_update(char_value="", **kwargs):
    lbl_1.configure(text=char_value)

window = Tk()
window.title(fgc + " state setting")

frame_0 = Frame(window)
frame_0.grid(column=0, row=0)
frame_1 = Frame(window)
frame_1.grid(column=0, row=1)

#### Row 0: Set mode ####

lbl_00 = Label(frame_0, text="Mode:")
lbl_00.grid(column=0, row=0)

combo_10 = Combobox(frame_0, values=states)
combo_10.grid(column=1, row=0, sticky="NSEW")

btn_20 = Button(frame_0, text="Send", command=send_mode) 
btn_20.grid(column=2, row=0, sticky="NSEW")


#### Row 1: Set CCV value ####

lbl_01 = Label(frame_0, text="Value:")
lbl_01.grid(column=0, row=1)

entry_11 = Entry(frame_0)
entry_11.grid(column=1, row=1, sticky="NSEW")
entry_11.bind("<FocusIn>", clear_ccv)

btn_21 = Button(frame_0, text="Send", command=send_ccv) 
btn_21.grid(column=2, row=1, sticky="NSEW")


#### Row 2: Display current 

lbl_1 = Label(frame_1)
lbl_1.grid(column=0, row=0)

curr_state = epics.PV(state_get_rec)
curr_state.add_callback(curr_state_update)

# Set callback to update CCV value

 
window.mainloop()
