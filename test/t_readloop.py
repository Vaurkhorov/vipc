#!/usr/bin/env python3
import os
import struct
import ctypes

PROC_PATH = "/proc/vipc"
PID_SIZE = ctypes.sizeof(ctypes.c_int)
PID_FMT = "<q" if PID_SIZE == 8 else "<i"

mode = input("Enter 's' to send messages, 'r' to get them, 'x' to quit: ")

if mode.lower() == "r":
    print("PID->", os.getpid())

    while True:
        if input("Press enter to get a message, or 'x' and then enter to quit: "):
            break
        
        with open(PROC_PATH, "r") as f:
            print(f.readlines())
elif mode.lower() == "s":
    receiver_pid = int(input("Enter receiver PID (decimal): "))

    while msg := input("Enter message(leave blank to quit): "):
        try:
            payload = struct.pack(PID_FMT, receiver_pid) + msg.encode("utf-8")
            with open(PROC_PATH, "wb") as f:
                f.write(payload)
        except Exception as e:
            print("Could not write to", PROC_PATH, ":", e)


