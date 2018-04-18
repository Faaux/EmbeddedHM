# Aufgabe 2: QNX - Fester Zeittakt
Embedded Computing, Teilgruppe 2, Florian Tobusch - Chris Brammer

### Teilaufgabe 1
##### Realisierung des Zeittaktes
Der Takt von einer Millisekunde wurde realisiert indem die derzeitige Zeit mit "clock_gettime" abgerufen wurde. 
Anschliessend wurde 1ms addiert, hierbei muss darauf geachtet werden das der Nanosekunden Teil des Paramerters zum Überlauf führen kann (siehe Code). 

Der neu errechnete Zeitpunkt wird der Funktion clock_nanosleep übergeben. D.h diese soll bis zu dem gegeben Zeitpunkt schlafen. 
Dies hat zur Folge, dass Code der zwischen clock_gettime und clock_nanosleep liegt nicht den Zeitpunkt des "aufwachens" verändert.
Sollte der ausgeführte Code länger dauern als die Zeit die geschlafen werden soll (z.B 1 Millisekunde) kehrt clock_nanosleep sofort zurück,
da der Zeitpunkt des Aufwachens (anfängliche Addition von 1 Millisekunde) schon vergangen ist.

Der clock_id Parameter in den Funktionen clock_nanosleep und clock_gettime wurde auf CLOCK_REALTIME gesetzt. Der Grund hierfür ist das diese Clock
den CPU auch aus dem Powersaving Zustand aufweckt.
