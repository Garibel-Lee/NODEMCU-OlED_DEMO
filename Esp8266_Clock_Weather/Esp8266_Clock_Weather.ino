#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <ESP8266HTTPClient.h>
#define TCP_SERVER_ADDR "bemfa.com"
#define TCP_SERVER_PORT "8344"

//********************远程开关机需要修改的部分*******************//

#define MACAddress "2C:F0:5D:AC:19:85"  //电脑的MAC地址
String UID = "a8aa48cca59f4f0d287d4f955c209178";  //用户私钥，可在控制台获取,修改为自己的UID
String TOPIC =   "PC001";         //主题名字，可在控制台新建
String computer_ip = "http://192.168.123.48:5001";  //电脑的IPv4地址
String secret_code = "2333";   //远程关机控制码

//**************************************************//


String PCurl = computer_ip+"/"+secret_code+"/"+"shutdown";
//最大字节数
#define MAX_PACKETSIZE 512
//设置心跳值30s
#define KEEPALIVEATIME 30*1000


//tcp客户端相关初始化，默认即可
WiFiClient TCPclient;
String TcpClient_Buff = "";
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;//心跳
unsigned long preTCPStartTick = 0;//连接
bool preTCPConnected = false;
WiFiUDP UDP;
WakeOnLan WOL(UDP);


//相关函数初始化
//检查wifi连接状态
void doWiFiTick();

//TCP初始化连接
void doTCPClientTick();
void startTCPClient();
void sendtoTCPServer(String p);

//开关机命令 控制函数
void openComputer();
void closeComputer();

/*
  *发送数据到TCP服务器
 */
void sendtoTCPServer(String p){
  if (!TCPclient.connected()) 
  {
    Serial.println("Client is not readly");
    return;
  }
  TCPclient.print(p);
  Serial.println("[Send to TCPServer]:String");
  Serial.println(p);
}


/*
  *初始化和服务器建立连接
*/
void startTCPClient(){
  if(TCPclient.connect(TCP_SERVER_ADDR, atoi(TCP_SERVER_PORT))){
    Serial.print("\nConnected to server:");
    Serial.printf("%s:%d\r\n",TCP_SERVER_ADDR,atoi(TCP_SERVER_PORT));
    
    String tcpTemp="";  //初始化字符串
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC+"\r\n"; //构建订阅指令
    sendtoTCPServer(tcpTemp); //发送订阅指令
    tcpTemp="";//清空
    
    preTCPConnected = true;
    preHeartTick = millis();
    TCPclient.setNoDelay(true);
  }
  else{
    Serial.print("Failed connected to server:");
    Serial.println(TCP_SERVER_ADDR);
    TCPclient.stop();
    preTCPConnected = false;
  }
  preTCPStartTick = millis();
}


/*
  *检查数据，发送心跳
*/
void doTCPClientTick(){
 //检查是否断开，断开后重连
   if(WiFi.status() != WL_CONNECTED) return;

  if (!TCPclient.connected()) {//断开重连

  if(preTCPConnected == true){

    preTCPConnected = false;
    preTCPStartTick = millis();
    Serial.println();
    Serial.println("TCP Client disconnected.");
    TCPclient.stop();
  }
  else if(millis() - preTCPStartTick > 1*1000)//重新连接
    startTCPClient();
  }
  else
  {
    if (TCPclient.available()) {//收数据
      char c =TCPclient.read();
      TcpClient_Buff +=c;
      TcpClient_BuffIndex++;
      TcpClient_preTick = millis();
      if(TcpClient_BuffIndex>=MAX_PACKETSIZE - 1){
        TcpClient_BuffIndex = MAX_PACKETSIZE-2;
        TcpClient_preTick = TcpClient_preTick - 200;
      }
      preHeartTick = millis();
    }
    if(millis() - preHeartTick >= KEEPALIVEATIME){//保持心跳
      preHeartTick = millis();
      Serial.println("--Keep alive:");
      sendtoTCPServer("cmd=0&msg=keep\r\n");
    }
  }
  if((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick>=200))
  {
    //data ready
    TCPclient.flush();
    Serial.println("Buff");
    Serial.println(TcpClient_Buff);
    if((TcpClient_Buff.indexOf("&msg=on") > 0)) {
      openComputer();
    }else if((TcpClient_Buff.indexOf("&msg=off") > 0)) {
      closeComputer();
    }
   TcpClient_Buff="";
   TcpClient_BuffIndex = 0;
  }
}




