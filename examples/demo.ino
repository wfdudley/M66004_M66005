#include <Arduino.h>
#include <SPI.h>
#include <M66005.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS   15

SPIClass *hspi = NULL;

M66005 vfd(HSPI_CS);

void setup (void) {
    Serial.begin(115200);
    delay(100);
    Serial.println(F("demo setup()"));
    hspi = new SPIClass(HSPI);
    hspi->begin();
    hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS); //SCLK, MISO, MOSI, SS
    vfd.init(hspi);
    vfd.brightness(0x0c);
}

int32_t loopctr;

#define POS_LEN_TEST 1
#define ICONS_TEST 0
#define BRIGHTNESS_TEST 1

// on my display, the 16th digit is all the annunciators! (speaker A, B, etc)

char *t1[] = {
    "                ",
#if POS_LEN_TEST
    ".A              ",
    ".AB             ",
    ".ABC            ",
    ".ABCD           ",
    ".ABCDE          ",
    ".ABCDEF         ",
    ".ABCDEFG        ",
    ".ABCDEFGH       ",
    ".ABCDEFGHI      ",
    ".ABCDEFGHIJ     ",
    ".ABCDEFGHIJK    ",
    ".ABCDEFGHIJKL   ",
    ".ABCDEFGHIJKLM  ",
    ".ABCDEFGHIJKLMN ",
#endif
    ".ABCDEFGHIJKLMNO",
};

#if ICONS_TEST
    const char backwardsf[] = { 0x80, 0x90, 0x90, 0x90, 0xfe };
    char icon_test_16[16][5];
#endif

void wait(void) {
    while(!Serial.available()) ;
    Serial.read();
}

void loop (void) {
char flstr[20];
int st;
#if 0
    Serial.printf("%d\n", loopctr);
    sprintf(flstr, "%15d", loopctr++);
    vfd.write_string(flstr, 15, 1);
#endif
#if POS_LEN_TEST
    for(int i = 0 ; i < sizeof(t1)/sizeof(char *) ; i++) {
	vfd.write_string(t1[i], 16, 1);
	Serial.println(t1[i]);
	delay(100);
    }
    vfd.write_string(t1[0], 15, 1);
    flstr[1] = '\0';
    flstr[2] = '\0';
#if 1
    for(int i = 0 ; i < 15 ; i++) {
	flstr[0] = 'A' + i;
	// flstr[1] = 'x';
	vfd.write_string(flstr, 1, i + 1);
	Serial.println(flstr);
	delay(100);
    }
    vfd.clear();
    vfd.write_string("FooB", 4, 4);
    delay(1000);
#endif
    vfd.clear();
    vfd.write_string("FooBaz", 6, 4);
    delay(1000);
#endif
#if ICONS_TEST
    vfd.clear();
#if ALL_ON_TEST
    flstr[0] = 0x7f;	// all icons on
    strcat(flstr, " nothing here");
    vfd.write_string(flstr, strlen(flstr), 0);
    delay(1000);
    vfd.clear();
    vfd.define_char(0, backwardsf);
    flstr[0] = 0x90;	// first char in ram
    vfd.write_string(flstr, strlen(flstr), 1);
    delay(1000);
#endif
    memset(icon_test_16, '\0', 16 * 5);
    memset(flstr, '\0', 20);
    st = 0;
    for(int ch = st ; ch < 16 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	icon_test_16[ch][col] = 2 << row;
	vfd.define_char(ch - st, icon_test_16[ch - st]);
	flstr[ch - st] = 0x90 + (ch - st);
    }
    vfd.write_string(flstr, strlen(flstr), 0);
    delay(1000);
    for(int ch = st ; ch < 16 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	unsigned char x = (unsigned char)(0x90 + ch - st);
	sprintf(flstr, "%c%c r %d c %d n %d", x, x, row, col, ch);
	Serial.printf("%2x row %d col %d ch %d\n", x, row, col, ch);
	vfd.write_string(flstr, strlen(flstr), 0);
	wait();
    }
    memset(icon_test_16, '\0', 16 * 5);
    memset(flstr, '\0', 20);
    st = 16;
    for(int ch = st ; ch < 16 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	icon_test_16[ch - st][col] = 2 << row;
	vfd.define_char(ch - st, icon_test_16[ch - st]);
	flstr[ch - st] = 0x90 + (ch - st);
    }
    vfd.write_string(flstr, strlen(flstr), 0);
    delay(1000);
    for(int ch = st ; ch < 16 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	unsigned char x = (unsigned char)(0x90 + ch - st);
	sprintf(flstr, "%c%c r %d c %d n %d", x, x, row, col, ch);
	Serial.printf("%2x row %d col %d ch %d\n", x, row, col, ch);
	vfd.write_string(flstr, strlen(flstr), 0);
	wait();
    }
    memset(icon_test_16, '\0', 16 * 5);
    memset(flstr, '\0', 20);
    st = 32;
    for(int ch = st ; ch < 4 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	icon_test_16[ch - st][col] = 2 << row;
	vfd.define_char(ch - st, icon_test_16[ch - st]);
	flstr[ch - st] = 0x90 + (ch - st);
    }
    vfd.write_string(flstr, strlen(flstr), 0);
    delay(1000);
    for(int ch = st ; ch < 4 + st ; ch++) {
	int row = ch % 7;
	int col = ch / 7;
	unsigned char x = (unsigned char)(0x90 + ch - st);
	sprintf(flstr, "%c%c r %d c %d n %d", x, x, row, col, ch);
	Serial.printf("%2x row %d col %d ch %d\n", x, row, col, ch);
	vfd.write_string(flstr, strlen(flstr), 0);
	wait();
    }
    sprintf(flstr, "%c%c all icons    ", 0x7f, 0x7f);
    vfd.write_string(flstr, strlen(flstr), 0);
    wait();
#endif
#if BRIGHTNESS_TEST
    vfd.clear();
    sprintf(flstr, "%c%c all icons    ", 0x7f, 0x7f);
    vfd.write_string(flstr, strlen(flstr), 0);
    vfd.brightness(loopctr++);
    if(loopctr > 7) { loopctr = 0; }
    delay(500);
#endif
}
