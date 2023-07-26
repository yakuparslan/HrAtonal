#ifndef NOTES_H__
#define NOTES_H__
const char* hostname = "HR02";
const int notes_on[9] = {0, 557, 703, 1500, 2125, 2500, 3125, 3750, 4250};
const int notes_off[9] = {125, 682, 828, 1625, 2250, 2625, 3250, 3875, 4375};
const int array_length = sizeof(notes_on)/sizeof(notes_on[0]);

#endif