#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <strings.h>
#include <time.h>

#include "solarmon.h"


static const char *BatteryType[] = {
    "    AGM",
    "Flooded",
    "   User"
};
static const char *ChargeSource[] = {
    "    Solar first",
    "Solar & Utility",
    "     Only solar"
};
static const char *OutputPriority[] = {
    "S-U-B", //Solar-Utility-Battery",
    "S-B-U"  //Solar-Battery-Utility"
};
static const char *OutputSetting[] = {
    "  Single",
    "Parallel",
    " Phase 1",
    " Phase 2",
    " Phase 3"
};

int ratedInformation(MQTTClient client)
{
  int value_AC_input_rating_voltage_, value_AC_input_rating_current_, value_AC_output_rating_voltage_,
    value_AC_output_rating_frequency_, value_AC_output_rating_current_, 
    value_AC_output_rating_apparent_power_, value_AC_output_rating_active_power_, 
    value_Battery_rating_voltage_, value_Battery_recharge_voltage_, value_Battery_redischarge_voltage_,
    value_Battery_under_voltage_, value_Battery_bulk_voltage_, value_Battery_float_voltage_, 
    value_Battery_type_, value_Max_AC_charging_current_, value_Max_charging_current_, 
    value_Input_voltage_range_, value_Output_source_priority_, value_Charger_source_priority_, 
    value_Parallel_max_num_, value_Machine_type_, value_Topology_, value_Output_model_setting_, 
    value_Solar_power_priority_, value_MPPT_string_;
  
  char tmpBuf[120];
  if (sendCommand((uint8_t *)"^P007PIRI")!=TRUE)
      return FALSE;
  if (readport((uint8_t *)tmpBuf)!=TRUE)
      return FALSE;
  debugPrint("%s\n", tmpBuf);
  sscanf( tmpBuf,                                                                               // NOLINT
      "^D088%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
      &value_AC_input_rating_voltage_, &value_AC_input_rating_current_, &value_AC_output_rating_voltage_,
      &value_AC_output_rating_frequency_, &value_AC_output_rating_current_, 
      &value_AC_output_rating_apparent_power_, &value_AC_output_rating_active_power_, 
      &value_Battery_rating_voltage_, &value_Battery_recharge_voltage_, &value_Battery_redischarge_voltage_,
      &value_Battery_under_voltage_, &value_Battery_bulk_voltage_, &value_Battery_float_voltage_, 
      &value_Battery_type_, &value_Max_AC_charging_current_, &value_Max_charging_current_, 
      &value_Input_voltage_range_, &value_Output_source_priority_, &value_Charger_source_priority_, 
      &value_Parallel_max_num_, &value_Machine_type_, &value_Topology_, &value_Output_model_setting_, 
      &value_Solar_power_priority_, &value_MPPT_string_);


  sendMQTTmessage(client, "Out_source_priority", (void *)OutputPriority[value_Output_source_priority_]);
  sendMQTTmessage(client, "Charger_source_priority", (void *)ChargeSource[value_Charger_source_priority_]);
  /* sendMQTTmessage(client, "Load_status_on", PowerDirection[value_battery_power_direction_]);
  sendMQTTmessage(client, "Load_status_on", );
  sendMQTTmessage(client, "Load_status_on", );
  printf("\t  AC input rating voltage\t\t%5.1fV\
        AC input rating current\t\t  %4.1fA\n\
        AC output rating voltage\t\t%5.1fV\
        AC output rating frequency\t  %4.1fHz\n\
        AC output rating current \t\t %4.1fA\
        AC output rating apparent power\t  %4dVA\n\
        AC output rating active power\t\t %4dW\
        Battery rating voltage\t\t  %4.1fA\n\
        Battery re-charge voltage\t\t %4.1fV\
        Battery re-discharge voltage\t  %4.1fV\n\
        Battery under voltage\t\t\t %4.1fV\
        Battery bulk voltage\t\t  %4.1fV\n\
        Battery float voltage\t\t\t %4.1fV\
        Battery type\t\t\t%s\n\
        Max AC charging current\t\t\t  %3dA\
        Max charging current\t\t   %3dA\n\
        Input voltage range\t\t     %s\
        Output source priority\t\t %s\n\
        Charger source priority\t\t%s\
        Parallel max num\t\t    %d\n\
        Machine type\t\t\t      %s\
        Topology\t\t\t %s\n\
        Output model setting\t\t      %s\
        Solar power priority\t\t %s\n\
        MPPT string %d \n",
        (float)value_AC_input_rating_voltage_/10, (float)value_AC_input_rating_current_/10, 
        (float)value_AC_output_rating_voltage_ /10, (float)value_AC_output_rating_frequency_/10, 
        (float)value_AC_output_rating_current_/10, value_AC_output_rating_apparent_power_, 
        value_AC_output_rating_active_power_,(float)value_Battery_rating_voltage_/10, 
        (float)value_Battery_recharge_voltage_/10, (float)value_Battery_redischarge_voltage_/10,
        (float)value_Battery_under_voltage_/10, (float)value_Battery_bulk_voltage_/10, 
        (float)value_Battery_float_voltage_/10, BatteryType[value_Battery_type_], 
        value_Max_AC_charging_current_, value_Max_charging_current_, 
        (value_Input_voltage_range_)?"appliance":"UPS", OutputPriority[value_Output_source_priority_], 
        ChargeSource[value_Charger_source_priority_], value_Parallel_max_num_, 
        (value_Machine_type_)?"Grid Tie":"Off Grid", (value_Topology_)?"Transformer":"Transformerless",
        OutputSetting[value_Output_model_setting_], (value_Solar_power_priority_)?"L-B-U":"B-L-U",
        value_MPPT_string_);
*/ 
  return TRUE;
}