#ifndef __M66005_H__
#define __M66005_H__

#include "Arduino.h"
#include <SPI.h>

class M66005 {

    private:
	uint8_t _fl_cs;
	SPIClass *_pspi;

    public:
	M66005(uint8_t cs);
	void init(SPIClass *pspi);
	void brightness(uint8_t brightness);
	void write_string(const char *str, int len, int pos);
	void clear(void);
	void define_char(int ramaddress, const char * fivebyseven);
};

#endif
