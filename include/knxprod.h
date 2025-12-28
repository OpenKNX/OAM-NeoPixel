#pragma once


#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                         (time & 0x3FFF) * 3600000 ) : 0 )
                                             
#define ETS_ModuleId_NONE 0
#define ETS_ModuleId_BASE 1
#define ETS_ModuleId_NET 2
#define ETS_ModuleId_UCT 3
#define ETS_ModuleId_LOG 4
#define ETS_ModuleId_NEOEFF 5
#define ETS_ModuleId_NEO 6
#define ETS_ModuleId_NEOSTRIP 7
#define MAIN_FirmwareName "NeoPixel KNX Adapter (Dev)"
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 1
#define MAIN_ApplicationVersion 1
#define MAIN_ApplicationEncoding iso-8859-15
#define MAIN_ParameterSize 7358
#define MAIN_MaxKoNumber 1223
#define MAIN_OrderNumber "NeoPixel"
#define BASE_ModuleVersion 22
#define NET_ModuleVersion 5
#define UCT_ModuleVersion 4
#define LOG_ModuleVersion 56
#define NEO_ModuleVersion 0
// Parameter with single occurrence


#define BASE_StartupDelayBase                     0      // 2 Bits, Bit 7-6
#define     BASE_StartupDelayBaseMask 0xC0
#define     BASE_StartupDelayBaseShift 6
#define BASE_StartupDelayTime                     0      // 14 Bits, Bit 13-0
#define     BASE_StartupDelayTimeMask 0x3FFF
#define     BASE_StartupDelayTimeShift 0
#define BASE_HeartbeatDelayBase                   2      // 2 Bits, Bit 7-6
#define     BASE_HeartbeatDelayBaseMask 0xC0
#define     BASE_HeartbeatDelayBaseShift 6
#define BASE_HeartbeatDelayTime                   2      // 14 Bits, Bit 13-0
#define     BASE_HeartbeatDelayTimeMask 0x3FFF
#define     BASE_HeartbeatDelayTimeShift 0
#define BASE_Timezone                             4      // 5 Bits, Bit 7-3
#define     BASE_TimezoneMask 0xF8
#define     BASE_TimezoneShift 3
#define BASE_CombinedTimeDate                     4      // 1 Bit, Bit 2
#define     BASE_CombinedTimeDateMask 0x04
#define     BASE_CombinedTimeDateShift 2
#define BASE_SummertimeAll                        4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeAllMask 0x03
#define     BASE_SummertimeAllShift 0
#define BASE_SummertimeDE                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeDEMask 0x03
#define     BASE_SummertimeDEShift 0
#define BASE_SummertimeWorld                      4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeWorldMask 0x03
#define     BASE_SummertimeWorldShift 0
#define BASE_SummertimeKO                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeKOMask 0x03
#define     BASE_SummertimeKOShift 0
#define BASE_TimezoneCustom                       5      // char*, 63 Byte
#define BASE_Latitude                            69      // float
#define BASE_Longitude                           73      // float
#define BASE_Diagnose                            78      // 1 Bit, Bit 7
#define     BASE_DiagnoseMask 0x80
#define     BASE_DiagnoseShift 7
#define BASE_Watchdog                            78      // 1 Bit, Bit 6
#define     BASE_WatchdogMask 0x40
#define     BASE_WatchdogShift 6
#define BASE_ReadTimeDate                        78      // 1 Bit, Bit 5
#define     BASE_ReadTimeDateMask 0x20
#define     BASE_ReadTimeDateShift 5
#define BASE_HeartbeatExtended                   78      // 1 Bit, Bit 4
#define     BASE_HeartbeatExtendedMask 0x10
#define     BASE_HeartbeatExtendedShift 4
#define BASE_InternalTime                        78      // 1 Bit, Bit 3
#define     BASE_InternalTimeMask 0x08
#define     BASE_InternalTimeShift 3
#define BASE_ManualSave                          78      // 3 Bits, Bit 2-0
#define     BASE_ManualSaveMask 0x07
#define     BASE_ManualSaveShift 0
#define BASE_PeriodicSave                        79      // 8 Bits, Bit 7-0
#define BASE_Info1LedFunc                        80      // 16 Bits, Bit 15-0
#define BASE_Info2LedFunc                        82      // 16 Bits, Bit 15-0
#define BASE_Info3LedFunc                        84      // 16 Bits, Bit 15-0
#define BASE_Dummy                               109      // uint8_t
#define BASE_ModuleEnabled_NET                   110      // 1 Bit, Bit 6
#define     BASE_ModuleEnabled_NETMask 0x40
#define     BASE_ModuleEnabled_NETShift 6
#define BASE_ModuleEnabled_UCT                   110      // 1 Bit, Bit 5
#define     BASE_ModuleEnabled_UCTMask 0x20
#define     BASE_ModuleEnabled_UCTShift 5
#define BASE_ModuleEnabled_LOG                   110      // 1 Bit, Bit 4
#define     BASE_ModuleEnabled_LOGMask 0x10
#define     BASE_ModuleEnabled_LOGShift 4
#define BASE_ModuleEnabled_NEOEFF                110      // 1 Bit, Bit 3
#define     BASE_ModuleEnabled_NEOEFFMask 0x08
#define     BASE_ModuleEnabled_NEOEFFShift 3
#define BASE_ModuleEnabled_NEO                   110      // 1 Bit, Bit 2
#define     BASE_ModuleEnabled_NEOMask 0x04
#define     BASE_ModuleEnabled_NEOShift 2
#define BASE_ModuleEnabled_NEOSTRIP              110      // 1 Bit, Bit 1
#define     BASE_ModuleEnabled_NEOSTRIPMask 0x02
#define     BASE_ModuleEnabled_NEOSTRIPShift 1

