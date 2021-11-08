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
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "solarmon.h"

const char *MODEMDEVICE = "/dev/ttyUSB0";
const long TIMEOUT   =  10000L;
#define BAUDRATE B2400
int debugFlag =0;
int readport(uint8_t *recv_buf)
{
  int n,  i = 0;
  bzero(recv_buf, sizeof(recv_buf));
  if (ser_fd >= 0)
  {
    debugPrint("Reading from Port \n");
    do
    {
      n = read(ser_fd, &recv_buf[i], 1);
      if (n < 0)
        perror(" Failed to read byte \n");
      debugPrint("readport: %03d: %02X >>> (%c)\n", i, recv_buf[i], (uint8_t)recv_buf[i]);

    } while (((recv_buf[i] != '\n') && (recv_buf[i++] != '\r')) && (i < 150));
  }
  if ((recv_buf[0] == '^') && (recv_buf[1] == '0')){
      debugPrint( "Command Refused \n");
      return (-2);   
  }
  if ((recv_buf[0] != '^') || (recv_buf[1] != 'D')){
      debugPrint( "Response corrupted \n");
      return (-1);   
  }
  //debugPrint("%d --->>%s\n", n, recv_buf);
  uint16_t recCRC = cal_crc_half(recv_buf, i-1);
  if (0 != recCRC){
    debugPrint( " CRC (%04X) Failed in Received message \n", recCRC);
    debugPrint("CRC " );//return FALSE;
  }
  return TRUE;
}
// Sending byte by byte as Inverter DSP does not like stream
int sendport(uint8_t *tmpbuffer)
{
  int n, j, i = 0;
  int status = FALSE;
  int len = strlen((char *)tmpbuffer);
  uint8_t t;
  debugPrint("enter write  %s  with len %ld\n", tmpbuffer, len);
  if (ser_fd >= 0)
  {
    tcflush(ser_fd, TCIOFLUSH); //clear the buffer
    do // send one char at a time for slow CPU on inverter
    {
      t = tmpbuffer[i++];
      n = write(ser_fd, &t , 1);
      for (j=0; j< 30000; j++);
    } while ((t != '\r') &&(i< len+1));//
    if (n < 0)
      debugPrint("writeof (%ld) bytes failed!\n", len+1);
    else{
      status = TRUE;
      debugPrint(" %d Bytes Sent successfully to fd %d staus %d\n",i, ser_fd, n);
    }
  }
  return status;
}

int sendCommand(uint8_t *cmd)
{
  uint8_t sendStr[30];
  uint32_t cmdLen = strlen((char *)cmd);
  uint16_t cmdCrc = cal_crc_half(cmd, cmdLen);
  sprintf((uint8_t *)sendStr, "%s%c%c\r", cmd, (uint8_t)(cmdCrc >> 8), (uint8_t)(cmdCrc & 0xFF));
  debugPrint("sendCommand: %02lu bytes %s to send---CRC %04X\n", strlen((char *)sendStr), sendStr, cmdCrc);
  return (sendport(sendStr));
}

int openport(void)
{
  struct termios oldtp, newtp;
  ser_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
  debugPrint("\n%s :%d\n", PROG_NAME, ser_fd);
  if (ser_fd < 0)
  {
    perror(MODEMDEVICE);
    debugPrint("\n%s is unable to open serial :%d\n", PROG_NAME, ser_fd);
    return (-1);
  }
  debugPrint("\n%s openining serial (%d):%d\n", PROG_NAME, BAUDRATE, ser_fd);
  fcntl(ser_fd, F_SETFL, 0);
  tcgetattr(ser_fd, &oldtp); /* save current serial port settings */
  //tcgetattr(ser_fd,&newtp); /* save current serial port settings to modify*/
  //manual setup of comm Port
  // following not needed, but I prefer to define explicitly to avoid surprises
  bzero(&newtp, sizeof(newtp)); // create from scratch
  newtp.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtp.c_iflag = IGNPAR;
  newtp.c_oflag = 0;
  newtp.c_lflag = 0;        //ICANON;
  newtp.c_cc[VINTR] = 0;    /* Ctrl-c */
  newtp.c_cc[VQUIT] = 0;    /* Ctrl-\ */
  newtp.c_cc[VERASE] = 0;   /* del */
  newtp.c_cc[VKILL] = 0;    /* @ */
  newtp.c_cc[VTIME] = 10;   /* inter-character timer unused */
  newtp.c_cc[VMIN] = 0;     /* no blocking, read until 8 character arrives */
  newtp.c_cc[VSWTC] = 0;    /* '\0' */
  newtp.c_cc[VSTART] = 0;   /* Ctrl-q */
  newtp.c_cc[VSTOP] = 0;    /* Ctrl-s */
  newtp.c_cc[VSUSP] = 0;    /* Ctrl-z */
  newtp.c_cc[VEOL] = 0;     /* '\0' */
  newtp.c_cc[VREPRINT] = 0; /* Ctrl-r */
  newtp.c_cc[VDISCARD] = 0; /* Ctrl-u */
  newtp.c_cc[VWERASE] = 0;  /* Ctrl-w */
  newtp.c_cc[VLNEXT] = 0;   /* Ctrl-v */
  newtp.c_cc[VEOL2] = 0;    /* '\0' */
  tcflush(ser_fd, TCIFLUSH);
  tcsetattr(ser_fd, TCSANOW, &newtp);
  tcflush(ser_fd, TCIFLUSH);
  // explicit configuration ends
  return 0;
}

