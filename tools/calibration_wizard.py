#!/usr/bin/env python3

import struct
import sys
import textwrap

import serial


def read_register(com, address):
    com.write(struct.pack('<BB', 0, address))
    msg_id, result, addr, value = struct.unpack('<BbBH', com.read(5))
    assert (msg_id, result, addr) == (1, 0, address)
    return value


def write_register(com, address, value):
    com.write(struct.pack('<BBH', 2, address, value))
    assert struct.unpack('<BbBH', com.read(5)) == (3, 0, address, value)
    # last_data = read_register(com, 0x06)
    # assert last_data == value


def _main(*args):
    try:
        _, serial_port = args
    except ValueError:
        print("usage: calibration_wizard.py <serial_port>")
    else:
        with serial.Serial(serial_port, 115200) as com:
            adj_start = read_register(com, 0x30)
            assert adj_start in (0x6886, 0x5678)
            # start adjustment
            write_register(com, 0x30, 0x5678)
            default_gain = read_register(com, 0x31)
            print("""
Connect a voltage source to the device, a purely resistive load of known value 
between L_OUT and N, and a DMM across L and N close to the device's screw 
terminals.""")
            r_load = int(input("Done? Enter the load's value in Ohms: "))
            print(f"The load resistance has been set to {r_load} Ω.")
            while True:
                un = float(input("Enter the voltage reading: "))  # FIXME UT61B
                urms = read_register(com, 0x49)/100.0
                ugain = int((26400 * un) / urms)
                print(textwrap.fill(f"""
The device's voltage reading at the default gain of {default_gain} was {urms} V.
The real voltage reading has been said to be {un} V. This would yield a new gain
of {ugain}."""))
                if input("Apply? [y/N] ") in ('y', 'Y'):
                    break
            write_register(com, 0x31, ugain)
            # current gain
            cur_meal = read_register(com, 0x48)
            ib = read_register(com, 0x49) / r_load
            igain_l = int((31251 * ib) / cur_meal)
            write_register(com, 0x32, igain_l)
            # current offset
            print("Disconnect the load.")
            input("Done? Press Enter to confirm.")
            cur_meal = read_register(com, 0x48)
            ioffsetl = int(-(cur_meal * (igain_l / 2 ** 16) * (2 ** 8)))
            write_register(com, 0x35, ioffsetl)
            # finish calibration
            print("You may test the calibration now.")
            if input("Persist? [y/N] ") in ('y', 'Y'):
                write_register(com, 0x30, 0x8765)
                print("Persisted.")


if __name__ == '__main__':
    _main(*sys.argv)
