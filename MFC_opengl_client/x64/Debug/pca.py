# -*- coding: utf-8 -*-
"""
Created on Tue Apr 26 21:11:53 2022

@author: labpc
"""

import pandas as pd
from sklearn.preprocessing import StandardScaler  # 표준화 패키지 라이브러리 
from sklearn.decomposition import PCA
import csv
import matplotlib.pyplot as plt

pd_data = pd.read_csv("C://Users/labpc/Desktop/ConsoleApplication1/data/Kinect_220427_123823.csv")

pd_data_1 = pd_data.loc[[0]]

index_1_list = pd_data_1.values[0].tolist()

temp = [[0 for col in range(3)] for row in range(9)]

n = 1

for i in range(0, 9):
    for k in range(0, 3):
        temp[i][k] = index_1_list[n]
        n += 1
col_name = ['x', 'y', 'z']

df_temp = pd.DataFrame(temp, columns=col_name)

x = StandardScaler().fit_transform(df_temp) # x객체에 x를 표준화한 데이터를 저장
pca = PCA(n_components=2)
printcipalComponents = pca.fit_transform(x)
principalDf = pd.DataFrame(data=printcipalComponents, columns = ['x', 'y'])

plt.scatter(principalDf['x'],principalDf['y'])
plt.show()

###########################################################################전체 데이터
temp2 = [[-225.287369, 112.056854],
         [-226.605133, 112.734703],
         [-208.933197, 103.962708],
         [-134.184784, 66.728233],
          [-118.378067, 58.864017],
          [-211.432922, 105.141052],
          [-188.594376, 93.745667],
          [-120.990784, 60.085762],
          [-106.071075, 52.664368]]

col_name2 = ['x', 'y']
df_temp2 = pd.DataFrame(temp2, columns=col_name2)

df_temp3 = StandardScaler().fit_transform(df_temp2) # x객체에 x를 표준화한 데이터를 저장

df_temp3 = pd.DataFrame(df_temp3, columns=['x', 'y'])

plt.scatter(df_temp3['x'],df_temp3['y'])
plt.show()

###########################################################################전체 데이터

x = pd_data.drop(['TIME'], axis = 1).values
y = pd_data['TIME'].values # 종속변인 추출
x = StandardScaler().fit_transform(x) # x객체에 x를 표준화한 데이터를 저장

features = [ 'NECK_x','NECK_y','NECK_z','SHOULDER_LEFT_x','SHOULDER_LEFT_y','SHOULDER_LEFT_z',	
            'ELBOW_LEFT_x',	 'ELBOW_LEFT_y',	 'ELBOW_LEFT_z',	 
            'WRIST_LEFT_x',	 'WRIST_LEFT_y',	 'WRIST_LEFT_z',	 
            'HAND_LEFT_x',	 'HAND_LEFT_y',	 'HAND_LEFT_z',	
            'SHOULDER_RIGHT_x',	 'SHOULDER_RIGHT_y',	 'SHOULDER_RIGHT_z',	
            'ELBOW_RIGHT_x',	 'ELBOW_RIGHT_y',	 'ELBOW_RIGHT_z',	 
            'WRIST_RIGHTT_x',	 'WRIST_RIGHT_y',	 'WRIST_RIGHT_z',	 
            'HAND_RIGHT_x',	 'HAND_RIGHT_y',	 'HAND_RIGHT_z']

pd.DataFrame(x, columns=features).head()

pca = PCA(n_components=2)
printcipalComponents = pca.fit_transform(x)
principalDf = pd.DataFrame(data=printcipalComponents, columns = ['principal component1', 'principal component2'])


# url = "https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data"
# df = pd.read_csv(url, names=['sepal length','sepal width','petal length','petal width','target'])
# df.head()

# x = df.drop(['target'], axis=1).values # 독립변인들의 value값만 추출
# y = df['target'].values # 종속변인 추출

# x = StandardScaler().fit_transform(x) # x객체에 x를 표준화한 데이터를 저장

# features = ['sepal length', 'sepal width', 'petal length', 'petal width']
# pd.DataFrame(x, columns=features).head()

# pca = PCA(n_components=2) # 주성분을 몇개로 할지 결정
# printcipalComponents = pca.fit_transform(x)
# principalDf = pd.DataFrame(data=printcipalComponents, columns = ['principal component1', 'principal component2'])
# # 주성분으로 이루어진 데이터 프레임 구성

# principalDf.head()
# val_list = principalDf.values.tolist()

