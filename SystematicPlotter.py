
# coding: utf-8

# In[14]:

import math
import numpy as np
import os
import sys

import seaborn as sns
sns.set(style="ticks", palette="muted", color_codes=True)

import xml.etree.ElementTree as ET
tree  = ET.parse('data/Systematics.xml')
root = tree.getroot()

import pandas as pd


# In[7]:

PlotDir='figs/Results'

if not os.path.exists(PlotDir):
    os.makedirs(PlotDir)
    
imageType='png'

beast=True
show=True


# In[ ]:

if len(sys.argv) == 4:
    if int(sys.argv[1])==0:
        beast=False
    if int(sys.argv[2])==0:
        show=False
    imageType=sys.argv[3]


# In[8]:

a=0.0

Data=[]


for Detector in root:
    for Beam in Detector:
        for BGSource in Beam:
            
            value=0
            sumup=0
            sumdown=0
            for Systematic in BGSource:
                #if Systematic.tag=="Location":
                #    continue
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
           
        
                
HERLER = pd.DataFrame(Data, columns=['Beam', 'Source', 'Detector', 'ratio', 'lowerror', 'uppererror'])
    


# In[9]:

import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches



import belle2style_mpl
style = belle2style_mpl.b2_style_mpl()  #style created by Michael Hedges to match BELLE II root style
plt.style.use(style)

TColor = "#5A8F29"
GColor = "#3C7DC4"


if beast:    
    TColor = "#CDAD2C"
    GColor = "#42ACB2"


# In[10]:

from matplotlib.ticker import FuncFormatter

def labeller(x, pos):
    return '10$^{'+str(int(np.log10(x)))+'}$'
    


# In[13]:


for beam in 'HER', 'LER':
    
    plt.figure(figsize=(800/80, 500/80))

    mainAx = plt.subplot(211)
    
    for source in 'Touschek', 'Beam Gas':
        Frame=HERLER.loc[lambda df: (df.Beam == beam), :].loc[lambda df: (df.Source == source), :]
            
        ThisColor=TColor
        xlabel=''
        if source == 'Touschek':
            ax = mainAx
            plt.setp(ax.get_xticklabels(), visible=False)
        elif source == 'Beam Gas':
            ax = plt.subplot(212, sharex=mainAx)
            ThisColor=GColor
            xlabel='Data/Sim ('+beam+')'            
            plt.setp(ax.get_xticklabels(), visible=True)

        
        sns.stripplot(Frame.ratio, Frame.Detector , jitter=False, size=10, color=ThisColor, linewidth=0)

        for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
            f = Frame['Detector'] == ylabel.get_text() 
            ax.errorbar(Frame.ratio[f].values, 
                        np.ones_like(Frame.ratio[f].values)*(y), 
                        xerr=[Frame.lowerror[f].values, Frame.uppererror[f].values], 
                        #ls='none', 
                        elinewidth=2,
                        capthick=2,
                        color=ThisColor)


        plt.ylabel(source)
   
        plt.xlabel(xlabel)

    
        ax.set_xscale("log", nonposx='clip')

    
        ax.tick_params(direction='in', pad=15)
        ax.tick_params(which='minor', direction='in', pad=15)
        ax.grid(True)


        gridlines = ax.get_xgridlines() + ax.get_ygridlines()
        for line in gridlines:
            line.set_linestyle('dotted')
            line.set_color('black')
    
        custom_formatter = FuncFormatter(labeller)

        ax.xaxis.set_major_formatter(custom_formatter)
    
        
        plt.tight_layout()
    
    plt.savefig(PlotDir+'/'+beam+'RatioPlot.'+imageType)
              
    
    if show:
        sns.plt.show()


# In[ ]:




# In[ ]:



