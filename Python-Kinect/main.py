# -*- coding: utf-8 -*-
"""
Created on Mon Mar 28 13:24:34 2022

@author: labpc
"""
import serial, threading
from multiprocessing import Process
import multiprocessing
from viewer import color_view

import time as timmer

from tkinter import ttk
import tkinter as tk # Tkinter

import numpy as np
from pandas import DataFrame
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import quaternion # pip install numpy numpy-quaternion numba
from spatialmath import *
from spatialmath.pose3d import *

from matplotlib.animation import FuncAnimation

from sympy import Integral, Symbol

def calculate_vel(previous_speed, present_acc, previous_acc, present_time, previous_time):
            
    velocity = previous_speed + 0.5*(present_acc + previous_acc) * (present_time - previous_time)
        
    return velocity

def draw_gui(imu_0_data, time_0_data, imu_0_vel, time_0_vel, imu_0_dis, time_0_dis,
             imu_1_data, time_1_data, imu_1_vel, time_1_vel, imu_1_dis, time_1_dis):
    
    width = 4; height = 1.5;
    
    win = tk.Tk()
    
    win.title("Kinect") # 제목 표시줄 추가
    win.geometry("1000x1200+50+50") # 지오메트리: 너비x높이+x좌표+y좌표
    win.resizable(False, False) # x축, y축 크기 조정 비활성화
    
    #imu sensor viewer 프레임 선언 그리드로 분류
    imu_0_viewer_frame = tk.Frame(win)
    imu_0_viewer_frame.grid(row=0, column=0)
    
    imu_1_viewer_frame = tk.Frame(win)
    imu_1_viewer_frame.grid(row=0, column=1)
    
#################################################################################################    
    #imu 센서 데이터 프레임 선언 그리드로 분류
    imu_0_data_frame = tk.Frame(win)
    imu_0_data_frame.grid(row=1, column=0)
    
    imu_1_data_frame = tk.Frame(win)
    imu_1_data_frame.grid(row=1, column=1)
       
    # 라벨 추가
    lbl1 = tk.Label(imu_0_data_frame, text="imu_0 data")
    lbl1.pack(side="top", fill="both")
    
    lbl2 = tk.Label(imu_1_data_frame, text="imu_1 data")
    lbl2.pack(side="top", fill="both")    

    # ﻿데이터 출력 창
    listbox_imu_0 = tk.Listbox(imu_0_data_frame, selectmode="extended", width=40, height = 1)
    listbox_imu_0.pack(side="left", fill="both")
    
    listbox_imu_1 = tk.Listbox(imu_1_data_frame, selectmode="extended", width=40, height = 1)
    listbox_imu_1.pack(side="left", fill="both")
    
    #스크롤바 생성
    imu_0_scroll = ttk.Scrollbar(imu_0_data_frame, orient="vertical", command=listbox_imu_0.yview)
    imu_0_scroll.pack(side="right", fill="y") # 라벨 행, 열 배치
    
    imu_1_scroll = ttk.Scrollbar(imu_1_data_frame, orient="vertical", command=listbox_imu_1.yview)
    imu_1_scroll.pack(side="right", fill="y") # 라벨 행, 열 배치
    
    #생성된 스크롤바 적용
    listbox_imu_0.configure(yscrollcommand=imu_0_scroll.set)
    listbox_imu_1.configure(yscrollcommand=imu_1_scroll.set)
    
#################################################################################################      
    #imu 0 시각화 부분
    imu_0_viewer = plt.figure(figsize=(width,2.5), dpi=100)    
    imu0 = imu_0_viewer.add_subplot(111, projection='3d')
    
    imu0.set_xlim(-0.5, 0.5)
    imu0.set_ylim(-0.5, 0.5)
    imu0.set_zlim(0, 1)
    
    x0 = [imu0.quiver(0, 0, 0, 0, 0, 0), imu0.quiver(0, 0, 0, 0, 0, 0), imu0.quiver(0, 0, 0, 0, 0, 0)]
        
    colors = ['#E16F6D', '#4eb604', '#0d87c3']
    
    line0 = FigureCanvasTkAgg(imu_0_viewer, imu_0_viewer_frame)#프레임에 출력한다고 선언  
    line0.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)
    
    def imu_0_update(count):
        #count가 존재하지 않으면 핸들값이 없다고 나옴
        #No handles with labels found to put in legend.
        qq0 = UnitQuaternion([float(imu_0_data[-1].split(',')[1]), float(imu_0_data[-1].split(',')[2]),float(imu_0_data[-1].split(',')[3]),float(imu_0_data[-1].split(',')[4])]).R #아마도 Rotation
        
        for i in range(3):
            x0[i].remove()
            x0[i] = imu0.quiver(0, 0, 0.5, qq0[0][i], qq0[1][i], qq0[2][i], length=0.5, normalize=True, color=colors[i])
            imu0.set_title('imu 0')         
    
    imu_0_ani = FuncAnimation(imu_0_viewer, imu_0_update, frames = 200, interval=50)
    
    
