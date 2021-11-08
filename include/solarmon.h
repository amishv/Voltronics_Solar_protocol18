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
// #solarmon.h 
//srarted on 5/3/2019
#ifndef SOLARMON_H__
#define SOLARMON_H__

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include <MQTTClient.h>

#define CLIENTID    "Solarmon"
//#define _POSIX_SOURCE 1 /* POSIX compliant source */

static const int enDay  = 1;
static const int enMon  = 2;
static const int enYear = 3;

static const int TRUE = 1;
static const int FALSE = -1;

static const char *PROG_NAME = "SolarMon";
static const char mqttTopic[100];
static int ser_fd;

static const char *inverterStatus[] = {
    "Power on mode",
    "Standby mode",
    "Bypass mode",
    "Battery mode",
    "Fault mode",
    "Hybrid mode"
};

static const char  command[][10] = {
                      "^P005PI",
                      "^P005GS",
                      "^P005ET",
                      "^P009EY",
                      "^P011EM",
                      "^P013ED"
                      };

uint16_t cal_crc_half(uint8_t  *pin, uint8_t len);
int sendCommand(uint8_t *cmd);
int openport(void);
int readport(uint8_t *recv_buf);  
int sendMQTTmessage(MQTTClient client, char *topic, void *msg);
int sensorInit(MQTTClient client, int init_uninit);
int getInverterMode(MQTTClient client);
int getInverterTime(void) ;
int generalStatusDisplay (MQTTClient client);
float energyGenerated( struct tm *tm, const int type);
int ratedInformation(MQTTClient client);
void debugPrint(const char *format, ...);

#endif //SOLARMON_H__
//gcc src/solarmon.c src/qpigs.c src/qpiri.c src/communication.c src/qmisc.c  -I./include -lpaho-mqtt3c