// Zeitbasis
#define ParamBASE_StartupDelayBase                    ((knx.paramByte(BASE_StartupDelayBase) & BASE_StartupDelayBaseMask) >> BASE_StartupDelayBaseShift)
// Zeit
#define ParamBASE_StartupDelayTime                    (knx.paramWord(BASE_StartupDelayTime) & BASE_StartupDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_StartupDelayTimeMS                  (paramDelay(knx.paramWord(BASE_StartupDelayTime)))
// Zeitbasis
#define ParamBASE_HeartbeatDelayBase                  ((knx.paramByte(BASE_HeartbeatDelayBase) & BASE_HeartbeatDelayBaseMask) >> BASE_HeartbeatDelayBaseShift)
// Zeit
#define ParamBASE_HeartbeatDelayTime                  (knx.paramWord(BASE_HeartbeatDelayTime) & BASE_HeartbeatDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_HeartbeatDelayTimeMS                (paramDelay(knx.paramWord(BASE_HeartbeatDelayTime)))
// Zeitzone
#define ParamBASE_Timezone                            ((knx.paramByte(BASE_Timezone) & BASE_TimezoneMask) >> BASE_TimezoneShift)
// Empfangen über
#define ParamBASE_CombinedTimeDate                    ((bool)(knx.paramByte(BASE_CombinedTimeDate) & BASE_CombinedTimeDateMask))
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeAll                       (knx.paramByte(BASE_SummertimeAll) & BASE_SummertimeAllMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeDE                        (knx.paramByte(BASE_SummertimeDE) & BASE_SummertimeDEMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeWorld                     (knx.paramByte(BASE_SummertimeWorld) & BASE_SummertimeWorldMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeKO                        (knx.paramByte(BASE_SummertimeKO) & BASE_SummertimeKOMask)
// POSIX TZ-String
#define ParamBASE_TimezoneCustom                      (knx.paramData(BASE_TimezoneCustom))
// Breitengrad
#define ParamBASE_Latitude                            (knx.paramFloat(BASE_Latitude, Float_Enc_IEEE754Single))
// Längengrad
#define ParamBASE_Longitude                           (knx.paramFloat(BASE_Longitude, Float_Enc_IEEE754Single))
// Diagnoseobjekt anzeigen
#define ParamBASE_Diagnose                            ((bool)(knx.paramByte(BASE_Diagnose) & BASE_DiagnoseMask))
// Watchdog aktivieren
#define ParamBASE_Watchdog                            ((bool)(knx.paramByte(BASE_Watchdog) & BASE_WatchdogMask))
// Bei Neustart vom Bus lesen
#define ParamBASE_ReadTimeDate                        ((bool)(knx.paramByte(BASE_ReadTimeDate) & BASE_ReadTimeDateMask))
// Erweitertes "In Betrieb"
#define ParamBASE_HeartbeatExtended                   ((bool)(knx.paramByte(BASE_HeartbeatExtended) & BASE_HeartbeatExtendedMask))
// InternalTime
#define ParamBASE_InternalTime                        ((bool)(knx.paramByte(BASE_InternalTime) & BASE_InternalTimeMask))
// Manuelles speichern
#define ParamBASE_ManualSave                          (knx.paramByte(BASE_ManualSave) & BASE_ManualSaveMask)
// Zyklisches speichern
#define ParamBASE_PeriodicSave                        (knx.paramByte(BASE_PeriodicSave))
// Info1
#define ParamBASE_Info1LedFunc                        (knx.paramWord(BASE_Info1LedFunc))
// Info2
#define ParamBASE_Info2LedFunc                        (knx.paramWord(BASE_Info2LedFunc))
// Info3
#define ParamBASE_Info3LedFunc                        (knx.paramWord(BASE_Info3LedFunc))
// 
#define ParamBASE_Dummy                               (knx.paramByte(BASE_Dummy))
// NET
#define ParamBASE_ModuleEnabled_NET                   ((bool)(knx.paramByte(BASE_ModuleEnabled_NET) & BASE_ModuleEnabled_NETMask))
// UCT
#define ParamBASE_ModuleEnabled_UCT                   ((bool)(knx.paramByte(BASE_ModuleEnabled_UCT) & BASE_ModuleEnabled_UCTMask))
// LOG
#define ParamBASE_ModuleEnabled_LOG                   ((bool)(knx.paramByte(BASE_ModuleEnabled_LOG) & BASE_ModuleEnabled_LOGMask))
// NEOEFF
#define ParamBASE_ModuleEnabled_NEOEFF                ((bool)(knx.paramByte(BASE_ModuleEnabled_NEOEFF) & BASE_ModuleEnabled_NEOEFFMask))
// NEO
#define ParamBASE_ModuleEnabled_NEO                   ((bool)(knx.paramByte(BASE_ModuleEnabled_NEO) & BASE_ModuleEnabled_NEOMask))
// NEOSTRIP
#define ParamBASE_ModuleEnabled_NEOSTRIP              ((bool)(knx.paramByte(BASE_ModuleEnabled_NEOSTRIP) & BASE_ModuleEnabled_NEOSTRIPMask))

#define BASE_KoHeartbeat 1
#define BASE_KoTime 2
#define BASE_KoDate 3
#define BASE_KoDateTime 4
#define BASE_KoIsSummertime 5
#define BASE_KoManualSave 6
#define BASE_KoDiagnose 7

// In Betrieb
#define KoBASE_Heartbeat                           (knx.getGroupObject(BASE_KoHeartbeat))
// Uhrzeit
#define KoBASE_Time                                (knx.getGroupObject(BASE_KoTime))
// Datum
#define KoBASE_Date                                (knx.getGroupObject(BASE_KoDate))
// Uhrzeit/Datum
#define KoBASE_DateTime                            (knx.getGroupObject(BASE_KoDateTime))
// Sommerzeit aktiv
#define KoBASE_IsSummertime                        (knx.getGroupObject(BASE_KoIsSummertime))
// Speichern
#define KoBASE_ManualSave                          (knx.getGroupObject(BASE_KoManualSave))
// Diagnose
#define KoBASE_Diagnose                            (knx.getGroupObject(BASE_KoDiagnose))

#define NET_HostAddress                         114      // IP address, 4 Byte
#define NET_SubnetMask                          118      // IP address, 4 Byte
#define NET_GatewayAddress                      122      // IP address, 4 Byte
#define NET_NameserverAddress                   126      // IP address, 4 Byte
#define NET_CustomHostname                      130      // 1 Bit, Bit 7
#define     NET_CustomHostnameMask 0x80
#define     NET_CustomHostnameShift 7
#define NET_StaticIP                            130      // 1 Bit, Bit 6
#define     NET_StaticIPMask 0x40
#define     NET_StaticIPShift 6
#define NET_mDNS                                131      // 1 Bit, Bit 7
#define     NET_mDNSMask 0x80
#define     NET_mDNSShift 7
#define NET_HTTP                                131      // 1 Bit, Bit 6
#define     NET_HTTPMask 0x40
#define     NET_HTTPShift 6
#define NET_NTP                                 131      // 1 Bit, Bit 5
#define     NET_NTPMask 0x20
#define     NET_NTPShift 5
#define NET_OTAUpdate                           131      // 2 Bits, Bit 4-3
#define     NET_OTAUpdateMask 0x18
#define     NET_OTAUpdateShift 3
#define NET_HostName                            132      // char*, 24 Byte
#define NET_LanMode                             173      // 4 Bits, Bit 7-4
#define     NET_LanModeMask 0xF0
#define     NET_LanModeShift 4
#define NET_NTPServer                           174      // char*, 50 Byte

// IP-Adresse
#define ParamNET_HostAddress                         (knx.paramInt(NET_HostAddress))
// Subnetzsmaske
#define ParamNET_SubnetMask                          (knx.paramInt(NET_SubnetMask))
// Standardgateway
#define ParamNET_GatewayAddress                      (knx.paramInt(NET_GatewayAddress))
// Nameserver
#define ParamNET_NameserverAddress                   (knx.paramInt(NET_NameserverAddress))
// Hostname anpassen
#define ParamNET_CustomHostname                      ((bool)(knx.paramByte(NET_CustomHostname) & NET_CustomHostnameMask))
// DHCP
#define ParamNET_StaticIP                            ((bool)(knx.paramByte(NET_StaticIP) & NET_StaticIPMask))
// mDNS
#define ParamNET_mDNS                                ((bool)(knx.paramByte(NET_mDNS) & NET_mDNSMask))
// Weberver
#define ParamNET_HTTP                                ((bool)(knx.paramByte(NET_HTTP) & NET_HTTPMask))
// NTP-Client
#define ParamNET_NTP                                 ((bool)(knx.paramByte(NET_NTP) & NET_NTPMask))
// OTA-Update
#define ParamNET_OTAUpdate                           ((knx.paramByte(NET_OTAUpdate) & NET_OTAUpdateMask) >> NET_OTAUpdateShift)
// Hostname
#define ParamNET_HostName                            (knx.paramData(NET_HostName))
// LAN-Modus
#define ParamNET_LanMode                             ((knx.paramByte(NET_LanMode) & NET_LanModeMask) >> NET_LanModeShift)
// Zeitserver
#define ParamNET_NTPServer                           (knx.paramData(NET_NTPServer))



#define LOG_BuzzerInstalled                     225      // 1 Bit, Bit 7
#define     LOG_BuzzerInstalledMask 0x80
#define     LOG_BuzzerInstalledShift 7
#define LOG_LedInstalled                        225      // 1 Bit, Bit 6
#define     LOG_LedInstalledMask 0x40
#define     LOG_LedInstalledShift 6
#define LOG_VacationKo                          225      // 1 Bit, Bit 5
#define     LOG_VacationKoMask 0x20
#define     LOG_VacationKoShift 5
#define LOG_HolidayKo                           225      // 1 Bit, Bit 4
#define     LOG_HolidayKoMask 0x10
#define     LOG_HolidayKoShift 4
#define LOG_VacationRead                        225      // 1 Bit, Bit 3
#define     LOG_VacationReadMask 0x08
#define     LOG_VacationReadShift 3
#define LOG_HolidaySend                         225      // 1 Bit, Bit 2
#define     LOG_HolidaySendMask 0x04
#define     LOG_HolidaySendShift 2
#define LOG_Neujahr                             226      // 1 Bit, Bit 7
#define     LOG_NeujahrMask 0x80
#define     LOG_NeujahrShift 7
#define LOG_DreiKoenige                         226      // 1 Bit, Bit 6
#define     LOG_DreiKoenigeMask 0x40
#define     LOG_DreiKoenigeShift 6
#define LOG_Weiberfastnacht                     226      // 1 Bit, Bit 5
#define     LOG_WeiberfastnachtMask 0x20
#define     LOG_WeiberfastnachtShift 5
#define LOG_Rosenmontag                         226      // 1 Bit, Bit 4
#define     LOG_RosenmontagMask 0x10
#define     LOG_RosenmontagShift 4
#define LOG_Fastnachtsdienstag                  226      // 1 Bit, Bit 3
#define     LOG_FastnachtsdienstagMask 0x08
#define     LOG_FastnachtsdienstagShift 3
#define LOG_Aschermittwoch                      226      // 1 Bit, Bit 2
#define     LOG_AschermittwochMask 0x04
#define     LOG_AschermittwochShift 2
#define LOG_Frauentag                           226      // 1 Bit, Bit 1
#define     LOG_FrauentagMask 0x02
#define     LOG_FrauentagShift 1
#define LOG_Gruendonnerstag                     226      // 1 Bit, Bit 0
#define     LOG_GruendonnerstagMask 0x01
#define     LOG_GruendonnerstagShift 0
#define LOG_Karfreitag                          227      // 1 Bit, Bit 7
#define     LOG_KarfreitagMask 0x80
#define     LOG_KarfreitagShift 7
#define LOG_Ostersonntag                        227      // 1 Bit, Bit 6
#define     LOG_OstersonntagMask 0x40
#define     LOG_OstersonntagShift 6
#define LOG_Ostermontag                         227      // 1 Bit, Bit 5
#define     LOG_OstermontagMask 0x20
#define     LOG_OstermontagShift 5
#define LOG_TagDerArbeit                        227      // 1 Bit, Bit 4
#define     LOG_TagDerArbeitMask 0x10
#define     LOG_TagDerArbeitShift 4
#define LOG_Himmelfahrt                         227      // 1 Bit, Bit 3
#define     LOG_HimmelfahrtMask 0x08
#define     LOG_HimmelfahrtShift 3
#define LOG_Pfingstsonntag                      227      // 1 Bit, Bit 2
#define     LOG_PfingstsonntagMask 0x04
#define     LOG_PfingstsonntagShift 2
#define LOG_Pfingstmontag                       227      // 1 Bit, Bit 1
#define     LOG_PfingstmontagMask 0x02
#define     LOG_PfingstmontagShift 1
#define LOG_Fronleichnam                        227      // 1 Bit, Bit 0
#define     LOG_FronleichnamMask 0x01
#define     LOG_FronleichnamShift 0
#define LOG_Friedensfest                        228      // 1 Bit, Bit 7
#define     LOG_FriedensfestMask 0x80
#define     LOG_FriedensfestShift 7
#define LOG_MariaHimmelfahrt                    228      // 1 Bit, Bit 6
#define     LOG_MariaHimmelfahrtMask 0x40
#define     LOG_MariaHimmelfahrtShift 6
#define LOG_DeutscheEinheit                     228      // 1 Bit, Bit 5
#define     LOG_DeutscheEinheitMask 0x20
#define     LOG_DeutscheEinheitShift 5
#define LOG_Reformationstag                     228      // 1 Bit, Bit 4
#define     LOG_ReformationstagMask 0x10
#define     LOG_ReformationstagShift 4
#define LOG_Allerheiligen                       228      // 1 Bit, Bit 3
#define     LOG_AllerheiligenMask 0x08
#define     LOG_AllerheiligenShift 3
#define LOG_BussBettag                          228      // 1 Bit, Bit 2
#define     LOG_BussBettagMask 0x04
#define     LOG_BussBettagShift 2
#define LOG_Advent1                             228      // 1 Bit, Bit 1
#define     LOG_Advent1Mask 0x02
#define     LOG_Advent1Shift 1
#define LOG_Advent2                             228      // 1 Bit, Bit 0
#define     LOG_Advent2Mask 0x01
#define     LOG_Advent2Shift 0
#define LOG_Advent3                             229      // 1 Bit, Bit 7
#define     LOG_Advent3Mask 0x80
#define     LOG_Advent3Shift 7
#define LOG_Advent4                             229      // 1 Bit, Bit 6
#define     LOG_Advent4Mask 0x40
#define     LOG_Advent4Shift 6
#define LOG_Heiligabend                         229      // 1 Bit, Bit 5
#define     LOG_HeiligabendMask 0x20
#define     LOG_HeiligabendShift 5
#define LOG_Weihnachtstag1                      229      // 1 Bit, Bit 4
#define     LOG_Weihnachtstag1Mask 0x10
#define     LOG_Weihnachtstag1Shift 4
#define LOG_Weihnachtstag2                      229      // 1 Bit, Bit 3
#define     LOG_Weihnachtstag2Mask 0x08
#define     LOG_Weihnachtstag2Shift 3
#define LOG_Silvester                           229      // 1 Bit, Bit 2
#define     LOG_SilvesterMask 0x04
#define     LOG_SilvesterShift 2
#define LOG_Nationalfeiertag                    229      // 1 Bit, Bit 1
#define     LOG_NationalfeiertagMask 0x02
#define     LOG_NationalfeiertagShift 1
#define LOG_MariaEmpfaengnis                    229      // 1 Bit, Bit 0
#define     LOG_MariaEmpfaengnisMask 0x01
#define     LOG_MariaEmpfaengnisShift 0
#define LOG_NationalfeiertagSchweiz             230      // 1 Bit, Bit 7
#define     LOG_NationalfeiertagSchweizMask 0x80
#define     LOG_NationalfeiertagSchweizShift 7
#define LOG_Totensonntag                        230      // 1 Bit, Bit 6
#define     LOG_TotensonntagMask 0x40
#define     LOG_TotensonntagShift 6
#define LOG_Weltkindertag                       230      // 1 Bit, Bit 5
#define     LOG_WeltkindertagMask 0x20
#define     LOG_WeltkindertagShift 5
#define LOG_BuzzerSilent                        231      // uint16_t
#define LOG_BuzzerNormal                        233      // uint16_t
#define LOG_BuzzerLoud                          235      // uint16_t
#define LOG_VisibleChannels                     237      // uint8_t
#define LOG_LedMapping                          238      // 3 Bits, Bit 7-5
#define     LOG_LedMappingMask 0xE0
#define     LOG_LedMappingShift 5
#define LOG_UserFormula1                        239      // char*, 99 Byte
#define LOG_UserFormula1Active                  338      // 1 Bit, Bit 7
#define     LOG_UserFormula1ActiveMask 0x80
#define     LOG_UserFormula1ActiveShift 7
#define LOG_UserFormula2                        339      // char*, 99 Byte
#define LOG_UserFormula2Active                  438      // 1 Bit, Bit 7
#define     LOG_UserFormula2ActiveMask 0x80
#define     LOG_UserFormula2ActiveShift 7
#define LOG_UserFormula3                        439      // char*, 99 Byte
#define LOG_UserFormula3Active                  538      // 1 Bit, Bit 7
#define     LOG_UserFormula3ActiveMask 0x80
#define     LOG_UserFormula3ActiveShift 7
#define LOG_UserFormula4                        539      // char*, 99 Byte
#define LOG_UserFormula4Active                  638      // 1 Bit, Bit 7
#define     LOG_UserFormula4ActiveMask 0x80
#define     LOG_UserFormula4ActiveShift 7
#define LOG_UserFormula5                        639      // char*, 99 Byte
#define LOG_UserFormula5Active                  738      // 1 Bit, Bit 7
#define     LOG_UserFormula5ActiveMask 0x80
#define     LOG_UserFormula5ActiveShift 7
#define LOG_UserFormula6                        739      // char*, 99 Byte
#define LOG_UserFormula6Active                  838      // 1 Bit, Bit 7
#define     LOG_UserFormula6ActiveMask 0x80
#define     LOG_UserFormula6ActiveShift 7
#define LOG_UserFormula7                        839      // char*, 99 Byte
#define LOG_UserFormula7Active                  938      // 1 Bit, Bit 7
#define     LOG_UserFormula7ActiveMask 0x80
#define     LOG_UserFormula7ActiveShift 7
#define LOG_UserFormula8                        939      // char*, 99 Byte
#define LOG_UserFormula8Active                  1038      // 1 Bit, Bit 7
#define     LOG_UserFormula8ActiveMask 0x80
#define     LOG_UserFormula8ActiveShift 7
#define LOG_UserFormula9                        1039      // char*, 99 Byte
#define LOG_UserFormula9Active                  1138      // 1 Bit, Bit 7
#define     LOG_UserFormula9ActiveMask 0x80
#define     LOG_UserFormula9ActiveShift 7
#define LOG_UserFormula10                       1139      // char*, 99 Byte
#define LOG_UserFormula10Active                 1238      // 1 Bit, Bit 7
#define     LOG_UserFormula10ActiveMask 0x80
#define     LOG_UserFormula10ActiveShift 7
#define LOG_UserFormula11                       1239      // char*, 99 Byte
#define LOG_UserFormula11Active                 1338      // 1 Bit, Bit 7
#define     LOG_UserFormula11ActiveMask 0x80
#define     LOG_UserFormula11ActiveShift 7
#define LOG_UserFormula12                       1339      // char*, 99 Byte
#define LOG_UserFormula12Active                 1438      // 1 Bit, Bit 7
#define     LOG_UserFormula12ActiveMask 0x80
#define     LOG_UserFormula12ActiveShift 7
#define LOG_UserFormula13                       1439      // char*, 99 Byte
#define LOG_UserFormula13Active                 1538      // 1 Bit, Bit 7
#define     LOG_UserFormula13ActiveMask 0x80
#define     LOG_UserFormula13ActiveShift 7
#define LOG_UserFormula14                       1539      // char*, 99 Byte
#define LOG_UserFormula14Active                 1638      // 1 Bit, Bit 7
#define     LOG_UserFormula14ActiveMask 0x80
#define     LOG_UserFormula14ActiveShift 7
#define LOG_UserFormula15                       1639      // char*, 99 Byte
#define LOG_UserFormula15Active                 1738      // 1 Bit, Bit 7
#define     LOG_UserFormula15ActiveMask 0x80
#define     LOG_UserFormula15ActiveShift 7
#define LOG_UserFormula16                       1739      // char*, 99 Byte
#define LOG_UserFormula16Active                 1838      // 1 Bit, Bit 7
#define     LOG_UserFormula16ActiveMask 0x80
#define     LOG_UserFormula16ActiveShift 7
#define LOG_UserFormula17                       1839      // char*, 99 Byte
#define LOG_UserFormula17Active                 1938      // 1 Bit, Bit 7
#define     LOG_UserFormula17ActiveMask 0x80
#define     LOG_UserFormula17ActiveShift 7
#define LOG_UserFormula18                       1939      // char*, 99 Byte
#define LOG_UserFormula18Active                 2038      // 1 Bit, Bit 7
#define     LOG_UserFormula18ActiveMask 0x80
#define     LOG_UserFormula18ActiveShift 7
#define LOG_UserFormula19                       2039      // char*, 99 Byte
#define LOG_UserFormula19Active                 2138      // 1 Bit, Bit 7
#define     LOG_UserFormula19ActiveMask 0x80
#define     LOG_UserFormula19ActiveShift 7
#define LOG_UserFormula20                       2139      // char*, 99 Byte
#define LOG_UserFormula20Active                 2238      // 1 Bit, Bit 7
#define     LOG_UserFormula20ActiveMask 0x80
#define     LOG_UserFormula20ActiveShift 7
#define LOG_UserFormula21                       2239      // char*, 99 Byte
#define LOG_UserFormula21Active                 2338      // 1 Bit, Bit 7
#define     LOG_UserFormula21ActiveMask 0x80
#define     LOG_UserFormula21ActiveShift 7
#define LOG_UserFormula22                       2339      // char*, 99 Byte
#define LOG_UserFormula22Active                 2438      // 1 Bit, Bit 7
#define     LOG_UserFormula22ActiveMask 0x80
#define     LOG_UserFormula22ActiveShift 7
#define LOG_UserFormula23                       2439      // char*, 99 Byte
#define LOG_UserFormula23Active                 2538      // 1 Bit, Bit 7
#define     LOG_UserFormula23ActiveMask 0x80
#define     LOG_UserFormula23ActiveShift 7
#define LOG_UserFormula24                       2539      // char*, 99 Byte
#define LOG_UserFormula24Active                 2638      // 1 Bit, Bit 7
#define     LOG_UserFormula24ActiveMask 0x80
#define     LOG_UserFormula24ActiveShift 7
#define LOG_UserFormula25                       2639      // char*, 99 Byte
#define LOG_UserFormula25Active                 2738      // 1 Bit, Bit 7
#define     LOG_UserFormula25ActiveMask 0x80
#define     LOG_UserFormula25ActiveShift 7
#define LOG_UserFormula26                       2739      // char*, 99 Byte
#define LOG_UserFormula26Active                 2838      // 1 Bit, Bit 7
#define     LOG_UserFormula26ActiveMask 0x80
#define     LOG_UserFormula26ActiveShift 7
#define LOG_UserFormula27                       2839      // char*, 99 Byte
#define LOG_UserFormula27Active                 2938      // 1 Bit, Bit 7
#define     LOG_UserFormula27ActiveMask 0x80
#define     LOG_UserFormula27ActiveShift 7
#define LOG_UserFormula28                       2939      // char*, 99 Byte
#define LOG_UserFormula28Active                 3038      // 1 Bit, Bit 7
#define     LOG_UserFormula28ActiveMask 0x80
#define     LOG_UserFormula28ActiveShift 7
#define LOG_UserFormula29                       3039      // char*, 99 Byte
#define LOG_UserFormula29Active                 3138      // 1 Bit, Bit 7
#define     LOG_UserFormula29ActiveMask 0x80
#define     LOG_UserFormula29ActiveShift 7
#define LOG_UserFormula30                       3139      // char*, 99 Byte
#define LOG_UserFormula30Active                 3238      // 1 Bit, Bit 7
#define     LOG_UserFormula30ActiveMask 0x80
#define     LOG_UserFormula30ActiveShift 7

// Akustischer Signalgeber vorhanden (Buzzer)?
#define ParamLOG_BuzzerInstalled                     ((bool)(knx.paramByte(LOG_BuzzerInstalled) & LOG_BuzzerInstalledMask))
// Optischer Signalgeber vorhanden (RGB-LED)?
#define ParamLOG_LedInstalled                        ((bool)(knx.paramByte(LOG_LedInstalled) & LOG_LedInstalledMask))
// Urlaubsbehandlung aktivieren?
#define ParamLOG_VacationKo                          ((bool)(knx.paramByte(LOG_VacationKo) & LOG_VacationKoMask))
// Feiertage auf dem Bus verfügbar machen?
#define ParamLOG_HolidayKo                           ((bool)(knx.paramByte(LOG_HolidayKo) & LOG_HolidayKoMask))
// Nach Neustart Urlaubsinfo lesen?
#define ParamLOG_VacationRead                        ((bool)(knx.paramByte(LOG_VacationRead) & LOG_VacationReadMask))
// Nach Neuberechnung Feiertagsinfo senden?
#define ParamLOG_HolidaySend                         ((bool)(knx.paramByte(LOG_HolidaySend) & LOG_HolidaySendMask))
// 1. Neujahr
#define ParamLOG_Neujahr                             ((bool)(knx.paramByte(LOG_Neujahr) & LOG_NeujahrMask))
// 2. Heilige Drei Könige
#define ParamLOG_DreiKoenige                         ((bool)(knx.paramByte(LOG_DreiKoenige) & LOG_DreiKoenigeMask))
// 3. Weiberfastnacht
#define ParamLOG_Weiberfastnacht                     ((bool)(knx.paramByte(LOG_Weiberfastnacht) & LOG_WeiberfastnachtMask))
// 4. Rosenmontag
#define ParamLOG_Rosenmontag                         ((bool)(knx.paramByte(LOG_Rosenmontag) & LOG_RosenmontagMask))
// 5. Fastnachtsdienstag
#define ParamLOG_Fastnachtsdienstag                  ((bool)(knx.paramByte(LOG_Fastnachtsdienstag) & LOG_FastnachtsdienstagMask))
// 6. Aschermittwoch
#define ParamLOG_Aschermittwoch                      ((bool)(knx.paramByte(LOG_Aschermittwoch) & LOG_AschermittwochMask))
// 7. Frauentag
#define ParamLOG_Frauentag                           ((bool)(knx.paramByte(LOG_Frauentag) & LOG_FrauentagMask))
// 8. Gründonnerstag
#define ParamLOG_Gruendonnerstag                     ((bool)(knx.paramByte(LOG_Gruendonnerstag) & LOG_GruendonnerstagMask))
// 9. Karfreitag
#define ParamLOG_Karfreitag                          ((bool)(knx.paramByte(LOG_Karfreitag) & LOG_KarfreitagMask))
// 10. Ostersonntag
#define ParamLOG_Ostersonntag                        ((bool)(knx.paramByte(LOG_Ostersonntag) & LOG_OstersonntagMask))
// 11. Ostermontag
#define ParamLOG_Ostermontag                         ((bool)(knx.paramByte(LOG_Ostermontag) & LOG_OstermontagMask))
// 12. Tag der Arbeit
#define ParamLOG_TagDerArbeit                        ((bool)(knx.paramByte(LOG_TagDerArbeit) & LOG_TagDerArbeitMask))
// 13. Christi Himmelfahrt
#define ParamLOG_Himmelfahrt                         ((bool)(knx.paramByte(LOG_Himmelfahrt) & LOG_HimmelfahrtMask))
// 14. Pfingstsonntag
#define ParamLOG_Pfingstsonntag                      ((bool)(knx.paramByte(LOG_Pfingstsonntag) & LOG_PfingstsonntagMask))
// 15. Pfingstmontag
#define ParamLOG_Pfingstmontag                       ((bool)(knx.paramByte(LOG_Pfingstmontag) & LOG_PfingstmontagMask))
// 16. Fronleichnam
#define ParamLOG_Fronleichnam                        ((bool)(knx.paramByte(LOG_Fronleichnam) & LOG_FronleichnamMask))
// 17. Hohes Friedensfest
#define ParamLOG_Friedensfest                        ((bool)(knx.paramByte(LOG_Friedensfest) & LOG_FriedensfestMask))
// 18. Mariä Himmelfahrt
#define ParamLOG_MariaHimmelfahrt                    ((bool)(knx.paramByte(LOG_MariaHimmelfahrt) & LOG_MariaHimmelfahrtMask))
// 19. Tag der Deutschen Einheit
#define ParamLOG_DeutscheEinheit                     ((bool)(knx.paramByte(LOG_DeutscheEinheit) & LOG_DeutscheEinheitMask))
// 20. Reformationstag
#define ParamLOG_Reformationstag                     ((bool)(knx.paramByte(LOG_Reformationstag) & LOG_ReformationstagMask))
// 21. Allerheiligen
#define ParamLOG_Allerheiligen                       ((bool)(knx.paramByte(LOG_Allerheiligen) & LOG_AllerheiligenMask))
// 22. Buß- und Bettag
#define ParamLOG_BussBettag                          ((bool)(knx.paramByte(LOG_BussBettag) & LOG_BussBettagMask))
// 23. Erster Advent
#define ParamLOG_Advent1                             ((bool)(knx.paramByte(LOG_Advent1) & LOG_Advent1Mask))
// 24. Zweiter Advent
#define ParamLOG_Advent2                             ((bool)(knx.paramByte(LOG_Advent2) & LOG_Advent2Mask))
// 25. Dritter Advent
#define ParamLOG_Advent3                             ((bool)(knx.paramByte(LOG_Advent3) & LOG_Advent3Mask))
// 26. Vierter Advent
#define ParamLOG_Advent4                             ((bool)(knx.paramByte(LOG_Advent4) & LOG_Advent4Mask))
// 27. Heiligabend
#define ParamLOG_Heiligabend                         ((bool)(knx.paramByte(LOG_Heiligabend) & LOG_HeiligabendMask))
// 28. Erster Weihnachtstag
#define ParamLOG_Weihnachtstag1                      ((bool)(knx.paramByte(LOG_Weihnachtstag1) & LOG_Weihnachtstag1Mask))
// 29. Zweiter Weihnachtstag
#define ParamLOG_Weihnachtstag2                      ((bool)(knx.paramByte(LOG_Weihnachtstag2) & LOG_Weihnachtstag2Mask))
// 30. Silvester
#define ParamLOG_Silvester                           ((bool)(knx.paramByte(LOG_Silvester) & LOG_SilvesterMask))
// 31. Nationalfeiertag (AT)
#define ParamLOG_Nationalfeiertag                    ((bool)(knx.paramByte(LOG_Nationalfeiertag) & LOG_NationalfeiertagMask))
// 32. Maria Empfängnis (AT)
#define ParamLOG_MariaEmpfaengnis                    ((bool)(knx.paramByte(LOG_MariaEmpfaengnis) & LOG_MariaEmpfaengnisMask))
// 33. Nationalfeiertag (CH)
#define ParamLOG_NationalfeiertagSchweiz             ((bool)(knx.paramByte(LOG_NationalfeiertagSchweiz) & LOG_NationalfeiertagSchweizMask))
// 34. Totensonntag
#define ParamLOG_Totensonntag                        ((bool)(knx.paramByte(LOG_Totensonntag) & LOG_TotensonntagMask))
// 35. Weltkindertag
#define ParamLOG_Weltkindertag                       ((bool)(knx.paramByte(LOG_Weltkindertag) & LOG_WeltkindertagMask))
// Frequenz für Buzzer (leise)
#define ParamLOG_BuzzerSilent                        (knx.paramWord(LOG_BuzzerSilent))
// Frequenz für Buzzer (normal)
#define ParamLOG_BuzzerNormal                        (knx.paramWord(LOG_BuzzerNormal))
// Frequenz für Buzzer (laut)
#define ParamLOG_BuzzerLoud                          (knx.paramWord(LOG_BuzzerLoud))
// Verfügbare Kanäle
#define ParamLOG_VisibleChannels                     (knx.paramByte(LOG_VisibleChannels))
// Lötpad A / B / C entspricht
#define ParamLOG_LedMapping                          ((knx.paramByte(LOG_LedMapping) & LOG_LedMappingMask) >> LOG_LedMappingShift)
// Formeldefinition
#define ParamLOG_UserFormula1                        (knx.paramData(LOG_UserFormula1))
// Benutzerformel 1 aktiv
#define ParamLOG_UserFormula1Active                  ((bool)(knx.paramByte(LOG_UserFormula1Active) & LOG_UserFormula1ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula2                        (knx.paramData(LOG_UserFormula2))
// Benutzerformel 2 aktiv
#define ParamLOG_UserFormula2Active                  ((bool)(knx.paramByte(LOG_UserFormula2Active) & LOG_UserFormula2ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula3                        (knx.paramData(LOG_UserFormula3))
// Benutzerformel 3 aktiv
#define ParamLOG_UserFormula3Active                  ((bool)(knx.paramByte(LOG_UserFormula3Active) & LOG_UserFormula3ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula4                        (knx.paramData(LOG_UserFormula4))
// Benutzerformel 4 aktiv
#define ParamLOG_UserFormula4Active                  ((bool)(knx.paramByte(LOG_UserFormula4Active) & LOG_UserFormula4ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula5                        (knx.paramData(LOG_UserFormula5))
// Benutzerformel 5 aktiv
#define ParamLOG_UserFormula5Active                  ((bool)(knx.paramByte(LOG_UserFormula5Active) & LOG_UserFormula5ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula6                        (knx.paramData(LOG_UserFormula6))
// Benutzerformel 6 aktiv
#define ParamLOG_UserFormula6Active                  ((bool)(knx.paramByte(LOG_UserFormula6Active) & LOG_UserFormula6ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula7                        (knx.paramData(LOG_UserFormula7))
// Benutzerformel 7 aktiv
#define ParamLOG_UserFormula7Active                  ((bool)(knx.paramByte(LOG_UserFormula7Active) & LOG_UserFormula7ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula8                        (knx.paramData(LOG_UserFormula8))
// Benutzerformel 8 aktiv
#define ParamLOG_UserFormula8Active                  ((bool)(knx.paramByte(LOG_UserFormula8Active) & LOG_UserFormula8ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula9                        (knx.paramData(LOG_UserFormula9))
// Benutzerformel 9 aktiv
#define ParamLOG_UserFormula9Active                  ((bool)(knx.paramByte(LOG_UserFormula9Active) & LOG_UserFormula9ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula10                       (knx.paramData(LOG_UserFormula10))
// Benutzerformel 10 aktiv
#define ParamLOG_UserFormula10Active                 ((bool)(knx.paramByte(LOG_UserFormula10Active) & LOG_UserFormula10ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula11                       (knx.paramData(LOG_UserFormula11))
// Benutzerformel 11 aktiv
#define ParamLOG_UserFormula11Active                 ((bool)(knx.paramByte(LOG_UserFormula11Active) & LOG_UserFormula11ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula12                       (knx.paramData(LOG_UserFormula12))
// Benutzerformel 12 aktiv
#define ParamLOG_UserFormula12Active                 ((bool)(knx.paramByte(LOG_UserFormula12Active) & LOG_UserFormula12ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula13                       (knx.paramData(LOG_UserFormula13))
// Benutzerformel 13 aktiv
#define ParamLOG_UserFormula13Active                 ((bool)(knx.paramByte(LOG_UserFormula13Active) & LOG_UserFormula13ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula14                       (knx.paramData(LOG_UserFormula14))
// Benutzerformel 14 aktiv
#define ParamLOG_UserFormula14Active                 ((bool)(knx.paramByte(LOG_UserFormula14Active) & LOG_UserFormula14ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula15                       (knx.paramData(LOG_UserFormula15))
// Benutzerformel 15 aktiv
#define ParamLOG_UserFormula15Active                 ((bool)(knx.paramByte(LOG_UserFormula15Active) & LOG_UserFormula15ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula16                       (knx.paramData(LOG_UserFormula16))
// Benutzerformel 16 aktiv
#define ParamLOG_UserFormula16Active                 ((bool)(knx.paramByte(LOG_UserFormula16Active) & LOG_UserFormula16ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula17                       (knx.paramData(LOG_UserFormula17))
// Benutzerformel 17 aktiv
#define ParamLOG_UserFormula17Active                 ((bool)(knx.paramByte(LOG_UserFormula17Active) & LOG_UserFormula17ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula18                       (knx.paramData(LOG_UserFormula18))
// Benutzerformel 18 aktiv
#define ParamLOG_UserFormula18Active                 ((bool)(knx.paramByte(LOG_UserFormula18Active) & LOG_UserFormula18ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula19                       (knx.paramData(LOG_UserFormula19))
// Benutzerformel 19 aktiv
#define ParamLOG_UserFormula19Active                 ((bool)(knx.paramByte(LOG_UserFormula19Active) & LOG_UserFormula19ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula20                       (knx.paramData(LOG_UserFormula20))
// Benutzerformel 20 aktiv
#define ParamLOG_UserFormula20Active                 ((bool)(knx.paramByte(LOG_UserFormula20Active) & LOG_UserFormula20ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula21                       (knx.paramData(LOG_UserFormula21))
// Benutzerformel 21 aktiv
#define ParamLOG_UserFormula21Active                 ((bool)(knx.paramByte(LOG_UserFormula21Active) & LOG_UserFormula21ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula22                       (knx.paramData(LOG_UserFormula22))
// Benutzerformel 22 aktiv
#define ParamLOG_UserFormula22Active                 ((bool)(knx.paramByte(LOG_UserFormula22Active) & LOG_UserFormula22ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula23                       (knx.paramData(LOG_UserFormula23))
// Benutzerformel 23 aktiv
#define ParamLOG_UserFormula23Active                 ((bool)(knx.paramByte(LOG_UserFormula23Active) & LOG_UserFormula23ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula24                       (knx.paramData(LOG_UserFormula24))
// Benutzerformel 24 aktiv
#define ParamLOG_UserFormula24Active                 ((bool)(knx.paramByte(LOG_UserFormula24Active) & LOG_UserFormula24ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula25                       (knx.paramData(LOG_UserFormula25))
// Benutzerformel 25 aktiv
#define ParamLOG_UserFormula25Active                 ((bool)(knx.paramByte(LOG_UserFormula25Active) & LOG_UserFormula25ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula26                       (knx.paramData(LOG_UserFormula26))
// Benutzerformel 26 aktiv
#define ParamLOG_UserFormula26Active                 ((bool)(knx.paramByte(LOG_UserFormula26Active) & LOG_UserFormula26ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula27                       (knx.paramData(LOG_UserFormula27))
// Benutzerformel 27 aktiv
#define ParamLOG_UserFormula27Active                 ((bool)(knx.paramByte(LOG_UserFormula27Active) & LOG_UserFormula27ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula28                       (knx.paramData(LOG_UserFormula28))
// Benutzerformel 28 aktiv
#define ParamLOG_UserFormula28Active                 ((bool)(knx.paramByte(LOG_UserFormula28Active) & LOG_UserFormula28ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula29                       (knx.paramData(LOG_UserFormula29))
// Benutzerformel 29 aktiv
#define ParamLOG_UserFormula29Active                 ((bool)(knx.paramByte(LOG_UserFormula29Active) & LOG_UserFormula29ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula30                       (knx.paramData(LOG_UserFormula30))
// Benutzerformel 30 aktiv
#define ParamLOG_UserFormula30Active                 ((bool)(knx.paramByte(LOG_UserFormula30Active) & LOG_UserFormula30ActiveMask))

#define LOG_KoVacation 15
#define LOG_KoHoliday1 16
#define LOG_KoHoliday2 17
#define LOG_KoLedLock 18
#define LOG_KoBuzzerLock 19

// Urlaub
#define KoLOG_Vacation                            (knx.getGroupObject(LOG_KoVacation))
// Welcher Feiertag ist heute?
#define KoLOG_Holiday1                            (knx.getGroupObject(LOG_KoHoliday1))
// Welcher Feiertag ist morgen?
#define KoLOG_Holiday2                            (knx.getGroupObject(LOG_KoHoliday2))
// LED sperren
#define KoLOG_LedLock                             (knx.getGroupObject(LOG_KoLedLock))
// Buzzer sperren
#define KoLOG_BuzzerLock                          (knx.getGroupObject(LOG_KoBuzzerLock))

#define LOG_ChannelCount 20

// Parameter per channel
#define LOG_ParamBlockOffset 3239
#define LOG_ParamBlockSize 85
#define LOG_ParamCalcIndex(index) (index + LOG_ParamBlockOffset + _channelIndex * LOG_ParamBlockSize)

#define LOG_fChannelDelayBase                    0      // 2 Bits, Bit 7-6
#define     LOG_fChannelDelayBaseMask 0xC0
#define     LOG_fChannelDelayBaseShift 6
#define LOG_fChannelDelayTime                    0      // 14 Bits, Bit 13-0
#define     LOG_fChannelDelayTimeMask 0x3FFF
#define     LOG_fChannelDelayTimeShift 0
#define LOG_fLogic                               2      // 8 Bits, Bit 7-0
#define LOG_fCalculate                           3      // 2 Bits, Bit 1-0
#define     LOG_fCalculateMask 0x03
#define     LOG_fCalculateShift 0
#define LOG_fDisable                             3      // 1 Bit, Bit 2
#define     LOG_fDisableMask 0x04
#define     LOG_fDisableShift 2
#define LOG_fAlarm                               3      // 1 Bit, Bit 3
#define     LOG_fAlarmMask 0x08
#define     LOG_fAlarmShift 3
#define LOG_fTGate                               3      // 1 Bit, Bit 4
#define     LOG_fTGateMask 0x10
#define     LOG_fTGateShift 4
#define LOG_fOInternalOn                         3      // 1 Bit, Bit 5
#define     LOG_fOInternalOnMask 0x20
#define     LOG_fOInternalOnShift 5
#define LOG_fOInternalOff                        3      // 1 Bit, Bit 6
#define     LOG_fOInternalOffMask 0x40
#define     LOG_fOInternalOffShift 6
#define LOG_fTrigger                             4      // 8 Bits, Bit 7-0
#define LOG_fTriggerE1                           4      // 1 Bit, Bit 0
#define     LOG_fTriggerE1Mask 0x01
#define     LOG_fTriggerE1Shift 0
#define LOG_fTriggerE2                           4      // 1 Bit, Bit 1
#define     LOG_fTriggerE2Mask 0x02
#define     LOG_fTriggerE2Shift 1
#define LOG_fTriggerI1                           4      // 1 Bit, Bit 2
#define     LOG_fTriggerI1Mask 0x04
#define     LOG_fTriggerI1Shift 2
#define LOG_fTriggerI2                           4      // 1 Bit, Bit 3
#define     LOG_fTriggerI2Mask 0x08
#define     LOG_fTriggerI2Shift 3
#define LOG_fTriggerTime                         4      // 8 Bits, Bit 7-0
#define LOG_fTriggerGateClose                    5      // 2 Bits, Bit 7-6
#define     LOG_fTriggerGateCloseMask 0xC0
#define     LOG_fTriggerGateCloseShift 6
#define LOG_fTriggerGateOpen                     5      // 2 Bits, Bit 5-4
#define     LOG_fTriggerGateOpenMask 0x30
#define     LOG_fTriggerGateOpenShift 4
#define LOG_fE1ConvertInt                        6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertIntMask 0xF0
#define     LOG_fE1ConvertIntShift 4
#define LOG_fE1Convert                           6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertMask 0xF0
#define     LOG_fE1ConvertShift 4
#define LOG_fE1ConvertFloat                      6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertFloatMask 0xF0
#define     LOG_fE1ConvertFloatShift 4
#define LOG_fE1ConvertSpecial                    6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertSpecialMask 0xF0
#define     LOG_fE1ConvertSpecialShift 4
#define LOG_fE1ConvertBool                       6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertBoolMask 0xF0
#define     LOG_fE1ConvertBoolShift 4
#define LOG_fE1                                  6      // 2 Bits, Bit 1-0
#define     LOG_fE1Mask 0x03
#define     LOG_fE1Shift 0
#define LOG_fE1Dpt                               7      // 8 Bits, Bit 7-0
#define LOG_fE1RepeatBase                        8      // 2 Bits, Bit 7-6
#define     LOG_fE1RepeatBaseMask 0xC0
#define     LOG_fE1RepeatBaseShift 6
#define LOG_fE1RepeatTime                        8      // 14 Bits, Bit 13-0
#define     LOG_fE1RepeatTimeMask 0x3FFF
#define     LOG_fE1RepeatTimeShift 0
#define LOG_fE1OtherKO                          10      // uint16_t
#define LOG_fE1OtherKORel                       10      // int16_t
#define LOG_fE1Default                          12      // 2 Bits, Bit 1-0
#define     LOG_fE1DefaultMask 0x03
#define     LOG_fE1DefaultShift 0
#define LOG_fE1DefaultExt                       12      // 2 Bits, Bit 1-0
#define     LOG_fE1DefaultExtMask 0x03
#define     LOG_fE1DefaultExtShift 0
#define LOG_fE1DefaultEEPROM                    12      // 1 Bit, Bit 2
#define     LOG_fE1DefaultEEPROMMask 0x04
#define     LOG_fE1DefaultEEPROMShift 2
#define LOG_fE1DefaultRepeat                    12      // 1 Bit, Bit 3
#define     LOG_fE1DefaultRepeatMask 0x08
#define     LOG_fE1DefaultRepeatShift 3
#define LOG_fE1UseOtherKO                       12      // 2 Bits, Bit 5-4
#define     LOG_fE1UseOtherKOMask 0x30
#define     LOG_fE1UseOtherKOShift 4
#define LOG_fE1LowDelta                         13      // int32_t
#define LOG_fE1HighDelta                        17      // int32_t
#define LOG_fE1LowDeltaFloat                    13      // float
#define LOG_fE1HighDeltaFloat                   17      // float
#define LOG_fE1LowDeltaDouble                   13      // float
#define LOG_fE1HighDeltaDouble                  17      // float
#define LOG_fE1Low0Valid                        20      // 1 Bit, Bit 7
#define     LOG_fE1Low0ValidMask 0x80
#define     LOG_fE1Low0ValidShift 7
#define LOG_fE1Low1Valid                        20      // 1 Bit, Bit 6
#define     LOG_fE1Low1ValidMask 0x40
#define     LOG_fE1Low1ValidShift 6
#define LOG_fE1Low2Valid                        20      // 1 Bit, Bit 5
#define     LOG_fE1Low2ValidMask 0x20
#define     LOG_fE1Low2ValidShift 5
#define LOG_fE1Low3Valid                        20      // 1 Bit, Bit 4
#define     LOG_fE1Low3ValidMask 0x10
#define     LOG_fE1Low3ValidShift 4
#define LOG_fE1Low4Valid                        20      // 1 Bit, Bit 3
#define     LOG_fE1Low4ValidMask 0x08
#define     LOG_fE1Low4ValidShift 3
#define LOG_fE1Low5Valid                        20      // 1 Bit, Bit 2
#define     LOG_fE1Low5ValidMask 0x04
#define     LOG_fE1Low5ValidShift 2
#define LOG_fE1Low6Valid                        20      // 1 Bit, Bit 1
#define     LOG_fE1Low6ValidMask 0x02
#define     LOG_fE1Low6ValidShift 1
#define LOG_fE1Low0Dpt2                         13      // 8 Bits, Bit 7-0
#define LOG_fE1Low1Dpt2                         14      // 8 Bits, Bit 7-0
#define LOG_fE1Low2Dpt2                         15      // 8 Bits, Bit 7-0
#define LOG_fE1Low3Dpt2                         16      // 8 Bits, Bit 7-0
#define LOG_fE1LowDpt2Fix                       13      // 8 Bits, Bit 7-0
#define LOG_fE1Low0Dpt3Dir                      13      // 5 Bits, Bit 7-3
#define     LOG_fE1Low0Dpt3DirMask 0xF8
#define     LOG_fE1Low0Dpt3DirShift 3
#define LOG_fE1Low0Dpt3Dim                      13      // 3 Bits, Bit 2-0
#define     LOG_fE1Low0Dpt3DimMask 0x07
#define     LOG_fE1Low0Dpt3DimShift 0
#define LOG_fE1Low1Dpt3Dir                      14      // 5 Bits, Bit 7-3
#define     LOG_fE1Low1Dpt3DirMask 0xF8
#define     LOG_fE1Low1Dpt3DirShift 3
#define LOG_fE1Low1Dpt3Dim                      14      // 3 Bits, Bit 2-0
#define     LOG_fE1Low1Dpt3DimMask 0x07
#define     LOG_fE1Low1Dpt3DimShift 0
#define LOG_fE1Low2Dpt3Dir                      15      // 5 Bits, Bit 7-3
#define     LOG_fE1Low2Dpt3DirMask 0xF8
#define     LOG_fE1Low2Dpt3DirShift 3
#define LOG_fE1Low2Dpt3Dim                      15      // 3 Bits, Bit 2-0
#define     LOG_fE1Low2Dpt3DimMask 0x07
#define     LOG_fE1Low2Dpt3DimShift 0
#define LOG_fE1Low3Dpt3Dir                      16      // 5 Bits, Bit 7-3
#define     LOG_fE1Low3Dpt3DirMask 0xF8
#define     LOG_fE1Low3Dpt3DirShift 3
#define LOG_fE1Low3Dpt3Dim                      16      // 3 Bits, Bit 2-0
#define     LOG_fE1Low3Dpt3DimMask 0x07
#define     LOG_fE1Low3Dpt3DimShift 0
#define LOG_fE1LowDpt3FixDir                    13      // 5 Bits, Bit 7-3
#define     LOG_fE1LowDpt3FixDirMask 0xF8
#define     LOG_fE1LowDpt3FixDirShift 3
#define LOG_fE1LowDpt3FixDim                    13      // 3 Bits, Bit 2-0
#define     LOG_fE1LowDpt3FixDimMask 0x07
#define     LOG_fE1LowDpt3FixDimShift 0
#define LOG_fE1LowDpt5                          13      // uint8_t
#define LOG_fE1HighDpt5                         17      // uint8_t
#define LOG_fE1Low0Dpt5In                       13      // uint8_t
#define LOG_fE1Low1Dpt5In                       14      // uint8_t
#define LOG_fE1Low2Dpt5In                       15      // uint8_t
#define LOG_fE1Low3Dpt5In                       16      // uint8_t
#define LOG_fE1Low4Dpt5In                       17      // uint8_t
#define LOG_fE1Low5Dpt5In                       18      // uint8_t
#define LOG_fE1Low6Dpt5In                       19      // uint8_t
#define LOG_fE1LowDpt5Fix                       13      // uint8_t
#define LOG_fE1LowDpt5001                       13      // uint8_t
#define LOG_fE1HighDpt5001                      17      // uint8_t
#define LOG_fE1Low0Dpt5xIn                      13      // uint8_t
#define LOG_fE1Low1Dpt5xIn                      14      // uint8_t
#define LOG_fE1Low2Dpt5xIn                      15      // uint8_t
#define LOG_fE1Low3Dpt5xIn                      16      // uint8_t
#define LOG_fE1Low4Dpt5xIn                      17      // uint8_t
#define LOG_fE1Low5Dpt5xIn                      18      // uint8_t
#define LOG_fE1Low6Dpt5xIn                      19      // uint8_t
#define LOG_fE1LowDpt5xFix                      13      // uint8_t
#define LOG_fE1LowDpt6                          13      // int8_t
#define LOG_fE1HighDpt6                         17      // int8_t
#define LOG_fE1Low0Dpt6In                       13      // int8_t
#define LOG_fE1Low1Dpt6In                       14      // int8_t
#define LOG_fE1Low2Dpt6In                       15      // int8_t
#define LOG_fE1Low3Dpt6In                       16      // int8_t
#define LOG_fE1Low4Dpt6In                       17      // int8_t
#define LOG_fE1Low5Dpt6In                       18      // int8_t
#define LOG_fE1Low6Dpt6In                       19      // int8_t
#define LOG_fE1LowDpt6Fix                       13      // int8_t
#define LOG_fE1LowDpt7                          13      // uint16_t
#define LOG_fE1HighDpt7                         17      // uint16_t
#define LOG_fE1Low0Dpt7In                       13      // uint16_t
#define LOG_fE1Low1Dpt7In                       15      // uint16_t
#define LOG_fE1Low2Dpt7In                       17      // uint16_t
#define LOG_fE1LowDpt7Fix                       13      // uint16_t
#define LOG_fE1LowDpt8                          13      // int16_t
#define LOG_fE1HighDpt8                         17      // int16_t
#define LOG_fE1Low0Dpt8In                       13      // int16_t
#define LOG_fE1Low1Dpt8In                       15      // int16_t
#define LOG_fE1Low2Dpt8In                       17      // int16_t
#define LOG_fE1LowDpt8Fix                       13      // int16_t
#define LOG_fE1LowDpt9                          13      // float
#define LOG_fE1HighDpt9                         17      // float
#define LOG_fE1LowDpt9Fix                       13      // float
#define LOG_fE1LowDpt12                         13      // uint32_t
#define LOG_fE1HighDpt12                        17      // uint32_t
#define LOG_fE1LowDpt12Fix                      13      // uint32_t
#define LOG_fE1LowDpt13                         13      // int32_t
#define LOG_fE1HighDpt13                        17      // int32_t
#define LOG_fE1LowDpt13Fix                      13      // int32_t
#define LOG_fE1LowDpt14                         13      // float
#define LOG_fE1HighDpt14                        17      // float
#define LOG_fE1LowDpt14Fix                      13      // float
#define LOG_fE1Low0Dpt17                        13      // 8 Bits, Bit 7-0
#define LOG_fE1Low1Dpt17                        14      // 8 Bits, Bit 7-0
#define LOG_fE1Low2Dpt17                        15      // 8 Bits, Bit 7-0
#define LOG_fE1Low3Dpt17                        16      // 8 Bits, Bit 7-0
#define LOG_fE1Low4Dpt17                        17      // 8 Bits, Bit 7-0
#define LOG_fE1Low5Dpt17                        18      // 8 Bits, Bit 7-0
#define LOG_fE1Low6Dpt17                        19      // 8 Bits, Bit 7-0
#define LOG_fE1Low7Dpt17                        20      // 8 Bits, Bit 7-0
#define LOG_fE1LowDpt17Fix                      13      // 8 Bits, Bit 7-0
#define LOG_fE1LowDptRGB                        13      // int32_t
#define LOG_fE1HighDptRGB                       17      // int32_t
#define LOG_fE1LowDptRGBFix                     13      // int32_t
#define LOG_fE2ConvertInt                       21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertIntMask 0xF0
#define     LOG_fE2ConvertIntShift 4
#define LOG_fE2Convert                          21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertMask 0xF0
#define     LOG_fE2ConvertShift 4
#define LOG_fE2ConvertFloat                     21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertFloatMask 0xF0
#define     LOG_fE2ConvertFloatShift 4
#define LOG_fE2ConvertSpecial                   21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertSpecialMask 0xF0
#define     LOG_fE2ConvertSpecialShift 4
#define LOG_fE2ConvertBool                      21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertBoolMask 0xF0
#define     LOG_fE2ConvertBoolShift 4
#define LOG_fE2                                 21      // 2 Bits, Bit 1-0
#define     LOG_fE2Mask 0x03
#define     LOG_fE2Shift 0
#define LOG_fE2Dpt                              22      // 8 Bits, Bit 7-0
#define LOG_fE2RepeatBase                       23      // 2 Bits, Bit 7-6
#define     LOG_fE2RepeatBaseMask 0xC0
#define     LOG_fE2RepeatBaseShift 6
#define LOG_fE2RepeatTime                       23      // 14 Bits, Bit 13-0
#define     LOG_fE2RepeatTimeMask 0x3FFF
#define     LOG_fE2RepeatTimeShift 0
#define LOG_fE2OtherKO                          25      // uint16_t
#define LOG_fE2OtherKORel                       25      // int16_t
#define LOG_fE2Default                          27      // 2 Bits, Bit 1-0
#define     LOG_fE2DefaultMask 0x03
#define     LOG_fE2DefaultShift 0
#define LOG_fE2DefaultExt                       27      // 2 Bits, Bit 1-0
#define     LOG_fE2DefaultExtMask 0x03
#define     LOG_fE2DefaultExtShift 0
#define LOG_fE2DefaultEEPROM                    27      // 1 Bit, Bit 2
#define     LOG_fE2DefaultEEPROMMask 0x04
#define     LOG_fE2DefaultEEPROMShift 2
#define LOG_fE2DefaultRepeat                    27      // 1 Bit, Bit 3
#define     LOG_fE2DefaultRepeatMask 0x08
#define     LOG_fE2DefaultRepeatShift 3
#define LOG_fE2UseOtherKO                       27      // 2 Bits, Bit 5-4
#define     LOG_fE2UseOtherKOMask 0x30
#define     LOG_fE2UseOtherKOShift 4
#define LOG_fE2LowDelta                         28      // int32_t
#define LOG_fE2HighDelta                        32      // int32_t
#define LOG_fE2LowDeltaFloat                    28      // float
#define LOG_fE2HighDeltaFloat                   32      // float
#define LOG_fE2LowDeltaDouble                   28      // float
#define LOG_fE2HighDeltaDouble                  32      // float
#define LOG_fE2Low0Valid                        35      // 1 Bit, Bit 7
#define     LOG_fE2Low0ValidMask 0x80
#define     LOG_fE2Low0ValidShift 7
#define LOG_fE2Low1Valid                        35      // 1 Bit, Bit 6
#define     LOG_fE2Low1ValidMask 0x40
#define     LOG_fE2Low1ValidShift 6
#define LOG_fE2Low2Valid                        35      // 1 Bit, Bit 5
#define     LOG_fE2Low2ValidMask 0x20
#define     LOG_fE2Low2ValidShift 5
#define LOG_fE2Low3Valid                        35      // 1 Bit, Bit 4
#define     LOG_fE2Low3ValidMask 0x10
#define     LOG_fE2Low3ValidShift 4
#define LOG_fE2Low4Valid                        35      // 1 Bit, Bit 3
#define     LOG_fE2Low4ValidMask 0x08
#define     LOG_fE2Low4ValidShift 3
#define LOG_fE2Low5Valid                        35      // 1 Bit, Bit 2
#define     LOG_fE2Low5ValidMask 0x04
#define     LOG_fE2Low5ValidShift 2
#define LOG_fE2Low6Valid                        35      // 1 Bit, Bit 1
#define     LOG_fE2Low6ValidMask 0x02
#define     LOG_fE2Low6ValidShift 1
#define LOG_fE2Low0Dpt2                         28      // 8 Bits, Bit 7-0
#define LOG_fE2Low1Dpt2                         29      // 8 Bits, Bit 7-0
#define LOG_fE2Low2Dpt2                         30      // 8 Bits, Bit 7-0
#define LOG_fE2Low3Dpt2                         31      // 8 Bits, Bit 7-0
#define LOG_fE2LowDpt2Fix                       28      // 8 Bits, Bit 7-0
#define LOG_fE2Low0Dpt3Dir                      28      // 5 Bits, Bit 7-3
#define     LOG_fE2Low0Dpt3DirMask 0xF8
#define     LOG_fE2Low0Dpt3DirShift 3
#define LOG_fE2Low0Dpt3Dim                      28      // 3 Bits, Bit 2-0
#define     LOG_fE2Low0Dpt3DimMask 0x07
#define     LOG_fE2Low0Dpt3DimShift 0
#define LOG_fE2Low1Dpt3Dir                      29      // 5 Bits, Bit 7-3
#define     LOG_fE2Low1Dpt3DirMask 0xF8
#define     LOG_fE2Low1Dpt3DirShift 3
#define LOG_fE2Low1Dpt3Dim                      29      // 3 Bits, Bit 2-0
#define     LOG_fE2Low1Dpt3DimMask 0x07
#define     LOG_fE2Low1Dpt3DimShift 0
#define LOG_fE2Low2Dpt3Dir                      30      // 5 Bits, Bit 7-3
#define     LOG_fE2Low2Dpt3DirMask 0xF8
#define     LOG_fE2Low2Dpt3DirShift 3
#define LOG_fE2Low2Dpt3Dim                      30      // 3 Bits, Bit 2-0
#define     LOG_fE2Low2Dpt3DimMask 0x07
#define     LOG_fE2Low2Dpt3DimShift 0
#define LOG_fE2Low3Dpt3Dir                      31      // 5 Bits, Bit 7-3
#define     LOG_fE2Low3Dpt3DirMask 0xF8
#define     LOG_fE2Low3Dpt3DirShift 3
#define LOG_fE2Low3Dpt3Dim                      31      // 3 Bits, Bit 2-0
#define     LOG_fE2Low3Dpt3DimMask 0x07
#define     LOG_fE2Low3Dpt3DimShift 0
#define LOG_fE2LowDpt3FixDir                    28      // 5 Bits, Bit 7-3
#define     LOG_fE2LowDpt3FixDirMask 0xF8
#define     LOG_fE2LowDpt3FixDirShift 3
#define LOG_fE2LowDpt3FixDim                    28      // 3 Bits, Bit 2-0
#define     LOG_fE2LowDpt3FixDimMask 0x07
#define     LOG_fE2LowDpt3FixDimShift 0
#define LOG_fE2LowDpt5                          28      // uint8_t
#define LOG_fE2HighDpt5                         32      // uint8_t
#define LOG_fE2Low0Dpt5In                       28      // uint8_t
#define LOG_fE2Low1Dpt5In                       29      // uint8_t
#define LOG_fE2Low2Dpt5In                       30      // uint8_t
#define LOG_fE2Low3Dpt5In                       31      // uint8_t
#define LOG_fE2Low4Dpt5In                       32      // uint8_t
#define LOG_fE2Low5Dpt5In                       33      // uint8_t
#define LOG_fE2Low6Dpt5In                       34      // uint8_t
#define LOG_fE2LowDpt5Fix                       28      // uint8_t
#define LOG_fE2LowDpt5001                       28      // uint8_t
#define LOG_fE2HighDpt5001                      32      // uint8_t
#define LOG_fE2Low0Dpt5xIn                      28      // uint8_t
#define LOG_fE2Low1Dpt5xIn                      29      // uint8_t
#define LOG_fE2Low2Dpt5xIn                      30      // uint8_t
#define LOG_fE2Low3Dpt5xIn                      31      // uint8_t
#define LOG_fE2Low4Dpt5xIn                      32      // uint8_t
#define LOG_fE2Low5Dpt5xIn                      33      // uint8_t
#define LOG_fE2Low6Dpt5xIn                      34      // uint8_t
#define LOG_fE2LowDpt5xFix                      28      // uint8_t
#define LOG_fE2LowDpt6                          28      // int8_t
#define LOG_fE2HighDpt6                         32      // int8_t
#define LOG_fE2Low0Dpt6In                       28      // int8_t
#define LOG_fE2Low1Dpt6In                       29      // int8_t
#define LOG_fE2Low2Dpt6In                       30      // int8_t
#define LOG_fE2Low3Dpt6In                       31      // int8_t
#define LOG_fE2Low4Dpt6In                       32      // int8_t
#define LOG_fE2Low5Dpt6In                       33      // int8_t
#define LOG_fE2Low6Dpt6In                       34      // int8_t
#define LOG_fE2LowDpt6Fix                       28      // int8_t
#define LOG_fE2LowDpt7                          28      // uint16_t
#define LOG_fE2HighDpt7                         32      // uint16_t
#define LOG_fE2Low0Dpt7In                       28      // uint16_t
#define LOG_fE2Low1Dpt7In                       30      // uint16_t
#define LOG_fE2Low2Dpt7In                       32      // uint16_t
#define LOG_fE2LowDpt7Fix                       28      // uint16_t
#define LOG_fE2LowDpt8                          28      // int16_t
#define LOG_fE2HighDpt8                         32      // int16_t
#define LOG_fE2Low0Dpt8In                       28      // int16_t
#define LOG_fE2Low1Dpt8In                       30      // int16_t
#define LOG_fE2Low2Dpt8In                       32      // int16_t
#define LOG_fE2LowDpt8Fix                       28      // int16_t
#define LOG_fE2LowDpt9                          28      // float
#define LOG_fE2HighDpt9                         32      // float
#define LOG_fE2LowDpt9Fix                       28      // float
#define LOG_fE2LowDpt12                         28      // uint32_t
#define LOG_fE2HighDpt12                        32      // uint32_t
#define LOG_fE2LowDpt12Fix                      28      // uint32_t
#define LOG_fE2LowDpt13                         28      // int32_t
#define LOG_fE2HighDpt13                        32      // int32_t
#define LOG_fE2LowDpt13Fix                      28      // int32_t
#define LOG_fE2LowDpt14                         28      // float
#define LOG_fE2HighDpt14                        32      // float
#define LOG_fE2LowDpt14Fix                      28      // float
#define LOG_fE2Low0Dpt17                        28      // 8 Bits, Bit 7-0
#define LOG_fE2Low1Dpt17                        29      // 8 Bits, Bit 7-0
#define LOG_fE2Low2Dpt17                        30      // 8 Bits, Bit 7-0
#define LOG_fE2Low3Dpt17                        31      // 8 Bits, Bit 7-0
#define LOG_fE2Low4Dpt17                        32      // 8 Bits, Bit 7-0
#define LOG_fE2Low5Dpt17                        33      // 8 Bits, Bit 7-0
#define LOG_fE2Low6Dpt17                        34      // 8 Bits, Bit 7-0
#define LOG_fE2Low7Dpt17                        35      // 8 Bits, Bit 7-0
#define LOG_fE2LowDpt17Fix                      28      // 8 Bits, Bit 7-0
#define LOG_fE2LowDptRGB                        28      // int32_t
#define LOG_fE2HighDptRGB                       32      // int32_t
#define LOG_fE2LowDptRGBFix                     28      // int32_t
#define LOG_fTd1DuskDawn                         6      // 4 Bits, Bit 7-4
#define     LOG_fTd1DuskDawnMask 0xF0
#define     LOG_fTd1DuskDawnShift 4
#define LOG_fTd2DuskDawn                         6      // 4 Bits, Bit 3-0
#define     LOG_fTd2DuskDawnMask 0x0F
#define     LOG_fTd2DuskDawnShift 0
#define LOG_fTd3DuskDawn                         7      // 4 Bits, Bit 7-4
#define     LOG_fTd3DuskDawnMask 0xF0
#define     LOG_fTd3DuskDawnShift 4
#define LOG_fTd4DuskDawn                         7      // 4 Bits, Bit 3-0
#define     LOG_fTd4DuskDawnMask 0x0F
#define     LOG_fTd4DuskDawnShift 0
#define LOG_fTd5DuskDawn                         8      // 4 Bits, Bit 7-4
#define     LOG_fTd5DuskDawnMask 0xF0
#define     LOG_fTd5DuskDawnShift 4
#define LOG_fTd6DuskDawn                         8      // 4 Bits, Bit 3-0
#define     LOG_fTd6DuskDawnMask 0x0F
#define     LOG_fTd6DuskDawnShift 0
#define LOG_fTd7DuskDawn                         9      // 4 Bits, Bit 7-4
#define     LOG_fTd7DuskDawnMask 0xF0
#define     LOG_fTd7DuskDawnShift 4
#define LOG_fTd8DuskDawn                         9      // 4 Bits, Bit 3-0
#define     LOG_fTd8DuskDawnMask 0x0F
#define     LOG_fTd8DuskDawnShift 0
#define LOG_fTYearDay                           10      // 2 Bits, Bit 7-6
#define     LOG_fTYearDayMask 0xC0
#define     LOG_fTYearDayShift 6
#define LOG_fTHoliday                           10      // 2 Bits, Bit 5-4
#define     LOG_fTHolidayMask 0x30
#define     LOG_fTHolidayShift 4
#define LOG_fTRestoreState                      10      // 2 Bits, Bit 3-2
#define     LOG_fTRestoreStateMask 0x0C
#define     LOG_fTRestoreStateShift 2
#define LOG_fTVacation                          10      // 2 Bits, Bit 1-0
#define     LOG_fTVacationMask 0x03
#define     LOG_fTVacationShift 0
#define LOG_fTd1ValueNum                        11      // uint8_t
#define LOG_fTd2ValueNum                        12      // uint8_t
#define LOG_fTd3ValueNum                        13      // uint8_t
#define LOG_fTd4ValueNum                        14      // uint8_t
#define LOG_fTd5ValueNum                        15      // uint8_t
#define LOG_fTd6ValueNum                        16      // uint8_t
#define LOG_fTd7ValueNum                        17      // uint8_t
#define LOG_fTd8ValueNum                        18      // uint8_t
#define LOG_fTd1Value                           20      // 1 Bit, Bit 7
#define     LOG_fTd1ValueMask 0x80
#define     LOG_fTd1ValueShift 7
#define LOG_fTd1Degree                          20      // 6 Bits, Bit 6-1
#define     LOG_fTd1DegreeMask 0x7E
#define     LOG_fTd1DegreeShift 1
#define LOG_fTd1HourAbs                         20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourAbsMask 0x3E
#define     LOG_fTd1HourAbsShift 1
#define LOG_fTd1HourRel                         20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourRelMask 0x3E
#define     LOG_fTd1HourRelShift 1
#define LOG_fTd1HourRelShort                    20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourRelShortMask 0x3E
#define     LOG_fTd1HourRelShortShift 1
#define LOG_fTd1MinuteAbs                       20      // 6 Bits, Bit 0--5
#define LOG_fTd1MinuteRel                       20      // 6 Bits, Bit 0--5
#define LOG_fTd1Weekday                         21      // 3 Bits, Bit 2-0
#define     LOG_fTd1WeekdayMask 0x07
#define     LOG_fTd1WeekdayShift 0
#define LOG_fTd2Value                           22      // 1 Bit, Bit 7
#define     LOG_fTd2ValueMask 0x80
#define     LOG_fTd2ValueShift 7
#define LOG_fTd2Degree                          22      // 6 Bits, Bit 6-1
#define     LOG_fTd2DegreeMask 0x7E
#define     LOG_fTd2DegreeShift 1
#define LOG_fTd2HourAbs                         22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourAbsMask 0x3E
#define     LOG_fTd2HourAbsShift 1
#define LOG_fTd2HourRel                         22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourRelMask 0x3E
#define     LOG_fTd2HourRelShift 1
#define LOG_fTd2HourRelShort                    22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourRelShortMask 0x3E
#define     LOG_fTd2HourRelShortShift 1
#define LOG_fTd2MinuteAbs                       22      // 6 Bits, Bit 0--5
#define LOG_fTd2MinuteRel                       22      // 6 Bits, Bit 0--5
#define LOG_fTd2Weekday                         23      // 3 Bits, Bit 2-0
#define     LOG_fTd2WeekdayMask 0x07
#define     LOG_fTd2WeekdayShift 0
#define LOG_fTd3Value                           24      // 1 Bit, Bit 7
#define     LOG_fTd3ValueMask 0x80
#define     LOG_fTd3ValueShift 7
#define LOG_fTd3Degree                          24      // 6 Bits, Bit 6-1
#define     LOG_fTd3DegreeMask 0x7E
#define     LOG_fTd3DegreeShift 1
#define LOG_fTd3HourAbs                         24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourAbsMask 0x3E
#define     LOG_fTd3HourAbsShift 1
#define LOG_fTd3HourRel                         24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourRelMask 0x3E
#define     LOG_fTd3HourRelShift 1
#define LOG_fTd3HourRelShort                    24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourRelShortMask 0x3E
#define     LOG_fTd3HourRelShortShift 1
#define LOG_fTd3MinuteAbs                       24      // 6 Bits, Bit 0--5
#define LOG_fTd3MinuteRel                       24      // 6 Bits, Bit 0--5
#define LOG_fTd3Weekday                         25      // 3 Bits, Bit 2-0
#define     LOG_fTd3WeekdayMask 0x07
#define     LOG_fTd3WeekdayShift 0
#define LOG_fTd4Value                           26      // 1 Bit, Bit 7
#define     LOG_fTd4ValueMask 0x80
#define     LOG_fTd4ValueShift 7
#define LOG_fTd4Degree                          26      // 6 Bits, Bit 6-1
#define     LOG_fTd4DegreeMask 0x7E
#define     LOG_fTd4DegreeShift 1
#define LOG_fTd4HourAbs                         26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourAbsMask 0x3E
#define     LOG_fTd4HourAbsShift 1
#define LOG_fTd4HourRel                         26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourRelMask 0x3E
#define     LOG_fTd4HourRelShift 1
#define LOG_fTd4HourRelShort                    26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourRelShortMask 0x3E
#define     LOG_fTd4HourRelShortShift 1
#define LOG_fTd4MinuteAbs                       26      // 6 Bits, Bit 0--5
#define LOG_fTd4MinuteRel                       26      // 6 Bits, Bit 0--5
#define LOG_fTd4Weekday                         27      // 3 Bits, Bit 2-0
#define     LOG_fTd4WeekdayMask 0x07
#define     LOG_fTd4WeekdayShift 0
#define LOG_fTd5Value                           28      // 1 Bit, Bit 7
#define     LOG_fTd5ValueMask 0x80
#define     LOG_fTd5ValueShift 7
#define LOG_fTd5Degree                          28      // 6 Bits, Bit 6-1
#define     LOG_fTd5DegreeMask 0x7E
#define     LOG_fTd5DegreeShift 1
#define LOG_fTd5HourAbs                         28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourAbsMask 0x3E
#define     LOG_fTd5HourAbsShift 1
#define LOG_fTd5HourRel                         28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourRelMask 0x3E
#define     LOG_fTd5HourRelShift 1
#define LOG_fTd5HourRelShort                    28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourRelShortMask 0x3E
#define     LOG_fTd5HourRelShortShift 1
#define LOG_fTd5MinuteAbs                       28      // 6 Bits, Bit 0--5
#define LOG_fTd5MinuteRel                       28      // 6 Bits, Bit 0--5
#define LOG_fTd5Weekday                         29      // 3 Bits, Bit 2-0
#define     LOG_fTd5WeekdayMask 0x07
#define     LOG_fTd5WeekdayShift 0
#define LOG_fTd6Value                           30      // 1 Bit, Bit 7
#define     LOG_fTd6ValueMask 0x80
#define     LOG_fTd6ValueShift 7
#define LOG_fTd6Degree                          30      // 6 Bits, Bit 6-1
#define     LOG_fTd6DegreeMask 0x7E
#define     LOG_fTd6DegreeShift 1
#define LOG_fTd6HourAbs                         30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourAbsMask 0x3E
#define     LOG_fTd6HourAbsShift 1
#define LOG_fTd6HourRel                         30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourRelMask 0x3E
#define     LOG_fTd6HourRelShift 1
#define LOG_fTd6HourRelShort                    30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourRelShortMask 0x3E
#define     LOG_fTd6HourRelShortShift 1
#define LOG_fTd6MinuteAbs                       30      // 6 Bits, Bit 0--5
#define LOG_fTd6MinuteRel                       30      // 6 Bits, Bit 0--5
#define LOG_fTd6Weekday                         31      // 3 Bits, Bit 2-0
#define     LOG_fTd6WeekdayMask 0x07
#define     LOG_fTd6WeekdayShift 0
#define LOG_fTd7Value                           32      // 1 Bit, Bit 7
#define     LOG_fTd7ValueMask 0x80
#define     LOG_fTd7ValueShift 7
#define LOG_fTd7Degree                          32      // 6 Bits, Bit 6-1
#define     LOG_fTd7DegreeMask 0x7E
#define     LOG_fTd7DegreeShift 1
#define LOG_fTd7HourAbs                         32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourAbsMask 0x3E
#define     LOG_fTd7HourAbsShift 1
#define LOG_fTd7HourRel                         32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourRelMask 0x3E
#define     LOG_fTd7HourRelShift 1
#define LOG_fTd7HourRelShort                    32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourRelShortMask 0x3E
#define     LOG_fTd7HourRelShortShift 1
#define LOG_fTd7MinuteAbs                       32      // 6 Bits, Bit 0--5
#define LOG_fTd7MinuteRel                       32      // 6 Bits, Bit 0--5
#define LOG_fTd7Weekday                         33      // 3 Bits, Bit 2-0
#define     LOG_fTd7WeekdayMask 0x07
#define     LOG_fTd7WeekdayShift 0
#define LOG_fTd8Value                           34      // 1 Bit, Bit 7
#define     LOG_fTd8ValueMask 0x80
#define     LOG_fTd8ValueShift 7
#define LOG_fTd8Degree                          34      // 6 Bits, Bit 6-1
#define     LOG_fTd8DegreeMask 0x7E
#define     LOG_fTd8DegreeShift 1
#define LOG_fTd8HourAbs                         34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourAbsMask 0x3E
#define     LOG_fTd8HourAbsShift 1
#define LOG_fTd8HourRel                         34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourRelMask 0x3E
#define     LOG_fTd8HourRelShift 1
#define LOG_fTd8HourRelShort                    34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourRelShortMask 0x3E
#define     LOG_fTd8HourRelShortShift 1
#define LOG_fTd8MinuteAbs                       34      // 6 Bits, Bit 0--5
#define LOG_fTd8MinuteRel                       34      // 6 Bits, Bit 0--5
#define LOG_fTd8Weekday                         35      // 3 Bits, Bit 2-0
#define     LOG_fTd8WeekdayMask 0x07
#define     LOG_fTd8WeekdayShift 0
#define LOG_fTy1Weekday1                        28      // 1 Bit, Bit 7
#define     LOG_fTy1Weekday1Mask 0x80
#define     LOG_fTy1Weekday1Shift 7
#define LOG_fTy1Weekday2                        28      // 1 Bit, Bit 6
#define     LOG_fTy1Weekday2Mask 0x40
#define     LOG_fTy1Weekday2Shift 6
#define LOG_fTy1Weekday3                        28      // 1 Bit, Bit 5
#define     LOG_fTy1Weekday3Mask 0x20
#define     LOG_fTy1Weekday3Shift 5
#define LOG_fTy1Weekday4                        28      // 1 Bit, Bit 4
#define     LOG_fTy1Weekday4Mask 0x10
#define     LOG_fTy1Weekday4Shift 4
#define LOG_fTy1Weekday5                        28      // 1 Bit, Bit 3
#define     LOG_fTy1Weekday5Mask 0x08
#define     LOG_fTy1Weekday5Shift 3
#define LOG_fTy1Weekday6                        28      // 1 Bit, Bit 2
#define     LOG_fTy1Weekday6Mask 0x04
#define     LOG_fTy1Weekday6Shift 2
#define LOG_fTy1Weekday7                        28      // 1 Bit, Bit 1
#define     LOG_fTy1Weekday7Mask 0x02
#define     LOG_fTy1Weekday7Shift 1
#define LOG_fTy1Day                             28      // 7 Bits, Bit 7-1
#define     LOG_fTy1DayMask 0xFE
#define     LOG_fTy1DayShift 1
#define LOG_fTy1IsWeekday                       28      // 1 Bit, Bit 0
#define     LOG_fTy1IsWeekdayMask 0x01
#define     LOG_fTy1IsWeekdayShift 0
#define LOG_fTy1Month                           29      // 4 Bits, Bit 7-4
#define     LOG_fTy1MonthMask 0xF0
#define     LOG_fTy1MonthShift 4
#define LOG_fTy2Weekday1                        30      // 1 Bit, Bit 7
#define     LOG_fTy2Weekday1Mask 0x80
#define     LOG_fTy2Weekday1Shift 7
#define LOG_fTy2Weekday2                        30      // 1 Bit, Bit 6
#define     LOG_fTy2Weekday2Mask 0x40
#define     LOG_fTy2Weekday2Shift 6
#define LOG_fTy2Weekday3                        30      // 1 Bit, Bit 5
#define     LOG_fTy2Weekday3Mask 0x20
#define     LOG_fTy2Weekday3Shift 5
#define LOG_fTy2Weekday4                        30      // 1 Bit, Bit 4
#define     LOG_fTy2Weekday4Mask 0x10
#define     LOG_fTy2Weekday4Shift 4
#define LOG_fTy2Weekday5                        30      // 1 Bit, Bit 3
#define     LOG_fTy2Weekday5Mask 0x08
#define     LOG_fTy2Weekday5Shift 3
#define LOG_fTy2Weekday6                        30      // 1 Bit, Bit 2
#define     LOG_fTy2Weekday6Mask 0x04
#define     LOG_fTy2Weekday6Shift 2
#define LOG_fTy2Weekday7                        30      // 1 Bit, Bit 1
#define     LOG_fTy2Weekday7Mask 0x02
#define     LOG_fTy2Weekday7Shift 1
#define LOG_fTy2Day                             30      // 7 Bits, Bit 7-1
#define     LOG_fTy2DayMask 0xFE
#define     LOG_fTy2DayShift 1
#define LOG_fTy2IsWeekday                       30      // 1 Bit, Bit 0
#define     LOG_fTy2IsWeekdayMask 0x01
#define     LOG_fTy2IsWeekdayShift 0
#define LOG_fTy2Month                           31      // 4 Bits, Bit 7-4
#define     LOG_fTy2MonthMask 0xF0
#define     LOG_fTy2MonthShift 4
#define LOG_fTy3Weekday1                        32      // 1 Bit, Bit 7
#define     LOG_fTy3Weekday1Mask 0x80
#define     LOG_fTy3Weekday1Shift 7
#define LOG_fTy3Weekday2                        32      // 1 Bit, Bit 6
#define     LOG_fTy3Weekday2Mask 0x40
#define     LOG_fTy3Weekday2Shift 6
#define LOG_fTy3Weekday3                        32      // 1 Bit, Bit 5
#define     LOG_fTy3Weekday3Mask 0x20
#define     LOG_fTy3Weekday3Shift 5
#define LOG_fTy3Weekday4                        32      // 1 Bit, Bit 4
#define     LOG_fTy3Weekday4Mask 0x10
#define     LOG_fTy3Weekday4Shift 4
#define LOG_fTy3Weekday5                        32      // 1 Bit, Bit 3
#define     LOG_fTy3Weekday5Mask 0x08
#define     LOG_fTy3Weekday5Shift 3
#define LOG_fTy3Weekday6                        32      // 1 Bit, Bit 2
#define     LOG_fTy3Weekday6Mask 0x04
#define     LOG_fTy3Weekday6Shift 2
#define LOG_fTy3Weekday7                        32      // 1 Bit, Bit 1
#define     LOG_fTy3Weekday7Mask 0x02
#define     LOG_fTy3Weekday7Shift 1
#define LOG_fTy3Day                             32      // 7 Bits, Bit 7-1
#define     LOG_fTy3DayMask 0xFE
#define     LOG_fTy3DayShift 1
#define LOG_fTy3IsWeekday                       32      // 1 Bit, Bit 0
#define     LOG_fTy3IsWeekdayMask 0x01
#define     LOG_fTy3IsWeekdayShift 0
#define LOG_fTy3Month                           33      // 4 Bits, Bit 7-4
#define     LOG_fTy3MonthMask 0xF0
#define     LOG_fTy3MonthShift 4
#define LOG_fTy4Weekday1                        34      // 1 Bit, Bit 7
#define     LOG_fTy4Weekday1Mask 0x80
#define     LOG_fTy4Weekday1Shift 7
#define LOG_fTy4Weekday2                        34      // 1 Bit, Bit 6
#define     LOG_fTy4Weekday2Mask 0x40
#define     LOG_fTy4Weekday2Shift 6
#define LOG_fTy4Weekday3                        34      // 1 Bit, Bit 5
#define     LOG_fTy4Weekday3Mask 0x20
#define     LOG_fTy4Weekday3Shift 5
#define LOG_fTy4Weekday4                        34      // 1 Bit, Bit 4
#define     LOG_fTy4Weekday4Mask 0x10
#define     LOG_fTy4Weekday4Shift 4
#define LOG_fTy4Weekday5                        34      // 1 Bit, Bit 3
#define     LOG_fTy4Weekday5Mask 0x08
#define     LOG_fTy4Weekday5Shift 3
#define LOG_fTy4Weekday6                        34      // 1 Bit, Bit 2
#define     LOG_fTy4Weekday6Mask 0x04
#define     LOG_fTy4Weekday6Shift 2
#define LOG_fTy4Weekday7                        34      // 1 Bit, Bit 1
#define     LOG_fTy4Weekday7Mask 0x02
#define     LOG_fTy4Weekday7Shift 1
#define LOG_fTy4Day                             34      // 7 Bits, Bit 7-1
#define     LOG_fTy4DayMask 0xFE
#define     LOG_fTy4DayShift 1
#define LOG_fTy4IsWeekday                       34      // 1 Bit, Bit 0
#define     LOG_fTy4IsWeekdayMask 0x01
#define     LOG_fTy4IsWeekdayShift 0
#define LOG_fTy4Month                           35      // 4 Bits, Bit 7-4
#define     LOG_fTy4MonthMask 0xF0
#define     LOG_fTy4MonthShift 4
#define LOG_fI1                                 36      // 2 Bits, Bit 7-6
#define     LOG_fI1Mask 0xC0
#define     LOG_fI1Shift 6
#define LOG_fI1Kind                             36      // 2 Bits, Bit 5-4
#define     LOG_fI1KindMask 0x30
#define     LOG_fI1KindShift 4
#define LOG_fI1AsTrigger                        36      // 1 Bit, Bit 3
#define     LOG_fI1AsTriggerMask 0x08
#define     LOG_fI1AsTriggerShift 3
#define LOG_fI1Function                         37      // uint8_t
#define LOG_fI1FunctionRel                      37      // int8_t
#define LOG_fI2                                 38      // 2 Bits, Bit 7-6
#define     LOG_fI2Mask 0xC0
#define     LOG_fI2Shift 6
#define LOG_fI2Kind                             38      // 2 Bits, Bit 5-4
#define     LOG_fI2KindMask 0x30
#define     LOG_fI2KindShift 4
#define LOG_fI2AsTrigger                        38      // 1 Bit, Bit 3
#define     LOG_fI2AsTriggerMask 0x08
#define     LOG_fI2AsTriggerShift 3
#define LOG_fI2Function                         39      // uint8_t
#define LOG_fI2FunctionRel                      39      // int8_t
#define LOG_fOStairtimeBase                     40      // 2 Bits, Bit 7-6
#define     LOG_fOStairtimeBaseMask 0xC0
#define     LOG_fOStairtimeBaseShift 6
#define LOG_fOStairtimeTime                     40      // 14 Bits, Bit 13-0
#define     LOG_fOStairtimeTimeMask 0x3FFF
#define     LOG_fOStairtimeTimeShift 0
#define LOG_fOBlinkBase                         42      // 2 Bits, Bit 7-6
#define     LOG_fOBlinkBaseMask 0xC0
#define     LOG_fOBlinkBaseShift 6
#define LOG_fOBlinkTime                         42      // 14 Bits, Bit 13-0
#define     LOG_fOBlinkTimeMask 0x3FFF
#define     LOG_fOBlinkTimeShift 0
#define LOG_fODelayOnBase                       44      // 2 Bits, Bit 7-6
#define     LOG_fODelayOnBaseMask 0xC0
#define     LOG_fODelayOnBaseShift 6
#define LOG_fODelayOnTime                       44      // 14 Bits, Bit 13-0
#define     LOG_fODelayOnTimeMask 0x3FFF
#define     LOG_fODelayOnTimeShift 0
#define LOG_fODelayOffBase                      46      // 2 Bits, Bit 7-6
#define     LOG_fODelayOffBaseMask 0xC0
#define     LOG_fODelayOffBaseShift 6
#define LOG_fODelayOffTime                      46      // 14 Bits, Bit 13-0
#define     LOG_fODelayOffTimeMask 0x3FFF
#define     LOG_fODelayOffTimeShift 0
#define LOG_fORepeatOnBase                      48      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOnBaseMask 0xC0
#define     LOG_fORepeatOnBaseShift 6
#define LOG_fORepeatOnTime                      48      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOnTimeMask 0x3FFF
#define     LOG_fORepeatOnTimeShift 0
#define LOG_fORepeatOffBase                     50      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOffBaseMask 0xC0
#define     LOG_fORepeatOffBaseShift 6
#define LOG_fORepeatOffTime                     50      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOffTimeMask 0x3FFF
#define     LOG_fORepeatOffTimeShift 0
#define LOG_fODelay                             52      // 1 Bit, Bit 7
#define     LOG_fODelayMask 0x80
#define     LOG_fODelayShift 7
#define LOG_fODelayOnRepeat                     52      // 2 Bits, Bit 6-5
#define     LOG_fODelayOnRepeatMask 0x60
#define     LOG_fODelayOnRepeatShift 5
#define LOG_fODelayOnReset                      52      // 1 Bit, Bit 4
#define     LOG_fODelayOnResetMask 0x10
#define     LOG_fODelayOnResetShift 4
#define LOG_fODelayOffRepeat                    52      // 2 Bits, Bit 3-2
#define     LOG_fODelayOffRepeatMask 0x0C
#define     LOG_fODelayOffRepeatShift 2
#define LOG_fODelayOffReset                     52      // 1 Bit, Bit 1
#define     LOG_fODelayOffResetMask 0x02
#define     LOG_fODelayOffResetShift 1
#define LOG_fOStair                             52      // 1 Bit, Bit 0
#define     LOG_fOStairMask 0x01
#define     LOG_fOStairShift 0
#define LOG_fORetrigger                         53      // 1 Bit, Bit 7
#define     LOG_fORetriggerMask 0x80
#define     LOG_fORetriggerShift 7
#define LOG_fOStairOff                          53      // 1 Bit, Bit 6
#define     LOG_fOStairOffMask 0x40
#define     LOG_fOStairOffShift 6
#define LOG_fORepeat                            53      // 1 Bit, Bit 5
#define     LOG_fORepeatMask 0x20
#define     LOG_fORepeatShift 5
#define LOG_fOOutputFilter                      53      // 2 Bits, Bit 4-3
#define     LOG_fOOutputFilterMask 0x18
#define     LOG_fOOutputFilterShift 3
#define LOG_fOSendOnChange                      53      // 1 Bit, Bit 2
#define     LOG_fOSendOnChangeMask 0x04
#define     LOG_fOSendOnChangeShift 2
#define LOG_fODpt                               54      // 8 Bits, Bit 7-0
#define LOG_fOOn                                55      // 8 Bits, Bit 7-0
#define LOG_fOOnBuzzer                          55      // 8 Bits, Bit 7-0
#define LOG_fOOnLed                             55      // 8 Bits, Bit 7-0
#define LOG_fOOnAll                             55      // 8 Bits, Bit 7-0
#define LOG_fOOnTone                            56      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt1                            56      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt2                            56      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt3Dir                         56      // 5 Bits, Bit 7-3
#define     LOG_fOOnDpt3DirMask 0xF8
#define     LOG_fOOnDpt3DirShift 3
#define LOG_fOOnDpt3Dim                         56      // 3 Bits, Bit 2-0
#define     LOG_fOOnDpt3DimMask 0x07
#define     LOG_fOOnDpt3DimShift 0
#define LOG_fOOnDpt5                            56      // uint8_t
#define LOG_fOOnDpt5001                         56      // uint8_t
#define LOG_fOOnDpt6                            56      // int8_t
#define LOG_fOOnDpt7                            56      // uint16_t
#define LOG_fOOnDpt8                            56      // int16_t
#define LOG_fOOnDpt9                            56      // float
#define LOG_fOOnDpt12                           56      // uint32_t
#define LOG_fOOnDpt13                           56      // int32_t
#define LOG_fOOnDpt14                           56      // float
#define LOG_fOOnDpt16                           56      // char*, 14 Byte
#define LOG_fOOnDpt17                           56      // 8 Bits, Bit 7-0
#define LOG_fOOnRGB                             56      // 24 Bits, Bit 31-8
#define     LOG_fOOnRGBMask 0xFFFFFF00
#define     LOG_fOOnRGBShift 8
#define LOG_fOOnPAArea                          56      // 4 Bits, Bit 7-4
#define     LOG_fOOnPAAreaMask 0xF0
#define     LOG_fOOnPAAreaShift 4
#define LOG_fOOnPALine                          56      // 4 Bits, Bit 3-0
#define     LOG_fOOnPALineMask 0x0F
#define     LOG_fOOnPALineShift 0
#define LOG_fOOnPADevice                        57      // uint8_t
#define LOG_fOOnFunction                        56      // 8 Bits, Bit 7-0
#define LOG_fOOnKOKind                          61      // 2 Bits, Bit 7-6
#define     LOG_fOOnKOKindMask 0xC0
#define     LOG_fOOnKOKindShift 6
#define LOG_fOOnKONumber                        56      // uint16_t
#define LOG_fOOnKONumberRel                     56      // int16_t
#define LOG_fOOnKODpt                           58      // 8 Bits, Bit 7-0
#define LOG_fOOnKOSend                          61      // 2 Bits, Bit 5-4
#define     LOG_fOOnKOSendMask 0x30
#define     LOG_fOOnKOSendShift 4
#define LOG_fOOnKOSendNumber                    62      // uint16_t
#define LOG_fOOnKOSendNumberRel                 62      // int16_t
#define LOG_fOOff                               70      // 8 Bits, Bit 7-0
#define LOG_fOOffBuzzer                         70      // 8 Bits, Bit 7-0
#define LOG_fOOffLed                            70      // 8 Bits, Bit 7-0
#define LOG_fOOffAll                            70      // 8 Bits, Bit 7-0
#define LOG_fOOffTone                           71      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt1                           71      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt2                           71      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt3Dir                        71      // 5 Bits, Bit 7-3
#define     LOG_fOOffDpt3DirMask 0xF8
#define     LOG_fOOffDpt3DirShift 3
#define LOG_fOOffDpt3Dim                        71      // 3 Bits, Bit 2-0
#define     LOG_fOOffDpt3DimMask 0x07
#define     LOG_fOOffDpt3DimShift 0
#define LOG_fOOffDpt5                           71      // uint8_t
#define LOG_fOOffDpt5001                        71      // uint8_t
#define LOG_fOOffDpt6                           71      // int8_t
#define LOG_fOOffDpt7                           71      // uint16_t
#define LOG_fOOffDpt8                           71      // int16_t
#define LOG_fOOffDpt9                           71      // float
#define LOG_fOOffDpt12                          71      // uint32_t
#define LOG_fOOffDpt13                          71      // int32_t
#define LOG_fOOffDpt14                          71      // float
#define LOG_fOOffDpt16                          71      // char*, 14 Byte
#define LOG_fOOffDpt17                          71      // 8 Bits, Bit 7-0
#define LOG_fOOffRGB                            71      // 24 Bits, Bit 31-8
#define     LOG_fOOffRGBMask 0xFFFFFF00
#define     LOG_fOOffRGBShift 8
#define LOG_fOOffPAArea                         71      // 4 Bits, Bit 7-4
#define     LOG_fOOffPAAreaMask 0xF0
#define     LOG_fOOffPAAreaShift 4
#define LOG_fOOffPALine                         71      // 4 Bits, Bit 3-0
#define     LOG_fOOffPALineMask 0x0F
#define     LOG_fOOffPALineShift 0
#define LOG_fOOffPADevice                       72      // uint8_t
#define LOG_fOOffFunction                       71      // 8 Bits, Bit 7-0
#define LOG_fOOffKOKind                         76      // 2 Bits, Bit 7-6
#define     LOG_fOOffKOKindMask 0xC0
#define     LOG_fOOffKOKindShift 6
#define LOG_fOOffKONumber                       71      // uint16_t
#define LOG_fOOffKONumberRel                    71      // int16_t
#define LOG_fOOffKODpt                          73      // 8 Bits, Bit 7-0
#define LOG_fOOffKOSend                         76      // 2 Bits, Bit 5-4
#define     LOG_fOOffKOSendMask 0x30
#define     LOG_fOOffKOSendShift 4
#define LOG_fOOffKOSendNumber                   77      // uint16_t
#define LOG_fOOffKOSendNumberRel                77      // int16_t

// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayBase                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fChannelDelayBase)) & LOG_fChannelDelayBaseMask) >> LOG_fChannelDelayBaseShift)
// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayTime                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime)) & LOG_fChannelDelayTimeMask)
// Zeit bis der Kanal nach einem Neustart aktiv wird (in Millisekunden)
#define ParamLOG_fChannelDelayTimeMS                 (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime))))
// Logik-Operation
#define ParamLOG_fLogic                              (knx.paramByte(LOG_ParamCalcIndex(LOG_fLogic)))
// Logik auswerten
#define ParamLOG_fCalculate                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fCalculate)) & LOG_fCalculateMask)
// Kanal deaktivieren (zu Testzwecken)
#define ParamLOG_fDisable                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fDisable)) & LOG_fDisableMask))
// Alarmausgabe (Buzzer oder LED trotz Sperre schalten)?
#define ParamLOG_fAlarm                              ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fAlarm)) & LOG_fAlarmMask))
// Tor geht sofort wieder zu
#define ParamLOG_fTGate                              ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTGate)) & LOG_fTGateMask))
// Wert EIN intern weiterleiten
#define ParamLOG_fOInternalOn                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOInternalOn)) & LOG_fOInternalOnMask))
// Wert AUS intern weiterleiten
#define ParamLOG_fOInternalOff                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOInternalOff)) & LOG_fOInternalOffMask))
// Logik sendet ihren Wert weiter
#define ParamLOG_fTrigger                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fTrigger)))
//           Eingang 1
#define ParamLOG_fTriggerE1                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerE1)) & LOG_fTriggerE1Mask))
//           Eingang 2
#define ParamLOG_fTriggerE2                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerE2)) & LOG_fTriggerE2Mask))
//           Interner Eingang 3
#define ParamLOG_fTriggerI1                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerI1)) & LOG_fTriggerI1Mask))
//           Interner Eingang 4
#define ParamLOG_fTriggerI2                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerI2)) & LOG_fTriggerI2Mask))
// Logik sendet ihren Wert weiter
#define ParamLOG_fTriggerTime                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerTime)))
// Beim schließen vom Tor wird
#define ParamLOG_fTriggerGateClose                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateClose)) & LOG_fTriggerGateCloseMask) >> LOG_fTriggerGateCloseShift)
// Beim öffnen vom Tor wird
#define ParamLOG_fTriggerGateOpen                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateOpen)) & LOG_fTriggerGateOpenMask) >> LOG_fTriggerGateOpenShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertInt                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertInt)) & LOG_fE1ConvertIntMask) >> LOG_fE1ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1Convert                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Convert)) & LOG_fE1ConvertMask) >> LOG_fE1ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertFloat                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertFloat)) & LOG_fE1ConvertFloatMask) >> LOG_fE1ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertSpecial                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertSpecial)) & LOG_fE1ConvertSpecialMask) >> LOG_fE1ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertBool                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertBool)) & LOG_fE1ConvertBoolMask) >> LOG_fE1ConvertBoolShift)
// Eingang 1
#define ParamLOG_fE1                                 (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1)) & LOG_fE1Mask)
// DPT für Eingang
#define ParamLOG_fE1Dpt                              (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Dpt)))
// Eingang wird gelesen alle
#define ParamLOG_fE1RepeatBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1RepeatBase)) & LOG_fE1RepeatBaseMask) >> LOG_fE1RepeatBaseShift)
// Eingang wird gelesen alle
#define ParamLOG_fE1RepeatTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1RepeatTime)) & LOG_fE1RepeatTimeMask)
// Eingang wird gelesen alle (in Millisekunden)
#define ParamLOG_fE1RepeatTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fE1RepeatTime))))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE1OtherKO                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1OtherKO)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE1OtherKORel                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1OtherKORel)))
// Falls Vorbelegung aus dem Speicher nicht möglich oder nicht gewünscht, dann vorbelegen mit
#define ParamLOG_fE1Default                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Default)) & LOG_fE1DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE1DefaultExt                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultExt)) & LOG_fE1DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE1DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultEEPROM)) & LOG_fE1DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE1DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultRepeat)) & LOG_fE1DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE1UseOtherKO                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1UseOtherKO)) & LOG_fE1UseOtherKOMask) >> LOG_fE1UseOtherKOShift)
// Von-Wert
#define ParamLOG_fE1LowDelta                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDelta)))
// Bis-Wert
#define ParamLOG_fE1HighDelta                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDelta)))
// Von-Wert
#define ParamLOG_fE1LowDeltaFloat                    (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDeltaFloat), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDeltaFloat                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDeltaFloat), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE1LowDeltaDouble                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDeltaDouble), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDeltaDouble                  (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDeltaDouble), Float_Enc_IEEE754Single))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low0Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Valid)) & LOG_fE1Low0ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low1Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Valid)) & LOG_fE1Low1ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low2Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Valid)) & LOG_fE1Low2ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low3Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Valid)) & LOG_fE1Low3ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low4Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Valid)) & LOG_fE1Low4ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low5Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Valid)) & LOG_fE1Low5ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low6Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Valid)) & LOG_fE1Low6ValidMask))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE1Low0Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low1Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low2Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low3Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt2)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt2Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt2Fix)))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE1Low0Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt3Dir)) & LOG_fE1Low0Dpt3DirMask) >> LOG_fE1Low0Dpt3DirShift)
// 
#define ParamLOG_fE1Low0Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt3Dim)) & LOG_fE1Low0Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low1Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt3Dir)) & LOG_fE1Low1Dpt3DirMask) >> LOG_fE1Low1Dpt3DirShift)
// 
#define ParamLOG_fE1Low1Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt3Dim)) & LOG_fE1Low1Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low2Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt3Dir)) & LOG_fE1Low2Dpt3DirMask) >> LOG_fE1Low2Dpt3DirShift)
// 
#define ParamLOG_fE1Low2Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt3Dim)) & LOG_fE1Low2Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low3Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt3Dir)) & LOG_fE1Low3Dpt3DirMask) >> LOG_fE1Low3Dpt3DirShift)
// 
#define ParamLOG_fE1Low3Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt3Dim)) & LOG_fE1Low3Dpt3DimMask)
// Eingang ist konstant
#define ParamLOG_fE1LowDpt3FixDir                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt3FixDir)) & LOG_fE1LowDpt3FixDirMask) >> LOG_fE1LowDpt3FixDirShift)
// 
#define ParamLOG_fE1LowDpt3FixDim                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt3FixDim)) & LOG_fE1LowDpt3FixDimMask)
// Von-Wert
#define ParamLOG_fE1LowDpt5                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5)))
// Bis-Wert
#define ParamLOG_fE1HighDpt5                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt5)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt5In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt5Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt5001                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5001)))
// Bis-Wert
#define ParamLOG_fE1HighDpt5001                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt5001)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt5xIn)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt5xFix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5xFix)))
// Von-Wert
#define ParamLOG_fE1LowDpt6                          ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt6)))
// Bis-Wert
#define ParamLOG_fE1HighDpt6                         ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt6)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt6In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt6Fix                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt6Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt7                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt7)))
// Bis-Wert
#define ParamLOG_fE1HighDpt7                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1HighDpt7)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low0Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low1Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low2Dpt7In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt7Fix                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt7Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt8                          ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt8)))
// Bis-Wert
#define ParamLOG_fE1HighDpt8                         ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1HighDpt8)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low0Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low1Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low2Dpt8In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt8Fix                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt8Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt9                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt9), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDpt9                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDpt9), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt9Fix                       (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt9Fix), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE1LowDpt12                         (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt12)))
// Bis-Wert
#define ParamLOG_fE1HighDpt12                        (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDpt12)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt12Fix                      (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt12Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt13                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt13)))
// Bis-Wert
#define ParamLOG_fE1HighDpt13                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDpt13)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt13Fix                      ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt13Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt14                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt14), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDpt14                        (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDpt14), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt14Fix                      (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt14Fix), Float_Enc_IEEE754Single))
// Eingang ist EIN bei Szene
#define ParamLOG_fE1Low0Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low1Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low2Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low3Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low4Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low5Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low6Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low7Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low7Dpt17)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt17Fix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt17Fix)))
// Von-Wert
#define ParamLOG_fE1LowDptRGB                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDptRGB)))
// Bis-Wert
#define ParamLOG_fE1HighDptRGB                       ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDptRGB)))
// Eingang ist konstant
#define ParamLOG_fE1LowDptRGBFix                     ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDptRGBFix)))
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertInt                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertInt)) & LOG_fE2ConvertIntMask) >> LOG_fE2ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2Convert                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Convert)) & LOG_fE2ConvertMask) >> LOG_fE2ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertFloat                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertFloat)) & LOG_fE2ConvertFloatMask) >> LOG_fE2ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertSpecial                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertSpecial)) & LOG_fE2ConvertSpecialMask) >> LOG_fE2ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertBool                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertBool)) & LOG_fE2ConvertBoolMask) >> LOG_fE2ConvertBoolShift)
// Eingang 2
#define ParamLOG_fE2                                 (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2)) & LOG_fE2Mask)
// DPT für Eingang
#define ParamLOG_fE2Dpt                              (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Dpt)))
// Eingang wird gelesen alle
#define ParamLOG_fE2RepeatBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2RepeatBase)) & LOG_fE2RepeatBaseMask) >> LOG_fE2RepeatBaseShift)
// Eingang wird gelesen alle
#define ParamLOG_fE2RepeatTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2RepeatTime)) & LOG_fE2RepeatTimeMask)
// Eingang wird gelesen alle (in Millisekunden)
#define ParamLOG_fE2RepeatTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fE2RepeatTime))))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE2OtherKO                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2OtherKO)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE2OtherKORel                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2OtherKORel)))
// Falls Vorbelegung aus dem Speicher nicht möglich oder nicht gewünscht, dann vorbelegen mit
#define ParamLOG_fE2Default                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Default)) & LOG_fE2DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE2DefaultExt                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultExt)) & LOG_fE2DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE2DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultEEPROM)) & LOG_fE2DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE2DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultRepeat)) & LOG_fE2DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE2UseOtherKO                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2UseOtherKO)) & LOG_fE2UseOtherKOMask) >> LOG_fE2UseOtherKOShift)
// Von-Wert
#define ParamLOG_fE2LowDelta                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDelta)))
// Bis-Wert
#define ParamLOG_fE2HighDelta                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDelta)))
// Von-Wert
#define ParamLOG_fE2LowDeltaFloat                    (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDeltaFloat), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDeltaFloat                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDeltaFloat), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE2LowDeltaDouble                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDeltaDouble), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDeltaDouble                  (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDeltaDouble), Float_Enc_IEEE754Single))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low0Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Valid)) & LOG_fE2Low0ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low1Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Valid)) & LOG_fE2Low1ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low2Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Valid)) & LOG_fE2Low2ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low3Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Valid)) & LOG_fE2Low3ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low4Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Valid)) & LOG_fE2Low4ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low5Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Valid)) & LOG_fE2Low5ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low6Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Valid)) & LOG_fE2Low6ValidMask))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE2Low0Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low1Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low2Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low3Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt2)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt2Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt2Fix)))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE2Low0Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt3Dir)) & LOG_fE2Low0Dpt3DirMask) >> LOG_fE2Low0Dpt3DirShift)
// 
#define ParamLOG_fE2Low0Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt3Dim)) & LOG_fE2Low0Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low1Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt3Dir)) & LOG_fE2Low1Dpt3DirMask) >> LOG_fE2Low1Dpt3DirShift)
// 
#define ParamLOG_fE2Low1Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt3Dim)) & LOG_fE2Low1Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low2Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt3Dir)) & LOG_fE2Low2Dpt3DirMask) >> LOG_fE2Low2Dpt3DirShift)
// 
#define ParamLOG_fE2Low2Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt3Dim)) & LOG_fE2Low2Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low3Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt3Dir)) & LOG_fE2Low3Dpt3DirMask) >> LOG_fE2Low3Dpt3DirShift)
// 
#define ParamLOG_fE2Low3Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt3Dim)) & LOG_fE2Low3Dpt3DimMask)
// Eingang ist konstant
#define ParamLOG_fE2LowDpt3FixDir                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt3FixDir)) & LOG_fE2LowDpt3FixDirMask) >> LOG_fE2LowDpt3FixDirShift)
// 
#define ParamLOG_fE2LowDpt3FixDim                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt3FixDim)) & LOG_fE2LowDpt3FixDimMask)
// Von-Wert
#define ParamLOG_fE2LowDpt5                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5)))
// Bis-Wert
#define ParamLOG_fE2HighDpt5                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt5)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt5In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt5Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt5001                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5001)))
// Bis-Wert
#define ParamLOG_fE2HighDpt5001                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt5001)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt5xIn)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt5xFix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5xFix)))
// Von-Wert
#define ParamLOG_fE2LowDpt6                          ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt6)))
// Bis-Wert
#define ParamLOG_fE2HighDpt6                         ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt6)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt6In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt6Fix                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt6Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt7                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt7)))
// Bis-Wert
#define ParamLOG_fE2HighDpt7                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2HighDpt7)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low0Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low1Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low2Dpt7In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt7Fix                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt7Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt8                          ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt8)))
// Bis-Wert
#define ParamLOG_fE2HighDpt8                         ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2HighDpt8)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low0Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low1Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low2Dpt8In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt8Fix                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt8Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt9                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt9), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDpt9                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDpt9), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt9Fix                       (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt9Fix), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE2LowDpt12                         (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt12)))
// Bis-Wert
#define ParamLOG_fE2HighDpt12                        (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDpt12)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt12Fix                      (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt12Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt13                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt13)))
// Bis-Wert
#define ParamLOG_fE2HighDpt13                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDpt13)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt13Fix                      ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt13Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt14                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt14), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDpt14                        (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDpt14), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt14Fix                      (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt14Fix), Float_Enc_IEEE754Single))
// Eingang ist EIN bei Szene
#define ParamLOG_fE2Low0Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low1Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low2Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low3Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low4Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low5Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low6Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low7Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low7Dpt17)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt17Fix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt17Fix)))
// Von-Wert
#define ParamLOG_fE2LowDptRGB                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDptRGB)))
// Bis-Wert
#define ParamLOG_fE2HighDptRGB                       ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDptRGB)))
// Eingang ist konstant
#define ParamLOG_fE2LowDptRGBFix                     ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDptRGBFix)))
// Zeitbezug
#define ParamLOG_fTd1DuskDawn                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1DuskDawn)) & LOG_fTd1DuskDawnMask) >> LOG_fTd1DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd2DuskDawn                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2DuskDawn)) & LOG_fTd2DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd3DuskDawn                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3DuskDawn)) & LOG_fTd3DuskDawnMask) >> LOG_fTd3DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd4DuskDawn                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4DuskDawn)) & LOG_fTd4DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd5DuskDawn                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5DuskDawn)) & LOG_fTd5DuskDawnMask) >> LOG_fTd5DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd6DuskDawn                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6DuskDawn)) & LOG_fTd6DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd7DuskDawn                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7DuskDawn)) & LOG_fTd7DuskDawnMask) >> LOG_fTd7DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd8DuskDawn                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8DuskDawn)) & LOG_fTd8DuskDawnMask)
// Typ der Zeitschaltuhr
#define ParamLOG_fTYearDay                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTYearDay)) & LOG_fTYearDayMask) >> LOG_fTYearDayShift)
// Feiertagsbehandlung
#define ParamLOG_fTHoliday                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTHoliday)) & LOG_fTHolidayMask) >> LOG_fTHolidayShift)
// Bei Neustart letzte Schaltzeit nachholen
#define ParamLOG_fTRestoreState                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTRestoreState)) & LOG_fTRestoreStateMask) >> LOG_fTRestoreStateShift)
// Urlaubsbehandlung
#define ParamLOG_fTVacation                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fTVacation)) & LOG_fTVacationMask)
// Zahlenwert
#define ParamLOG_fTd1ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1ValueNum)))
// Zahlenwert
#define ParamLOG_fTd2ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2ValueNum)))
// Zahlenwert
#define ParamLOG_fTd3ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3ValueNum)))
// Zahlenwert
#define ParamLOG_fTd4ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4ValueNum)))
// Zahlenwert
#define ParamLOG_fTd5ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5ValueNum)))
// Zahlenwert
#define ParamLOG_fTd6ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6ValueNum)))
// Zahlenwert
#define ParamLOG_fTd7ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7ValueNum)))
// Zahlenwert
#define ParamLOG_fTd8ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8ValueNum)))
// Schaltwert
#define ParamLOG_fTd1Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Value)) & LOG_fTd1ValueMask))
// Grad
#define ParamLOG_fTd1Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Degree)) & LOG_fTd1DegreeMask) >> LOG_fTd1DegreeShift)
// Stunde
#define ParamLOG_fTd1HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourAbs)) & LOG_fTd1HourAbsMask) >> LOG_fTd1HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd1HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourRel)) & LOG_fTd1HourRelMask) >> LOG_fTd1HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd1HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourRelShort)) & LOG_fTd1HourRelShortMask) >> LOG_fTd1HourRelShortShift)
// Minute
#define ParamLOG_fTd1MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1MinuteRel)))
// Wochentag
#define ParamLOG_fTd1Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Weekday)) & LOG_fTd1WeekdayMask)
// Schaltwert
#define ParamLOG_fTd2Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Value)) & LOG_fTd2ValueMask))
// Grad
#define ParamLOG_fTd2Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Degree)) & LOG_fTd2DegreeMask) >> LOG_fTd2DegreeShift)
// Stunde
#define ParamLOG_fTd2HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourAbs)) & LOG_fTd2HourAbsMask) >> LOG_fTd2HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd2HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourRel)) & LOG_fTd2HourRelMask) >> LOG_fTd2HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd2HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourRelShort)) & LOG_fTd2HourRelShortMask) >> LOG_fTd2HourRelShortShift)
// Minute
#define ParamLOG_fTd2MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2MinuteRel)))
// Wochentag
#define ParamLOG_fTd2Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Weekday)) & LOG_fTd2WeekdayMask)
// Schaltwert
#define ParamLOG_fTd3Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Value)) & LOG_fTd3ValueMask))
// Grad
#define ParamLOG_fTd3Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Degree)) & LOG_fTd3DegreeMask) >> LOG_fTd3DegreeShift)
// Stunde
#define ParamLOG_fTd3HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourAbs)) & LOG_fTd3HourAbsMask) >> LOG_fTd3HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd3HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourRel)) & LOG_fTd3HourRelMask) >> LOG_fTd3HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd3HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourRelShort)) & LOG_fTd3HourRelShortMask) >> LOG_fTd3HourRelShortShift)
// Minute
#define ParamLOG_fTd3MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3MinuteRel)))
// Wochentag
#define ParamLOG_fTd3Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Weekday)) & LOG_fTd3WeekdayMask)
// Schaltwert
#define ParamLOG_fTd4Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Value)) & LOG_fTd4ValueMask))
// Grad
#define ParamLOG_fTd4Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Degree)) & LOG_fTd4DegreeMask) >> LOG_fTd4DegreeShift)
// Stunde
#define ParamLOG_fTd4HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourAbs)) & LOG_fTd4HourAbsMask) >> LOG_fTd4HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd4HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourRel)) & LOG_fTd4HourRelMask) >> LOG_fTd4HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd4HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourRelShort)) & LOG_fTd4HourRelShortMask) >> LOG_fTd4HourRelShortShift)
// Minute
#define ParamLOG_fTd4MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4MinuteRel)))
// Wochentag
#define ParamLOG_fTd4Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Weekday)) & LOG_fTd4WeekdayMask)
// Schaltwert
#define ParamLOG_fTd5Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Value)) & LOG_fTd5ValueMask))
// Grad
#define ParamLOG_fTd5Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Degree)) & LOG_fTd5DegreeMask) >> LOG_fTd5DegreeShift)
// Stunde
#define ParamLOG_fTd5HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourAbs)) & LOG_fTd5HourAbsMask) >> LOG_fTd5HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd5HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourRel)) & LOG_fTd5HourRelMask) >> LOG_fTd5HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd5HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourRelShort)) & LOG_fTd5HourRelShortMask) >> LOG_fTd5HourRelShortShift)
// Minute
#define ParamLOG_fTd5MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5MinuteRel)))
// Wochentag
#define ParamLOG_fTd5Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Weekday)) & LOG_fTd5WeekdayMask)
// Schaltwert
#define ParamLOG_fTd6Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Value)) & LOG_fTd6ValueMask))
// Grad
#define ParamLOG_fTd6Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Degree)) & LOG_fTd6DegreeMask) >> LOG_fTd6DegreeShift)
// Stunde
#define ParamLOG_fTd6HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourAbs)) & LOG_fTd6HourAbsMask) >> LOG_fTd6HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd6HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourRel)) & LOG_fTd6HourRelMask) >> LOG_fTd6HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd6HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourRelShort)) & LOG_fTd6HourRelShortMask) >> LOG_fTd6HourRelShortShift)
// Minute
#define ParamLOG_fTd6MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6MinuteRel)))
// Wochentag
#define ParamLOG_fTd6Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Weekday)) & LOG_fTd6WeekdayMask)
// Schaltwert
#define ParamLOG_fTd7Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Value)) & LOG_fTd7ValueMask))
// Grad
#define ParamLOG_fTd7Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Degree)) & LOG_fTd7DegreeMask) >> LOG_fTd7DegreeShift)
// Stunde
#define ParamLOG_fTd7HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourAbs)) & LOG_fTd7HourAbsMask) >> LOG_fTd7HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd7HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourRel)) & LOG_fTd7HourRelMask) >> LOG_fTd7HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd7HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourRelShort)) & LOG_fTd7HourRelShortMask) >> LOG_fTd7HourRelShortShift)
// Minute
#define ParamLOG_fTd7MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7MinuteRel)))
// Wochentag
#define ParamLOG_fTd7Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Weekday)) & LOG_fTd7WeekdayMask)
// Schaltwert
#define ParamLOG_fTd8Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Value)) & LOG_fTd8ValueMask))
// Grad
#define ParamLOG_fTd8Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Degree)) & LOG_fTd8DegreeMask) >> LOG_fTd8DegreeShift)
// Stunde
#define ParamLOG_fTd8HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourAbs)) & LOG_fTd8HourAbsMask) >> LOG_fTd8HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd8HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourRel)) & LOG_fTd8HourRelMask) >> LOG_fTd8HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd8HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourRelShort)) & LOG_fTd8HourRelShortMask) >> LOG_fTd8HourRelShortShift)
// Minute
#define ParamLOG_fTd8MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8MinuteRel)))
// Wochentag
#define ParamLOG_fTd8Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Weekday)) & LOG_fTd8WeekdayMask)
// Mo
#define ParamLOG_fTy1Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday1)) & LOG_fTy1Weekday1Mask))
// Di
#define ParamLOG_fTy1Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday2)) & LOG_fTy1Weekday2Mask))
// Mi
#define ParamLOG_fTy1Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday3)) & LOG_fTy1Weekday3Mask))
// Do
#define ParamLOG_fTy1Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday4)) & LOG_fTy1Weekday4Mask))
// Fr
#define ParamLOG_fTy1Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday5)) & LOG_fTy1Weekday5Mask))
// Sa
#define ParamLOG_fTy1Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday6)) & LOG_fTy1Weekday6Mask))
// So
#define ParamLOG_fTy1Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday7)) & LOG_fTy1Weekday7Mask))
// Tag
#define ParamLOG_fTy1Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Day)) & LOG_fTy1DayMask) >> LOG_fTy1DayShift)
// Wochentag
#define ParamLOG_fTy1IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1IsWeekday)) & LOG_fTy1IsWeekdayMask))
// Monat
#define ParamLOG_fTy1Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Month)) & LOG_fTy1MonthMask) >> LOG_fTy1MonthShift)
// Mo
#define ParamLOG_fTy2Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday1)) & LOG_fTy2Weekday1Mask))
// Di
#define ParamLOG_fTy2Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday2)) & LOG_fTy2Weekday2Mask))
// Mi
#define ParamLOG_fTy2Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday3)) & LOG_fTy2Weekday3Mask))
// Do
#define ParamLOG_fTy2Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday4)) & LOG_fTy2Weekday4Mask))
// Fr
#define ParamLOG_fTy2Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday5)) & LOG_fTy2Weekday5Mask))
// Sa
#define ParamLOG_fTy2Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday6)) & LOG_fTy2Weekday6Mask))
// So
#define ParamLOG_fTy2Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday7)) & LOG_fTy2Weekday7Mask))
// Tag
#define ParamLOG_fTy2Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Day)) & LOG_fTy2DayMask) >> LOG_fTy2DayShift)
// Wochentag
#define ParamLOG_fTy2IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2IsWeekday)) & LOG_fTy2IsWeekdayMask))
// Monat
#define ParamLOG_fTy2Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Month)) & LOG_fTy2MonthMask) >> LOG_fTy2MonthShift)
// Mo
#define ParamLOG_fTy3Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday1)) & LOG_fTy3Weekday1Mask))
// Di
#define ParamLOG_fTy3Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday2)) & LOG_fTy3Weekday2Mask))
// Mi
#define ParamLOG_fTy3Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday3)) & LOG_fTy3Weekday3Mask))
// Do
#define ParamLOG_fTy3Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday4)) & LOG_fTy3Weekday4Mask))
// Fr
#define ParamLOG_fTy3Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday5)) & LOG_fTy3Weekday5Mask))
// Sa
#define ParamLOG_fTy3Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday6)) & LOG_fTy3Weekday6Mask))
// So
#define ParamLOG_fTy3Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday7)) & LOG_fTy3Weekday7Mask))
// Tag
#define ParamLOG_fTy3Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Day)) & LOG_fTy3DayMask) >> LOG_fTy3DayShift)
// Wochentag
#define ParamLOG_fTy3IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3IsWeekday)) & LOG_fTy3IsWeekdayMask))
// Monat
#define ParamLOG_fTy3Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Month)) & LOG_fTy3MonthMask) >> LOG_fTy3MonthShift)
// Mo
#define ParamLOG_fTy4Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday1)) & LOG_fTy4Weekday1Mask))
// Di
#define ParamLOG_fTy4Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday2)) & LOG_fTy4Weekday2Mask))
// Mi
#define ParamLOG_fTy4Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday3)) & LOG_fTy4Weekday3Mask))
// Do
#define ParamLOG_fTy4Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday4)) & LOG_fTy4Weekday4Mask))
// Fr
#define ParamLOG_fTy4Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday5)) & LOG_fTy4Weekday5Mask))
// Sa
#define ParamLOG_fTy4Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday6)) & LOG_fTy4Weekday6Mask))
// So
#define ParamLOG_fTy4Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday7)) & LOG_fTy4Weekday7Mask))
// Tag
#define ParamLOG_fTy4Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Day)) & LOG_fTy4DayMask) >> LOG_fTy4DayShift)
// Wochentag
#define ParamLOG_fTy4IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4IsWeekday)) & LOG_fTy4IsWeekdayMask))
// Monat
#define ParamLOG_fTy4Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Month)) & LOG_fTy4MonthMask) >> LOG_fTy4MonthShift)
// Interner Eingang 3
#define ParamLOG_fI1                                 ((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1)) & LOG_fI1Mask) >> LOG_fI1Shift)
// Art der Verknüpfung
#define ParamLOG_fI1Kind                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Kind)) & LOG_fI1KindMask) >> LOG_fI1KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI1AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI1AsTrigger)) & LOG_fI1AsTriggerMask))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI1FunctionRel)))
// Interner Eingang 4
#define ParamLOG_fI2                                 ((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2)) & LOG_fI2Mask) >> LOG_fI2Shift)
// Art der Verknüpfung
#define ParamLOG_fI2Kind                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Kind)) & LOG_fI2KindMask) >> LOG_fI2KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI2AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI2AsTrigger)) & LOG_fI2AsTriggerMask))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI2FunctionRel)))
// Zeit für Treppenlicht
#define ParamLOG_fOStairtimeBase                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOStairtimeBase)) & LOG_fOStairtimeBaseMask) >> LOG_fOStairtimeBaseShift)
// Zeit für Treppenlicht
#define ParamLOG_fOStairtimeTime                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fOStairtimeTime)) & LOG_fOStairtimeTimeMask)
// Zeit für Treppenlicht (in Millisekunden)
#define ParamLOG_fOStairtimeTimeMS                   (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fOStairtimeTime))))
// Treppenlicht blinkt im Rhythmus
#define ParamLOG_fOBlinkBase                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOBlinkBase)) & LOG_fOBlinkBaseMask) >> LOG_fOBlinkBaseShift)
// Treppenlicht blinkt im Rhythmus
#define ParamLOG_fOBlinkTime                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fOBlinkTime)) & LOG_fOBlinkTimeMask)
// Treppenlicht blinkt im Rhythmus (in Millisekunden)
#define ParamLOG_fOBlinkTimeMS                       (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fOBlinkTime))))
// EINschalten wird verzögert um
#define ParamLOG_fODelayOnBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnBase)) & LOG_fODelayOnBaseMask) >> LOG_fODelayOnBaseShift)
// EINschalten wird verzögert um
#define ParamLOG_fODelayOnTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOnTime)) & LOG_fODelayOnTimeMask)
// EINschalten wird verzögert um (in Millisekunden)
#define ParamLOG_fODelayOnTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOnTime))))
// AUSschalten wird verzögert um
#define ParamLOG_fODelayOffBase                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffBase)) & LOG_fODelayOffBaseMask) >> LOG_fODelayOffBaseShift)
// AUSschalten wird verzögert um
#define ParamLOG_fODelayOffTime                      (knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOffTime)) & LOG_fODelayOffTimeMask)
// AUSschalten wird verzögert um (in Millisekunden)
#define ParamLOG_fODelayOffTimeMS                    (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOffTime))))
// EIN-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOnBase                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeatOnBase)) & LOG_fORepeatOnBaseMask) >> LOG_fORepeatOnBaseShift)
// EIN-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOnTime                      (knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOnTime)) & LOG_fORepeatOnTimeMask)
// EIN-Telegramm wird wiederholt alle (in Millisekunden)
#define ParamLOG_fORepeatOnTimeMS                    (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOnTime))))
// AUS-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOffBase                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeatOffBase)) & LOG_fORepeatOffBaseMask) >> LOG_fORepeatOffBaseShift)
// AUS-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOffTime                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOffTime)) & LOG_fORepeatOffTimeMask)
// AUS-Telegramm wird wiederholt alle (in Millisekunden)
#define ParamLOG_fORepeatOffTimeMS                   (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOffTime))))
// Ausgang schaltet zeitverzögert
#define ParamLOG_fODelay                             ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODelay)) & LOG_fODelayMask))
// Erneutes EIN führt zu
#define ParamLOG_fODelayOnRepeat                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnRepeat)) & LOG_fODelayOnRepeatMask) >> LOG_fODelayOnRepeatShift)
// Darauffolgendes AUS führt zu
#define ParamLOG_fODelayOnReset                      ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnReset)) & LOG_fODelayOnResetMask))
// Erneutes AUS führt zu
#define ParamLOG_fODelayOffRepeat                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffRepeat)) & LOG_fODelayOffRepeatMask) >> LOG_fODelayOffRepeatShift)
// Darauffolgendes EIN führt zu
#define ParamLOG_fODelayOffReset                     ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffReset)) & LOG_fODelayOffResetMask))
// Ausgang hat eine Treppenlichtfunktion
#define ParamLOG_fOStair                             ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStair)) & LOG_fOStairMask))
// Treppenlicht kann verlängert werden
#define ParamLOG_fORetrigger                         ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORetrigger)) & LOG_fORetriggerMask))
// Treppenlicht kann ausgeschaltet werden
#define ParamLOG_fOStairOff                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStairOff)) & LOG_fOStairOffMask))
// Ausgang wiederholt zyklisch
#define ParamLOG_fORepeat                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeat)) & LOG_fORepeatMask))
// Wiederholungsfilter
#define ParamLOG_fOOutputFilter                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOutputFilter)) & LOG_fOOutputFilterMask) >> LOG_fOOutputFilterShift)
// Sendeverhalten für Ausgang
#define ParamLOG_fOSendOnChange                      ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOSendOnChange)) & LOG_fOSendOnChangeMask))
// DPT für Ausgang
#define ParamLOG_fODpt                               (knx.paramByte(LOG_ParamCalcIndex(LOG_fODpt)))
// Wert für EIN senden?
#define ParamLOG_fOOn                                (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOn)))
// Wert für EIN senden?
#define ParamLOG_fOOnBuzzer                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnBuzzer)))
// Wert für EIN senden?
#define ParamLOG_fOOnLed                             (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnLed)))
// Wert für EIN senden?
#define ParamLOG_fOOnAll                             (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnAll)))
//     Wert für EIN senden als
#define ParamLOG_fOOnTone                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnTone)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt1                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt1)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt2                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt2)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt3Dir                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt3Dir)) & LOG_fOOnDpt3DirMask) >> LOG_fOOnDpt3DirShift)
// 
#define ParamLOG_fOOnDpt3Dim                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt3Dim)) & LOG_fOOnDpt3DimMask)
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt5                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt5)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt5001                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt5001)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt6                            ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt6)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt7                            (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnDpt7)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt8                            ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnDpt8)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt9                            (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOnDpt9), Float_Enc_IEEE754Single))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt12                           (knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnDpt12)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt13                           ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnDpt13)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt14                           (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOnDpt14), Float_Enc_IEEE754Single))
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt16                           (knx.paramData(LOG_ParamCalcIndex(LOG_fOOnDpt16)))
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt17                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt17)))
//     Wert für EIN senden als (3-Byte-RGB)
#define ParamLOG_fOOnRGB                             ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnRGB)) & LOG_fOOnRGBMask) >> LOG_fOOnRGBShift)
// 
#define ParamLOG_fOOnPAArea                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPAArea)) & LOG_fOOnPAAreaMask) >> LOG_fOOnPAAreaShift)
// 
#define ParamLOG_fOOnPALine                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPALine)) & LOG_fOOnPALineMask)
// 
#define ParamLOG_fOOnPADevice                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPADevice)))
//     Wert für EIN ermitteln als
#define ParamLOG_fOOnFunction                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKOKind                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOKind)) & LOG_fOOnKOKindMask) >> LOG_fOOnKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumber                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumberRel                     ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOnKODpt                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKODpt)))
//     Wert für EIN an ein zusätzliches    KO senden?
#define ParamLOG_fOOnKOSend                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOSend)) & LOG_fOOnKOSendMask) >> LOG_fOOnKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumber                    (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumberRel                 ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumberRel)))
// Wert für AUS senden?
#define ParamLOG_fOOff                               (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOff)))
// Wert für AUS senden?
#define ParamLOG_fOOffBuzzer                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffBuzzer)))
// Wert für AUS senden?
#define ParamLOG_fOOffLed                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffLed)))
// Wert für AUS senden?
#define ParamLOG_fOOffAll                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffAll)))
//     Wert für AUS senden als
#define ParamLOG_fOOffTone                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffTone)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt1                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt1)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt2                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt2)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt3Dir                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt3Dir)) & LOG_fOOffDpt3DirMask) >> LOG_fOOffDpt3DirShift)
// 
#define ParamLOG_fOOffDpt3Dim                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt3Dim)) & LOG_fOOffDpt3DimMask)
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt5                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt5)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt5001                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt5001)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt6                           ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt6)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt7                           (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffDpt7)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt8                           ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffDpt8)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt9                           (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOffDpt9), Float_Enc_IEEE754Single))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt12                          (knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffDpt12)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt13                          ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffDpt13)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt14                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOffDpt14), Float_Enc_IEEE754Single))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt16                          (knx.paramData(LOG_ParamCalcIndex(LOG_fOOffDpt16)))
//     Wert für AUS senden als 
#define ParamLOG_fOOffDpt17                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt17)))
//     Wert für AUS senden als (3-Byte-RGB)
#define ParamLOG_fOOffRGB                            ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffRGB)) & LOG_fOOffRGBMask) >> LOG_fOOffRGBShift)
// 
#define ParamLOG_fOOffPAArea                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPAArea)) & LOG_fOOffPAAreaMask) >> LOG_fOOffPAAreaShift)
// 
#define ParamLOG_fOOffPALine                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPALine)) & LOG_fOOffPALineMask)
// 
#define ParamLOG_fOOffPADevice                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPADevice)))
//     Wert für AUS ermitteln als
#define ParamLOG_fOOffFunction                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKOKind                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOKind)) & LOG_fOOffKOKindMask) >> LOG_fOOffKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumber                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumberRel                    ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOffKODpt                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKODpt)))
//     Wert für AUS an ein zusätzliches    KO senden?
#define ParamLOG_fOOffKOSend                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOSend)) & LOG_fOOffKOSendMask) >> LOG_fOOffKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumber                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumberRel                ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumberRel)))

