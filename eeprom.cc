#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "spidev_lib++.h"

// For AT25F1024
const uint32_t EEPROM_SIZE = 1024u*1024u/8u; // 1Mbits 
const uint32_t EEPROM_SECTOR_SIZE = 0x8000u;  

void show(SPI *mySPI, uint32_t addr) 
{
   uint8_t tx_buffer[32];
   uint8_t rx_buffer[32];
    memset(tx_buffer,0,sizeof(tx_buffer));
    memset(rx_buffer,0,sizeof(rx_buffer));
    tx_buffer[0] = 0x03;	// READ
    tx_buffer[1] = (addr >> 16u) & 0xffu; // msb
    tx_buffer[2] = (addr >> 8u) & 0xffu;  //
    tx_buffer[3] = addr & 0xffu;          // lsb
    //printf("sending %02x\n",tx_buffer[0]); 
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),rx_buffer,sizeof(rx_buffer));
    printf("read[%02x]\n",sizeof(rx_buffer));
    for (uint32_t i = 4; i < sizeof(rx_buffer); ++i) {
       printf(" %02x",rx_buffer[i]);
    }
    printf("\n");
}

void enable(SPI *mySPI,bool enable) 
{
   uint8_t tx_buffer[1];
    memset(tx_buffer,0,sizeof(tx_buffer));
    tx_buffer[0] = enable ? 0x06: 0x04;	// WREN/-DI
    //printf("enable %02x\n",tx_buffer[0]); 
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),nullptr,0);
}

uint8_t statusread(SPI *mySPI) 
{
   uint8_t tx_buffer[2];
   uint8_t rx_buffer[2];
    memset(tx_buffer,0,sizeof(tx_buffer));
    memset(rx_buffer,0,sizeof(rx_buffer));
    tx_buffer[0] = 0x05;	// RDSR
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),rx_buffer,sizeof(rx_buffer));
    return rx_buffer[1];
}

void wait(SPI *mySPI) 
{
   int i = 0;
   while (i < 1000) {
      uint8_t stat = statusread(mySPI);
      if ((stat & 0x01) == 0x0) {
	 break;
      }
      ++i;
      usleep(10000ul); // 10ms
   }
   if (i > 100) {
       printf("Wait took %d tries\n", i);
   }
}

// addr will always be interpreted as a multiple of 256
void erase_sector(SPI* mySPI, uint32_t addr)
{
   uint8_t tx_buffer[4];
   enable(mySPI,true);
   tx_buffer[0] = 0x52;	// sector erase
   tx_buffer[1] = (addr >> 16u) & 0xffu; // msb
   tx_buffer[2] = (addr >> 8u) & 0xffu;  //
   tx_buffer[3] = addr & 0xffu;          // lsb
   mySPI->xfer(tx_buffer,sizeof(tx_buffer),nullptr,0);
   printf("erase addr %06x\n",addr);
   wait(mySPI);
}


// it is expected that WPEN has been disabled
// addr will always be interpreted as a multiple of 256
void write(SPI *mySPI, FILE *f, uint32_t addr) 
{
   uint8_t tx_buffer[4u+256u];
   uint32_t write = 0u;
   uint32_t last_sector = 0x1000;
   memset(tx_buffer,0,sizeof(tx_buffer));
   while (write < EEPROM_SIZE) {
      uint32_t sector = addr / EEPROM_SECTOR_SIZE;
      if (sector != last_sector) {
          erase_sector(mySPI, addr);
	  last_sector = sector;
      }
      enable(mySPI,true);
      tx_buffer[0] = 0x02;	// PROGRAM
      tx_buffer[1] = (addr >> 16u) & 0xffu; // msb
      tx_buffer[2] = (addr >> 8u) & 0xffu;  //
      tx_buffer[3] = addr & 0xffu;          // lsb
      uint32_t size = fread(&tx_buffer[4], 1, sizeof(tx_buffer)-4u, f);
      if (size == 0) {
         break;
      }
      printf("write addr %06x\n",addr);
      mySPI->xfer(tx_buffer,4u+size,nullptr,0);
      addr += size;
      write += size;
      wait(mySPI);
   }
}

