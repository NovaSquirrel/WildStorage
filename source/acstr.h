/*
Table to translate from ACSTR (Animal Crossing: Wild World character
encoding) to ASCII.  Based on source code of AnimalMap from
aibohack.com

By Damian Yerrick. No rights reserved and ABSOLUTELY NO WARRANTY.
*/

#ifndef ACSTR_H
#define ACSTR_H
typedef unsigned char ACSTR;
char *acstrDecode(char *restrict dst,
                  const ACSTR *restrict src,
                  int srcLen);
#endif