// deprecated
#define LOG_KoOffset 340

// Communication objects per channel (multiple occurrence)
#define LOG_KoBlockOffset 340
#define LOG_KoBlockSize 3

#define LOG_KoCalcNumber(index) (index + LOG_KoBlockOffset + _channelIndex * LOG_KoBlockSize)
#define LOG_KoCalcIndex(number) ((number >= LOG_KoCalcNumber(0) && number < LOG_KoCalcNumber(LOG_KoBlockSize)) ? (number - LOG_KoBlockOffset) % LOG_KoBlockSize : -1)
#define LOG_KoCalcChannel(number) ((number >= LOG_KoBlockOffset && number < LOG_KoBlockOffset + LOG_ChannelCount * LOG_KoBlockSize) ? (number - LOG_KoBlockOffset) / LOG_KoBlockSize : -1)

#define LOG_KoKOfE1 0
#define LOG_KoKOfE2 1
#define LOG_KoKOfO 2

// Eingang 1
#define KoLOG_KOfE1                               (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfE1)))
// Eingang 2
#define KoLOG_KOfE2                               (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfE2)))
// Ausgang
#define KoLOG_KOfO                                (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfO)))

#define NEO_HCLtype                             4939      // 3 Bits, Bit 7-5
#define     NEO_HCLtypeMask 0xE0
#define     NEO_HCLtypeShift 5
#define NEO_HCLStartHour                        4940      // 5 Bits, Bit 7-3
#define     NEO_HCLStartHourMask 0xF8
#define     NEO_HCLStartHourShift 3
#define NEO_HCLStartMinute                      4941      // 6 Bits, Bit 7-2
#define     NEO_HCLStartMinuteMask 0xFC
#define     NEO_HCLStartMinuteShift 2
#define NEO_HCLEndHour                          4942      // 5 Bits, Bit 7-3
#define     NEO_HCLEndHourMask 0xF8
#define     NEO_HCLEndHourShift 3
#define NEO_HCLEndMinute                        4943      // 6 Bits, Bit 7-2
#define     NEO_HCLEndMinuteMask 0xFC
#define     NEO_HCLEndMinuteShift 2
#define NEO_HCLoffsetSunrise                    4944      // 10 Bits, Bit 15-6
#define     NEO_HCLoffsetSunriseMask 0xFFC0
#define     NEO_HCLoffsetSunriseShift 6
#define NEO_HclApplyMode                        4945      // 3 Bits, Bit 4-2
#define     NEO_HclApplyModeMask 0x1C
#define     NEO_HclApplyModeShift 2
#define NEO_HCLoffsetSunset                     4946      // 10 Bits, Bit 15-6
#define     NEO_HCLoffsetSunsetMask 0xFFC0
#define     NEO_HCLoffsetSunsetShift 6
#define NEO_HCLPreserveCurve                    4947      // 3 Bits, Bit 3-1
#define     NEO_HCLPreserveCurveMask 0x0E
#define     NEO_HCLPreserveCurveShift 1
#define NEO_HCLSatThreshold                     4948      // uint8_t
#define NEO_HCLminKelvin                        4949      // 14 Bits, Bit 15-2
#define     NEO_HCLminKelvinMask 0xFFFC
#define     NEO_HCLminKelvinShift 2
#define NEO_HCLmaxKelvin                        4951      // 14 Bits, Bit 15-2
#define     NEO_HCLmaxKelvinMask 0xFFFC
#define     NEO_HCLmaxKelvinShift 2
#define NEO_HCLStrength                         4953      // 7 Bits, Bit 7-1
#define     NEO_HCLStrengthMask 0xFE
#define     NEO_HCLStrengthShift 1
#define NEO_HCLTransitionTime                   4954      // uint8_t
#define NEO_HCLBrightnessCompensation           4955      // 7 Bits, Bit 7-1
#define     NEO_HCLBrightnessCompensationMask 0xFE
#define     NEO_HCLBrightnessCompensationShift 1
#define NEO_HCLWhiteMix                         4956      // 7 Bits, Bit 7-1
#define     NEO_HCLWhiteMixMask 0xFE
#define     NEO_HCLWhiteMixShift 1
#define NEO_NEONumberOfLEDStrips                4959      // 4 Bits, Bit 7-4
#define     NEO_NEONumberOfLEDStripsMask 0xF0
#define     NEO_NEONumberOfLEDStripsShift 4
#define NEO_NEONumberOfSegments                 4960      // 5 Bits, Bit 7-3
#define     NEO_NEONumberOfSegmentsMask 0xF8
#define     NEO_NEONumberOfSegmentsShift 3
#define NEO_NEOTurnOffBeforeRestart             4960      // 1 Bit, Bit 2
#define     NEO_NEOTurnOffBeforeRestartMask 0x04
#define     NEO_NEOTurnOffBeforeRestartShift 2
#define NEO_NEOUpdateSpeed                      4961      // 8 Bits, Bit 7-0
#define NEO_NEOGlobalStartupBehavior            4962      // 2 Bits, Bit 7-6
#define     NEO_NEOGlobalStartupBehaviorMask 0xC0
#define     NEO_NEOGlobalStartupBehaviorShift 6
#define NEO_NEOGlobalStartupR                   4963      // uint8_t
#define NEO_NEOGlobalStartupG                   4964      // uint8_t
#define NEO_NEOGlobalStartupB                   4965      // uint8_t
#define NEO_NEOGlobalStartupW                   4966      // uint8_t
#define NEO_NEOGlobalStartupBrightness          4967      // uint8_t
#define NEO_NEOGlobalStartupEffect              4968      // 8 Bits, Bit 7-0
#define NEO_VirtualStripPos1                    4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1Mask 0xF0
#define     NEO_VirtualStripPos1Shift 4
#define NEO_VirtualStripPos2                    4969      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2Mask 0x0F
#define     NEO_VirtualStripPos2Shift 0
#define NEO_VirtualStripPos3                    4970      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3Mask 0xF0
#define     NEO_VirtualStripPos3Shift 4
#define NEO_VirtualStripPos4                    4970      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4Mask 0x0F
#define     NEO_VirtualStripPos4Shift 0
#define NEO_VirtualStripPos5                    4971      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5Mask 0xF0
#define     NEO_VirtualStripPos5Shift 4
#define NEO_VirtualStripPos6                    4971      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos6Mask 0x0F
#define     NEO_VirtualStripPos6Shift 0
#define NEO_VirtualStripPos1_1                  4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_1Mask 0xF0
#define     NEO_VirtualStripPos1_1Shift 4
#define NEO_VirtualStripPos1_2                  4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_2Mask 0xF0
#define     NEO_VirtualStripPos1_2Shift 4
#define NEO_VirtualStripPos1_3                  4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_3Mask 0xF0
#define     NEO_VirtualStripPos1_3Shift 4
#define NEO_VirtualStripPos1_4                  4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_4Mask 0xF0
#define     NEO_VirtualStripPos1_4Shift 4
#define NEO_VirtualStripPos1_5                  4969      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_5Mask 0xF0
#define     NEO_VirtualStripPos1_5Shift 4
#define NEO_VirtualStripPos2_2                  4969      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_2Mask 0x0F
#define     NEO_VirtualStripPos2_2Shift 0
#define NEO_VirtualStripPos2_3                  4969      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_3Mask 0x0F
#define     NEO_VirtualStripPos2_3Shift 0
#define NEO_VirtualStripPos2_4                  4969      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_4Mask 0x0F
#define     NEO_VirtualStripPos2_4Shift 0
#define NEO_VirtualStripPos2_5                  4969      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_5Mask 0x0F
#define     NEO_VirtualStripPos2_5Shift 0
#define NEO_VirtualStripPos3_3                  4970      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_3Mask 0xF0
#define     NEO_VirtualStripPos3_3Shift 4
#define NEO_VirtualStripPos3_4                  4970      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_4Mask 0xF0
#define     NEO_VirtualStripPos3_4Shift 4
#define NEO_VirtualStripPos3_5                  4970      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_5Mask 0xF0
#define     NEO_VirtualStripPos3_5Shift 4
#define NEO_VirtualStripPos4_4                  4970      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_4Mask 0x0F
#define     NEO_VirtualStripPos4_4Shift 0
#define NEO_VirtualStripPos4_5                  4970      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_5Mask 0x0F
#define     NEO_VirtualStripPos4_5Shift 0
#define NEO_VirtualStripPos5_5                  4971      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5_5Mask 0xF0
#define     NEO_VirtualStripPos5_5Shift 4
#define NEO_VirtualStripHasDuplicates           4972      // 1 Bit, Bit 7
#define     NEO_VirtualStripHasDuplicatesMask 0x80
#define     NEO_VirtualStripHasDuplicatesShift 7
#define NEO_SegmentStartEndError                4972      // 1 Bit, Bit 6
#define     NEO_SegmentStartEndErrorMask 0x40
#define     NEO_SegmentStartEndErrorShift 6
#define NEO_VirtualStripStart1                  4974      // uint16_t
#define NEO_VirtualStripStart2                  4976      // uint16_t
#define NEO_VirtualStripStart3                  4978      // uint16_t
#define NEO_VirtualStripStart4                  4980      // uint16_t
#define NEO_VirtualStripStart5                  4982      // uint16_t
#define NEO_VirtualStripStart6                  4984      // uint16_t

