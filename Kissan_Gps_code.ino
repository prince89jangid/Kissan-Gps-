/* this code is created by prince jangid */
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);   // for gps
SoftwareSerial gsm(10, 9); // for gsm
int wire = 1;
int flag = 0;
String textmsg = ""; // iska kaam hoga user se msg recive kareke store kar lena
String lati = "";
String longi = "";
String sms = "";
String sms_objective = "";
String s; 


     /***********ENTER YOUR MOBILE NUMBER HERE :- **************/
    /*********************************************************/
   /*****/  String mobile_number = "+91XXXXXXXXXX"; /*******/
  /*******************************************************/
 /******************************************************/
void setup()
{
    Serial.begin(9600);
    ss.begin(9600);
    gsm.begin(9600);

    pinMode(2, INPUT_PULLUP); // so that digital pin d2 initially high

    Serial.println("Trilok Gps");

    gsm.println("AT"); // for attantion
    delay(500);

    gsm.println("AT+CMGF=1"); // enable sms mode
    delay(500);

    gsm.println("AT+CNMI=2,2,0,0,0"); // msg ko decode karna
    delay(500);

   
     // default is 0
    while(wire != 0)
    {
        Serial.println("please connect wire...");
        delay(1000);
        wire = digitalRead(2);

    }

    Serial.println("wire is connected");
    delay(5000);

    
    gsm.println("AT+CMGS=\"" + mobile_number + "\"");
    delay(500);

    gsm.println("device is on....");
    delay(500);

    gsm.write(26);
    delay(500);


// unsigned long startTime = millis();
// while (!gps.location.isValid()) {
//     while (ss.available() > 0) {
//         gps.encode(ss.read());
//     }

//     // Optional timeout condition (10 sec)
//     if (millis() - startTime > 10000) {
//         Serial.println("GPS fix timeout!");
//         break;
//     }

//     Serial.println("Waiting for GPS fix...");
//     delay(500); // thoda delay avoid too much serial spam
// }

//     Serial.println("gps location is fixed :");
    

}

void loop()
{ 
    
   wire = digitalRead(2); // default is 0
   
    if (wire == 1)
    {
        sms_objective = "Alert! Alert! Alert!\n Aapka Pashu khatre mai hai. Last location --->(Trilok GPS)\n";
    }
    if (wire == 0)
    {
        sms_objective = "Apke Pashu ki location ke liye link par click karen ---->(Trilok GPS)\n";
    }

    if (wire == 1)
    {

        if (flag == 0)
        {
            
            smslink(); // call that function which you made
            delay(5000);
            calluser();
            delay(1000);

            flag++; // 1
        }
    }

    if (wire == 0) // done
    {
        if (flag == 1)
        {
            flag = 0;
            delay(3000); // so that kisan ke paas time rahega 5 second ka ki usse close kar de
        }
    }

   
    gsm.listen();
    while (gsm.available() > 0)
    {


             textmsg = gsm.readString(); // lets ki mene beja GETLOC
     
             /* so that farmer can easily track cow location through sms */
        if (textmsg.indexOf("Gps") >= 0 || textmsg.indexOf("gps") >= 0 || textmsg.indexOf("GPS") >= 0 /* || textmsg.indexOf("+917877745269") >= 0 */ )
             {
                 smslink();
             }
     
             /* So that after the cow returns from grazing, the farmer can turn off the GPS device*/
     
        if( (textmsg.indexOf("OFF") >= 0 || textmsg.indexOf("off") >= 0 || textmsg.indexOf("Off") >= 0 ) && textmsg.indexOf(mobile_number) >= 0 )
             {
               flag = 2;   
               Serial.println("flag = 2");
               other_sms(" Security loop disable\nNow you can remove wire. if you disconnect by mistake then send on again");                          
                         
             }
             /* so that wapis sab normal ho jaaye */
     
        if( (textmsg.indexOf("On") >= 0 || textmsg.indexOf("ON") >= 0 || textmsg.indexOf("on") >= 0 ) && textmsg.indexOf(mobile_number) >= 0 )
             {
               flag = 0;  
               Serial.println("flag = 0");        
               other_sms(" Security loop enable ");         
                         
             }

             /* if user need to make an action through. but here is an issue such that anyone can access your machine because 
             there are no any unique condition. during testing period i can do that...*/

        if(textmsg.indexOf("RING") >= 0 )
             {
                delay(1000);
                                    
                gsm.println("ATA");
                Serial.println("call attending: ");

                delay(1000);
             }



    }
}


/* here user defined functions other then setup() and loop() */

void smslink()
{

    ss.listen();
    unsigned long start = millis();
    bool locationCaptured = false;

    while (millis() - start < 5000) // 5 seconds tak try karo
    {
        while (ss.available() > 0)
        {
            char c = ss.read();

            if (gps.encode(c))
            {
                if (gps.location.isValid())
                {

                    lati = String(gps.location.lat(), 6);
                    longi = String(gps.location.lng(), 6);
                    sms = "https://maps.google.com/?q=" + lati + "," + longi;
                    Serial.println(sms);

                    delay(200);

                    gsm.println("AT+CMGF=1"); // Text mode
                    delay(200);

                    // gsm.print("AT+CMGS=\"+917877745269\"\r"); // Mobile number
                    gsm.println("AT+CMGS=\""+mobile_number+"\"\r");
                    delay(200);

                    gsm.println(sms_objective + sms);
                    delay(200);

                    gsm.write(26);
                    delay(200);

                    Serial.println("SMS SENT ");

                    sms = "";
                    locationCaptured = true;
                    break;
                }
                else
                {
                    Serial.println("GPS not fixed yet...");
                }
            }
        }
        if (locationCaptured)
            break;
    }
}

void calluser()
{
    // gsm.listen();
    Serial.println("Calling...");

    // Mobile number ke aage country code lagana zaroori hai (India ke liye +91)
    // gsm.println("ATD+917877745269;"); // <-- Apna number yahan daalo
    
    gsm.println("ATD"+mobile_number+";");
    delay(20000); // 20 seconds tak call chalne do

    // gsm.println("ATH"); // Call cut karne ka command
    // delay(1000);

    Serial.println("Call Ended");
}

void other_sms(String s)
{
    gsm.println("AT");
    delay(100);

    gsm.println("AT+CMGS=\""+mobile_number+"\"");
    delay(100);

    gsm.println(s);
    delay(100);

    gsm.write(26);
    delay(100);

}



/* ©️ ALL RIGHTS ARE RESERVED */




