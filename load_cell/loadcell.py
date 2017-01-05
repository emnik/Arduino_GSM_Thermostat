"""
loadcell.py
 
Display analog data from Arduino using Python (matplotlib)
 
Author: Mahesh Venkitachalam, Nikiforakis Manos
Website: electronut.in
"""
 
import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque
import matplotlib.pyplot as plt 
import matplotlib.animation as animation

pause = False


# plot class
class AnalogPlot:
  # constr
  def __init__(self, strPort, maxLen):
      # open serial port
      self.ser = serial.Serial(strPort, 9600)
      self.ser.flush()
      self.ax = deque([0.0]*maxLen)
      self.ay = deque([0.0]*maxLen)
      self.maxLen = maxLen
 
  # add to buffer
  def addToBuf(self, buf, val):
      if len(buf) < self.maxLen:
          buf.append(val)
      else:
          buf.pop()
          buf.appendleft(val)
 
  # add data
  def add(self, data):
      assert(len(data) == 2)
      self.addToBuf(self.ax, data[0])
      self.addToBuf(self.ay, data[1])
 
  # update plot
  def update(self, frameNum, a0, a1):
      try:
          line = self.ser.readline()
          #data = [float(val) for val in line.split()]
          try:
              data = [float(line), 0.0] #the second argument is for a second graph. I set it to zero...
              if not pause:
                  print data
              if(len(data) == 2):
                  self.add(data)
                  if not pause: 
                      a0.set_data(range(self.maxLen), self.ax)
                      a1.set_data(range(self.maxLen), self.ay)
	  except ValueError:
              #pass
              sys.exc_clear() #just clears the last exception.
      except KeyboardInterrupt:
          print('exiting')
          self.close()
	  
      return a0,


  # clean up
  def close(self):
      # close serial
      self.ser.flush()
      self.ser.close()


def onClick(event):
     global pause
     pause ^= True
     if pause==True:
	print "Pause"

# main() function
def main():
  # create parser
  parser = argparse.ArgumentParser(description="loadcell live data plot")
  # add expected arguments
  parser.add_argument('--port', dest='port', required=True)
 
  # parse args
  args = parser.parse_args()
  
  #strPort = '/dev/ttyUSB0'
  strPort = args.port
 
  print('reading from serial port %s...' % strPort)
 
  # plot parameters
  analogPlot = AnalogPlot(strPort, 100)
 
  print('plotting data...')
  
  # set up animation
  fig = plt.figure()
  ax = plt.axes(xlim=(0, 100), ylim=(0, 5000))
  a0, = ax.plot([], [])
  a1, = ax.plot([], [])
  fig.canvas.mpl_connect('button_press_event', onClick)
  anim = animation.FuncAnimation(fig, analogPlot.update, 
                                 fargs=(a0, a1), 
                                 blit=True, interval=20, repeat=False)

  # show plot
  plt.show()
  
  # clean up
  analogPlot.close()
  print('exiting.')
  
 
# call main
if __name__ == '__main__':
  main()