// HCL Kurve Typ
#define ParamNEO_HCLtype                             ((knx.paramByte(NEO_HCLtype) & NEO_HCLtypeMask) >> NEO_HCLtypeShift)
// HCL Start (Stunde)
#define ParamNEO_HCLStartHour                        ((knx.paramByte(NEO_HCLStartHour) & NEO_HCLStartHourMask) >> NEO_HCLStartHourShift)
// HCL Start (Minute)
#define ParamNEO_HCLStartMinute                      ((knx.paramByte(NEO_HCLStartMinute) & NEO_HCLStartMinuteMask) >> NEO_HCLStartMinuteShift)
// HCL Ende (Stunde)
#define ParamNEO_HCLEndHour                          ((knx.paramByte(NEO_HCLEndHour) & NEO_HCLEndHourMask) >> NEO_HCLEndHourShift)
// HCL Ende (Minute)
#define ParamNEO_HCLEndMinute                        ((knx.paramByte(NEO_HCLEndMinute) & NEO_HCLEndMinuteMask) >> NEO_HCLEndMinuteShift)
// Verschiebe Sonnenaufgang
#define ParamNEO_HCLoffsetSunrise                    (((int16_t)knx.paramWord(NEO_HCLoffsetSunrise) & NEO_HCLoffsetSunriseMask) >> NEO_HCLoffsetSunriseShift)
// HCL Anwendungsmodus
#define ParamNEO_HclApplyMode                        ((knx.paramByte(NEO_HclApplyMode) & NEO_HclApplyModeMask) >> NEO_HclApplyModeShift)
// Verschiebe Sonnenuntergang
#define ParamNEO_HCLoffsetSunset                     (((int16_t)knx.paramWord(NEO_HCLoffsetSunset) & NEO_HCLoffsetSunsetMask) >> NEO_HCLoffsetSunsetShift)
// Form der Abblendung
#define ParamNEO_HCLPreserveCurve                    ((knx.paramByte(NEO_HCLPreserveCurve) & NEO_HCLPreserveCurveMask) >> NEO_HCLPreserveCurveShift)
// Sättigungs Schwelle
#define ParamNEO_HCLSatThreshold                     (knx.paramByte(NEO_HCLSatThreshold))
// Farbtemperatur (Min)
#define ParamNEO_HCLminKelvin                        ((knx.paramWord(NEO_HCLminKelvin) & NEO_HCLminKelvinMask) >> NEO_HCLminKelvinShift)
// Farbtemperatur (Max)
#define ParamNEO_HCLmaxKelvin                        ((knx.paramWord(NEO_HCLmaxKelvin) & NEO_HCLmaxKelvinMask) >> NEO_HCLmaxKelvinShift)
// HCL Strength / Mix
#define ParamNEO_HCLStrength                         ((knx.paramByte(NEO_HCLStrength) & NEO_HCLStrengthMask) >> NEO_HCLStrengthShift)
// Kelvin Slew / Transition Time
#define ParamNEO_HCLTransitionTime                   (knx.paramByte(NEO_HCLTransitionTime))
// HCL Helligkeitskompensation
#define ParamNEO_HCLBrightnessCompensation           ((knx.paramByte(NEO_HCLBrightnessCompensation) & NEO_HCLBrightnessCompensationMask) >> NEO_HCLBrightnessCompensationShift)
// HCL Weiß Mix
#define ParamNEO_HCLWhiteMix                         ((knx.paramByte(NEO_HCLWhiteMix) & NEO_HCLWhiteMixMask) >> NEO_HCLWhiteMixShift)
// Anzahl der LED-Streifen
#define ParamNEO_NEONumberOfLEDStrips                ((knx.paramByte(NEO_NEONumberOfLEDStrips) & NEO_NEONumberOfLEDStripsMask) >> NEO_NEONumberOfLEDStripsShift)
// Anzahl der Segmente
#define ParamNEO_NEONumberOfSegments                 ((knx.paramByte(NEO_NEONumberOfSegments) & NEO_NEONumberOfSegmentsMask) >> NEO_NEONumberOfSegmentsShift)
// LEDs vor Upload/Neustart ausschalten
#define ParamNEO_NEOTurnOffBeforeRestart             ((bool)(knx.paramByte(NEO_NEOTurnOffBeforeRestart) & NEO_NEOTurnOffBeforeRestartMask))
// Update Speed
#define ParamNEO_NEOUpdateSpeed                      (knx.paramByte(NEO_NEOUpdateSpeed))
// Verhalten nach Neustart (Global)
#define ParamNEO_NEOGlobalStartupBehavior            ((knx.paramByte(NEO_NEOGlobalStartupBehavior) & NEO_NEOGlobalStartupBehaviorMask) >> NEO_NEOGlobalStartupBehaviorShift)
// Standard-Farbe R
#define ParamNEO_NEOGlobalStartupR                   (knx.paramByte(NEO_NEOGlobalStartupR))
// Standard-Farbe G
#define ParamNEO_NEOGlobalStartupG                   (knx.paramByte(NEO_NEOGlobalStartupG))
// Standard-Farbe B
#define ParamNEO_NEOGlobalStartupB                   (knx.paramByte(NEO_NEOGlobalStartupB))
// Standard-Farbe W
#define ParamNEO_NEOGlobalStartupW                   (knx.paramByte(NEO_NEOGlobalStartupW))
// Standard-Helligkeit
#define ParamNEO_NEOGlobalStartupBrightness          (knx.paramByte(NEO_NEOGlobalStartupBrightness))
// Standard-Effekt
#define ParamNEO_NEOGlobalStartupEffect              (knx.paramByte(NEO_NEOGlobalStartupEffect))
// Position 1
#define ParamNEO_VirtualStripPos1                    ((knx.paramByte(NEO_VirtualStripPos1) & NEO_VirtualStripPos1Mask) >> NEO_VirtualStripPos1Shift)
// Position 2
#define ParamNEO_VirtualStripPos2                    (knx.paramByte(NEO_VirtualStripPos2) & NEO_VirtualStripPos2Mask)
// Position 3
#define ParamNEO_VirtualStripPos3                    ((knx.paramByte(NEO_VirtualStripPos3) & NEO_VirtualStripPos3Mask) >> NEO_VirtualStripPos3Shift)
// Position 4
#define ParamNEO_VirtualStripPos4                    (knx.paramByte(NEO_VirtualStripPos4) & NEO_VirtualStripPos4Mask)
// Position 5
#define ParamNEO_VirtualStripPos5                    ((knx.paramByte(NEO_VirtualStripPos5) & NEO_VirtualStripPos5Mask) >> NEO_VirtualStripPos5Shift)
// Position 6
#define ParamNEO_VirtualStripPos6                    (knx.paramByte(NEO_VirtualStripPos6) & NEO_VirtualStripPos6Mask)
// Position 1
#define ParamNEO_VirtualStripPos1_1                  ((knx.paramByte(NEO_VirtualStripPos1_1) & NEO_VirtualStripPos1_1Mask) >> NEO_VirtualStripPos1_1Shift)
// Position 1
#define ParamNEO_VirtualStripPos1_2                  ((knx.paramByte(NEO_VirtualStripPos1_2) & NEO_VirtualStripPos1_2Mask) >> NEO_VirtualStripPos1_2Shift)
// Position 1
#define ParamNEO_VirtualStripPos1_3                  ((knx.paramByte(NEO_VirtualStripPos1_3) & NEO_VirtualStripPos1_3Mask) >> NEO_VirtualStripPos1_3Shift)
// Position 1
#define ParamNEO_VirtualStripPos1_4                  ((knx.paramByte(NEO_VirtualStripPos1_4) & NEO_VirtualStripPos1_4Mask) >> NEO_VirtualStripPos1_4Shift)
// Position 1
#define ParamNEO_VirtualStripPos1_5                  ((knx.paramByte(NEO_VirtualStripPos1_5) & NEO_VirtualStripPos1_5Mask) >> NEO_VirtualStripPos1_5Shift)
// Position 2
#define ParamNEO_VirtualStripPos2_2                  (knx.paramByte(NEO_VirtualStripPos2_2) & NEO_VirtualStripPos2_2Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_3                  (knx.paramByte(NEO_VirtualStripPos2_3) & NEO_VirtualStripPos2_3Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_4                  (knx.paramByte(NEO_VirtualStripPos2_4) & NEO_VirtualStripPos2_4Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_5                  (knx.paramByte(NEO_VirtualStripPos2_5) & NEO_VirtualStripPos2_5Mask)
// Position 3
#define ParamNEO_VirtualStripPos3_3                  ((knx.paramByte(NEO_VirtualStripPos3_3) & NEO_VirtualStripPos3_3Mask) >> NEO_VirtualStripPos3_3Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_4                  ((knx.paramByte(NEO_VirtualStripPos3_4) & NEO_VirtualStripPos3_4Mask) >> NEO_VirtualStripPos3_4Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_5                  ((knx.paramByte(NEO_VirtualStripPos3_5) & NEO_VirtualStripPos3_5Mask) >> NEO_VirtualStripPos3_5Shift)
// Position 4
#define ParamNEO_VirtualStripPos4_4                  (knx.paramByte(NEO_VirtualStripPos4_4) & NEO_VirtualStripPos4_4Mask)
// Position 4
#define ParamNEO_VirtualStripPos4_5                  (knx.paramByte(NEO_VirtualStripPos4_5) & NEO_VirtualStripPos4_5Mask)
// Position 5
#define ParamNEO_VirtualStripPos5_5                  ((knx.paramByte(NEO_VirtualStripPos5_5) & NEO_VirtualStripPos5_5Mask) >> NEO_VirtualStripPos5_5Shift)
// Konfiguration besitzt Duplikate: 
#define ParamNEO_VirtualStripHasDuplicates           ((bool)(knx.paramByte(NEO_VirtualStripHasDuplicates) & NEO_VirtualStripHasDuplicatesMask))
// Segment Start/End Fehler: 
#define ParamNEO_SegmentStartEndError                ((bool)(knx.paramByte(NEO_SegmentStartEndError) & NEO_SegmentStartEndErrorMask))
// Start LED Position 1
#define ParamNEO_VirtualStripStart1                  (knx.paramWord(NEO_VirtualStripStart1))
// Start LED Position 2
#define ParamNEO_VirtualStripStart2                  (knx.paramWord(NEO_VirtualStripStart2))
// Start LED Position 3
#define ParamNEO_VirtualStripStart3                  (knx.paramWord(NEO_VirtualStripStart3))
// Start LED Position 4
#define ParamNEO_VirtualStripStart4                  (knx.paramWord(NEO_VirtualStripStart4))
// Start LED Position 5
#define ParamNEO_VirtualStripStart5                  (knx.paramWord(NEO_VirtualStripStart5))
// Start LED Position 6
#define ParamNEO_VirtualStripStart6                  (knx.paramWord(NEO_VirtualStripStart6))

