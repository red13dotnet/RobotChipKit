#include <WiFiCmdRobot.h>
#include <robot.h>
#include <LiquidCrystal_I2C.h> // LCD

extern LiquidCrystal_I2C lcd;

// Specify the IP+Port
extern IPv4 ipServer = {192,168,0,15};
extern unsigned short portServer = 44300;

// Specify the SSID
extern const char * szSsid = "WIFICOTEAU";

// WPA2 key
//You can find a tool to pre-calculate your PSK key at http://www.wireshark.org/tools/wpa-psk.html1.
//kitesurf9397+ WIFICOTEAU => 5d04ff38a46a0266990b98eb651959000bc52fe84ab902a25e95f2338a2d71f4
DWIFIcK::WPA2KEY key = {0x5d,0x04,0xff,0x38,0xa4,0x6a,0x02,0x66,0x99,0x0b,0x98,0xeb,0x65,0x19,0x59,0x00,0x0b,0xc5,0x2f,0xe8,0x4a,0xb9,0x02,0xa2,0x5e,0x95,0xf2,0x33,0x8a,0x2d,0x71,0xf4};

// SD variables
extern SdFile root;   // SD Root
SdFile FilePicture;   // SD File

TcpServer tcpServer;
TcpClient tcpClient;

byte rgbRead[1024];
int cbRead = 0;
int count = 0;
    
String szcmd;
uint16_t cmd[CMD_SIZE];
uint16_t cmd_GO[CMD_SIZE];
uint16_t t_GO = 10;     // 10s max for GO command
int16_t resp[RESP_SIZE];
int resp_len = 0;
   
DNETcK::STATUS status;

