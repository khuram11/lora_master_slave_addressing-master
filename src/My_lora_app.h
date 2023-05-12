#include <stdint.h>


#define pin_for_slave_1 3
#define pin_for_slave_2 4
#define pin_for_slave_3 5
#define pin_for_slave_4 6
#define pin_for_slave_5 7
#define pin_for_slave_6 8
#define pin_for_slave_7 9

#define master_Address 0xBB
#define no_of_slaves 7
byte Slave_Address[no_of_slaves]={0xB2,0xCC,0xBA,0xAB,0xA1,0xB0,0xB8};
#define cooldowm_time 1000

#define time_for_ack_resend  5000
#define no_of_resend  3
uint8_t Slave_to_send_message=0 ; 
bool doSendmessage=false;




void disp_list();



typedef enum 
{
    Waiting_for_ISR=0,
    Slave_1,
    Slave_2,
    Slave_3,
    Slave_4,
    Slave_5,
    Slave_6,
    Slave_7,
}slaves_t; 