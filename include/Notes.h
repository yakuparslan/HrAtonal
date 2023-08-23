#ifndef NOTES_H__
#define NOTES_H__
const int notes_on[16] = {0, 125, 250, 375, 500, 625, 750, 875, 1000, 1125, 1250, 1375, 1500, 1625, 1750, 1875};
const int notes_off[16] = {125, 250, 375, 500, 625, 750, 875, 1000, 1125, 1250, 1375, 1500, 1625, 1750, 1875, 2000};
const int array_length = sizeof(notes_on)/sizeof(notes_on[0]);
#endif
