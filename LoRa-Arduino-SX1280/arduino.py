import serial
from serial.tools import list_ports
import time
import sys
from colorama import Fore, Style

def getComPort() -> str:
    while True:
        ports = list(list_ports.comports())

        print(f"{Style.BRIGHT}{Fore.BLUE}Available COM ports:")
        for i in range(len(ports)):
            port = ports[i]
            print(f"{Style.BRIGHT}{Fore.BLUE}\t[{i}]\t{port.name}: {port.description}")

        inp_port = input(f"{Fore.YELLOW}Enter a port index to connect to: ")
        if not inp_port.isdigit():
            continue

        inp_port = int(inp_port)
        if 0 <= inp_port < len(ports):
            break

    print(Style.RESET_ALL)

    if sys.platform == "darwin":
        port_name = ports[inp_port].name
        if port_name[:3] == 'cu.':
            port_name = port_name[3:]
            
        inp_port = f"/dev/tty.{port_name}"
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
