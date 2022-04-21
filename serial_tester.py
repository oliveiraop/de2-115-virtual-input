import serial

ser = serial.Serial('/dev/ttyxxx')

def send_package(data: list or None, data_type, length):
  ser.write(chr(40).encode())
  ser.write(chr(55).encode())
  ser.write(chr(length).encode())
  ser.write(chr(data_type).encode())
  if length > 0 and data:
      for i in data:
          if isinstance(i, str):
              ser.write(chr(ord(i)).encode())
          elif isinstance(i, int):
              ser.write(chr(i).encode())

""" 
               * Read:
 *   41 - "Hi Arduino, GUI here."
 *   88 - "End connection."
 *   90 - "Power the board on and off."
 *   05 - Action on FPGA buttons (1 data byte):
 **   00 - "Button 3 changed state."
 **   01 - "Button 2 changed state."
 **   02 - "Button 1 changed state."
 **   03 - "Button 0 changed state."
 **   04 - "Switch 17 changed state."
 **   05 - "Switch 16 changed state."
 **   06 - "Switch 15 changed state."
 **   07 - "Switch 14 changed state."
 **   08 - "Switch 13 changed state."
 **   09 - "Switch 12 changed state."
 **   10 - "Switch 11 changed state."
 **   11 - "Switch 10 changed state."
 **   12 - "Switch 9 changed state."
 **   13 - "Switch 8 changed state."
 **   14 - "Switch 7 changed state."
 **   15 - "Switch 6 changed state."
 **   16 - "Switch 5 changed state."
 **   17 - "Switch 4 changed state."
 **   18 - "Switch 3 changed state."
 **   19 - "Switch 2 changed state."
 **   20 - "Switch 1 changed state."
 **   21 - "Switch 0 changed state."
 *   127 - RESET
 **/ """

while ():
  received = int(input("Digite o comando: "))

  if (received == 5):
    received = int(input("Digite o botão ou switch: "))
    send_package([received], 5, 1)
  else:
    send_package(None, received, 0)


