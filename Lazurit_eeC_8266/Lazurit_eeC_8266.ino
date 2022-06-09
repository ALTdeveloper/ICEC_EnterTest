#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "Uzryes"
#define STAPSK  "12qw34er"//密码
#endif

//引脚定义初始化
const int dataPin0 = 4; //8266的正常io引脚只能用少得可怜来形容
const int dataPin1 = 5;
const int dataPin2 = 1;
const int dataPin3 = 12;
const int timePin = 13;

const int x10tox2[10][4] = {
  {0, 0, 0, 0}, //0
  {0, 0, 0, 1}, //1
  {0, 0, 1, 0}, //2
  {0, 0, 1, 1}, //3
  {0, 1, 0, 0}, //4
  {0, 1, 0, 1}, //5
  {0, 1, 1, 0}, //6
  {0, 1, 1, 1}, //7
  {1, 0, 0, 0}, //8
  {1, 0, 0, 1} //9
};

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\r\n";       // a string to send back
char time_update[10] = ""; //存时间给51,字符串格式
int time_sendnum[] = {0, 0, 0, 0, 0, 0}; //存时间的每一位给51更新，这是上面的每一位数字

WiFiUDP Udp;

void send_51time(int num[]);//得先声明在调用
void time2anum(char time_num[], int target_num[]);

void setup()
{
  //io口初始化
  pinMode(dataPin0, OUTPUT);
  pinMode(dataPin1, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(dataPin3, OUTPUT);

  //串口调试和udp初始化
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
}

void loop()
{
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort(), Udp.destinationIP().toString().c_str(), Udp.localPort(), ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();

    //在这里开始往51发送数据
    time2anum(time_update, time_sendnum);
    send_51time(time_sendnum);
  }

}

void send_51time(int num[])
{
  int i;
  //此时时钟线应该是低的
  for (i = 0; i < 6; i++)
  {
    digitalWrite(dataPin0, x10tox2[num[i]][0]);
    digitalWrite(dataPin1, x10tox2[num[i]][1]);
    digitalWrite(dataPin2, x10tox2[num[i]][2]);
    digitalWrite(dataPin3, x10tox2[num[i]][3]);
    pinMode(timePin, OUTPUT);//arduino的尿性，得先设置输出才行
    digitalWrite(timePin, HIGH);//拉高时钟线
    pinMode(timePin, INPUT);
    while (digitalRead(timePin) == HIGH); //等待时钟线拉低
    delay(10);//延迟一点点，我担心快了出问题，没问题就去掉提高传输速度
  }



}

void time2anum(char time_num[], int target_num[])
{
  int i, ii = 0;
  char is_0 = '1';

  for (i = 0; is_0 == '0'; i++)
  {
    is_0 = time_num[i + 1];
    if (time_num[i] != 58)
    {
      target_num[ii] = time_num[i] - 48;
      ii++;
    }
  }


  for (i = 0; i < 3; i++)
  {
    target_num[2 * i] = time_num[i] / 10; //把十位提取出来
    target_num[2 * i + 1] = time_num[i] % 10; //把个位提取出来
  }
}
