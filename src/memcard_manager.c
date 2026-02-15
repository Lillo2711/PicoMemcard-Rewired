#include "memcard_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sd_config.h"
#include "memory_card.h"


uint16_t imageCount = 0;
uint16_t imagePosition = 0;
uint16_t imageToPosition = 0;
uint8_t image_names[(MAX_MC_FILENAME_LEN + 1) * MAX_MC_IMAGES];


static inline bool equalsIgnoreCase(const char* a, const char* b)
{
    if (!a || !b) return false;

    while (*a && *b) {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;

        // tolower ASCII
        if (ca >= 'A' && ca <= 'Z') ca |= 0x20;
        if (cb >= 'A' && cb <= 'Z') cb |= 0x20;

        if (ca != cb)
            return false;
    }

    // ambos precisam terminar juntos
    return (*a == '\0' && *b == '\0');
}

static inline bool endsWithIgnoreCase(const char* str, const char* suf){
    if (!str || !suf) return false;
    size_t lenStr = strlen(str);
    size_t lenSuf = strlen(suf);
    // extensão obrigatória
    if (lenSuf == 0 || lenSuf >= lenStr)
        return false;
    const char* p = str + (lenStr - lenSuf);
    const char* s = suf;
    while (*s) {
        unsigned char a = (unsigned char)*p++;
        unsigned char b = (unsigned char)*s++;
        // tolower ASCII
        if (a >= 'A' && a <= 'Z') a |= 0x20;
        if (b >= 'A' && b <= 'Z') b |= 0x20;

        if (a != b) return false;
    }
    return true;
}

static inline int8_t acceptEntry(const FILINFO* e, char* dstFileName)
{
    //0 - Rejeita
    //1 - OK
    //2 - Comprimento do nome deve ser no maximo de 60 caracteres + .mcr
    //3 - Arquivo .mcr danificado

    if (!e || !dstFileName)  return 0;
    if (e->fattrib & AM_HID) return 0; // Rejeita ocultos    
    if (e->fattrib & AM_DIR) return 0; // Rejeita diretórios

    // Escolhe nome: LFN se existir, senão 8.3
    const char* name =  e->fname;
    if (!name) return 0;

    // Extensao obrigatoria .mcr (case-insensitive)
    if (!endsWithIgnoreCase(name, ".mcr"))
        return 0;

    // Comprimento maximo: MAX_MC_FILENAME_LEN - 1
    size_t nameLen = strlen(name);    
    if (nameLen == 0 || nameLen >= MAX_MC_FILENAME_LEN)
        return 2; //Comprimento do nome deve ser no maximo de 60 caracteres + .mcr

    if (e->fsize != MC_SIZE)
        return 3; //Arquivo .mcr danificado
        
    // Cópia segura + terminador garantido
    memcpy(dstFileName, name, nameLen);
    dstFileName[nameLen] = '\0';

    return 1;
}




bool is_image_valid(const char* filename){
	if(!filename) return false;
	FILINFO f_info;
	FRESULT f_res = f_stat(filename, &f_info);
	if(f_res != FR_OK)
		return false;
	if(f_info.fsize != MC_SIZE)
		return false;
	return true;
}


bool memcard_manager_exist(const char* filename) {
    if (!filename) return false;
    FILINFO f_info;
    return (f_stat(filename, &f_info) == FR_OK);
}

uint32_t memcard_manager_count() {
	FILINFO f_info;
    DIR root;
	FRESULT res = f_opendir(&root, "");	// open root directory
	uint32_t count = 0;
	if(res == FR_OK) {
		while(true) {
			res = f_readdir(&root, &f_info);
			if(res != FR_OK || f_info.fname[0] == 0) break;
			if(!(f_info.fattrib & AM_DIR)) {	// not a directory
				if(is_image_valid(f_info.fname))
					++count;
			}
		}
	}
	return count;
}


/*void saveSelectMC(const char* selectMC) {
    if (!selectMC) return;
    FIL f;
    FRESULT fr = f_open(&f, "selectMC.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK)
        return;

    UINT bw = 0;
    fr = f_write(&f, selectMC, MAX_MC_FILENAME_LEN, &bw);
    if (fr == FR_OK && bw == MAX_MC_FILENAME_LEN) {
        f_sync(&f);
    }

    f_close(&f);
}*/