void doWiFiTick(){
  static bool taskStarted = false;
  static uint32_t lastWiFiCheckTick = 0;

  //断开连接重启设备接入网络
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 5000) {
        ESP.restart();
        delay(1000);
      }else{
         lastWiFiCheckTick = millis();
      }
  }
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
      Serial.print("\r\nGet IP Address: ");
      Serial.println(WiFi.localIP());
      startTCPClient();
    }
  }
}

//开机
void openComputer(){
  Serial.println("Turn ON");
  WOL.sendMagicPacket(MACAddress);
}

//关机
void closeComputer(){
  Serial.println("Turn OFF");
  HTTPClient http;
  http.begin(PCurl.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DNSServer.h>//密码直连将其三个库注释
#include <ESP8266WebServer.h>
#include <CustomWiFiManager.h>

#include <time.h>                       
#include <sys/time.h>                  
#include <coredecls.h>      


//#include "SH1106Wire.h"   //1.3寸用这个
#include "SSD1306Wire.h"    //0.96寸用这个
#include "OLEDDisplayUi.h"
#include "HeFeng.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"

/***************************
   Begin Settings
 **************************/

const char* BILIBILIID = "74383847";  //填写你的B站账号

//由于太多人使用我的秘钥，导致获取次数超额，所以不提供秘钥了，大家可以到https://dev.heweather.com/获取免费的
const char* HEFENG_KEY = "d2e72502a1ae4676874d1c0c5ea9263a";//填写你的和风天气秘钥
const char* HEFENG_LOCATION = "101070805";//填写你的城市ID,可到https://where.heweather.com/index.html查询
//const char* HEFENG_LOCATION = "auto_ip";//自动IP定位

#define TZ              8      // 中国时区为8
#define DST_MN          0      // 默认为0

const int UPDATE_INTERVAL_SECS = 5 * 60; // 5分钟更新一次天气
const int UPDATE_CURR_INTERVAL_SECS = 10 * 59; // 2分钟更新一次粉丝数

const int I2C_DISPLAY_ADDRESS = 0x3c;  //I2c地址默认
#if defined(ESP8266)
const int SDA_PIN = 4;  //引脚连接
const int SDC_PIN = 5;  //
#endif

const String WDAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  //月份

// web配网页面自定义我的图标请随便使用一个图片转base64工具转换https://tool.css-js.com/base64.html, 64*64
const char Icon[] PROGMEM = "data:image/png;base64,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";

/***************************
   End Settings
 **************************/
 
//SH1106Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 1.3寸用这个
SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 0.96寸用这个
OLEDDisplayUi   ui( &display );

HeFengCurrentData currentWeather; //实例化对象
HeFengForeData foreWeather[3];
HeFeng HeFengClient;

#define TZ_MN           ((TZ)*60)   //时间换算
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

time_t now; //实例化时间

bool readyForWeatherUpdate = false; // 天气更新标志
bool first = true;  //首次更新标志
long timeSinceLastWUpdate = 0;    //上次更新后的时间
long timeSinceLastCurrUpdate = 0;   //上次天气更新后的时间

String fans = "-1"; //粉丝数

void drawProgress(OLEDDisplay *display, int percentage, String label);   //提前声明函数
void updateData(OLEDDisplay *display);
void updateDatas(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void setReadyForWeatherUpdate();

//添加框架
//此数组保留指向所有帧的函数指针
//框架是从右向左滑动的单个视图
FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast };
//页面数量
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay }; //覆盖回调函数
int numberOfOverlays = 1;  //覆盖数

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // 屏幕初始化
  display.init();
  display.clear();
  display.display();

  display.flipScreenVertically(); //屏幕翻转
  display.setContrast(255); //屏幕亮度

  //Web配网，密码直连请注释
  webconnect();
  
  ui.setTargetFPS(30);  //刷新频率

  ui.setActiveSymbol(activeSymbole); //设置活动符号
  ui.setInactiveSymbol(inactiveSymbole); //设置非活动符号

  // 符号位置
  // 你可以把这个改成TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // 定义第一帧在栏中的位置
  ui.setIndicatorDirection(LEFT_RIGHT);

  // 屏幕切换方向
  // 您可以更改使用的屏幕切换方向 SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_RIGHT);

  ui.setFrames(frames, numberOfFrames); // 设置框架
  ui.setTimePerFrame(5000); //设置切换时间
  
  ui.setOverlays(overlays, numberOfOverlays); //设置覆盖

  // UI负责初始化显示
  ui.init();
  display.flipScreenVertically(); //屏幕反转

  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间，你也可用其他"pool.ntp.org","0.cn.pool.ntp.org","1.cn.pool.ntp.org","ntp1.aliyun.com"
  delay(200);

}

