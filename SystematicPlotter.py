
# coding: utf-8

# In[1]:

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


# In[2]:

# for use in batch mode
if len(sys.argv) == 4:
    if int(sys.argv[1])==0:
        beast=False
    if int(sys.argv[2])==0:
        show=False
    imageType=sys.argv[3]
    
    
if beast:
    print "Generating beast style figures in "+imageType+" format"
else:
    print "Generating custom style figures in "+imageType+" format"


# In[3]:

#read the xml data into a pandas dataframe
Data=[]
PS=[]
for Detector in root:
    det=""
    if Detector.tag=="LYSO_hitRate_forward":
        det="Forward LYSO"
    elif Detector.tag=="CSI_Pure_hitRate_forward":
        det="Forward Pure CSI"
    elif Detector.tag=="LYSO_hitRate_backward":
        det="Backward LYSO"
    elif Detector.tag=="CSI_Pure_hitRate_backward":
        det="Backward Pure CSI"
    elif Detector.tag=="BGO_dose_forward":
        det="Forward BGO"        
    elif Detector.tag=="BGO_dose_backward":
        det="Backward BGO"
    elif Detector.tag=="HE3_rate":
        det="$^3He$ tube"
    elif Detector.tag=="DIA_dose":
        det="Diamond"
    elif Detector.tag=="PIN_dose":
        det="PIN"
    elif Detector.tag=="SCI_rate":
        det="Scintillator"
    elif Detector.tag=="CLW_N_MIPs_offline":
        det="CLAWS"
    else:
        det=Detector.tag
    
        
    ch=Detector.attrib['value']    

    for Beam in Detector:
        value = float(Beam.attrib['value'])
        sumup=0
        sumdown=0
        for Systematic in Beam:
            #Get PScale values
            #print Systematic.tag
            if Systematic.tag=="PScaleVal":
                #print Systematic.attrib['value']
                PS.append([Beam.tag, det, float(Systematic.attrib['value']), float(Systematic.text)])
                continue
                
            #print         Systematic.find('down').text
            down=float(Systematic.find('down').text)
            up= float(Systematic.find('up').text)
                
            if down > 0:
                temp=up
                up=down
                down=temp
                    
            sumup+=up**2
            sumdown+=down**2
                                        
                #value= float(BGSource.attrib['value'])
            
        if sumdown==0 or sumup==0:
            value=1e-10
            sumup=0
            sumdown=0
        elif value/math.sqrt(sumdown)<1.03 or value/math.sqrt(sumup)<1.02:
            value=1e-10
            sumup=0
            sumdown=0
                
                
            
        Data.append([Beam.tag.replace('_', ' '), det, ch, value, math.sqrt(sumdown), math.sqrt(sumup)])
           
        
#create the dataframe                
HERLER = pd.DataFrame(Data, columns=['Beam', 'Detector', 'Channel', 'ratio', 'lowerror', 'uppererror'])

if show:
    print HERLER

#Create a PScale dataframe
PScale = pd.DataFrame(PS, columns=['Beam', 'Detector', 'PScale', 'Error']).drop_duplicates()

if show:
    print PScale


# In[4]:

#the font I'm using doesn't have a character for ^-, so I have to redefine how the axis is labeled

from matplotlib.ticker import FuncFormatter

def labeller(x, pos):
    #if int(np.log10(x))==0:
    #    return str(int(1))
    return '10$^{'+str(int(np.log10(x)))+'}$'

def neglabeller(x,pos):
    if x < 0:
        return "-"+str(-x)
    return(str(x))
    


# In[8]:



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
#HERColour = "#C92630"
#LERColour = "#3C7DC4"

HERColour = (201.0,  #R
             38.0,   #G
             48.0)   #B
LERColour = (60.0,
             125.0,
             196.0)

#the beast colours
if beast:
    HERColour = (0.0,
                 0.0,
                 255.0)
    LERColour = (255.0,
                 0.0,
                 0.0)

    
HERColour = np.divide(HERColour, 255)
LERColour = np.divide(LERColour, 255)    
    


# In[6]:


#Plot Data/Sim for each detector


#loop over the beams

    
plt.figure(figsize=(800/80, 500/80))

ax = plt.subplot(111)
        
Hoffset=-0.1875*1.5
Loffset=0.1875*1.5
    

