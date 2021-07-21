# 哔哩哔哩网络天气时钟小电视（太极创客小凯制作）

### 观看该项目可点击视频地址：https://www.bilibili.com/video/BV1FV411k7Pq/

![输入图片说明](https://images.gitee.com/uploads/images/2020/0617/232144_8529b92d_5087908.jpeg "IMG20200613233241.jpg")

![输入图片说明](https://images.gitee.com/uploads/images/2020/0617/232201_8bbc5691_5087908.png "屏幕截图.png")

## 使用注意：
请在 [Esp8266_Clock_Weather /
Esp8266_Clock_Weather.ino](https://gitee.com/taijichuangke/bilibili_weather_clock/blob/master/Esp8266_Clock_Weather/Esp8266_Clock_Weather.ino)主控程序中填入您自己的和风天气密钥信息以及BiliBili信息。否则天气时钟无法正常显示您所需的天气信息和BiliBili信息。

## 材料
| 材料         | 规格                                 | 参考价格 |
| ------------ | ------------------------------------| -------- |
| ESP01s       | 或者其他型号，尺寸合适即可            | 6-7元    |
| OLED         | 0.96寸，带3.3V稳压，尺寸见下方图片    | 9-11元   |
| 3.7V充电模块 | 见下方图片                           | 1-2元    |
| 3.7V锂电池   | 型号602525或者502525，尺寸见下方图片  | 5-7元    |
| 三脚开关     | 宽3.7长8.5                           | 0.1元    |
| 外壳         | 见3D文件                             |          |
| 线           | 若干                                 |          |
| 亚克力       | 见图纸CAD                             | 5元      |
| 螺钉         | M2*10                                | 1元      |
| USB转TTL     | 有3.3V的                             | 3-5元    |

![输入图片说明](https://images.gitee.com/uploads/images/2020/0617/232548_9bc5168f_5087908.png "屏幕截图.png")

## 焊接/连线  
上传程序接线

| ESP01S | USB-TTF |
| ------ | ------- |
| GND    | GND     |
| 3.3V   | 3.3V    |
| RX     | TX      |
| TX     | RX      |
| EN     | 3.3V    |
| IO0    | GND     |



![输入图片说明](https://images.gitee.com/uploads/images/2020/0612/131411_697c1bdf_5087908.png "上传程序接线.png")

主电路接线

（注意ESP01模块的VCC是接在OLED屏幕的3.3V降压管的引脚上哦，还有就是你的模块引脚跟图片可能不一样，注意区分，灵活变通）

| ESP01S | OLED       |
| ------ | ---------- |
| 3.3V   | 3.3V降压管 |
| GND    | GND        |
| IO0    | SDA        |
| IO2    | SCL        |

![输入图片说明](https://images.gitee.com/uploads/images/2020/0612/131420_d34f4364_5087908.png "电路接线.png")

## 3D打印外壳 
见3D文件夹，可自行修改

目前只针对0.96寸

1.3寸后期更新

## 程序  
需要修改的地方修改的地方我已在程序注释，请小伙伴认真阅读 


## 其他
网络时钟目前获取的是中科院以及阿里云的NTP

天气用的是和风天气API，可百度搜索并自行注册

## 太极创客团队信息

太极创客官网地址：http://www.taichi-maker.com/

太极创客哔哩哔哩主页：https://space.bilibili.com/103589285

太极创客YouTube：https://www.youtube.com/channel/UC8EkxMr5gGnrb9adVgR-UJw

太极创客GitHub：https://github.com/taichi-maker

太极创客码云：https://gitee.com/taijichuangke

## ESP8266-Seniverse库项目开发人员

小凯：https://gitee.com/xiaoxiaokai