#################################################################################################      
    #imu 1 시각화 부분
    imu_1_viewer = plt.figure(figsize=(width, 2.5), dpi=100)    
    imu1 = imu_1_viewer.add_subplot(111, projection='3d')
    
    imu1.set_xlim(-0.5, 0.5) #뷰어 설정
    imu1.set_ylim(-0.5, 0.5)
    imu1.set_zlim(0, 1)
    
    x1 = [imu1.quiver(0, 0, 0, 0, 0, 0), imu1.quiver(0, 0, 0, 0, 0, 0), imu1.quiver(0, 0, 0, 0, 0, 0)]
        
    colors = ['#E16F6D', '#4eb604', '#0d87c3']
    
    line1 = FigureCanvasTkAgg(imu_1_viewer, imu_1_viewer_frame)#프레임에 출력한다고 선언  
    line1.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)
    
    def imu_1_update(count):
        
        qq1 = UnitQuaternion([float(imu_1_data[-1].split(',')[1]), float(imu_1_data[-1].split(',')[2]),float(imu_1_data[-1].split(',')[3]),float(imu_1_data[-1].split(',')[4])]).R #아마도 Rotation
        #w, x, y, z 값을 
        
        for i in range(3):
            x1[i].remove()
            x1[i] = imu1.quiver(0, 0, 0.5, qq1[0][i], qq1[1][i], qq1[2][i], length=0.5, normalize=True, color=colors[i])
            #quiver 화살표 스케일링 해주는 함수 
            #quiver(x시작점, y시작점, z시작점(z값이 0인 경우에는 바닥에 붙어서 나옴), X, Y, Z 값)
            imu1.set_title('imu 1')         
    
    imu_1_ani = FuncAnimation(imu_1_viewer, imu_1_update, frames = 200, interval=50)
    #animator 효과를 주기 위한 함수

#################################################################################################  

    def list_insert():
        listbox_imu_0.insert(tk.END, (imu_0_data[-1].split(',')[1:5]));
        listbox_imu_0.see(tk.END) #스크롤바가 자동으로 내려가게 하는 함수 tk.END
        
        listbox_imu_1.insert(tk.END, (imu_1_data[-1].split(',')[1:5]));
        listbox_imu_1.see(tk.END)
            
        win.after(100, list_insert)
            
    win.after(1000, list_insert) #윈도우 창 생성후 1초 뒤 부터 값을 불러올것임. 그렇지 않으면 무한 루프에 들어가기 때문
    
    
#################################################################################################    
    #imu 센서 데이터 프레임 선언(가속도) 그리드로 분류
    imu_0_acceleration_frame = tk.Frame(win)
    imu_0_acceleration_frame.grid(row=2, column=0)
    
#################################################################################################      
    # #imu 0 가속도 시각화 부분
    imu_0_acceleration_viewer = plt.figure(figsize=(width, height), dpi=100)    
    imu0_acceleration = imu_0_acceleration_viewer.add_subplot(111)

    imu0_acceleration.set_ylim(-1.0, 1.0)
        
    imu0_acceleration_line = FigureCanvasTkAgg(imu_0_acceleration_viewer, imu_0_acceleration_frame)#프레임에 출력한다고 선언  
    imu0_acceleration_line.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)
    
    imu_0_accerlation_data_time = []
    imu_0_accerlation_data_x = []
    imu_0_accerlation_data_y = []    
    imu_0_accerlation_data_z = []
    
    def imu_0_acceleration_update(count):
        imu_0_accerlation_data_time.append(time_0_data[-1])
        
        if len(time_0_data) > 1:
            imu_0_accerlation_data_x.append(float(imu_0_data[-1].split(',')[6]))
            imu_0_accerlation_data_y.append(float(imu_0_data[-1].split(',')[5]))        
            imu_0_accerlation_data_z.append(float(imu_0_data[-1].split(',')[7]))
            
            imu0_acceleration.cla()
            
            imu0_acceleration.plot(imu_0_accerlation_data_time[-100:-1], imu_0_accerlation_data_x[-100:-1], color = '#E16F6D')
            imu0_acceleration.plot(imu_0_accerlation_data_time[-100:-1], imu_0_accerlation_data_y[-100:-1], color = '#4eb604')
            imu0_acceleration.plot(imu_0_accerlation_data_time[-100:-1], imu_0_accerlation_data_z[-100:-1], color = '#0d87c3')
        
        # imu0_acceleration.legend(loc = 'upper left')
        
            imu0_acceleration.set_title('imu 0_acceleration')
            
    imu_0_acceleration_ani = FuncAnimation(imu_0_acceleration_viewer, imu_0_acceleration_update, frames = 200, interval=50)
    
