
# coding: utf-8

# In[33]:

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys


# In[34]:

if len(sys.argv) < 3:
    print "Usage: python point-generator.py <num_of_points_to_generate> <output_file>"
    sys.exit(1)

size = long(sys.argv[1])
outputFileName = sys.argv[2]


# In[35]:

x = np.random.uniform(0.0, size*2*100, size*2)
y = np.random.uniform(0.0, size*2*100, size*2)

x = np.sort(x)


# In[36]:

x, idx = np.unique(x, return_index=True)
y = y[idx]

x = x[0:size]
y = y[0:size]


# In[38]:

data = pd.DataFrame(data=[x,y])


# In[41]:

data.T.to_csv(outputFileName, header=False)


# In[ ]:



