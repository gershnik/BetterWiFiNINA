/*
  This file is part of the BetterWiFiNINA library.
  Copyright (c) 2024 Eugene Gershnik. All rights reserved.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  This library is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License 
  for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <https://www.gnu.org/licenses/>.
*/

#include "Arduino.h"
#include <SPI.h>
#include "spi_ext.h"
#include "pins_arduino.h"
#include "WiFi.h"
#include "utility/debug.h"

#define WAIT_START_CMD(x) SpiDrv::waitSpiChar(START_CMD)

#define IF_CHECK_START_CMD(x) \
  if (!WAIT_START_CMD(_data)) { \
    TOGGLE_TRIGGER() \
    WARN("Error waiting START_CMD"); \
    return 0; \
  } else

#define CHECK_DATA(check, x) \
  if (!SpiDrv::readAndCheckChar(check, &x)) { \
    TOGGLE_TRIGGER() \
    WARN("Reply error"); \
    INFO2(check, (uint8_t)x); \
    return 0; \
  } else

int SpiExt_waitResponseParams(uint8_t cmd, uint8_t numParam, tDataParam* params)
{
    char _data = 0;
    int i =0, ii = 0;


    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t _numParam = SpiDrv::readChar();
        if (_numParam != 0)
        {        
            for (i=0; i<_numParam; ++i)
            {
                params[i].dataLen = SpiDrv::readParamLen16();
                for (ii=0; ii<params[i].dataLen; ++ii)
                {
                    // Get Params data
                    params[i].data[ii] = SpiDrv::spiTransfer(DUMMY_DATA);
                } 
            }
        } else
        {
            WARN("Error numParam == 0");
            return 0;
        }

        if (numParam != _numParam)
        {
            WARN("Mismatch numParam");
            return 0;
        }

        SpiDrv::readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}

