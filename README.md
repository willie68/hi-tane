# hi-tane
hardware implementation of the game "keep talk and nobody explodes"

German only

Hier beschreibe ich die Entwicklung meiner Hardware variante des Spiels "keep talk and nobody explodes".
Ziel dieses Spiels ist eine "Bombenentschärfung".  Das ganze ist als Geburtstagsgeschenk an meine Frau gedacht. 

Eingebaut in einen kleinen Koffer. 

# Einleitung

Hallo Bob, 
stell dir vor du bist ein einem Raum eingeschlossen und hast leider nur einen rudimentären Kontakt zur Außenwelt. D.h. du kannst nur über ein WalkieTalkie mit deinen Kollegen, Peter und Justus (im weiteren PJ), Kontakt aufnehmen. Und neben dir steht ein Koffer mit einer tickenden Bombe. Du hast 60 min Zeit die Bombe zu entschärfen. Das Gute daran, deine Kollegen draußen haben eine Anleitung zur Entschärfung gefunden. Nun müsst ihr zusammen die Bombe entschärfen, wichtig ist das ihr redet. Du beschreibst was du siehst und deine Kollegen versuchen die richtigen Kniffe zur Entschärfung zu finden.

# Hardware

Das Spiel ist eigentlich im original ein Handy Spiel, ich habe aber dieses schöne Repository auf github gefunden, dass mir ein klein wenig als Stütze dient. https://heathbar.github.io/keep-talking/

Aufgebaut besteht das Spiel aus einem zentralen Controller, der die Steuerung der Module und die Zeitmessung übernimmt. Nebenher zählt er auch die Fehlversuche. Die einzelnen Module sind als eigenständige Einheiten aufgebaut, die über ein Bussystem mit dem zentralen Controller kommunizieren.

Folgende Module sind geplant:

- 6x6 Maze: finde den Weg zum Ausgang
- Wires: entferne die richtigen Kabel
- Simon says: ähnlich unserem Senso
- Password: finde das richtige Passwort
- Morse: Höre und Antworte
- 

## Allgemein

Das ganze Spiel ist in einem Koffer eingebaut. Als Stromversorgung dient ein 18650 Akku. Der Controller mit den Strike LEDS, Lautsprecher, CoundDown Anzeige und 20x4 Display mit Encoder ist im Deckel montiert.

Die Spielmodule sind in einem Rahmen im eigentlichen Fach montiert und können dort ersetzt werden. Dadurch ist es möglich, verschiedenen Module zu verwenden. Derzeit hat der Rahmen platz für 6 Module. 

Alles zum Drucken findest du hier: https://www.tinkercad.com/users/fKoj7udgQYq-wklaas



## Controller

Der Controller koordiniert verschiedenen Dinge des Spiels:

- Zeitmessung: nach dem Spielstart wird die Zeit (e.g. 60 min) mit einem Counter herunter gezählt. Ist die Zeit abgelaufen und die Bombe wurde nicht entschärft, ist das Spiel vorbei und verloren. Wurde die Bombe entschärft, wird die Restzeit angezeigt.
- Strike-Counter: man hat 3 Fehlversuche zur Entschärfung. Diese werden zusätzlich angezeigt
- Initialisierung: Zum Spielstart werden die verschiedenen Module in ihren Anfangszustand zurück gesetzt, es können verschiedenen Spielstufen, wie auch die Spielzeit und verschiedene weitere Parameter justiert werden. Basierend auf der Spielstufe und den Parametern, können die Module verschiedene Szenarien verwenden. Mögliche Parameter:
  - Seriennummer
  - Version
  - Anzahl der bereits vorhandenen Strikes
  - Anzahl der gelösten Rätsel
- Ermittlung der Module: der Controller ermittelt zunächst welche Module montiert sind. Anhand dieser Liste kann er dann entscheiden, wieviel Zeit tatsächlich dem Entschärfer zur Verfügung gestellt wird.

Als Controller dient ein Arduino Nano mit folgender Peripherie:

- 20x4 Display mit Encoder zum Einstellen und Abrufen verschiedener Parameter
- Ein Lautsprecher als Beeper, später evtl. ein MP3 Modul
- ein 4-stelliges 7-Segment Display für die Spielzeitanzeige
- 3 RGB LEDs als Strikecounter

## 6x6 Maze: finde den Weg zum Ausgang

Basis ist eine 6x6 große RGB LED Matrix.

In diesem wird ein nicht sichtbares Labyrinth durch 2 feststehende gelbe Marker gekennzeichnet. Auch der Start (weiß)- und Zielpunkt (rot) werden angezeigt. Diese Informationen übergibst du an PJ. Die müssen nun das richtige Labyrinth identifizieren und Bob erklären wie er vom Startpunkt zu seinem Ziel kommt.

## Wires: entferne die richtigen Kabel

Bob sieht vor sich ein paar Kabel auf einem Steckfeld. Welche und wie viele Kabel muss er nun entfernen, um die Bombe zu entschärfen. PJ versuchen anhand der Erklärung von Bob die richtigen Kabel zu finden.

## Simon says: ähnlich unserem Senso

In 3 Spielstufen geben die Taster eine Blinkreihenfolge ab. Bob muss diese an PJ übermitteln. Die können nun in der Anleitung die richtige Antwort darauf finden. Bob muss diese dann eingeben. Das Blöde ist nur, Bob hat nix zum Schreiben...

## Password: finde das richtige Passwort

Bob muss ein Passwort knacken. Anhand der möglichen Buchstaben pro Stelle ergibt sich nur 1 mögliches Passwort. Findet es.

## Keypad: Höre und Antworte

Beim Aktivieren ertönt eine Morsecode. Dieser muss dechiffriert werden und ergibt dann eine 5-stellige Pin. Bob muss diese dann eingeben.

# Aufbau

Ein Bild sagt mehr als 1000 Worte, Stand derzeit März 2025

![aufbau_01](H:\privat\git-sourcen\hi-tane\aufbau_01.jpg)