#define NEO_KoPower 400
#define NEO_KoPowerState 401
#define NEO_KoHCLState 402
#define NEO_KoBrightness 403
#define NEO_KoBrightnessState 404

// Ein
#define KoNEO_Power                               (knx.getGroupObject(NEO_KoPower))
// Ein Status
#define KoNEO_PowerState                          (knx.getGroupObject(NEO_KoPowerState))
// HCL Zustand
#define KoNEO_HCLState                            (knx.getGroupObject(NEO_KoHCLState))
// Helligkeit
#define KoNEO_Brightness                          (knx.getGroupObject(NEO_KoBrightness))
// Helligkeit Status
#define KoNEO_BrightnessState                     (knx.getGroupObject(NEO_KoBrightnessState))

#define NEO_ChannelCount 16

// Parameter per channel
#define NEO_ParamBlockOffset 4986
#define NEO_ParamBlockSize 140
#define NEO_ParamCalcIndex(index) (index + NEO_ParamBlockOffset + _channelIndex * NEO_ParamBlockSize)

#define NEO_NEOSegmentStart                      0      // 14 Bits, Bit 15-2
#define     NEO_NEOSegmentStartMask 0xFFFC
#define     NEO_NEOSegmentStartShift 2
#define NEO_NEOSegmentEnd                        2      // 14 Bits, Bit 15-2
#define     NEO_NEOSegmentEndMask 0xFFFC
#define     NEO_NEOSegmentEndShift 2
#define NEO_NEOSegmentOffset                     4      // uint16_t
#define NEO_NEOSegmentGrouping                   6      // uint16_t
#define NEO_NEOSegmentSpacing                    8      // uint16_t
#define NEO_NEOSegmentReverseDirection          10      // 1 Bit, Bit 7
#define     NEO_NEOSegmentReverseDirectionMask 0x80
#define     NEO_NEOSegmentReverseDirectionShift 7
#define NEO_NEOSegmentMirrorEffect              10      // 1 Bit, Bit 6
#define     NEO_NEOSegmentMirrorEffectMask 0x40
#define     NEO_NEOSegmentMirrorEffectShift 6
#define NEO_NEOSegmentStartupBehavior           10      // 3 Bits, Bit 5-3
#define     NEO_NEOSegmentStartupBehaviorMask 0x38
#define     NEO_NEOSegmentStartupBehaviorShift 3
#define NEO_NEONEOEffectType                    11      // 8 Bits, Bit 7-0
#define NEO_NEOSegmentStartupR                  24      // uint8_t
#define NEO_NEOSegmentStartupG                  25      // uint8_t
#define NEO_NEOSegmentStartupB                  26      // uint8_t
#define NEO_NEOSegmentStartupW                  27      // uint8_t
#define NEO_NEOSegmentStartupBrightness         28      // uint8_t
#define NEO_NEOSegmentStartupEffect             29      // 8 Bits, Bit 7-0
#define NEO_NEOWipeSpeed                        30      // uint8_t
#define NEO_NEOWipeDirection                    31      // uint8_t
#define NEO_NEORainbowSpeed                     32      // uint8_t
#define NEO_NEORainbowDelta                     33      // uint8_t
#define NEO_NEORainbowCycleSpeed                34      // uint8_t
#define NEO_NEORainbowCycleSaturation           35      // uint8_t
#define NEO_NEORainbowCycleDensity              36      // uint8_t
#define NEO_NEORainbowCycleReverse              37      // 1 Bit, Bit 0
#define     NEO_NEORainbowCycleReverseMask 0x01
#define     NEO_NEORainbowCycleReverseShift 0
#define NEO_NEOPride2015Speed                   38      // uint8_t
#define NEO_NEOConfettiFadeSpeed                39      // uint8_t
#define NEO_NEOConfettiSaturation               40      // uint8_t
#define NEO_NEOJuggleSpeed                      41      // uint8_t
#define NEO_NEOJuggleNumDots                    42      // uint8_t
#define NEO_NEOJuggleFadeSpeed                  43      // uint8_t
#define NEO_NEOJuggleHueOffset                  44      // uint8_t
#define NEO_NEOBPMBPM                           45      // uint8_t
#define NEO_NEOBPMHue                           46      // uint8_t
#define NEO_NEOCylonSpeed                       47      // uint8_t
#define NEO_NEOCylonHue                         48      // uint8_t
#define NEO_NEOCylonEyeSize                     49      // uint8_t
#define NEO_NEOCylonFadeAmount                  50      // uint8_t
#define NEO_NEORGBWTestPhaseDuration            51      // uint8_t
#define NEO_NEOGarageDoorPhase                  52      // uint8_t
#define NEO_NEOGarageDoorArrowSize              53      // uint8_t
#define NEO_NEOGarageDoorArrowSpeed             54      // uint8_t
#define NEO_NEOGarageDoorRunwayGroupSize        55      // uint8_t
#define NEO_NEOGarageDoorRunwaySpeed            56      // uint8_t
#define NEO_NEOGarageDoorBreathingSpeed         57      // uint8_t
#define NEO_NEOGarageDoorOpeningDuration        58      // uint8_t
#define NEO_NEOGarageDoorRunwayDuration         59      // uint8_t
#define NEO_NEOFireSpeed                        60      // uint8_t
#define NEO_NEOFireCooling                      61      // uint8_t
#define NEO_NEOFireSparking                     62      // uint8_t
#define NEO_NEOFireReverseDirection             63      // 1 Bit, Bit 0
#define     NEO_NEOFireReverseDirectionMask 0x01
#define     NEO_NEOFireReverseDirectionShift 0
#define NEO_NEOFireBlueFireMode                 64      // 1 Bit, Bit 0
#define     NEO_NEOFireBlueFireModeMask 0x01
#define     NEO_NEOFireBlueFireModeShift 0
#define NEO_NEOTheaterChaseSpeed                65      // uint8_t
#define NEO_NEOTheaterChaseSpacing              66      // uint8_t
#define NEO_NEOTheaterChaseDotSize              67      // uint8_t
#define NEO_NEOTheaterChaseTrailMode            68      // 1 Bit, Bit 0
#define     NEO_NEOTheaterChaseTrailModeMask 0x01
#define     NEO_NEOTheaterChaseTrailModeShift 0
#define NEO_NEOTheaterChaseRainbowSpeed         69      // uint8_t
#define NEO_NEOTheaterChaseRainbowSpacing       70      // uint8_t
#define NEO_NEOTheaterChaseRainbowDotSize       71      // uint8_t
#define NEO_NEOTheaterChaseRainbowColorSpeed    72      // uint8_t
#define NEO_NEOTheaterChaseRainbowTrailMode     73      // 1 Bit, Bit 0
#define     NEO_NEOTheaterChaseRainbowTrailModeMask 0x01
#define     NEO_NEOTheaterChaseRainbowTrailModeShift 0
#define NEO_NEOSinelonSpeed                     74      // uint8_t
#define NEO_NEOSinelonFadeRate                  75      // uint8_t
#define NEO_NEOSinelonDotSize                   76      // uint8_t
#define NEO_NEOSinelonRainbowMode               77      // 1 Bit, Bit 0
#define     NEO_NEOSinelonRainbowModeMask 0x01
#define     NEO_NEOSinelonRainbowModeShift 0
#define NEO_NEOSinelonBounceMode                78      // 1 Bit, Bit 0
#define     NEO_NEOSinelonBounceModeMask 0x01
#define     NEO_NEOSinelonBounceModeShift 0
#define NEO_NEOTwinkleSpeed                     79      // uint8_t
#define NEO_NEOTwinkleFadeRate                  80      // uint8_t
#define NEO_NEOTwinkleDensity                   81      // uint8_t
#define NEO_NEOTwinkleRainbowMode               82      // 1 Bit, Bit 0
#define     NEO_NEOTwinkleRainbowModeMask 0x01
#define     NEO_NEOTwinkleRainbowModeShift 0
#define NEO_NEOTwinkleVariableBrightness        83      // 1 Bit, Bit 0
#define     NEO_NEOTwinkleVariableBrightnessMask 0x01
#define     NEO_NEOTwinkleVariableBrightnessShift 0
#define NEO_NEOSparkleSpeed                     84      // uint8_t
#define NEO_NEOSparkleFadeRate                  85      // uint8_t
#define NEO_NEOSparkleSparkleCount              86      // uint8_t
#define NEO_NEOSparkleProbability               87      // uint8_t
#define NEO_NEOSparkleWhiteOnly                 88      // 1 Bit, Bit 0
#define     NEO_NEOSparkleWhiteOnlyMask 0x01
#define     NEO_NEOSparkleWhiteOnlyShift 0
#define NEO_NEOSparkleBurstMode                 89      // 1 Bit, Bit 0
#define     NEO_NEOSparkleBurstModeMask 0x01
#define     NEO_NEOSparkleBurstModeShift 0
#define NEO_NEOBreathingSpeed                   90      // uint8_t
#define NEO_NEOBreathingMinBrightness           91      // uint8_t
#define NEO_NEOBreathingCurve                   92      // 1 Bit, Bit 0
#define     NEO_NEOBreathingCurveMask 0x01
#define     NEO_NEOBreathingCurveShift 0
#define NEO_NEOBreathingHoldAtPeak              93      // 1 Bit, Bit 0
#define     NEO_NEOBreathingHoldAtPeakMask 0x01
#define     NEO_NEOBreathingHoldAtPeakShift 0
#define NEO_NEOBreathingRainbowBreathing        94      // uint8_t
#define NEO_NEOStrobeSpeed                      95      // uint8_t
#define NEO_NEOStrobeOnRatio                    96      // uint8_t
#define NEO_NEOStrobeMinBrightness              97      // uint8_t
#define NEO_NEOStrobeRandomTiming               98      // 1 Bit, Bit 0
#define     NEO_NEOStrobeRandomTimingMask 0x01
#define     NEO_NEOStrobeRandomTimingShift 0
#define NEO_NEOStrobeRainbowStrobe              99      // 1 Bit, Bit 0
#define     NEO_NEOStrobeRainbowStrobeMask 0x01
#define     NEO_NEOStrobeRainbowStrobeShift 0
#define NEO_NEOPulseSpeed                       100      // uint8_t
#define NEO_NEOPulsePulseWidth                  101      // uint8_t
#define NEO_NEOPulseGamma                       102      // uint8_t
#define NEO_NEOPulseSharpPulse                  103      // 1 Bit, Bit 0
#define     NEO_NEOPulseSharpPulseMask 0x01
#define     NEO_NEOPulseSharpPulseShift 0
#define NEO_NEOPulseRainbowPulse                104      // 1 Bit, Bit 0
#define     NEO_NEOPulseRainbowPulseMask 0x01
#define     NEO_NEOPulseRainbowPulseShift 0
#define NEO_NEOCometSpeed                       105      // uint8_t
#define NEO_NEOCometFadeRate                    106      // uint8_t
#define NEO_NEOCometTailLength                  107      // uint8_t
#define NEO_NEOCometBounceMode                  108      // 1 Bit, Bit 0
#define     NEO_NEOCometBounceModeMask 0x01
#define     NEO_NEOCometBounceModeShift 0
#define NEO_NEOCometRainbowMode                 109      // 1 Bit, Bit 0
#define     NEO_NEOCometRainbowModeMask 0x01
#define     NEO_NEOCometRainbowModeShift 0
#define NEO_NEOMeteorSpeed                      110      // uint8_t
#define NEO_NEOMeteorMeteorSize                 111      // uint8_t
#define NEO_NEOMeteorFrequency                  112      // uint8_t
#define NEO_NEOMeteorRandomColors               113      // 1 Bit, Bit 0
#define     NEO_NEOMeteorRandomColorsMask 0x01
#define     NEO_NEOMeteorRandomColorsShift 0
#define NEO_NEOMeteorMultiMeteor                114      // 1 Bit, Bit 0
#define     NEO_NEOMeteorMultiMeteorMask 0x01
#define     NEO_NEOMeteorMultiMeteorShift 0
#define NEO_NEONoiseSpeed                       115      // uint8_t
#define NEO_NEONoiseScale                       116      // uint8_t
#define NEO_NEONoiseSaturation                  117      // uint8_t
#define NEO_NEONoiseHueOffset                   118      // uint8_t
#define NEO_NEOPaletteSpeed                     119      // uint8_t
#define NEO_NEOPalettePalette                   120      // uint8_t
#define NEO_NEOPaletteBlend                     121      // uint8_t
#define NEO_NEOPaletteSpacing                   122      // uint8_t
#define NEO_NEOBlitzSpeed                       123      // uint8_t
#define NEO_NEOBlitzWidth                       124      // uint8_t
#define NEO_NEOBlitzDecay                       125      // uint8_t
#define NEO_NEOBlitzHue                         126      // uint8_t
#define NEO_NEOGradientSpeed                    127      // uint8_t
#define NEO_NEOGradientStartHue                 128      // uint8_t
#define NEO_NEOGradientEndHue                   129      // uint8_t
#define NEO_NEOGradientSaturation               130      // uint8_t

