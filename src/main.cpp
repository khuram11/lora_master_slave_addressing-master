/*
  Heltec.LoRa Multiple Communication

  This example provide a simple way to achieve one to multiple devices
  communication.

  Each devices send datas in broadcast method. Make sure each devices
  working in the same BAND, then set the localAddress and destination
  as you want.
  
  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFD as the broadcast address. You can set this address as you
  want.

  Note: while sending, Heltec.LoRa radio is not listening for incoming messages.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
#include "My_lora_app.h" 
// #include <stdbool.h>


volatile byte destination = 0xFD;      // destination to send to

#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6


String payload = "Follow ME !!!"; 






/*******************************ISRs for Each pins
 ************************************************/
void IRAM_ATTR slave_1_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[0];
  doSendmessage=true;

}
void IRAM_ATTR slave_2_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[1];
  doSendmessage=true;

}
void IRAM_ATTR slave_3_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[2];
  doSendmessage=true;

}
void IRAM_ATTR slave_4_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[3];
  doSendmessage=true;

}
void IRAM_ATTR slave_5_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[4];
  doSendmessage=true;

}
void IRAM_ATTR slave_6_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[5];
  doSendmessage=true;
}
void IRAM_ATTR slave_7_ISR()
{
  // This fucntion will be called when Pin1(defined in the header) goes from low to high
  destination= Slave_Address[6];
  doSendmessage=true;
}



String outgoing;              // outgoing message

byte localAddress = master_Address;     // address of this device


byte msgCount = 0;            // count of outgoing messages
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
bool onReceive(int packetSize);
void sendMessage(String outgoing);
void setup()
{
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  pinMode(pin_for_slave_1, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_1), slave_1_ISR, RISING);
  pinMode(pin_for_slave_2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_2), slave_2_ISR, RISING);
  pinMode(pin_for_slave_3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_3), slave_3_ISR, RISING);
  pinMode(pin_for_slave_4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_4), slave_4_ISR, RISING);
  pinMode(pin_for_slave_5, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_5), slave_5_ISR, RISING);
  pinMode(pin_for_slave_6, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_6), slave_6_ISR, RISING);
  pinMode(pin_for_slave_7, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_for_slave_7), slave_7_ISR, RISING);
  Serial.println("Heltec Master");

  /**********For heltec display**********/
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawCircle(64,32,20);
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->display();
  delay(1000);

 
}

void loop()
{
  if(doSendmessage)
  {
    if (millis() - lastSendTime > cooldowm_time)
    {
      sendMessage(payload);
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "sending message");
      Heltec.display->drawString(0, 20, "slave add :");
      char Addr[2]={'\0'};
      sprintf(Addr,"%d",destination);
      Heltec.display->drawString(0, 40, Addr);
      Heltec.display->display();
      Serial.println("Sending " + payload);
      Serial.println("To address " + destination);
      lastSendTime = millis();            // timestamp the message
      Slave_to_send_message= Waiting_for_ISR;
      uint64_t waiting_time= millis();
      uint8_t resend_count=0;
      while (!onReceive)
    {
    //waiting to receive the acknowledgement
    if(millis()-waiting_time>time_for_ack_resend)
    {
      if(resend_count<=no_of_resend)
      {
        sendMessage(payload); 
        resend_count++;
        waiting_time= millis(); 
      }else break;
    }
    //add delay here if system goes to watch-dog reset
    delay(100);
    }
    }
    doSendmessage=false;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  disp_list();
}

void sendMessage(String outgoing)
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

bool onReceive(int packetSize)
{
  if (packetSize == 0) return false;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  {   // check length for error
    Serial.println("error: message length does not match length");
    return false;                             // skip rest of function
  }
 

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return false;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();

   if (strstr((incoming.c_str()),"ACK"))
  {
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"received ack");
  Heltec.display->display();
    return true;
  }else return false;
}



void disp_list(){
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"Slaves List :");
  Heltec.display->drawString(15,0,(String)Slave_Address[0]);
  Heltec.display->drawString(15,64,(String)Slave_Address[1]);
  Heltec.display->drawString(30,0,(String)Slave_Address[2]);
  Heltec.display->drawString(30,64,(String)Slave_Address[3]);
  Heltec.display->drawString(45,0,(String)Slave_Address[4]);
  Heltec.display->drawString(45,64,(String)Slave_Address[5]);
  Heltec.display->drawString(60,0,(String)Slave_Address[6]);
  Heltec.display->display();


}