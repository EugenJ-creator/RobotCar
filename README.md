# RobotCar
## Projektbeschreibung: 
Entwicklung eines RobotCars mit verteilter Steuerarchitektur


<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/RobotCar.jpg" width=65% height=65%>

# Zielsetzung
Ziel des Projekts war die Entwicklung eines intelligenten RobotCars mit verteilter Mikrocontroller-Architektur, das über eine Android-App gesteuert wird. Die Architektur sollte modular, skalierbar und kommunikationsfähig über CAN-Bus sein, um verschiedene Funktionen wie Fahrsteuerung, Beleuchtung, Umgebungserfassung und spätere autonome Fahrfunktionen zu ermöglichen. 

# Das RobotCar basiert auf dem "DIY Smart Car Chassis Kit", ausgestattet mit:

* Einem antriebsmotor
* MG996R-Servo für Lenkung
* Stabile Aluminium-/Acryl-Plattform mit Kugellagern
* Diese Plattform bietet mechanische Stabilität, Traktion und Modularität für zusätzliche Komponenten wie Sensoren, Controller und Aktuatoren.
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/Platform.jpg" width=50% height=50%>

# Platform weiterentwicklung
* Die Plattform wurde mechanisch erweitert und skaliert, um die Integration aller elektronischen Komponenten (Mikrocontroller, Sensoren, Stromversorgung, Radar, Kamera usw.) zu ermöglichen.

<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/Entwicklung.jpg" width=50% height=50%>

* Zusätzliche Trägerstrukturen und Halterungen (aus Aluminium, Kusstoff und 3D-gedruckten Teilen) wurden konstruiert, um:
Sensoren wie Lichtsensor mechanisch sicher zu montieren.
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/PhotoSensor.jpg" width=50% height=50%>

# Systemarchitektur
## Verteilte Steuerungseinheiten:
* TM4C123GXL (ARM Cortex-M4F) als Hauptcontroller für zentrale Steuerung und Sensorfusion. Kommunikation mit Android APP über Bluetooth (CC2650) Modul.
* ESP32 #1: Zuständig für die Lichtsteuerung des Fahrzeugs.
* ESP32 #2: Verantwortlich für das Radarsystem mit rotierendem Lidar.
* Raspberry Pi 3: Als zentrale Datenverarbeitungs- und Visualisierungseinheit; Video streaming; Datenübertragung;  Kommunikation mit Android-App über WIFI Modul (Wird implementiert)

# TM4C123GXL
OS - RTOS

 * Duty Cycle Steuerung	Direkte Einstellung des PWM-Verhältnisses für den Motor (Motor driver DC L298N) oder Geschwindigkeitsvorgabe	(PID-Regler regelt Duty Cycle basierend auf gemessener Drehzahl)

* Lesen und Verarbeitung von Sensordaten,  Sensorschnittstellen (ADC, GPIO, I2C)
* Lichtlogik (Automatische Lichtregelung und Manuelle)
* Kommunikation mit ESP32, Raspberry via CAN (CAN-Transceiver	MCP2551)
* Kommunikation mit Bluetooth Modul (CC2650 (UART Rx/Tx)),  BLE-Kommando Weiterleitung 

# ESP32 #1

* Bare Metal 
* Empfang von Steuerbefehlen über CAN-Bus 
* Licht Steuerung (PWM, GPIO)


# ESP32 #2
* Bare Metal
* Radarsteuerung (Wird implementiert)
* Senden aller wichtigen Daten über CAN-Bus an Raspberry Pi (Wird implementiert)

# CC2650 (Firmware mit TI BLE Stack):
* BLE Advertise + Connect
* GATT-Server mit benutzerdefiniertem Service/Characteristic 
* Empfangene Daten via UART an Tiva TM4C123GXL

# Raspberry Pi 3
* Der Raspberry Pi 3 soll die zentrale Steuerung, Kommunikation und Benutzerinteraktion übernehmen (Wird implementiert)
* Alle Microcontroller (TM4C123, ESP32) bleiben über CAN-Bus mit dem Raspberry Pi verbunden.
* Kamera, WLAN, Cloud-Anbindung etc.
* Kommunikation über CAN-Modul MCP2515 (SPI)

# Komponenten

* Lichtsensor 	ADC	Helligkeitsmessung
* Temp/Feuchte (Ens160 aht21)	I²C	Umgebungserkennung
* Magnetometer (MPU9250)	I²C	, Kompassfunktion (Wurde kalibriert)
* Ledtreiber (MOSFET)
* Radarsystem (DC motor, Hall Sensor, Magnet, Lidar)
* Motor driver L298N


# Spannungsversorgung im Robot Car

* 14 V	Hauptversorgung Battarie 12800mah  37.44Wh
* 5 V	Mikrocontroller (TM4C, ESP32, CC2650, Raspberry)	Step-Down-Regler von 14 V
* 3.3 V	 Sensoren, LED-Treiber, 	5V Zu 3,3 V DC-DC Step Down Netzteil Buck Modul AMS1117 800MA

# Testphase
* Zur Überprüfung der Funktionalität und Qualität der Signalverarbeitung und Kommunikation im verteilten System, wurden Messungen mit einem Oszilloskop durchgeführt. 
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/CAN_TestPhase.jpg" width=50% height=50%>
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/CAN_Signal.jpg" width=50% height=50%>

# Android-App

* Bluetooth-Verbindung zum RobotCar

* Tastenfelder für Steuerung (↑ ↓ ← → STOPP), Tempomat aktivierung, Huppe

* Anzeige von Telemetrie: Temperatur, Feuchtigkeit, Geschwindigkeit,  Kompass

* Lichtsteuerung

<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/DashboardAndroidApp.jpg" width=50% height=50%>
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/MenuAndroidApp.jpg" width=50% height=50%>
<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/LightIntencityAndroidApp.jpg" width=50% height=50%>

# Problemlösung

Beim Messen der Drehzahl des DC-Motors mit einem fotoelektrischen Geschwindigkeitssensor tritt ein Störsignal 50 MHz auf. Dieses Rauschen kommt vom Motorbetrieb - PWM Frequenz. 
Um ein sauberes Drehzahlsignal aus dem gestörten Sensorsignal zu gewinnen, habe ich einen analogen Tiefpassfilter in Kombination mit einem Komparator eingesetzt.  

<img src="https://github.com/EugenJ-creator/RobotCar/blob/main/Rauschen.jpg" width=65% height=65%>


# Zukünftige Erweiterungen

* Raspberry  Kamera für Bilderkennung oder Livestream einsetzen
* Plotten Radar Entfernungswerte auf Android App 
* Sprachsteuerung per Android-App
* Autonomer Modus mit Umschaltung zwischen App und Sensorik (Kollision erkennung, Abstand tempomat)
* Daten auf dem Pi loggen. Webserver für den Pi bauen
* Battarie Spannung an Android App übermitteln


# Fazit
Das Projekt demonstriert ein verteiltes Steuerungssystem. Es bietet eine  Plattform, um Kompetenzen in Embedded-Systemen, Mikrocontrollerprogrammierung, Kommunikationstechnik und Automatisierung aufzubauen. Durch den modularen Aufbau kann das System flexibel erweitert und für weitere Experimente genutzt werden.
