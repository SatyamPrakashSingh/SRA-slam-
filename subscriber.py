#!/usr/bin/env python

###################################################################################################################
# CODE TO COMPUTE ODOMETRY DATA FROM RAW ENCODER TICKS
# THE MODEL APPLIES TO DIFFERENTIAL DRIVE ROBOTS
# ODOMETRY INFORMATION IS PUBLISHED ON THE "odom" TOPIC AND ROBOT BASE MOVES WITH THE "odom" FRAME AS THE ORIGIN
# TF TREE IS BROADCAST AS FOLLOWS (laser_frame --> base_link--> odom)
###################################################################################################################

import rospy
# from std_msgs.msg import Float32MultiArray,Int16MultiArray
from math import *
import tf
from sat.msg import sat



def callback(data):
    x=0.00
    y=0.00
    w=0
    r=(2*3.14*3.5*data.count_0)/100
    l=(2*3.14*3.5*data.count_1)/100
    c=(r+l)/2
    w=w+((r-l)/26.2)
    x=x+(c*cos(w))
    y=y+(c*sin(w))


    print(x)
    print(y)
    print(w)
    rospy.loginfo(data.count_0)
    rospy.loginfo(data.count_1)
    
def listener():
    rospy.init_node('sub', anonymous=True)
    rospy.Subscriber("chatter", sat , callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()


