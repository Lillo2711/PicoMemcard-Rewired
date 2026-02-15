#ifndef __MEMCARD_MANAGER_H__
#define __MEMCARD_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>

/* Error codes */
#define MM_OK					0
#define MM_NO_SD_CARD			1
#define MM_LIMITE_MC	        2
#define MM_FILE_WRITE_ERR		3
#define MM_FILE_READ_ERR		4
#define MM_FILE_OPEN_ERR		5
#define MM_NEW_MC_ERR		    6

#define MM_BAD_PARAM		    9

extern uint16_t imageCount;
extern uint16_t imagePosition;
extern uint16_t imageToPosition;

bool memcard_manager_exist(const char* filename);
uint32_t memcard_manager_count();
uint32_t memcard_manager_get(uint32_t index, uint8_t* out_filename);
//#define memcard_manager_get_first(out_filename) memcard_manager_get(0, (out_filename))
int8_t memcard_manager_restart();
int8_t memcard_manager_get_position(uint16_t position, char* out_nextfile);
uint8_t memcard_manager_create(char*outFileName);
void saveSelectMC(const char* selectMC);


#endif /* __MEMCARD_MANAGER_H__ */