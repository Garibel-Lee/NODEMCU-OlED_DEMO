#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
#include <Blinker.h>
char auth[] = ""; //点灯的密钥
char ssid[] = ""; //wifi的SSID
char pswd[] = ""; //wifi的密码
WiFiUDP udp;
bool oState = false;
void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);
    if (state == BLINKER_CMD_ON) 
    {
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
         pcawaking();
         oState = false;
    }
    else if (state == BLINKER_CMD_OFF) 
    {
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        oState = false;
    }
}
void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);
    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;     
    }
}
void pcawaking()
{
    int i=0;
    char mac[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  //电脑网卡的mac地址
    char pac[102];
    char * Address = "192.168.1.255";//局域网地址
    int Port = 3333;
    for(i=0;i<6;i++)
    {
      pac[i]=0xFF;
    }
    for(i=6;i<102;i+=6)
    {
      memcpy(pac+i,mac,6);
    }
    udp.beginPacket(Address, Port);
    udp.write((byte*)pac, 102);
    udp.endPacket();
}
void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    Blinker.begin(auth, ssid, pswd);
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachQuery(miotQuery);
}
void loop()
{
    Blinker.run();
}