xlabel='$\mathcal{O}^{data}/\mathcal{O}^{sim}$'

        
for beam in ['HER', 'LER']:
    
    HFrame=HERLER.loc[lambda df: (df.Beam == beam), :] 
    
        
    
    c=HERColour
    m='^'
    o=Hoffset
    if beam == 'LER':
        c=LERColour
        m='o'
        o=Loffset
        
    triplet = [int(i) for i in np.multiply(c, 255)]
    hexC = '#'+''.join(map(chr, triplet)).encode('hex')     
        
    #plot data
    #sns.stripplot(x="ratio", y="Detector", hue="Beam", data=AFrame, jitter=False, split=True, size=10, marker='o', edgecolor=['#FF0000','#0000FF'], linewidth=1)
    sns.stripplot(HFrame.ratio, HFrame.Detector , jitter=False, size=15, marker=m,edgecolor=hexC, linewidth=1)
  
    for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
        f = HFrame['Detector'] == ylabel.get_text() 
        ax.errorbar(HFrame.ratio[f].values, 
                    np.ones_like(HFrame.ratio[f].values)*(y+o), 
                    xerr=[HFrame.lowerror[f].values, HFrame.uppererror[f].values], 
                    alpha=.25,
                    elinewidth=15,
                    capthick=0,
                    color=c,
                    linewidth=0)

        
for point in ax.collections:
    point.set_facecolor('none')
           
    offsets=point.get_offsets()
    o=Loffset
    
    eq=np.equal(point.get_edgecolors()[0][0:3], HERColour)
    
    if np.all(eq):
        o=Hoffset
       
    for i in range(0,len(offsets)):
        offsets[i][1] = offsets[i][1]+o

    point.set_offsets(offsets)
        

        

#set axis titles
plt.ylabel('')
plt.xlabel(xlabel, horizontalalignment='right', x=1.0)
        
    
#set log scale on x
ax.set_xscale("log", nonposx='clip')

ax.set_xlim(0.01, 1e9)
ax.set_ylim(10.75, -0.75)
        
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


# In[ ]:




# In[7]:

noColor=True

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
        
    c=HERColour
    m='^'
    o=Hoffset
    if beam == 'LER':
        c=LERColour
        m='o'
        o=Loffset
            
    triplet = [int(i) for i in np.multiply(c, 255)]

    hexC = '#'+''.join(map(chr, triplet)).encode('hex')         

    #plot data
    if noColor:
        sns.stripplot(Frame.PScale, Frame.Detector , jitter=False, size=10, color='black', linewidth=1)
        c=[0,0,0]
    else:
        sns.stripplot(Frame.PScale, Frame.Detector , jitter=False, size=10, marker=m, edgecolor=hexC, color='white', linewidth=1)

    
    #add error abrs
    for y,ylabel in zip(ax.get_yticks(), ax.get_yticklabels()):
        f = Frame['Detector'] == ylabel.get_text() 
        ax.errorbar(Frame.PScale[f].values, 
                    np.ones_like(Frame.PScale[f].values)*(y), 
                    xerr=[Frame.Error[f].values, Frame.Error[f].values], 
                    #ls='none', 
                    elinewidth=2,
                    capthick=2,
                    color=c)

    #set axis titles
    plt.ylabel(beam)
    xlab = "$P_{Scale}$"
    if beam == 'HER':
        xlab = xlab+"$Z_{e}^{2}$"
    plt.xlabel(xlab)


    #set ticks pointing in
    ax.tick_params(direction='in', pad=15)
    ax.tick_params(which='minor', direction='in', pad=15)
        
    plt.tick_params(axis='y',          # changes apply to the x-axis
                which='minor',      # both major and minor ticks are affected
                left='off',      # ticks along the bottom edge are off
                right='off') # labels along the bottom edge are off   
        
        
    #don't add grid if belle style is requested
    if beast==False:
        ax.grid(True)
        
        gridlines = ax.get_xgridlines() + ax.get_ygridlines()
        for line in gridlines:
            line.set_linestyle('dotted')
            line.set_color('black')
       
    plt.tight_layout()
    
    #custom_formatter = FuncFormatter(neglabeller)
    #ax.xaxis.set_major_formatter(custom_formatter)
    
    large =  max(np.add(Frame.PScale.values, Frame.Error.values))
    ax.set_xlim(0,large*1.1)
    
    #save plot
    plt.savefig(PlotDir+'/'+beam+'PScale.'+imageType)
              
    #show plot
if show:
    sns.plt.show()







# In[ ]:




# In[ ]:



