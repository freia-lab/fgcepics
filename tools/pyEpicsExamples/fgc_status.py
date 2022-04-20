import sys
from tkinter import Tk, Text, Label, Frame

import epics

# Select which published properties to display, by order.
# Currently, it is configured for class 63 - CAN BE EDITED.

data_fields = {

#   <property_name> :
#   {
#       'TYPE':   <data_type>,
#       'RECORD': <record_name>
#   }

    'CLASS_ID':    {'TYPE': 'INT'  , 'RECORD': 'PUB:CLASS_ID'    ,},
    'DATA_STATUS': {'TYPE': 'FLAGS', 'RECORD': 'PUB:DATA_STATUS' ,},
    'STATE_PLL':   {'TYPE': 'ENUM' , 'RECORD': 'PUB:STATE_PLL'   ,},
    'STATE_VS':    {'TYPE': 'ENUM' , 'RECORD': 'PUB:STATE_VS'    ,},
    'STATE_OP':    {'TYPE': 'ENUM' , 'RECORD': 'PUB:STATE_OP'    ,},
    'STATE_PC':    {'TYPE': 'ENUM' , 'RECORD': 'PUB:STATE_PC'    ,},
    'I_EARTH_PCNT':{'TYPE': 'HEX'  , 'RECORD': 'PUB:I_EARTH_PCNT',},
    'EVENT_GROUP': {'TYPE': 'HEX'  , 'RECORD': 'PUB:EVENT_GROUP' ,},
    'I_ERR_MA':    {'TYPE': 'HEX'  , 'RECORD': 'PUB:I_ERR_MA'    ,},
    'I_DIFF_MA':   {'TYPE': 'HEX'  , 'RECORD': 'PUB:I_DIFF_MA'   ,},
    'I_REF':       {'TYPE': 'FLOAT', 'RECORD': 'PUB:I_REF'       ,},
    'I_MEAS':      {'TYPE': 'FLOAT', 'RECORD': 'PUB:I_MEAS'      ,},
    'V_REF':       {'TYPE': 'FLOAT', 'RECORD': 'PUB:V_REF'       ,},
    'V_MEAS':      {'TYPE': 'FLOAT', 'RECORD': 'PUB:V_MEAS'      ,},
    'ST_FAULTS':   {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_FAULTS'   ,},
    'ST_WARNINGS': {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_WARNINGS' ,},
    'ST_LATCHED':  {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_LATCHED'  ,},
    'ST_UNLATCHED':{'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_UNLATCHED',},
    'ST_ADC_A':    {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_ADC_A'    ,},
    'ST_ADC_B':    {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_ADC_B'    ,},
    'ST_ADC_C':    {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_ADC_C'    ,},
    'ST_ADC_D':    {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_ADC_D'    ,},
    'ST_DCCT_A':   {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_DCCT_A'   ,},
    'ST_DCCT_B':   {'TYPE': 'FLAGS', 'RECORD': 'PUB:ST_DCCT_B'   ,},
}

font_style = "helvetica 10"
relief_style = "ridge"
border_size = 2


# Handle each data type properly
# Create callbacks for each data type

def update_label_f(widget, disp_type):

    def update_widget_enum(**kwargs):
        widget.configure(text=kwargs['value'])

    def update_widget_flags(**kwargs):
        widget.configure(text=kwargs['char_value'].replace(' ', '\n'))

    def update_widget_int(**kwargs):
        widget.configure(text=str(kwargs['value']))

    def update_widget_hex(**kwargs):
        formated_str = '{0:#010x} | {0}'.format(kwargs['value'])
        widget.configure(text=formated_str)

    def update_widget_float(**kwargs):
        widget.configure(text="{:.6E}".format(kwargs['value']))

    def update_nothing(**kwargs):
        pass

    switch_case = {
        'ENUM': update_widget_enum,
        'FLAGS': update_widget_flags,
        'INT': update_widget_int,
        'HEX': update_widget_hex,
        'FLOAT': update_widget_float,
    }

    return switch_case.get(disp_type, update_nothing)  


# Widget factory - updated data display (right column)

def value_display_factory(parent, fgc, template):

    widget = Label(
        parent, 
        borderwidth=border_size, 
        relief=relief_style,
        bg="snow", 
        font=font_style)  

    # Create callback function for new data 
    callback_f = update_label_f(widget, template['TYPE'])

    # COnfigure full record name
    record_name = fgc + ':' + template['RECORD']

    # Create object to access EPICS process variable
    pv = epics.PV(record_name)
    pv.add_callback(callback_f)

    return widget 


# Widget factory - property names (left column)

def name_display_factory(parent, fgc, template):

    widget = Label(
        aux_frame, text=name, 
        borderwidth=border_size, 
        relief=relief_style, 
        font=' '.join([font_style,"bold"]))
    
    return widget


# Main script: Set up GUI, and run loop

fgc = sys.argv[1]
window = Tk()
window.title(fgc + " published properties")

for name, desc in data_fields.items():

    aux_frame = Frame(window) 
    aux_frame.pack(side="top", fill='both', expand=True)

    widget_name = name_display_factory(aux_frame, fgc, desc) 
    widget_name.grid(column=0, row=0, sticky="NSEW")    

    widget_value = value_display_factory(aux_frame,fgc, desc)
    widget_value.grid(column=1, row=0, sticky="NSEW")

    aux_frame.grid_columnconfigure(0, weight=1, uniform="group1")
    aux_frame.grid_columnconfigure(1, weight=2, uniform="group1")
    aux_frame.grid_rowconfigure(0, weight=1)

window.mainloop()

# EOF
