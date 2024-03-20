/*
Table to translate from ACSTR (Animal Crossing: Wild World character
encoding) to Latin-1.  Based on source code of AnimalMap from
aibohack.com

By Damian Yerrick. No rights reserved and ABSOLUTELY NO WARRANTY.
*/

#include "acstr.h"

static const char acstrToLatin1[256][4] = {

// 00-1F
  "$00","A",  "B",  "C",  "D",  "E",  "F",  "G",
  "H",  "I",  "J",  "K",  "L",  "M",  "N",  "O",
  "P",  "Q",  "R",  "S",  "T",  "U",  "V",  "W",
  "X",  "Y",  "Z",  "a",  "b",  "c",  "d",  "e",

// 20-3F
  "f",  "g",  "h",  "i",  "j",  "k",  "l",  "m",
  "n",  "o",  "p",  "q",  "r",  "s",  "t",  "u",
  "v",  "w",  "x",  "y",  "z",  "0",  "1",  "2",
  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "$3F",

// 40-5F
  "$40","Œ",  "$42","$43","œ",  "$45","$46","À",
  "Á",  "Â",  "$4A","Ä",  "$4C","$4D","Ç",  "È",
  "É",  "Ê",  "Ë",  "Ì",  "Í",  "Î",  "Ï",  "$57",
  "Ñ",  "Ò",  "Ó",  "Ô",  "$5C","Ö",  "$5E","Ù",

// 60-7F
  "Ú",  "Û",  "Ü",  "$63","$64","ß",  "à",  "á",
  "â",  "$69","ä",  "$6B","$6C","ç",  "è",  "é",
  "ê",  "ë",  "ì",  "í",  "î",  "ï",  "$76","ñ",
  "ò",  "ó",  "ô",  "$7B","ö",  "$7D","ù",  "ú",

// 80-9F
  "û",  "ü",  "$82","$83","$84"," ",  "\\n","!",
  "\"", "$89","$8A","$8B","&",  "'",  "(",  ")",
  "$90","+",  ",",  "-",  ".",  "/",  ":",  ";",
  "<",  "=",  ">",  "?",  "@",  "[",  "$9E","]",

// Letters from game characters use the apostrophe 8D.
// But one apostrophe I typed ended up as a B1.

// A0-FF
  "$A0","_",  "$A2","$A3","$A4","$A5","~",  "€",
  "$A8","$A9","$AA","$AB","$AC","$AD","$AE","$AF",
  "$B0","'",  "$B2","$B3","$B4","$B5","$B6","$B7",
  "$B8","$B9","$BA","¡",  "¢",  "£",  "$BE","$BF",
  "$C0","$C1","$C2","$C3","$C4","$C5","$C6","$C7",
  "$C8","$C9","$CA","$CB","$CC","$CD","$CE","$CF",
  "$D0","dot","$D2","$D3","$D4","$D5","$D6","$D7",
  "$D8","¿",  "×",  "÷",  "drp","*",  "<3", "$DF",
  "$E0","$E1","$E2","$E3","$E4","$E5","$E6","$E7",
  "$E8","$E9","$EA","$EB","$EC","$ED","$EE","$EF",
  "$F0","$F1","$F2","$F3","$F4","$F5","$F6","$F7",
  "$F8","$F9","$FA","$FB","$FC","$FD","$FE","$FF"
};

/**
 * Decodes a string from ACWW encoding to ISO-8859-1 encoding
 * with some Windows-1252 extensions (Euro, oe, OE).
 * @param dst destination; points to a buffer of at least 3*srcLen+1 bytes
 * @param src source; points to a buffer of srcLen bytes
 * @param srcLen maximum length (in AC characters) of string.
 * If srcLen > 0, a NUL in src terminates decoding.
 * If srcLen < 0, a NUL in src is translated to "$00".
 * @return pointer to NUL terminator in dst, as with stpcpy()
 */
char *acstrDecode(char *restrict dst,
                 const ACSTR *restrict src,
                 int srcLen) {
  int withNuls = srcLen < 0;
  if (srcLen < 0) {
    srcLen = -srcLen;
  }
  while (srcLen > 0) {
    int c = *(unsigned char *)src++;
    if (!c && !withNuls) {
      break;
    }
    const char *copySrc = acstrToLatin1[c];
    while (*copySrc) {
      *dst++ = *copySrc++;
    }
    --srcLen;
  }
  *dst = 0;
  return dst;
}
