#!/usr/bin/env python
# license removed for brevity
import serial
import rospy
from sat.msg import sat
ser = serial.Serial('/dev/ttyUSB0',115200)

def talker():
        pass
#     pub = rospy.Publisher('chatter', sat , queue_size=10)
#     rospy.init_node('pub', anonymous=True)
#     rate = rospy.Rate(10) # 10hz
   # i = 1
   
pub = rospy.Publisher('chatter', sat , queue_size=10)
rospy.init_node('pub', anonymous=True)
rate = rospy.Rate(10) # 10hz
import time
while True: 
        msg = sat()

        y = ser.readline()
        ser = serial.Serial('/dev/ttyUSB0',115200)
        #time.sleep(.1)
        #print(y)
        data = [ x for x in y.split(",") ]
        #ser.close()
        #ser.open()
        #ser.flush()
        
        # data = data[1:3]
        try:
                #if(len(data)!=4 or data[1]==" " or data[2]==" "):
                 #       raise Exception
                
                a = int(data[1])
                b = int(data[2])
                
                # print(b)
                # data = [ int(data[1]),int(data[2])]
                #print(a,b)
        
                msg.count_0 = a
                msg.count_1 = b
                rospy.loginfo(msg)
                pub.publish(msg)
        except:
                #print("Wrong Data Input")
                pass
        #rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