uint16_t cal_crc_half(uint8_t *pin, uint8_t len)
{
  uint16_t crc;
  uint8_t da;
  uint8_t *ptr;
  uint8_t bCRCHign;
  uint8_t bCRCLow;
  uint16_t crc_ta[16] =
      {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
      };
  ptr = pin;
  crc = 0;
  while (len-- != 0)
  {
    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= crc_ta[da ^ (*ptr >> 4)];
    da = ((uint8_t)(crc >> 8)) >> 4;
    crc <<= 4;
    crc ^= crc_ta[da ^ (*ptr & 0x0f)];
    ptr++;
  }
  bCRCLow = crc;
  bCRCHign = (uint8_t)(crc >> 8);
  if (bCRCLow == 0x28 || bCRCLow == 0x0d || bCRCLow == 0x0a)
  {
    bCRCLow++;
  }
  if (bCRCHign == 0x28 || bCRCHign == 0x0d || bCRCHign == 0x0a)
  {
    bCRCHign++;
  }
  crc = ((uint16_t)bCRCHign) << 8;
  crc += bCRCLow;
  return (crc);
}

void debugPrint(const char *format, ...)
  {
    // Only print if debug flag is set, else do nothing
    if (debugFlag) {
      va_list ap;
      char fmt[2048];

      //actual time
      time_t rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo = localtime(&rawtime);
      char buf[256];
      strcpy(buf, asctime(timeinfo));
      buf[strlen(buf)-1] = 0;

      //connect with args
      snprintf(fmt, sizeof(fmt), "%s %s\n", buf, format);

      //put on screen:
      va_start(ap, format);
      vprintf(fmt, ap);
      va_end(ap);
    }
  }
  int sendMQTTmessage(MQTTClient client, char *topic_str, void *msg)
  {
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer; 
    static int errorCount = 0;
    int mqttStatus;
    char topic[100];
    sprintf(topic, "homeassistant/sensor/%s/state",topic_str);
    pubmsg.payload = msg;    
    pubmsg.payloadlen = (int)strlen(msg);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    if ((mqttStatus = MQTTClient_publishMessage(client, topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
      debugPrint("Failed to publish message \'%s\' len %d, return code %d\n", topic,pubmsg.payloadlen, mqttStatus);
      perror ("sendMQTT");
      errorCount--;
      return errorCount;
    }
    if ((mqttStatus =MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS)
    {
      debugPrint("Failed to publish message \'%s\' len %d, return code %d\n", topic,pubmsg.payloadlen, mqttStatus);
      perror ("sendMQTT");
      errorCount--;
      return errorCount;
    }
  debugPrint("Message with delivery token %d delivered\n", token);
  errorCount = 0; //reset counter after a successfull delivery
  return mqttStatus;
  }
/*
    Registering in Hass https://www.home-assistant.io/docs/mqtt/discovery/

    Configuration Parameters:
     
    topic: homeassistant/sensor/voltronics_<topic_str>/config
    Payload: 
    {
      "name": "<toppic_str>", 
      "unit_of_measurement": "<unit_name>",
      "unique_id": "toppic_str",
      "state_topic": "homeassistant/sensor/<toppic_str>/state",
      "icon": "mdi:<icon_name>",
      "retain": true
    }
*/
int registerTopic (MQTTClient client, char *topic_str, char *unit_name, char *icon_name,  int init_uninit)
{
  MQTTClient_deliveryToken token;
  MQTTClient_message pubmsg = MQTTClient_message_initializer; 
  static int errorCount = 0;
  int mqttStatus;
  char topic[100];
  char message[500];
  sprintf(topic, "homeassistant/sensor/voltronic_%s/config",topic_str);
  if (init_uninit ==1)
    sprintf(message, "{ \n\
                        \"name\": \"%s\",\n\
                        \"unit_of_measurement\": \"%s\",\n\
                        \"unique_id\": \"topic_str\",\n\
                        \"state_topic\": \"homeassistant/binary_sensor/%s/state\",\n\
                        \"icon\": \"mdi:%s\",\n\
                        \"retain: true\n\
                      }\n", topic_str, unit_name, topic_str, icon_name);
  else //send a blank payload to deregister the sensor
    sprintf(message, "");
  debugPrint("topic: %s\n", topic);
  debugPrint("message: %s\n", message);
  pubmsg.payload = message;    
  pubmsg.payloadlen = (int)strlen(message);
  pubmsg.qos = 1;
  pubmsg.retained = 0;
  if ((mqttStatus = MQTTClient_publishMessage(client, topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
  {
    debugPrint("Failed to publish message \'%s\' len %d, return code %d\n", topic,pubmsg.payloadlen, mqttStatus);
    perror ("sendMQTT");
    errorCount--;
    return errorCount;
  }
  debugPrint("Waiting for  for publication of %s\n"
      "on topic %s for client with ClientID: %s\n",  pubmsg.payload, topic, CLIENTID);
  if ((mqttStatus =MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS)
  {
    debugPrint("Failed to publish message \'%s\' len %d, return code %d\n", topic,pubmsg.payloadlen, mqttStatus);
    perror ("sendMQTT");
    errorCount--;
    return errorCount;
  }
  debugPrint("Message with delivery token %d delivered\n", token);
  errorCount = 0; //reset counter after a successfull delivery
  return mqttStatus;

}
int sensorInit(MQTTClient client, int init_uninit)
{
  int status = FALSE;
  status = registerTopic( client,"Inverter_mode", "", "solar-power", init_uninit);
  status = registerTopic( client,"AC_grid_voltage","V","power-plug", init_uninit);
  status = registerTopic( client,"AC_grid_frequency", "Hz", "current-ac", init_uninit);
  status = registerTopic( client,"AC_out_voltage","V","power-plug", init_uninit);
  status = registerTopic( client,"AC_out_frequency","Hz","current-ac", init_uninit);
  status = registerTopic( client,"PV_in_voltage","V","solar-panel-large", init_uninit);
  status = registerTopic( client,"PV_in_current","A","solar-panel-large", init_uninit);
  status = registerTopic( client,"PV_in_power","W","solar-panel-large", init_uninit);
  status = registerTopic( client, "SCC_voltage","V","current-dc", init_uninit);
  status = registerTopic( client, "Load_pct","%%","brightness-percent", init_uninit);
  status = registerTopic( client, "Load_watt","W","chart-bell-curve", init_uninit);
  status = registerTopic( client, "Load_watthour","Wh","chart-bell-curve", init_uninit);
  status = registerTopic( client, "Load_va","VA","chart-bell-curve", init_uninit);
  status = registerTopic( client, "Bus_voltage","V","details", init_uninit);
  status = registerTopic( client, "Heatsink_temperature","C","details", init_uninit);
  status = registerTopic( client, "MPPT_temperature","C","details", init_uninit);
  status = registerTopic( client, "Battery_capacity","%%","battery-outline", init_uninit);
  status = registerTopic( client, "Battery_voltage","V","battery-outline", init_uninit);
  status = registerTopic( client, "Battery_charge_current","A","current-dc", init_uninit);
  status = registerTopic( client, "Battery_discharge_current","A","current-dc", init_uninit);
  status = registerTopic( client, "Load_status_on","","power", init_uninit);
  status = registerTopic( client, "AC_Power_dir","","power", init_uninit);
  status = registerTopic( client, "DC/AC_Power_dir","","power", init_uninit);
  status = registerTopic( client, "SCC_charge_on","","power", init_uninit);
  status = registerTopic( client, "AC_charge_on","","power", init_uninit);
  status = registerTopic( client, "Battery_recharge_voltage","V","current-dc", init_uninit);
  status = registerTopic( client, "Battery_under_voltage","V","current-dc", init_uninit);
  status = registerTopic( client, "Battery_bulk_voltage","V","current-dc", init_uninit);
  status = registerTopic( client, "Battery_float_voltage","V","current-dc", init_uninit);
  status = registerTopic( client, "Max_grid_charge_current","A","current-ac", init_uninit);
  status = registerTopic( client, "Max_charge_current","A","current-ac", init_uninit);
  status = registerTopic( client, "Out_source_priority","","grid",init_uninit);
  status = registerTopic( client, "Charger_source_priority","","solar-power", init_uninit);
  status = registerTopic( client, "Battery_redischarge_voltage","V","battery-negative", init_uninit);
  return status;
}