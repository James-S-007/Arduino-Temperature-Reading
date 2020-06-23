import serial
from graphTemp import graph

def cleanCSV(fileName):
    with open(fileName, 'r') as f:
        lines = f.readlines()
    with open(fileName, 'w') as f:
        startFlag = False
        for line in lines:
            if line.find('Temperature') != -1:
                f.write('Temperature, Time\n')
                startFlag = True
            elif startFlag == True:
                f.write(line)
        f.close()

fileName = "Test.csv"


serialPort = serial.Serial(port = "COM3", baudrate=9600,
                           bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
print("Gathering Data")
csvFile = open(fileName, "w", newline = '')
try:
    while True:
        if(serialPort.in_waiting > 0):
            line = serialPort.readline()
            csvFile.write(line.decode("utf-8"))
except KeyboardInterrupt:
    serialPort.close()
    csvFile.close()
    print("Done Gathering Data")
    print("Cleaning CSV")
    titleName = fileName[0:len(fileName)-4]
    cleanCSV(fileName)
    print("Done Cleaning CSV")
    graph(fileName, titleName)




