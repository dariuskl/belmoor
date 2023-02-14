#!/usr/bin/env python3

import struct
import sys

from binascii import hexlify

import serial


def read_register(com, address):
    com.write(struct.pack('<BB', 0, address))
    response = com.read(5)
    print("response:", hexlify(response))
    msg_id, result, addr, value = struct.unpack('<BbBH', response)
    assert msg_id == 1
    assert result == 0
    assert addr == address
    return value


def write_register(com, address, value):
    com.write(struct.pack('<BBH', 2, address, value))
    response = com.read(5)
    print("response:", hexlify(response))
    assert struct.unpack('<BbBH', response) == (3, 0, address, value)


def _main(*args):
    try:
        _, serial_port = args
    except ValueError:
        print("usage: calibration_wizard.py <serial_port>")
    else:
        with serial.Serial(serial_port, 115200) as com:
            # start adjustment
            status = read_register(com, 0x01)
            print("Status: {:x}".format(status))
            adjstart = read_register(com, 0x30)
            print("AdjStart: {:x}".format(adjstart))
            assert adjstart == 0x6886
            write_register(com, 0x30, 0x5678)
            default_gain = read_register(com, 0x31)
            print("""
Connect a voltage source to the device, a purely resistive load of known value 
between L_OUT and N, and a DMM across L and N close to the device's screw 
terminals.""")
            r_load = input("Done? Enter the load's value in Ohms: ")
            print(f"The load resistance has been set to {r_load} Ω.")
            un = float(input("Enter the voltage reading: "))  # FIXME UT61B
            urms = read_register(com, 0x49)/100.0
            ugain = int((26400 * un) / urms)
            print(f"""
The device's voltage reading at the default gain of ??? was {urms} V. The real voltage
reading has been said to be {un} V. This yields a new gain of {ugain}.""")
            write_register(com, 0x31, ugain)
            cur_meal = read_register(com, 0x48)
            ib = read_register(com, 0x49) / r_load
            igainl = int((31251 * ib) / cur_meal)
            write_register(com, 0x32, igainl)
            print("Disconnect the load.")
            input("Done? Press Enter to confirm.")
            cur_meal = read_register(com, 0x48)
            ioffsetl = int(-(cur_meal * (igainl / 2 ** 16) * (2 ** 8)))
            write_register(com, 0x35, ioffsetl)


if __name__ == '__main__':
    _main(*sys.argv)
