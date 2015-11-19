#include "MultiWiiProtocol.h"
#include <stdlib.h>
#include <stdio.h>

//Generates a frame according to the MSP format. DATA SHOULD BE FREED
//Gera um quadro no formato aceito pelo MSP. CHAMAR FREE SOBRE O RETORNO

//Each [] is an octet
//Format -> ['$']['M']['<'][n_bytes][opcode]n_bytes*[data] [checksum]

void get_msp(unsigned char opcode, unsigned char* data, unsigned char n_bytes, unsigned char* msp_message)
{
    //Minimum number of bytes on a frame
    int cont = 6;
    unsigned char checksum = 0;
//    unsigned char* ret =(unsigned char*)malloc(50*sizeof(unsigned char));

    //Insert the header
    msp_message[0] = '$';
    msp_message[1] = 'M';
    msp_message[2] = '<';

    //Insert the number of bytes to be sent along with the message
    msp_message[3] = (n_bytes&0xff);
    checksum ^= (n_bytes&0xff);

    //Insert opcode
    msp_message[4] = opcode;
    checksum ^= opcode;
    //insert data
    int i = 0;
    for(i = 0; i < n_bytes; i++)
    {
        msp_message[i+5] = (data[i]);
        checksum ^= data[i];
        cont++;
    }
    // insert the checksum
    msp_message[i+5] = checksum;
  //  printf("check %d\n", checksum);
//    return msp_message;
}