// size will always be interpreted as a multiple of 256
void read(SPI *mySPI, FILE *f, uint32_t addr, uint32_t size) 
{
   uint8_t tx_buffer[4u+256];
   uint8_t rx_buffer[4u+256];
   uint32_t read = 0u;
   memset(tx_buffer,0,sizeof(tx_buffer));
   while (read < size) {
      memset(rx_buffer,0,sizeof(rx_buffer));
      tx_buffer[0] = 0x03;	// READ
      tx_buffer[1] = (addr >> 16u) & 0xffu; // msb
      tx_buffer[2] = (addr >> 8u) & 0xffu;  //
      tx_buffer[3] = addr & 0xffu;          // lsb
      mySPI->xfer(tx_buffer,sizeof(tx_buffer),rx_buffer,sizeof(rx_buffer));
      printf("read addr %06x\n",addr);
      fwrite(&rx_buffer[4], 1, sizeof(rx_buffer)-4u, f);
      addr += sizeof(rx_buffer)-4u;
      read += sizeof(rx_buffer)-4u;
   }
}



void statuswrite(SPI *mySPI,uint8_t status) 
{
    enable(mySPI,true);
    uint8_t tx_buffer[2];
    memset(tx_buffer,0,sizeof(tx_buffer));
    tx_buffer[0] = 0x01;	// WRSR
    tx_buffer[1] = status;
    printf("status wr  %02x\n",tx_buffer[1]); 
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),nullptr,0);
    enable(mySPI,false);
}


void rdid(SPI *mySPI) 
{
   uint8_t tx_buffer[3];
   uint8_t rx_buffer[3];
    memset(tx_buffer,0,sizeof(tx_buffer));
    memset(rx_buffer,0,sizeof(rx_buffer));
    tx_buffer[0] = 0x15;	// RDID
    //printf("sending %02x\n", tx_buffer[0]); 
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),rx_buffer,sizeof(rx_buffer));
    printf("rdid\n");
    for (uint32_t i = 1; i < sizeof(rx_buffer); ++i) {
       printf(" %02x",rx_buffer[i]);
    }
    printf("\n");
}

void erase(SPI *mySPI)
{
    statuswrite(mySPI,0x00); // wr enable
    enable(mySPI,true);
    wait(mySPI);
    uint8_t tx_buffer[1];
    tx_buffer[0] = 0x62;	// Chip erase
    printf("erase %02x\n", tx_buffer[0]); 
    mySPI->xfer(tx_buffer,sizeof(tx_buffer),nullptr,0);
    wait(mySPI);
    enable(mySPI,false);
    wait(mySPI);
    statuswrite(mySPI,0x80); // wr protect
    wait(mySPI);
}

int main(int argc,char **argv)
{
  spi_config_t spi_config;
  spi_config.mode=0;
  spi_config.speed=1000000;
  spi_config.delay=0;
  spi_config.bits_per_word=8;
  bool info = false;

  SPI mySPI("/dev/spidev0.0",&spi_config);
  if (mySPI.begin()) {
    rdid(&mySPI);
    printf("status rd %02x\n", statusread(&mySPI));
    if (argc > 1 && strcmp(argv[1],"e") == 0) {
       erase(&mySPI);
    }
    else if (argc > 2 && strcmp(argv[1],"r") == 0) {
	FILE* f = fopen(argv[2],"w");
        read(&mySPI,f,0u,EEPROM_SIZE);
	fclose(f);
    }
    else if (argc > 2 && strcmp(argv[1],"w") == 0) {
	FILE* f = fopen(argv[2],"r");
        write(&mySPI,f,0u);
	fclose(f);
    }
    else if (argc > 1 && strcmp(argv[1],"p") == 0) {
        statuswrite(&mySPI,0x80); // wr protect
        wait(&mySPI);
    }
    else if (argc > 1 && strcmp(argv[1],"u") == 0) {
        statuswrite(&mySPI,0x00); // wr enable
        wait(&mySPI);
    }
    else {
        info = true;
    }
    printf("status rd %02x\n", statusread(&mySPI));
    show(&mySPI,0u);
    if (info) {
       printf("command line parameters:\n");
       printf(" e      erase eeprom\n");
       printf(" r file read eeprom\n");
       printf(" w file write eeprom\n");
       printf(" p      protect eeprom\n");
       printf(" u      unprotect eeprom\n");
    }
    return 0;
  }
  else {
    printf("No device?\n");
  }
  return 1;
}
