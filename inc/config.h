#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Global configuration options for PicoMemcard */
#define TUD_MOUNT_TIMEOUT	3000			// max time (in ms) before giving up on MSC mode (USB) and starting memcard simulation
#define MSC_WRITE_SYNC_TIMEOUT 1 * 1000		// time (in ms) expired since last MSC write before exporting RAM disk into LFS
#define IDLE_AUTOSYNC_TIMEOUT 5 * 1000		// time (in ms) the memory card must be inactive before automatic sync from RAM to LFS
#define MAX_MC_FILENAME_LEN	64				// max length of memory card file name (including extension)
#define MAX_MC_IMAGES	255					// maximum number of different mc images
#define MC_RECONNECT_TIME	1000			// time (in ms) the memory card stays disconnected when simulating reconnection

#ifdef PICO_1
    //PICO
    #define PIN_MISO	    16
    #define PIN_SS	        17
    #define PIN_MOSI	    19
    #define PIN_SCK	        18
    #define PIN_BTN         3
    #define PIN_DAT         5   // original PIN_DAT su Pi pico
    #define PIN_CMD         6	// original PIN_CMD su Pi pico
    #define PIN_SEL         7	// original PIN_SEL su Pi pico
    #define PIN_CLK         8	// original PIN_CLK su Pi pico
    #define PIN_ACK         9  // original PIN_ACK su Pi pico


    #ifdef SET_GP_LED
        #define PICO_LED_PIN SET_GP_LED
    #else
        #define PICO_LED_PIN 25
    #endif
#else
    //PICO_ZERO
    #define PIN_MISO	    0
    #define PIN_SS		    1
    #define PIN_SCK		    2
    #define PIN_MOSI	    3
    #define PIN_BTN         4
    #define PIN_DAT 9   // original PIN_DAT su rp2040
    #define PIN_CMD 10	// original PIN_CMD su rp2040
    #define PIN_SEL 11	// original PIN_SEL su rp2040
    #define PIN_CLK 12	// original PIN_CLK su rp2040
    #define PIN_ACK 13  // original PIN_ACK su rp2040

    #ifdef SET_GP_LED
        #define PICO_LED_PIN SET_GP_LED
    #endif
#endif


/* SD Card Configuration */
#define BLOCK_SIZE	512				// SD card communicate using only 512 block size for consistency
#define BAUD_RATE	5000 * 1000

#endif
