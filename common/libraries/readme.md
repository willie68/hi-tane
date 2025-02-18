# Messages

## CMD_HEARTBEAT

Heartbeat will be send every second to syncronise all modiules to the controller game time

**Sender:** controller

**Receiver:** every module

**Priority:** high

**ID:** 0x010

**Payload:** gametime high, gametime low, strikes

## CMD_GAMESETTINGS

Game settings will be send short before starting the game. 

**Sender:** controller

**Receiver:** every module

**Priority:** high

**ID:** 0x011

**Payload:** difficulty, indicators low, indicator high, snr  low, snr middle, snr high 

## CMD_AMBIENTSETTINGS

Ambient settings will be send if changed. 

**Sender:** controller

**Receiver:** every module

**Priority:** medium

**ID:** 0x020

**Payload:** brightness



## CMD_STRIKE

The module has a strike occured

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x03# (where # is the module id)

**Payload:** moduleID

## CMD_DISARM

The module has been disarmed

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x04# (where # is the module id)

**Payload:** moduleID

## CMD_ERROR

The module has encountered an error

**Sender:** every module

**Receiver:** controller

**Priority:** medium

**ID:** 0x05# (where # is the module id)

**Payload:** moduleID, error number

