import serial
import numpy as np
from PIL import Image
import sys

s = serial.Serial('/dev/ttyACM0', 115200)

def trig():
    print("WATCH")
    v = s.read_until()
    while b'TRIGTRIGTRIG' not in v:
        print('Unrecognized {}'.format(v))
        v = s.read_until()

    print("TRIG")
    # p = [int(x) for x in s.read_until().decode('ascii').strip().split(' ')]
    cur = b''
    for i in range(16):
        cur += s.read(50112//16)
        print("Ret {} of {}".format(len(cur), 50112))

    print("Got {} bytes".format(len(cur)))
    return list(cur)

def color(v):
    mtx = np.array([ [v[i*2+1],  v[i*2], v[i*2+2] ] if i % 2 == 0 else [v[i*2+1],  v[i*2-2] ,v[i*2]] for i in range(174*144)], 'uint8')
    return Image.fromarray(mtx.reshape((144, 174, 3)), 'YCbCr')

if __name__ == "__main__":
    while True:
        d = b""
        while len(d) != 50112:
            print("REload")
            d = trig()
        mtx = np.array(d, 'uint8').reshape((144, 174*2))
        color(d).show()
