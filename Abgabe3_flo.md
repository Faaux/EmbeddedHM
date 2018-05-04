# Aufgabe 2: QNX - Verarbeitungszeit
Embedded Computing, Teilgruppe 2, Florian Tobusch - Chris Brammer

##### Funktion "waste_msecs(unsigned int msecs)"

Mit der Funktion "calibrate_waste_scaling()" wird der Faktor zur Anpassung der Schleifendurchläufe für "waste_msecs()" berechnet.
Für die Kalibrierung wird die Funktion "waste_msec_internal()", die die Fibonacci-Folge bis 200.000 berechnet, x-Mal aufgerufen. Um eine bessere Genauigkeit zu erhalten wird die Funktion "waste_msec_internal()" nicht nur einmal aufgerufen. Nach der Zeitmessung für die x-Aufrufe von "waste_msec_internal()" wird die Zeit für einen Durchlauf berechnet, die dann zur Ermittlung des neuen Faktors verwendet wird.

Mit "do_waste_stess_test()" kann nun nach erfolgreicher Kalibrierung die CPU eine bestimmte Anzahl an Millisekunden ausgelastet werden. Unser Programm gibt auch noch die Zeit aus die tatsächlich geschlafen wurde, sowie die Abweichung.


##### Prioritäten aller laufenden Prozesse ermitteln

Über das Kommandozeilen-Programm "pidin" können die Prioritäten aller laufenden Prozesse angezeigt werden. Hierfür muss eine Verbindung über telnet auf das BeagleBone hergestellt werden.


##### Messroutine auf höchste Priorität stellen

Neben der Messroutine, die direkt in Main ausgeführt wird, gibt es noch eine zweite Routine, die als eigener Thread mit maximaler Priorität läuft. Der Thread führt ebenfalls eine Kalibrierung aus und blockiert die CPU danach für eine definierte Zeit an milli-Sekunden.

```c
    pthread_t worker;
    pthread_attr_t worker_attr;

    struct sched_param worker_sched_param; //sched_curpriority is ignored
    worker_sched_param.sched_priority = sched_get_priority_max(SCHED_RR);

    pthread_attr_init(&worker_attr); //initialize thread-attribute object to default values
    pthread_attr_setinheritsched(&worker_attr, PTHREAD_EXPLICIT_SCHED); //disable scheduling policy from parent thread
    pthread_attr_setschedparam(&worker_attr, &worker_sched_param); //define scheduling parameters

    if (EOK != pthread_create(&worker, &worker_attr, &worker_thread, NULL)) {
        perror("Error during thread creation");
    }
```

Um den neuen Thread auf die maximale Priorität zu setzen, müssen die Attribute von pthread definiert werden. Die maximale Priorität wird als sched_param in ein sturct gespeichert. Mit "pthread_attr_init" werden die Attribute erstmalig initialisiert. Um die Priorität des Threads ändern zu können, müssen die Scheduling-Parameter des Eltern-Threads deaktiviert werden, was über "pthread_attr_setinheritsched" erfolgt. Danach können die eigenen Scheduling-Parameter gesetzt werden.


##### Beurteilung der Messungen

Bei 100 Durchläufen sollen für jeweils 900ms die CPU ausgelastet werden. Hierbei ergeben sich folgende Abweichungen:

Main-Thread mit normaler Priorität:
* Durchschnittliche absolute Abweichung: 54
* Standardabweichung: 27

Worker-Thread mit maximaler Priorität:
* Durchschnittliche absolute Abweichung: 9
* Standardabweichung: 20