// Start LED
#define ParamNEO_NEOSegmentStart                     ((knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentStart)) & NEO_NEOSegmentStartMask) >> NEO_NEOSegmentStartShift)
// Ende LED
#define ParamNEO_NEOSegmentEnd                       ((knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentEnd)) & NEO_NEOSegmentEndMask) >> NEO_NEOSegmentEndShift)
// Offset
#define ParamNEO_NEOSegmentOffset                    (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentOffset)))
// Gruppierung
#define ParamNEO_NEOSegmentGrouping                  (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentGrouping)))
// Spacing
#define ParamNEO_NEOSegmentSpacing                   (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentSpacing)))
// ReverseDirection
#define ParamNEO_NEOSegmentReverseDirection          ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentReverseDirection)) & NEO_NEOSegmentReverseDirectionMask))
// Spiegelung des Effekts
#define ParamNEO_NEOSegmentMirrorEffect              ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentMirrorEffect)) & NEO_NEOSegmentMirrorEffectMask))
// Verhalten nach Neustart
#define ParamNEO_NEOSegmentStartupBehavior           ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupBehavior)) & NEO_NEOSegmentStartupBehaviorMask) >> NEO_NEOSegmentStartupBehaviorShift)
// Effekt Typ
#define ParamNEO_NEONEOEffectType                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONEOEffectType)))
// Standard-Farbe R
#define ParamNEO_NEOSegmentStartupR                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupR)))
// Standard-Farbe G
#define ParamNEO_NEOSegmentStartupG                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupG)))
// Standard-Farbe B
#define ParamNEO_NEOSegmentStartupB                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupB)))
// Standard-Farbe W
#define ParamNEO_NEOSegmentStartupW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupW)))
// Standard-Helligkeit
#define ParamNEO_NEOSegmentStartupBrightness         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupBrightness)))
// Standard-Effekt
#define ParamNEO_NEOSegmentStartupEffect             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupEffect)))
// Speed (Wipe)
#define ParamNEO_NEOWipeSpeed                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOWipeSpeed)))
// Direction (Wipe)
#define ParamNEO_NEOWipeDirection                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOWipeDirection)))
// Speed (Rainbow)
#define ParamNEO_NEORainbowSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowSpeed)))
// Delta (Rainbow)
#define ParamNEO_NEORainbowDelta                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowDelta)))
// Speed (Rainbow Cycle)
#define ParamNEO_NEORainbowCycleSpeed                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowCycleSpeed)))
// Saturation (Rainbow Cycle)
#define ParamNEO_NEORainbowCycleSaturation           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowCycleSaturation)))
// Density (Rainbow Cycle)
#define ParamNEO_NEORainbowCycleDensity              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowCycleDensity)))
// Reverse (Rainbow Cycle)
#define ParamNEO_NEORainbowCycleReverse              ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowCycleReverse)) & NEO_NEORainbowCycleReverseMask))
// Speed (Pride2015)
#define ParamNEO_NEOPride2015Speed                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPride2015Speed)))
// FadeSpeed (Confetti)
#define ParamNEO_NEOConfettiFadeSpeed                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOConfettiFadeSpeed)))
// Saturation (Confetti)
#define ParamNEO_NEOConfettiSaturation               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOConfettiSaturation)))
// Speed (Juggle)
#define ParamNEO_NEOJuggleSpeed                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOJuggleSpeed)))
// NumDots (Juggle)
#define ParamNEO_NEOJuggleNumDots                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOJuggleNumDots)))
// FadeSpeed (Juggle)
#define ParamNEO_NEOJuggleFadeSpeed                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOJuggleFadeSpeed)))
// HueOffset (Juggle)
#define ParamNEO_NEOJuggleHueOffset                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOJuggleHueOffset)))
// BPM (BPM)
#define ParamNEO_NEOBPMBPM                           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBPMBPM)))
// Hue (BPM)
#define ParamNEO_NEOBPMHue                           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBPMHue)))
// Speed (Cylon)
#define ParamNEO_NEOCylonSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonSpeed)))
// Hue (Cylon)
#define ParamNEO_NEOCylonHue                         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonHue)))
// EyeSize (Cylon)
#define ParamNEO_NEOCylonEyeSize                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonEyeSize)))
// FadeAmount (Cylon)
#define ParamNEO_NEOCylonFadeAmount                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonFadeAmount)))
// PhaseDuration (RGBWTest)
#define ParamNEO_NEORGBWTestPhaseDuration            (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORGBWTestPhaseDuration)))
// Phase (GarageDoor)
#define ParamNEO_NEOGarageDoorPhase                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorPhase)))
// ArrowSize (GarageDoor)
#define ParamNEO_NEOGarageDoorArrowSize              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorArrowSize)))
// ArrowSpeed (GarageDoor)
#define ParamNEO_NEOGarageDoorArrowSpeed             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorArrowSpeed)))
// RunwayGroupSize (GarageDoor)
#define ParamNEO_NEOGarageDoorRunwayGroupSize        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorRunwayGroupSize)))
// RunwaySpeed (GarageDoor)
#define ParamNEO_NEOGarageDoorRunwaySpeed            (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorRunwaySpeed)))
// BreathingSpeed (GarageDoor)
#define ParamNEO_NEOGarageDoorBreathingSpeed         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorBreathingSpeed)))
// OpeningDuration (GarageDoor)
#define ParamNEO_NEOGarageDoorOpeningDuration        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorOpeningDuration)))
// RunwayDuration (GarageDoor)
#define ParamNEO_NEOGarageDoorRunwayDuration         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGarageDoorRunwayDuration)))
// Speed (Fire)
#define ParamNEO_NEOFireSpeed                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOFireSpeed)))
// Cooling (Fire)
#define ParamNEO_NEOFireCooling                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOFireCooling)))
// Sparking (Fire)
#define ParamNEO_NEOFireSparking                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOFireSparking)))
// ReverseDirection (Fire)
#define ParamNEO_NEOFireReverseDirection             ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOFireReverseDirection)) & NEO_NEOFireReverseDirectionMask))
// BlueFireMode (Fire)
#define ParamNEO_NEOFireBlueFireMode                 ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOFireBlueFireMode)) & NEO_NEOFireBlueFireModeMask))
// Speed (Theater Chase)
#define ParamNEO_NEOTheaterChaseSpeed                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseSpeed)))
// Spacing (Theater Chase)
#define ParamNEO_NEOTheaterChaseSpacing              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseSpacing)))
// DotSize (Theater Chase)
#define ParamNEO_NEOTheaterChaseDotSize              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseDotSize)))
// TrailMode (Theater Chase)
#define ParamNEO_NEOTheaterChaseTrailMode            ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseTrailMode)) & NEO_NEOTheaterChaseTrailModeMask))
// Speed (Theater Chase Rainbow)
#define ParamNEO_NEOTheaterChaseRainbowSpeed         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseRainbowSpeed)))
// Spacing (Theater Chase Rainbow)
#define ParamNEO_NEOTheaterChaseRainbowSpacing       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseRainbowSpacing)))
// DotSize (Theater Chase Rainbow)
#define ParamNEO_NEOTheaterChaseRainbowDotSize       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseRainbowDotSize)))
// ColorSpeed (Theater Chase Rainbow)
#define ParamNEO_NEOTheaterChaseRainbowColorSpeed    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseRainbowColorSpeed)))
// TrailMode (Theater Chase Rainbow)
#define ParamNEO_NEOTheaterChaseRainbowTrailMode     ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseRainbowTrailMode)) & NEO_NEOTheaterChaseRainbowTrailModeMask))
// Speed (Sinelon)
#define ParamNEO_NEOSinelonSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSinelonSpeed)))
// FadeRate (Sinelon)
#define ParamNEO_NEOSinelonFadeRate                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSinelonFadeRate)))
// DotSize (Sinelon)
#define ParamNEO_NEOSinelonDotSize                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSinelonDotSize)))
// RainbowMode (Sinelon)
#define ParamNEO_NEOSinelonRainbowMode               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSinelonRainbowMode)) & NEO_NEOSinelonRainbowModeMask))
// BounceMode (Sinelon)
#define ParamNEO_NEOSinelonBounceMode                ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSinelonBounceMode)) & NEO_NEOSinelonBounceModeMask))
// Speed (Twinkle)
#define ParamNEO_NEOTwinkleSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTwinkleSpeed)))
// FadeRate (Twinkle)
#define ParamNEO_NEOTwinkleFadeRate                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTwinkleFadeRate)))
// Density (Twinkle)
#define ParamNEO_NEOTwinkleDensity                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTwinkleDensity)))
// RainbowMode (Twinkle)
#define ParamNEO_NEOTwinkleRainbowMode               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTwinkleRainbowMode)) & NEO_NEOTwinkleRainbowModeMask))
// VariableBrightness (Twinkle)
#define ParamNEO_NEOTwinkleVariableBrightness        ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTwinkleVariableBrightness)) & NEO_NEOTwinkleVariableBrightnessMask))
// Speed (Sparkle)
#define ParamNEO_NEOSparkleSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleSpeed)))
// FadeRate (Sparkle)
#define ParamNEO_NEOSparkleFadeRate                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleFadeRate)))
// SparkleCount (Sparkle)
#define ParamNEO_NEOSparkleSparkleCount              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleSparkleCount)))
// Probability (Sparkle)
#define ParamNEO_NEOSparkleProbability               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleProbability)))
// WhiteOnly (Sparkle)
#define ParamNEO_NEOSparkleWhiteOnly                 ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleWhiteOnly)) & NEO_NEOSparkleWhiteOnlyMask))
// BurstMode (Sparkle)
#define ParamNEO_NEOSparkleBurstMode                 ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleBurstMode)) & NEO_NEOSparkleBurstModeMask))
// Speed (Breathing)
#define ParamNEO_NEOBreathingSpeed                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingSpeed)))
// MinBrightness (Breathing)
#define ParamNEO_NEOBreathingMinBrightness           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingMinBrightness)))
// Curve (Breathing)
#define ParamNEO_NEOBreathingCurve                   ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingCurve)) & NEO_NEOBreathingCurveMask))
// HoldAtPeak (Breathing)
#define ParamNEO_NEOBreathingHoldAtPeak              ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingHoldAtPeak)) & NEO_NEOBreathingHoldAtPeakMask))
// RainbowBreathing (Breathing)
#define ParamNEO_NEOBreathingRainbowBreathing        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingRainbowBreathing)))
// Speed (Strobe)
#define ParamNEO_NEOStrobeSpeed                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStrobeSpeed)))
// OnRatio (Strobe)
#define ParamNEO_NEOStrobeOnRatio                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStrobeOnRatio)))
// MinBrightness (Strobe)
#define ParamNEO_NEOStrobeMinBrightness              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStrobeMinBrightness)))
// RandomTiming (Strobe)
#define ParamNEO_NEOStrobeRandomTiming               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStrobeRandomTiming)) & NEO_NEOStrobeRandomTimingMask))
// RainbowStrobe (Strobe)
#define ParamNEO_NEOStrobeRainbowStrobe              ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStrobeRainbowStrobe)) & NEO_NEOStrobeRainbowStrobeMask))
// Speed (Pulse)
#define ParamNEO_NEOPulseSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPulseSpeed)))
// PulseWidth (Pulse)
#define ParamNEO_NEOPulsePulseWidth                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPulsePulseWidth)))
// Gamma (Pulse)
#define ParamNEO_NEOPulseGamma                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPulseGamma)))
// SharpPulse (Pulse)
#define ParamNEO_NEOPulseSharpPulse                  ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPulseSharpPulse)) & NEO_NEOPulseSharpPulseMask))
// RainbowPulse (Pulse)
#define ParamNEO_NEOPulseRainbowPulse                ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPulseRainbowPulse)) & NEO_NEOPulseRainbowPulseMask))
// Speed (Comet)
#define ParamNEO_NEOCometSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometSpeed)))
// FadeRate (Comet)
#define ParamNEO_NEOCometFadeRate                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometFadeRate)))
// TailLength (Comet)
#define ParamNEO_NEOCometTailLength                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometTailLength)))
// BounceMode (Comet)
#define ParamNEO_NEOCometBounceMode                  ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometBounceMode)) & NEO_NEOCometBounceModeMask))
// RainbowMode (Comet)
#define ParamNEO_NEOCometRainbowMode                 ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometRainbowMode)) & NEO_NEOCometRainbowModeMask))
// Speed (Meteor)
#define ParamNEO_NEOMeteorSpeed                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMeteorSpeed)))
// MeteorSize (Meteor)
#define ParamNEO_NEOMeteorMeteorSize                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMeteorMeteorSize)))
// Frequency (Meteor)
#define ParamNEO_NEOMeteorFrequency                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMeteorFrequency)))
// RandomColors (Meteor)
#define ParamNEO_NEOMeteorRandomColors               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMeteorRandomColors)) & NEO_NEOMeteorRandomColorsMask))
// MultiMeteor (Meteor)
#define ParamNEO_NEOMeteorMultiMeteor                ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMeteorMultiMeteor)) & NEO_NEOMeteorMultiMeteorMask))
// Speed (Noise)
#define ParamNEO_NEONoiseSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseSpeed)))
// Scale (Noise)
#define ParamNEO_NEONoiseScale                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseScale)))
// Saturation (Noise)
#define ParamNEO_NEONoiseSaturation                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseSaturation)))
// HueOffset (Noise)
#define ParamNEO_NEONoiseHueOffset                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseHueOffset)))
// Speed (Palette)
#define ParamNEO_NEOPaletteSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPaletteSpeed)))
// Palette (Palette)
#define ParamNEO_NEOPalettePalette                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPalettePalette)))
// Blend (Palette)
#define ParamNEO_NEOPaletteBlend                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPaletteBlend)))
// Spacing (Palette)
#define ParamNEO_NEOPaletteSpacing                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPaletteSpacing)))
// Speed (Blitz)
#define ParamNEO_NEOBlitzSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBlitzSpeed)))
// Width (Blitz)
#define ParamNEO_NEOBlitzWidth                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBlitzWidth)))
// Decay (Blitz)
#define ParamNEO_NEOBlitzDecay                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBlitzDecay)))
// Hue (Blitz)
#define ParamNEO_NEOBlitzHue                         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBlitzHue)))
// Speed (Gradient)
#define ParamNEO_NEOGradientSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientSpeed)))
// StartHue (Gradient)
#define ParamNEO_NEOGradientStartHue                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientStartHue)))
// EndHue (Gradient)
#define ParamNEO_NEOGradientEndHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientEndHue)))
// Saturation (Gradient)
#define ParamNEO_NEOGradientSaturation               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientSaturation)))