int WiFiCmdRobot::WiFiCmdRobot_begin() {
  
    int conID = DWIFIcK::INVALID_CONNECTION_ID;
    int cNetworks = 0;
    int iNetwork = 0; 
    int ret=SUCCESS;
  
    // initialize the SD-Card    
    ret = initSDCard();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Init SD-Card, error: ");
        Serial.println(ret);
    }                                                                    
    else
    {
        Serial.println("Init SD-Card OK");
        Serial.println("");
    }
    
    // get infos from SD-Card  
    ret=infoSDCard();
    if (ret != SUCCESS)
    {  
        Serial.print("Error Infos SD-Card, error: ");
        Serial.println(ret);
    }
    
    Serial.println("Start WIFI Init"); 
    lcd.clear();
    lcd.print("Start WIFI Init");
           
    // set my default wait time to nothing
    DNETcK::setDefaultBlockTime(DNETcK::msImmediate); 

    // start a scan
    DWIFIcK::beginScan();
    while (1)
    {
            // every pass through loop(), keep the stack alive
            DNETcK::periodicTasks(); 
            if(DWIFIcK::isScanDone(&cNetworks, &status))
            {
                Serial.println("Scan Done");
                break;
            }
            else if(DNETcK::isStatusAnError(status))
            {
                Serial.print("Scan Failed");
                return -1;
            }
            
    }        
    
    while (1)
    {            
            // every pass through loop(), keep the stack alive
            DNETcK::periodicTasks();            
            if(iNetwork < cNetworks)
            {
                DWIFIcK::SCANINFO scanInfo;
                int j = 0;

                if(DWIFIcK::getScanInfo(iNetwork, &scanInfo))
                {
                    Serial.print("Scan info for index: ");
                    Serial.println(iNetwork, DEC);

                    Serial.print("SSID: ");
                    Serial.println(scanInfo.szSsid);

                    Serial.print("Secuity type: ");
                    Serial.println(scanInfo.securityType, DEC);
                    switch(scanInfo.securityType)
                    {
                           case DWIFIcK::WF_SECURITY_OPEN:
                                Serial.println("SECURITY OPEN");
                                break;
                           case DWIFIcK::WF_SECURITY_WEP_40:
                                Serial.println("WF SECURITY WEP 40");
                                break;
                           case DWIFIcK::WF_SECURITY_WEP_104:
                                Serial.println("SECURITY WEP 104");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_WITH_KEY:
                                Serial.println("SECURITY WPA WITH KEY");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA WITH PASS PHRASE");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA2_WITH_KEY:
                                Serial.println("SECURITY WPA2 WITH KEY");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA2_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA2 WITH PASS PHRASE");
                                break;
                           case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_KEY:
                                Serial.println("SECURITY WPA AUTO WITH KEY");
                                break; 
                           case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
                                Serial.println("SECURITY WPA AUTO WITH PASS PHRASE");
                                break;                                
                    }
                    
                    Serial.print("Channel: ");
                    Serial.println(scanInfo.channel, DEC);    

                    Serial.print("Signal Strength: ");
                    Serial.println(scanInfo.signalStrength, DEC);    

                    Serial.print("Count of supported bit rates: ");
                    Serial.println(scanInfo.cBasicRates, DEC);    

                    for( j= 0; j< scanInfo.cBasicRates; j++)
                    {
                        Serial.print("\tSupported Rate: ");
                        Serial.print(scanInfo.basicRates[j], DEC); 
                        Serial.println(" bps");
                    }

                    Serial.print("SSID MAC: ");
                    for(j=0; j<sizeof(scanInfo.ssidMAC); j++)
                    {
                        if(scanInfo.ssidMAC[j] < 16)
                        {
                            Serial.print(0, HEX);
                        }
                        Serial.print(scanInfo.ssidMAC[j], HEX);
                    }

                    Serial.print("Beacon Period: ");
                    Serial.println(scanInfo.beconPeriod, DEC);    

                    Serial.print("dtimPeriod: ");
                    Serial.println(scanInfo.dtimPeriod, DEC);    

                    Serial.print("atimWindow: ");
                    Serial.println(scanInfo.atimWindow, DEC); 
                }
                else
                {
                    Serial.print("Unable to get scan info for iNetwork: ");
                    Serial.println(iNetwork, DEC);
                }

                iNetwork++;
            }
            else
            {
                break;
            }           
    }

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID)
    {
          Serial.print("Connection Created, ConID = ");
          Serial.println(conID, DEC);
    }
    else
    {
          Serial.print("Unable to connection, status: ");
          Serial.println(status, DEC);
          return -2;
    }
 
    while (1)
    { 
           // every pass through loop(), keep the stack alive
           DNETcK::periodicTasks(); 
            
          // initialize the stack with a static IP
          DNETcK::begin(ipServer);
          if(DNETcK::isInitialized(&status))
          {
                Serial.println("IP Stack Initialized");
                break;
          }
          else if(DNETcK::isStatusAnError(status))
          {
                Serial.print("Error in initializing, status: ");
                Serial.println(status, DEC);
                return -3;
          }
    }
        
    Serial.println("");
    IPv4 ip;

    DNETcK::getMyIP(&ip);
    Serial.print("My ");
    printIP(ip);
    Serial.println("");

    DNETcK::getGateway(&ip);
    Serial.print("Gateway ");
    printIP(ip);
    Serial.println("");

    DNETcK::getSubnetMask(&ip);
    Serial.print("Subnet mask: ");
    printNumb(ip.rgbIP, 4, '.');
    Serial.println("");

    DNETcK::getDns1(&ip);
    Serial.print("Dns1 ");
    printIP(ip);
    Serial.println("");

    DNETcK::getDns2(&ip);
    Serial.print("Dns2 ");
    printIP(ip);
    Serial.println("");
       
    
    DWIFIcK::CONFIGINFO configInfo;
    if(DWIFIcK::getConfigInfo(&configInfo))
    {
        Serial.println("WiFi config information");

        Serial.print("Scan Type: ");
        switch(configInfo.scanType)
        {
                   case DWIFIcK::WF_ACTIVE_SCAN:
                        Serial.println("ACTIVE SCAN");
                        break;
                   case DWIFIcK::WF_PASSIVE_SCAN:
                        Serial.println("PASSIVE SCAN");
                        break;                
        }

        Serial.print("Beacon Timeout: ");
        Serial.println(configInfo.beaconTimeout, DEC);

        Serial.print("Connect Retry Count: ");
        Serial.println(configInfo.connectRetryCount, DEC);

        Serial.print("Scan Count: ");
        Serial.println(configInfo.scanCount, DEC);

        Serial.print("Minimum Signal Strength: ");
        Serial.println(configInfo.minSignalStrength, DEC);

        Serial.print("Minimum Channel Time: ");
        Serial.println(configInfo.minChannelTime, DEC);

        Serial.print("Maximum Channel Time: ");
        Serial.println(configInfo.maxChannelTime, DEC);

        Serial.print("Probe Delay: ");
        Serial.println(configInfo.probeDelay, DEC);

        Serial.print("Polling Interval: ");
        Serial.println(configInfo.pollingInterval, DEC);                                                 
    }
    else
    {
        Serial.println("Unable to get WiFi config data");
        return -4; 
    }                  
   
    tcpServer.startListening(portServer);

    Serial.println("End WIFI Init");
    Serial.println("");
    lcd.setCursor(0,1); 
    lcd.print("End   WIFI Init");
    
    return SUCCESS;                        
}

