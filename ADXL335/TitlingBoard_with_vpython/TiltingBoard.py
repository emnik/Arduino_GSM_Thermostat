
from visual import *
import serial, sys, time


def getAngles():
        # Tell the Arduino how many points to grab
        # the number will be sent as a char, thus the limit of 255.
        ser.write(chr(N))

        # Now grab that many
        for i in range(N):
                # read a line from the serial port
                line = ser.readline()

                # split the line into milliseconds and value
                brokenline = line.split()
                xtilt=int(brokenline[1])
                ytilt=int(brokenline[2])
		ztilt=int(brokenline[3])
        return degToRad(xtilt),degToRad(ytilt),degToRad(ztilt)


def rotateAboutX(A, b): #A angle, b box, N normal      
        b.rotate(angle=A, axis=(1,0,0), origin=(0,0,0))

def rotateAboutY(A, b): 
        b.rotate(angle=A, axis=(0,1,0), origin=(0,0,0))

def rotateAboutZ(A, b):       
        b.rotate(angle=A, axis=(0,0,1), origin=(0,0,0))

def degToRad(D):
    return D*pi/180


# Scene Setup
scene.forward=vector(0,0,1)
scene.up=vector(0,1,0)
scene.ambient=color.gray(.5)
scene.range = 40
scene.width=800
scene.height=800

L = 50
thick = 4
board = box(pos=(0,0,0), size=(L,thick,L), material=materials.wood)


port = '/dev/ttyUSB0'

N = 1

# Start the serial port
ser = serial.Serial(port, 9600, timeout=2)
# The following line is necessary to give the arduino time to start
# accepting stuff.
time.sleep(1.5)


# Use angle data from arduino to rotate board
initialang = [0,0,0]
angles = [0,0,0]
newangles=[0,0,0]

while true:

    newangles[0],newangles[1],newangles[2]=getAngles()
    angles[0] = newangles[0] - initialang[0]
    angles[1] = newangles[1] - initialang[1]
    angles[2] = newangles[2] - initialang[2]

  
    rotateAboutX(angles[0], board)
    rotateAboutZ(-angles[1], board) # Z axis in VPython is the y axis of the accelerometer
    #rotateAboutY(-angles[2], board)

    initialang[0] = newangles[0]
    initialang[1] = newangles[1]
    initialang[2] = newangles[2]