// deprecated
#define NEO_KoOffset 600

// Communication objects per channel (multiple occurrence)
#define NEO_KoBlockOffset 600
#define NEO_KoBlockSize 39

#define NEO_KoCalcNumber(index) (index + NEO_KoBlockOffset + _channelIndex * NEO_KoBlockSize)
#define NEO_KoCalcIndex(number) ((number >= NEO_KoCalcNumber(0) && number < NEO_KoCalcNumber(NEO_KoBlockSize)) ? (number - NEO_KoBlockOffset) % NEO_KoBlockSize : -1)
#define NEO_KoCalcChannel(number) ((number >= NEO_KoBlockOffset && number < NEO_KoBlockOffset + NEO_ChannelCount * NEO_KoBlockSize) ? (number - NEO_KoBlockOffset) / NEO_KoBlockSize : -1)

#define NEO_KoSegmentPower 0
#define NEO_KoSegmentPowerState 1
#define NEO_KoSegmentBrightness 2
#define NEO_KoSegmentBrightnessState 3
#define NEO_KoR 4
#define NEO_KoG 5
#define NEO_KoB 6
#define NEO_KoW 7
#define NEO_KoCCT 8
#define NEO_KoCCTState 9
#define NEO_KoWW 10
#define NEO_KoCW 11
#define NEO_KoH 12
#define NEO_KoS 13
#define NEO_KoV 14
#define NEO_KoFx 15
#define NEO_KoFxState 16
#define NEO_KoPreset 17
#define NEO_KoPresetState 18
#define NEO_KoRGB 19
#define NEO_KoRGBState 20
#define NEO_KoHSV 21
#define NEO_KoHSVState 22
#define NEO_KoRGBW 23
#define NEO_KoRGBWState 24
#define NEO_KoBriRel 25
#define NEO_KoRRel 26
#define NEO_KoGRel 27
#define NEO_KoBRel 28
#define NEO_KoWRel 29
#define NEO_KoWWRel 30
#define NEO_KoCWRel 31
#define NEO_KoHRel 32
#define NEO_KoSRel 33
#define NEO_KoVRel 34
#define NEO_KoFxRel 35
#define NEO_KoRGBRel 36
#define NEO_KoHSVRel 37
#define NEO_KoRGBWRel 38

