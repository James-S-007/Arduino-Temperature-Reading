import matplotlib.pyplot as plt
import csv

def graph(fileName, titleName):

    times = []
    temperatures = []

    with open(fileName, 'r') as csvFile:
        plots = csv.reader(csvFile, delimiter = ',')
        count = 0
        for row in plots:
            if count != 0:  
                times.append(float(row[1]))
                temperatures.append(float(row[0]))
            count += 1
        
    plt.plot(times, temperatures)
    plt.xlabel('Time (Seconds)')
    plt.ylabel('Temperature (F)')
    plt.title(titleName)
    plt.show()

graph('NewVersionTest2.csv', "NewVersionTest2")