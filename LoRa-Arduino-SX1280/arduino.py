import serial
from serial.tools import list_ports
import time
import sys
from colorama import Fore


def getComPort() -> str:
    while True:
        ports = list(list_ports.comports())
        port_names = [port.name.lower() for port in ports]

        print(f"{Fore.BLUE}Available COM ports:")
        for port in ports:
            print(f"{Fore.BLUE}\t{port.name}: {port.description}")

        inp_port = input(f"{Fore.YELLOW}Enter a port to connect to: ")
        if inp_port.lower() in port_names:
            break

    print(Fore.RESET)

    return inp_port


try:
    # create a new serial connection
    ser = serial.Serial(getComPort(), baudrate=9600, timeout=5)

    # check if the serial connection is open
    if ser.is_open:
        print("Serial connection is available")
except serial.SerialException:
    # handle exception if the serial connection cannot be established
    print("Serial connection is not available")
    sys.exit()

while True:
    if ser.inWaiting() > 0:
        input = ser.readline()
        print(input)
        time.sleep(0.5)
