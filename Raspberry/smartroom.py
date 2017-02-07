import serial

ser2 = serial.Serial(port='/dev/ttyUSB0',baudrate=9600,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=0)
ser= serial.Serial(port='/dev/ttyUSB1',baudrate=9600,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=0)
print("Connected to"+ser.portstr)
print("Connected to"+ser2.portstr)
 
line = []
command = 'd'

def adjustVolume():
	command=ser.read()	
	if(command == 'u'):
		print("Speaker Volume Up")
		ser2.write("svu\r\n")
	elif(command == 'd'):
		print("Speaker Volume Down")
		ser2.write("svd\r\n")

while True :
	for c in ser.read():
		line.append(c)
		if(c == 'c'):
			print(c+ser.readline())
			break
		elif(c == 's'):
			if(ser.read() == 'v'):
				adjustVolume()

ser.close()