void saveSelectMC(const char* selectMC) {
    if (!selectMC) return;

    size_t len = strlen(selectMC);
    if (len == 0 || len >= MAX_MC_FILENAME_LEN)
        return;

    FIL f;
    if (f_open(&f, "selectMC.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        return;

    UINT bw;
    f_write(&f, selectMC, len, &bw);
    f_close(&f);
}

bool loadSelectMC(char* out) {
    if (!out) return false;

    FIL f;
    if (f_open(&f, "selectMC.txt", FA_READ) != FR_OK)
        return false;

    UINT br;
    f_read(&f, out, MAX_MC_FILENAME_LEN - 1, &br);
    f_close(&f);

    out[br] = '\0';   // termina string
    return br > 0;
}




int8_t memcard_manager_restart(char*outFileName) {
    FILINFO currentEntry, nextEntry;
    DIR dir; // open root directory
	FRESULT res = f_opendir(&dir, "");	
    if(res != FR_OK) {
        //No SD Card
        return MM_NO_SD_CARD;
    }
    imageCount = 0;
    bool hasNext = false;
	currentEntry.fname[0] = '\0';
    res = f_readdir(&dir, &currentEntry);
    if (res == FR_OK && currentEntry.fname[0] != '\0'){
        res = f_readdir(&dir, &nextEntry);
        hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        while (true){
            if(acceptEntry(&currentEntry, &image_names[MAX_MC_FILENAME_LEN * imageCount])==1){
                if (++imageCount >= MAX_MC_IMAGES){
                    hasNext = 0;
                }
            }
            if (hasNext == 0)
            {
                break;
            }
            currentEntry = nextEntry;
            res = f_readdir(&dir, &nextEntry);
            hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        }
    }
    
    imagePosition = 0;
    if(imageCount<=0){
        //NOVO MEMORY CARD
		int8_t status = memcard_manager_create(outFileName);
		if(status != MM_OK) {
            return status;
		}
        
    }else if(imageCount>1){        
        // Ordem Alfabética
    	qsort(image_names, imageCount, MAX_MC_FILENAME_LEN, (__compar_fn_t) strcmp);

        //Seleciona o ultimo mc usado
        char selectMCTmp[MAX_MC_FILENAME_LEN];
        if(loadSelectMC(selectMCTmp)){
            for(int i=0;i<imageCount;i++){
                const char* name = (char*)&image_names[MAX_MC_FILENAME_LEN * i];
                if (equalsIgnoreCase(name, selectMCTmp)) {
                    imagePosition = i;
                    break;
                }
            }
        }
    }

    memset(outFileName, 0, MAX_MC_FILENAME_LEN);
    memcpy(outFileName, &image_names[MAX_MC_FILENAME_LEN * imagePosition], MAX_MC_FILENAME_LEN-1);
    outFileName[MAX_MC_FILENAME_LEN - 1] = '\0';

    return MM_OK;
}


int8_t memcard_manager_get_position(uint16_t position, char* outFileName) {
	if(position>=imageCount || !outFileName)
		return MM_BAD_PARAM;

    imagePosition = position;
    
    memset(outFileName, 0, MAX_MC_FILENAME_LEN);
    memcpy(outFileName, &image_names[MAX_MC_FILENAME_LEN * imagePosition], MAX_MC_FILENAME_LEN-1);
    outFileName[MAX_MC_FILENAME_LEN - 1] = '\0';
    return MM_OK;
}

uint8_t memcard_manager_create(char*outFileName) {
	if(!outFileName)
		return MM_BAD_PARAM;

    if (imageCount >= MAX_MC_IMAGES)
        return MM_LIMITE_MC;

	uint32_t memcard_n = 1;
    do {
        if (memcard_n+1>= MAX_MC_IMAGES) return MM_LIMITE_MC;
        memset(outFileName, 0, MAX_MC_FILENAME_LEN);
        snprintf(outFileName, MAX_MC_FILENAME_LEN-1, "memorycard%03d.mcr", memcard_n++);
    } while(memcard_manager_exist(outFileName));


	// Generate image file
	FIL memcard_image;
	FRESULT f_res = f_open(&memcard_image, outFileName, FA_CREATE_NEW | FA_WRITE);
	if(f_res == FR_OK) {
		UINT bytes_written = 0;
		uint8_t buffer[MC_SEC_SIZE];
		uint8_t xor;
		// header frame (block 0, sec 0)
		buffer[0] = 'M';
		buffer[1] = 'C';
		xor = buffer[0] ^ buffer[1];
		for(int i = 2; i < MC_SEC_SIZE - 1; i++) {
			buffer[i] = 0;
			xor = xor ^ buffer[i];
		}
		buffer[MC_SEC_SIZE - 1] = xor;
		f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
		if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
			f_close(&memcard_image);
			return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
		}
		// directory frames (block 0, sec 1..15)
		buffer[0] = 0xa0;	// free block
		xor = buffer[0];
		for(int i = 1; i < 8; i++) {
			buffer[i] = 0;
			xor = xor ^ buffer[i];
		}
		buffer[8] = buffer[9] = 0xff;	// no next block
		xor = xor ^ buffer[8] ^ buffer[9];
		for(int i = 10; i < MC_SEC_SIZE - 1; i++) {
			buffer[i] = 0;
			xor = xor ^ buffer[i];
		}
		buffer[MC_SEC_SIZE - 1] = xor;
		for(int i = 0; i < 15; i++) {
			f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
			if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
				f_close(&memcard_image);
				return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
			}
		}
		/* broken sector list (block 0, sec 16..35) */
		buffer[0] = buffer[1] = buffer[2] = buffer[3] = 0xff;	// no broken sector
		xor = buffer[0] ^ buffer[1] ^ buffer[2] ^ buffer[3];
		buffer[4] = buffer[5] = buffer[6] = buffer[7] = 0x00;	// 0 fill
		xor = xor ^ buffer[4] ^ buffer[5] ^ buffer[6] ^ buffer[7];
		buffer[8] = buffer[9] = 0xff;	// 1 fill
		xor = xor ^ buffer[8] ^ buffer[9];
		for(int i = 10; i < MC_SEC_SIZE - 1; i++) {
			buffer[i] = 0x00;
			xor = xor ^ buffer[i];
		}
		buffer[MC_SEC_SIZE - 1] = xor;
		for(int i = 0; i < 20; i++) {
			f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
			if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
				f_close(&memcard_image);
				return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
			}
		}
		/* broken sector replacement data (block 0, sec 36..55) and unused frames (block 0, sec 56..62) */
		for(int i = 0; i < MC_SEC_SIZE; i++) {
			buffer[i] = 0x00;
		}
		for(int i = 0; i < 27; i++) {
			f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
			if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
				f_close(&memcard_image);
				return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
			}
		}
		/* test write sector (block 0, sec 63) */
		buffer[0] = 'M';
		buffer[1] = 'C';
		xor = buffer[0] ^ buffer[1];
		for(int i = 2; i < MC_SEC_SIZE - 1; i++) {
			buffer[i] = 0;
			xor = xor ^ buffer[i];
		}
		buffer[MC_SEC_SIZE - 1] = xor;
		f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
		if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
			f_close(&memcard_image);
			return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
		}
		/* fill remaining 15 blocks with zeros */
		for(int i = 0; i < MC_SEC_SIZE; i++) {
			buffer[i] = 0;
		}
		for(int i = 0; i < MC_SEC_COUNT - 64; i++) {	// 64 are the number of sectors written already (forming block 0)
			f_res = f_write(&memcard_image, buffer, MC_SEC_SIZE, &bytes_written);
			if(f_res != FR_OK || bytes_written != MC_SEC_SIZE) {
				f_close(&memcard_image);
				return MM_NEW_MC_ERR;//MM_FILE_WRITE_ERR;
			}
		}
        f_sync(&memcard_image);
		f_close(&memcard_image);

        // Cópia segura + terminador garantido
        memcpy(&image_names[MAX_MC_FILENAME_LEN * imageCount], outFileName, MAX_MC_FILENAME_LEN-1);
        image_names[MAX_MC_FILENAME_LEN-1] = '\0';        
        imageCount++;

        return MM_OK;
	} else {
		return MM_NEW_MC_ERR;//MM_FILE_OPEN_ERR;
	}
}