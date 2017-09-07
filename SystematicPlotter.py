#!/usr/bin/python

#import relevant libraries`
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


#set plot directory, make it if it doesn't exist
PlotDir='figs/Results'

if not os.path.exists(PlotDir):
    os.makedirs(PlotDir)
    
imageType='pdf'

beast=True
show=True



# for use in batch mode
if len(sys.argv) == 4:
    if int(sys.argv[1])==0:
        beast=False
    if int(sys.argv[2])==0:
        show=False
    imageType=sys.argv[3]

#read the xml data into a pandas dataframe
Data=[]
PS=[]
for Detector in root:
    det=""
    if Detector.tag=="LYSO_hitRate_forward":
        det="Forward LYSO"
    if Detector.tag=="CSI_Pure_hitRate_forward":
        det="Forward Pure CSI"
    if Detector.tag=="BGO_dose_forward":
        det="Forward BGO"
    if Detector.tag=="BGO_dose_backward":
        det="Backward BGO"
    if Detector.tag=="HE3_rate":
        det="$^3He$ tube"
        
    ch=Detector.attrib['value']    

    for Beam in Detector:
        for BGSource in Beam:
            #Get PScale values
            if BGSource.tag=="PScale":
                PS.append([Beam.tag, det, float(BGSource.attrib['value']), float(BGSource.text)])
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
            
            Data.append([Beam.tag.replace('_', ' '), BGSource.tag.replace('_', ' '), det, ch, value, math.sqrt(sumdown), math.sqrt(sumup)])
           
        
#create the dataframe                
HERLER = pd.DataFrame(Data, columns=['Beam', 'Source', 'Detector', 'Channel', 'ratio', 'lowerror', 'uppererror'])

#Create a PScale dataframe
PScale = pd.DataFrame(PS, columns=['Beam', 'Detector', 'PScale', 'Error'])

#print HERLER

#import matplotlib
import matplotlib as mpl
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


#import the belle II style
import belle2style_mpl
style = belle2style_mpl.b2_style_mpl()  #style created by Michael Hedges to match BELLE II root style
plt.style.use(style)

#my colours
HERColour = "#000000"
LERColour = "#000000"

#the beast colours
if beast:
    HERColour = "#0000FF"
    LERColour = "#FF0000"



from matplotlib.ticker import FuncFormatter

def labeller(x, pos):
    return '10$^{'+str(int(np.log10(x)))+'}$'



   
plt.figure(figsize=(800/80, 800/80))

mainAx = plt.subplot(211)
        
Hoffset=-0.1875
Loffset=0.1875
    
#loop over Touschek and beam gas
for source in 'Beam Gas', 'Touschek':
    #select appropriate data from the data frame
    AFrame=HERLER.loc[lambda df: (df.Source == source), :] 
        
    
    xlabel='$O_{bg}^{data}/O_{bg}^{sim}$'
    if source == 'Beam Gas':
        ax = mainAx
        plt.setp(ax.get_xticklabels(), visible=False)
    elif source == 'Touschek':
        ax = plt.subplot(212, sharex=mainAx)
        xlabel='$O_{T}^{data}/O_{T}^{sim}$'            
        plt.setp(ax.get_xticklabels(), visible=True)

        
    for beam in 'HER', 'LER':
    
        HFrame=HERLER.loc[lambda df: (df.Beam == beam), :].loc[lambda df: (df.Source == source), :] 
    
    
        c=HERColour
        m='^'
        o=Hoffset
        if beam == 'LER':
            c=LERColour
            m='o'
            o=Loffset
        
        #plot data
    #sns.stripplot(x="ratio", y="Detector", hue="Beam", data=AFrame, jitter=False, split=True, size=10, marker='o', edgecolor=['#FF0000','#0000FF'], linewidth=1)
        sns.stripplot(HFrame.ratio, HFrame.Detector , jitter=False, size=10, marker=m,edgecolor=c, linewidth=1)
  
        for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
            f = HFrame['Detector'] == ylabel.get_text() 
            ax.errorbar(HFrame.ratio[f].values, 
                        np.ones_like(HFrame.ratio[f].values)*(y+o), 
                        xerr=[HFrame.lowerror[f].values, HFrame.uppererror[f].values], 
                        alpha=.25,
                        elinewidth=10,
                        capthick=0,
                        color=c,
                        linewidth=0)

        
    for point in ax.collections:
        point.set_facecolor('none')
           
        offsets=point.get_offsets()
        o=Loffset
        if point.get_edgecolors()[0][0]<0.5:
            o=Hoffset
       
        
        for i in range(0,len(offsets)):
            offsets[i][1] = offsets[i][1]+o
            

        point.set_offsets(offsets)


        
            
        
        
     
        

    #set axis titles
    plt.ylabel('')
    plt.xlabel(xlabel, horizontalalignment='right', x=1.0)
        
    
    #set log scale on x
    ax.set_xscale("log", nonposx='clip')

    ax.set_xlim(0.01, 10000)
    ax.set_ylim(4.5, -0.5)
        
    #set ticks pointing in
        
    plt.tick_params(axis='y',          # changes apply to the x-axis
                    which='minor',      # both major and minor ticks are affected
                    left='off',      # ticks along the bottom edge are off
                    right='off') # labels along the bottom edge are off   
    
    plt.tick_params(axis='y',          # changes apply to the x-axis
                    which='major',      # both major and minor ticks are affected
                    direction='in',
                    pad=15) # labels along the bottom edge are off       
        
        
    plt.tick_params(axis='x',          # changes apply to the x-axis
                    which='both',      # both major and minor ticks are affected
                    direction='in',
                    pad=15) # labels along the bottom edge are off    
        
    
        
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
    plt.savefig(PlotDir+'/RatioPlot.'+imageType)
              
    #show plot
if show:
    sns.plt.show()










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
    sns.stripplot(Frame.PScale, Frame.Detector , jitter=False, size=10, color='black', linewidth=0)

    #add error abrs
    for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
        f = Frame['Detector'] == ylabel.get_text() 
        ax.errorbar(Frame.PScale[f].values, 
                    np.ones_like(Frame.PScale[f].values)*(y), 
                    xerr=[Frame.Error[f].values, Frame.Error[f].values], 
                    #ls='none', 
                    elinewidth=2,
                    capthick=2,
                    color='black')

    #set axis titles
    plt.ylabel(beam)
    xlab = "$P_{Scale}$"
    if beam == 'HER':
        xlab = xlab+"$Z_{e}^{2}$"
    plt.xlabel(xlab)


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





