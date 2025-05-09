#include <Arduino.h>
#include <SPI.h>
#include "M66005.h"
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
/* originally by george9801
 * heavily re-written and fixed by W.F. Dudley Jr. 2025 01 09
 * more mods by W.F. Dudley Jr. 2025 02 06
 */

/*  text by george9801:
 *  In the end I gave up on the Bit Bang method and switched to SPI. I wrote
 *  the library for this screen, but I have a problem with it, it displays
 *  the text in reverse (from right to left).
 *  The problem seems to be in the while structure, it takes my characters
 *  in the order in which they are given. How can I reverse the order of
 *  characters?
 *  The two "for" structures are for bit inversion.
 *
 * notes by WFD:
 *  solved the "characters are reversed problem" with RT_TO_LEFT_MODE
 *  The 66005 part comes up in 66004 mode after reset
 */

// if your hardware was designed by a native Japanese speaker, with
// character 0 at the RIGHT end, then enable RT_TO_LEFT_MODE
#define RT_TO_LEFT_MODE 1
// my display uses position 15 for all the icons, so my first character
// location is position 14, and I can write 15 chars max.  If this is
// set to 0, then you can use all the locations for characters
#define ICONS_OFFSET 1

#define BEGIN_TR_DELAY 1
#define INTRA_CHAR_DELAY 15
#define END_TR_DELAY 5
#define SPI_SPEED 400000

#if M68005_MODE
static const uint8_t seq1[] = { 1, 0xfe };	// set to M68005 mode
static const uint8_t seq2[] = { 2, 0x10, 0x07 }; // set length 16 digits
static const uint8_t seq3[] = { 2, 0x11, 0x07 }; // set brightness to 14/16
static const uint8_t seq4[] = { 1, 0xf1 };	// set "all digit display" to "normal"
static const uint8_t seq5[] = { 1, 0xf5 };	// auto-increment char position
static const uint8_t seq6[] = { 16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xa0 };	// write blanks (clear) display
#else	// M68004 mode
static const uint8_t seq1[] = { 1, 0xfd };	// set to M68004 mode (redundant)
static const uint8_t seq2[] = { 1, 0x07 };	// set length 16 digits
static const uint8_t seq3[] = { 1, 0x0f };	// set brightness to 14/16
static const uint8_t seq4[] = { 1, 0xf1 };	// set "all digit display" to "normal"
static const uint8_t seq5[] = { 1, 0xf5 };	// auto-increment char position
static const uint8_t seq6[] = { 16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xa0 };	// write blanks (clear) display
#endif

const uint8_t *init_sequences[] = {
	seq1, seq2, seq3, seq4, seq5, seq6
    };

M66005::M66005(uint8_t cs) {
    this->_fl_cs = cs;
    this->_pspi = NULL;
}

void M66005::init(SPIClass *pspi) {
unsigned char c;
    pinMode(_fl_cs, OUTPUT);
    _pspi = pspi;
#if 1
    for(uint16_t i = 0 ; i < sizeof(init_sequences)/sizeof(uint8_t *) ; i++) {
	_pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	digitalWrite(_fl_cs, LOW);	// CS pin low, enable data read
	delayMicroseconds(BEGIN_TR_DELAY);
	for(uint8_t j = 1 ; j <= init_sequences[i][0] ; j++) {
	    c = init_sequences[i][j];
	    _pspi->transfer(c);
	    delayMicroseconds(INTRA_CHAR_DELAY);
	}
	digitalWrite(_fl_cs, HIGH);	// CS pin high, disable data read
	_pspi->endTransaction();
	delayMicroseconds(END_TR_DELAY);
    }
#endif
}

void M66005::brightness(uint8_t brightness) {
unsigned char c;
    _pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(_fl_cs, LOW);	// CS pin low, enable data read
    delayMicroseconds(BEGIN_TR_DELAY);
    c = 0xfd;	// set to M66004 mode
    _pspi->transfer(c);
    delayMicroseconds(INTRA_CHAR_DELAY);
    c = 0x08 | (brightness & 0x07);
    _pspi->transfer(c);
    delayMicroseconds(INTRA_CHAR_DELAY);
    digitalWrite(_fl_cs, HIGH);	// CS pin high, disable data read
    _pspi->endTransaction();
}

// all off = 0, normal operation = 0x01, all on = 0x03
void M66005::all_segments(uint8_t off_norm_on) {
unsigned char c;
    _pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(_fl_cs, LOW);	// CS pin low, enable data read
    delayMicroseconds(BEGIN_TR_DELAY);
    c = 0xf0 | (off_norm_on & 0x03);
    _pspi->transfer(c);
    delayMicroseconds(INTRA_CHAR_DELAY);
    digitalWrite(_fl_cs, HIGH);	// CS pin high, disable data read
    _pspi->endTransaction();
}

// pos: 0 means "first" character in the display
// character codes starting with 0000, 0001, 1000, 1110, or 1111 are
// reserved for commands
// "normal" ASCII is outside of these ranges (starts with 0010 through 0111)
// in other words, character codes are (0x20 through 0x7f and 0xa0 through 0xdf)
void M66005::write_string(const char *str, int len, int pos) {
    _pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(_fl_cs, LOW);
#if RT_TO_LEFT_MODE
    pos = (15 + ICONS_OFFSET) - (len + pos);
#endif
    delayMicroseconds(BEGIN_TR_DELAY);

    _pspi->transfer(0xf5);	// auto increment char position
    delayMicroseconds(INTRA_CHAR_DELAY);
    
    _pspi->transfer(0xe0 | (uint8_t)pos);	// set cursor to "pos"
    delayMicroseconds(INTRA_CHAR_DELAY);
#if RT_TO_LEFT_MODE
    // reverse the string here
    for(int i = len - 1 ; i >= 0 ; i--)
#else
    for(int i = 0 ; i < len ; i++)
#endif
    {
	_pspi->transfer(str[i]);
	delayMicroseconds(INTRA_CHAR_DELAY);
    }
    
    digitalWrite(_fl_cs, HIGH);
    _pspi->endTransaction();
    delayMicroseconds(END_TR_DELAY);
}

void M66005::clear(void) {
int pos;
    _pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(_fl_cs, LOW);
    pos = 0;
    delayMicroseconds(BEGIN_TR_DELAY);

    _pspi->transfer(0xf5);	// auto increment char position
    delayMicroseconds(INTRA_CHAR_DELAY);
    
    _pspi->transfer(0xe0 | (uint8_t)pos);	// set cursor to "pos"
    delayMicroseconds(INTRA_CHAR_DELAY);
    for(int i = 0 ; i < 16 ; i++)
    {
	_pspi->transfer(' ');
	delayMicroseconds(INTRA_CHAR_DELAY);
    }

    digitalWrite(_fl_cs, HIGH);
    _pspi->endTransaction();
    delayMicroseconds(END_TR_DELAY);
}

void M66005::define_char(int ramaddress, const char * fivebyseven) {
    _pspi->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(_fl_cs, LOW);
    delayMicroseconds(BEGIN_TR_DELAY);
    _pspi->transfer(0xfc);	// write to ram
    delayMicroseconds(INTRA_CHAR_DELAY);
    _pspi->transfer(ramaddress);	// address in ram
    delayMicroseconds(INTRA_CHAR_DELAY);
    for(int i = 0 ; i < 5 ; i++)
    {
	_pspi->transfer(fivebyseven[i]);
	delayMicroseconds(INTRA_CHAR_DELAY);
    }
    digitalWrite(_fl_cs, HIGH);
    _pspi->endTransaction();
    delayMicroseconds(END_TR_DELAY);
}
