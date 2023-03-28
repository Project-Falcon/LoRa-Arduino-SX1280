import serial
import time
import sys
# Check if serial connection is available
try:
    # create a new serial connection
    ser = serial.Serial('/dev/ttyUSB0', baud_rate=9600, timeout=5)
    
    # check if the serial connection is open
    if ser.isopen():
        print('Serial connection is available')
    
except serial.SerialException:
    # handle exception if the serial connection cannot be established
    print('Serial connection is not available')
    sys.exit()

while True:
    if (ser.inWaiting() > 0):
            input = ser.readline()
            print(input)
            time.sleep(1)
