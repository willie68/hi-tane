# Messages

## HEARTBEAT

Heartbeat will be send every second to syncronise all modiules to the controller game time

**Sender:** controller

**Receiver:** every module

**Priority:** high

**ID:** 0x0001

**Payload:** gametime high, gametime low, strikes

## GAMESETTINGS

Game settings will be send short before starting the game. 

**Sender:** controller

**Receiver:** every module

**Priority:** high

**ID:** 0x0003

**Payload:** (byte) difficulty, indicators low, indicator high, snr  low, snr middle, snr high 

## AMBIENTSETTINGS

Ambient settings will be send if changed. 

**Sender:** controller

**Receiver:** every module

**Priority:** medium

**ID:** 0x0002

**Payload:** (byte) brightness

## INITIALISATION

this will be sended to evaluate which modules are installed.

**Sender:** controller

**Receiver:** every module

**Priority:** medium

**ID:** 0x0004

**Payload:** n.n.

## GAME PAUSED

the game is paused until a new heartbeat occurs.

**Sender:** controller

**Receiver:** every module

**Priority:** medium

**ID:** 0x0005

**Payload:** n.n.

## ERROR

Module error

**Sender:** modul

**Receiver:** controller

**Priority:** medium

**ID:** 0x03## (where ## is the module id)

**Payload:** error number

## STATE

Module state

**Sender:** modul

**Receiver:** controller

**Priority:** medium

**ID:** 0x04## (where ## is the module id)

**Payload:** module_id, state

## STRIKE

The module has a strike occured

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x05## (where ## is the module id)

**Payload:** moduleID

## MODULEINIT

The module confirms the init command

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x06## (where ## is the module id)

**Payload:** moduleID

## BEEP

Send a beep to the controller

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x0010

**Payload:** moduleID

