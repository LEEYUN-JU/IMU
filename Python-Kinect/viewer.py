import cv2
import numpy as np

import pyk4a
from pyk4a import Config, PyK4A
from tkinter import messagebox

print_num = 0
status = 0

def print_status(status):
    
    if status == 0:
        messagebox.showinfo("notification", "cam called")
    if status == 1:
        messagebox.showinfo("notification", "capture successed start drawing")
    if status == 2:
        messagebox.showinfo("notification", "capture failed")
    if status == 3:
        messagebox.showinfo("notification", "cam closed")

def color_view():
    # print_status(0)
    global print_num
    
    k4a = PyK4A(
        Config(
            color_resolution=pyk4a.ColorResolution.RES_720P,
            depth_mode=pyk4a.DepthMode.NFOV_UNBINNED,
            synchronized_images_only=True,
        )
    )
    k4a.start() #type:pyk4a.PyK4A

    # getters and setters directly get and set on device
    k4a.whitebalance = 4500
    assert k4a.whitebalance == 4500
    k4a.whitebalance = 4510
    assert k4a.whitebalance == 4510      


    while 1:
        capture = k4a.get_capture()
        if capture != 0:
            if print_num == 0:
                # print_status(1)
                print_num = 1
            else:
                if np.any(capture.color):
                    cv2.imshow("color_img", capture.color[:, :, :3])
                    print(type(capture.color))
                    key = cv2.waitKey(10)
                    if key != -1:
                        cv2.destroyAllWindows()
                        break
        else:
            # print_status(2)
            k4a.stop()


    k4a.stop()
    # print_status(3)
 


if __name__ == "__main__":
    color_view()