// Segment Ein/Aus
#define KoNEO_SegmentPower                        (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSegmentPower)))
// Segment Ein/Aus Status
#define KoNEO_SegmentPowerState                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSegmentPowerState)))
// Segment Helligkeit
#define KoNEO_SegmentBrightness                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSegmentBrightness)))
// Segment Helligkeit Status
#define KoNEO_SegmentBrightnessState              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSegmentBrightnessState)))
// Rot
#define KoNEO_R                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoR)))
// Grün
#define KoNEO_G                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoG)))
// Blau
#define KoNEO_B                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoB)))
// Weiß
#define KoNEO_W                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoW)))
// Farbtemperatur
#define KoNEO_CCT                                 (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCCT)))
// Farbtemperatur Status
#define KoNEO_CCTState                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCCTState)))
// Warmweiß
#define KoNEO_WW                                  (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoWW)))
// Kaltweiß
#define KoNEO_CW                                  (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCW)))
// Farbton (Hue)
#define KoNEO_H                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoH)))
// Sättigung
#define KoNEO_S                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoS)))
// Helligkeit (Value)
#define KoNEO_V                                   (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoV)))
// Effekt
#define KoNEO_Fx                                  (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoFx)))
// Effekt Status
#define KoNEO_FxState                             (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoFxState)))
// Voreinstellung
#define KoNEO_Preset                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoPreset)))
// Voreinstellung Status
#define KoNEO_PresetState                         (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoPresetState)))
// RGB
#define KoNEO_RGB                                 (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGB)))
// RGB Status
#define KoNEO_RGBState                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBState)))
// HSV
#define KoNEO_HSV                                 (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHSV)))
// HSV Status
#define KoNEO_HSVState                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHSVState)))
// RGBW
#define KoNEO_RGBW                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBW)))
// RGBW Status
#define KoNEO_RGBWState                           (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBWState)))
// Helligkeit Relativ
#define KoNEO_BriRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoBriRel)))
// Rot Relativ
#define KoNEO_RRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRRel)))
// Grün Relativ
#define KoNEO_GRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoGRel)))
// Blau Relativ
#define KoNEO_BRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoBRel)))
// Weiß Relativ
#define KoNEO_WRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoWRel)))
// Warmweiß Relativ
#define KoNEO_WWRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoWWRel)))
// Kaltweiß Relativ
#define KoNEO_CWRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCWRel)))
// Hue Relativ
#define KoNEO_HRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHRel)))
// Sättigung Relativ
#define KoNEO_SRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSRel)))
// Helligkeit (Value) Relativ
#define KoNEO_VRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoVRel)))
// Effekt Relativ
#define KoNEO_FxRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoFxRel)))
// RGB Relativ
#define KoNEO_RGBRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBRel)))
// HSV Relativ
#define KoNEO_HSVRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHSVRel)))
// RGBW Relativ
#define KoNEO_RGBWRel                             (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBWRel)))

#define NEOSTRIP_ChannelCount 6

// Parameter per channel
#define NEOSTRIP_ParamBlockOffset 7226
#define NEOSTRIP_ParamBlockSize 22
#define NEOSTRIP_ParamCalcIndex(index) (index + NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize)

#define NEOSTRIP_NEOLEDType                           0      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOColourOrder                       1      // 3 Bits, Bit 7-5
#define     NEOSTRIP_NEOColourOrderMask 0xE0
#define     NEOSTRIP_NEOColourOrderShift 5
#define NEOSTRIP_NEOSwap                              1      // 3 Bits, Bit 4-2
#define     NEOSTRIP_NEOSwapMask 0x1C
#define     NEOSTRIP_NEOSwapShift 2
#define NEOSTRIP_NEODataGPIO                          2      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEODataGPIOMask 0xFE
#define     NEOSTRIP_NEODataGPIOShift 1
#define NEOSTRIP_NEOClockGPIO                         3      // 6 Bits, Bit 7-2
#define     NEOSTRIP_NEOClockGPIOMask 0xFC
#define     NEOSTRIP_NEOClockGPIOShift 2
#define NEOSTRIP_NEOSPIMOSIGPIO                       4      // 6 Bits, Bit 7-2
#define     NEOSTRIP_NEOSPIMOSIGPIOMask 0xFC
#define     NEOSTRIP_NEOSPIMOSIGPIOShift 2
#define NEOSTRIP_NEOSPICLK                            5      // 3 Bits, Bit 7-5
#define     NEOSTRIP_NEOSPICLKMask 0xE0
#define     NEOSTRIP_NEOSPICLKShift 5
#define NEOSTRIP_NEOSPICLKManual                      5      // 1 Bit, Bit 4
#define     NEOSTRIP_NEOSPICLKManualMask 0x10
#define     NEOSTRIP_NEOSPICLKManualShift 4
#define NEOSTRIP_NEOGPIOManual                        5      // 1 Bit, Bit 3
#define     NEOSTRIP_NEOGPIOManualMask 0x08
#define     NEOSTRIP_NEOGPIOManualShift 3
#define NEOSTRIP_NEOLength                            6      // 14 Bits, Bit 15-2
#define     NEOSTRIP_NEOLengthMask 0xFFFC
#define     NEOSTRIP_NEOLengthShift 2
#define NEOSTRIP_NEOSkipFirstLEDs                     8      // uint16_t
#define NEOSTRIP_NEOpowerLimitEnabled                10      // 1 Bit, Bit 7
#define     NEOSTRIP_NEOpowerLimitEnabledMask 0x80
#define     NEOSTRIP_NEOpowerLimitEnabledShift 7
#define NEOSTRIP_NEOpowerLimitType                   10      // 3 Bits, Bit 6-4
#define     NEOSTRIP_NEOpowerLimitTypeMask 0x70
#define     NEOSTRIP_NEOpowerLimitTypeShift 4
#define NEOSTRIP_NEOTiming                           10      // 4 Bits, Bit 5-2
#define     NEOSTRIP_NEOTimingMask 0x3C
#define     NEOSTRIP_NEOTimingShift 2
#define NEOSTRIP_NEOpowerLimitGlobal                 11      // uint16_t
#define NEOSTRIP_NEOcurrentPerLED                    13      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOcurrentPerLEDMask 0xFE
#define     NEOSTRIP_NEOcurrentPerLEDShift 1
#define NEOSTRIP_NEOcurrentPerChannel                14      // uint16_t
#define NEOSTRIP_NEOautoBrightnessLimit              16      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOautoBrightnessLimitMask 0xFE
#define     NEOSTRIP_NEOautoBrightnessLimitShift 1
#define NEOSTRIP_NEOpowerLimitThreshold              17      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOpowerLimitThresholdMask 0xFE
#define     NEOSTRIP_NEOpowerLimitThresholdShift 1
#define NEOSTRIP_NEOWhiteBalanceCorrection           18      // 1 Bit, Bit 7
#define     NEOSTRIP_NEOWhiteBalanceCorrectionMask 0x80
#define     NEOSTRIP_NEOWhiteBalanceCorrectionShift 7
#define NEOSTRIP_NEOGammaCorrection                  18      // 1 Bit, Bit 6
#define     NEOSTRIP_NEOGammaCorrectionMask 0x40
#define     NEOSTRIP_NEOGammaCorrectionShift 6
#define NEOSTRIP_NEOGammaValue                       18      // 4 Bits, Bit 5-2
#define     NEOSTRIP_NEOGammaValueMask 0x3C
#define     NEOSTRIP_NEOGammaValueShift 2
#define NEOSTRIP_NEOablSlewRatePercent               19      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOablSlewRatePercentMask 0xFE
#define     NEOSTRIP_NEOablSlewRatePercentShift 1
#define NEOSTRIP_NEOWhiteBalanceRed                  20      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOWhiteBalanceRedMask 0xFE
#define     NEOSTRIP_NEOWhiteBalanceRedShift 1
#define NEOSTRIP_NEOWhiteBalanceGreen                21      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOWhiteBalanceGreenMask 0xFE
#define     NEOSTRIP_NEOWhiteBalanceGreenShift 1
#define NEOSTRIP_NEOWhiteBalanceBlue                 22      // 7 Bits, Bit 7-1
#define     NEOSTRIP_NEOWhiteBalanceBlueMask 0xFE
#define     NEOSTRIP_NEOWhiteBalanceBlueShift 1

// LED Typ
#define ParamNEOSTRIP_NEOLEDType                          (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOLEDType)))
// LED Farbordnung
#define ParamNEOSTRIP_NEOColourOrder                      ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOColourOrder)) & NEOSTRIP_NEOColourOrderMask) >> NEOSTRIP_NEOColourOrderShift)
// Swap
#define ParamNEOSTRIP_NEOSwap                             ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSwap)) & NEOSTRIP_NEOSwapMask) >> NEOSTRIP_NEOSwapShift)
// Daten GPIO
#define ParamNEOSTRIP_NEODataGPIO                         ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEODataGPIO)) & NEOSTRIP_NEODataGPIOMask) >> NEOSTRIP_NEODataGPIOShift)
// Clock GPIO
#define ParamNEOSTRIP_NEOClockGPIO                        ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOClockGPIO)) & NEOSTRIP_NEOClockGPIOMask) >> NEOSTRIP_NEOClockGPIOShift)
// MOSI GPIO
#define ParamNEOSTRIP_NEOSPIMOSIGPIO                      ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPIMOSIGPIO)) & NEOSTRIP_NEOSPIMOSIGPIOMask) >> NEOSTRIP_NEOSPIMOSIGPIOShift)
// SPI Clock
#define ParamNEOSTRIP_NEOSPICLK                           ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPICLK)) & NEOSTRIP_NEOSPICLKMask) >> NEOSTRIP_NEOSPICLKShift)
// SPI Takt manuell konfigurieren
#define ParamNEOSTRIP_NEOSPICLKManual                     ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPICLKManual)) & NEOSTRIP_NEOSPICLKManualMask))
// GPIO manuell konfigurieren
#define ParamNEOSTRIP_NEOGPIOManual                       ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOManual)) & NEOSTRIP_NEOGPIOManualMask))
// Anzahl an LEDs/LEDs-Gruppen
#define ParamNEOSTRIP_NEOLength                           ((knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOLength)) & NEOSTRIP_NEOLengthMask) >> NEOSTRIP_NEOLengthShift)
// Überspringe erste LEDs
#define ParamNEOSTRIP_NEOSkipFirstLEDs                    (knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSkipFirstLEDs)))
// Strombegrenzung aktiviert
#define ParamNEOSTRIP_NEOpowerLimitEnabled                ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOpowerLimitEnabled)) & NEOSTRIP_NEOpowerLimitEnabledMask))
// Strombegrenzung Typ
#define ParamNEOSTRIP_NEOpowerLimitType                   ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOpowerLimitType)) & NEOSTRIP_NEOpowerLimitTypeMask) >> NEOSTRIP_NEOpowerLimitTypeShift)
// Timing
#define ParamNEOSTRIP_NEOTiming                           ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOTiming)) & NEOSTRIP_NEOTimingMask) >> NEOSTRIP_NEOTimingShift)
// Globale Strombegrenzung (mA)
#define ParamNEOSTRIP_NEOpowerLimitGlobal                 (knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOpowerLimitGlobal)))
// Strom pro LED (mA)
#define ParamNEOSTRIP_NEOcurrentPerLED                    ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOcurrentPerLED)) & NEOSTRIP_NEOcurrentPerLEDMask) >> NEOSTRIP_NEOcurrentPerLEDShift)
// Strom pro Kanal (mA)
#define ParamNEOSTRIP_NEOcurrentPerChannel                (knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOcurrentPerChannel)))
// Auto-Helligkeitsbegrenzung (%)
#define ParamNEOSTRIP_NEOautoBrightnessLimit              ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOautoBrightnessLimit)) & NEOSTRIP_NEOautoBrightnessLimitMask) >> NEOSTRIP_NEOautoBrightnessLimitShift)
// Strombegrenzung Schwellwert (%)
#define ParamNEOSTRIP_NEOpowerLimitThreshold              ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOpowerLimitThreshold)) & NEOSTRIP_NEOpowerLimitThresholdMask) >> NEOSTRIP_NEOpowerLimitThresholdShift)
// Weißabgleich
#define ParamNEOSTRIP_NEOWhiteBalanceCorrection           ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceCorrection)) & NEOSTRIP_NEOWhiteBalanceCorrectionMask))
// Gamma Korrektur (empfohlen!)
#define ParamNEOSTRIP_NEOGammaCorrection                  ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGammaCorrection)) & NEOSTRIP_NEOGammaCorrectionMask))
// Gamma Wert
#define ParamNEOSTRIP_NEOGammaValue                       ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGammaValue)) & NEOSTRIP_NEOGammaValueMask) >> NEOSTRIP_NEOGammaValueShift)
// Anstiegszeit ABL (%)
#define ParamNEOSTRIP_NEOablSlewRatePercent               ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOablSlewRatePercent)) & NEOSTRIP_NEOablSlewRatePercentMask) >> NEOSTRIP_NEOablSlewRatePercentShift)
// Weißabgleich Rot (%)
#define ParamNEOSTRIP_NEOWhiteBalanceRed                  ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceRed)) & NEOSTRIP_NEOWhiteBalanceRedMask) >> NEOSTRIP_NEOWhiteBalanceRedShift)
// Weißabgleich Grün (%)
#define ParamNEOSTRIP_NEOWhiteBalanceGreen                ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceGreen)) & NEOSTRIP_NEOWhiteBalanceGreenMask) >> NEOSTRIP_NEOWhiteBalanceGreenShift)
// Weißabgleich Blau (%)
#define ParamNEOSTRIP_NEOWhiteBalanceBlue                 ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceBlue)) & NEOSTRIP_NEOWhiteBalanceBlueMask) >> NEOSTRIP_NEOWhiteBalanceBlueShift)

// deprecated
#define NEOSTRIP_KoOffset 500

// Communication objects per channel (multiple occurrence)
#define NEOSTRIP_KoBlockOffset 500
#define NEOSTRIP_KoBlockSize 5

#define NEOSTRIP_KoCalcNumber(index) (index + NEOSTRIP_KoBlockOffset + _channelIndex * NEOSTRIP_KoBlockSize)
#define NEOSTRIP_KoCalcIndex(number) ((number >= NEOSTRIP_KoCalcNumber(0) && number < NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBlockSize)) ? (number - NEOSTRIP_KoBlockOffset) % NEOSTRIP_KoBlockSize : -1)
#define NEOSTRIP_KoCalcChannel(number) ((number >= NEOSTRIP_KoBlockOffset && number < NEOSTRIP_KoBlockOffset + NEOSTRIP_ChannelCount * NEOSTRIP_KoBlockSize) ? (number - NEOSTRIP_KoBlockOffset) / NEOSTRIP_KoBlockSize : -1)

#define NEOSTRIP_KoPower 0
#define NEOSTRIP_KoPowerState 1
#define NEOSTRIP_KoHCLState 2
#define NEOSTRIP_KoBrightness 3
#define NEOSTRIP_KoBrightnessState 4

// Ein
#define KoNEOSTRIP_Power                               (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPower)))
// Ein Status
#define KoNEOSTRIP_PowerState                          (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPowerState)))
// HCL Zustand
#define KoNEOSTRIP_HCLState                            (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoHCLState)))
// Helligkeit
#define KoNEOSTRIP_Brightness                          (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBrightness)))
// Helligkeit Status
#define KoNEOSTRIP_BrightnessState                     (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBrightnessState)))



// Header generation for Module 'BASE_KommentarModule'

#define BASE_KommentarModuleCount 0
#define BASE_KommentarModuleModuleParamSize 0
#define BASE_KommentarModuleSubmodulesParamSize 0
#define BASE_KommentarModuleParamSize 0
#define BASE_KommentarModuleParamOffset 7358
#define BASE_KommentarModuleCalcIndex(index, m1) (index + BASE_KommentarModuleParamOffset + _channelIndex * BASE_KommentarModuleCount * BASE_KommentarModuleParamSize + m1 * BASE_KommentarModuleParamSize)



#ifdef MAIN_FirmwareRevision
#ifndef FIRMWARE_REVISION
#define FIRMWARE_REVISION MAIN_FirmwareRevision
#endif
#endif
#ifdef MAIN_FirmwareName
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME MAIN_FirmwareName
#endif
#endif
