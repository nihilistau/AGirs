// Dummy file to placify the silly Arduino IDE.
// The real program resides in Girs.cpp.
// Rationale: see http://www.gammon.com.au/forum/?id=12625

#include "config.h"
#include <InfraredTypes.h>
#include <LedLcdManager.h>

#ifdef ETHERNET
#include <Ethernet.h>
#endif

#ifdef LCD
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#endif

#ifdef NAMED_COMMAND
#include "IrNamedRemote.h"
#endif

#ifdef BEACON
#include <Beacon.h>
#endif