################################################################################################   
    #imu 센서 데이터 프레임 선언(가속도) 그리드로 분류
    
    imu_1_acceleration_frame = tk.Frame(win)
    imu_1_acceleration_frame.grid(row=2, column=1)
    
#################################################################################################      
    # #imu 0 가속도 시각화 부분
    imu_1_acceleration_viewer = plt.figure(figsize=(width, height), dpi=100)    
    imu1_acceleration = imu_1_acceleration_viewer.add_subplot(111)
        
    imu1_acceleration_line = FigureCanvasTkAgg(imu_1_acceleration_viewer, imu_1_acceleration_frame)#프레임에 출력한다고 선언  
    imu1_acceleration_line.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)
    
    imu_1_accerlation_data_time = []
    imu_1_accerlation_data_x = []
    imu_1_accerlation_data_y = []    
    imu_1_accerlation_data_z = []
    
    def imu_1_acceleration_update(count):
        imu_1_accerlation_data_time.append(time_1_data[-1])
        
        if len(time_1_data) > 1:
            imu_1_accerlation_data_x.append(float(imu_1_data[-1].split(',')[6]))
            imu_1_accerlation_data_y.append(float(imu_1_data[-1].split(',')[5]))        
            imu_1_accerlation_data_z.append(float(imu_1_data[-1].split(',')[7]))        
                    
            imu1_acceleration.cla()
            
            imu1_acceleration.plot(imu_1_accerlation_data_time[-100:-1], imu_1_accerlation_data_x[-100:-1], color = 'blue')
            imu1_acceleration.plot(imu_1_accerlation_data_time[-100:-1], imu_1_accerlation_data_y[-100:-1], color = 'red')
            imu1_acceleration.plot(imu_1_accerlation_data_time[-100:-1], imu_1_accerlation_data_z[-100:-1], color = 'green')        
            
            # imu1_acceleration.legend(loc = 'upper left')
            
            imu1_acceleration.set_title('imu 1_acceleration')
    
    imu_1_acceleration_ani = FuncAnimation(imu_1_acceleration_viewer, imu_1_acceleration_update, frames = 200, interval=50)
    
################################################################################################   
  #imu 센서 데이터 프레임 선언(속도) 그리드로 분류
    imu_0_velocity_frame = tk.Frame(win)
    imu_0_velocity_frame.grid(row=3, column=0)   
    
