
# coding: utf-8

# In[5]:

#import relevant libraries
import math
import numpy as np
import os
import sys

import seaborn as sns
sns.set(style="ticks", palette="muted", color_codes=True)

import xml.etree.ElementTree as ET

#get the systematics xml file
tree  = ET.parse('data/Systematics.xml')
root = tree.getroot()

import pandas as pd


# In[6]:

#set plot directory, make it if it doesn't exist
PlotDir='figs/Results'

if not os.path.exists(PlotDir):
    os.makedirs(PlotDir)
    
imageType='png'

beast=True
show=True


# In[ ]:

# for use in batch mode
if len(sys.argv) == 4:
    if int(sys.argv[1])==0:
        beast=False
    if int(sys.argv[2])==0:
        show=False
    imageType=sys.argv[3]


# In[47]:


#read the xml data into a pandas dataframe
Data=[]
PS=[]
for Detector in root:
    for Beam in Detector:
        for BGSource in Beam:
            #Get PScale values
            if BGSource.tag=="PScale":
                PS.append([Beam.tag, Detector.tag, float(BGSource.attrib['value']), float(BGSource.text)])
                continue
            value=0
            sumup=0
            sumdown=0
            for Systematic in BGSource:
                down=float(Systematic.find('down').text)
                up= float(Systematic.find('up').text)
                
                if down > 0:
                    temp=up
                    up=down
                    down=temp
                    
                sumup+=up**2
                sumdown+=down**2
                                        
                value= float(BGSource.attrib['value'])
            
            Data.append([Beam.tag.replace('_', ' '), BGSource.tag.replace('_', ' '), Detector.tag.replace('_', ' '), value, math.sqrt(sumdown), math.sqrt(sumup)])
           
        
#create the dataframe                
HERLER = pd.DataFrame(Data, columns=['Beam', 'Source', 'Detector', 'ratio', 'lowerror', 'uppererror'])

#Create a PScale dataframe
PScale = pd.DataFrame(PS, columns=['Beam', 'Detector', 'PScale', 'Error'])



# In[42]:

#import matplotlib
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


#import the belle II style
import belle2style_mpl
style = belle2style_mpl.b2_style_mpl()  #style created by Michael Hedges to match BELLE II root style
plt.style.use(style)

#my colours
TColour = "#5A8F29"
GColour = "#3C7DC4"
HERColour = "#D22630"
LERColour = "#3C7DC4"

#the beast colours
if beast:    
    TColour = "#CDAD2C"
    GColour = "#42ACB2"
    HERColour = "#004855"
    LERColour = "#D92E27"


# In[35]:

#the font I'm using doesn't have a character for ^-, so I have to redefine how the axis is labeled
from matplotlib.ticker import FuncFormatter

def labeller(x, pos):
    return '10$^{'+str(int(np.log10(x)))+'}$'
    


# In[40]:

#Plot Data/Sim for each detector

#loop over the beams
for beam in 'HER', 'LER':
    
    plt.figure(figsize=(800/80, 500/80))

    mainAx = plt.subplot(211)
    
    #loop over Touschek and beam gas
    for source in 'Touschek', 'Beam Gas':
        #select appropriate data from the data frame
        Frame=HERLER.loc[lambda df: (df.Beam == beam), :].loc[lambda df: (df.Source == source), :] 
        
        #The Touschek plot is the top half, beam gas is the bottom half
        ThisColour=TColour
        xlabel=''
        if source == 'Touschek':
            ax = mainAx
            plt.setp(ax.get_xticklabels(), visible=False)
        elif source == 'Beam Gas':
            ax = plt.subplot(212, sharex=mainAx)
            ThisColour=GColour
            xlabel='Data/Sim ('+beam+')'            
            plt.setp(ax.get_xticklabels(), visible=True)

        #plot data
        sns.stripplot(Frame.ratio, Frame.Detector , jitter=False, size=10, color=ThisColour, linewidth=0)

        #add error abrs
        for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
            f = Frame['Detector'] == ylabel.get_text() 
            ax.errorbar(Frame.ratio[f].values, 
                        np.ones_like(Frame.ratio[f].values)*(y), 
                        xerr=[Frame.lowerror[f].values, Frame.uppererror[f].values], 
                        #ls='none', 
                        elinewidth=2,
                        capthick=2,
                        color=ThisColour)

        #set axis titles
        plt.ylabel(source)
        plt.xlabel(xlabel)

        #set log scale on x
        ax.set_xscale("log", nonposx='clip')

        #set ticks pointing in
        ax.tick_params(direction='in', pad=15)
        ax.tick_params(which='minor', direction='in', pad=15)
        
        #don't add grid if belle style is requested
        if beast==False:
            ax.grid(True)
        
            gridlines = ax.get_xgridlines() + ax.get_ygridlines()
            for line in gridlines:
                line.set_linestyle('dotted')
                line.set_color('black')
    
        custom_formatter = FuncFormatter(labeller)
        ax.xaxis.set_major_formatter(custom_formatter)
    
        
        plt.tight_layout()
    
    #save plot
    plt.savefig(PlotDir+'/'+beam+'RatioPlot.'+imageType)
              
    #show plot
    if show:
        sns.plt.show()


# In[46]:

#Plot PScale for each detector

#loop over beams
for beam in 'HER', 'LER':
    plt.figure(figsize=(800/80, 500/80))
    
    ax = plt.subplot(111)
    
    #select appropriate data from the data frame
    Frame=PScale.loc[lambda df: (df.Beam == beam), :]
        
    plt.setp(ax.get_xticklabels(), visible=True)
    
    ThisColour=HERColour
    if beam == 'LER':
        ThisColour=LERColour
        

    #plot data
    sns.stripplot(Frame.PScale, Frame.Detector , jitter=False, size=10, color=ThisColour, linewidth=0)

    #add error abrs
    for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
        f = Frame['Detector'] == ylabel.get_text() 
        ax.errorbar(Frame.PScale[f].values, 
                    np.ones_like(Frame.PScale[f].values)*(y), 
                    xerr=[Frame.Error[f].values, Frame.Error[f].values], 
                    #ls='none', 
                    elinewidth=2,
                    capthick=2,
                    color=ThisColour)

    #set axis titles
    plt.ylabel(beam)
    plt.xlabel('PScale')

    #set ticks pointing in
    ax.tick_params(direction='in', pad=15)
    ax.tick_params(which='minor', direction='in', pad=15)
        
    #don't add grid if belle style is requested
    if beast==False:
        ax.grid(True)
        
        gridlines = ax.get_xgridlines() + ax.get_ygridlines()
        for line in gridlines:
            line.set_linestyle('dotted')
            line.set_color('black')
       
    plt.tight_layout()
    
    #save plot
    plt.savefig(PlotDir+'/'+beam+'PScale.'+imageType)
              
    #show plot
    if show:
        sns.plt.show()


# In[ ]:



