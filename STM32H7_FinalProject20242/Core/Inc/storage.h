#ifndef APPS_STORRAGE_STORAGE_H_
#define APPS_STORRAGE_STORAGE_H_
#include "stdlib.h"
#include "global.h"

#define SD_DATA_SECTOR_BEGIN 200
#define SD_DATA_SECTOR_END 30000000

void ReadData(LIFO_inst *q);
void SendData(LIFO_inst *q, uint8_t nmb_send_element);
void SaveData(LIFO_inst *q, char * data);
void SavePointer(LIFO_inst* q);
void LoadPointer(LIFO_inst* q);
uint8_t QueueIsFull(LIFO_inst *q);
uint8_t QueueIsEmpty(LIFO_inst *q);
#endif
