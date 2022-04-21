import serial
import time

"""
Create virtual serial interface for test

socat -d -d pty,raw,echo=0 pty,raw,echo=0
"""

ser = serial.Serial('/dev/pts/2', timeout=0)

ser.close()
gui_on = False
ser.open()
while (1):
  
  received = ser.read().decode("ascii","ignore")
  if len(received) > 0:
    print(ord(received), end='\n')
  if received == chr(41):
    print("Received 41")
    gui_on = True
    ser.write(chr(42).encode())
  if received == chr(90):
    ser.write(chr(91).encode())

  if not gui_on:
    ser.write(chr(40).encode())
    time.sleep(1/500)
    

