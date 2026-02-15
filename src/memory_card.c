#include "memory_card.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include "config.h"
#include "ff.h"
#include <string.h>

int8_t memory_card_import(memory_card_t* mc, const char* fileName)
{
    if (!mc || !mc->data)
        return MC_NO_INIT;

    if (!fileName)
        return MC_FILE_OPEN_ERR;

    FIL memcard;
    FRESULT fr = f_open(&memcard, fileName, FA_READ);
    if (fr != FR_OK)
        return MC_FILE_OPEN_ERR;

    // valida tamanho do arquivo
    if (f_size(&memcard) != MC_SIZE) {
        f_close(&memcard);
        return MC_FILE_SIZE_ERR;
    }

    mc->flag_byte = MC_FLAG_BYTE_DEF;

    UINT bytes_read = 0;
    fr = f_read(&memcard, mc->data, MC_SIZE, &bytes_read);
    f_close(&memcard);

    if (fr != FR_OK || bytes_read != MC_SIZE) {
        memset(mc->data, 0, MC_SIZE);
        return MC_FILE_READ_ERR;
    }

    return MC_OK;
}


bool memory_card_is_sector_valid(memory_card_t* mc, sector_t sector) {
	(void) mc;
	if(sector < 0 || sector >= MC_SEC_COUNT)
		return false;
	return true;
}

uint8_t* memory_card_get_sector_ptr(memory_card_t* mc, sector_t sector) {
	if(mc)
		return &mc->data[sector * MC_SEC_SIZE];
	return NULL;
}

void memory_card_reset_seen_flag(memory_card_t* mc) {
	if(mc)
		mc->flag_byte &= ~(1 << 3);
}

/***
 *	Sync memory card modified sectors back into flash storage.
 *	Does not create concurrency problem as it only reads from the in-RAM copy.
 * 	If a sector is being synced while the in-RAM copy is being modified,
 * 	then there is a transient loss of consistency. Consistency is eventually
 * 	resolved since there will be another entry further down the queue
 * 	enforcing the sync for that same sector to occurr once again.
 */
uint32_t memory_card_sync_sector(memory_card_t* mc, sector_t sector, uint8_t* file_name) {
	uint32_t status = MC_OK;
	FIL memcard;

	if(FR_OK == f_open(&memcard, file_name, FA_READ | FA_WRITE)) {
		UINT bytes_written;
		f_lseek(&memcard, (sector * MC_SEC_SIZE));
		if(FR_OK == f_write(&memcard, &mc->data[sector * MC_SEC_SIZE], MC_SEC_SIZE, &bytes_written)) {
			if(MC_SEC_SIZE != bytes_written) {
				status = MC_FILE_SIZE_ERR;
			}
            f_sync(&memcard);   // < ISSO FAZ GRAVAR DE VERDADE
		} else {
			status = MC_FILE_WRITE_ERR;
		}

		f_close(&memcard);
	} else {
		status = MC_FILE_OPEN_ERR;
	}

	return status;
}

