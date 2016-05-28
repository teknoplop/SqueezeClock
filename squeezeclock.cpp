#include "application.h"
//SYSTEM_THREAD(ENABLED);



#include <memory>
#include <algorithm>
#include <string>
#include <set>

#include "ServiceDirectory.h"
#include "Config.h"

#include "AlarmSettingsMonitor.h"
#include "ButtonMonitor.h"
#include "PlayerQuery.h"
#include "ClockDisplay.h"
#include "TimeMonitor.h"
#include "AlarmBackup.h"

#include "SparkFunMAX17043.h"

std::shared_ptr< ServiceDirectory >   _svc;
std::shared_ptr< ConfigDetails >      _conf;
std::set< std::shared_ptr< Looper > > _looperSet;

UDP SyslogUdp;


void sendSyslogMessage( const char *sz )
{
  Serial.println( sz );

  IPAddress ipSvr( 192, 168, 1, 11 );
  SyslogUdp.beginPacket(ipSvr, 514);
  SyslogUdp.write("<14>SqueezeClock ");
  SyslogUdp.write( sz );
  SyslogUdp.endPacket();
}

void
setup() 
{
  // TODO: debug print class
  Serial.begin(9600);   
  //while(!Serial.available()) Particle.process();

  RGB.control( true );
  RGB.brightness( 10 );
  RGB.control( false );


  Serial.println( "SqueezeClock");  
  Serial.println( "~~~~~~~~~~~~");  
  Serial.println();
  Serial.printlnf("System version: %s", System.version().c_str());

  Serial.printlnf( "SSID:          %s", WiFi.SSID());  
  Serial.printlnf( "Strength:      %d", WiFi.RSSI());
  Serial.printlnf( "IP:            %s", WiFi.localIP().toString().c_str() );  
  Serial.printlnf( "Subnet:        %s", WiFi.subnetMask().toString().c_str() );
  Serial.printlnf( "Gateway:       %s", WiFi.gatewayIP().toString().c_str() );

  _svc  = std::make_shared< ServiceDirectory >();
  _conf = std::make_shared< Config >();

  Serial.println( "Inserting loopers" );

  // Setup pass1 populates the service directory
  _looperSet.insert( std::make_shared< AlarmSettingsMonitor >( _svc, _conf ) );
  _looperSet.insert( std::make_shared< ButtonMonitor >( _svc, _conf ) );
  _looperSet.insert( std::make_shared< PlayerQuery >( _svc, _conf ) );
  _looperSet.insert( std::make_shared< ClockDisplay >( _svc, _conf ) );
  _looperSet.insert( std::make_shared< TimeMonitor >( _svc, _conf ) );
  _looperSet.insert( std::make_shared< AlarmBackup >( _svc, _conf ) );


  Serial.printlnf( "Looper count: %d", _looperSet.size() );
  Serial.println( "Looping setup" );
  delay(1000);

  // Setup pass2 does the actual setup with the service dependencies in place
  for( auto const& looper : _looperSet ) 
  { 
     looper->setup();
  }

    // Set up the MAX17043 LiPo fuel gauge:
  lipo.begin(); // Initialize the MAX17043 LiPo fuel gauge

  // Quick start restarts the MAX17043 in hopes of getting a more accurate
  // guess for the SOC.
  lipo.quickStart();

  // We can set an interrupt to alert when the battery SoC gets too low.
  // We can alert at anywhere between 1% - 32%:
  lipo.setThreshold(20); // Set alert threshold to 20%.

  SyslogUdp.begin(8888);
const size_t bufferSize = 1024;
char buffer[bufferSize];
  IPAddress ipSvr( 192, 168, 1, 11 );
SyslogUdp.sendPacket(buffer, bufferSize, ipSvr, 514);

    Serial.println( "setup complete" );
    Serial.printlnf("free memory: %d", System.freeMemory());

    Serial.flush();

    pinMode(A0, INPUT);


    delay(1000);

}

void 
loop() 
{
  for( auto const& looper : _looperSet ) 
  { 
     looper->loop();
  }

  static int fuelCount = 0;

  if ( fuelCount++ == 500 )
  {
     // sprintf( sz, "Lightness: %d", a0 );
    char sz[128];
    sprintf( sz, "Lipo: %1.2fv %2.1f%% Light:%d Mem:%d", lipo.getVoltage(), lipo.getSOC(), analogRead(A0), System.freeMemory() );
    sendSyslogMessage( sz );


    fuelCount = 0;
  }

  // TODO: the particle way
  delay(10);
}



#ifdef ITCHYANDSCRATCHY


Thread* led1Thread;
Thread* led2Thread;
Thread* led3Thread;


  led1Thread = new Thread("sample", ledBlink, (void*)0);
  led2Thread = new Thread("sample", ledBlink, (void*)1);
  led3Thread = new Thread("sample", ledBlink, (void*)2);


os_thread_return_t 
ledBlink(void* param)
{
  {
        CriticalSection cs;
        Serial.printlnf("Thread Parameters: %d", (int)param );    
  }

  while( true );
}


uint16_t      s_lastTouched = 0;


void cbSnooze( const std::string& snoozeRsp )
{
   
   Serial.println( snoozeRsp.c_str() );
    
        infoCallBack = 0;
        sqControl.stop();
}




  //EEPROM.write(0, 1);
  s_DST= EEPROM.read(0) > 0;

  pinMode( c_ledPin, OUTPUT  );
/*
  if (!s_buttonMux.begin()) 
  {
    digitalWrite( c_ledPin, HIGH );  
    while(1);
  }
  else
  {
    digitalWrite( c_ledPin, LOW );  
  }
*/


    digitalWrite( c_ledPin, HIGH );  
    




  // on a rising edge toggle the counter
  uint16_t currtouched = s_buttonMux.touched();
  if ( ( currtouched & 1 ) != 0 && ( s_lastTouched & 1 ) == 0 )
  {
    s_countEnabled = !s_countEnabled;
  }
  s_lastTouched = currtouched;


    // TODO: adjust for DST
    //DateTime now = s_rtClock.now();
    //s_clockTime = ClockTime( now.hour(), now.minute() );
   // s_clockTime = ClockTime( Time.hour(), Time.minute() );

    
    // DST is stored in the RTC NVRAM at address 08h
   // s_DST = s_rtClock.readnvram(0x08) > 0;
    //s_DST = EEPROM.read( 0x00 ) > 0; 
    

  #endif



