# hi-tane
hardware implementation of the game "talk and nobody explodes"

German only

Hier beschreibe ich die Entwicklung meiner HArdware variante des Spiels "talk and nobody explodes".
Ziel dieses Spiels ist eine "Bombenentschärfung". 

# Einleitung

Hallo Bob, 
stell dir vor du bist ein einem Raum eingeschlossen und hast leider nur einen rudimentären Kontakt zur Außenwelt. D.h. du kannst nur über ein WalkieTalkie mit deinen Kollegen, Peter und Justus, Kontakt aufnehmen. Und neben dir steht ein Koffer mit einer tickenden Bombe. Du hast 60 min Zeit die Bombe zu entschärfen. Das Gute daran, deine Kollegen draußen haben eine Anleitung zur Entschärfung gefunden. Nun müsst ihr zusammen die Bombe entschärfen, wichtig ist das ihr redet. Du beschreibst was du siehst und deine Kollegen versuchen die richtigen Kniffe zur Entschärfung zu finden.

# Hardware

Das Spiel ist eigentlich im original ein Handy Spiel, ich habe aber dieses schöne Repository auf github gefunden, daß mir ein kleinwenig als Stütze dient. https://heathbar.github.io/keep-talking/

Aufgebaut besteht das Spiel aus einem zentralen Controller, der die Steuerung der Module und die Zeitmessung übernimmt. NEbenher zählt er auch die Fehlversuche. Die einzelnen Module sind als eigenständige Einhaiten aufgebaut, die über ein Busdsystem mit dem zentralen Controller kommunizieren.

Folgende Module sind geplant:

- 8x8 Maze: finde den Weg zum Ausgang
- Wires: entferne die richtigen Kabel
- Simon says: ähnlich unserem Senso
- Password: finde das richtige Passwort
- Keypad: Höre und Antworte

## Allgemein

Das ganze Spiel ist in einem Koffer eingebaut. Als Stromversorgung dient ein 18450 Akku. Der Controller mit den Stike LEDS, Lautsprecher, CoundDown Anzeige und 16x2 Display mit Encoder ist im Deckel montiert.

Die Spielmodule sind in einem Rahmen im eigentlichen Fach montiert und können dort ersetzt werden. Dadurch ist es möglich, verschiedenen Module zu verwenden. 

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

Als Controller dient ein Arduino mit folgender Peripherie:

- 16x2 Display mit Encoder zum Einstellen und Abrufen verschiedener Parameter
- Ein Lautsprecher als Beeper, später evtl. ein MP3 Modul
- ein 4-stelliges 7-Segmant Display für die Spielzeitanzeige
- 3 Dual-Color oder RGB LEDs als Strikecounter

## 8x8 Maze: finde den Weg zum Ausgang

Basis ist eine 8x8 große LED Matrix.

In diesem wird ein nicht sichtbares Labyrinth durch 2 feststehende LEDs gekennzeichnet. Auch der Start- und Zielpunkt werden angezeigt. Diese Informationen übergibst du an Peter und Justus. Die müssen nun das richtige Labyrinth identifizieren und Bob erklären wie er vom Startpunkt zu seinem Ziel kommt.

## Wires: entferne die richtigen Kabel

## Simon says: ähnlich unserem Senso

## Password: finde das richtige Passwort

## Keypad: Höre und Antworte
