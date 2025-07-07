import os.path
import csv
from datetime import datetime

class Decoder:
    def __init__(self, logFileLoc: str):
        if not os.path.isfile(logFileLoc):
            raise Exception(f"The file {logFileLoc} does not exist.")
        
        self.logFileLoc = logFileLoc
        self.parseLogFile()

    def parseLogFile(self):
        self.data: list[dict] = []
        with open(self.logFileLoc, 'r') as f:
            reader = csv.reader(f)

            # Skip the first three lines.
            next(reader)
            next(reader)
            next(reader)

            for row in reader:
                self.data.append({
                    "date":         datetime.strptime(row[0], '%d/%m/%Y %H:%M:%S'),        
                    "msg":          row[1],
                    "rssi":         int(row[2]),
                    "crc_errors":   int(row[4]),
                    "lat" :         float(row[5]),
                    "lon" :         float(row[6]),
                    "alt" :         float(row[7]),
                    "fix" :         int(row[8]),
                    "satellites":   int(row[9]),
                })

    def waypointToText(self, values: dict) -> str:
        return f"<b>{values['date'].strftime('%d/%m/%Y %H:%M:%S')} <br></b>" \
               f"Message: {values['msg']} <br>" \
               f"Latitude: {values['lat']:.8f} <br>" \
               f"Longitude: {values['lon']:.8f} <br>" \
               f"Altitude: {values['alt']:.8f} <br>"
    
    def getWaypoints(self) -> list[list]:
        return [[point["lat"], point["lon"]] for point in self.data]
    
    def getAverageWaypoint(self) -> tuple:
        lat = [point["lat"]for point in self.data]
        lon = [point["lon"]for point in self.data]
        return (sum(lat)/len(lat), sum(lon)/len(lon))
    
    def getMissingPoints(self) -> list[dict]:
        missing: list[dict] = []
        for i in range(len(self.data) - 1):
            msg: int = self.getNumberFromMsg(self.data[i]['msg'])
            nextMsg: int = self.getNumberFromMsg(self.data[i+1]['msg'])

            missingPoints: int = nextMsg - msg - 1
            if missingPoints == 0: continue

            for missI in range(missingPoints):
                date = self.interpolate(self.data[i]['date'], self.data[i+1]['date'], missingPoints+1, missI+1)
                lat = self.interpolate(self.data[i]['lat'], self.data[i+1]['lat'], missingPoints+1, missI+1)
                lon = self.interpolate(self.data[i]['lon'], self.data[i+1]['lon'], missingPoints+1, missI+1)
                alt = self.interpolate(self.data[i]['alt'], self.data[i+1]['alt'], missingPoints+1, missI+1)
                missingPoint = {
                    "date":         date,        
                    "msg":          f">{msg+missI+1}<",
                    "lat" :         lat,
                    "lon" :         lon,
                    "alt" :         alt,
                }
                missing.append(missingPoint)
        return missing

    def interpolate(self, initVal, endVal, totalIndices: int, index: int):
        delta = (endVal - initVal) / totalIndices
        return initVal + delta * index

    def getNumberFromMsg(self, input: str) -> int:
        # Removes the > < from the message
        return int(input[1:-1])

if __name__ == "__main__":
    print("This is a module. Run Main.py.")