//-----------------------------------------------------------------------------
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// CRC16
//-----------------------------------------------------------------------------
#include "crc16.h"

static uint16_t crc_table[256];
static bool crc_table_init = false;
static CrcType_t crc_type = CRC_NONE;

void init_table(CrcType_t ct) {
	
	// same crc algo, and initialised already
	if ( ct == crc_type && crc_table_init) 
		return;
	
	// not the same crc algo. reset table.
	if ( ct != crc_type)
		reset_table();
	
	crc_type = ct;
	
	switch (ct) {
		case CRC_14A:
		case CRC_14B:
		case CRC_15:
		case CRC_15_ICLASS: generate_table(CRC16_POLY_CCITT, true); break;
		case CRC_FELICA: generate_table(CRC16_POLY_CCITT, false); break;
		case CRC_LEGIC: generate_table(CRC16_POLY_LEGIC, true); break;
		case CRC_DNP: generate_table(CRC16_POLY_DNP, true); break;
		case CRC_CCITT: generate_table(CRC16_POLY_CCITT, false); break;
		default:
			crc_table_init = false;
			crc_type = CRC_NONE;
			break;
	}
}

void generate_table( uint16_t polynomial, bool refin) {

	uint16_t i, j, crc, c;

	for (i = 0; i < 256; i++) {
		crc = 0;
		if (refin)
			c = reflect8(i) << 8;
		else
			c = i << 8;

        for (j = 0; j < 8; j++) {

            if ( (crc ^ c) & 0x8000 )
				crc = ( crc << 1 ) ^ polynomial;
            else
				crc =   crc << 1;

            c = c << 1;
        }
		if (refin) 
			crc = reflect16(crc);
		
		crc_table[i] = crc;
	}
    crc_table_init = true;
}

void reset_table(void) {
	memset(crc_table, 0, sizeof(crc_table));
	crc_table_init = false;
	crc_type = CRC_NONE;
}

// table lookup LUT solution
uint16_t crc16_fast(uint8_t const *d, size_t n, uint16_t initval, bool refin, bool refout) {

	// fast lookup table algorithm without augmented zero bytes, e.g. used in pkzip.
	// only usable with polynom orders of 8, 16, 24 or 32.
	if (n == 0)
        return (~initval);
	
	uint16_t crc = initval;

	if (refin) 
		crc = reflect16(crc);

	if (!refin) 
		while (n--) crc = (crc << 8) ^ crc_table[ ((crc >> 8) ^ *d++) & 0xFF ];
	else 
		while (n--) crc = (crc >> 8) ^ crc_table[ (crc & 0xFF) ^ *d++];

	if (refout^refin) 
		crc = reflect16(crc);
	
	return crc;
}

// bit looped solution
uint16_t update_crc16_ex( uint16_t crc, uint8_t c, uint16_t polynomial ) {
	uint16_t i, v, tmp = 0;

	v = (crc ^ c) & 0xff;
	
	for (i = 0; i < 8; i++) {
		
		if ( (tmp ^ v) & 1 )
			tmp = ( tmp >> 1 ) ^ polynomial;
		else
			tmp >>= 1;
		
		v >>= 1;
	}
	return ((crc >> 8) ^ tmp) & 0xffff;
}
uint16_t update_crc16( uint16_t crc, uint8_t c ) {
	return update_crc16_ex( crc, c, CRC16_POLY_CCITT);
}

// two ways.  msb or lsb loop.
uint16_t crc16(uint8_t const *d, size_t length, uint16_t remainder, uint16_t polynomial, bool refin, bool refout) {
	if (length == 0)
        return (~remainder);

	uint8_t c;
    for (uint32_t i = 0; i < length; ++i) {
		c = d[i];
		if (refin) c = reflect8(c);

		// xor in at msb
        remainder ^= (c << 8);
		
		// 8 iteration loop		
        for (uint8_t j = 8; j; --j) {
            if (remainder & 0x8000) {
                remainder = (remainder << 1) ^ polynomial;
            } else {
                remainder <<=  1;
            }
        }		
    }
	if (refout) 
		remainder = reflect16(remainder);
	
    return remainder;
}

//poly=0x1021  init=0xffff  refin=false  refout=false  xorout=0x0000  check=0x29b1  residue=0x0000  name="CRC-16/CCITT-FALSE"
uint16_t crc16_ccitt(uint8_t const *d, size_t n) {
	return crc16_fast(d, n, 0xffff, false, false);
}
//poly=0x1021  init=0x0000  refin=true  refout=true  xorout=0x0000 name="KERMIT"
uint16_t crc16_kermit(uint8_t const *d, size_t n) {
	return crc16_fast(d, n, 0x0000, true, true);
}
// FeliCa uses XMODEM
//poly=0x1021  init=0x0000  refin=false  refout=false  xorout=0x0000 name="XMODEM"
uint16_t crc16_xmodem(uint8_t const *d, size_t n) {
	return crc16_fast(d, n, 0x0000, false, false); 
}

// Following standards uses X-25
//   ISO 15693,
//   ISO 14443 CRC-B
//   ISO/IEC 13239 (formerly ISO/IEC 3309)
//poly=0x1021  init=0xffff  refin=true  refout=true  xorout=0xffff name="X-25"
uint16_t crc16_x25(uint8_t const *d, size_t n) {	
	uint16_t crc = crc16_fast(d, n, 0xffff, true, true);
	crc = ~crc;
	return crc;
}
//    CRC-A (14443-3)
//poly=0x1021 init=0xc6c6 refin=true refout=true xorout=0x0000 name="CRC-A"
uint16_t crc16_a(uint8_t const *d, size_t n) {	
	return crc16_fast(d, n, 0xC6C6, true, true);
}

// iClass crc
// initvalue  0x4807 reflected 0xE012
// poly       0x1021 reflected 0x8408
// poly=0x1021  init=0x4807  refin=true  refout=true  xorout=0x0BC3  check=0xF0B8  name="CRC-16/ICLASS"
uint16_t crc16_iclass(uint8_t const *d, size_t n) {
	return crc16_fast(d, n, 0x4807, true, true);
}

// This CRC-16 is used in Legic Advant systems. 
// poly=0xB400,  init=depends  refin=true  refout=true  xorout=0x0000  check=  name="CRC-16/LEGIC"
uint16_t crc16_legic(uint8_t const *d, size_t n, uint8_t uidcrc) {
	//uint16_t initial = reflect8(uidcrc);
	//initial |= initial << 8;
	uint16_t initial = uidcrc << 8 | uidcrc;
	return crc16_fast(d, n, initial, true, true);
}

// poly=0x3d65  init=0x0000  refin=true  refout=true  xorout=0xffff  check=0xea82  name="CRC-16/DNP"
uint16_t crc16_dnp(uint8_t const *d, size_t n) {
	uint16_t crc = crc16_fast(d, n, 0, true, true);
	crc = ~crc;
	return crc;
}


// -----------------CHECK functions.
bool check_crc16_ccitt(uint8_t const *d, size_t n) {
	if (n < 3) return false;

	uint16_t crc = crc16_ccitt(d, n - 2);	
	if ((( crc & 0xff ) == d[n-2]) && (( crc >> 8 ) == d[n-1]))
		return true;
	return false;
}