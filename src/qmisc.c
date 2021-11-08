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
#include <sys/stat.h>
#include <strings.h>
#include <time.h>

#include "solarmon.h"
int getInverterMode(MQTTClient client)
{
  char tmpBuf[20];
  int status;
  if (sendCommand("^P006MOD")==TRUE)
    if (readport(tmpBuf)==TRUE){
      debugPrint("\n \tInverter is in %s. \n", inverterStatus[tmpBuf[6]-'0']); 
      status = sendMQTTmessage(client, "Inverter_mode", (void *) inverterStatus[tmpBuf[6]-'0']);
    }
  return status;
}
int getInverterTime(void) {
  char tmpBuf[30], protocol[3];
  int year,mon,day,hr,min,sec;
  if (!sendCommand("^P005PI"))
        return FALSE;
  if (!readport((uint8_t *)tmpBuf))
        return FALSE;
  debugPrint("%s\n", tmpBuf);
  sscanf((uint8_t *)tmpBuf,"^D005%2s", protocol);
  if (!sendCommand((uint8_t *)"^P004T"))
        return FALSE;
  if (!readport((uint8_t *)tmpBuf))
        return FALSE;
  debugPrint("%s\n", tmpBuf);
  sscanf(tmpBuf,"^D017%04d%02d%02d%02d%02d%02d",&year,&mon,&day,&hr,&min,&sec);
  debugPrint("\n\tInverter Time : %04d/%02d/%02d     %02d:%02d:%02d\t\t\t\tProtocol:%s\n",year,mon,day,hr,min,sec, protocol);
}


long energyDay( struct tm *tm)
{
    char tmpBuf[20];
    int energyGen;
    sprintf(tmpBuf, "^P013ED20%02d%02d%02d", tm->tm_year-100, tm->tm_mon+1,tm->tm_mday);
     if (sendCommand((uint8_t *)tmpBuf)!=TRUE)
        return FALSE;
    bzero(tmpBuf, sizeof(tmpBuf));
    if (readport((uint8_t *)tmpBuf)!=TRUE)
        return FALSE; 
    sscanf(tmpBuf,"^D011%08d", &energyGen ) ;
    debugPrint("Energy Response %s\n return %08d", tmpBuf, energyGen); 
    return energyGen;
}
long energyMonth( struct tm *tm)
{
    char tmpBuf[20];
    int energyGen;
    sprintf(tmpBuf, "^P011EM20%02d%02d", tm->tm_year-100, tm->tm_mon+1);
     if (sendCommand((uint8_t *)tmpBuf)!=TRUE)
        return FALSE;
    bzero(tmpBuf, sizeof(tmpBuf));
    if (readport((uint8_t *)tmpBuf)!=TRUE)
        return FALSE;
    debugPrint("Energy Response %s\n", tmpBuf);  
    sscanf(tmpBuf,"^D011%d", &energyGen ) ;
    return energyGen;
}

float energyGenerated( struct tm *tm, int type)
{
    char tmpBuf[20];
    int energyGen;
    if (type == enDay)
      sprintf(tmpBuf, "^P013ED%04d%02d%02d", tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday);
    else if (type == enMon)
      sprintf(tmpBuf, "^P011EM%04d%02d", tm->tm_year+1900, tm->tm_mon+1);
    else  if (type == enYear)
      sprintf(tmpBuf, "^P009EY%04d", tm->tm_year+1900);
    else 
      return (FALSE);
    if (sendCommand((uint8_t *) tmpBuf)!=TRUE)
        return FALSE;
    bzero(tmpBuf, sizeof(tmpBuf));
    if (readport((uint8_t *)tmpBuf)!=TRUE)
        return FALSE; 
    sscanf(tmpBuf,"^D011%08d", &energyGen ) ;
    debugPrint("Energy Response %s\n return %08d", tmpBuf, energyGen); 
    return (float)energyGen/1000;
}