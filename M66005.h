/* Copyright 2025 William F. Dudley Jr.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __M66005_H__
#define __M66005_H__

#include "Arduino.h"
#include <SPI.h>

#define ALL_SEGMENTS_OFF    0
#define ALL_SEGMENTS_NORMAL 1
#define ALL_SEGMENTS_ON     3

class M66005 {

    private:
	uint8_t _fl_cs;
	SPIClass *_pspi;

    public:
	M66005(uint8_t cs);
	void init(SPIClass *pspi);
	void brightness(uint8_t brightness);
	void all_segments(uint8_t off_norm_on);
	void write_string(const char *str, int len, int pos);
	void clear(void);
	void define_char(int ramaddress, const char * fivebyseven);
};

#endif
