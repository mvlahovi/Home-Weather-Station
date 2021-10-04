/**************************************************************************/
/*!
 * @file Adafruit_EEPROM_I2C.cpp
 */
/**************************************************************************/
#include <math.h>
#include <stdlib.h>

#include "Adafruit_EEPROM_I2C.h"

/*========================================================================*/
/*                            CONSTRUCTORS                                */
/*========================================================================*/

/**************************************************************************/
/*!
    Constructor
*/
/**************************************************************************/
Adafruit_EEPROM_I2C::Adafruit_EEPROM_I2C(void) {}

/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  addr
 *            The I2C address to be used.
 *    @param  theWire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_EEPROM_I2C::begin(uint8_t addr, TwoWire *theWire) {
  i2c_addr = addr;
  _wire = theWire;

  _wire->begin();

  // A basic scanner, see if it ACK's
  _wire->beginTransmission(i2c_addr);
  if (_wire->endTransmission() == 0) {
    return true;
  }

  return false;
}

/**************************************************************************/
/*!
    @brief  Writes a byte at the specific EEPROM address

    @param[in] addr
                The 16-bit address to write to in EEPROM memory
    @param[in] value
                The 8-bit value to write at addr
*/
/**************************************************************************/
void Adafruit_EEPROM_I2C::write8(uint16_t addr, uint8_t value) {
  _wire->beginTransmission(i2c_addr);
  _wire->write(addr >> 8);
  _wire->write(addr & 0xFF);
  _wire->write(value);
  _wire->endTransmission();

  // Wait until it acks!
  while (1) {
    _wire->beginTransmission(i2c_addr);
    if (_wire->endTransmission() == 0) {
      return;
    }
    delay(1);
  }
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value from the specified EEPROM address
    @param addr
                The 16-bit address to read from in EEPROM memory
    @returns    The 8-bit value retrieved at addr
*/
/**************************************************************************/
uint8_t Adafruit_EEPROM_I2C::read8(uint16_t addr) {
  _wire->beginTransmission(i2c_addr);
  _wire->write(addr >> 8);
  _wire->write(addr & 0xFF);
  _wire->endTransmission();

  size_t recv = _wire->requestFrom(i2c_addr, (uint8_t)1);
  if (recv != 1) {
    return 0;
  }
  return _wire->read();
}

/*************************************************************************/

uint16_t Adafruit_EEPROM_I2C::write16(uint16_t addr, uint16_t value)
{
  write8 (addr,  (uint8_t)(value>>8));
  write8 (addr+1,  (uint8_t)(value & 0x00FF));
  /*
  uint8_t* p;
  p = (uint8_t*) &value;
  uint16_t i;
  _wire->beginTransmission(i2c_addr);
  _wire->write(addr >> 8);
  _wire->write(addr & 0xFF);
  for (i = 0; i < 2; i++) { _wire->write(*p); p++; }
  _wire->endTransmission();
  // Wait until it acks!
    while (1) {
      _wire->beginTransmission(i2c_addr);
      if (_wire->endTransmission() == 0) {
        return i;
      }
      delay(1);
    }
  return i;
  */
}


uint16_t  Adafruit_EEPROM_I2C::read16(uint16_t addr)
{
  uint16_t i;
  i = ((uint16_t)read8 (addr)) << 8;
  i = i | ((uint16_t)read8 (addr + 1) & 0x00FF);
  return i;
  /*
  uint8_t* p;
  p = (uint8_t*) &value;
  uint16_t i;
  _wire->beginTransmission(i2c_addr);
  _wire->write(addr >> 8);
  _wire->write(addr & 0xFF);
  _wire->endTransmission();
  _wire->requestFrom(i2c_addr,sizeof(value));
  for (i = 0; i < 2; i++)
  if(_wire->available())
  *p++ = _wire->read();
  return i;
  */
}
/////////////////////////////////////////////////////////////////////
uint32_t Adafruit_EEPROM_I2C::read32(uint16_t addr)
{
	uint32_t data;
	data = ((uint32_t)read16( addr )) <<16;
	data = data | (((uint32_t)read16( addr + 2 )) & 0x0000FFFF);
	return ( data);
}

void Adafruit_EEPROM_I2C::write32(uint16_t addr, uint32_t data)
{
	write16( addr, (uint16_t)((data >> 16) & 0x0000FFFF));
	write16( addr+2, (uint16_t)(data  & 0x0000FFFF));
}