##################################################################################################
    #imu 0 속도 시각화 부분 
    imu_0_velocity_viewer = plt.figure(figsize=(width, height), dpi=100)    
    imu0_velocity = imu_0_velocity_viewer.add_subplot(111)
    
    imu0_velocity.set_ylim([-1.0, 1.0])
        
    imu0_velocity_line = FigureCanvasTkAgg(imu_0_velocity_viewer, imu_0_velocity_frame)#프레임에 출력한다고 선언  
    imu0_velocity_line.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)    
    
    imu0_velocity_data_time = []
    imu0_velocity_data_x = []
    imu0_velocity_data_y = []    
    imu0_velocity_data_z = []
    
    def imu_0_velocity_update(count):             
        imu0_velocity_data_time.append(time_0_vel[-1])        
        
        if len(imu0_velocity_data_x) < 1 and len(imu0_velocity_data_time) > 1:      
            imu0_velocity_data_x.append(calculate_vel(0, float(imu_0_vel[-1].split(',')[6]), float(imu_0_vel[-2].split(',')[6]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu0_velocity_data_y.append(calculate_vel(0, float(imu_0_vel[-1].split(',')[5]), float(imu_0_vel[-2].split(',')[5]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu0_velocity_data_z.append(calculate_vel(0, float(imu_0_vel[-1].split(',')[7]), float(imu_0_vel[-2].split(',')[7]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu_0_dis.append([imu0_velocity_data_x[-1], imu0_velocity_data_y[-1], imu0_velocity_data_z[-1]])            
            
        if len(imu0_velocity_data_x) > 0 and len(imu0_velocity_data_time) > 0:
            
            imu0_velocity_data_x.append(calculate_vel(imu0_velocity_data_x[-1], float(imu_0_vel[-1].split(',')[6]), float(imu_0_vel[-2].split(',')[6]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu0_velocity_data_y.append(calculate_vel(imu0_velocity_data_y[-1], float(imu_0_vel[-1].split(',')[5]), float(imu_0_vel[-2].split(',')[5]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu0_velocity_data_z.append(calculate_vel(imu0_velocity_data_z[-1], float(imu_0_vel[-1].split(',')[7]), float(imu_0_vel[-2].split(',')[7]), 
                                                      imu0_velocity_data_time[-1], imu0_velocity_data_time[-2]))
            
            imu_0_dis.append([imu0_velocity_data_x[-1], imu0_velocity_data_y[-1], imu0_velocity_data_z[-1]])
            
        imu0_velocity.cla()
        
        imu0_velocity.plot(imu0_velocity_data_time[-100:-1], imu0_velocity_data_x[-100:-1], color = '#E16F6D')
        imu0_velocity.plot(imu0_velocity_data_time[-100:-1], imu0_velocity_data_y[-100:-1], color = '#4eb604')
        imu0_velocity.plot(imu0_velocity_data_time[-100:-1], imu0_velocity_data_z[-100:-1], color = '#0d87c3')
        
        # imu0_velocity.legend(loc = 'upper left')
        
        imu0_velocity.set_title('imu 0 velocity')
        imu0_velocity.title.set_size(5)
            
    imu_0_velocity_ani = FuncAnimation(imu_0_velocity_viewer, imu_0_velocity_update, frames = 200, interval=50)

    
# #################################################################################################   
  #imu 센서 데이터 프레임 선언(속도) 그리드로 분류
    imu_1_velocity_frame = tk.Frame(win)
    imu_1_velocity_frame.grid(row=3, column=1)   
    
# #################################################################################################
    #imu 0 속도 시각화 부분 
    imu_1_velocity_viewer = plt.figure(figsize=(width, height), dpi=100)    
    imu1_velocity = imu_1_velocity_viewer.add_subplot(111)
    
    imu1_velocity.set_ylim([-1.0, 1.0])
        
    imu1_velocity_line = FigureCanvasTkAgg(imu_1_velocity_viewer, imu_1_velocity_frame)#프레임에 출력한다고 선언  
    imu1_velocity_line.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)    
    
    imu1_velocity_data_time = []
    imu1_velocity_data_x = []
    imu1_velocity_data_y = []    
    imu1_velocity_data_z = []
    
    def imu_1_velocity_update(count):             
        imu1_velocity_data_time.append(time_1_vel[-1])
        
        if len(imu1_velocity_data_x) < 1 and len(imu1_velocity_data_time) > 1:
            imu1_velocity_data_x.append(calculate_vel(0, float(imu_1_vel[-1].split(',')[6]), float(imu_1_vel[-2].split(',')[6]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
            
            imu1_velocity_data_y.append(calculate_vel(0, float(imu_1_vel[-1].split(',')[5]), float(imu_1_vel[-2].split(',')[5]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
            
            imu1_velocity_data_z.append(calculate_vel(0, float(imu_1_vel[-1].split(',')[7]), float(imu_1_vel[-2].split(',')[7]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
           
            imu_1_dis.append([imu1_velocity_data_x[-1], imu1_velocity_data_y[-1], imu1_velocity_data_z[-1]])     
    
        if len(imu1_velocity_data_x) > 0 and len(imu1_velocity_data_time) > 0:
            imu1_velocity_data_x.append(calculate_vel(imu1_velocity_data_x[-1], float(imu_1_vel[-1].split(',')[6]), float(imu_1_vel[-2].split(',')[6]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
            
            imu1_velocity_data_y.append(calculate_vel(imu1_velocity_data_y[-1], float(imu_1_vel[-1].split(',')[5]), float(imu_1_vel[-2].split(',')[5]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
            
            imu1_velocity_data_z.append(calculate_vel(imu1_velocity_data_z[-1], float(imu_1_vel[-1].split(',')[7]), float(imu_1_vel[-2].split(',')[7]), 
                                                      imu1_velocity_data_time[-1], imu1_velocity_data_time[-2]))
            
            imu_1_dis.append([imu1_velocity_data_x[-1], imu1_velocity_data_y[-1], imu1_velocity_data_z[-1]])     

        imu1_velocity.cla()
        
        imu1_velocity.plot(imu1_velocity_data_time[-100:-1], imu1_velocity_data_x[-100:-1], color = '#E16F6D')
        imu1_velocity.plot(imu1_velocity_data_time[-100:-1], imu1_velocity_data_y[-100:-1], color = '#4eb604')
        imu1_velocity.plot(imu1_velocity_data_time[-100:-1], imu1_velocity_data_z[-100:-1], color = '#0d87c3')
        
        # imu1_velocity.legend(loc = 'upper left')
        
        imu1_velocity.set_title('imu 1 velocity')
            
    imu_1_velocity_ani = FuncAnimation(imu_1_velocity_viewer, imu_1_velocity_update, frames = 200, interval=50)

    
################################################################################################# 
    #imu 센서 데이터 프레임 선언(방향) 그리드로 분류
#     imu_0_dis_frame = tk.Frame(win)
#     imu_0_dis_frame.grid(row=4, column=0)   
    
# ###################################################################################################
#     #imu 0 속도 시각화 부분 
#     imu_0_dis_viewer = plt.figure(figsize=(width, 2.5), dpi=100)
#     imu0_dis = imu_0_dis_viewer.add_subplot(111)    
#     # imu0_dis = imu_0_dis_viewer.add_subplot(111, projection='3d')
    
#     # imu0_dis.set_xlim(-0.5, 0.5)
#     # imu0_dis.set_ylim(-0.5, 0.5)
#     # imu0_dis.set_zlim(0, 1)
        
#     imu0_dis_line = FigureCanvasTkAgg(imu_0_dis_viewer, imu_0_dis_frame)#프레임에 출력한다고 선언  
#     imu0_dis_line.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH)    
    
#     imu0_dis_data_time = []
#     imu0_dis_data_x = []
#     imu0_dis_data_y = []    
#     imu0_dis_data_z = []
    
#     def imu_0_dis_update(count):             
#         imu0_dis_data_time.append(time_0_dis[-1])        
        
#         if len(imu0_dis_data_x) < 1 and len(imu0_dis_data_time) > 1:      
#             imu0_dis_data_x.append(calculate_vel(0, float(imu_0_dis[-1][0]), float(imu_0_dis[-2][0]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
            
#             imu0_dis_data_y.append(calculate_vel(0, float(imu_0_dis[-1][1]), float(imu_0_dis[-1][1]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
            
#             imu0_dis_data_z.append(calculate_vel(0, float(imu_0_dis[-1][2]), float(imu_0_dis[-1][2]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
             
            
#         if len(imu0_dis_data_x) > 0 and len(imu0_dis_data_time) > 0:
            
#             imu0_dis_data_x.append(calculate_vel(imu0_dis_data_x[-1], float(imu_0_dis[-1][0]), float(imu_0_dis[-2][0]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
            
#             imu0_dis_data_y.append(calculate_vel(imu0_dis_data_y[-1], float(imu_0_dis[-1][1]), float(imu_0_dis[-2][1]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
            
#             imu0_dis_data_z.append(calculate_vel(imu0_dis_data_z[-1], float(imu_0_dis[-1][2]), float(imu_0_dis[-2][2]), float(imu0_dis_data_time[-1]), float(imu0_dis_data_time[-2])))
                        
#         imu0_dis.cla()
        
#         imu0_dis.scatter(imu0_dis_data_x, imu0_dis_data_y, marker='o', s=15, c='darkgreen')
#         # imu0_dis.scatter(imu0_dis_data_x, imu0_dis_data_y, imu0_dis_data_z,  marker='o', s=15, c='darkgreen')
        
#         # imu0_dis.legend(loc = 'upper left')
        
#         imu0_dis.set_title('imu 0 dis')
            
#     imu_0_dis_ani = FuncAnimation(imu_0_dis_viewer, imu_0_dis_update, frames = 200, interval=50) 

    # fig = plt.figure(figsize=(5, 5))
    # ax = fig.gca(projection='3d')
    
    # for i in range(0, len(imu_0_dis)):        
    #     ax.scatter(imu_0_dis[i][0], imu_0_dis[i][1], imu_0_dis[i][2], marker='o', s=15, c='darkgreen')
    
    # plt.show()
        
#################################################################################################  

    win.mainloop() #GUI 시작

################################################################################################# 

def serial_load(imu_0_data, time_0_data, imu_0_vel, time_0_vel, imu_0_dis, time_0_dis,
                imu_1_data, time_1_data, imu_1_vel, time_1_vel, imu_1_dis, time_1_dis):
    #시리얼 사용을 위해 정의
    ser = serial.Serial() # 시리얼을 연결한다.
    
    ser.port = 'COM3' # 아두이노가 연결된 포트
    ser.baudrate = 1000000 # 해당 IMU 센서는 budrate 1000000으로 설정해줘야 값이 나온다.
        
    ser.timeout = 0.01 #시리얼에 데이터를 불러올 때 지정하는 딜레이
    
    # 시리얼을 열어준다.
    ser.open()
    
    if ser.isOpen() == True:
        print("Opend")
    else:
        print("Falsed")
    
    # 반복해서 데이터를 출력하기 위해 while 을 만들어주었다.
    start = 0
    start = timmer.time()
    while True:        
        if ser.readable():
            original_data = ser.readline() # serial에 출력되는 데이터를 읽어준다.
            str_data = original_data.decode('utf-8')
            
            if len(str_data) > 50: 
                #간혹 가다 데이터가 0인 경우가 있음. (데이터 손실)
                imu_num = str_data.split(',')[0]
                
                if imu_num == '100-0':
                    imu_0_data.append(str_data)     #100뒤의 숫자가 Imu 센서 번호에 해당한다
                    time_0_data.append(float((round((timmer.time() - start), 3))))
                   
                    imu_0_vel.append(str_data)
                    time_0_vel.append(float((round((timmer.time() - start), 3))))
                    
                    time_0_dis.append(float((round((timmer.time() - start), 3))))
                    # 0 , 1, 2, 3, 4, 5, 6, 7, 8                     
                    # id, w, x, y, z, x, y, z, battery
                    # w,x,y,z 각속도 센서, 가속도 센서, 지자기 센서의 데이터를 모두 연산하여 최종 자세 데이터 출력
                        
                if imu_num == '100-1':                    
                    imu_1_data.append(str_data)
                    time_1_data.append(float((round((timmer.time() - start), 3))))
                    
                    imu_1_vel.append(str_data)
                    time_1_vel.append(float((round((timmer.time() - start), 3))))
                
                # if len(imu_0_data) > 1000 and len(imu_1_data) > 1000:
                #     print("time :", timmer.time() - start)
                #     break
        
                
            
        else:
            print("Serial destoied")
            ser.close()
            
        
    ser.close() # serial 사용이 끝나면 닫아줘야 나중에 오류가 생기지 않는다고 한다.
    
   
if __name__ == '__main__': 
    #imu sensor data 저장 클래스
    #shared memory
    manager = multiprocessing.Manager()
    

################################################################################################# 
    imu_0_data = manager.list()#각속도
    time_0_data = manager.list()
    
    imu_0_vel = manager.list()#
    time_0_vel = manager.list()    
    
    imu_0_dis = manager.list() #속도값
    time_0_dis = manager.list() 

#################################################################################################   
    imu_1_data = manager.list()
    time_1_data = manager.list()
    
    imu_1_vel = manager.list()
    time_1_vel = manager.list()    
    
    imu_1_dis = manager.list()
    time_1_dis = manager.list() 

################################################################################################# 

    #멀티 쓰레딩 Process 사용     
    procs = [] 
        
    proc_color_view = Process(target=color_view) #color_view
    proc_gui = Process(target=draw_gui, args = (imu_0_data, time_0_data, imu_0_vel, time_0_vel, imu_0_dis, time_0_dis,
                                                imu_1_data, time_1_data, imu_1_vel, time_1_vel, imu_1_dis, time_1_dis)) #gui 부분
    # proc_imu_draw = Process(target=list_insert, args = (imu_0_data, imu_1_data))     #읽기만 할거야
    proc_imu = Process(target=serial_load, args = (imu_0_data, time_0_data, imu_0_vel, time_0_vel, imu_0_dis, time_0_dis, 
                                                   imu_1_data, time_1_data, imu_1_vel, time_1_vel, imu_1_dis, time_1_dis)) #쓰기만 할거야
    
    procs.append(proc_color_view)   #viewer.py 에서 실행
    procs.append(proc_gui)          #gui 생성
    # procs.append(proc_imu_draw)     #kinect
    procs.append(proc_imu)          #imu sensor
    
    proc_color_view.start();       
    # proc_imu_draw.start();  
    proc_gui.start();       
    proc_imu.start();
    
    for proc in procs:
        proc.join()
    


