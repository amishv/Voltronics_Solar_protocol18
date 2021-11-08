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

const char *mpptChargerStatus[] = {
    "Abnormal",
    "Not Charging",
    "Charging"};

const char *PowerDirection[] = {
    "None",
    "Charging",
    "Discharging"};
const char *LineDirection[] = {
    "None  ---",
    "Input <<<",
    "Ouput >>>"};

int generalStatusDisplay(MQTTClient client)
{
  int value_grid_voltage_, value_grid_frequency_, value_ac_output_voltage_, // NOLINT
      value_ac_output_frequency_, value_ac_output_apparent_power_,          // NOLINT
      value_ac_output_active_power_, value_output_load_percent_,            // NOLINT
      value_battery_voltage_, value_battery_voltage_scc1_,
      value_battery_voltage_scc2_, value_battery_discharge_current_,
      valuebattery_charging_current_, value_battery_capacity_percent_,
      value_inverter_heat_sink_temperature_, value_mppt1_charger_temperature_,
      value_mppt2_charger_temperature_, value_pv1_input_power_,
      value_pv2_input_power_, value_pv1_input_voltage_, value_pv2_input_voltage_,
      value_setting_value_configuration_state_, value_mppt1_charger_status_,
      mppt2_charger_status_, value_load_connection_, value_battery_power_direction_,
      value_dc_ac_power_direction_,value_line_power_direction_, value_local_parallel_id_;
  int status;
  char tmpBuf[120];
  if (sendCommand((uint8_t *)"^P005GS") != TRUE)
    return FALSE;
  if (readport((uint8_t *)tmpBuf) != TRUE)
    return FALSE;
  debugPrint("%s\n", tmpBuf);

  sscanf(tmpBuf,                                                                                     // NOLINT
         "^D106%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", // NOLINT
         &value_grid_voltage_, &value_grid_frequency_, &value_ac_output_voltage_,                    // NOLINT
         &value_ac_output_frequency_, &value_ac_output_apparent_power_,                              // NOLINT
         &value_ac_output_active_power_, &value_output_load_percent_,                                // NOLINT
         &value_battery_voltage_, &value_battery_voltage_scc1_, &value_battery_voltage_scc2_,        // NOLINT
         &value_battery_discharge_current_, &valuebattery_charging_current_,                         // NOLINT
         &value_battery_capacity_percent_, &value_inverter_heat_sink_temperature_,                   // NOLINT
         &value_mppt1_charger_temperature_, &value_mppt2_charger_temperature_,                       // NOLINT
         &value_pv1_input_power_, &value_pv2_input_power_, &value_pv1_input_voltage_,                // NOLINT
         &value_pv2_input_voltage_, &value_setting_value_configuration_state_,                       // NOLINT
         &value_mppt1_charger_status_, &mppt2_charger_status_, &value_load_connection_,              // NOLINT
         &value_battery_power_direction_, &value_dc_ac_power_direction_,
         &value_line_power_direction_, &value_local_parallel_id_);
  //convert to ascii and send MQTT messages
  sprintf(tmpBuf, "%5.1f", (float)value_grid_voltage_ / 10);
  sendMQTTmessage(client, "AC_grid_voltage", tmpBuf);
  sprintf(tmpBuf, "%5.1f", (float)value_grid_frequency_ / 10);
  sendMQTTmessage(client, "AC_grid_frequency", tmpBuf);
  sprintf(tmpBuf, "%5.1f", (float)value_ac_output_voltage_ / 10);
  sendMQTTmessage(client, "AC_out_voltage", tmpBuf);
  sprintf(tmpBuf, "%5.1f", (float)value_ac_output_frequency_ / 10);
  sendMQTTmessage(client, "AC_out_frequency", tmpBuf);
  sprintf(tmpBuf, "%5.1f", (float)value_pv1_input_voltage_ / 10);
  sendMQTTmessage(client, "PV_in_voltage", tmpBuf);
  sprintf(tmpBuf, "%d", value_pv1_input_power_ );
  sendMQTTmessage(client, "PV_in_power", tmpBuf);

  sprintf(tmpBuf, "%5.1f", (float)value_battery_voltage_scc1_ / 10);
  sendMQTTmessage(client, "SCC_voltage", tmpBuf);
  sprintf(tmpBuf, "%d", value_output_load_percent_ );
  sendMQTTmessage(client, "Load_pct", tmpBuf);
  sprintf(tmpBuf, "%d", value_ac_output_active_power_ );
  sendMQTTmessage(client, "Load_watt", tmpBuf);
  sprintf(tmpBuf, "%d", value_ac_output_apparent_power_ );
  sendMQTTmessage(client, "Load_va", tmpBuf);
  sprintf(tmpBuf, "%d", value_inverter_heat_sink_temperature_ );
  sendMQTTmessage(client, "Heatsink_temperature", tmpBuf);
  sprintf(tmpBuf, "%d", value_mppt1_charger_temperature_ );
  sendMQTTmessage(client, "MPPT_temperature", tmpBuf);
  sprintf(tmpBuf, "%d", value_battery_capacity_percent_);
  sendMQTTmessage(client, "Battery_capacity", tmpBuf);
  sprintf(tmpBuf, "%5.1f", (float)value_battery_voltage_ / 10);
  sendMQTTmessage(client, "Battery_voltage", tmpBuf);
  sprintf(tmpBuf, "%d", valuebattery_charging_current_);
  sendMQTTmessage(client, "Battery_charge_current", tmpBuf);
  sprintf(tmpBuf, "%d",  value_battery_discharge_current_ );
  sendMQTTmessage(client, "Battery_discharge_current", tmpBuf);

  sendMQTTmessage(client, "Load_status_on", (value_load_connection_)?"Connected":"Disconnected");
  sendMQTTmessage(client, "SCC_charge_on", (void *)mpptChargerStatus[value_mppt1_charger_status_]);
  
  sendMQTTmessage(client, "AC_Power_dir", (void *)LineDirection[value_line_power_direction_]);
  sendMQTTmessage(client, "DC/AC_Power_dir", (void *)PowerDirection[value_dc_ac_power_direction_]);
  /*     Grid frequency\t\t\t%4.1fHz\n\
         AC output voltage\t\t\t%5.1fV \
        AC output frequency\t\t%4.1fHZ\n\n\
        AC output apparent power\t\t %4dVA\
        AC output active power\t\t%4dVA\n\
        Output load percent\t\t\t  %3d%% \
        Battery voltage\t\t\t%4.1fV\n\
        Battery voltage from SCC\t\t %4.1fV \
        Battery voltage from SCC2\t%4.1fV \n\
        Battery discharge current\t\t  %3dA \
        Battery charging current\t\t %3dA \n\
        Battery capacity\t\t\t  %3d%% \
        Inverter heat sink temperature\t %3dC \n\
        MPPT1 charger temperature\t\t  %3dC \
        MPPT2 charger temperature\t %3dC \n\n\
        PV1 Input power	\t\t\t%'4dVA\
        PV2 Input power\t\t\t%4dVA\n\
        PV1 Input voltage\t\t\t%5.1fV \
        PV2 Input voltage\t       %5.1fV \n\n\
        Setting value configuration state     %s\
        MPPT1 charger status\t  %s\n\
        MPPT2 charger status\t\t   %s\
        Load connection\t\t     %s \n\
        Battery power direction\t\t    %s\
        DC/AC power direction\t   %s\n\
        Line power direction\t\t      %s\
        Local parallel ID\t\t%d \n",
        (float)value_grid_voltage_/10, (float)value_grid_frequency_/10, (float)value_ac_output_voltage_/10,                 // NOLINT
        (float)value_ac_output_frequency_/10, value_ac_output_apparent_power_,                           // NOLINT
        value_ac_output_active_power_, value_output_load_percent_,                            // NOLINT
        (float)value_battery_voltage_/10, (float)value_battery_voltage_scc1_/10, 
        (float)value_battery_voltage_scc2_/10,     // NOLINT
        value_battery_discharge_current_, valuebattery_charging_current_,                      // NOLINT
        value_battery_capacity_percent_, value_inverter_heat_sink_temperature_,                // NOLINT
        value_mppt1_charger_temperature_, value_mppt2_charger_temperature_,                    // NOLINT
        value_pv1_input_power_, value_pv2_input_power_, (float)value_pv1_input_voltage_/10,             // NOLINT
        (float)value_pv2_input_voltage_/10, 
        (value_setting_value_configuration_state_)?"  Changed":"No Change",                    // NOLINT
        mpptChargerStatus[value_mppt1_charger_status_], mpptChargerStatus[mppt2_charger_status_], 
        (value_load_connection_)?"Connected":"Disconnected",           // NOLINT
        PowerDirection[value_battery_power_direction_],PowerDirection[value_dc_ac_power_direction_], 
        LineDirection[value_line_power_direction_], value_local_parallel_id_ );

    time_t rawtime;
    struct tm * localTime;
    time ( &rawtime );
    localTime = localtime ( &rawtime );
    //sprintf(tmpBuf,"^S018DAT%02d%02d%02d%02d%02d%02d",localTime->tm_year-100,localTime->tm_mon+1,localTime->tm_mday,localTime->tm_hour,localTime->tm_min+1,localTime->tm_sec );
    //printf("%s\n", tmpBuf);
    //sendCommand(tmpBuf);
    printf("\n\tenergy generated today: (%02d/%02d/%04d):-> %'8.3fkWh\n", 
            localTime->tm_mday , localTime->tm_mon+1, localTime->tm_year-100,energyGenerated(localTime, enDay));
    printf("\tenergy generated this month (%2d/%04d):->%'8.3fkWh\n", 
            localTime->tm_mon+1, localTime->tm_year-100, energyGenerated(localTime, enMon));
    printf("\tenergy generated this year (%04d):  -> %'8.3fkWh\n", 
            localTime->tm_year+1900, energyGenerated(localTime, enYear));
*/
}