int WiFiCmdRobot::WiFiCmdRobot_main() {
    String stringRead;
    int conx = 0;
    unsigned long timeout = 5; // 5s
    unsigned long start = 0;
    int ret=SUCCESS;

    // not specifically needed, we could go right to AVAILABLECLIENT
    // but this is a nice way to print to the serial monitor that we are 
    // actively listening.
    // Remember, this can have non-fatal falures, so check the status
    while (1) { 
        if(tcpServer.isListening(&status))
        {
            Serial.print("Listening on port: ");
            Serial.println(portServer, DEC);
            digitalWrite(Led_Yellow, HIGH);               // turn on led yellow
            break;
        }  
        else if(DNETcK::isStatusAnError(status))
        {
            Serial.println("Error Listening");
            tcpClient.close();
            tcpServer.close();
            return -1;
        }
    }

    // wait for a connection until timeout
    conx = 0;
    start = millis();
    while ((millis() - start < timeout*1000) && (conx == 0)) { 
        if((count = tcpServer.availableClients()) > 0)
        {
            Serial.print("Got ");
            Serial.print(count, DEC);
            Serial.println(" clients pending");
            conx = 1;
            // probably unneeded, but just to make sure we have
            // tcpClient in the  "just constructed" state
            tcpClient.close();             
        }
    }

    // accept the client 
    if((conx == 1) && (tcpServer.acceptClient(&tcpClient)))
    {
       Serial.println("Got a Connection");
       lcd.clear();
       lcd.print("Got a Connection");
       
       stringRead = "";
       while (tcpClient.isConnected())
       {
                if (tcpClient.available())
                {
                    char c = tcpClient.readByte();
                    if (c == '\n')
                    {
                          Serial.println (stringRead);
                          if (stringRead.startsWith("GET"))  
                          {
                                Serial.println("GET");
                                ret = Cmd (stringRead);
                                
                                if (ret == SUCCESS)
                                {
                                    ret = ReplyOK ();
                                }
                                else    
                                {
                                    cmd_GO[0] = 0; //reset GO command
                                    ret = ReplyKO ();
                                }
                                break;                             
 
                          }
                          else if (stringRead.startsWith("\r"))
                          {
                                // empty line => end
                                Serial.println("empty line => end");
                                break;
                          }                          
                          else
                          {
                                // no GET
                                Serial.println("no GET => ignore");
                          }
                          stringRead = "";              
                    }
                    else
                    {
                          stringRead += c;
                    }
                }
       } // end while            
    }
    else if((cmd_GO[0] == CMD_GO) && (cmd_GO[1] > t_GO+(uint16_t)timeout))  // GO ongoing
    {
       Serial.println("Continue command GO");
       cmd_GO[1] = cmd_GO[1] - t_GO - (uint16_t)timeout;
       
       cmd[0] = cmd_GO[0];
       cmd[1] = t_GO;
       cmd[2] = cmd_GO[2];   
       ret = CmdRobot (cmd, resp, &resp_len);
       
       Serial.print("Call CmdRobot, ret: ");
       Serial.print(ret);
       Serial.print(" / resp_len: ");
       Serial.println(resp_len);        
    }
    else if(cmd_GO[0] == CMD_GO) // end GO
    {
       Serial.println("End command GO");
       cmd_GO[0] = 0; //reset GO command
      
       cmd[0] = CMD_STOP;  // Stop after GO
       cmd[1] = 0;
       cmd[2] = 0;   
       ret = CmdRobot (cmd, resp, &resp_len);
       
       Serial.print("Call CmdRobot, ret: ");
       Serial.print(ret);
       Serial.print(" / resp_len: ");
       Serial.println(resp_len);        
    }   
      
    // Close
    tcpClient.close();
    digitalWrite(Led_Yellow, LOW);               // turn off led yellow
    Serial.println("Closing TcpClient");
    
    return SUCCESS;    
}