void loop() {
  //远程开关机模块循环检查wifi连接TCP连接
  doWiFiTick();
  doTCPClientTick();

  //桌面时钟日期的首次加载
  if (first) { 
    updateDatas(&display);
    first = false;
  }
  if (millis() - timeSinceLastWUpdate > (1000L * UPDATE_INTERVAL_SECS)) { //屏幕刷新
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }
  if (millis() - timeSinceLastCurrUpdate > (1000L * UPDATE_CURR_INTERVAL_SECS)) { //粉丝数更新
    HeFengClient.fans(&currentWeather, BILIBILIID);
    fans = String(currentWeather.follower);
    timeSinceLastCurrUpdate = millis();
  }

  if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED) { //天气更新
    updateData(&display);
  }

  int remainingTimeBudget = ui.update(); //剩余时间预算

  if (remainingTimeBudget > 0) {
    //你可以在这里工作如果你低于你的时间预算。
    delay(remainingTimeBudget);
  }

}

//Web配网，密码直连将其注释
void webconnect() {  
  display.clear();
  display.drawXbm(0, 0, 128, 64, bilibili); //显示哔哩哔哩
  display.display();

  WiFiManager wifiManager;  //实例化WiFiManager
  wifiManager.setDebugOutput(false); //关闭Debug
  //wifiManager.setConnectTimeout(10); //设置超时
  wifiManager.setHeadImgBase64(FPSTR(Icon)); //设置图标
  wifiManager.setPageTitle("开始接入WiFi吧");  //设置页标题

  if (!wifiManager.autoConnect("Garibel-IOT-Display")) {  //AP模式
    Serial.println("连接失败并超时");
    //重新设置并再试一次，或者让它进入深度睡眠状态
    ESP.restart();
    delay(1000);
  }
  Serial.println("connected...^_^");
  yield();
}

void drawProgress(OLEDDisplay *display, int percentage, String label) {    //绘制进度
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}

void updateData(OLEDDisplay *display) {  //天气更新
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  readyForWeatherUpdate = false;
}

void updateDatas(OLEDDisplay *display) {  //首次天气更新
  drawProgress(display, 0, "Updating fansnumb...");
  HeFengClient.fans(&currentWeather, BILIBILIID);
  fans = String(currentWeather.follower);
  
  drawProgress(display, 33, "Updating weather...");
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  drawProgress(display, 66, "Updating forecasts...");
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  readyForWeatherUpdate = false;
  drawProgress(display, 100, "Done...");
  delay(200);
  
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示时间
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[16];

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  String date = WDAY_NAMES[timeInfo->tm_wday];

  sprintf_P(buff, PSTR("%04d-%02d-%02d  %s"), timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, WDAY_NAMES[timeInfo->tm_wday].c_str());
  display->drawString(64 + x, 5 + y, String(buff));
  display->setFont(ArialMT_Plain_24);

  sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 22 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示天气
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38 + y, currentWeather.cond_txt + "    |   Wind: " + currentWeather.wind_sc + "  ");

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = currentWeather.tmp + "°C" ;
  display->drawString(60 + x, 3 + y, temp);
  display->setFont(ArialMT_Plain_10);
  display->drawString(62 + x, 26 + y, currentWeather.fl + "°C | " + currentWeather.hum + "%");
  display->setFont(Meteocons_Plain_36);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(32 + x, 0 + y, currentWeather.iconMeteoCon);
}

void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //天气预报
  drawForecastDetails(display, x, y, 0);
  drawForecastDetails(display, x + 44, y, 1);
  drawForecastDetails(display, x + 88, y, 2);
}

void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex) {  //天气预报

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y, foreWeather[dayIndex].datestr);
  display->setFont(Meteocons_Plain_21);
  display->drawString(x + 20, y + 12, foreWeather[dayIndex].iconMeteoCon);

  String temp = foreWeather[dayIndex].tmp_min + " | " + foreWeather[dayIndex].tmp_max;
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y + 34, temp);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {   //绘图页眉覆盖
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[14];
  sprintf_P(buff, PSTR("%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(6, 54, String(buff));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = fans;
  display->drawString(122, 54, temp);
  display->drawHorizontalLine(0, 52, 128);
}

void setReadyForWeatherUpdate() {  //为天气更新做好准备
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}
