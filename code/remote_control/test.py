import serial
import pygame
import time

pygame.init()
pygame.joystick.init()
ser = serial.Serial("COM5", 250000, timeout=0.1)

stage = pygame.joystick.get_init()
if stage:
    print("系统初始化成功")
else:
    print("系统初始化失败")

if ser.isOpen():
    print("打开串口成功。")
    print(ser.name)
else:
    print("打开串口失败。")

print(f"接入{pygame.joystick.get_count()}个设备")

joystick = pygame.joystick.Joystick(0)
joystick.init()
numaxes = joystick.get_numaxes()#获取键摇杆数量

print(f"手柄初始化{joystick.get_init()}  id:{joystick.get_id()}  名称:{joystick.get_name()}")

while True:
    out_data = []
    for event in pygame.event.get():
            
            if event.type == pygame.JOYAXISMOTION:  #LX LY RX RY LT RT

                for i in range(numaxes):
                    out_data.append(f"{joystick.get_axis(i):>6.3f}")

                ser.write("C{}E".format(out_data).encode('utf-8'))
                print("C{}E".format(out_data).encode('utf-8'))

                out_data = []
