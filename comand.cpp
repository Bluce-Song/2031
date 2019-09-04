#include "comand.h"

comand::comand()
{
    w_port = new QSerialPort();
}

unsigned char comand::getChecksum(unsigned char *info, int start, int bytes)
{
    int i;
    int checksum = 0;

    for(i=start;i<(bytes+start);i++)
    {
        checksum += info[i];
    }
    return checksum&0xff;
}

/*********************************
***方法名：touch
***说明：握手命令
********************************/
int comand::touch(int port)
{
   char cmd[6];

   cmd[0] = 0x7B;
   cmd[1] = 0x06;
   cmd[2] = 0x00;
   cmd[3] = 0x00;
   cmd[4] = 0x06;    //校验和
   cmd[5] = 0x7D;

   w_port->write(cmd);

}

/*********************************/
/***方法名：machineState  *********/
/***说明：读取状态         *********/
/********************************/
int comand::machineState()
{
    int i;
    unsigned char cmd[6];

    cmd[0] = 0x7B;
    cmd[1] = 0x06;
    cmd[2] = 0x00;
    cmd[3] = 0x03;
    cmd[4] = 0x09;    //校验和
    cmd[5] = 0x7D;



    return i;
}

int comand::startTest()
{


}
