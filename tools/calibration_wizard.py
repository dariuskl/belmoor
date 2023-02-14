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


def _current_offset(igainl, cur_meal):
    return ((~int(cur_meal * igainl / 256)) + 1) & 0xffff


def _cs2(ugain, igainl, ioffsetl):
    from functools import reduce
    igainn = 0x7530
    # other registers are zero, so they may be omitted from the calculation
    regbytes = b''.join(struct.pack('H', r) for r in (ugain, igainl, igainn, ioffsetl))
    return reduce(lambda x, y: x + y, regbytes) | (reduce(lambda x, y: x ^ y, regbytes) << 8)


assert _cs2(0x6763, 0x7b19, 8) == 0x2b0b


def _main(*args):
    try:
        _, serial_port = args
    except ValueError:
        print("usage: calibration_wizard.py <serial_port>")
    else:
        with serial.Serial(serial_port, 115200) as com:
            adj_start = read_register(com, 0x30)
            if adj_start == 0x8765:
                print(f"Voltage gain: {read_register(com, 0x31):x}")
                print(f"Current gain: {read_register(com, 0x32):x}")
                print(f"Current offset: {read_register(com, 0x35):x}")
                print(f"Checksum 2 (per device): {read_register(com, 0x3b):x}")
                if input("Already calibrated. Redo? [y/N] ") not in ('y', 'Y'):
                    if input("Persist? [y/N] ") in ('y', 'Y'):
                        write_register(com, 0x31, ugain)
                        write_register(com, 0x32, igainl)
                        write_register(com, 0x35, ioffsetl)
                        write_register(com, 0x3b, _cs2(ugain, igainl, ioffsetl))
                        write_register(com, 0x30, 0x8765)
                    return 0
            assert adj_start in (0x6886, 0x5678, 0x8765)
            # start adjustment
            write_register(com, 0x30, 0x5678)

            ugain = read_register(com, 0x31)
            igainl = read_register(com, 0x32)

            print("""
Connect a voltage source to the device, a purely resistive load of known value 
between L_OUT and N, and a DMM across L and N close to the device's screw 
terminals.""")
            input("Done? Press Enter to confirm.")

            # voltage gain
            while True:
                un = float(input("Enter the voltage reading: "))
                urms = read_register(com, 0x49) / 100.0
                ugain = int((ugain * un) / urms)
                write_register(com, 0x31, ugain)
                print(textwrap.fill(f"""
The device's voltage reading at the default gain was {urms} V. The real
voltage reading has been said to be {un} V. This yields a new voltage gain of
{ugain}, which has been applied."""))
                if input("Keep? [y/N] ") in ('y', 'Y'):
                    break

            # current gain
            while True:
                ib = float(input("Enter the current reading: "))
                cur_meal = read_register(com, 0x48) / 1000.0
                igainl = int((igainl * ib) / cur_meal)
                write_register(com, 0x32, igainl)
                print(f"""
The device's current reading at the previous gain was {cur_meal} A. The real
current reading has been said to be {ib} A. This yields a new current gain of
{igainl}, which has been applied.""")
                if input("Keep? [y/N] ") in ('y', 'Y'):
                    break

            # current offset
            print("To determine the current offset, disconnect the load.")
            input("Done? Press Enter to confirm.")
            cur_meal = read_register(com, 0x48) / 1000.0
            ioffsetl = _current_offset(igainl, cur_meal)
            print(f"Offset current read as {cur_meal} A. Writing {ioffsetl}.")
            write_register(com, 0x35, ioffsetl)

            # finish calibration
            print("You may test the calibration now.")
            if input("Persist? [y/N] ") in ('y', 'Y'):
                cs2 = _cs2(ugain, igainl, ioffsetl)
                write_register(com, 0x3b, cs2)
                write_register(com, 0x30, 0x8765)
                print(f"Persisted with a checksum (CS2) of {cs2:x}.")


if __name__ == '__main__':
    _main(*sys.argv)
