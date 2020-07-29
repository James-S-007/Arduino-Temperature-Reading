import serial
from graphTemp import graph

def cleanCSV(fileName):
    with open(fileName, 'r') as f:
        lines = f.readlines()
    with open(fileName, 'w') as f:
        startFlag = False
        for line in lines:
            if line.find('Trial') != -1:
                f.write('Trial, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5, Sensor6, Sensor7, Sensor8, Sensor9, SensorA, SensorB, SensorC, SensorD, SensorE, SensorF, Sensor10, Max Temp, Max Recorded On Sensor\n')
                startFlag = True
            elif startFlag == True:
                f.write(line)
        f.close()

fileName = "24inPass.csv"


serialPort = serial.Serial(port = "COM4", baudrate=9600,
                           bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
print("Gathering Data")
csvFile = open(fileName, "w", newline = '')
#csvFile.write("Trial, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5, Sensor6, Sensor7, Sensor8, Sensor9, SensorA, SensorB, SensorC, SensorD, SensorE, SensorF, Sensor10, MaxTemp, Max Recorded on Sensor")
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
    #graph(fileName, titleName)




