
# coding: utf-8

# In[50]:

from scipy.spatial import Delaunay
import pandas as pd
import numpy as np
import sys


# In[51]:

#points = np.array([[0,24],[6,14],[13,30],[16,0],[22,18],[23,11],[29,31],[39,15]])
#print sys.argv
if len(sys.argv) < 3:
    print "Usage: python validator.py <points_file> <edges_file>"
    sys.exit(1)
else:
    pointsFileName = sys.argv[1]
    edgesFileName = sys.argv[2]


# In[49]:

data = pd.read_csv(pointsFileName, header=None)

points = data.as_matrix()[:,1:3]

dt = Delaunay(points)


# In[37]:

#import matplotlib.pyplot as plt

#plt.triplot(points[:,0], points[:,1], dt.simplices.copy())
#plt.plot(points[:,0], points[:,1], 'o')
#plt.show()


# In[38]:

edges = []
bad = []
for t in dt.simplices:
    edges.append(str(t[0])+" "+str(t[1]))
    edges.append(str(t[1])+" "+str(t[0]))
    edges.append(str(t[1])+" "+str(t[2]))
    edges.append(str(t[2])+" "+str(t[1]))
    edges.append(str(t[0])+" "+str(t[2]))
    edges.append(str(t[2])+" "+str(t[0]))


# In[39]:

f = open(edgesFileName, "r")
lines = f.readlines()
f.close()


# In[47]:

for l in lines:
    if l.strip('\n') not in edges:
        bad.append(l)

print "Delunay Triangulation Verification Status:"

if len(bad) == 0:
    print "All Good!"
else:
    print "Following edges are not expected"
    for e in bad:
        print e


# In[ ]:



