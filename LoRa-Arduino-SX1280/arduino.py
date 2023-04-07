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
        if port_name[:3] == "cu.":
            port_name = port_name[3:]

        inp_port = f"/dev/tty.{port_name}"
    else:
        inp_port = ports[inp_port].name

    return inp_port


class TransmitMessage:
    def __init__(self, raw_msg: str):
        msg_data_split = raw_msg.split(";")
        msg_data_split = [data.split(":", 1) for data in msg_data_split]
        msg_data = {data[0]: data[1] for data in msg_data_split}

        self.status = msg_data.get("Status")
        self.tx_power = msg_data.get("dBm")
        self.msg = msg_data.get("Packet")
        if self.msg:
            self.msg = self.msg[:-1]
        self.bytes_sent = msg_data.get("BytesSent")
        self.crc = msg_data.get("CRC")
        self.transmit_time = msg_data.get("TransmitTime")
        self.irq_reg = msg_data.get("IRQReg")
        self.irq_status = msg_data.get("IRQStatus")

    def __repr__(self):
        return f"[TRANSMIT {self.status}] msg: {self.msg}, transmit time: {self.transmit_time}"


class ReceiveMessage:
    def __init__(self, raw_msg: str):
        msg_data_split = raw_msg.split(";")
        msg_data_split = [data.split(":", 1) for data in msg_data_split]
        msg_data = {data[0]: data[1] for data in msg_data_split}

        self.status = msg_data.get("Status")
        self.msg = msg_data.get("Packet")
        if self.msg:
            self.msg = self.msg[:-1]
        self.crc = msg_data.get("CRC")
        self.rssi = msg_data.get("RSSI")
        self.snr = msg_data.get("SNR")
        self.bytes_sent = msg_data.get("BytesRec")
        self.irq_reg = msg_data.get("IRQReg")
        self.irq_status = msg_data.get("IRQStatus")

    def __repr__(self):
        return f"[RECEIVE {self.status}] msg: {self.msg}, RSSI: {self.rssi}, SNR: {self.snr}"


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
        ser_input = ser.readline().decode("utf-8").strip()
        if ser_input[:8] == "TRANSMIT":
            t_msg = TransmitMessage(ser_input[8:])
            print(t_msg)
        elif ser_input[:7] == "RECEIVE":
            r_msg = ReceiveMessage(ser_input[7:])
            print(r_msg)
        else:
            print(ser_input)
        time.sleep(0.5)
