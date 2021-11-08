/*
 * Copyright (c) 2021 amish <<https://github.com/amishv/Voltronics_Solar_protocol18>>
 * 
 * Created Date: Saturday, November 6th 2021, 7:47:35 pm
 * Author: amish
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS
 * IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * HISTORY:
 * Date      	By	Comments
 * ----------	---	----------------------------------------------------------
 * 11/3/2021 	 amish	 Initial commit                                        
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <locale.h>
#include "solarmon.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://your_mqtt_server:1883"
#define QOS         1
#define TIMEOUT     10000L
#define TOPIC       "testmessage"
#define PAYLOAD     "Hello World!"

int main(int argc, char *argv[])
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int mqttStatus;
  uint8_t tmpBuf[150];
  if (openport() < 0)
    exit(EXIT_FAILURE);
  if ((mqttStatus = MQTTClient_create(&client, ADDRESS, CLIENTID,
      MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
  {
    debugPrint("Failed to create client, return code %d\n", mqttStatus);
    exit(EXIT_FAILURE);
  }
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  if ((mqttStatus = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
      debugPrint("Failed to connect, return code %d\n", mqttStatus);
      exit(EXIT_FAILURE);
  }
  sensorInit( client, 1);
  getInverterTime();

  if (argc > 1)
  if (sendCommand(argv[1])==TRUE)
    if (readport(tmpBuf)==TRUE)
      debugPrint("\n \tInverter Response %s. \n", &tmpBuf[5]); 
  //send mqtt every 15 sec
  int time_count = 0; 
  while(1)
  {
    time_count++;
    mqttStatus = getInverterMode( client);
    mqttStatus = ratedInformation(client);
    mqttStatus = generalStatusDisplay(client);
    sleep(15);
    if (time_count % 20 == 0)
      mqttStatus = sensorInit( client, 1); //configure sensors every 5 min
    if (mqttStatus < -15)
    {
      printf("\n Quitting after %d unsuccessful attempts", mqttStatus*(-1));
      break;  //quit if unable to send message, for daemon to be restarted
    }
      
  }
  close(ser_fd);
  sensorInit( client, 0);
  if ((mqttStatus = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    debugPrint("Failed to disconnect, return code %d\n", mqttStatus);
  MQTTClient_destroy(&client);
  return mqttStatus;
}