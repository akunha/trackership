
#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>

#define PIN_TX    10
#define PIN_RX    11
SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,

//make sure that the baud rate of SIM900 is 9600!
//you can use the AT Command(AT+IPR=9600) to set it through SerialDebug

//DFRobot_SIM808 sim808(&Serial);

//char http_cmd[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\n\r\n";
char http_men[60];
//char http_cmd[] = "GET /api/storePosition/2/-19.888/-59.8888 HTTP/1.0\r\n\r\n";
char lat[12];
char lon[12];
char wspeed[12];
char datetime[50];
char identificador[2] = "1";

char buffer[512];

void setup(){
  mySerial.begin(9600);
  Serial.begin(9600);
  
  //******** Initialize sim808 module *************
  while(!sim808.init()) {
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }
  delay(3000);
           

  
  if( sim808.attachGPS())
      Serial.println("Open the GPS power success");
  else 
      Serial.println("Open the GPS power failure");
  delay(3000);

}

void loop(){
       //************** Get GPS data *******************
   if (sim808.getGPS()) {
    Serial.print(sim808.GPSdata.year);
    Serial.print("/");
    Serial.print(sim808.GPSdata.month);
    Serial.print("/");
    Serial.print(sim808.GPSdata.day);
    Serial.print(" ");
    Serial.print(sim808.GPSdata.hour);
    Serial.print(":");
    Serial.print(sim808.GPSdata.minute);
    Serial.print(":");
    Serial.print(sim808.GPSdata.second);

    Serial.print("latitude :");
    Serial.println(sim808.GPSdata.lat,6);
    

    Serial.print("longitude :");
    Serial.println(sim808.GPSdata.lon,6);

    
    Serial.print("speed_kph :");
    Serial.println(sim808.GPSdata.speed_kph);
    Serial.print("heading :");
    Serial.println(sim808.GPSdata.heading);
   
    float la = sim808.GPSdata.lat;
    float lo = sim808.GPSdata.lon;
    float ws = sim808.GPSdata.speed_kph;

    dtostrf(la, 6, 6, lat); //put float value of la into char array of lat. 6 = number of digits before decimal sign. 2 = number of digits after the decimal sign.
    dtostrf(lo, 6, 6, lon); //put float value of lo into char array of lon    
    dtostrf(ws, 2, 0, wspeed);  //put float value of ws into char array of wspeed
    sprintf(http_men, "GET /api/storePosition/%s/%s/%s HTTP/1.0\r\n\r\n",identificador, lat, lon);
      //int inputLen = sizeof(http_men);
      Serial.println(http_men);

  //*********** Attempt DHCP *******************
  while(!sim808.join(F("cmnet"))) {
      Serial.println("Sim808 join network error");
      delay(2000);
  }

  //************ Successful DHCP ****************
  Serial.print("IP Address is ");
  Serial.println(sim808.getIPAddress());



      //*********** Establish a TCP connection ************
//  if(!sim808.connect(TCP,"mbed.org", 80)) {
  if(!sim808.connect(TCP,"104.154.54.192", 80)) {
      Serial.println("Connect error");
  }else{
//      Serial.println("Connect mbed.org success");
      Serial.println("Connect 104.154.54.192 success");
  }
  delay(3000);
      //*********** Send a GET request *****************
  
  Serial.println("waiting to fetch...");
  sim808.send(http_men, sizeof(http_men)-1);
  while (true) {
      int ret = sim808.recv(buffer, sizeof(buffer)-1);
      if (ret <= 0){
          Serial.println("fetch over...");
          break; 
      }
      buffer[ret] = '\0';
      Serial.print("Recv: ");
      Serial.print(ret);
      Serial.print(" bytes: ");
      Serial.println(buffer);
      break;
  }

  //************* Close TCP or UDP connections **********
  sim808.close();

  //*** Disconnect wireless connection, Close Moving Scene *******
  sim808.disconnect();
  
    //delay(30000);
    //************* Turn off the GPS power ************
    sim808.detachGPS();
   }

}
