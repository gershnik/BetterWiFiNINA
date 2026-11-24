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

#ifndef SPI_Ext_h
#define SPI_Ext_h

#include "spi_types.h"
#include "spi_drv.h"

int SpiExt_waitResponseParams(uint8_t cmd, uint8_t numParam, tDataParam* params);

#endif
