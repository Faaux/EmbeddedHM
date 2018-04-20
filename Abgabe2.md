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

Der clock_id Parameter in den Funktionen clock_nanosleep und clock_gettime wurde auf CLOCK_REALTIME gesetzt. Der Grund hierfür ist das diese Clock den CPU auch aus dem Powersaving Zustand aufweckt.

Die korrekte Funktionsweise wird ermittelt indem die Zeiten vor und nach dem Aufruf von clock_nanosleep miteinander verglichen werden. Diese Zeiten werden in folgenden Aufgaben genauer betrachtet, da diese vom SystemTick und der zu schlafenden Zeit abhängig sind.

### Teilaufgabe 2
##### Realisierung einer Funktion die den Systemtakt ändert

Der Systemtakt (Tickrate) gibt die Frequenz an, wie oft die CPU von der Clock interrupted wird. Mit der Funktion "ClockPeriod" können Änderungen an der Frequenz vorgenommen werden, sowie die aktuelle Frequenz der clock abgefragt werden. Zuerst setzen wir die Frequenz auf den an die Funktion "changeSystemTick" übergebenen Mikrosekunden Wert. "ChangeSystemTick" gibt uns gleichzeitig die Tickrate vor der Änderung zurück. Anschließend wird die aktuelle Tickrate abgefragt und mit der alten Tickrate verglichen, um festzustellen ob eine Änderung der Frequenz statt gefunden hat.

Minimaler Wert für die Zykluszeit des Systemtakts sind 10 Mikrosekunden. Bei einem Wert unterhalb von 10 Mikrosekunden gibt "ClockPeriod" den Fehler "Invalid Argument" zurück.

### Teilaufgabe 3
##### Auswirkungen auf die Teilaufgabe 1 durch Modifikation des Systemtakts

![Diagramm.pdf](https://github.com/Faaux/EmbeddedHM/blob/master/diagramm2.png)

Das obige Diagramm zeigt, wie die Änderungen des Systemtakts den in Teilaufgabe 1 erstellen Takt von 1ms beeinflussen. Der Systemtakt wird von 10 Mikrosekunden (kleinster möglicher Wert) kontinuierlich auf einen Systemtakt von 1ms erhöht. Die y-Achse zeigt wie lange das Programm tatsächlich geschlafen hat. Normalerweise sollte das Diagramm eine horizontale Line bei 1.000.000 ns zeigen, was genau dem Takt von 1ms entspricht. Die CPU ist allerdings bei den hohen Tickraten so mit sich selbst beschäftigt, ständig die Zeit zu überwachen und Interrupts auszulösen, so dass das Programm erst viel später wieder aufgweckt wird. Aus der hohen Auslastung der CPU resultieren somit die Abweichungen des Takts.

![Diagramm.pdf](https://github.com/Faaux/EmbeddedHM/blob/master/diagramm.png)

Im zweiten Diagramm ist der Ausschnitt des System Tick noch vergrößert. Hier geht der System Tick über den Takt von 1ms hinaus. Das Programm soll 1ms schlafen, wird beispielsweise beim einem System Tick von 2ms aber erst nach 2ms aufgeweckt.