/***    void printIP(IPv4& ip)
 *
 *    Parameters:
 *          ip, the IP to print
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print the IP address out
 *      on the serial monitor.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printIP(IPv4& ip)
{
    Serial.print("IP: ");
    WiFiCmdRobot::printNumb(ip.rgbIP, 4, '.');
}

/***    void printMAC(MAC& mac)
 *
 *    Parameters:
 *          mac, the MAC to print
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print the MAC address out
 *      on the serial monitor.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printMAC(MAC& mac)
{
    Serial.print("MAC: ");
    WiFiCmdRobot::printNumb(mac.rgbMAC, 6, ':');
}

/***    void printNumb(byte * rgb, int cb, char chDelim)
 *
 *    Parameters:
 *          rgb - a pointer to a MAC or IP, or any byte string to print
 *          cb  - the number of bytes in rgb
 *          chDelim - the delimiter to use between bytes printed
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      A simple routine to print to the serial monitor bytes in either HEX or DEC
 *      If the delimiter is a ':' the output will be in HEX
 *      All other delimiters will be in DEC.
 *      Only HEX numbers get leading 0
 *      DEC number have no leading zeros on the number.
 * ------------------------------------------------------------ */
void WiFiCmdRobot::printNumb(byte * rgb, int cb, char chDelim)
{
    for(int i=0; i<cb; i++)
    {
        if(chDelim == ':' && rgb[i] < 16)
        {
            Serial.print(0, DEC);
        }  
        if(chDelim == ':')
        {
            Serial.print(rgb[i], HEX);
        }
        else
        {
            Serial.print(rgb[i], DEC);
        }  
        if(i < cb-1)
        {
            Serial.print(chDelim);  
        }
    }
}


int WiFiCmdRobot::Cmd (String s)
{
   int cmdTag;
   int And;
   String szparam[10];
   int iparam[10];
   int paramTag;
   int Start;
   int Semicolumn;
   int ret=SUCCESS;
   
   //GET /Robot?CMD=MOVE_TILT_PAN&PARAM=122;118; HTTP/1.1[\r][\n]
   //GET /Robot?CMD=INFOS HTTP/1.1[\r][\n]

   cmdTag = s.indexOf("CMD=");
   if (cmdTag > 0)
   {   
       Serial.print("CMD=");
       And = s.indexOf('&', cmdTag + 1);
       if (And == -1) And = s.indexOf(' ', cmdTag + 1);  
       szcmd = s.substring(cmdTag + 4, And);
       Serial.println(szcmd);

       paramTag = s.indexOf("PARAM=");
       if (paramTag > 0)
       {   
          Start = paramTag + 6;
          Semicolumn = s.indexOf('%3B', Start + 1); // semi column is encoded in URL
          for (int p=0; Semicolumn != -1; p++)
          {
               szparam[p] = s.substring(Start, Semicolumn - 2);
               iparam[p] = szparam[p].toInt();
               Serial.print("param");
               Serial.print(p);
               Serial.print("=");
               Serial.print(szparam[p]);
               Serial.print("/");
               Serial.println(iparam[p]);
               Start = Semicolumn + 1;
               Semicolumn = s.indexOf('%3B', Start + 1);
               
          }                                   
       }
       // CMD
       
       if (szcmd == "START")
       {
               cmd[0] = CMD_START;
               cmd[1] = iparam[0];
               cmd[2] = 0;
               cmd_GO[0]= 0; //reset GO command
       }        
       if (szcmd == "STOP")
       {
               cmd[0] = CMD_STOP;
               cmd[1] = 0;
               cmd[2] = 0;
               cmd_GO[0]= 0; //reset GO command
       }                                 
       if (szcmd == "INFOS")
       {
               cmd[0] = CMD_INFOS;
               cmd[1] = 0;
               cmd[2] = 0;
       }                                    
       if (szcmd == "PICTURE")
       {
               cmd[0] = CMD_PICTURE;
               cmd[1] = 0;
               cmd[2] = 0;
       }                                    
       if (szcmd == "TURN_RIGHT")
       {
               cmd[0] = CMD_TURN_RIGHT;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                    
       if (szcmd == "TURN_LEFT")
       {
               cmd[0] = CMD_TURN_LEFT;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                           
       if (szcmd == "CHECK_AROUND")
       {
               cmd[0] = CMD_CHECK_AROUND;
               cmd[1] = iparam[0];
               cmd[2] = 0;
       }                                    
       if (szcmd == "MOVE_TILT_PAN")
       {
               cmd[0] = CMD_MOVE_TILT_PAN;
               cmd[1] = iparam[0];
               cmd[2] = iparam[1];
       }                                    
       if (szcmd == "GO")
       {
               cmd_GO[0] = CMD_GO;
               cmd_GO[1] = iparam[0];
               cmd_GO[2] = iparam[1];
               cmd[0] = cmd_GO[0];
               cmd[1] = t_GO;
               cmd[2] = cmd_GO[2];
       } 
       
       ret = CmdRobot (cmd, resp, &resp_len);
       
       Serial.print("Call CmdRobot, ret: ");
       Serial.print(ret);
       Serial.print(" / resp_len: ");
       Serial.println(resp_len); 
       
       return ret;                                                                                           
   }
   else
   {
       // no CMD
       tcpClient.println("HTTP/1.1 400 Bad Request");
       tcpClient.println("Content-Type: text/html");
       tcpClient.println("Server: ChipkitEDH/0.1"); 
       tcpClient.println(); 
       
       return -400;                              
   }
   
}


int WiFiCmdRobot::ReplyOK ()
{
    int ret=SUCCESS; 
    
    if (cmd[0] == CMD_PICTURE)
    { 
          Serial.println("cmd[0] == CMD_PICTURE");
          if (tcpClient.isConnected()) Serial.println("tcpClient.isConnected");
          else Serial.println("tcpClient.is not Connected");
          tcpClient.println("HTTP/1.1 200 OK");
          tcpClient.println("Content-Type: application/octet-stream");
          tcpClient.println("Server: ChipkitEDH/0.1");                                             
          tcpClient.println();

          ret= WiFiSendPicture (resp[0]);
          
          if (ret != SUCCESS){  Serial.print("WiFiSendPicture error"); Serial.print(ret);}
    }
    else                                           
    {                               
          tcpClient.println("HTTP/1.1 200 OK");
          for(int i=0; i<resp_len; i++)
          {
              tcpClient.print("Field ");
              tcpClient.print(String(i));
              tcpClient.print(":");
              tcpClient.print(String((int)resp[i]));
              tcpClient.println(";");
         }
         tcpClient.println("Content-Type: text/html");
         tcpClient.println("Server: ChipkitEDH/0.1");                                             
         tcpClient.println();
    }
    
    return ret;    
}

int WiFiCmdRobot::ReplyKO ()
{
     tcpClient.println("HTTP/1.1 500 Internal Server Error");
     tcpClient.println("Content-Type: text/html");
     tcpClient.println("Server: ChipkitEDH/0.1");                                   
     tcpClient.println();
     
     return SUCCESS;
                                  
}



int WiFiCmdRobot::WiFiSendPicture (int16_t n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  uint8_t buf[PAYLOAD_SIZE];
  char filename[12+1];
 
  Serial.print("n: ");
  Serial.println(n);
  
  // Open the file
  sprintf(filename, "PICT%02d.jpg", n);
  if (!FilePicture.open(&root, filename, O_READ)) return FILE_OPEN_ERROR;  

  Serial.print("Open file: ");
  Serial.println(filename);

  // read from the file until there's nothing else in it:
  while ((nbytes = FilePicture.read(buf, sizeof(buf))) > 0 && ret == SUCCESS) {
       for (unsigned int idx = 0; idx<nbytes;idx++)
       {
           tcpClient.print(buf[idx]);
       }
 
  }// while
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}
