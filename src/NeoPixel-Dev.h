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
#define ETS_ModuleId_FCB 5
#define ETS_ModuleId_LMG 6
#define ETS_ModuleId_NEO 7
#define ETS_ModuleId_NEOEM 8
#define MAIN_FirmwareName "OpenKNX NeoPixel (Dev)"
#define MAIN_OpenKnxId 0xAD
#define MAIN_ApplicationNumber 1
#define MAIN_ApplicationVersion 5
#define MAIN_ApplicationEncoding iso-8859-15
#define MAIN_ParameterSize 43659
#define MAIN_MaxKoNumber 1851
#define MAIN_OrderNumber "NeoPixel"
#define BASE_ModuleVersion 24
#define NET_ModuleVersion 5
#define UCT_ModuleVersion 6
#define LOG_ModuleVersion 67
#define FCB_ModuleVersion 10
#define LMG_ModuleVersion 3
#define NEO_ModuleVersion 5
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
#define     BASE_TimezoneCustomLength 63
#define BASE_Latitude                            69      // float (4 Byte)
#define BASE_Longitude                           73      // float (4 Byte)
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
#define BASE_DefaultLedFunc                      86      // 1 Bit, Bit 7
#define     BASE_DefaultLedFuncMask 0x80
#define     BASE_DefaultLedFuncShift 7
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
#define BASE_ModuleEnabled_FCB                   110      // 1 Bit, Bit 3
#define     BASE_ModuleEnabled_FCBMask 0x08
#define     BASE_ModuleEnabled_FCBShift 3
#define BASE_ModuleEnabled_LMG                   110      // 1 Bit, Bit 2
#define     BASE_ModuleEnabled_LMGMask 0x04
#define     BASE_ModuleEnabled_LMGShift 2
#define BASE_ModuleEnabled_NEO                   110      // 1 Bit, Bit 1
#define     BASE_ModuleEnabled_NEOMask 0x02
#define     BASE_ModuleEnabled_NEOShift 1
#define BASE_ModuleEnabled_NEOEM                 110      // 1 Bit, Bit 0
#define     BASE_ModuleEnabled_NEOEMMask 0x01
#define     BASE_ModuleEnabled_NEOEMShift 0

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
#define ParamBASE_TimezoneCustomStr                   (knx.paramString(BASE_TimezoneCustom, BASE_TimezoneCustomLength))
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
#define ParamBASE_DefaultLedFunc                      ((bool)(knx.paramByte(BASE_DefaultLedFunc) & BASE_DefaultLedFuncMask))
// 
#define ParamBASE_Dummy                               (knx.paramByte(BASE_Dummy))
// NET
#define ParamBASE_ModuleEnabled_NET                   ((bool)(knx.paramByte(BASE_ModuleEnabled_NET) & BASE_ModuleEnabled_NETMask))
// UCT
#define ParamBASE_ModuleEnabled_UCT                   ((bool)(knx.paramByte(BASE_ModuleEnabled_UCT) & BASE_ModuleEnabled_UCTMask))
// LOG
#define ParamBASE_ModuleEnabled_LOG                   ((bool)(knx.paramByte(BASE_ModuleEnabled_LOG) & BASE_ModuleEnabled_LOGMask))
// FCB
#define ParamBASE_ModuleEnabled_FCB                   ((bool)(knx.paramByte(BASE_ModuleEnabled_FCB) & BASE_ModuleEnabled_FCBMask))
// LMG
#define ParamBASE_ModuleEnabled_LMG                   ((bool)(knx.paramByte(BASE_ModuleEnabled_LMG) & BASE_ModuleEnabled_LMGMask))
// NEO
#define ParamBASE_ModuleEnabled_NEO                   ((bool)(knx.paramByte(BASE_ModuleEnabled_NEO) & BASE_ModuleEnabled_NEOMask))
// NEOEM
#define ParamBASE_ModuleEnabled_NEOEM                 ((bool)(knx.paramByte(BASE_ModuleEnabled_NEOEM) & BASE_ModuleEnabled_NEOEMMask))

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
#define     NET_HostNameLength 24
#define NET_LanMode                             173      // 4 Bits, Bit 7-4
#define     NET_LanModeMask 0xF0
#define     NET_LanModeShift 4
#define NET_NTPServer                           174      // char*, 50 Byte
#define     NET_NTPServerLength 50

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
#define ParamNET_HostNameStr                         (knx.paramString(NET_HostName, NET_HostNameLength))
// LAN-Modus
#define ParamNET_LanMode                             ((knx.paramByte(NET_LanMode) & NET_LanModeMask) >> NET_LanModeShift)
// Zeitserver
#define ParamNET_NTPServer                           (knx.paramData(NET_NTPServer))
#define ParamNET_NTPServerStr                        (knx.paramString(NET_NTPServer, NET_NTPServerLength))



#define LOG_VisibleChannels                     225      // uint8_t
#define LOG_VacationKo                          226      // 1 Bit, Bit 7
#define     LOG_VacationKoMask 0x80
#define     LOG_VacationKoShift 7
#define LOG_HolidayKo                           226      // 1 Bit, Bit 6
#define     LOG_HolidayKoMask 0x40
#define     LOG_HolidayKoShift 6
#define LOG_VacationRead                        226      // 1 Bit, Bit 5
#define     LOG_VacationReadMask 0x20
#define     LOG_VacationReadShift 5
#define LOG_HolidaySend                         226      // 1 Bit, Bit 4
#define     LOG_HolidaySendMask 0x10
#define     LOG_HolidaySendShift 4
#define LOG_Neujahr                             227      // 1 Bit, Bit 7
#define     LOG_NeujahrMask 0x80
#define     LOG_NeujahrShift 7
#define LOG_DreiKoenige                         227      // 1 Bit, Bit 6
#define     LOG_DreiKoenigeMask 0x40
#define     LOG_DreiKoenigeShift 6
#define LOG_Weiberfastnacht                     227      // 1 Bit, Bit 5
#define     LOG_WeiberfastnachtMask 0x20
#define     LOG_WeiberfastnachtShift 5
#define LOG_Rosenmontag                         227      // 1 Bit, Bit 4
#define     LOG_RosenmontagMask 0x10
#define     LOG_RosenmontagShift 4
#define LOG_Fastnachtsdienstag                  227      // 1 Bit, Bit 3
#define     LOG_FastnachtsdienstagMask 0x08
#define     LOG_FastnachtsdienstagShift 3
#define LOG_Aschermittwoch                      227      // 1 Bit, Bit 2
#define     LOG_AschermittwochMask 0x04
#define     LOG_AschermittwochShift 2
#define LOG_Frauentag                           227      // 1 Bit, Bit 1
#define     LOG_FrauentagMask 0x02
#define     LOG_FrauentagShift 1
#define LOG_Gruendonnerstag                     227      // 1 Bit, Bit 0
#define     LOG_GruendonnerstagMask 0x01
#define     LOG_GruendonnerstagShift 0
#define LOG_Karfreitag                          228      // 1 Bit, Bit 7
#define     LOG_KarfreitagMask 0x80
#define     LOG_KarfreitagShift 7
#define LOG_Ostersonntag                        228      // 1 Bit, Bit 6
#define     LOG_OstersonntagMask 0x40
#define     LOG_OstersonntagShift 6
#define LOG_Ostermontag                         228      // 1 Bit, Bit 5
#define     LOG_OstermontagMask 0x20
#define     LOG_OstermontagShift 5
#define LOG_TagDerArbeit                        228      // 1 Bit, Bit 4
#define     LOG_TagDerArbeitMask 0x10
#define     LOG_TagDerArbeitShift 4
#define LOG_Himmelfahrt                         228      // 1 Bit, Bit 3
#define     LOG_HimmelfahrtMask 0x08
#define     LOG_HimmelfahrtShift 3
#define LOG_Pfingstsonntag                      228      // 1 Bit, Bit 2
#define     LOG_PfingstsonntagMask 0x04
#define     LOG_PfingstsonntagShift 2
#define LOG_Pfingstmontag                       228      // 1 Bit, Bit 1
#define     LOG_PfingstmontagMask 0x02
#define     LOG_PfingstmontagShift 1
#define LOG_Fronleichnam                        228      // 1 Bit, Bit 0
#define     LOG_FronleichnamMask 0x01
#define     LOG_FronleichnamShift 0
#define LOG_Friedensfest                        229      // 1 Bit, Bit 7
#define     LOG_FriedensfestMask 0x80
#define     LOG_FriedensfestShift 7
#define LOG_MariaHimmelfahrt                    229      // 1 Bit, Bit 6
#define     LOG_MariaHimmelfahrtMask 0x40
#define     LOG_MariaHimmelfahrtShift 6
#define LOG_DeutscheEinheit                     229      // 1 Bit, Bit 5
#define     LOG_DeutscheEinheitMask 0x20
#define     LOG_DeutscheEinheitShift 5
#define LOG_Reformationstag                     229      // 1 Bit, Bit 4
#define     LOG_ReformationstagMask 0x10
#define     LOG_ReformationstagShift 4
#define LOG_Allerheiligen                       229      // 1 Bit, Bit 3
#define     LOG_AllerheiligenMask 0x08
#define     LOG_AllerheiligenShift 3
#define LOG_BussBettag                          229      // 1 Bit, Bit 2
#define     LOG_BussBettagMask 0x04
#define     LOG_BussBettagShift 2
#define LOG_Advent1                             229      // 1 Bit, Bit 1
#define     LOG_Advent1Mask 0x02
#define     LOG_Advent1Shift 1
#define LOG_Advent2                             229      // 1 Bit, Bit 0
#define     LOG_Advent2Mask 0x01
#define     LOG_Advent2Shift 0
#define LOG_Advent3                             230      // 1 Bit, Bit 7
#define     LOG_Advent3Mask 0x80
#define     LOG_Advent3Shift 7
#define LOG_Advent4                             230      // 1 Bit, Bit 6
#define     LOG_Advent4Mask 0x40
#define     LOG_Advent4Shift 6
#define LOG_Heiligabend                         230      // 1 Bit, Bit 5
#define     LOG_HeiligabendMask 0x20
#define     LOG_HeiligabendShift 5
#define LOG_Weihnachtstag1                      230      // 1 Bit, Bit 4
#define     LOG_Weihnachtstag1Mask 0x10
#define     LOG_Weihnachtstag1Shift 4
#define LOG_Weihnachtstag2                      230      // 1 Bit, Bit 3
#define     LOG_Weihnachtstag2Mask 0x08
#define     LOG_Weihnachtstag2Shift 3
#define LOG_Silvester                           230      // 1 Bit, Bit 2
#define     LOG_SilvesterMask 0x04
#define     LOG_SilvesterShift 2
#define LOG_Nationalfeiertag                    230      // 1 Bit, Bit 1
#define     LOG_NationalfeiertagMask 0x02
#define     LOG_NationalfeiertagShift 1
#define LOG_MariaEmpfaengnis                    230      // 1 Bit, Bit 0
#define     LOG_MariaEmpfaengnisMask 0x01
#define     LOG_MariaEmpfaengnisShift 0
#define LOG_NationalfeiertagSchweiz             231      // 1 Bit, Bit 7
#define     LOG_NationalfeiertagSchweizMask 0x80
#define     LOG_NationalfeiertagSchweizShift 7
#define LOG_Totensonntag                        231      // 1 Bit, Bit 6
#define     LOG_TotensonntagMask 0x40
#define     LOG_TotensonntagShift 6
#define LOG_Weltkindertag                       231      // 1 Bit, Bit 5
#define     LOG_WeltkindertagMask 0x20
#define     LOG_WeltkindertagShift 5
#define LOG_UserFormula1                        232      // char*, 99 Byte
#define     LOG_UserFormula1Length 99
#define LOG_UserFormula1Active                  331      // 1 Bit, Bit 7
#define     LOG_UserFormula1ActiveMask 0x80
#define     LOG_UserFormula1ActiveShift 7
#define LOG_UserFormula2                        332      // char*, 99 Byte
#define     LOG_UserFormula2Length 99
#define LOG_UserFormula2Active                  431      // 1 Bit, Bit 7
#define     LOG_UserFormula2ActiveMask 0x80
#define     LOG_UserFormula2ActiveShift 7
#define LOG_UserFormula3                        432      // char*, 99 Byte
#define     LOG_UserFormula3Length 99
#define LOG_UserFormula3Active                  531      // 1 Bit, Bit 7
#define     LOG_UserFormula3ActiveMask 0x80
#define     LOG_UserFormula3ActiveShift 7
#define LOG_UserFormula4                        532      // char*, 99 Byte
#define     LOG_UserFormula4Length 99
#define LOG_UserFormula4Active                  631      // 1 Bit, Bit 7
#define     LOG_UserFormula4ActiveMask 0x80
#define     LOG_UserFormula4ActiveShift 7
#define LOG_UserFormula5                        632      // char*, 99 Byte
#define     LOG_UserFormula5Length 99
#define LOG_UserFormula5Active                  731      // 1 Bit, Bit 7
#define     LOG_UserFormula5ActiveMask 0x80
#define     LOG_UserFormula5ActiveShift 7
#define LOG_UserFormula6                        732      // char*, 99 Byte
#define     LOG_UserFormula6Length 99
#define LOG_UserFormula6Active                  831      // 1 Bit, Bit 7
#define     LOG_UserFormula6ActiveMask 0x80
#define     LOG_UserFormula6ActiveShift 7
#define LOG_UserFormula7                        832      // char*, 99 Byte
#define     LOG_UserFormula7Length 99
#define LOG_UserFormula7Active                  931      // 1 Bit, Bit 7
#define     LOG_UserFormula7ActiveMask 0x80
#define     LOG_UserFormula7ActiveShift 7
#define LOG_UserFormula8                        932      // char*, 99 Byte
#define     LOG_UserFormula8Length 99
#define LOG_UserFormula8Active                  1031      // 1 Bit, Bit 7
#define     LOG_UserFormula8ActiveMask 0x80
#define     LOG_UserFormula8ActiveShift 7
#define LOG_UserFormula9                        1032      // char*, 99 Byte
#define     LOG_UserFormula9Length 99
#define LOG_UserFormula9Active                  1131      // 1 Bit, Bit 7
#define     LOG_UserFormula9ActiveMask 0x80
#define     LOG_UserFormula9ActiveShift 7
#define LOG_UserFormula10                       1132      // char*, 99 Byte
#define     LOG_UserFormula10Length 99
#define LOG_UserFormula10Active                 1231      // 1 Bit, Bit 7
#define     LOG_UserFormula10ActiveMask 0x80
#define     LOG_UserFormula10ActiveShift 7
#define LOG_UserFormula11                       1232      // char*, 99 Byte
#define     LOG_UserFormula11Length 99
#define LOG_UserFormula11Active                 1331      // 1 Bit, Bit 7
#define     LOG_UserFormula11ActiveMask 0x80
#define     LOG_UserFormula11ActiveShift 7
#define LOG_UserFormula12                       1332      // char*, 99 Byte
#define     LOG_UserFormula12Length 99
#define LOG_UserFormula12Active                 1431      // 1 Bit, Bit 7
#define     LOG_UserFormula12ActiveMask 0x80
#define     LOG_UserFormula12ActiveShift 7
#define LOG_UserFormula13                       1432      // char*, 99 Byte
#define     LOG_UserFormula13Length 99
#define LOG_UserFormula13Active                 1531      // 1 Bit, Bit 7
#define     LOG_UserFormula13ActiveMask 0x80
#define     LOG_UserFormula13ActiveShift 7
#define LOG_UserFormula14                       1532      // char*, 99 Byte
#define     LOG_UserFormula14Length 99
#define LOG_UserFormula14Active                 1631      // 1 Bit, Bit 7
#define     LOG_UserFormula14ActiveMask 0x80
#define     LOG_UserFormula14ActiveShift 7
#define LOG_UserFormula15                       1632      // char*, 99 Byte
#define     LOG_UserFormula15Length 99
#define LOG_UserFormula15Active                 1731      // 1 Bit, Bit 7
#define     LOG_UserFormula15ActiveMask 0x80
#define     LOG_UserFormula15ActiveShift 7
#define LOG_UserFormula16                       1732      // char*, 99 Byte
#define     LOG_UserFormula16Length 99
#define LOG_UserFormula16Active                 1831      // 1 Bit, Bit 7
#define     LOG_UserFormula16ActiveMask 0x80
#define     LOG_UserFormula16ActiveShift 7
#define LOG_UserFormula17                       1832      // char*, 99 Byte
#define     LOG_UserFormula17Length 99
#define LOG_UserFormula17Active                 1931      // 1 Bit, Bit 7
#define     LOG_UserFormula17ActiveMask 0x80
#define     LOG_UserFormula17ActiveShift 7
#define LOG_UserFormula18                       1932      // char*, 99 Byte
#define     LOG_UserFormula18Length 99
#define LOG_UserFormula18Active                 2031      // 1 Bit, Bit 7
#define     LOG_UserFormula18ActiveMask 0x80
#define     LOG_UserFormula18ActiveShift 7
#define LOG_UserFormula19                       2032      // char*, 99 Byte
#define     LOG_UserFormula19Length 99
#define LOG_UserFormula19Active                 2131      // 1 Bit, Bit 7
#define     LOG_UserFormula19ActiveMask 0x80
#define     LOG_UserFormula19ActiveShift 7
#define LOG_UserFormula20                       2132      // char*, 99 Byte
#define     LOG_UserFormula20Length 99
#define LOG_UserFormula20Active                 2231      // 1 Bit, Bit 7
#define     LOG_UserFormula20ActiveMask 0x80
#define     LOG_UserFormula20ActiveShift 7
#define LOG_UserFormula21                       2232      // char*, 99 Byte
#define     LOG_UserFormula21Length 99
#define LOG_UserFormula21Active                 2331      // 1 Bit, Bit 7
#define     LOG_UserFormula21ActiveMask 0x80
#define     LOG_UserFormula21ActiveShift 7
#define LOG_UserFormula22                       2332      // char*, 99 Byte
#define     LOG_UserFormula22Length 99
#define LOG_UserFormula22Active                 2431      // 1 Bit, Bit 7
#define     LOG_UserFormula22ActiveMask 0x80
#define     LOG_UserFormula22ActiveShift 7
#define LOG_UserFormula23                       2432      // char*, 99 Byte
#define     LOG_UserFormula23Length 99
#define LOG_UserFormula23Active                 2531      // 1 Bit, Bit 7
#define     LOG_UserFormula23ActiveMask 0x80
#define     LOG_UserFormula23ActiveShift 7
#define LOG_UserFormula24                       2532      // char*, 99 Byte
#define     LOG_UserFormula24Length 99
#define LOG_UserFormula24Active                 2631      // 1 Bit, Bit 7
#define     LOG_UserFormula24ActiveMask 0x80
#define     LOG_UserFormula24ActiveShift 7
#define LOG_UserFormula25                       2632      // char*, 99 Byte
#define     LOG_UserFormula25Length 99
#define LOG_UserFormula25Active                 2731      // 1 Bit, Bit 7
#define     LOG_UserFormula25ActiveMask 0x80
#define     LOG_UserFormula25ActiveShift 7
#define LOG_UserFormula26                       2732      // char*, 99 Byte
#define     LOG_UserFormula26Length 99
#define LOG_UserFormula26Active                 2831      // 1 Bit, Bit 7
#define     LOG_UserFormula26ActiveMask 0x80
#define     LOG_UserFormula26ActiveShift 7
#define LOG_UserFormula27                       2832      // char*, 99 Byte
#define     LOG_UserFormula27Length 99
#define LOG_UserFormula27Active                 2931      // 1 Bit, Bit 7
#define     LOG_UserFormula27ActiveMask 0x80
#define     LOG_UserFormula27ActiveShift 7
#define LOG_UserFormula28                       2932      // char*, 99 Byte
#define     LOG_UserFormula28Length 99
#define LOG_UserFormula28Active                 3031      // 1 Bit, Bit 7
#define     LOG_UserFormula28ActiveMask 0x80
#define     LOG_UserFormula28ActiveShift 7
#define LOG_UserFormula29                       3032      // char*, 99 Byte
#define     LOG_UserFormula29Length 99
#define LOG_UserFormula29Active                 3131      // 1 Bit, Bit 7
#define     LOG_UserFormula29ActiveMask 0x80
#define     LOG_UserFormula29ActiveShift 7
#define LOG_UserFormula30                       3132      // char*, 99 Byte
#define     LOG_UserFormula30Length 99
#define LOG_UserFormula30Active                 3231      // 1 Bit, Bit 7
#define     LOG_UserFormula30ActiveMask 0x80
#define     LOG_UserFormula30ActiveShift 7

// Verfügbare Kanäle
#define ParamLOG_VisibleChannels                     (knx.paramByte(LOG_VisibleChannels))
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
// Formeldefinition
#define ParamLOG_UserFormula1                        (knx.paramData(LOG_UserFormula1))
#define ParamLOG_UserFormula1Str                     (knx.paramString(LOG_UserFormula1, LOG_UserFormula1Length))
// Benutzerformel 1 aktiv
#define ParamLOG_UserFormula1Active                  ((bool)(knx.paramByte(LOG_UserFormula1Active) & LOG_UserFormula1ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula2                        (knx.paramData(LOG_UserFormula2))
#define ParamLOG_UserFormula2Str                     (knx.paramString(LOG_UserFormula2, LOG_UserFormula2Length))
// Benutzerformel 2 aktiv
#define ParamLOG_UserFormula2Active                  ((bool)(knx.paramByte(LOG_UserFormula2Active) & LOG_UserFormula2ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula3                        (knx.paramData(LOG_UserFormula3))
#define ParamLOG_UserFormula3Str                     (knx.paramString(LOG_UserFormula3, LOG_UserFormula3Length))
// Benutzerformel 3 aktiv
#define ParamLOG_UserFormula3Active                  ((bool)(knx.paramByte(LOG_UserFormula3Active) & LOG_UserFormula3ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula4                        (knx.paramData(LOG_UserFormula4))
#define ParamLOG_UserFormula4Str                     (knx.paramString(LOG_UserFormula4, LOG_UserFormula4Length))
// Benutzerformel 4 aktiv
#define ParamLOG_UserFormula4Active                  ((bool)(knx.paramByte(LOG_UserFormula4Active) & LOG_UserFormula4ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula5                        (knx.paramData(LOG_UserFormula5))
#define ParamLOG_UserFormula5Str                     (knx.paramString(LOG_UserFormula5, LOG_UserFormula5Length))
// Benutzerformel 5 aktiv
#define ParamLOG_UserFormula5Active                  ((bool)(knx.paramByte(LOG_UserFormula5Active) & LOG_UserFormula5ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula6                        (knx.paramData(LOG_UserFormula6))
#define ParamLOG_UserFormula6Str                     (knx.paramString(LOG_UserFormula6, LOG_UserFormula6Length))
// Benutzerformel 6 aktiv
#define ParamLOG_UserFormula6Active                  ((bool)(knx.paramByte(LOG_UserFormula6Active) & LOG_UserFormula6ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula7                        (knx.paramData(LOG_UserFormula7))
#define ParamLOG_UserFormula7Str                     (knx.paramString(LOG_UserFormula7, LOG_UserFormula7Length))
// Benutzerformel 7 aktiv
#define ParamLOG_UserFormula7Active                  ((bool)(knx.paramByte(LOG_UserFormula7Active) & LOG_UserFormula7ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula8                        (knx.paramData(LOG_UserFormula8))
#define ParamLOG_UserFormula8Str                     (knx.paramString(LOG_UserFormula8, LOG_UserFormula8Length))
// Benutzerformel 8 aktiv
#define ParamLOG_UserFormula8Active                  ((bool)(knx.paramByte(LOG_UserFormula8Active) & LOG_UserFormula8ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula9                        (knx.paramData(LOG_UserFormula9))
#define ParamLOG_UserFormula9Str                     (knx.paramString(LOG_UserFormula9, LOG_UserFormula9Length))
// Benutzerformel 9 aktiv
#define ParamLOG_UserFormula9Active                  ((bool)(knx.paramByte(LOG_UserFormula9Active) & LOG_UserFormula9ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula10                       (knx.paramData(LOG_UserFormula10))
#define ParamLOG_UserFormula10Str                    (knx.paramString(LOG_UserFormula10, LOG_UserFormula10Length))
// Benutzerformel 10 aktiv
#define ParamLOG_UserFormula10Active                 ((bool)(knx.paramByte(LOG_UserFormula10Active) & LOG_UserFormula10ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula11                       (knx.paramData(LOG_UserFormula11))
#define ParamLOG_UserFormula11Str                    (knx.paramString(LOG_UserFormula11, LOG_UserFormula11Length))
// Benutzerformel 11 aktiv
#define ParamLOG_UserFormula11Active                 ((bool)(knx.paramByte(LOG_UserFormula11Active) & LOG_UserFormula11ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula12                       (knx.paramData(LOG_UserFormula12))
#define ParamLOG_UserFormula12Str                    (knx.paramString(LOG_UserFormula12, LOG_UserFormula12Length))
// Benutzerformel 12 aktiv
#define ParamLOG_UserFormula12Active                 ((bool)(knx.paramByte(LOG_UserFormula12Active) & LOG_UserFormula12ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula13                       (knx.paramData(LOG_UserFormula13))
#define ParamLOG_UserFormula13Str                    (knx.paramString(LOG_UserFormula13, LOG_UserFormula13Length))
// Benutzerformel 13 aktiv
#define ParamLOG_UserFormula13Active                 ((bool)(knx.paramByte(LOG_UserFormula13Active) & LOG_UserFormula13ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula14                       (knx.paramData(LOG_UserFormula14))
#define ParamLOG_UserFormula14Str                    (knx.paramString(LOG_UserFormula14, LOG_UserFormula14Length))
// Benutzerformel 14 aktiv
#define ParamLOG_UserFormula14Active                 ((bool)(knx.paramByte(LOG_UserFormula14Active) & LOG_UserFormula14ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula15                       (knx.paramData(LOG_UserFormula15))
#define ParamLOG_UserFormula15Str                    (knx.paramString(LOG_UserFormula15, LOG_UserFormula15Length))
// Benutzerformel 15 aktiv
#define ParamLOG_UserFormula15Active                 ((bool)(knx.paramByte(LOG_UserFormula15Active) & LOG_UserFormula15ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula16                       (knx.paramData(LOG_UserFormula16))
#define ParamLOG_UserFormula16Str                    (knx.paramString(LOG_UserFormula16, LOG_UserFormula16Length))
// Benutzerformel 16 aktiv
#define ParamLOG_UserFormula16Active                 ((bool)(knx.paramByte(LOG_UserFormula16Active) & LOG_UserFormula16ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula17                       (knx.paramData(LOG_UserFormula17))
#define ParamLOG_UserFormula17Str                    (knx.paramString(LOG_UserFormula17, LOG_UserFormula17Length))
// Benutzerformel 17 aktiv
#define ParamLOG_UserFormula17Active                 ((bool)(knx.paramByte(LOG_UserFormula17Active) & LOG_UserFormula17ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula18                       (knx.paramData(LOG_UserFormula18))
#define ParamLOG_UserFormula18Str                    (knx.paramString(LOG_UserFormula18, LOG_UserFormula18Length))
// Benutzerformel 18 aktiv
#define ParamLOG_UserFormula18Active                 ((bool)(knx.paramByte(LOG_UserFormula18Active) & LOG_UserFormula18ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula19                       (knx.paramData(LOG_UserFormula19))
#define ParamLOG_UserFormula19Str                    (knx.paramString(LOG_UserFormula19, LOG_UserFormula19Length))
// Benutzerformel 19 aktiv
#define ParamLOG_UserFormula19Active                 ((bool)(knx.paramByte(LOG_UserFormula19Active) & LOG_UserFormula19ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula20                       (knx.paramData(LOG_UserFormula20))
#define ParamLOG_UserFormula20Str                    (knx.paramString(LOG_UserFormula20, LOG_UserFormula20Length))
// Benutzerformel 20 aktiv
#define ParamLOG_UserFormula20Active                 ((bool)(knx.paramByte(LOG_UserFormula20Active) & LOG_UserFormula20ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula21                       (knx.paramData(LOG_UserFormula21))
#define ParamLOG_UserFormula21Str                    (knx.paramString(LOG_UserFormula21, LOG_UserFormula21Length))
// Benutzerformel 21 aktiv
#define ParamLOG_UserFormula21Active                 ((bool)(knx.paramByte(LOG_UserFormula21Active) & LOG_UserFormula21ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula22                       (knx.paramData(LOG_UserFormula22))
#define ParamLOG_UserFormula22Str                    (knx.paramString(LOG_UserFormula22, LOG_UserFormula22Length))
// Benutzerformel 22 aktiv
#define ParamLOG_UserFormula22Active                 ((bool)(knx.paramByte(LOG_UserFormula22Active) & LOG_UserFormula22ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula23                       (knx.paramData(LOG_UserFormula23))
#define ParamLOG_UserFormula23Str                    (knx.paramString(LOG_UserFormula23, LOG_UserFormula23Length))
// Benutzerformel 23 aktiv
#define ParamLOG_UserFormula23Active                 ((bool)(knx.paramByte(LOG_UserFormula23Active) & LOG_UserFormula23ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula24                       (knx.paramData(LOG_UserFormula24))
#define ParamLOG_UserFormula24Str                    (knx.paramString(LOG_UserFormula24, LOG_UserFormula24Length))
// Benutzerformel 24 aktiv
#define ParamLOG_UserFormula24Active                 ((bool)(knx.paramByte(LOG_UserFormula24Active) & LOG_UserFormula24ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula25                       (knx.paramData(LOG_UserFormula25))
#define ParamLOG_UserFormula25Str                    (knx.paramString(LOG_UserFormula25, LOG_UserFormula25Length))
// Benutzerformel 25 aktiv
#define ParamLOG_UserFormula25Active                 ((bool)(knx.paramByte(LOG_UserFormula25Active) & LOG_UserFormula25ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula26                       (knx.paramData(LOG_UserFormula26))
#define ParamLOG_UserFormula26Str                    (knx.paramString(LOG_UserFormula26, LOG_UserFormula26Length))
// Benutzerformel 26 aktiv
#define ParamLOG_UserFormula26Active                 ((bool)(knx.paramByte(LOG_UserFormula26Active) & LOG_UserFormula26ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula27                       (knx.paramData(LOG_UserFormula27))
#define ParamLOG_UserFormula27Str                    (knx.paramString(LOG_UserFormula27, LOG_UserFormula27Length))
// Benutzerformel 27 aktiv
#define ParamLOG_UserFormula27Active                 ((bool)(knx.paramByte(LOG_UserFormula27Active) & LOG_UserFormula27ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula28                       (knx.paramData(LOG_UserFormula28))
#define ParamLOG_UserFormula28Str                    (knx.paramString(LOG_UserFormula28, LOG_UserFormula28Length))
// Benutzerformel 28 aktiv
#define ParamLOG_UserFormula28Active                 ((bool)(knx.paramByte(LOG_UserFormula28Active) & LOG_UserFormula28ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula29                       (knx.paramData(LOG_UserFormula29))
#define ParamLOG_UserFormula29Str                    (knx.paramString(LOG_UserFormula29, LOG_UserFormula29Length))
// Benutzerformel 29 aktiv
#define ParamLOG_UserFormula29Active                 ((bool)(knx.paramByte(LOG_UserFormula29Active) & LOG_UserFormula29ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula30                       (knx.paramData(LOG_UserFormula30))
#define ParamLOG_UserFormula30Str                    (knx.paramString(LOG_UserFormula30, LOG_UserFormula30Length))
// Benutzerformel 30 aktiv
#define ParamLOG_UserFormula30Active                 ((bool)(knx.paramByte(LOG_UserFormula30Active) & LOG_UserFormula30ActiveMask))

#define LOG_KoVacation 15
#define LOG_KoHoliday1 16
#define LOG_KoHoliday2 17

// Urlaub
#define KoLOG_Vacation                            (knx.getGroupObject(LOG_KoVacation))
// Welcher Feiertag ist heute?
#define KoLOG_Holiday1                            (knx.getGroupObject(LOG_KoHoliday1))
// Welcher Feiertag ist morgen?
#define KoLOG_Holiday2                            (knx.getGroupObject(LOG_KoHoliday2))

#define LOG_ChannelCount 50

// Parameter per channel
#define LOG_ParamBlockOffset 3232
#define LOG_ParamBlockSize 89
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
#define LOG_fE1LowDeltaFloat                    13      // float (4 Byte)
#define LOG_fE1HighDeltaFloat                   17      // float (4 Byte)
#define LOG_fE1LowDeltaDouble                   13      // float (4 Byte)
#define LOG_fE1HighDeltaDouble                  17      // float (4 Byte)
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
#define LOG_fE1LowDpt9                          13      // float (4 Byte)
#define LOG_fE1HighDpt9                         17      // float (4 Byte)
#define LOG_fE1LowDpt9Fix                       13      // float (4 Byte)
#define LOG_fE1LowDpt12                         13      // uint32_t
#define LOG_fE1HighDpt12                        17      // uint32_t
#define LOG_fE1LowDpt12Fix                      13      // uint32_t
#define LOG_fE1LowDpt13                         13      // int32_t
#define LOG_fE1HighDpt13                        17      // int32_t
#define LOG_fE1LowDpt13Fix                      13      // int32_t
#define LOG_fE1LowDpt14                         13      // float (4 Byte)
#define LOG_fE1HighDpt14                        17      // float (4 Byte)
#define LOG_fE1LowDpt14Fix                      13      // float (4 Byte)
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
#define LOG_fE2LowDeltaFloat                    28      // float (4 Byte)
#define LOG_fE2HighDeltaFloat                   32      // float (4 Byte)
#define LOG_fE2LowDeltaDouble                   28      // float (4 Byte)
#define LOG_fE2HighDeltaDouble                  32      // float (4 Byte)
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
#define LOG_fE2LowDpt9                          28      // float (4 Byte)
#define LOG_fE2HighDpt9                         32      // float (4 Byte)
#define LOG_fE2LowDpt9Fix                       28      // float (4 Byte)
#define LOG_fE2LowDpt12                         28      // uint32_t
#define LOG_fE2HighDpt12                        32      // uint32_t
#define LOG_fE2LowDpt12Fix                      28      // uint32_t
#define LOG_fE2LowDpt13                         28      // int32_t
#define LOG_fE2HighDpt13                        32      // int32_t
#define LOG_fE2LowDpt13Fix                      28      // int32_t
#define LOG_fE2LowDpt14                         28      // float (4 Byte)
#define LOG_fE2HighDpt14                        32      // float (4 Byte)
#define LOG_fE2LowDpt14Fix                      28      // float (4 Byte)
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
#define LOG_fI1InternalInputType                36      // 1 Bit, Bit 2
#define     LOG_fI1InternalInputTypeMask 0x04
#define     LOG_fI1InternalInputTypeShift 2
#define LOG_fI1Function                         37      // uint8_t
#define LOG_fI1FunctionRel                      37      // int8_t
#define LOG_fI1StatusLed                        37      // 16 Bits, Bit 15-0
#define LOG_fI2                                 39      // 2 Bits, Bit 7-6
#define     LOG_fI2Mask 0xC0
#define     LOG_fI2Shift 6
#define LOG_fI2Kind                             39      // 2 Bits, Bit 5-4
#define     LOG_fI2KindMask 0x30
#define     LOG_fI2KindShift 4
#define LOG_fI2AsTrigger                        39      // 1 Bit, Bit 3
#define     LOG_fI2AsTriggerMask 0x08
#define     LOG_fI2AsTriggerShift 3
#define LOG_fI2InternalInputType                39      // 1 Bit, Bit 2
#define     LOG_fI2InternalInputTypeMask 0x04
#define     LOG_fI2InternalInputTypeShift 2
#define LOG_fI2Function                         40      // uint8_t
#define LOG_fI2FunctionRel                      40      // int8_t
#define LOG_fI2StatusLed                        40      // 16 Bits, Bit 15-0
#define LOG_fOStairtimeBase                     42      // 2 Bits, Bit 7-6
#define     LOG_fOStairtimeBaseMask 0xC0
#define     LOG_fOStairtimeBaseShift 6
#define LOG_fOStairtimeTime                     42      // 14 Bits, Bit 13-0
#define     LOG_fOStairtimeTimeMask 0x3FFF
#define     LOG_fOStairtimeTimeShift 0
#define LOG_fOBlinkBase                         44      // 2 Bits, Bit 7-6
#define     LOG_fOBlinkBaseMask 0xC0
#define     LOG_fOBlinkBaseShift 6
#define LOG_fOBlinkTime                         44      // 14 Bits, Bit 13-0
#define     LOG_fOBlinkTimeMask 0x3FFF
#define     LOG_fOBlinkTimeShift 0
#define LOG_fODelayOnBase                       46      // 2 Bits, Bit 7-6
#define     LOG_fODelayOnBaseMask 0xC0
#define     LOG_fODelayOnBaseShift 6
#define LOG_fODelayOnTime                       46      // 14 Bits, Bit 13-0
#define     LOG_fODelayOnTimeMask 0x3FFF
#define     LOG_fODelayOnTimeShift 0
#define LOG_fODelayOffBase                      48      // 2 Bits, Bit 7-6
#define     LOG_fODelayOffBaseMask 0xC0
#define     LOG_fODelayOffBaseShift 6
#define LOG_fODelayOffTime                      48      // 14 Bits, Bit 13-0
#define     LOG_fODelayOffTimeMask 0x3FFF
#define     LOG_fODelayOffTimeShift 0
#define LOG_fORepeatOnBase                      50      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOnBaseMask 0xC0
#define     LOG_fORepeatOnBaseShift 6
#define LOG_fORepeatOnTime                      50      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOnTimeMask 0x3FFF
#define     LOG_fORepeatOnTimeShift 0
#define LOG_fORepeatOffBase                     52      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOffBaseMask 0xC0
#define     LOG_fORepeatOffBaseShift 6
#define LOG_fORepeatOffTime                     52      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOffTimeMask 0x3FFF
#define     LOG_fORepeatOffTimeShift 0
#define LOG_fODelay                             54      // 1 Bit, Bit 7
#define     LOG_fODelayMask 0x80
#define     LOG_fODelayShift 7
#define LOG_fODelayOnRepeat                     54      // 2 Bits, Bit 6-5
#define     LOG_fODelayOnRepeatMask 0x60
#define     LOG_fODelayOnRepeatShift 5
#define LOG_fODelayOnReset                      54      // 1 Bit, Bit 4
#define     LOG_fODelayOnResetMask 0x10
#define     LOG_fODelayOnResetShift 4
#define LOG_fODelayOffRepeat                    54      // 2 Bits, Bit 3-2
#define     LOG_fODelayOffRepeatMask 0x0C
#define     LOG_fODelayOffRepeatShift 2
#define LOG_fODelayOffReset                     54      // 1 Bit, Bit 1
#define     LOG_fODelayOffResetMask 0x02
#define     LOG_fODelayOffResetShift 1
#define LOG_fOStair                             54      // 1 Bit, Bit 0
#define     LOG_fOStairMask 0x01
#define     LOG_fOStairShift 0
#define LOG_fORetrigger                         55      // 1 Bit, Bit 7
#define     LOG_fORetriggerMask 0x80
#define     LOG_fORetriggerShift 7
#define LOG_fOStairOff                          55      // 1 Bit, Bit 6
#define     LOG_fOStairOffMask 0x40
#define     LOG_fOStairOffShift 6
#define LOG_fORepeat                            55      // 1 Bit, Bit 5
#define     LOG_fORepeatMask 0x20
#define     LOG_fORepeatShift 5
#define LOG_fOOutputFilter                      55      // 2 Bits, Bit 4-3
#define     LOG_fOOutputFilterMask 0x18
#define     LOG_fOOutputFilterShift 3
#define LOG_fOSendOnChange                      55      // 1 Bit, Bit 2
#define     LOG_fOSendOnChangeMask 0x04
#define     LOG_fOSendOnChangeShift 2
#define LOG_fOLockEnabled                       55      // 1 Bit, Bit 1
#define     LOG_fOLockEnabledMask 0x02
#define     LOG_fOLockEnabledShift 1
#define LOG_fODpt                               56      // 8 Bits, Bit 7-0
#define LOG_fOLockTriggerLock                   57      // 2 Bits, Bit 7-6
#define     LOG_fOLockTriggerLockMask 0xC0
#define     LOG_fOLockTriggerLockShift 6
#define LOG_fOLockTriggerUnlock                 57      // 2 Bits, Bit 5-4
#define     LOG_fOLockTriggerUnlockMask 0x30
#define     LOG_fOLockTriggerUnlockShift 4
#define LOG_fOLockResetQueue                    57      // 2 Bits, Bit 3-2
#define     LOG_fOLockResetQueueMask 0x0C
#define     LOG_fOLockResetQueueShift 2
#define LOG_fOLockKind                          57      // 2 Bits, Bit 1-0
#define     LOG_fOLockKindMask 0x03
#define     LOG_fOLockKindShift 0
#define LOG_fOLockFunction                      58      // uint8_t
#define LOG_fOLockFunctionRel                   58      // int8_t
#define LOG_fOOnAll                             59      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt1                            60      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt2                            60      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt3Dir                         60      // 5 Bits, Bit 7-3
#define     LOG_fOOnDpt3DirMask 0xF8
#define     LOG_fOOnDpt3DirShift 3
#define LOG_fOOnDpt3Dim                         60      // 3 Bits, Bit 2-0
#define     LOG_fOOnDpt3DimMask 0x07
#define     LOG_fOOnDpt3DimShift 0
#define LOG_fOOnDpt5                            60      // uint8_t
#define LOG_fOOnDpt5001                         60      // uint8_t
#define LOG_fOOnDpt6                            60      // int8_t
#define LOG_fOOnDpt7                            60      // uint16_t
#define LOG_fOOnDpt8                            60      // int16_t
#define LOG_fOOnDpt9                            60      // float (4 Byte)
#define LOG_fOOnDpt12                           60      // uint32_t
#define LOG_fOOnDpt13                           60      // int32_t
#define LOG_fOOnDpt14                           60      // float (4 Byte)
#define LOG_fOOnDpt16                           60      // char*, 14 Byte
#define     LOG_fOOnDpt16Length 14
#define LOG_fOOnDpt17                           60      // 8 Bits, Bit 7-0
#define LOG_fOOnRGB                             60      // 24 Bits, Bit 31-8
#define     LOG_fOOnRGBMask 0xFFFFFF00
#define     LOG_fOOnRGBShift 8
#define LOG_fOOnLedProvider                     64      // 3 Bits, Bit 2-0
#define     LOG_fOOnLedProviderMask 0x07
#define     LOG_fOOnLedProviderShift 0
#define LOG_fOOnLedEffect                       65      // 3 Bits, Bit 2-0
#define     LOG_fOOnLedEffectMask 0x07
#define     LOG_fOOnLedEffectShift 0
#define LOG_fOOnLedDuration                     66      // uint16_t
#define LOG_fOOnPAArea                          60      // 4 Bits, Bit 7-4
#define     LOG_fOOnPAAreaMask 0xF0
#define     LOG_fOOnPAAreaShift 4
#define LOG_fOOnPALine                          60      // 4 Bits, Bit 3-0
#define     LOG_fOOnPALineMask 0x0F
#define     LOG_fOOnPALineShift 0
#define LOG_fOOnPADevice                        61      // uint8_t
#define LOG_fOOnFunction                        60      // 8 Bits, Bit 7-0
#define LOG_fOOnKOKind                          65      // 2 Bits, Bit 7-6
#define     LOG_fOOnKOKindMask 0xC0
#define     LOG_fOOnKOKindShift 6
#define LOG_fOOnKONumber                        60      // uint16_t
#define LOG_fOOnKONumberRel                     60      // int16_t
#define LOG_fOOnKODpt                           62      // 8 Bits, Bit 7-0
#define LOG_fOOnKOSend                          65      // 2 Bits, Bit 5-4
#define     LOG_fOOnKOSendMask 0x30
#define     LOG_fOOnKOSendShift 4
#define LOG_fOOnKOSendNumber                    66      // uint16_t
#define LOG_fOOnKOSendNumberRel                 66      // int16_t
#define LOG_fOOffAll                            74      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt1                           75      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt2                           75      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt3Dir                        75      // 5 Bits, Bit 7-3
#define     LOG_fOOffDpt3DirMask 0xF8
#define     LOG_fOOffDpt3DirShift 3
#define LOG_fOOffDpt3Dim                        75      // 3 Bits, Bit 2-0
#define     LOG_fOOffDpt3DimMask 0x07
#define     LOG_fOOffDpt3DimShift 0
#define LOG_fOOffDpt5                           75      // uint8_t
#define LOG_fOOffDpt5001                        75      // uint8_t
#define LOG_fOOffDpt6                           75      // int8_t
#define LOG_fOOffDpt7                           75      // uint16_t
#define LOG_fOOffDpt8                           75      // int16_t
#define LOG_fOOffDpt9                           75      // float (4 Byte)
#define LOG_fOOffDpt12                          75      // uint32_t
#define LOG_fOOffDpt13                          75      // int32_t
#define LOG_fOOffDpt14                          75      // float (4 Byte)
#define LOG_fOOffDpt16                          75      // char*, 14 Byte
#define     LOG_fOOffDpt16Length 14
#define LOG_fOOffDpt17                          75      // 8 Bits, Bit 7-0
#define LOG_fOOffRGB                            75      // 24 Bits, Bit 31-8
#define     LOG_fOOffRGBMask 0xFFFFFF00
#define     LOG_fOOffRGBShift 8
#define LOG_fOOffLedProvider                    79      // 3 Bits, Bit 2-0
#define     LOG_fOOffLedProviderMask 0x07
#define     LOG_fOOffLedProviderShift 0
#define LOG_fOOffLedEffect                      80      // 3 Bits, Bit 2-0
#define     LOG_fOOffLedEffectMask 0x07
#define     LOG_fOOffLedEffectShift 0
#define LOG_fOOffLedDuration                    81      // uint16_t
#define LOG_fOOffPAArea                         75      // 4 Bits, Bit 7-4
#define     LOG_fOOffPAAreaMask 0xF0
#define     LOG_fOOffPAAreaShift 4
#define LOG_fOOffPALine                         75      // 4 Bits, Bit 3-0
#define     LOG_fOOffPALineMask 0x0F
#define     LOG_fOOffPALineShift 0
#define LOG_fOOffPADevice                       76      // uint8_t
#define LOG_fOOffFunction                       75      // 8 Bits, Bit 7-0
#define LOG_fOOffKOKind                         80      // 2 Bits, Bit 7-6
#define     LOG_fOOffKOKindMask 0xC0
#define     LOG_fOOffKOKindShift 6
#define LOG_fOOffKONumber                       75      // uint16_t
#define LOG_fOOffKONumberRel                    75      // int16_t
#define LOG_fOOffKODpt                          77      // 8 Bits, Bit 7-0
#define LOG_fOOffKOSend                         80      // 2 Bits, Bit 5-4
#define     LOG_fOOffKOSendMask 0x30
#define     LOG_fOOffKOSendShift 4
#define LOG_fOOffKOSendNumber                   81      // uint16_t
#define LOG_fOOffKOSendNumberRel                81      // int16_t

// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayBase                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fChannelDelayBase)) & LOG_fChannelDelayBaseMask) >> LOG_fChannelDelayBaseShift)
// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayTime                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime)) & LOG_fChannelDelayTimeMask)
// Zeit bis der Kanal nach einem Neustart aktiv wird (in Millisekunden)
#define ParamLOG_fChannelDelayTimeMS                 (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime))))
// Logik-Operation
#define ParamLOG_fLogic                              (PT_Logic)(knx.paramByte(LOG_ParamCalcIndex(LOG_fLogic)))
// Logik auswerten
#define ParamLOG_fCalculate                          (PT_Calculate)(knx.paramByte(LOG_ParamCalcIndex(LOG_fCalculate)) & LOG_fCalculateMask)
// Kanal deaktivieren (zu Testzwecken)
#define ParamLOG_fDisable                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fDisable)) & LOG_fDisableMask))
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
#define ParamLOG_fTriggerGateClose                   (PT_GateTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateClose)) & LOG_fTriggerGateCloseMask) >> LOG_fTriggerGateCloseShift)
// Beim öffnen vom Tor wird
#define ParamLOG_fTriggerGateOpen                    (PT_GateTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateOpen)) & LOG_fTriggerGateOpenMask) >> LOG_fTriggerGateOpenShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertInt                       (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertInt)) & LOG_fE1ConvertIntMask) >> LOG_fE1ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1Convert                          (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Convert)) & LOG_fE1ConvertMask) >> LOG_fE1ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertFloat                     (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertFloat)) & LOG_fE1ConvertFloatMask) >> LOG_fE1ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertSpecial                   (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertSpecial)) & LOG_fE1ConvertSpecialMask) >> LOG_fE1ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertBool                      (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertBool)) & LOG_fE1ConvertBoolMask) >> LOG_fE1ConvertBoolShift)
// Eingang 1
#define ParamLOG_fE1                                 (PT_InputEnable)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1)) & LOG_fE1Mask)
// DPT für Eingang
#define ParamLOG_fE1Dpt                              (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Dpt)))
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
#define ParamLOG_fE1Default                          (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Default)) & LOG_fE1DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE1DefaultExt                       (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultExt)) & LOG_fE1DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE1DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultEEPROM)) & LOG_fE1DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE1DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultRepeat)) & LOG_fE1DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE1UseOtherKO                       (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1UseOtherKO)) & LOG_fE1UseOtherKOMask) >> LOG_fE1UseOtherKOShift)
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
#define ParamLOG_fE2ConvertInt                       (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertInt)) & LOG_fE2ConvertIntMask) >> LOG_fE2ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2Convert                          (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Convert)) & LOG_fE2ConvertMask) >> LOG_fE2ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertFloat                     (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertFloat)) & LOG_fE2ConvertFloatMask) >> LOG_fE2ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertSpecial                   (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertSpecial)) & LOG_fE2ConvertSpecialMask) >> LOG_fE2ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertBool                      (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertBool)) & LOG_fE2ConvertBoolMask) >> LOG_fE2ConvertBoolShift)
// Eingang 2
#define ParamLOG_fE2                                 (PT_InputEnable)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2)) & LOG_fE2Mask)
// DPT für Eingang
#define ParamLOG_fE2Dpt                              (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Dpt)))
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
#define ParamLOG_fE2Default                          (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Default)) & LOG_fE2DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE2DefaultExt                       (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultExt)) & LOG_fE2DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE2DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultEEPROM)) & LOG_fE2DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE2DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultRepeat)) & LOG_fE2DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE2UseOtherKO                       (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2UseOtherKO)) & LOG_fE2UseOtherKOMask) >> LOG_fE2UseOtherKOShift)
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
#define ParamLOG_fTd1DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1DuskDawn)) & LOG_fTd1DuskDawnMask) >> LOG_fTd1DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd2DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2DuskDawn)) & LOG_fTd2DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd3DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3DuskDawn)) & LOG_fTd3DuskDawnMask) >> LOG_fTd3DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd4DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4DuskDawn)) & LOG_fTd4DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd5DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5DuskDawn)) & LOG_fTd5DuskDawnMask) >> LOG_fTd5DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd6DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6DuskDawn)) & LOG_fTd6DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd7DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7DuskDawn)) & LOG_fTd7DuskDawnMask) >> LOG_fTd7DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd8DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8DuskDawn)) & LOG_fTd8DuskDawnMask)
// Typ der Zeitschaltuhr
#define ParamLOG_fTYearDay                           (PT_YearDay)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTYearDay)) & LOG_fTYearDayMask) >> LOG_fTYearDayShift)
// Feiertagsbehandlung
#define ParamLOG_fTHoliday                           (PT_Holiday)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTHoliday)) & LOG_fTHolidayMask) >> LOG_fTHolidayShift)
// Bei Neustart letzte Schaltzeit nachholen
#define ParamLOG_fTRestoreState                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTRestoreState)) & LOG_fTRestoreStateMask) >> LOG_fTRestoreStateShift)
// Urlaubsbehandlung
#define ParamLOG_fTVacation                          (PT_Vacation)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTVacation)) & LOG_fTVacationMask)
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
#define ParamLOG_fI1                                 (PT_InputEnable)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1)) & LOG_fI1Mask) >> LOG_fI1Shift)
// Art der Verknüpfung
#define ParamLOG_fI1Kind                             (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Kind)) & LOG_fI1KindMask) >> LOG_fI1KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI1AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI1AsTrigger)) & LOG_fI1AsTriggerMask))
// Interner Eingang wird versorgt vom
#define ParamLOG_fI1InternalInputType                (PT_InternalInputType)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1InternalInputType)) & LOG_fI1InternalInputTypeMask) >> LOG_fI1InternalInputTypeShift)
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI1FunctionRel)))
// Statuskanal
#define ParamLOG_fI1StatusLed                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fI1StatusLed)))
// Interner Eingang 4
#define ParamLOG_fI2                                 (PT_InputEnable)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2)) & LOG_fI2Mask) >> LOG_fI2Shift)
// Art der Verknüpfung
#define ParamLOG_fI2Kind                             (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Kind)) & LOG_fI2KindMask) >> LOG_fI2KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI2AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI2AsTrigger)) & LOG_fI2AsTriggerMask))
// Interner Eingang wird versorgt vom
#define ParamLOG_fI2InternalInputType                (PT_InternalInputType)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2InternalInputType)) & LOG_fI2InternalInputTypeMask) >> LOG_fI2InternalInputTypeShift)
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI2FunctionRel)))
// Statuskanal
#define ParamLOG_fI2StatusLed                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fI2StatusLed)))
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
#define ParamLOG_fODelayOnRepeat                     (PT_OnOffRepeat)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnRepeat)) & LOG_fODelayOnRepeatMask) >> LOG_fODelayOnRepeatShift)
// Darauffolgendes AUS führt zu
#define ParamLOG_fODelayOnReset                      (PT_OnOffReset)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnReset)) & LOG_fODelayOnResetMask) >> LOG_fODelayOnResetShift)
// Erneutes AUS führt zu
#define ParamLOG_fODelayOffRepeat                    (PT_OnOffRepeat)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffRepeat)) & LOG_fODelayOffRepeatMask) >> LOG_fODelayOffRepeatShift)
// Darauffolgendes EIN führt zu
#define ParamLOG_fODelayOffReset                     (PT_OnOffReset)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffReset)) & LOG_fODelayOffResetMask) >> LOG_fODelayOffResetShift)
// Ausgang hat eine Treppenlichtfunktion
#define ParamLOG_fOStair                             ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStair)) & LOG_fOStairMask))
// Treppenlicht kann verlängert werden
#define ParamLOG_fORetrigger                         ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORetrigger)) & LOG_fORetriggerMask))
// Treppenlicht kann ausgeschaltet werden
#define ParamLOG_fOStairOff                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStairOff)) & LOG_fOStairOffMask))
// Ausgang wiederholt zyklisch
#define ParamLOG_fORepeat                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeat)) & LOG_fORepeatMask))
// Wiederholungsfilter
#define ParamLOG_fOOutputFilter                      (PT_OutputFilter)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOutputFilter)) & LOG_fOOutputFilterMask) >> LOG_fOOutputFilterShift)
// Sendeverhalten für Ausgang
#define ParamLOG_fOSendOnChange                      (PT_SendOnChange)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOSendOnChange)) & LOG_fOSendOnChangeMask) >> LOG_fOSendOnChangeShift)
// Sperre aktivieren
#define ParamLOG_fOLockEnabled                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockEnabled)) & LOG_fOLockEnabledMask))
// DPT für Ausgang
#define ParamLOG_fODpt                               (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODpt)))
// Beim Sperren
#define ParamLOG_fOLockTriggerLock                   (PT_LockTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockTriggerLock)) & LOG_fOLockTriggerLockMask) >> LOG_fOLockTriggerLockShift)
// Beim Entsperren
#define ParamLOG_fOLockTriggerUnlock                 (PT_LockTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockTriggerUnlock)) & LOG_fOLockTriggerUnlockMask) >> LOG_fOLockTriggerUnlockShift)
// Anschließend die Signalverarbeitung
#define ParamLOG_fOLockResetQueue                    (PT_LockResetQueue)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockResetQueue)) & LOG_fOLockResetQueueMask) >> LOG_fOLockResetQueueShift)
// Art der Verknüpfung
#define ParamLOG_fOLockKind                          (PT_KORelInput)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockKind)) & LOG_fOLockKindMask)
// Sperre verbinden mit Kanal Nr.
#define ParamLOG_fOLockFunction                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockFunction)))
// Sperre verbinden mit Kanal Nr.
#define ParamLOG_fOLockFunctionRel                   ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockFunctionRel)))
// Wert für EIN senden?
#define ParamLOG_fOOnAll                             (PT_OutputSend)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnAll)))
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
#define ParamLOG_fOOnDpt16Str                        (knx.paramString(LOG_ParamCalcIndex(LOG_fOOnDpt16), LOG_fOOnDpt16Length))
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt17                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt17)))
//     Wert für EIN senden als (3-Byte-RGB)
#define ParamLOG_fOOnRGB                             ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnRGB)) & LOG_fOOnRGBMask) >> LOG_fOOnRGBShift)
//     Status-LED Kanal
#define ParamLOG_fOOnLedProvider                     (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnLedProvider)) & LOG_fOOnLedProviderMask)
//     Status-LED Effekt
#define ParamLOG_fOOnLedEffect                       (PT_StatusLedEffect)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnLedEffect)) & LOG_fOOnLedEffectMask)
//     Status-LED Effektdauer
#define ParamLOG_fOOnLedDuration                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnLedDuration)))
// 
#define ParamLOG_fOOnPAArea                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPAArea)) & LOG_fOOnPAAreaMask) >> LOG_fOOnPAAreaShift)
// 
#define ParamLOG_fOOnPALine                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPALine)) & LOG_fOOnPALineMask)
// 
#define ParamLOG_fOOnPADevice                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPADevice)))
//     Wert für EIN ermitteln als
#define ParamLOG_fOOnFunction                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKOKind                          (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOKind)) & LOG_fOOnKOKindMask) >> LOG_fOOnKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumber                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumberRel                     ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOnKODpt                           (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKODpt)))
//     Wert für EIN an ein zusätzliches    KO senden?
#define ParamLOG_fOOnKOSend                          (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOSend)) & LOG_fOOnKOSendMask) >> LOG_fOOnKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumber                    (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumberRel                 ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumberRel)))
// Wert für AUS senden?
#define ParamLOG_fOOffAll                            (PT_OutputSend)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffAll)))
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
#define ParamLOG_fOOffDpt16Str                       (knx.paramString(LOG_ParamCalcIndex(LOG_fOOffDpt16), LOG_fOOffDpt16Length))
//     Wert für AUS senden als 
#define ParamLOG_fOOffDpt17                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt17)))
//     Wert für AUS senden als (3-Byte-RGB)
#define ParamLOG_fOOffRGB                            ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffRGB)) & LOG_fOOffRGBMask) >> LOG_fOOffRGBShift)
//     Status-LED-Kanal
#define ParamLOG_fOOffLedProvider                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffLedProvider)) & LOG_fOOffLedProviderMask)
//     Status-LED Effekt
#define ParamLOG_fOOffLedEffect                      (PT_StatusLedEffect)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffLedEffect)) & LOG_fOOffLedEffectMask)
//     Status-LED Effektdauer
#define ParamLOG_fOOffLedDuration                    (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffLedDuration)))
// 
#define ParamLOG_fOOffPAArea                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPAArea)) & LOG_fOOffPAAreaMask) >> LOG_fOOffPAAreaShift)
// 
#define ParamLOG_fOOffPALine                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPALine)) & LOG_fOOffPALineMask)
// 
#define ParamLOG_fOOffPADevice                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPADevice)))
//     Wert für AUS ermitteln als
#define ParamLOG_fOOffFunction                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKOKind                         (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOKind)) & LOG_fOOffKOKindMask) >> LOG_fOOffKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumber                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumberRel                    ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOffKODpt                          (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKODpt)))
//     Wert für AUS an ein zusätzliches    KO senden?
#define ParamLOG_fOOffKOSend                         (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOSend)) & LOG_fOOffKOSendMask) >> LOG_fOOffKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumber                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumberRel                ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumberRel)))

// deprecated
#define LOG_KoOffset 100

// Communication objects per channel (multiple occurrence)
#define LOG_KoBlockOffset 100
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

#define FCB_VisibleChannels                     7682      // uint8_t

// Verfügbare Kanäle
#define ParamFCB_VisibleChannels                     (knx.paramByte(FCB_VisibleChannels))

#define FCB_ChannelCount 15

// Parameter per channel
#define FCB_ParamBlockOffset 7683
#define FCB_ParamBlockSize 81
#define FCB_ParamCalcIndex(index) (index + FCB_ParamBlockOffset + _channelIndex * FCB_ParamBlockSize)

#define FCB_CHChannelType                        0      // 8 Bits, Bit 7-0
#define FCB_CHChannelDisabled                    1      // 1 Bit, Bit 7
#define     FCB_CHChannelDisabledMask 0x80
#define     FCB_CHChannelDisabledShift 7
#define FCB_CHLogicKo0D                          2      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo0DMask 0xC0
#define     FCB_CHLogicKo0DShift 6
#define FCB_CHLogicKo1D                          2      // 2 Bits, Bit 5-4
#define     FCB_CHLogicKo1DMask 0x30
#define     FCB_CHLogicKo1DShift 4
#define FCB_CHLogicKo2D                          2      // 2 Bits, Bit 3-2
#define     FCB_CHLogicKo2DMask 0x0C
#define     FCB_CHLogicKo2DShift 2
#define FCB_CHLogicKo3D                          2      // 2 Bits, Bit 1-0
#define     FCB_CHLogicKo3DMask 0x03
#define     FCB_CHLogicKo3DShift 0
#define FCB_CHLogicKo4D                          3      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo4DMask 0xC0
#define     FCB_CHLogicKo4DShift 6
#define FCB_CHLogicKo5D                          3      // 2 Bits, Bit 5-4
#define     FCB_CHLogicKo5DMask 0x30
#define     FCB_CHLogicKo5DShift 4
#define FCB_CHLogicKo6D                          3      // 2 Bits, Bit 3-2
#define     FCB_CHLogicKo6DMask 0x0C
#define     FCB_CHLogicKo6DShift 2
#define FCB_CHLogicKo7D                          3      // 2 Bits, Bit 1-0
#define     FCB_CHLogicKo7DMask 0x03
#define     FCB_CHLogicKo7DShift 0
#define FCB_CHLogicKo8D                          4      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo8DMask 0xC0
#define     FCB_CHLogicKo8DShift 6
#define FCB_CHLogicOutInv                        4      // 1 Bit, Bit 4
#define     FCB_CHLogicOutInvMask 0x10
#define     FCB_CHLogicOutInvShift 4
#define FCB_CHLogicBehavOut                      4      // 1 Bit, Bit 3
#define     FCB_CHLogicBehavOutMask 0x08
#define     FCB_CHLogicBehavOutShift 3
#define FCB_CHLogicBehavKo0                      5      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo0Mask 0xF0
#define     FCB_CHLogicBehavKo0Shift 4
#define FCB_CHLogicBehavKo1                      5      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo1Mask 0x0F
#define     FCB_CHLogicBehavKo1Shift 0
#define FCB_CHLogicBehavKo2                      6      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo2Mask 0xF0
#define     FCB_CHLogicBehavKo2Shift 4
#define FCB_CHLogicBehavKo3                      6      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo3Mask 0x0F
#define     FCB_CHLogicBehavKo3Shift 0
#define FCB_CHLogicBehavKo4                      7      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo4Mask 0xF0
#define     FCB_CHLogicBehavKo4Shift 4
#define FCB_CHLogicBehavKo5                      7      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo5Mask 0x0F
#define     FCB_CHLogicBehavKo5Shift 0
#define FCB_CHLogicBehavKo6                      8      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo6Mask 0xF0
#define     FCB_CHLogicBehavKo6Shift 4
#define FCB_CHLogicBehavKo7                      8      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo7Mask 0x0F
#define     FCB_CHLogicBehavKo7Shift 0
#define FCB_CHLogicBehavKo8                      9      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo8Mask 0xF0
#define     FCB_CHLogicBehavKo8Shift 4
#define FCB_CHBayesianPrior                     10      // uint8_t
#define FCB_CHBayesianThreshold                 11      // uint8_t
#define FCB_CHBayesianEnableProbOutput          12      // 1 Bit, Bit 7
#define     FCB_CHBayesianEnableProbOutputMask 0x80
#define     FCB_CHBayesianEnableProbOutputShift 7
#define FCB_CHLogicKo0BayesProbTrue             15      // uint8_t
#define FCB_CHLogicKo0BayesProbFalse            16      // uint8_t
#define FCB_CHLogicKo1BayesProbTrue             17      // uint8_t
#define FCB_CHLogicKo1BayesProbFalse            18      // uint8_t
#define FCB_CHLogicKo2BayesProbTrue             19      // uint8_t
#define FCB_CHLogicKo2BayesProbFalse            20      // uint8_t
#define FCB_CHLogicKo3BayesProbTrue             21      // uint8_t
#define FCB_CHLogicKo3BayesProbFalse            22      // uint8_t
#define FCB_CHLogicKo4BayesProbTrue             23      // uint8_t
#define FCB_CHLogicKo4BayesProbFalse            24      // uint8_t
#define FCB_CHLogicKo5BayesProbTrue             25      // uint8_t
#define FCB_CHLogicKo5BayesProbFalse            26      // uint8_t
#define FCB_CHLogicKo6BayesProbTrue             27      // uint8_t
#define FCB_CHLogicKo6BayesProbFalse            28      // uint8_t
#define FCB_CHLogicKo7BayesProbTrue             29      // uint8_t
#define FCB_CHLogicKo7BayesProbFalse            30      // uint8_t
#define FCB_CHLogicKo8BayesProbTrue             31      // uint8_t
#define FCB_CHLogicKo8BayesProbFalse            32      // uint8_t
#define FCB_CHPrioKo0D                           2      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo0DMask 0xC0
#define     FCB_CHPrioKo0DShift 6
#define FCB_CHPrioKo1D                           2      // 2 Bits, Bit 5-4
#define     FCB_CHPrioKo1DMask 0x30
#define     FCB_CHPrioKo1DShift 4
#define FCB_CHPrioKo2D                           2      // 2 Bits, Bit 3-2
#define     FCB_CHPrioKo2DMask 0x0C
#define     FCB_CHPrioKo2DShift 2
#define FCB_CHPrioKo3D                           2      // 2 Bits, Bit 1-0
#define     FCB_CHPrioKo3DMask 0x03
#define     FCB_CHPrioKo3DShift 0
#define FCB_CHPrioKo4D                           3      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo4DMask 0xC0
#define     FCB_CHPrioKo4DShift 6
#define FCB_CHPrioKo5D                           3      // 2 Bits, Bit 5-4
#define     FCB_CHPrioKo5DMask 0x30
#define     FCB_CHPrioKo5DShift 4
#define FCB_CHPrioKo6D                           3      // 2 Bits, Bit 3-2
#define     FCB_CHPrioKo6DMask 0x0C
#define     FCB_CHPrioKo6DShift 2
#define FCB_CHPrioKo7D                           3      // 2 Bits, Bit 1-0
#define     FCB_CHPrioKo7DMask 0x03
#define     FCB_CHPrioKo7DShift 0
#define FCB_CHPrioKo8D                           4      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo8DMask 0xC0
#define     FCB_CHPrioKo8DShift 6
#define FCB_CHPrioOutputType                     4      // 2 Bits, Bit 5-4
#define     FCB_CHPrioOutputTypeMask 0x30
#define     FCB_CHPrioOutputTypeShift 4
#define FCB_CHPrioOutPKo0                        5      // uint8_t
#define FCB_CHPrioOutByteKo0                     5      // uint8_t
#define FCB_CHPrioOutSceneKo0                    5      // uint8_t
#define FCB_CHPrioOutPKo1                        6      // uint8_t
#define FCB_CHPrioOutByteKo1                     6      // uint8_t
#define FCB_CHPrioOutSceneKo1                    6      // uint8_t
#define FCB_CHPrioOutPKo2                        7      // uint8_t
#define FCB_CHPrioOutByteKo2                     7      // uint8_t
#define FCB_CHPrioOutSceneKo2                    7      // uint8_t
#define FCB_CHPrioOutPKo3                        8      // uint8_t
#define FCB_CHPrioOutByteKo3                     8      // uint8_t
#define FCB_CHPrioOutSceneKo3                    8      // uint8_t
#define FCB_CHPrioOutPKo4                        9      // uint8_t
#define FCB_CHPrioOutByteKo4                     9      // uint8_t
#define FCB_CHPrioOutSceneKo4                    9      // uint8_t
#define FCB_CHPrioOutPKo5                       10      // uint8_t
#define FCB_CHPrioOutByteKo5                    10      // uint8_t
#define FCB_CHPrioOutSceneKo5                   10      // uint8_t
#define FCB_CHPrioOutPKo6                       11      // uint8_t
#define FCB_CHPrioOutByteKo6                    11      // uint8_t
#define FCB_CHPrioOutSceneKo6                   11      // uint8_t
#define FCB_CHPrioOutPKo7                       12      // uint8_t
#define FCB_CHPrioOutByteKo7                    12      // uint8_t
#define FCB_CHPrioOutSceneKo7                   12      // uint8_t
#define FCB_CHPrioOutPKo8                       13      // uint8_t
#define FCB_CHPrioOutByteKo8                    13      // uint8_t
#define FCB_CHPrioOutSceneKo8                   13      // uint8_t
#define FCB_CHPrioOutPDefault                   14      // uint8_t
#define FCB_CHPrioOutByteDefault                14      // uint8_t
#define FCB_CHPrioOutSceneDefault               14      // uint8_t
#define FCB_CHPrioBehavKo0                      15      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo0Mask 0xF0
#define     FCB_CHPrioBehavKo0Shift 4
#define FCB_CHPrioBehavKo1                      15      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo1Mask 0x0F
#define     FCB_CHPrioBehavKo1Shift 0
#define FCB_CHPrioBehavKo2                      16      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo2Mask 0xF0
#define     FCB_CHPrioBehavKo2Shift 4
#define FCB_CHPrioBehavKo3                      16      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo3Mask 0x0F
#define     FCB_CHPrioBehavKo3Shift 0
#define FCB_CHPrioBehavKo4                      17      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo4Mask 0xF0
#define     FCB_CHPrioBehavKo4Shift 4
#define FCB_CHPrioBehavKo5                      17      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo5Mask 0x0F
#define     FCB_CHPrioBehavKo5Shift 0
#define FCB_CHPrioBehavKo6                      18      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo6Mask 0xF0
#define     FCB_CHPrioBehavKo6Shift 4
#define FCB_CHPrioBehavKo7                      18      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo7Mask 0x0F
#define     FCB_CHPrioBehavKo7Shift 0
#define FCB_CHPrioBehavKo8                      19      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo8Mask 0xF0
#define     FCB_CHPrioBehavKo8Shift 4
#define FCB_CHPrioBehavOut                      19      // 1 Bit, Bit 3
#define     FCB_CHPrioBehavOutMask 0x08
#define     FCB_CHPrioBehavOutShift 3
#define FCB_CHAggWeight                          2      // 1 Bit, Bit 7
#define     FCB_CHAggWeightMask 0x80
#define     FCB_CHAggWeightShift 7
#define FCB_CHAggType                            2      // 7 Bits, Bit 6-0
#define     FCB_CHAggTypeMask 0x7F
#define     FCB_CHAggTypeShift 0
#define FCB_CHAggKo0D                            3      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo0DMask 0xC0
#define     FCB_CHAggKo0DShift 6
#define FCB_CHAggKo1D                            3      // 2 Bits, Bit 5-4
#define     FCB_CHAggKo1DMask 0x30
#define     FCB_CHAggKo1DShift 4
#define FCB_CHAggKo2D                            3      // 2 Bits, Bit 3-2
#define     FCB_CHAggKo2DMask 0x0C
#define     FCB_CHAggKo2DShift 2
#define FCB_CHAggKo3D                            3      // 2 Bits, Bit 1-0
#define     FCB_CHAggKo3DMask 0x03
#define     FCB_CHAggKo3DShift 0
#define FCB_CHAggKo4D                            4      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo4DMask 0xC0
#define     FCB_CHAggKo4DShift 6
#define FCB_CHAggKo5D                            4      // 2 Bits, Bit 5-4
#define     FCB_CHAggKo5DMask 0x30
#define     FCB_CHAggKo5DShift 4
#define FCB_CHAggKo6D                            4      // 2 Bits, Bit 3-2
#define     FCB_CHAggKo6DMask 0x0C
#define     FCB_CHAggKo6DShift 2
#define FCB_CHAggKo7D                            4      // 2 Bits, Bit 1-0
#define     FCB_CHAggKo7DMask 0x03
#define     FCB_CHAggKo7DShift 0
#define FCB_CHAggKo8D                            5      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo8DMask 0xC0
#define     FCB_CHAggKo8DShift 6
#define FCB_CHAggBehavOut                        5      // 1 Bit, Bit 5
#define     FCB_CHAggBehavOutMask 0x20
#define     FCB_CHAggBehavOutShift 5
#define FCB_CHAggOutputRounding                  5      // 1 Bit, Bit 3
#define     FCB_CHAggOutputRoundingMask 0x08
#define     FCB_CHAggOutputRoundingShift 3
#define FCB_CHAggOutputOverflow                  5      // 2 Bits, Bit 2-1
#define     FCB_CHAggOutputOverflowMask 0x06
#define     FCB_CHAggOutputOverflowShift 1
#define FCB_CHAggInputDpt                        6      // 8 Bits, Bit 7-0
#define FCB_CHAggOutputDptEff                    7      // 8 Bits, Bit 7-0
#define FCB_CHAggKo0W                            8      // int8_t
#define FCB_CHAggKo1W                            9      // int8_t
#define FCB_CHAggKo2W                           10      // int8_t
#define FCB_CHAggKo3W                           11      // int8_t
#define FCB_CHAggKo4W                           12      // int8_t
#define FCB_CHAggKo5W                           13      // int8_t
#define FCB_CHAggKo6W                           14      // int8_t
#define FCB_CHAggKo7W                           15      // int8_t
#define FCB_CHAggKo8W                           16      // int8_t
#define FCB_CHCountDownTimeStartKo               2      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownTimeStartKoMask 0xF0
#define     FCB_CHCountDownTimeStartKoShift 4
#define FCB_CHCountDownDelayBase                 3      // 2 Bits, Bit 7-6
#define     FCB_CHCountDownDelayBaseMask 0xC0
#define     FCB_CHCountDownDelayBaseShift 6
#define FCB_CHCountDownDelayTime                 3      // 14 Bits, Bit 13-0
#define     FCB_CHCountDownDelayTimeMask 0x3FFF
#define     FCB_CHCountDownDelayTimeShift 0
#define FCB_CHCountDownTimeOffset                5      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownTimeOffsetMask 0xF0
#define     FCB_CHCountDownTimeOffsetShift 4
#define FCB_CHCountDownTrigger                   5      // 4 Bits, Bit 3-0
#define     FCB_CHCountDownTriggerMask 0x0F
#define     FCB_CHCountDownTriggerShift 0
#define FCB_CHCountDownTemplate                  6      // char*, 14 Byte
#define     FCB_CHCountDownTemplateLength 14
#define FCB_CHCountDownTemplate1h               20      // char*, 14 Byte
#define     FCB_CHCountDownTemplate1hLength 14
#define FCB_CHCountDownTemplate1m               34      // char*, 14 Byte
#define     FCB_CHCountDownTemplate1mLength 14
#define FCB_CHCountDownTemplateEnd              48      // char*, 14 Byte
#define     FCB_CHCountDownTemplateEndLength 14
#define FCB_CHCountDownTextPause                62      // char*, 1 Byte
#define     FCB_CHCountDownTextPauseLength 1
#define FCB_CHCountDownTextRun                  63      // char*, 1 Byte
#define     FCB_CHCountDownTextRunLength 1
#define FCB_CHCountDownCounterKo                64      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownCounterKoMask 0xF0
#define     FCB_CHCountDownCounterKoShift 4
#define FCB_CHCountDownTextKo                   64      // 2 Bits, Bit 3-2
#define     FCB_CHCountDownTextKoMask 0x0C
#define     FCB_CHCountDownTextKoShift 2
#define FCB_CHCountDownTemplateStopp            65      // char*, 14 Byte
#define     FCB_CHCountDownTemplateStoppLength 14
#define FCB_CHCountDownMaxDelayBase             79      // 2 Bits, Bit 7-6
#define     FCB_CHCountDownMaxDelayBaseMask 0xC0
#define     FCB_CHCountDownMaxDelayBaseShift 6
#define FCB_CHCountDownMaxDelayTime             79      // 14 Bits, Bit 13-0
#define     FCB_CHCountDownMaxDelayTimeMask 0x3FFF
#define     FCB_CHCountDownMaxDelayTimeShift 0
#define FCB_CHMonitoringValueType                2      // 8 Bits, Bit 7-0
#define FCB_CHMonitoringWDEnabled                3      // 1 Bit, Bit 7
#define     FCB_CHMonitoringWDEnabledMask 0x80
#define     FCB_CHMonitoringWDEnabledShift 7
#define FCB_CHMonitoringWDTTimeoutDelayBase      4      // 2 Bits, Bit 7-6
#define     FCB_CHMonitoringWDTTimeoutDelayBaseMask 0xC0
#define     FCB_CHMonitoringWDTTimeoutDelayBaseShift 6
#define FCB_CHMonitoringWDTTimeoutDelayTime      4      // 14 Bits, Bit 13-0
#define     FCB_CHMonitoringWDTTimeoutDelayTimeMask 0x3FFF
#define     FCB_CHMonitoringWDTTimeoutDelayTimeShift 0
#define FCB_CHMonitoringWDBehavior               6      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringWDBehaviorMask 0xF0
#define     FCB_CHMonitoringWDBehaviorShift 4
#define FCB_CHMonitoringStart                    6      // 2 Bits, Bit 3-2
#define     FCB_CHMonitoringStartMask 0x0C
#define     FCB_CHMonitoringStartShift 2
#define FCB_CHMonitoringWDDpt1                   7      // 8 Bits, Bit 7-0
#define FCB_CHMonitoringWDDpt5                   7      // uint8_t
#define FCB_CHMonitoringWDDpt5001                7      // uint8_t
#define FCB_CHMonitoringWDDpt6                   7      // int8_t
#define FCB_CHMonitoringWDDpt7                   7      // uint16_t
#define FCB_CHMonitoringWDDpt8                   7      // int16_t
#define FCB_CHMonitoringWDDpt9                   7      // float (4 Byte)
#define FCB_CHMonitoringWDDpt12                  7      // uint32_t
#define FCB_CHMonitoringWDDpt13                  7      // int32_t
#define FCB_CHMonitoringWDDpt14                  7      // float (4 Byte)
#define FCB_CHMonitoringWDDpt16                  7      // char*, 14 Byte
#define     FCB_CHMonitoringWDDpt16Length 14
#define FCB_CHMonitoringMin                     22      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringMinMask 0xF0
#define     FCB_CHMonitoringMinShift 4
#define FCB_CHMonitoringMinDpt1                 23      // 1 Bit, Bit 7
#define     FCB_CHMonitoringMinDpt1Mask 0x80
#define     FCB_CHMonitoringMinDpt1Shift 7
#define FCB_CHMonitoringMinDpt5                 23      // uint8_t
#define FCB_CHMonitoringMinDpt5001              23      // uint8_t
#define FCB_CHMonitoringMinDpt6                 23      // int8_t
#define FCB_CHMonitoringMinDpt7                 23      // uint16_t
#define FCB_CHMonitoringMinDpt8                 23      // int16_t
#define FCB_CHMonitoringMinDpt9                 23      // float (4 Byte)
#define FCB_CHMonitoringMinDpt12                23      // uint32_t
#define FCB_CHMonitoringMinDpt13                23      // int32_t
#define FCB_CHMonitoringMinDpt14                23      // float (4 Byte)
#define FCB_CHMonitoringMax                     27      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringMaxMask 0xF0
#define     FCB_CHMonitoringMaxShift 4
#define FCB_CHMonitoringMaxDpt1                 28      // 1 Bit, Bit 7
#define     FCB_CHMonitoringMaxDpt1Mask 0x80
#define     FCB_CHMonitoringMaxDpt1Shift 7
#define FCB_CHMonitoringMaxDpt5                 28      // uint8_t
#define FCB_CHMonitoringMaxDpt5001              28      // uint8_t
#define FCB_CHMonitoringMaxDpt6                 28      // int8_t
#define FCB_CHMonitoringMaxDpt7                 28      // uint16_t
#define FCB_CHMonitoringMaxDpt8                 28      // int16_t
#define FCB_CHMonitoringMaxDpt9                 28      // float (4 Byte)
#define FCB_CHMonitoringMaxDpt12                28      // uint32_t
#define FCB_CHMonitoringMaxDpt13                28      // int32_t
#define FCB_CHMonitoringMaxDpt14                28      // float (4 Byte)
#define FCB_CHMonitoringOutput                  32      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringOutputMask 0xF0
#define     FCB_CHMonitoringOutputShift 4
#define FCB_CHSelectionValueType                 2      // 8 Bits, Bit 7-0
#define FCB_CHSelectionType                      3      // 8 Bits, Bit 7-0
#define FCB_CHSelectionSwitching                 4      // 4 Bits, Bit 7-4
#define     FCB_CHSelectionSwitchingMask 0xF0
#define     FCB_CHSelectionSwitchingShift 4
#define FCB_CHSelectionStateOutput               4      // 1 Bit, Bit 3
#define     FCB_CHSelectionStateOutputMask 0x08
#define     FCB_CHSelectionStateOutputShift 3
#define FCB_CHBlinkerOnDelayBase                 4      // 2 Bits, Bit 7-6
#define     FCB_CHBlinkerOnDelayBaseMask 0xC0
#define     FCB_CHBlinkerOnDelayBaseShift 6
#define FCB_CHBlinkerOnDelayTime                 4      // 14 Bits, Bit 13-0
#define     FCB_CHBlinkerOnDelayTimeMask 0x3FFF
#define     FCB_CHBlinkerOnDelayTimeShift 0
#define FCB_CHBlinkerOffDelayBase                6      // 2 Bits, Bit 7-6
#define     FCB_CHBlinkerOffDelayBaseMask 0xC0
#define     FCB_CHBlinkerOffDelayBaseShift 6
#define FCB_CHBlinkerOffDelayTime                6      // 14 Bits, Bit 13-0
#define     FCB_CHBlinkerOffDelayTimeMask 0x3FFF
#define     FCB_CHBlinkerOffDelayTimeShift 0
#define FCB_CHBlinkerStart                       8      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerStartMask 0xF0
#define     FCB_CHBlinkerStartShift 4
#define FCB_CHBlinkerStop                        8      // 4 Bits, Bit 3-0
#define     FCB_CHBlinkerStopMask 0x0F
#define     FCB_CHBlinkerStopShift 0
#define FCB_CHBlinkerBreak                       9      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerBreakMask 0xF0
#define     FCB_CHBlinkerBreakShift 4
#define FCB_CHBlinkerBreakWithoutBreak           9      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerBreakWithoutBreakMask 0xF0
#define     FCB_CHBlinkerBreakWithoutBreakShift 4
#define FCB_CHBlinkerOutputDpt                  10      // 8 Bits, Bit 7-0
#define FCB_CHBlinkerOnPercentage               11      // uint8_t
#define FCB_CHBlinkerOffPercentage              12      // uint8_t
#define FCB_CHBlinkerCount                      13      // 8 Bits, Bit 7-0
#define FCB_CHBlinkerStartAnzahl                14      // 1 Bit, Bit 7
#define     FCB_CHBlinkerStartAnzahlMask 0x80
#define     FCB_CHBlinkerStartAnzahlShift 7
#define FCB_CHFormatString                       2      // char*, 28 Byte
#define     FCB_CHFormatStringLength 28
#define FCB_CHFormatOff                         30      // char*, 14 Byte
#define     FCB_CHFormatOffLength 14
#define FCB_CHFormatOn                          44      // char*, 14 Byte
#define     FCB_CHFormatOnLength 14
#define FCB_CHFormatThousand                    58      // char*, 1 Byte
#define     FCB_CHFormatThousandLength 1
#define FCB_CHFormatIn1                         59      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat1                 60      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat1Mask 0xC0
#define     FCB_CHFormatRoundFloat1Shift 6
#define FCB_CHFormatRound1                      60      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound1Mask 0xC0
#define     FCB_CHFormatRound1Shift 6
#define FCB_CHFCBFormatRound5_1                 60      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_1Mask 0x20
#define     FCB_CHFCBFormatRound5_1Shift 5
#define FCB_CHFormatDecimalPlaces1              60      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces1Mask 0x0F
#define     FCB_CHFormatDecimalPlaces1Shift 0
#define FCB_CHFormatSignificant1                60      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant1Mask 0x0F
#define     FCB_CHFormatSignificant1Shift 0
#define FCB_CHFormatFillupPrecomma1             61      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma1Mask 0xF0
#define     FCB_CHFormatFillupPrecomma1Shift 4
#define FCB_CHFormatFillupMode1                 61      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode1Mask 0xF0
#define     FCB_CHFormatFillupMode1Shift 4
#define FCB_CHFormatFillupAfterComma1           61      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma1Mask 0x0F
#define     FCB_CHFormatFillupAfterComma1Shift 0
#define FCB_CHFCBFormatRoundType1               62      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType1Mask 0xF0
#define     FCB_CHFCBFormatRoundType1Shift 4
#define FCB_CHFormatFillupLength1               62      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength1Mask 0x0F
#define     FCB_CHFormatFillupLength1Shift 0
#define FCB_CHFormatBit1                        60      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn2                         63      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat2                 64      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat2Mask 0xC0
#define     FCB_CHFormatRoundFloat2Shift 6
#define FCB_CHFormatRound2                      64      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound2Mask 0xC0
#define     FCB_CHFormatRound2Shift 6
#define FCB_CHFCBFormatRound5_2                 64      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_2Mask 0x20
#define     FCB_CHFCBFormatRound5_2Shift 5
#define FCB_CHFormatDecimalPlaces2              64      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces2Mask 0x0F
#define     FCB_CHFormatDecimalPlaces2Shift 0
#define FCB_CHFormatSignificant2                64      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant2Mask 0x0F
#define     FCB_CHFormatSignificant2Shift 0
#define FCB_CHFormatFillupPrecomma2             65      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma2Mask 0xF0
#define     FCB_CHFormatFillupPrecomma2Shift 4
#define FCB_CHFormatFillupMode2                 65      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode2Mask 0xF0
#define     FCB_CHFormatFillupMode2Shift 4
#define FCB_CHFormatFillupAfterComma2           65      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma2Mask 0x0F
#define     FCB_CHFormatFillupAfterComma2Shift 0
#define FCB_CHFCBFormatRoundType2               66      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType2Mask 0xF0
#define     FCB_CHFCBFormatRoundType2Shift 4
#define FCB_CHFormatFillupLength2               66      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength2Mask 0x0F
#define     FCB_CHFormatFillupLength2Shift 0
#define FCB_CHFormatBit2                        64      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn3                         67      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat3                 68      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat3Mask 0xC0
#define     FCB_CHFormatRoundFloat3Shift 6
#define FCB_CHFormatRound3                      68      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound3Mask 0xC0
#define     FCB_CHFormatRound3Shift 6
#define FCB_CHFCBFormatRound5_3                 68      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_3Mask 0x20
#define     FCB_CHFCBFormatRound5_3Shift 5
#define FCB_CHFormatDecimalPlaces3              68      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces3Mask 0x0F
#define     FCB_CHFormatDecimalPlaces3Shift 0
#define FCB_CHFormatSignificant3                68      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant3Mask 0x0F
#define     FCB_CHFormatSignificant3Shift 0
#define FCB_CHFormatFillupPrecomma3             69      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma3Mask 0xF0
#define     FCB_CHFormatFillupPrecomma3Shift 4
#define FCB_CHFormatFillupMode3                 69      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode3Mask 0xF0
#define     FCB_CHFormatFillupMode3Shift 4
#define FCB_CHFormatFillupAfterComma3           69      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma3Mask 0x0F
#define     FCB_CHFormatFillupAfterComma3Shift 0
#define FCB_CHFCBFormatRoundType3               70      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType3Mask 0xF0
#define     FCB_CHFCBFormatRoundType3Shift 4
#define FCB_CHFormatFillupLength3               70      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength3Mask 0x0F
#define     FCB_CHFormatFillupLength3Shift 0
#define FCB_CHFormatBit3                        68      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn4                         71      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat4                 72      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat4Mask 0xC0
#define     FCB_CHFormatRoundFloat4Shift 6
#define FCB_CHFormatRound4                      72      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound4Mask 0xC0
#define     FCB_CHFormatRound4Shift 6
#define FCB_CHFCBFormatRound5_4                 72      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_4Mask 0x20
#define     FCB_CHFCBFormatRound5_4Shift 5
#define FCB_CHFormatDecimalPlaces4              72      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces4Mask 0x0F
#define     FCB_CHFormatDecimalPlaces4Shift 0
#define FCB_CHFormatSignificant4                72      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant4Mask 0x0F
#define     FCB_CHFormatSignificant4Shift 0
#define FCB_CHFormatFillupPrecomma4             73      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma4Mask 0xF0
#define     FCB_CHFormatFillupPrecomma4Shift 4
#define FCB_CHFormatFillupMode4                 73      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode4Mask 0xF0
#define     FCB_CHFormatFillupMode4Shift 4
#define FCB_CHFormatFillupAfterComma4           73      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma4Mask 0x0F
#define     FCB_CHFormatFillupAfterComma4Shift 0
#define FCB_CHFCBFormatRoundType4               74      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType4Mask 0xF0
#define     FCB_CHFCBFormatRoundType4Shift 4
#define FCB_CHFormatFillupLength4               74      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength4Mask 0x0F
#define     FCB_CHFormatFillupLength4Shift 0
#define FCB_CHFormatBit4                        72      // 8 Bits, Bit 7-0

// Type
#define ParamFCB_CHChannelType                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHChannelType)))
// Kanal deaktivieren (zu Testzwecken)
#define ParamFCB_CHChannelDisabled                   ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHChannelDisabled)) & FCB_CHChannelDisabledMask))
// Eingang 1
#define ParamFCB_CHLogicKo0D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0D)) & FCB_CHLogicKo0DMask) >> FCB_CHLogicKo0DShift)
// Eingang 2
#define ParamFCB_CHLogicKo1D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1D)) & FCB_CHLogicKo1DMask) >> FCB_CHLogicKo1DShift)
// Eingang 3
#define ParamFCB_CHLogicKo2D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2D)) & FCB_CHLogicKo2DMask) >> FCB_CHLogicKo2DShift)
// Eingang 4
#define ParamFCB_CHLogicKo3D                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3D)) & FCB_CHLogicKo3DMask)
// Eingang 5
#define ParamFCB_CHLogicKo4D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4D)) & FCB_CHLogicKo4DMask) >> FCB_CHLogicKo4DShift)
// Eingang 6
#define ParamFCB_CHLogicKo5D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5D)) & FCB_CHLogicKo5DMask) >> FCB_CHLogicKo5DShift)
// Eingang 7
#define ParamFCB_CHLogicKo6D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6D)) & FCB_CHLogicKo6DMask) >> FCB_CHLogicKo6DShift)
// Eingang 8
#define ParamFCB_CHLogicKo7D                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7D)) & FCB_CHLogicKo7DMask)
// Eingang 9
#define ParamFCB_CHLogicKo8D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8D)) & FCB_CHLogicKo8DMask) >> FCB_CHLogicKo8DShift)
// Invertiert
#define ParamFCB_CHLogicOutInv                       ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicOutInv)) & FCB_CHLogicOutInvMask))
// Sendeverhalten
#define ParamFCB_CHLogicBehavOut                     ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavOut)) & FCB_CHLogicBehavOutMask))
// Initialisierung
#define ParamFCB_CHLogicBehavKo0                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo0)) & FCB_CHLogicBehavKo0Mask) >> FCB_CHLogicBehavKo0Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo1                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo1)) & FCB_CHLogicBehavKo1Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo2                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo2)) & FCB_CHLogicBehavKo2Mask) >> FCB_CHLogicBehavKo2Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo3                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo3)) & FCB_CHLogicBehavKo3Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo4                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo4)) & FCB_CHLogicBehavKo4Mask) >> FCB_CHLogicBehavKo4Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo5                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo5)) & FCB_CHLogicBehavKo5Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo6                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo6)) & FCB_CHLogicBehavKo6Mask) >> FCB_CHLogicBehavKo6Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo7                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo7)) & FCB_CHLogicBehavKo7Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo8                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo8)) & FCB_CHLogicBehavKo8Mask) >> FCB_CHLogicBehavKo8Shift)
// Prior-Wahrscheinlichkeit
#define ParamFCB_CHBayesianPrior                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianPrior)))
// Schwellwert für binären Ausgang
#define ParamFCB_CHBayesianThreshold                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianThreshold)))
// Wahrscheinlichkeits-Ausgang aktivieren
#define ParamFCB_CHBayesianEnableProbOutput          ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianEnableProbOutput)) & FCB_CHBayesianEnableProbOutputMask))
// P(A|E_1)
#define ParamFCB_CHLogicKo0BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0BayesProbTrue)))
// P(A|!E_1)
#define ParamFCB_CHLogicKo0BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0BayesProbFalse)))
// P(A|E_2)
#define ParamFCB_CHLogicKo1BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1BayesProbTrue)))
// P(A|!E_2)
#define ParamFCB_CHLogicKo1BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1BayesProbFalse)))
// P(A|E_3)
#define ParamFCB_CHLogicKo2BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2BayesProbTrue)))
// P(A|!E_3)
#define ParamFCB_CHLogicKo2BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2BayesProbFalse)))
// P(A|E_4)
#define ParamFCB_CHLogicKo3BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3BayesProbTrue)))
// P(A|!E_4)
#define ParamFCB_CHLogicKo3BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3BayesProbFalse)))
// P(A|E_5)
#define ParamFCB_CHLogicKo4BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4BayesProbTrue)))
// P(A|!E_5)
#define ParamFCB_CHLogicKo4BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4BayesProbFalse)))
// P(A|E_6)
#define ParamFCB_CHLogicKo5BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5BayesProbTrue)))
// P(A|!E_6)
#define ParamFCB_CHLogicKo5BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5BayesProbFalse)))
// P(A|E_7)
#define ParamFCB_CHLogicKo6BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6BayesProbTrue)))
// P(A|!E_7)
#define ParamFCB_CHLogicKo6BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6BayesProbFalse)))
// P(A|E_8)
#define ParamFCB_CHLogicKo7BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7BayesProbTrue)))
// P(A|!E_8)
#define ParamFCB_CHLogicKo7BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7BayesProbFalse)))
// P(A|E_9)
#define ParamFCB_CHLogicKo8BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8BayesProbTrue)))
// P(A|!E_9)
#define ParamFCB_CHLogicKo8BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8BayesProbFalse)))
// Eingang 1
#define ParamFCB_CHPrioKo0D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo0D)) & FCB_CHPrioKo0DMask) >> FCB_CHPrioKo0DShift)
// Eingang 2
#define ParamFCB_CHPrioKo1D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo1D)) & FCB_CHPrioKo1DMask) >> FCB_CHPrioKo1DShift)
// Eingang 3
#define ParamFCB_CHPrioKo2D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo2D)) & FCB_CHPrioKo2DMask) >> FCB_CHPrioKo2DShift)
// Eingang 4
#define ParamFCB_CHPrioKo3D                          (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo3D)) & FCB_CHPrioKo3DMask)
// Eingang 5
#define ParamFCB_CHPrioKo4D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo4D)) & FCB_CHPrioKo4DMask) >> FCB_CHPrioKo4DShift)
// Eingang 6
#define ParamFCB_CHPrioKo5D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo5D)) & FCB_CHPrioKo5DMask) >> FCB_CHPrioKo5DShift)
// Eingang 7
#define ParamFCB_CHPrioKo6D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo6D)) & FCB_CHPrioKo6DMask) >> FCB_CHPrioKo6DShift)
// Eingang 8
#define ParamFCB_CHPrioKo7D                          (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo7D)) & FCB_CHPrioKo7DMask)
// Eingang 9
#define ParamFCB_CHPrioKo8D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo8D)) & FCB_CHPrioKo8DMask) >> FCB_CHPrioKo8DShift)
// Type
#define ParamFCB_CHPrioOutputType                    ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutputType)) & FCB_CHPrioOutputTypeMask) >> FCB_CHPrioOutputTypeShift)
// Ausgangswert
#define ParamFCB_CHPrioOutPKo0                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo0)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo0                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo0)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo0                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo0)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo1                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo1)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo1                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo1)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo1                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo1)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo2                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo2)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo2                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo2)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo2                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo2)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo3                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo3)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo3                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo3)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo3                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo3)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo4                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo4)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo4                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo4)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo4                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo4)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo5                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo5)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo5                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo5)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo5                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo5)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo6                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo6)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo6                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo6)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo6                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo6)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo7                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo7)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo7                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo7)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo7                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo7)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo8                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo8)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo8                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo8)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo8                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo8)))
// Ausgangswert wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutPDefault                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPDefault)))
// Ausgangswert wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutByteDefault                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteDefault)))
// Ausgangswert Szenennummer wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutSceneDefault               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneDefault)))
// Initialisierung
#define ParamFCB_CHPrioBehavKo0                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo0)) & FCB_CHPrioBehavKo0Mask) >> FCB_CHPrioBehavKo0Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo1                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo1)) & FCB_CHPrioBehavKo1Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo2                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo2)) & FCB_CHPrioBehavKo2Mask) >> FCB_CHPrioBehavKo2Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo3                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo3)) & FCB_CHPrioBehavKo3Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo4                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo4)) & FCB_CHPrioBehavKo4Mask) >> FCB_CHPrioBehavKo4Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo5                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo5)) & FCB_CHPrioBehavKo5Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo6                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo6)) & FCB_CHPrioBehavKo6Mask) >> FCB_CHPrioBehavKo6Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo7                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo7)) & FCB_CHPrioBehavKo7Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo8                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo8)) & FCB_CHPrioBehavKo8Mask) >> FCB_CHPrioBehavKo8Shift)
// Sendeverhalten
#define ParamFCB_CHPrioBehavOut                      ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavOut)) & FCB_CHPrioBehavOutMask))
// Gewichtung der Eingänge
#define ParamFCB_CHAggWeight                         ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggWeight)) & FCB_CHAggWeightMask))
// Funktion
#define ParamFCB_CHAggType                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggType)) & FCB_CHAggTypeMask)
// Eingang 1
#define ParamFCB_CHAggKo0D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo0D)) & FCB_CHAggKo0DMask) >> FCB_CHAggKo0DShift)
// Eingang 2
#define ParamFCB_CHAggKo1D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo1D)) & FCB_CHAggKo1DMask) >> FCB_CHAggKo1DShift)
// Eingang 3
#define ParamFCB_CHAggKo2D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo2D)) & FCB_CHAggKo2DMask) >> FCB_CHAggKo2DShift)
// Eingang 4
#define ParamFCB_CHAggKo3D                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo3D)) & FCB_CHAggKo3DMask)
// Eingang 5
#define ParamFCB_CHAggKo4D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo4D)) & FCB_CHAggKo4DMask) >> FCB_CHAggKo4DShift)
// Eingang 6
#define ParamFCB_CHAggKo5D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo5D)) & FCB_CHAggKo5DMask) >> FCB_CHAggKo5DShift)
// Eingang 7
#define ParamFCB_CHAggKo6D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo6D)) & FCB_CHAggKo6DMask) >> FCB_CHAggKo6DShift)
// Eingang 8
#define ParamFCB_CHAggKo7D                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo7D)) & FCB_CHAggKo7DMask)
// Eingang 9
#define ParamFCB_CHAggKo8D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo8D)) & FCB_CHAggKo8DMask) >> FCB_CHAggKo8DShift)
// Sendeverhalten
#define ParamFCB_CHAggBehavOut                       ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggBehavOut)) & FCB_CHAggBehavOutMask))
// Rundungsmodus
#define ParamFCB_CHAggOutputRounding                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputRounding)) & FCB_CHAggOutputRoundingMask))
// Bei Überschreiten des Wertebereichs
#define ParamFCB_CHAggOutputOverflow                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputOverflow)) & FCB_CHAggOutputOverflowMask) >> FCB_CHAggOutputOverflowShift)
// Wertetype / DPT
#define ParamFCB_CHAggInputDpt                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggInputDpt)))
// DPT Ausgang
#define ParamFCB_CHAggOutputDptEff                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputDptEff)))
// Gewicht Eingang 1
#define ParamFCB_CHAggKo0W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo0W)))
// Gewicht Eingang 2
#define ParamFCB_CHAggKo1W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo1W)))
// Gewicht Eingang 3
#define ParamFCB_CHAggKo2W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo2W)))
// Gewicht Eingang 4
#define ParamFCB_CHAggKo3W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo3W)))
// Gewicht Eingang 5
#define ParamFCB_CHAggKo4W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo4W)))
// Gewicht Eingang 6
#define ParamFCB_CHAggKo5W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo5W)))
// Gewicht Eingang 7
#define ParamFCB_CHAggKo6W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo6W)))
// Gewicht Eingang 8
#define ParamFCB_CHAggKo7W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo7W)))
// Gewicht Eingang 9
#define ParamFCB_CHAggKo8W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo8W)))
// Start mit Zeit
#define ParamFCB_CHCountDownTimeStartKo              ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTimeStartKo)) & FCB_CHCountDownTimeStartKoMask) >> FCB_CHCountDownTimeStartKoShift)
// Ablaufzeit Einheit
#define ParamFCB_CHCountDownDelayBase                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownDelayBase)) & FCB_CHCountDownDelayBaseMask) >> FCB_CHCountDownDelayBaseShift)
// Ablaufzeit
#define ParamFCB_CHCountDownDelayTime                (knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownDelayTime)) & FCB_CHCountDownDelayTimeMask)
// Ablaufzeit (in Millisekunden)
#define ParamFCB_CHCountDownDelayTimeMS              (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownDelayTime))))
// Laufzeit Verringern / Erhöhen
#define ParamFCB_CHCountDownTimeOffset               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTimeOffset)) & FCB_CHCountDownTimeOffsetMask) >> FCB_CHCountDownTimeOffsetShift)
// Auslöser / Ende
#define ParamFCB_CHCountDownTrigger                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTrigger)) & FCB_CHCountDownTriggerMask)
// Standard
#define ParamFCB_CHCountDownTemplate                 (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate)))
#define ParamFCB_CHCountDownTemplateStr              (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate), FCB_CHCountDownTemplateLength))
// kleiner eine Stunde
#define ParamFCB_CHCountDownTemplate1h               (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1h)))
#define ParamFCB_CHCountDownTemplate1hStr            (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1h), FCB_CHCountDownTemplate1hLength))
// kleiner eine Minute
#define ParamFCB_CHCountDownTemplate1m               (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1m)))
#define ParamFCB_CHCountDownTemplate1mStr            (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1m), FCB_CHCountDownTemplate1mLength))
// Ende
#define ParamFCB_CHCountDownTemplateEnd              (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplateEnd)))
#define ParamFCB_CHCountDownTemplateEndStr           (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplateEnd), FCB_CHCountDownTemplateEndLength))
// Pause
#define ParamFCB_CHCountDownTextPause                (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTextPause)))
#define ParamFCB_CHCountDownTextPauseStr             (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTextPause), FCB_CHCountDownTextPauseLength))
// Läuft
#define ParamFCB_CHCountDownTextRun                  (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTextRun)))
#define ParamFCB_CHCountDownTextRunStr               (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTextRun), FCB_CHCountDownTextRunLength))
// Zähler
#define ParamFCB_CHCountDownCounterKo                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownCounterKo)) & FCB_CHCountDownCounterKoMask) >> FCB_CHCountDownCounterKoShift)
// Text
#define ParamFCB_CHCountDownTextKo                   ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTextKo)) & FCB_CHCountDownTextKoMask) >> FCB_CHCountDownTextKoShift)
// Stopp
#define ParamFCB_CHCountDownTemplateStopp            (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplateStopp)))
#define ParamFCB_CHCountDownTemplateStoppStr         (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplateStopp), FCB_CHCountDownTemplateStoppLength))
// Maximalzeit Einheit
#define ParamFCB_CHCountDownMaxDelayBase             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayBase)) & FCB_CHCountDownMaxDelayBaseMask) >> FCB_CHCountDownMaxDelayBaseShift)
// Maximalzeit
#define ParamFCB_CHCountDownMaxDelayTime             (knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayTime)) & FCB_CHCountDownMaxDelayTimeMask)
// Maximalzeit (in Millisekunden)
#define ParamFCB_CHCountDownMaxDelayTimeMS           (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayTime))))
// Werttype
#define ParamFCB_CHMonitoringValueType               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringValueType)))
// Zeitüberwachung aktiv
#define ParamFCB_CHMonitoringWDEnabled               ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDEnabled)) & FCB_CHMonitoringWDEnabledMask))
// Watchdog Zeitbasis
#define ParamFCB_CHMonitoringWDTTimeoutDelayBase     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayBase)) & FCB_CHMonitoringWDTTimeoutDelayBaseMask) >> FCB_CHMonitoringWDTTimeoutDelayBaseShift)
// Watchdog Zeit
#define ParamFCB_CHMonitoringWDTTimeoutDelayTime     (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayTime)) & FCB_CHMonitoringWDTTimeoutDelayTimeMask)
// Watchdog Zeit (in Millisekunden)
#define ParamFCB_CHMonitoringWDTTimeoutDelayTimeMS   (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayTime))))
// Verhalten bei Zeitüberschreitung
#define ParamFCB_CHMonitoringWDBehavior              ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDBehavior)) & FCB_CHMonitoringWDBehaviorMask) >> FCB_CHMonitoringWDBehaviorShift)
// Verhalten beim Start
#define ParamFCB_CHMonitoringStart                   ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringStart)) & FCB_CHMonitoringStartMask) >> FCB_CHMonitoringStartShift)
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt1                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt1)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt5                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt5)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt5001               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt5001)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt6                  ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt6)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt7                  (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt7)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt8                  ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt8)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt9                  (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt9), Float_Enc_IEEE754Single))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt12                 (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt12)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt13                 ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt13)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt14                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt14), Float_Enc_IEEE754Single))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt16                 (knx.paramData(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt16)))
#define ParamFCB_CHMonitoringWDDpt16Str              (knx.paramString(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt16), FCB_CHMonitoringWDDpt16Length))
// Verhalten bei Wertunterschreitung
#define ParamFCB_CHMonitoringMin                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMin)) & FCB_CHMonitoringMinMask) >> FCB_CHMonitoringMinShift)
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt1)) & FCB_CHMonitoringMinDpt1Mask))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt5                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt5)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt5001              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt5001)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt6                 ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt6)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt7                 (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt7)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt8                 ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt8)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt9                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt9), Float_Enc_IEEE754Single))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt12                (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt12)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt13                ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt13)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt14                (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt14), Float_Enc_IEEE754Single))
// Verhalten bei Wertüberschreitung
#define ParamFCB_CHMonitoringMax                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMax)) & FCB_CHMonitoringMaxMask) >> FCB_CHMonitoringMaxShift)
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt1)) & FCB_CHMonitoringMaxDpt1Mask))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt5                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt5)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt5001              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt5001)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt6                 ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt6)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt7                 (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt7)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt8                 ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt8)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt9                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt9), Float_Enc_IEEE754Single))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt12                (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt12)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt13                ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt13)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt14                (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt14), Float_Enc_IEEE754Single))
// Sendeverhalten
#define ParamFCB_CHMonitoringOutput                  ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringOutput)) & FCB_CHMonitoringOutputMask) >> FCB_CHMonitoringOutputShift)
// Datentype
#define ParamFCB_CHSelectionValueType                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionValueType)))
// Anzahl und Typ der Auswahlen (mit gemeinsamen Auswahl-Eingang)
#define ParamFCB_CHSelectionType                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionType)))
// Bei Umschaltung
#define ParamFCB_CHSelectionSwitching                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionSwitching)) & FCB_CHSelectionSwitchingMask) >> FCB_CHSelectionSwitchingShift)
// Auswahl Status Objekt
#define ParamFCB_CHSelectionStateOutput              ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionStateOutput)) & FCB_CHSelectionStateOutputMask))
// Blinker EIN Zeitbasis
#define ParamFCB_CHBlinkerOnDelayBase                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayBase)) & FCB_CHBlinkerOnDelayBaseMask) >> FCB_CHBlinkerOnDelayBaseShift)
// Blinker EIN Zeit
#define ParamFCB_CHBlinkerOnDelayTime                (knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayTime)) & FCB_CHBlinkerOnDelayTimeMask)
// Blinker EIN Zeit (in Millisekunden)
#define ParamFCB_CHBlinkerOnDelayTimeMS              (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayTime))))
// Blinker AUS Zeitbasis
#define ParamFCB_CHBlinkerOffDelayBase               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayBase)) & FCB_CHBlinkerOffDelayBaseMask) >> FCB_CHBlinkerOffDelayBaseShift)
// Blinker AUS Zeit
#define ParamFCB_CHBlinkerOffDelayTime               (knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayTime)) & FCB_CHBlinkerOffDelayTimeMask)
// Blinker AUS Zeit (in Millisekunden)
#define ParamFCB_CHBlinkerOffDelayTimeMS             (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayTime))))
// Start
#define ParamFCB_CHBlinkerStart                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStart)) & FCB_CHBlinkerStartMask) >> FCB_CHBlinkerStartShift)
// Ende
#define ParamFCB_CHBlinkerStop                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStop)) & FCB_CHBlinkerStopMask)
// AUS Telegram am 'Start' Eingang
#define ParamFCB_CHBlinkerBreak                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerBreak)) & FCB_CHBlinkerBreakMask) >> FCB_CHBlinkerBreakShift)
// AUS Telegram am 'Start' Eingang
#define ParamFCB_CHBlinkerBreakWithoutBreak          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerBreakWithoutBreak)) & FCB_CHBlinkerBreakWithoutBreakMask) >> FCB_CHBlinkerBreakWithoutBreakShift)
// Ausgang
#define ParamFCB_CHBlinkerOutputDpt                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOutputDpt)))
// Wert für EIN
#define ParamFCB_CHBlinkerOnPercentage               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOnPercentage)))
// Wert für AUS
#define ParamFCB_CHBlinkerOffPercentage              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOffPercentage)))
// Anzahl der Blinkvorgänge
#define ParamFCB_CHBlinkerCount                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerCount)))
// Objekt zum Starten mit Anzahl
#define ParamFCB_CHBlinkerStartAnzahl                ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStartAnzahl)) & FCB_CHBlinkerStartAnzahlMask))
// Format
#define ParamFCB_CHFormatString                      (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatString)))
#define ParamFCB_CHFormatStringStr                   (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatString), FCB_CHFormatStringLength))
// Textbaustein Aus
#define ParamFCB_CHFormatOff                         (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatOff)))
#define ParamFCB_CHFormatOffStr                      (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatOff), FCB_CHFormatOffLength))
// Textbaustein Ein
#define ParamFCB_CHFormatOn                          (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatOn)))
#define ParamFCB_CHFormatOnStr                       (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatOn), FCB_CHFormatOnLength))
// Tausendertrennzeichen
#define ParamFCB_CHFormatThousand                    (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatThousand)))
#define ParamFCB_CHFormatThousandStr                 (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatThousand), FCB_CHFormatThousandLength))
// Datentype
#define ParamFCB_CHFormatIn1                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn1)))
// Runden
#define ParamFCB_CHFormatRoundFloat1                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat1)) & FCB_CHFormatRoundFloat1Mask) >> FCB_CHFormatRoundFloat1Shift)
// Runden
#define ParamFCB_CHFormatRound1                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound1)) & FCB_CHFormatRound1Mask) >> FCB_CHFormatRound1Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_1)) & FCB_CHFCBFormatRound5_1Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces1              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces1)) & FCB_CHFormatDecimalPlaces1Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant1                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant1)) & FCB_CHFormatSignificant1Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma1             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma1)) & FCB_CHFormatFillupPrecomma1Mask) >> FCB_CHFormatFillupPrecomma1Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode1                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode1)) & FCB_CHFormatFillupMode1Mask) >> FCB_CHFormatFillupMode1Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma1           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma1)) & FCB_CHFormatFillupAfterComma1Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType1               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType1)) & FCB_CHFCBFormatRoundType1Mask) >> FCB_CHFCBFormatRoundType1Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength1               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength1)) & FCB_CHFormatFillupLength1Mask)
// Anzeige als
#define ParamFCB_CHFormatBit1                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit1)))
// Datentype
#define ParamFCB_CHFormatIn2                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn2)))
// Runden
#define ParamFCB_CHFormatRoundFloat2                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat2)) & FCB_CHFormatRoundFloat2Mask) >> FCB_CHFormatRoundFloat2Shift)
// Runden
#define ParamFCB_CHFormatRound2                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound2)) & FCB_CHFormatRound2Mask) >> FCB_CHFormatRound2Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_2                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_2)) & FCB_CHFCBFormatRound5_2Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces2              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces2)) & FCB_CHFormatDecimalPlaces2Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant2                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant2)) & FCB_CHFormatSignificant2Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma2             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma2)) & FCB_CHFormatFillupPrecomma2Mask) >> FCB_CHFormatFillupPrecomma2Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode2                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode2)) & FCB_CHFormatFillupMode2Mask) >> FCB_CHFormatFillupMode2Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma2           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma2)) & FCB_CHFormatFillupAfterComma2Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType2               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType2)) & FCB_CHFCBFormatRoundType2Mask) >> FCB_CHFCBFormatRoundType2Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength2               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength2)) & FCB_CHFormatFillupLength2Mask)
// Anzeige als
#define ParamFCB_CHFormatBit2                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit2)))
// Datentype
#define ParamFCB_CHFormatIn3                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn3)))
// Runden
#define ParamFCB_CHFormatRoundFloat3                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat3)) & FCB_CHFormatRoundFloat3Mask) >> FCB_CHFormatRoundFloat3Shift)
// Runden
#define ParamFCB_CHFormatRound3                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound3)) & FCB_CHFormatRound3Mask) >> FCB_CHFormatRound3Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_3                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_3)) & FCB_CHFCBFormatRound5_3Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces3              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces3)) & FCB_CHFormatDecimalPlaces3Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant3                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant3)) & FCB_CHFormatSignificant3Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma3             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma3)) & FCB_CHFormatFillupPrecomma3Mask) >> FCB_CHFormatFillupPrecomma3Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode3                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode3)) & FCB_CHFormatFillupMode3Mask) >> FCB_CHFormatFillupMode3Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma3           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma3)) & FCB_CHFormatFillupAfterComma3Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType3               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType3)) & FCB_CHFCBFormatRoundType3Mask) >> FCB_CHFCBFormatRoundType3Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength3               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength3)) & FCB_CHFormatFillupLength3Mask)
// Anzeige als
#define ParamFCB_CHFormatBit3                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit3)))
// Datentype
#define ParamFCB_CHFormatIn4                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn4)))
// Runden
#define ParamFCB_CHFormatRoundFloat4                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat4)) & FCB_CHFormatRoundFloat4Mask) >> FCB_CHFormatRoundFloat4Shift)
// Runden
#define ParamFCB_CHFormatRound4                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound4)) & FCB_CHFormatRound4Mask) >> FCB_CHFormatRound4Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_4                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_4)) & FCB_CHFCBFormatRound5_4Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces4              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces4)) & FCB_CHFormatDecimalPlaces4Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant4                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant4)) & FCB_CHFormatSignificant4Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma4             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma4)) & FCB_CHFormatFillupPrecomma4Mask) >> FCB_CHFormatFillupPrecomma4Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode4                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode4)) & FCB_CHFormatFillupMode4Mask) >> FCB_CHFormatFillupMode4Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma4           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma4)) & FCB_CHFormatFillupAfterComma4Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType4               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType4)) & FCB_CHFCBFormatRoundType4Mask) >> FCB_CHFCBFormatRoundType4Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength4               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength4)) & FCB_CHFormatFillupLength4Mask)
// Anzeige als
#define ParamFCB_CHFormatBit4                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit4)))

// deprecated
#define FCB_KoOffset 250

// Communication objects per channel (multiple occurrence)
#define FCB_KoBlockOffset 250
#define FCB_KoBlockSize 10

#define FCB_KoCalcNumber(index) (index + FCB_KoBlockOffset + _channelIndex * FCB_KoBlockSize)
#define FCB_KoCalcIndex(number) ((number >= FCB_KoCalcNumber(0) && number < FCB_KoCalcNumber(FCB_KoBlockSize)) ? (number - FCB_KoBlockOffset) % FCB_KoBlockSize : -1)
#define FCB_KoCalcChannel(number) ((number >= FCB_KoBlockOffset && number < FCB_KoBlockOffset + FCB_ChannelCount * FCB_KoBlockSize) ? (number - FCB_KoBlockOffset) / FCB_KoBlockSize : -1)

#define FCB_KoCHKO0 0
#define FCB_KoCHKO1 1
#define FCB_KoCHKO2 2
#define FCB_KoCHKO3 3
#define FCB_KoCHKO4 4
#define FCB_KoCHKO5 5
#define FCB_KoCHKO6 6
#define FCB_KoCHKO7 7
#define FCB_KoCHKO8 8
#define FCB_KoCHKO9 9

// 
#define KoFCB_CHKO0                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO0)))
// 
#define KoFCB_CHKO1                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO1)))
// 
#define KoFCB_CHKO2                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO2)))
// 
#define KoFCB_CHKO3                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO3)))
// 
#define KoFCB_CHKO4                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO4)))
// 
#define KoFCB_CHKO5                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO5)))
// 
#define KoFCB_CHKO6                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO6)))
// 
#define KoFCB_CHKO7                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO7)))
// 
#define KoFCB_CHKO8                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO8)))
// 
#define KoFCB_CHKO9                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO9)))

#define LMG_LMGHCLEnable                        8929      // 8 Bits, Bit 7-0
#define LMG_LMGHCLMasterCount                   8930      // uint8_t
#define LMG_LMGHCLUpdateInterval                8931      // uint16_t
#define LMG_LMGHCLFadeDuration                  8933      // uint8_t
#define LMG_LMGHCLLockFallback                  9939      // 8 Bits, Bit 7-0
#define LMG_LMGHCLFallbackPolicy                9936      // 8 Bits, Bit 7-0
#define LMG_LMGHCLFallbackDurationSec           9937      // uint16_t
#define LMG_LMGHCLFallbackReleaseTime           9941      // 16 Bits, Bit 15-0
#define LMG_LMGSummerActiveInit                 9943      // 8 Bits, Bit 7-0
#define LMG_LMGSummerActiveSavePower            9944      // 1 Bit, Bit 7
#define     LMG_LMGSummerActiveSavePowerMask 0x80
#define     LMG_LMGSummerActiveSavePowerShift 7

// Lichtmanager
#define ParamLMG_LMGHCLEnable                        (knx.paramByte(LMG_LMGHCLEnable))
// Verfügbare Kanäle
#define ParamLMG_LMGHCLMasterCount                   (knx.paramByte(LMG_LMGHCLMasterCount))
// Aktualisierungsintervall (legacy)
#define ParamLMG_LMGHCLUpdateInterval                (knx.paramWord(LMG_LMGHCLUpdateInterval))
// Überblendzeit (legacy)
#define ParamLMG_LMGHCLFadeDuration                  (knx.paramByte(LMG_LMGHCLFadeDuration))
// Rückfallzeit nach Sperre
#define ParamLMG_LMGHCLLockFallback                  (knx.paramByte(LMG_LMGHCLLockFallback))
// Rückfallstrategie nach Sperre
#define ParamLMG_LMGHCLFallbackPolicy                (knx.paramByte(LMG_LMGHCLFallbackPolicy))
// Freie Rückfalldauer
#define ParamLMG_LMGHCLFallbackDurationSec           (knx.paramWord(LMG_LMGHCLFallbackDurationSec))
// Rückfall-Uhrzeit
#define ParamLMG_LMGHCLFallbackReleaseTime           (knx.paramWord(LMG_LMGHCLFallbackReleaseTime))
// Falls Vorbelegung aus dem Speicher nicht möglich oder nicht gewünscht, dann vorbelegen mit
#define ParamLMG_LMGSummerActiveInit                 (knx.paramByte(LMG_LMGSummerActiveInit))
// Saisonalen Zustand speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLMG_LMGSummerActiveSavePower            ((bool)(knx.paramByte(LMG_LMGSummerActiveSavePower) & LMG_LMGSummerActiveSavePowerMask))

#define LMG_KoLMGHCLLock 1462
#define LMG_KoLMGHCLLockStatus 1463
#define LMG_KoLMGHCLReleaseTrigger 1464

// LM: Sperre (global)
#define KoLMG_LMGHCLLock                          (knx.getGroupObject(LMG_KoLMGHCLLock))
// LM: Status Sperre (global)
#define KoLMG_LMGHCLLockStatus                    (knx.getGroupObject(LMG_KoLMGHCLLockStatus))
// LM: Entsperr-Trigger (global)
#define KoLMG_LMGHCLReleaseTrigger                (knx.getGroupObject(LMG_KoLMGHCLReleaseTrigger))

#define LMG_ChannelCount 16

// Parameter per channel
#define LMG_ParamBlockOffset 9945
#define LMG_ParamBlockSize 879
#define LMG_ParamCalcIndex(index) (index + LMG_ParamBlockOffset + _channelIndex * LMG_ParamBlockSize)

#define LMG_CHAdaptiveActiveMode                170      // 8 Bits, Bit 7-0
#define LMG_CHAdaptiveCeilToHCL                 171      // 8 Bits, Bit 7-0
#define LMG_CHAdaptiveDayNightPolarity          172      // 8 Bits, Bit 7-0
#define LMG_CHAdaptiveDeadband                  173      // uint16_t
#define LMG_CHAdaptiveEndTime                   175      // 16 Bits, Bit 15-0
#define LMG_CHAdaptiveKp                        180      // 8 Bits, Bit 7-0
#define LMG_CHAdaptiveMaxLux                    181      // uint16_t
#define LMG_CHAdaptiveMinBrightness             183      // uint8_t
#define LMG_CHAdaptiveMinChange                 184      // uint8_t
#define LMG_CHAdaptiveMode                      169      // 8 Bits, Bit 7-0
#define LMG_CHAdaptiveSensorTimeout             185      // uint8_t
#define LMG_CHAdaptiveStartTime                 186      // 16 Bits, Bit 15-0
#define LMG_CHAdaptiveStrength                  191      // uint8_t
#define LMG_CHDSTOffsetDays                     51      // int8_t
#define LMG_CHFallbackDurationSec               194      // uint16_t
#define LMG_CHFallbackPolicy                    193      // 8 Bits, Bit 7-0
#define LMG_CHFallbackReleaseTime               196      // 16 Bits, Bit 15-0
#define LMG_CHLockFallback                      192      // 8 Bits, Bit 7-0
#define LMG_CHName                               0      // char*, 25 Byte
#define     LMG_CHNameLength 25
#define LMG_CHIntegrationMode                   201      // 2 Bits, Bit 7-6
#define     LMG_CHIntegrationModeMask 0xC0
#define     LMG_CHIntegrationModeShift 6
#define LMG_CHBusStatusEnable                   201      // 1 Bit, Bit 5
#define     LMG_CHBusStatusEnableMask 0x20
#define     LMG_CHBusStatusEnableShift 5
#define LMG_CHStatusKoOutput                    201      // 3 Bits, Bit 4-2
#define     LMG_CHStatusKoOutputMask 0x1C
#define     LMG_CHStatusKoOutputShift 2
#define LMG_CHUpdateInterval                    198      // uint16_t
#define LMG_CHFadeDuration                      200      // uint8_t
#define LMG_CHSunrise                           31      // 16 Bits, Bit 15-0
#define LMG_CHSunset                            38      // 16 Bits, Bit 15-0
#define LMG_CHHclAxes                           205      // 8 Bits, Bit 7-0
#define LMG_CHHclTimeWindow                     206      // 8 Bits, Bit 7-0
#define LMG_CHPreviewEnable                     207      // 1 Bit, Bit 7
#define     LMG_CHPreviewEnableMask 0x80
#define     LMG_CHPreviewEnableShift 7
#define LMG_CHLookAheadMinutes                  208      // uint8_t
#define LMG_CHProgressEnable                    209      // 1 Bit, Bit 7
#define     LMG_CHProgressEnableMask 0x80
#define     LMG_CHProgressEnableShift 7
#define LMG_CHProfileCount                      210      // 8 Bits, Bit 7-0
#define LMG_CHUseLock                           211      // 8 Bits, Bit 7-0
#define LMG_CHUseAdaptive                       212      // 1 Bit, Bit 7
#define     LMG_CHUseAdaptiveMask 0x80
#define     LMG_CHUseAdaptiveShift 7
#define LMG_CHDayNightSource                    213      // 8 Bits, Bit 7-0
#define LMG_CHExtColorTempSource                214      // 8 Bits, Bit 7-0
#define LMG_CHExtColorTempDpt                   215      // 8 Bits, Bit 7-0
#define LMG_CHExtBrightnessSource               216      // 8 Bits, Bit 7-0
#define LMG_CHExtBrightnessDpt                  217      // 8 Bits, Bit 7-0
#define LMG_CHExtFallbackTimeoutSec             218      // uint16_t
#define LMG_CHExtKelvinMin                      220      // uint16_t
#define LMG_CHExtKelvinMax                      222      // uint16_t
#define LMG_CHExtLuxMax                         224      // uint16_t
#define LMG_CHSeasonSource                      226      // 8 Bits, Bit 7-0
#define LMG_CHSeasonOffsetDays                  227      // int8_t
#define LMG_CHSummerStartMonth                  229      // 8 Bits, Bit 7-0
#define LMG_CHSummerStartDay                    228      // uint8_t
#define LMG_CHSummerEndMonth                    231      // 8 Bits, Bit 7-0
#define LMG_CHSummerEndDay                      230      // uint8_t
#define LMG_CHUseDayNightSlew                   232      // 1 Bit, Bit 7
#define     LMG_CHUseDayNightSlewMask 0x80
#define     LMG_CHUseDayNightSlewShift 7
#define LMG_CHSlewRateDay                       233      // uint16_t
#define LMG_CHSlewRateNight                     235      // uint16_t
#define LMG_CHAstroSource                       237      // 8 Bits, Bit 7-0
#define LMG_CHSlewRateBrightness                238      // uint8_t
#define LMG_CHP1_Active                         239      // 1 Bit, Bit 7
#define     LMG_CHP1_ActiveMask 0x80
#define     LMG_CHP1_ActiveShift 7
#define LMG_CHP1_Name                           240      // char*, 16 Byte
#define     LMG_CHP1_NameLength 16
#define LMG_CHP1_DayMo                          256      // 1 Bit, Bit 7
#define     LMG_CHP1_DayMoMask 0x80
#define     LMG_CHP1_DayMoShift 7
#define LMG_CHP1_DayDi                          256      // 1 Bit, Bit 6
#define     LMG_CHP1_DayDiMask 0x40
#define     LMG_CHP1_DayDiShift 6
#define LMG_CHP1_DayMi                          256      // 1 Bit, Bit 5
#define     LMG_CHP1_DayMiMask 0x20
#define     LMG_CHP1_DayMiShift 5
#define LMG_CHP1_DayDo                          256      // 1 Bit, Bit 4
#define     LMG_CHP1_DayDoMask 0x10
#define     LMG_CHP1_DayDoShift 4
#define LMG_CHP1_DayFr                          256      // 1 Bit, Bit 3
#define     LMG_CHP1_DayFrMask 0x08
#define     LMG_CHP1_DayFrShift 3
#define LMG_CHP1_DaySa                          256      // 1 Bit, Bit 2
#define     LMG_CHP1_DaySaMask 0x04
#define     LMG_CHP1_DaySaShift 2
#define LMG_CHP1_DaySo                          256      // 1 Bit, Bit 1
#define     LMG_CHP1_DaySoMask 0x02
#define     LMG_CHP1_DaySoShift 1
#define LMG_CHP1_DayUrl                         256      // 1 Bit, Bit 0
#define     LMG_CHP1_DayUrlMask 0x01
#define     LMG_CHP1_DayUrlShift 0
#define LMG_CHP1_DayFei                         257      // 1 Bit, Bit 7
#define     LMG_CHP1_DayFeiMask 0x80
#define     LMG_CHP1_DayFeiShift 7
#define LMG_CHP1_SPCount                        258      // 8 Bits, Bit 7-0
#define LMG_CHP2_Active                         259      // 1 Bit, Bit 7
#define     LMG_CHP2_ActiveMask 0x80
#define     LMG_CHP2_ActiveShift 7
#define LMG_CHP2_Name                           260      // char*, 16 Byte
#define     LMG_CHP2_NameLength 16
#define LMG_CHP2_DayMo                          276      // 1 Bit, Bit 7
#define     LMG_CHP2_DayMoMask 0x80
#define     LMG_CHP2_DayMoShift 7
#define LMG_CHP2_DayDi                          276      // 1 Bit, Bit 6
#define     LMG_CHP2_DayDiMask 0x40
#define     LMG_CHP2_DayDiShift 6
#define LMG_CHP2_DayMi                          276      // 1 Bit, Bit 5
#define     LMG_CHP2_DayMiMask 0x20
#define     LMG_CHP2_DayMiShift 5
#define LMG_CHP2_DayDo                          276      // 1 Bit, Bit 4
#define     LMG_CHP2_DayDoMask 0x10
#define     LMG_CHP2_DayDoShift 4
#define LMG_CHP2_DayFr                          276      // 1 Bit, Bit 3
#define     LMG_CHP2_DayFrMask 0x08
#define     LMG_CHP2_DayFrShift 3
#define LMG_CHP2_DaySa                          276      // 1 Bit, Bit 2
#define     LMG_CHP2_DaySaMask 0x04
#define     LMG_CHP2_DaySaShift 2
#define LMG_CHP2_DaySo                          276      // 1 Bit, Bit 1
#define     LMG_CHP2_DaySoMask 0x02
#define     LMG_CHP2_DaySoShift 1
#define LMG_CHP2_DayUrl                         276      // 1 Bit, Bit 0
#define     LMG_CHP2_DayUrlMask 0x01
#define     LMG_CHP2_DayUrlShift 0
#define LMG_CHP2_DayFei                         277      // 1 Bit, Bit 7
#define     LMG_CHP2_DayFeiMask 0x80
#define     LMG_CHP2_DayFeiShift 7
#define LMG_CHP2_SPCount                        278      // 8 Bits, Bit 7-0
#define LMG_CHP3_Active                         279      // 1 Bit, Bit 7
#define     LMG_CHP3_ActiveMask 0x80
#define     LMG_CHP3_ActiveShift 7
#define LMG_CHP3_Name                           280      // char*, 16 Byte
#define     LMG_CHP3_NameLength 16
#define LMG_CHP3_DayMo                          296      // 1 Bit, Bit 7
#define     LMG_CHP3_DayMoMask 0x80
#define     LMG_CHP3_DayMoShift 7
#define LMG_CHP3_DayDi                          296      // 1 Bit, Bit 6
#define     LMG_CHP3_DayDiMask 0x40
#define     LMG_CHP3_DayDiShift 6
#define LMG_CHP3_DayMi                          296      // 1 Bit, Bit 5
#define     LMG_CHP3_DayMiMask 0x20
#define     LMG_CHP3_DayMiShift 5
#define LMG_CHP3_DayDo                          296      // 1 Bit, Bit 4
#define     LMG_CHP3_DayDoMask 0x10
#define     LMG_CHP3_DayDoShift 4
#define LMG_CHP3_DayFr                          296      // 1 Bit, Bit 3
#define     LMG_CHP3_DayFrMask 0x08
#define     LMG_CHP3_DayFrShift 3
#define LMG_CHP3_DaySa                          296      // 1 Bit, Bit 2
#define     LMG_CHP3_DaySaMask 0x04
#define     LMG_CHP3_DaySaShift 2
#define LMG_CHP3_DaySo                          296      // 1 Bit, Bit 1
#define     LMG_CHP3_DaySoMask 0x02
#define     LMG_CHP3_DaySoShift 1
#define LMG_CHP3_DayUrl                         296      // 1 Bit, Bit 0
#define     LMG_CHP3_DayUrlMask 0x01
#define     LMG_CHP3_DayUrlShift 0
#define LMG_CHP3_DayFei                         297      // 1 Bit, Bit 7
#define     LMG_CHP3_DayFeiMask 0x80
#define     LMG_CHP3_DayFeiShift 7
#define LMG_CHP3_SPCount                        298      // 8 Bits, Bit 7-0
#define LMG_CHP4_Active                         299      // 1 Bit, Bit 7
#define     LMG_CHP4_ActiveMask 0x80
#define     LMG_CHP4_ActiveShift 7
#define LMG_CHP4_Name                           300      // char*, 16 Byte
#define     LMG_CHP4_NameLength 16
#define LMG_CHP4_DayMo                          316      // 1 Bit, Bit 7
#define     LMG_CHP4_DayMoMask 0x80
#define     LMG_CHP4_DayMoShift 7
#define LMG_CHP4_DayDi                          316      // 1 Bit, Bit 6
#define     LMG_CHP4_DayDiMask 0x40
#define     LMG_CHP4_DayDiShift 6
#define LMG_CHP4_DayMi                          316      // 1 Bit, Bit 5
#define     LMG_CHP4_DayMiMask 0x20
#define     LMG_CHP4_DayMiShift 5
#define LMG_CHP4_DayDo                          316      // 1 Bit, Bit 4
#define     LMG_CHP4_DayDoMask 0x10
#define     LMG_CHP4_DayDoShift 4
#define LMG_CHP4_DayFr                          316      // 1 Bit, Bit 3
#define     LMG_CHP4_DayFrMask 0x08
#define     LMG_CHP4_DayFrShift 3
#define LMG_CHP4_DaySa                          316      // 1 Bit, Bit 2
#define     LMG_CHP4_DaySaMask 0x04
#define     LMG_CHP4_DaySaShift 2
#define LMG_CHP4_DaySo                          316      // 1 Bit, Bit 1
#define     LMG_CHP4_DaySoMask 0x02
#define     LMG_CHP4_DaySoShift 1
#define LMG_CHP4_DayUrl                         316      // 1 Bit, Bit 0
#define     LMG_CHP4_DayUrlMask 0x01
#define     LMG_CHP4_DayUrlShift 0
#define LMG_CHP4_DayFei                         317      // 1 Bit, Bit 7
#define     LMG_CHP4_DayFeiMask 0x80
#define     LMG_CHP4_DayFeiShift 7
#define LMG_CHP1_SeasonFilter                   257      // 2 Bits, Bit 6-5
#define     LMG_CHP1_SeasonFilterMask 0x60
#define     LMG_CHP1_SeasonFilterShift 5
#define LMG_CHP2_SeasonFilter                   277      // 2 Bits, Bit 6-5
#define     LMG_CHP2_SeasonFilterMask 0x60
#define     LMG_CHP2_SeasonFilterShift 5
#define LMG_CHP3_SeasonFilter                   297      // 2 Bits, Bit 6-5
#define     LMG_CHP3_SeasonFilterMask 0x60
#define     LMG_CHP3_SeasonFilterShift 5
#define LMG_CHP4_SeasonFilter                   317      // 2 Bits, Bit 6-5
#define     LMG_CHP4_SeasonFilterMask 0x60
#define     LMG_CHP4_SeasonFilterShift 5
#define LMG_CHP4_SPCount                        318      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_Active                     319      // 1 Bit, Bit 7
#define     LMG_CHP1_SP1_ActiveMask 0x80
#define     LMG_CHP1_SP1_ActiveShift 7
#define LMG_CHP1_SP1_AnchorType                 320      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_Hour                       321      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_Minute                     322      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_OffsetMinutes              323      // int16_t
#define LMG_CHP1_SP1_ClampMode                  325      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_ClampHour                  326      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_ClampMinute                327      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_Kelvin                     328      // uint16_t
#define LMG_CHP1_SP1_Brightness                 330      // uint8_t
#define LMG_CHP1_SP1_ExtColorTempMode           331      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP1_ExtMixPercent              332      // uint8_t
#define LMG_CHP1_SP2_Active                     333      // 1 Bit, Bit 7
#define     LMG_CHP1_SP2_ActiveMask 0x80
#define     LMG_CHP1_SP2_ActiveShift 7
#define LMG_CHP1_SP2_AnchorType                 334      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_Hour                       335      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_Minute                     336      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_OffsetMinutes              337      // int16_t
#define LMG_CHP1_SP2_ClampMode                  339      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_ClampHour                  340      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_ClampMinute                341      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_Kelvin                     342      // uint16_t
#define LMG_CHP1_SP2_Brightness                 344      // uint8_t
#define LMG_CHP1_SP2_ExtColorTempMode           345      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP2_ExtMixPercent              346      // uint8_t
#define LMG_CHP1_SP3_Active                     347      // 1 Bit, Bit 7
#define     LMG_CHP1_SP3_ActiveMask 0x80
#define     LMG_CHP1_SP3_ActiveShift 7
#define LMG_CHP1_SP3_AnchorType                 348      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_Hour                       349      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_Minute                     350      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_OffsetMinutes              351      // int16_t
#define LMG_CHP1_SP3_ClampMode                  353      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_ClampHour                  354      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_ClampMinute                355      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_Kelvin                     356      // uint16_t
#define LMG_CHP1_SP3_Brightness                 358      // uint8_t
#define LMG_CHP1_SP3_ExtColorTempMode           359      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP3_ExtMixPercent              360      // uint8_t
#define LMG_CHP1_SP4_Active                     361      // 1 Bit, Bit 7
#define     LMG_CHP1_SP4_ActiveMask 0x80
#define     LMG_CHP1_SP4_ActiveShift 7
#define LMG_CHP1_SP4_AnchorType                 362      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_Hour                       363      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_Minute                     364      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_OffsetMinutes              365      // int16_t
#define LMG_CHP1_SP4_ClampMode                  367      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_ClampHour                  368      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_ClampMinute                369      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_Kelvin                     370      // uint16_t
#define LMG_CHP1_SP4_Brightness                 372      // uint8_t
#define LMG_CHP1_SP4_ExtColorTempMode           373      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP4_ExtMixPercent              374      // uint8_t
#define LMG_CHP1_SP5_Active                     375      // 1 Bit, Bit 7
#define     LMG_CHP1_SP5_ActiveMask 0x80
#define     LMG_CHP1_SP5_ActiveShift 7
#define LMG_CHP1_SP5_AnchorType                 376      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_Hour                       377      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_Minute                     378      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_OffsetMinutes              379      // int16_t
#define LMG_CHP1_SP5_ClampMode                  381      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_ClampHour                  382      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_ClampMinute                383      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_Kelvin                     384      // uint16_t
#define LMG_CHP1_SP5_Brightness                 386      // uint8_t
#define LMG_CHP1_SP5_ExtColorTempMode           387      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP5_ExtMixPercent              388      // uint8_t
#define LMG_CHP1_SP6_Active                     389      // 1 Bit, Bit 7
#define     LMG_CHP1_SP6_ActiveMask 0x80
#define     LMG_CHP1_SP6_ActiveShift 7
#define LMG_CHP1_SP6_AnchorType                 390      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_Hour                       391      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_Minute                     392      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_OffsetMinutes              393      // int16_t
#define LMG_CHP1_SP6_ClampMode                  395      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_ClampHour                  396      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_ClampMinute                397      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_Kelvin                     398      // uint16_t
#define LMG_CHP1_SP6_Brightness                 400      // uint8_t
#define LMG_CHP1_SP6_ExtColorTempMode           401      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP6_ExtMixPercent              402      // uint8_t
#define LMG_CHP1_SP7_Active                     403      // 1 Bit, Bit 7
#define     LMG_CHP1_SP7_ActiveMask 0x80
#define     LMG_CHP1_SP7_ActiveShift 7
#define LMG_CHP1_SP7_AnchorType                 404      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_Hour                       405      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_Minute                     406      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_OffsetMinutes              407      // int16_t
#define LMG_CHP1_SP7_ClampMode                  409      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_ClampHour                  410      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_ClampMinute                411      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_Kelvin                     412      // uint16_t
#define LMG_CHP1_SP7_Brightness                 414      // uint8_t
#define LMG_CHP1_SP7_ExtColorTempMode           415      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP7_ExtMixPercent              416      // uint8_t
#define LMG_CHP1_SP8_Active                     417      // 1 Bit, Bit 7
#define     LMG_CHP1_SP8_ActiveMask 0x80
#define     LMG_CHP1_SP8_ActiveShift 7
#define LMG_CHP1_SP8_AnchorType                 418      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_Hour                       419      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_Minute                     420      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_OffsetMinutes              421      // int16_t
#define LMG_CHP1_SP8_ClampMode                  423      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_ClampHour                  424      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_ClampMinute                425      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_Kelvin                     426      // uint16_t
#define LMG_CHP1_SP8_Brightness                 428      // uint8_t
#define LMG_CHP1_SP8_ExtColorTempMode           429      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP8_ExtMixPercent              430      // uint8_t
#define LMG_CHP1_SP9_Active                     431      // 1 Bit, Bit 7
#define     LMG_CHP1_SP9_ActiveMask 0x80
#define     LMG_CHP1_SP9_ActiveShift 7
#define LMG_CHP1_SP9_AnchorType                 432      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_Hour                       433      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_Minute                     434      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_OffsetMinutes              435      // int16_t
#define LMG_CHP1_SP9_ClampMode                  437      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_ClampHour                  438      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_ClampMinute                439      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_Kelvin                     440      // uint16_t
#define LMG_CHP1_SP9_Brightness                 442      // uint8_t
#define LMG_CHP1_SP9_ExtColorTempMode           443      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP9_ExtMixPercent              444      // uint8_t
#define LMG_CHP1_SP10_Active                    445      // 1 Bit, Bit 7
#define     LMG_CHP1_SP10_ActiveMask 0x80
#define     LMG_CHP1_SP10_ActiveShift 7
#define LMG_CHP1_SP10_AnchorType                446      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_Hour                      447      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_Minute                    448      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_OffsetMinutes             449      // int16_t
#define LMG_CHP1_SP10_ClampMode                 451      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_ClampHour                 452      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_ClampMinute               453      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_Kelvin                    454      // uint16_t
#define LMG_CHP1_SP10_Brightness                456      // uint8_t
#define LMG_CHP1_SP10_ExtColorTempMode          457      // 8 Bits, Bit 7-0
#define LMG_CHP1_SP10_ExtMixPercent             458      // uint8_t
#define LMG_CHP2_SP1_Active                     459      // 1 Bit, Bit 7
#define     LMG_CHP2_SP1_ActiveMask 0x80
#define     LMG_CHP2_SP1_ActiveShift 7
#define LMG_CHP2_SP1_AnchorType                 460      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_Hour                       461      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_Minute                     462      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_OffsetMinutes              463      // int16_t
#define LMG_CHP2_SP1_ClampMode                  465      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_ClampHour                  466      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_ClampMinute                467      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_Kelvin                     468      // uint16_t
#define LMG_CHP2_SP1_Brightness                 470      // uint8_t
#define LMG_CHP2_SP1_ExtColorTempMode           471      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP1_ExtMixPercent              472      // uint8_t
#define LMG_CHP2_SP2_Active                     473      // 1 Bit, Bit 7
#define     LMG_CHP2_SP2_ActiveMask 0x80
#define     LMG_CHP2_SP2_ActiveShift 7
#define LMG_CHP2_SP2_AnchorType                 474      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_Hour                       475      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_Minute                     476      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_OffsetMinutes              477      // int16_t
#define LMG_CHP2_SP2_ClampMode                  479      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_ClampHour                  480      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_ClampMinute                481      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_Kelvin                     482      // uint16_t
#define LMG_CHP2_SP2_Brightness                 484      // uint8_t
#define LMG_CHP2_SP2_ExtColorTempMode           485      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP2_ExtMixPercent              486      // uint8_t
#define LMG_CHP2_SP3_Active                     487      // 1 Bit, Bit 7
#define     LMG_CHP2_SP3_ActiveMask 0x80
#define     LMG_CHP2_SP3_ActiveShift 7
#define LMG_CHP2_SP3_AnchorType                 488      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_Hour                       489      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_Minute                     490      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_OffsetMinutes              491      // int16_t
#define LMG_CHP2_SP3_ClampMode                  493      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_ClampHour                  494      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_ClampMinute                495      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_Kelvin                     496      // uint16_t
#define LMG_CHP2_SP3_Brightness                 498      // uint8_t
#define LMG_CHP2_SP3_ExtColorTempMode           499      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP3_ExtMixPercent              500      // uint8_t
#define LMG_CHP2_SP4_Active                     501      // 1 Bit, Bit 7
#define     LMG_CHP2_SP4_ActiveMask 0x80
#define     LMG_CHP2_SP4_ActiveShift 7
#define LMG_CHP2_SP4_AnchorType                 502      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_Hour                       503      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_Minute                     504      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_OffsetMinutes              505      // int16_t
#define LMG_CHP2_SP4_ClampMode                  507      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_ClampHour                  508      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_ClampMinute                509      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_Kelvin                     510      // uint16_t
#define LMG_CHP2_SP4_Brightness                 512      // uint8_t
#define LMG_CHP2_SP4_ExtColorTempMode           513      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP4_ExtMixPercent              514      // uint8_t
#define LMG_CHP2_SP5_Active                     515      // 1 Bit, Bit 7
#define     LMG_CHP2_SP5_ActiveMask 0x80
#define     LMG_CHP2_SP5_ActiveShift 7
#define LMG_CHP2_SP5_AnchorType                 516      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_Hour                       517      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_Minute                     518      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_OffsetMinutes              519      // int16_t
#define LMG_CHP2_SP5_ClampMode                  521      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_ClampHour                  522      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_ClampMinute                523      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_Kelvin                     524      // uint16_t
#define LMG_CHP2_SP5_Brightness                 526      // uint8_t
#define LMG_CHP2_SP5_ExtColorTempMode           527      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP5_ExtMixPercent              528      // uint8_t
#define LMG_CHP2_SP6_Active                     529      // 1 Bit, Bit 7
#define     LMG_CHP2_SP6_ActiveMask 0x80
#define     LMG_CHP2_SP6_ActiveShift 7
#define LMG_CHP2_SP6_AnchorType                 530      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_Hour                       531      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_Minute                     532      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_OffsetMinutes              533      // int16_t
#define LMG_CHP2_SP6_ClampMode                  535      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_ClampHour                  536      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_ClampMinute                537      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_Kelvin                     538      // uint16_t
#define LMG_CHP2_SP6_Brightness                 540      // uint8_t
#define LMG_CHP2_SP6_ExtColorTempMode           541      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP6_ExtMixPercent              542      // uint8_t
#define LMG_CHP2_SP7_Active                     543      // 1 Bit, Bit 7
#define     LMG_CHP2_SP7_ActiveMask 0x80
#define     LMG_CHP2_SP7_ActiveShift 7
#define LMG_CHP2_SP7_AnchorType                 544      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_Hour                       545      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_Minute                     546      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_OffsetMinutes              547      // int16_t
#define LMG_CHP2_SP7_ClampMode                  549      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_ClampHour                  550      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_ClampMinute                551      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_Kelvin                     552      // uint16_t
#define LMG_CHP2_SP7_Brightness                 554      // uint8_t
#define LMG_CHP2_SP7_ExtColorTempMode           555      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP7_ExtMixPercent              556      // uint8_t
#define LMG_CHP2_SP8_Active                     557      // 1 Bit, Bit 7
#define     LMG_CHP2_SP8_ActiveMask 0x80
#define     LMG_CHP2_SP8_ActiveShift 7
#define LMG_CHP2_SP8_AnchorType                 558      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_Hour                       559      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_Minute                     560      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_OffsetMinutes              561      // int16_t
#define LMG_CHP2_SP8_ClampMode                  563      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_ClampHour                  564      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_ClampMinute                565      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_Kelvin                     566      // uint16_t
#define LMG_CHP2_SP8_Brightness                 568      // uint8_t
#define LMG_CHP2_SP8_ExtColorTempMode           569      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP8_ExtMixPercent              570      // uint8_t
#define LMG_CHP2_SP9_Active                     571      // 1 Bit, Bit 7
#define     LMG_CHP2_SP9_ActiveMask 0x80
#define     LMG_CHP2_SP9_ActiveShift 7
#define LMG_CHP2_SP9_AnchorType                 572      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_Hour                       573      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_Minute                     574      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_OffsetMinutes              575      // int16_t
#define LMG_CHP2_SP9_ClampMode                  577      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_ClampHour                  578      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_ClampMinute                579      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_Kelvin                     580      // uint16_t
#define LMG_CHP2_SP9_Brightness                 582      // uint8_t
#define LMG_CHP2_SP9_ExtColorTempMode           583      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP9_ExtMixPercent              584      // uint8_t
#define LMG_CHP2_SP10_Active                    585      // 1 Bit, Bit 7
#define     LMG_CHP2_SP10_ActiveMask 0x80
#define     LMG_CHP2_SP10_ActiveShift 7
#define LMG_CHP2_SP10_AnchorType                586      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_Hour                      587      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_Minute                    588      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_OffsetMinutes             589      // int16_t
#define LMG_CHP2_SP10_ClampMode                 591      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_ClampHour                 592      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_ClampMinute               593      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_Kelvin                    594      // uint16_t
#define LMG_CHP2_SP10_Brightness                596      // uint8_t
#define LMG_CHP2_SP10_ExtColorTempMode          597      // 8 Bits, Bit 7-0
#define LMG_CHP2_SP10_ExtMixPercent             598      // uint8_t
#define LMG_CHP3_SP1_Active                     599      // 1 Bit, Bit 7
#define     LMG_CHP3_SP1_ActiveMask 0x80
#define     LMG_CHP3_SP1_ActiveShift 7
#define LMG_CHP3_SP1_AnchorType                 600      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_Hour                       601      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_Minute                     602      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_OffsetMinutes              603      // int16_t
#define LMG_CHP3_SP1_ClampMode                  605      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_ClampHour                  606      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_ClampMinute                607      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_Kelvin                     608      // uint16_t
#define LMG_CHP3_SP1_Brightness                 610      // uint8_t
#define LMG_CHP3_SP1_ExtColorTempMode           611      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP1_ExtMixPercent              612      // uint8_t
#define LMG_CHP3_SP2_Active                     613      // 1 Bit, Bit 7
#define     LMG_CHP3_SP2_ActiveMask 0x80
#define     LMG_CHP3_SP2_ActiveShift 7
#define LMG_CHP3_SP2_AnchorType                 614      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_Hour                       615      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_Minute                     616      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_OffsetMinutes              617      // int16_t
#define LMG_CHP3_SP2_ClampMode                  619      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_ClampHour                  620      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_ClampMinute                621      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_Kelvin                     622      // uint16_t
#define LMG_CHP3_SP2_Brightness                 624      // uint8_t
#define LMG_CHP3_SP2_ExtColorTempMode           625      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP2_ExtMixPercent              626      // uint8_t
#define LMG_CHP3_SP3_Active                     627      // 1 Bit, Bit 7
#define     LMG_CHP3_SP3_ActiveMask 0x80
#define     LMG_CHP3_SP3_ActiveShift 7
#define LMG_CHP3_SP3_AnchorType                 628      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_Hour                       629      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_Minute                     630      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_OffsetMinutes              631      // int16_t
#define LMG_CHP3_SP3_ClampMode                  633      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_ClampHour                  634      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_ClampMinute                635      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_Kelvin                     636      // uint16_t
#define LMG_CHP3_SP3_Brightness                 638      // uint8_t
#define LMG_CHP3_SP3_ExtColorTempMode           639      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP3_ExtMixPercent              640      // uint8_t
#define LMG_CHP3_SP4_Active                     641      // 1 Bit, Bit 7
#define     LMG_CHP3_SP4_ActiveMask 0x80
#define     LMG_CHP3_SP4_ActiveShift 7
#define LMG_CHP3_SP4_AnchorType                 642      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_Hour                       643      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_Minute                     644      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_OffsetMinutes              645      // int16_t
#define LMG_CHP3_SP4_ClampMode                  647      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_ClampHour                  648      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_ClampMinute                649      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_Kelvin                     650      // uint16_t
#define LMG_CHP3_SP4_Brightness                 652      // uint8_t
#define LMG_CHP3_SP4_ExtColorTempMode           653      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP4_ExtMixPercent              654      // uint8_t
#define LMG_CHP3_SP5_Active                     655      // 1 Bit, Bit 7
#define     LMG_CHP3_SP5_ActiveMask 0x80
#define     LMG_CHP3_SP5_ActiveShift 7
#define LMG_CHP3_SP5_AnchorType                 656      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_Hour                       657      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_Minute                     658      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_OffsetMinutes              659      // int16_t
#define LMG_CHP3_SP5_ClampMode                  661      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_ClampHour                  662      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_ClampMinute                663      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_Kelvin                     664      // uint16_t
#define LMG_CHP3_SP5_Brightness                 666      // uint8_t
#define LMG_CHP3_SP5_ExtColorTempMode           667      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP5_ExtMixPercent              668      // uint8_t
#define LMG_CHP3_SP6_Active                     669      // 1 Bit, Bit 7
#define     LMG_CHP3_SP6_ActiveMask 0x80
#define     LMG_CHP3_SP6_ActiveShift 7
#define LMG_CHP3_SP6_AnchorType                 670      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_Hour                       671      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_Minute                     672      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_OffsetMinutes              673      // int16_t
#define LMG_CHP3_SP6_ClampMode                  675      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_ClampHour                  676      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_ClampMinute                677      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_Kelvin                     678      // uint16_t
#define LMG_CHP3_SP6_Brightness                 680      // uint8_t
#define LMG_CHP3_SP6_ExtColorTempMode           681      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP6_ExtMixPercent              682      // uint8_t
#define LMG_CHP3_SP7_Active                     683      // 1 Bit, Bit 7
#define     LMG_CHP3_SP7_ActiveMask 0x80
#define     LMG_CHP3_SP7_ActiveShift 7
#define LMG_CHP3_SP7_AnchorType                 684      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_Hour                       685      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_Minute                     686      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_OffsetMinutes              687      // int16_t
#define LMG_CHP3_SP7_ClampMode                  689      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_ClampHour                  690      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_ClampMinute                691      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_Kelvin                     692      // uint16_t
#define LMG_CHP3_SP7_Brightness                 694      // uint8_t
#define LMG_CHP3_SP7_ExtColorTempMode           695      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP7_ExtMixPercent              696      // uint8_t
#define LMG_CHP3_SP8_Active                     697      // 1 Bit, Bit 7
#define     LMG_CHP3_SP8_ActiveMask 0x80
#define     LMG_CHP3_SP8_ActiveShift 7
#define LMG_CHP3_SP8_AnchorType                 698      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_Hour                       699      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_Minute                     700      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_OffsetMinutes              701      // int16_t
#define LMG_CHP3_SP8_ClampMode                  703      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_ClampHour                  704      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_ClampMinute                705      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_Kelvin                     706      // uint16_t
#define LMG_CHP3_SP8_Brightness                 708      // uint8_t
#define LMG_CHP3_SP8_ExtColorTempMode           709      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP8_ExtMixPercent              710      // uint8_t
#define LMG_CHP3_SP9_Active                     711      // 1 Bit, Bit 7
#define     LMG_CHP3_SP9_ActiveMask 0x80
#define     LMG_CHP3_SP9_ActiveShift 7
#define LMG_CHP3_SP9_AnchorType                 712      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_Hour                       713      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_Minute                     714      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_OffsetMinutes              715      // int16_t
#define LMG_CHP3_SP9_ClampMode                  717      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_ClampHour                  718      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_ClampMinute                719      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_Kelvin                     720      // uint16_t
#define LMG_CHP3_SP9_Brightness                 722      // uint8_t
#define LMG_CHP3_SP9_ExtColorTempMode           723      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP9_ExtMixPercent              724      // uint8_t
#define LMG_CHP3_SP10_Active                    725      // 1 Bit, Bit 7
#define     LMG_CHP3_SP10_ActiveMask 0x80
#define     LMG_CHP3_SP10_ActiveShift 7
#define LMG_CHP3_SP10_AnchorType                726      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_Hour                      727      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_Minute                    728      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_OffsetMinutes             729      // int16_t
#define LMG_CHP3_SP10_ClampMode                 731      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_ClampHour                 732      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_ClampMinute               733      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_Kelvin                    734      // uint16_t
#define LMG_CHP3_SP10_Brightness                736      // uint8_t
#define LMG_CHP3_SP10_ExtColorTempMode          737      // 8 Bits, Bit 7-0
#define LMG_CHP3_SP10_ExtMixPercent             738      // uint8_t
#define LMG_CHP4_SP1_Active                     739      // 1 Bit, Bit 7
#define     LMG_CHP4_SP1_ActiveMask 0x80
#define     LMG_CHP4_SP1_ActiveShift 7
#define LMG_CHP4_SP1_AnchorType                 740      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_Hour                       741      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_Minute                     742      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_OffsetMinutes              743      // int16_t
#define LMG_CHP4_SP1_ClampMode                  745      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_ClampHour                  746      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_ClampMinute                747      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_Kelvin                     748      // uint16_t
#define LMG_CHP4_SP1_Brightness                 750      // uint8_t
#define LMG_CHP4_SP1_ExtColorTempMode           751      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP1_ExtMixPercent              752      // uint8_t
#define LMG_CHP4_SP2_Active                     753      // 1 Bit, Bit 7
#define     LMG_CHP4_SP2_ActiveMask 0x80
#define     LMG_CHP4_SP2_ActiveShift 7
#define LMG_CHP4_SP2_AnchorType                 754      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_Hour                       755      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_Minute                     756      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_OffsetMinutes              757      // int16_t
#define LMG_CHP4_SP2_ClampMode                  759      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_ClampHour                  760      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_ClampMinute                761      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_Kelvin                     762      // uint16_t
#define LMG_CHP4_SP2_Brightness                 764      // uint8_t
#define LMG_CHP4_SP2_ExtColorTempMode           765      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP2_ExtMixPercent              766      // uint8_t
#define LMG_CHP4_SP3_Active                     767      // 1 Bit, Bit 7
#define     LMG_CHP4_SP3_ActiveMask 0x80
#define     LMG_CHP4_SP3_ActiveShift 7
#define LMG_CHP4_SP3_AnchorType                 768      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_Hour                       769      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_Minute                     770      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_OffsetMinutes              771      // int16_t
#define LMG_CHP4_SP3_ClampMode                  773      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_ClampHour                  774      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_ClampMinute                775      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_Kelvin                     776      // uint16_t
#define LMG_CHP4_SP3_Brightness                 778      // uint8_t
#define LMG_CHP4_SP3_ExtColorTempMode           779      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP3_ExtMixPercent              780      // uint8_t
#define LMG_CHP4_SP4_Active                     781      // 1 Bit, Bit 7
#define     LMG_CHP4_SP4_ActiveMask 0x80
#define     LMG_CHP4_SP4_ActiveShift 7
#define LMG_CHP4_SP4_AnchorType                 782      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_Hour                       783      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_Minute                     784      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_OffsetMinutes              785      // int16_t
#define LMG_CHP4_SP4_ClampMode                  787      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_ClampHour                  788      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_ClampMinute                789      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_Kelvin                     790      // uint16_t
#define LMG_CHP4_SP4_Brightness                 792      // uint8_t
#define LMG_CHP4_SP4_ExtColorTempMode           793      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP4_ExtMixPercent              794      // uint8_t
#define LMG_CHP4_SP5_Active                     795      // 1 Bit, Bit 7
#define     LMG_CHP4_SP5_ActiveMask 0x80
#define     LMG_CHP4_SP5_ActiveShift 7
#define LMG_CHP4_SP5_AnchorType                 796      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_Hour                       797      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_Minute                     798      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_OffsetMinutes              799      // int16_t
#define LMG_CHP4_SP5_ClampMode                  801      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_ClampHour                  802      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_ClampMinute                803      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_Kelvin                     804      // uint16_t
#define LMG_CHP4_SP5_Brightness                 806      // uint8_t
#define LMG_CHP4_SP5_ExtColorTempMode           807      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP5_ExtMixPercent              808      // uint8_t
#define LMG_CHP4_SP6_Active                     809      // 1 Bit, Bit 7
#define     LMG_CHP4_SP6_ActiveMask 0x80
#define     LMG_CHP4_SP6_ActiveShift 7
#define LMG_CHP4_SP6_AnchorType                 810      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_Hour                       811      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_Minute                     812      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_OffsetMinutes              813      // int16_t
#define LMG_CHP4_SP6_ClampMode                  815      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_ClampHour                  816      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_ClampMinute                817      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_Kelvin                     818      // uint16_t
#define LMG_CHP4_SP6_Brightness                 820      // uint8_t
#define LMG_CHP4_SP6_ExtColorTempMode           821      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP6_ExtMixPercent              822      // uint8_t
#define LMG_CHP4_SP7_Active                     823      // 1 Bit, Bit 7
#define     LMG_CHP4_SP7_ActiveMask 0x80
#define     LMG_CHP4_SP7_ActiveShift 7
#define LMG_CHP4_SP7_AnchorType                 824      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_Hour                       825      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_Minute                     826      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_OffsetMinutes              827      // int16_t
#define LMG_CHP4_SP7_ClampMode                  829      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_ClampHour                  830      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_ClampMinute                831      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_Kelvin                     832      // uint16_t
#define LMG_CHP4_SP7_Brightness                 834      // uint8_t
#define LMG_CHP4_SP7_ExtColorTempMode           835      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP7_ExtMixPercent              836      // uint8_t
#define LMG_CHP4_SP8_Active                     837      // 1 Bit, Bit 7
#define     LMG_CHP4_SP8_ActiveMask 0x80
#define     LMG_CHP4_SP8_ActiveShift 7
#define LMG_CHP4_SP8_AnchorType                 838      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_Hour                       839      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_Minute                     840      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_OffsetMinutes              841      // int16_t
#define LMG_CHP4_SP8_ClampMode                  843      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_ClampHour                  844      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_ClampMinute                845      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_Kelvin                     846      // uint16_t
#define LMG_CHP4_SP8_Brightness                 848      // uint8_t
#define LMG_CHP4_SP8_ExtColorTempMode           849      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP8_ExtMixPercent              850      // uint8_t
#define LMG_CHP4_SP9_Active                     851      // 1 Bit, Bit 7
#define     LMG_CHP4_SP9_ActiveMask 0x80
#define     LMG_CHP4_SP9_ActiveShift 7
#define LMG_CHP4_SP9_AnchorType                 852      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_Hour                       853      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_Minute                     854      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_OffsetMinutes              855      // int16_t
#define LMG_CHP4_SP9_ClampMode                  857      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_ClampHour                  858      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_ClampMinute                859      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_Kelvin                     860      // uint16_t
#define LMG_CHP4_SP9_Brightness                 862      // uint8_t
#define LMG_CHP4_SP9_ExtColorTempMode           863      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP9_ExtMixPercent              864      // uint8_t
#define LMG_CHP4_SP10_Active                    865      // 1 Bit, Bit 7
#define     LMG_CHP4_SP10_ActiveMask 0x80
#define     LMG_CHP4_SP10_ActiveShift 7
#define LMG_CHP4_SP10_AnchorType                866      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_Hour                      867      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_Minute                    868      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_OffsetMinutes             869      // int16_t
#define LMG_CHP4_SP10_ClampMode                 871      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_ClampHour                 872      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_ClampMinute               873      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_Kelvin                    874      // uint16_t
#define LMG_CHP4_SP10_Brightness                876      // uint8_t
#define LMG_CHP4_SP10_ExtColorTempMode          877      // 8 Bits, Bit 7-0
#define LMG_CHP4_SP10_ExtMixPercent             878      // uint8_t

// Aktivierung
#define ParamLMG_CHAdaptiveActiveMode                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveActiveMode)))
// Auf HCL-Wert begrenzen
#define ParamLMG_CHAdaptiveCeilToHCL                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveCeilToHCL)))
// Tag/Nacht-Polarität
#define ParamLMG_CHAdaptiveDayNightPolarity          (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveDayNightPolarity)))
// Totband
#define ParamLMG_CHAdaptiveDeadband                  (knx.paramWord(LMG_ParamCalcIndex(LMG_CHAdaptiveDeadband)))
// Endzeit
#define ParamLMG_CHAdaptiveEndTime                   (knx.paramWord(LMG_ParamCalcIndex(LMG_CHAdaptiveEndTime)))
// P-Faktor
#define ParamLMG_CHAdaptiveKp                        (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveKp)))
// Skalierungsmaximum
#define ParamLMG_CHAdaptiveMaxLux                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHAdaptiveMaxLux)))
// Mindesthelligkeit
#define ParamLMG_CHAdaptiveMinBrightness             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveMinBrightness)))
// Mindestschrittgröße
#define ParamLMG_CHAdaptiveMinChange                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveMinChange)))
// Adaptive Helligkeit (Modus)
#define ParamLMG_CHAdaptiveMode                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveMode)))
// Sensor-Timeout (0=aus)
#define ParamLMG_CHAdaptiveSensorTimeout             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveSensorTimeout)))
// Startzeit
#define ParamLMG_CHAdaptiveStartTime                 (knx.paramWord(LMG_ParamCalcIndex(LMG_CHAdaptiveStartTime)))
// Kompensationsstärke
#define ParamLMG_CHAdaptiveStrength                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAdaptiveStrength)))
// DST-Offset (Tage)
#define ParamLMG_CHDSTOffsetDays                     ((int8_t)knx.paramByte(LMG_ParamCalcIndex(LMG_CHDSTOffsetDays)))
// Freie Rückfalldauer
#define ParamLMG_CHFallbackDurationSec               (knx.paramWord(LMG_ParamCalcIndex(LMG_CHFallbackDurationSec)))
// Rückfallstrategie nach Sperre
#define ParamLMG_CHFallbackPolicy                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHFallbackPolicy)))
// Rückfall-Uhrzeit
#define ParamLMG_CHFallbackReleaseTime               (knx.paramWord(LMG_ParamCalcIndex(LMG_CHFallbackReleaseTime)))
// Rückfallzeit nach Sperre
#define ParamLMG_CHLockFallback                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHLockFallback)))
// Name Lichtmanager %C%
#define ParamLMG_CHName                              (knx.paramData(LMG_ParamCalcIndex(LMG_CHName)))
#define ParamLMG_CHNameStr                           (knx.paramString(LMG_ParamCalcIndex(LMG_CHName), LMG_CHNameLength))
// Bereitstellung der Lichtmanager-Sollwerte
#define ParamLMG_CHIntegrationMode                   ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHIntegrationMode)) & LMG_CHIntegrationModeMask) >> LMG_CHIntegrationModeShift)
// Status auf KNX-Bus senden
#define ParamLMG_CHBusStatusEnable                   ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHBusStatusEnable)) & LMG_CHBusStatusEnableMask))
// Status-KO-Ausgabe
#define ParamLMG_CHStatusKoOutput                    ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHStatusKoOutput)) & LMG_CHStatusKoOutputMask) >> LMG_CHStatusKoOutputShift)
// Aktualisierungsintervall
#define ParamLMG_CHUpdateInterval                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHUpdateInterval)))
// Überblendzeit
#define ParamLMG_CHFadeDuration                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHFadeDuration)))
// Sonnenaufgang
#define ParamLMG_CHSunrise                           (knx.paramWord(LMG_ParamCalcIndex(LMG_CHSunrise)))
// Sonnenuntergang
#define ParamLMG_CHSunset                            (knx.paramWord(LMG_ParamCalcIndex(LMG_CHSunset)))
// HCL-Achsen
#define ParamLMG_CHHclAxes                           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHHclAxes)))
// HCL-Zeitfenster
#define ParamLMG_CHHclTimeWindow                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHHclTimeWindow)))
// Vorausschau-KOs senden
#define ParamLMG_CHPreviewEnable                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHPreviewEnable)) & LMG_CHPreviewEnableMask))
// Vorausschau-Horizont
#define ParamLMG_CHLookAheadMinutes                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHLookAheadMinutes)))
// Tages-Fortschritt senden
#define ParamLMG_CHProgressEnable                    ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHProgressEnable)) & LMG_CHProgressEnableMask))
// Anzahl Profile
#define ParamLMG_CHProfileCount                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHProfileCount)))
// Sperr-Verhalten
#define ParamLMG_CHUseLock                           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHUseLock)))
// Adaptive Helligkeit verwenden
#define ParamLMG_CHUseAdaptive                       ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHUseAdaptive)) & LMG_CHUseAdaptiveMask))
// Tag/Nacht-Quelle
#define ParamLMG_CHDayNightSource                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHDayNightSource)))
// Externe Farbtemperatur-Quelle
#define ParamLMG_CHExtColorTempSource                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHExtColorTempSource)))
// Externe Farbtemperatur-DPT
#define ParamLMG_CHExtColorTempDpt                   (knx.paramByte(LMG_ParamCalcIndex(LMG_CHExtColorTempDpt)))
// Externe Helligkeits-Quelle
#define ParamLMG_CHExtBrightnessSource               (knx.paramByte(LMG_ParamCalcIndex(LMG_CHExtBrightnessSource)))
// Externe Helligkeits-DPT
#define ParamLMG_CHExtBrightnessDpt                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHExtBrightnessDpt)))
// Externe Quellen Fallback-Timeout
#define ParamLMG_CHExtFallbackTimeoutSec             (knx.paramWord(LMG_ParamCalcIndex(LMG_CHExtFallbackTimeoutSec)))
// Skalar 0 = Kelvin (warm)
#define ParamLMG_CHExtKelvinMin                      (knx.paramWord(LMG_ParamCalcIndex(LMG_CHExtKelvinMin)))
// Skalar 255 = Kelvin (kalt)
#define ParamLMG_CHExtKelvinMax                      (knx.paramWord(LMG_ParamCalcIndex(LMG_CHExtKelvinMax)))
// Lux bei 100 % Helligkeit
#define ParamLMG_CHExtLuxMax                         (knx.paramWord(LMG_ParamCalcIndex(LMG_CHExtLuxMax)))
// Saisonale Umschaltung
#define ParamLMG_CHSeasonSource                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSeasonSource)))
// Saison-Offset
#define ParamLMG_CHSeasonOffsetDays                  ((int8_t)knx.paramByte(LMG_ParamCalcIndex(LMG_CHSeasonOffsetDays)))
// Sommer-Beginn Monat
#define ParamLMG_CHSummerStartMonth                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSummerStartMonth)))
// Sommer-Beginn Tag
#define ParamLMG_CHSummerStartDay                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSummerStartDay)))
// Sommer-Ende Monat
#define ParamLMG_CHSummerEndMonth                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSummerEndMonth)))
// Sommer-Ende Tag
#define ParamLMG_CHSummerEndDay                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSummerEndDay)))
// Tag/Nacht-Slew verwenden
#define ParamLMG_CHUseDayNightSlew                   ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHUseDayNightSlew)) & LMG_CHUseDayNightSlewMask))
// Slew Rate Tag
#define ParamLMG_CHSlewRateDay                       (knx.paramWord(LMG_ParamCalcIndex(LMG_CHSlewRateDay)))
// Slew Rate Nacht
#define ParamLMG_CHSlewRateNight                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHSlewRateNight)))
// Astro-Quelle
#define ParamLMG_CHAstroSource                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHAstroSource)))
// Slew-Rate Helligkeit
#define ParamLMG_CHSlewRateBrightness                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHSlewRateBrightness)))
// Profil 1 aktiv
#define ParamLMG_CHP1_Active                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_Active)) & LMG_CHP1_ActiveMask))
// Profil 1 Name
#define ParamLMG_CHP1_Name                           (knx.paramData(LMG_ParamCalcIndex(LMG_CHP1_Name)))
#define ParamLMG_CHP1_NameStr                        (knx.paramString(LMG_ParamCalcIndex(LMG_CHP1_Name), LMG_CHP1_NameLength))
// Mo
#define ParamLMG_CHP1_DayMo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayMo)) & LMG_CHP1_DayMoMask))
// Di
#define ParamLMG_CHP1_DayDi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayDi)) & LMG_CHP1_DayDiMask))
// Mi
#define ParamLMG_CHP1_DayMi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayMi)) & LMG_CHP1_DayMiMask))
// Do
#define ParamLMG_CHP1_DayDo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayDo)) & LMG_CHP1_DayDoMask))
// Fr
#define ParamLMG_CHP1_DayFr                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayFr)) & LMG_CHP1_DayFrMask))
// Sa
#define ParamLMG_CHP1_DaySa                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DaySa)) & LMG_CHP1_DaySaMask))
// So
#define ParamLMG_CHP1_DaySo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DaySo)) & LMG_CHP1_DaySoMask))
// Urlaub
#define ParamLMG_CHP1_DayUrl                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayUrl)) & LMG_CHP1_DayUrlMask))
// Feiertag
#define ParamLMG_CHP1_DayFei                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_DayFei)) & LMG_CHP1_DayFeiMask))
// Profil 1 Stützpunkte
#define ParamLMG_CHP1_SPCount                        (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SPCount)))
// Profil 2 aktiv
#define ParamLMG_CHP2_Active                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_Active)) & LMG_CHP2_ActiveMask))
// Profil 2 Name
#define ParamLMG_CHP2_Name                           (knx.paramData(LMG_ParamCalcIndex(LMG_CHP2_Name)))
#define ParamLMG_CHP2_NameStr                        (knx.paramString(LMG_ParamCalcIndex(LMG_CHP2_Name), LMG_CHP2_NameLength))
// Mo
#define ParamLMG_CHP2_DayMo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayMo)) & LMG_CHP2_DayMoMask))
// Di
#define ParamLMG_CHP2_DayDi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayDi)) & LMG_CHP2_DayDiMask))
// Mi
#define ParamLMG_CHP2_DayMi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayMi)) & LMG_CHP2_DayMiMask))
// Do
#define ParamLMG_CHP2_DayDo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayDo)) & LMG_CHP2_DayDoMask))
// Fr
#define ParamLMG_CHP2_DayFr                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayFr)) & LMG_CHP2_DayFrMask))
// Sa
#define ParamLMG_CHP2_DaySa                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DaySa)) & LMG_CHP2_DaySaMask))
// So
#define ParamLMG_CHP2_DaySo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DaySo)) & LMG_CHP2_DaySoMask))
// Urlaub
#define ParamLMG_CHP2_DayUrl                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayUrl)) & LMG_CHP2_DayUrlMask))
// Feiertag
#define ParamLMG_CHP2_DayFei                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_DayFei)) & LMG_CHP2_DayFeiMask))
// Profil 2 Stützpunkte
#define ParamLMG_CHP2_SPCount                        (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SPCount)))
// Profil 3 aktiv
#define ParamLMG_CHP3_Active                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_Active)) & LMG_CHP3_ActiveMask))
// Profil 3 Name
#define ParamLMG_CHP3_Name                           (knx.paramData(LMG_ParamCalcIndex(LMG_CHP3_Name)))
#define ParamLMG_CHP3_NameStr                        (knx.paramString(LMG_ParamCalcIndex(LMG_CHP3_Name), LMG_CHP3_NameLength))
// Mo
#define ParamLMG_CHP3_DayMo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayMo)) & LMG_CHP3_DayMoMask))
// Di
#define ParamLMG_CHP3_DayDi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayDi)) & LMG_CHP3_DayDiMask))
// Mi
#define ParamLMG_CHP3_DayMi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayMi)) & LMG_CHP3_DayMiMask))
// Do
#define ParamLMG_CHP3_DayDo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayDo)) & LMG_CHP3_DayDoMask))
// Fr
#define ParamLMG_CHP3_DayFr                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayFr)) & LMG_CHP3_DayFrMask))
// Sa
#define ParamLMG_CHP3_DaySa                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DaySa)) & LMG_CHP3_DaySaMask))
// So
#define ParamLMG_CHP3_DaySo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DaySo)) & LMG_CHP3_DaySoMask))
// Urlaub
#define ParamLMG_CHP3_DayUrl                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayUrl)) & LMG_CHP3_DayUrlMask))
// Feiertag
#define ParamLMG_CHP3_DayFei                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_DayFei)) & LMG_CHP3_DayFeiMask))
// Profil 3 Stützpunkte
#define ParamLMG_CHP3_SPCount                        (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SPCount)))
// Profil 4 aktiv
#define ParamLMG_CHP4_Active                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_Active)) & LMG_CHP4_ActiveMask))
// Profil 4 Name
#define ParamLMG_CHP4_Name                           (knx.paramData(LMG_ParamCalcIndex(LMG_CHP4_Name)))
#define ParamLMG_CHP4_NameStr                        (knx.paramString(LMG_ParamCalcIndex(LMG_CHP4_Name), LMG_CHP4_NameLength))
// Mo
#define ParamLMG_CHP4_DayMo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayMo)) & LMG_CHP4_DayMoMask))
// Di
#define ParamLMG_CHP4_DayDi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayDi)) & LMG_CHP4_DayDiMask))
// Mi
#define ParamLMG_CHP4_DayMi                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayMi)) & LMG_CHP4_DayMiMask))
// Do
#define ParamLMG_CHP4_DayDo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayDo)) & LMG_CHP4_DayDoMask))
// Fr
#define ParamLMG_CHP4_DayFr                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayFr)) & LMG_CHP4_DayFrMask))
// Sa
#define ParamLMG_CHP4_DaySa                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DaySa)) & LMG_CHP4_DaySaMask))
// So
#define ParamLMG_CHP4_DaySo                          ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DaySo)) & LMG_CHP4_DaySoMask))
// Urlaub
#define ParamLMG_CHP4_DayUrl                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayUrl)) & LMG_CHP4_DayUrlMask))
// Feiertag
#define ParamLMG_CHP4_DayFei                         ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_DayFei)) & LMG_CHP4_DayFeiMask))
// Saison
#define ParamLMG_CHP1_SeasonFilter                   ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SeasonFilter)) & LMG_CHP1_SeasonFilterMask) >> LMG_CHP1_SeasonFilterShift)
// Saison
#define ParamLMG_CHP2_SeasonFilter                   ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SeasonFilter)) & LMG_CHP2_SeasonFilterMask) >> LMG_CHP2_SeasonFilterShift)
// Saison
#define ParamLMG_CHP3_SeasonFilter                   ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SeasonFilter)) & LMG_CHP3_SeasonFilterMask) >> LMG_CHP3_SeasonFilterShift)
// Saison
#define ParamLMG_CHP4_SeasonFilter                   ((knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SeasonFilter)) & LMG_CHP4_SeasonFilterMask) >> LMG_CHP4_SeasonFilterShift)
// Profil 4 Stützpunkte
#define ParamLMG_CHP4_SPCount                        (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SPCount)))
// Profil 1 SP1 Active
#define ParamLMG_CHP1_SP1_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_Active)) & LMG_CHP1_SP1_ActiveMask))
// Profil 1 SP1 AnchorType
#define ParamLMG_CHP1_SP1_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_AnchorType)))
// Profil 1 SP1 Hour
#define ParamLMG_CHP1_SP1_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_Hour)))
// Profil 1 SP1 Minute
#define ParamLMG_CHP1_SP1_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_Minute)))
// Profil 1 SP1 OffsetMinutes
#define ParamLMG_CHP1_SP1_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP1_OffsetMinutes)))
// Profil 1 SP1 ClampMode
#define ParamLMG_CHP1_SP1_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_ClampMode)))
// Profil 1 SP1 ClampHour
#define ParamLMG_CHP1_SP1_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_ClampHour)))
// Profil 1 SP1 ClampMinute
#define ParamLMG_CHP1_SP1_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_ClampMinute)))
// Profil 1 SP1 Kelvin
#define ParamLMG_CHP1_SP1_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP1_Kelvin)))
// Profil 1 SP1 Brightness
#define ParamLMG_CHP1_SP1_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_Brightness)))
// Profil 1 SP1 ExtColorTempMode
#define ParamLMG_CHP1_SP1_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_ExtColorTempMode)))
// Profil 1 SP1 ExtMixPercent
#define ParamLMG_CHP1_SP1_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP1_ExtMixPercent)))
// Profil 1 SP2 Active
#define ParamLMG_CHP1_SP2_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_Active)) & LMG_CHP1_SP2_ActiveMask))
// Profil 1 SP2 AnchorType
#define ParamLMG_CHP1_SP2_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_AnchorType)))
// Profil 1 SP2 Hour
#define ParamLMG_CHP1_SP2_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_Hour)))
// Profil 1 SP2 Minute
#define ParamLMG_CHP1_SP2_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_Minute)))
// Profil 1 SP2 OffsetMinutes
#define ParamLMG_CHP1_SP2_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP2_OffsetMinutes)))
// Profil 1 SP2 ClampMode
#define ParamLMG_CHP1_SP2_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_ClampMode)))
// Profil 1 SP2 ClampHour
#define ParamLMG_CHP1_SP2_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_ClampHour)))
// Profil 1 SP2 ClampMinute
#define ParamLMG_CHP1_SP2_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_ClampMinute)))
// Profil 1 SP2 Kelvin
#define ParamLMG_CHP1_SP2_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP2_Kelvin)))
// Profil 1 SP2 Brightness
#define ParamLMG_CHP1_SP2_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_Brightness)))
// Profil 1 SP2 ExtColorTempMode
#define ParamLMG_CHP1_SP2_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_ExtColorTempMode)))
// Profil 1 SP2 ExtMixPercent
#define ParamLMG_CHP1_SP2_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP2_ExtMixPercent)))
// Profil 1 SP3 Active
#define ParamLMG_CHP1_SP3_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_Active)) & LMG_CHP1_SP3_ActiveMask))
// Profil 1 SP3 AnchorType
#define ParamLMG_CHP1_SP3_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_AnchorType)))
// Profil 1 SP3 Hour
#define ParamLMG_CHP1_SP3_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_Hour)))
// Profil 1 SP3 Minute
#define ParamLMG_CHP1_SP3_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_Minute)))
// Profil 1 SP3 OffsetMinutes
#define ParamLMG_CHP1_SP3_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP3_OffsetMinutes)))
// Profil 1 SP3 ClampMode
#define ParamLMG_CHP1_SP3_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_ClampMode)))
// Profil 1 SP3 ClampHour
#define ParamLMG_CHP1_SP3_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_ClampHour)))
// Profil 1 SP3 ClampMinute
#define ParamLMG_CHP1_SP3_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_ClampMinute)))
// Profil 1 SP3 Kelvin
#define ParamLMG_CHP1_SP3_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP3_Kelvin)))
// Profil 1 SP3 Brightness
#define ParamLMG_CHP1_SP3_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_Brightness)))
// Profil 1 SP3 ExtColorTempMode
#define ParamLMG_CHP1_SP3_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_ExtColorTempMode)))
// Profil 1 SP3 ExtMixPercent
#define ParamLMG_CHP1_SP3_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP3_ExtMixPercent)))
// Profil 1 SP4 Active
#define ParamLMG_CHP1_SP4_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_Active)) & LMG_CHP1_SP4_ActiveMask))
// Profil 1 SP4 AnchorType
#define ParamLMG_CHP1_SP4_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_AnchorType)))
// Profil 1 SP4 Hour
#define ParamLMG_CHP1_SP4_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_Hour)))
// Profil 1 SP4 Minute
#define ParamLMG_CHP1_SP4_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_Minute)))
// Profil 1 SP4 OffsetMinutes
#define ParamLMG_CHP1_SP4_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP4_OffsetMinutes)))
// Profil 1 SP4 ClampMode
#define ParamLMG_CHP1_SP4_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_ClampMode)))
// Profil 1 SP4 ClampHour
#define ParamLMG_CHP1_SP4_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_ClampHour)))
// Profil 1 SP4 ClampMinute
#define ParamLMG_CHP1_SP4_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_ClampMinute)))
// Profil 1 SP4 Kelvin
#define ParamLMG_CHP1_SP4_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP4_Kelvin)))
// Profil 1 SP4 Brightness
#define ParamLMG_CHP1_SP4_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_Brightness)))
// Profil 1 SP4 ExtColorTempMode
#define ParamLMG_CHP1_SP4_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_ExtColorTempMode)))
// Profil 1 SP4 ExtMixPercent
#define ParamLMG_CHP1_SP4_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP4_ExtMixPercent)))
// Profil 1 SP5 Active
#define ParamLMG_CHP1_SP5_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_Active)) & LMG_CHP1_SP5_ActiveMask))
// Profil 1 SP5 AnchorType
#define ParamLMG_CHP1_SP5_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_AnchorType)))
// Profil 1 SP5 Hour
#define ParamLMG_CHP1_SP5_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_Hour)))
// Profil 1 SP5 Minute
#define ParamLMG_CHP1_SP5_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_Minute)))
// Profil 1 SP5 OffsetMinutes
#define ParamLMG_CHP1_SP5_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP5_OffsetMinutes)))
// Profil 1 SP5 ClampMode
#define ParamLMG_CHP1_SP5_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_ClampMode)))
// Profil 1 SP5 ClampHour
#define ParamLMG_CHP1_SP5_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_ClampHour)))
// Profil 1 SP5 ClampMinute
#define ParamLMG_CHP1_SP5_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_ClampMinute)))
// Profil 1 SP5 Kelvin
#define ParamLMG_CHP1_SP5_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP5_Kelvin)))
// Profil 1 SP5 Brightness
#define ParamLMG_CHP1_SP5_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_Brightness)))
// Profil 1 SP5 ExtColorTempMode
#define ParamLMG_CHP1_SP5_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_ExtColorTempMode)))
// Profil 1 SP5 ExtMixPercent
#define ParamLMG_CHP1_SP5_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP5_ExtMixPercent)))
// Profil 1 SP6 Active
#define ParamLMG_CHP1_SP6_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_Active)) & LMG_CHP1_SP6_ActiveMask))
// Profil 1 SP6 AnchorType
#define ParamLMG_CHP1_SP6_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_AnchorType)))
// Profil 1 SP6 Hour
#define ParamLMG_CHP1_SP6_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_Hour)))
// Profil 1 SP6 Minute
#define ParamLMG_CHP1_SP6_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_Minute)))
// Profil 1 SP6 OffsetMinutes
#define ParamLMG_CHP1_SP6_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP6_OffsetMinutes)))
// Profil 1 SP6 ClampMode
#define ParamLMG_CHP1_SP6_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_ClampMode)))
// Profil 1 SP6 ClampHour
#define ParamLMG_CHP1_SP6_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_ClampHour)))
// Profil 1 SP6 ClampMinute
#define ParamLMG_CHP1_SP6_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_ClampMinute)))
// Profil 1 SP6 Kelvin
#define ParamLMG_CHP1_SP6_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP6_Kelvin)))
// Profil 1 SP6 Brightness
#define ParamLMG_CHP1_SP6_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_Brightness)))
// Profil 1 SP6 ExtColorTempMode
#define ParamLMG_CHP1_SP6_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_ExtColorTempMode)))
// Profil 1 SP6 ExtMixPercent
#define ParamLMG_CHP1_SP6_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP6_ExtMixPercent)))
// Profil 1 SP7 Active
#define ParamLMG_CHP1_SP7_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_Active)) & LMG_CHP1_SP7_ActiveMask))
// Profil 1 SP7 AnchorType
#define ParamLMG_CHP1_SP7_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_AnchorType)))
// Profil 1 SP7 Hour
#define ParamLMG_CHP1_SP7_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_Hour)))
// Profil 1 SP7 Minute
#define ParamLMG_CHP1_SP7_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_Minute)))
// Profil 1 SP7 OffsetMinutes
#define ParamLMG_CHP1_SP7_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP7_OffsetMinutes)))
// Profil 1 SP7 ClampMode
#define ParamLMG_CHP1_SP7_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_ClampMode)))
// Profil 1 SP7 ClampHour
#define ParamLMG_CHP1_SP7_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_ClampHour)))
// Profil 1 SP7 ClampMinute
#define ParamLMG_CHP1_SP7_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_ClampMinute)))
// Profil 1 SP7 Kelvin
#define ParamLMG_CHP1_SP7_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP7_Kelvin)))
// Profil 1 SP7 Brightness
#define ParamLMG_CHP1_SP7_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_Brightness)))
// Profil 1 SP7 ExtColorTempMode
#define ParamLMG_CHP1_SP7_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_ExtColorTempMode)))
// Profil 1 SP7 ExtMixPercent
#define ParamLMG_CHP1_SP7_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP7_ExtMixPercent)))
// Profil 1 SP8 Active
#define ParamLMG_CHP1_SP8_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_Active)) & LMG_CHP1_SP8_ActiveMask))
// Profil 1 SP8 AnchorType
#define ParamLMG_CHP1_SP8_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_AnchorType)))
// Profil 1 SP8 Hour
#define ParamLMG_CHP1_SP8_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_Hour)))
// Profil 1 SP8 Minute
#define ParamLMG_CHP1_SP8_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_Minute)))
// Profil 1 SP8 OffsetMinutes
#define ParamLMG_CHP1_SP8_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP8_OffsetMinutes)))
// Profil 1 SP8 ClampMode
#define ParamLMG_CHP1_SP8_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_ClampMode)))
// Profil 1 SP8 ClampHour
#define ParamLMG_CHP1_SP8_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_ClampHour)))
// Profil 1 SP8 ClampMinute
#define ParamLMG_CHP1_SP8_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_ClampMinute)))
// Profil 1 SP8 Kelvin
#define ParamLMG_CHP1_SP8_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP8_Kelvin)))
// Profil 1 SP8 Brightness
#define ParamLMG_CHP1_SP8_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_Brightness)))
// Profil 1 SP8 ExtColorTempMode
#define ParamLMG_CHP1_SP8_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_ExtColorTempMode)))
// Profil 1 SP8 ExtMixPercent
#define ParamLMG_CHP1_SP8_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP8_ExtMixPercent)))
// Profil 1 SP9 Active
#define ParamLMG_CHP1_SP9_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_Active)) & LMG_CHP1_SP9_ActiveMask))
// Profil 1 SP9 AnchorType
#define ParamLMG_CHP1_SP9_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_AnchorType)))
// Profil 1 SP9 Hour
#define ParamLMG_CHP1_SP9_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_Hour)))
// Profil 1 SP9 Minute
#define ParamLMG_CHP1_SP9_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_Minute)))
// Profil 1 SP9 OffsetMinutes
#define ParamLMG_CHP1_SP9_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP9_OffsetMinutes)))
// Profil 1 SP9 ClampMode
#define ParamLMG_CHP1_SP9_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_ClampMode)))
// Profil 1 SP9 ClampHour
#define ParamLMG_CHP1_SP9_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_ClampHour)))
// Profil 1 SP9 ClampMinute
#define ParamLMG_CHP1_SP9_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_ClampMinute)))
// Profil 1 SP9 Kelvin
#define ParamLMG_CHP1_SP9_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP9_Kelvin)))
// Profil 1 SP9 Brightness
#define ParamLMG_CHP1_SP9_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_Brightness)))
// Profil 1 SP9 ExtColorTempMode
#define ParamLMG_CHP1_SP9_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_ExtColorTempMode)))
// Profil 1 SP9 ExtMixPercent
#define ParamLMG_CHP1_SP9_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP9_ExtMixPercent)))
// Profil 1 SP10 Active
#define ParamLMG_CHP1_SP10_Active                    ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_Active)) & LMG_CHP1_SP10_ActiveMask))
// Profil 1 SP10 AnchorType
#define ParamLMG_CHP1_SP10_AnchorType                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_AnchorType)))
// Profil 1 SP10 Hour
#define ParamLMG_CHP1_SP10_Hour                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_Hour)))
// Profil 1 SP10 Minute
#define ParamLMG_CHP1_SP10_Minute                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_Minute)))
// Profil 1 SP10 OffsetMinutes
#define ParamLMG_CHP1_SP10_OffsetMinutes             ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP10_OffsetMinutes)))
// Profil 1 SP10 ClampMode
#define ParamLMG_CHP1_SP10_ClampMode                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_ClampMode)))
// Profil 1 SP10 ClampHour
#define ParamLMG_CHP1_SP10_ClampHour                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_ClampHour)))
// Profil 1 SP10 ClampMinute
#define ParamLMG_CHP1_SP10_ClampMinute               (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_ClampMinute)))
// Profil 1 SP10 Kelvin
#define ParamLMG_CHP1_SP10_Kelvin                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP1_SP10_Kelvin)))
// Profil 1 SP10 Brightness
#define ParamLMG_CHP1_SP10_Brightness                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_Brightness)))
// Profil 1 SP10 ExtColorTempMode
#define ParamLMG_CHP1_SP10_ExtColorTempMode          (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_ExtColorTempMode)))
// Profil 1 SP10 ExtMixPercent
#define ParamLMG_CHP1_SP10_ExtMixPercent             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP1_SP10_ExtMixPercent)))
// Profil 2 SP1 Active
#define ParamLMG_CHP2_SP1_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_Active)) & LMG_CHP2_SP1_ActiveMask))
// Profil 2 SP1 AnchorType
#define ParamLMG_CHP2_SP1_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_AnchorType)))
// Profil 2 SP1 Hour
#define ParamLMG_CHP2_SP1_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_Hour)))
// Profil 2 SP1 Minute
#define ParamLMG_CHP2_SP1_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_Minute)))
// Profil 2 SP1 OffsetMinutes
#define ParamLMG_CHP2_SP1_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP1_OffsetMinutes)))
// Profil 2 SP1 ClampMode
#define ParamLMG_CHP2_SP1_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_ClampMode)))
// Profil 2 SP1 ClampHour
#define ParamLMG_CHP2_SP1_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_ClampHour)))
// Profil 2 SP1 ClampMinute
#define ParamLMG_CHP2_SP1_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_ClampMinute)))
// Profil 2 SP1 Kelvin
#define ParamLMG_CHP2_SP1_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP1_Kelvin)))
// Profil 2 SP1 Brightness
#define ParamLMG_CHP2_SP1_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_Brightness)))
// Profil 2 SP1 ExtColorTempMode
#define ParamLMG_CHP2_SP1_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_ExtColorTempMode)))
// Profil 2 SP1 ExtMixPercent
#define ParamLMG_CHP2_SP1_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP1_ExtMixPercent)))
// Profil 2 SP2 Active
#define ParamLMG_CHP2_SP2_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_Active)) & LMG_CHP2_SP2_ActiveMask))
// Profil 2 SP2 AnchorType
#define ParamLMG_CHP2_SP2_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_AnchorType)))
// Profil 2 SP2 Hour
#define ParamLMG_CHP2_SP2_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_Hour)))
// Profil 2 SP2 Minute
#define ParamLMG_CHP2_SP2_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_Minute)))
// Profil 2 SP2 OffsetMinutes
#define ParamLMG_CHP2_SP2_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP2_OffsetMinutes)))
// Profil 2 SP2 ClampMode
#define ParamLMG_CHP2_SP2_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_ClampMode)))
// Profil 2 SP2 ClampHour
#define ParamLMG_CHP2_SP2_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_ClampHour)))
// Profil 2 SP2 ClampMinute
#define ParamLMG_CHP2_SP2_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_ClampMinute)))
// Profil 2 SP2 Kelvin
#define ParamLMG_CHP2_SP2_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP2_Kelvin)))
// Profil 2 SP2 Brightness
#define ParamLMG_CHP2_SP2_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_Brightness)))
// Profil 2 SP2 ExtColorTempMode
#define ParamLMG_CHP2_SP2_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_ExtColorTempMode)))
// Profil 2 SP2 ExtMixPercent
#define ParamLMG_CHP2_SP2_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP2_ExtMixPercent)))
// Profil 2 SP3 Active
#define ParamLMG_CHP2_SP3_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_Active)) & LMG_CHP2_SP3_ActiveMask))
// Profil 2 SP3 AnchorType
#define ParamLMG_CHP2_SP3_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_AnchorType)))
// Profil 2 SP3 Hour
#define ParamLMG_CHP2_SP3_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_Hour)))
// Profil 2 SP3 Minute
#define ParamLMG_CHP2_SP3_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_Minute)))
// Profil 2 SP3 OffsetMinutes
#define ParamLMG_CHP2_SP3_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP3_OffsetMinutes)))
// Profil 2 SP3 ClampMode
#define ParamLMG_CHP2_SP3_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_ClampMode)))
// Profil 2 SP3 ClampHour
#define ParamLMG_CHP2_SP3_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_ClampHour)))
// Profil 2 SP3 ClampMinute
#define ParamLMG_CHP2_SP3_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_ClampMinute)))
// Profil 2 SP3 Kelvin
#define ParamLMG_CHP2_SP3_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP3_Kelvin)))
// Profil 2 SP3 Brightness
#define ParamLMG_CHP2_SP3_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_Brightness)))
// Profil 2 SP3 ExtColorTempMode
#define ParamLMG_CHP2_SP3_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_ExtColorTempMode)))
// Profil 2 SP3 ExtMixPercent
#define ParamLMG_CHP2_SP3_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP3_ExtMixPercent)))
// Profil 2 SP4 Active
#define ParamLMG_CHP2_SP4_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_Active)) & LMG_CHP2_SP4_ActiveMask))
// Profil 2 SP4 AnchorType
#define ParamLMG_CHP2_SP4_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_AnchorType)))
// Profil 2 SP4 Hour
#define ParamLMG_CHP2_SP4_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_Hour)))
// Profil 2 SP4 Minute
#define ParamLMG_CHP2_SP4_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_Minute)))
// Profil 2 SP4 OffsetMinutes
#define ParamLMG_CHP2_SP4_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP4_OffsetMinutes)))
// Profil 2 SP4 ClampMode
#define ParamLMG_CHP2_SP4_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_ClampMode)))
// Profil 2 SP4 ClampHour
#define ParamLMG_CHP2_SP4_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_ClampHour)))
// Profil 2 SP4 ClampMinute
#define ParamLMG_CHP2_SP4_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_ClampMinute)))
// Profil 2 SP4 Kelvin
#define ParamLMG_CHP2_SP4_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP4_Kelvin)))
// Profil 2 SP4 Brightness
#define ParamLMG_CHP2_SP4_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_Brightness)))
// Profil 2 SP4 ExtColorTempMode
#define ParamLMG_CHP2_SP4_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_ExtColorTempMode)))
// Profil 2 SP4 ExtMixPercent
#define ParamLMG_CHP2_SP4_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP4_ExtMixPercent)))
// Profil 2 SP5 Active
#define ParamLMG_CHP2_SP5_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_Active)) & LMG_CHP2_SP5_ActiveMask))
// Profil 2 SP5 AnchorType
#define ParamLMG_CHP2_SP5_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_AnchorType)))
// Profil 2 SP5 Hour
#define ParamLMG_CHP2_SP5_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_Hour)))
// Profil 2 SP5 Minute
#define ParamLMG_CHP2_SP5_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_Minute)))
// Profil 2 SP5 OffsetMinutes
#define ParamLMG_CHP2_SP5_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP5_OffsetMinutes)))
// Profil 2 SP5 ClampMode
#define ParamLMG_CHP2_SP5_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_ClampMode)))
// Profil 2 SP5 ClampHour
#define ParamLMG_CHP2_SP5_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_ClampHour)))
// Profil 2 SP5 ClampMinute
#define ParamLMG_CHP2_SP5_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_ClampMinute)))
// Profil 2 SP5 Kelvin
#define ParamLMG_CHP2_SP5_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP5_Kelvin)))
// Profil 2 SP5 Brightness
#define ParamLMG_CHP2_SP5_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_Brightness)))
// Profil 2 SP5 ExtColorTempMode
#define ParamLMG_CHP2_SP5_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_ExtColorTempMode)))
// Profil 2 SP5 ExtMixPercent
#define ParamLMG_CHP2_SP5_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP5_ExtMixPercent)))
// Profil 2 SP6 Active
#define ParamLMG_CHP2_SP6_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_Active)) & LMG_CHP2_SP6_ActiveMask))
// Profil 2 SP6 AnchorType
#define ParamLMG_CHP2_SP6_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_AnchorType)))
// Profil 2 SP6 Hour
#define ParamLMG_CHP2_SP6_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_Hour)))
// Profil 2 SP6 Minute
#define ParamLMG_CHP2_SP6_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_Minute)))
// Profil 2 SP6 OffsetMinutes
#define ParamLMG_CHP2_SP6_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP6_OffsetMinutes)))
// Profil 2 SP6 ClampMode
#define ParamLMG_CHP2_SP6_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_ClampMode)))
// Profil 2 SP6 ClampHour
#define ParamLMG_CHP2_SP6_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_ClampHour)))
// Profil 2 SP6 ClampMinute
#define ParamLMG_CHP2_SP6_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_ClampMinute)))
// Profil 2 SP6 Kelvin
#define ParamLMG_CHP2_SP6_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP6_Kelvin)))
// Profil 2 SP6 Brightness
#define ParamLMG_CHP2_SP6_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_Brightness)))
// Profil 2 SP6 ExtColorTempMode
#define ParamLMG_CHP2_SP6_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_ExtColorTempMode)))
// Profil 2 SP6 ExtMixPercent
#define ParamLMG_CHP2_SP6_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP6_ExtMixPercent)))
// Profil 2 SP7 Active
#define ParamLMG_CHP2_SP7_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_Active)) & LMG_CHP2_SP7_ActiveMask))
// Profil 2 SP7 AnchorType
#define ParamLMG_CHP2_SP7_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_AnchorType)))
// Profil 2 SP7 Hour
#define ParamLMG_CHP2_SP7_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_Hour)))
// Profil 2 SP7 Minute
#define ParamLMG_CHP2_SP7_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_Minute)))
// Profil 2 SP7 OffsetMinutes
#define ParamLMG_CHP2_SP7_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP7_OffsetMinutes)))
// Profil 2 SP7 ClampMode
#define ParamLMG_CHP2_SP7_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_ClampMode)))
// Profil 2 SP7 ClampHour
#define ParamLMG_CHP2_SP7_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_ClampHour)))
// Profil 2 SP7 ClampMinute
#define ParamLMG_CHP2_SP7_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_ClampMinute)))
// Profil 2 SP7 Kelvin
#define ParamLMG_CHP2_SP7_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP7_Kelvin)))
// Profil 2 SP7 Brightness
#define ParamLMG_CHP2_SP7_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_Brightness)))
// Profil 2 SP7 ExtColorTempMode
#define ParamLMG_CHP2_SP7_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_ExtColorTempMode)))
// Profil 2 SP7 ExtMixPercent
#define ParamLMG_CHP2_SP7_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP7_ExtMixPercent)))
// Profil 2 SP8 Active
#define ParamLMG_CHP2_SP8_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_Active)) & LMG_CHP2_SP8_ActiveMask))
// Profil 2 SP8 AnchorType
#define ParamLMG_CHP2_SP8_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_AnchorType)))
// Profil 2 SP8 Hour
#define ParamLMG_CHP2_SP8_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_Hour)))
// Profil 2 SP8 Minute
#define ParamLMG_CHP2_SP8_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_Minute)))
// Profil 2 SP8 OffsetMinutes
#define ParamLMG_CHP2_SP8_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP8_OffsetMinutes)))
// Profil 2 SP8 ClampMode
#define ParamLMG_CHP2_SP8_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_ClampMode)))
// Profil 2 SP8 ClampHour
#define ParamLMG_CHP2_SP8_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_ClampHour)))
// Profil 2 SP8 ClampMinute
#define ParamLMG_CHP2_SP8_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_ClampMinute)))
// Profil 2 SP8 Kelvin
#define ParamLMG_CHP2_SP8_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP8_Kelvin)))
// Profil 2 SP8 Brightness
#define ParamLMG_CHP2_SP8_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_Brightness)))
// Profil 2 SP8 ExtColorTempMode
#define ParamLMG_CHP2_SP8_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_ExtColorTempMode)))
// Profil 2 SP8 ExtMixPercent
#define ParamLMG_CHP2_SP8_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP8_ExtMixPercent)))
// Profil 2 SP9 Active
#define ParamLMG_CHP2_SP9_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_Active)) & LMG_CHP2_SP9_ActiveMask))
// Profil 2 SP9 AnchorType
#define ParamLMG_CHP2_SP9_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_AnchorType)))
// Profil 2 SP9 Hour
#define ParamLMG_CHP2_SP9_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_Hour)))
// Profil 2 SP9 Minute
#define ParamLMG_CHP2_SP9_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_Minute)))
// Profil 2 SP9 OffsetMinutes
#define ParamLMG_CHP2_SP9_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP9_OffsetMinutes)))
// Profil 2 SP9 ClampMode
#define ParamLMG_CHP2_SP9_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_ClampMode)))
// Profil 2 SP9 ClampHour
#define ParamLMG_CHP2_SP9_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_ClampHour)))
// Profil 2 SP9 ClampMinute
#define ParamLMG_CHP2_SP9_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_ClampMinute)))
// Profil 2 SP9 Kelvin
#define ParamLMG_CHP2_SP9_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP9_Kelvin)))
// Profil 2 SP9 Brightness
#define ParamLMG_CHP2_SP9_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_Brightness)))
// Profil 2 SP9 ExtColorTempMode
#define ParamLMG_CHP2_SP9_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_ExtColorTempMode)))
// Profil 2 SP9 ExtMixPercent
#define ParamLMG_CHP2_SP9_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP9_ExtMixPercent)))
// Profil 2 SP10 Active
#define ParamLMG_CHP2_SP10_Active                    ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_Active)) & LMG_CHP2_SP10_ActiveMask))
// Profil 2 SP10 AnchorType
#define ParamLMG_CHP2_SP10_AnchorType                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_AnchorType)))
// Profil 2 SP10 Hour
#define ParamLMG_CHP2_SP10_Hour                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_Hour)))
// Profil 2 SP10 Minute
#define ParamLMG_CHP2_SP10_Minute                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_Minute)))
// Profil 2 SP10 OffsetMinutes
#define ParamLMG_CHP2_SP10_OffsetMinutes             ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP10_OffsetMinutes)))
// Profil 2 SP10 ClampMode
#define ParamLMG_CHP2_SP10_ClampMode                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_ClampMode)))
// Profil 2 SP10 ClampHour
#define ParamLMG_CHP2_SP10_ClampHour                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_ClampHour)))
// Profil 2 SP10 ClampMinute
#define ParamLMG_CHP2_SP10_ClampMinute               (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_ClampMinute)))
// Profil 2 SP10 Kelvin
#define ParamLMG_CHP2_SP10_Kelvin                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP2_SP10_Kelvin)))
// Profil 2 SP10 Brightness
#define ParamLMG_CHP2_SP10_Brightness                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_Brightness)))
// Profil 2 SP10 ExtColorTempMode
#define ParamLMG_CHP2_SP10_ExtColorTempMode          (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_ExtColorTempMode)))
// Profil 2 SP10 ExtMixPercent
#define ParamLMG_CHP2_SP10_ExtMixPercent             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP2_SP10_ExtMixPercent)))
// Profil 3 SP1 Active
#define ParamLMG_CHP3_SP1_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_Active)) & LMG_CHP3_SP1_ActiveMask))
// Profil 3 SP1 AnchorType
#define ParamLMG_CHP3_SP1_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_AnchorType)))
// Profil 3 SP1 Hour
#define ParamLMG_CHP3_SP1_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_Hour)))
// Profil 3 SP1 Minute
#define ParamLMG_CHP3_SP1_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_Minute)))
// Profil 3 SP1 OffsetMinutes
#define ParamLMG_CHP3_SP1_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP1_OffsetMinutes)))
// Profil 3 SP1 ClampMode
#define ParamLMG_CHP3_SP1_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_ClampMode)))
// Profil 3 SP1 ClampHour
#define ParamLMG_CHP3_SP1_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_ClampHour)))
// Profil 3 SP1 ClampMinute
#define ParamLMG_CHP3_SP1_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_ClampMinute)))
// Profil 3 SP1 Kelvin
#define ParamLMG_CHP3_SP1_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP1_Kelvin)))
// Profil 3 SP1 Brightness
#define ParamLMG_CHP3_SP1_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_Brightness)))
// Profil 3 SP1 ExtColorTempMode
#define ParamLMG_CHP3_SP1_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_ExtColorTempMode)))
// Profil 3 SP1 ExtMixPercent
#define ParamLMG_CHP3_SP1_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP1_ExtMixPercent)))
// Profil 3 SP2 Active
#define ParamLMG_CHP3_SP2_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_Active)) & LMG_CHP3_SP2_ActiveMask))
// Profil 3 SP2 AnchorType
#define ParamLMG_CHP3_SP2_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_AnchorType)))
// Profil 3 SP2 Hour
#define ParamLMG_CHP3_SP2_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_Hour)))
// Profil 3 SP2 Minute
#define ParamLMG_CHP3_SP2_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_Minute)))
// Profil 3 SP2 OffsetMinutes
#define ParamLMG_CHP3_SP2_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP2_OffsetMinutes)))
// Profil 3 SP2 ClampMode
#define ParamLMG_CHP3_SP2_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_ClampMode)))
// Profil 3 SP2 ClampHour
#define ParamLMG_CHP3_SP2_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_ClampHour)))
// Profil 3 SP2 ClampMinute
#define ParamLMG_CHP3_SP2_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_ClampMinute)))
// Profil 3 SP2 Kelvin
#define ParamLMG_CHP3_SP2_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP2_Kelvin)))
// Profil 3 SP2 Brightness
#define ParamLMG_CHP3_SP2_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_Brightness)))
// Profil 3 SP2 ExtColorTempMode
#define ParamLMG_CHP3_SP2_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_ExtColorTempMode)))
// Profil 3 SP2 ExtMixPercent
#define ParamLMG_CHP3_SP2_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP2_ExtMixPercent)))
// Profil 3 SP3 Active
#define ParamLMG_CHP3_SP3_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_Active)) & LMG_CHP3_SP3_ActiveMask))
// Profil 3 SP3 AnchorType
#define ParamLMG_CHP3_SP3_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_AnchorType)))
// Profil 3 SP3 Hour
#define ParamLMG_CHP3_SP3_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_Hour)))
// Profil 3 SP3 Minute
#define ParamLMG_CHP3_SP3_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_Minute)))
// Profil 3 SP3 OffsetMinutes
#define ParamLMG_CHP3_SP3_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP3_OffsetMinutes)))
// Profil 3 SP3 ClampMode
#define ParamLMG_CHP3_SP3_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_ClampMode)))
// Profil 3 SP3 ClampHour
#define ParamLMG_CHP3_SP3_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_ClampHour)))
// Profil 3 SP3 ClampMinute
#define ParamLMG_CHP3_SP3_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_ClampMinute)))
// Profil 3 SP3 Kelvin
#define ParamLMG_CHP3_SP3_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP3_Kelvin)))
// Profil 3 SP3 Brightness
#define ParamLMG_CHP3_SP3_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_Brightness)))
// Profil 3 SP3 ExtColorTempMode
#define ParamLMG_CHP3_SP3_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_ExtColorTempMode)))
// Profil 3 SP3 ExtMixPercent
#define ParamLMG_CHP3_SP3_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP3_ExtMixPercent)))
// Profil 3 SP4 Active
#define ParamLMG_CHP3_SP4_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_Active)) & LMG_CHP3_SP4_ActiveMask))
// Profil 3 SP4 AnchorType
#define ParamLMG_CHP3_SP4_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_AnchorType)))
// Profil 3 SP4 Hour
#define ParamLMG_CHP3_SP4_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_Hour)))
// Profil 3 SP4 Minute
#define ParamLMG_CHP3_SP4_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_Minute)))
// Profil 3 SP4 OffsetMinutes
#define ParamLMG_CHP3_SP4_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP4_OffsetMinutes)))
// Profil 3 SP4 ClampMode
#define ParamLMG_CHP3_SP4_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_ClampMode)))
// Profil 3 SP4 ClampHour
#define ParamLMG_CHP3_SP4_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_ClampHour)))
// Profil 3 SP4 ClampMinute
#define ParamLMG_CHP3_SP4_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_ClampMinute)))
// Profil 3 SP4 Kelvin
#define ParamLMG_CHP3_SP4_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP4_Kelvin)))
// Profil 3 SP4 Brightness
#define ParamLMG_CHP3_SP4_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_Brightness)))
// Profil 3 SP4 ExtColorTempMode
#define ParamLMG_CHP3_SP4_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_ExtColorTempMode)))
// Profil 3 SP4 ExtMixPercent
#define ParamLMG_CHP3_SP4_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP4_ExtMixPercent)))
// Profil 3 SP5 Active
#define ParamLMG_CHP3_SP5_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_Active)) & LMG_CHP3_SP5_ActiveMask))
// Profil 3 SP5 AnchorType
#define ParamLMG_CHP3_SP5_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_AnchorType)))
// Profil 3 SP5 Hour
#define ParamLMG_CHP3_SP5_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_Hour)))
// Profil 3 SP5 Minute
#define ParamLMG_CHP3_SP5_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_Minute)))
// Profil 3 SP5 OffsetMinutes
#define ParamLMG_CHP3_SP5_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP5_OffsetMinutes)))
// Profil 3 SP5 ClampMode
#define ParamLMG_CHP3_SP5_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_ClampMode)))
// Profil 3 SP5 ClampHour
#define ParamLMG_CHP3_SP5_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_ClampHour)))
// Profil 3 SP5 ClampMinute
#define ParamLMG_CHP3_SP5_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_ClampMinute)))
// Profil 3 SP5 Kelvin
#define ParamLMG_CHP3_SP5_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP5_Kelvin)))
// Profil 3 SP5 Brightness
#define ParamLMG_CHP3_SP5_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_Brightness)))
// Profil 3 SP5 ExtColorTempMode
#define ParamLMG_CHP3_SP5_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_ExtColorTempMode)))
// Profil 3 SP5 ExtMixPercent
#define ParamLMG_CHP3_SP5_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP5_ExtMixPercent)))
// Profil 3 SP6 Active
#define ParamLMG_CHP3_SP6_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_Active)) & LMG_CHP3_SP6_ActiveMask))
// Profil 3 SP6 AnchorType
#define ParamLMG_CHP3_SP6_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_AnchorType)))
// Profil 3 SP6 Hour
#define ParamLMG_CHP3_SP6_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_Hour)))
// Profil 3 SP6 Minute
#define ParamLMG_CHP3_SP6_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_Minute)))
// Profil 3 SP6 OffsetMinutes
#define ParamLMG_CHP3_SP6_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP6_OffsetMinutes)))
// Profil 3 SP6 ClampMode
#define ParamLMG_CHP3_SP6_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_ClampMode)))
// Profil 3 SP6 ClampHour
#define ParamLMG_CHP3_SP6_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_ClampHour)))
// Profil 3 SP6 ClampMinute
#define ParamLMG_CHP3_SP6_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_ClampMinute)))
// Profil 3 SP6 Kelvin
#define ParamLMG_CHP3_SP6_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP6_Kelvin)))
// Profil 3 SP6 Brightness
#define ParamLMG_CHP3_SP6_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_Brightness)))
// Profil 3 SP6 ExtColorTempMode
#define ParamLMG_CHP3_SP6_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_ExtColorTempMode)))
// Profil 3 SP6 ExtMixPercent
#define ParamLMG_CHP3_SP6_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP6_ExtMixPercent)))
// Profil 3 SP7 Active
#define ParamLMG_CHP3_SP7_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_Active)) & LMG_CHP3_SP7_ActiveMask))
// Profil 3 SP7 AnchorType
#define ParamLMG_CHP3_SP7_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_AnchorType)))
// Profil 3 SP7 Hour
#define ParamLMG_CHP3_SP7_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_Hour)))
// Profil 3 SP7 Minute
#define ParamLMG_CHP3_SP7_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_Minute)))
// Profil 3 SP7 OffsetMinutes
#define ParamLMG_CHP3_SP7_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP7_OffsetMinutes)))
// Profil 3 SP7 ClampMode
#define ParamLMG_CHP3_SP7_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_ClampMode)))
// Profil 3 SP7 ClampHour
#define ParamLMG_CHP3_SP7_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_ClampHour)))
// Profil 3 SP7 ClampMinute
#define ParamLMG_CHP3_SP7_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_ClampMinute)))
// Profil 3 SP7 Kelvin
#define ParamLMG_CHP3_SP7_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP7_Kelvin)))
// Profil 3 SP7 Brightness
#define ParamLMG_CHP3_SP7_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_Brightness)))
// Profil 3 SP7 ExtColorTempMode
#define ParamLMG_CHP3_SP7_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_ExtColorTempMode)))
// Profil 3 SP7 ExtMixPercent
#define ParamLMG_CHP3_SP7_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP7_ExtMixPercent)))
// Profil 3 SP8 Active
#define ParamLMG_CHP3_SP8_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_Active)) & LMG_CHP3_SP8_ActiveMask))
// Profil 3 SP8 AnchorType
#define ParamLMG_CHP3_SP8_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_AnchorType)))
// Profil 3 SP8 Hour
#define ParamLMG_CHP3_SP8_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_Hour)))
// Profil 3 SP8 Minute
#define ParamLMG_CHP3_SP8_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_Minute)))
// Profil 3 SP8 OffsetMinutes
#define ParamLMG_CHP3_SP8_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP8_OffsetMinutes)))
// Profil 3 SP8 ClampMode
#define ParamLMG_CHP3_SP8_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_ClampMode)))
// Profil 3 SP8 ClampHour
#define ParamLMG_CHP3_SP8_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_ClampHour)))
// Profil 3 SP8 ClampMinute
#define ParamLMG_CHP3_SP8_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_ClampMinute)))
// Profil 3 SP8 Kelvin
#define ParamLMG_CHP3_SP8_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP8_Kelvin)))
// Profil 3 SP8 Brightness
#define ParamLMG_CHP3_SP8_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_Brightness)))
// Profil 3 SP8 ExtColorTempMode
#define ParamLMG_CHP3_SP8_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_ExtColorTempMode)))
// Profil 3 SP8 ExtMixPercent
#define ParamLMG_CHP3_SP8_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP8_ExtMixPercent)))
// Profil 3 SP9 Active
#define ParamLMG_CHP3_SP9_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_Active)) & LMG_CHP3_SP9_ActiveMask))
// Profil 3 SP9 AnchorType
#define ParamLMG_CHP3_SP9_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_AnchorType)))
// Profil 3 SP9 Hour
#define ParamLMG_CHP3_SP9_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_Hour)))
// Profil 3 SP9 Minute
#define ParamLMG_CHP3_SP9_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_Minute)))
// Profil 3 SP9 OffsetMinutes
#define ParamLMG_CHP3_SP9_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP9_OffsetMinutes)))
// Profil 3 SP9 ClampMode
#define ParamLMG_CHP3_SP9_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_ClampMode)))
// Profil 3 SP9 ClampHour
#define ParamLMG_CHP3_SP9_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_ClampHour)))
// Profil 3 SP9 ClampMinute
#define ParamLMG_CHP3_SP9_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_ClampMinute)))
// Profil 3 SP9 Kelvin
#define ParamLMG_CHP3_SP9_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP9_Kelvin)))
// Profil 3 SP9 Brightness
#define ParamLMG_CHP3_SP9_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_Brightness)))
// Profil 3 SP9 ExtColorTempMode
#define ParamLMG_CHP3_SP9_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_ExtColorTempMode)))
// Profil 3 SP9 ExtMixPercent
#define ParamLMG_CHP3_SP9_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP9_ExtMixPercent)))
// Profil 3 SP10 Active
#define ParamLMG_CHP3_SP10_Active                    ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_Active)) & LMG_CHP3_SP10_ActiveMask))
// Profil 3 SP10 AnchorType
#define ParamLMG_CHP3_SP10_AnchorType                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_AnchorType)))
// Profil 3 SP10 Hour
#define ParamLMG_CHP3_SP10_Hour                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_Hour)))
// Profil 3 SP10 Minute
#define ParamLMG_CHP3_SP10_Minute                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_Minute)))
// Profil 3 SP10 OffsetMinutes
#define ParamLMG_CHP3_SP10_OffsetMinutes             ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP10_OffsetMinutes)))
// Profil 3 SP10 ClampMode
#define ParamLMG_CHP3_SP10_ClampMode                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_ClampMode)))
// Profil 3 SP10 ClampHour
#define ParamLMG_CHP3_SP10_ClampHour                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_ClampHour)))
// Profil 3 SP10 ClampMinute
#define ParamLMG_CHP3_SP10_ClampMinute               (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_ClampMinute)))
// Profil 3 SP10 Kelvin
#define ParamLMG_CHP3_SP10_Kelvin                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP3_SP10_Kelvin)))
// Profil 3 SP10 Brightness
#define ParamLMG_CHP3_SP10_Brightness                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_Brightness)))
// Profil 3 SP10 ExtColorTempMode
#define ParamLMG_CHP3_SP10_ExtColorTempMode          (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_ExtColorTempMode)))
// Profil 3 SP10 ExtMixPercent
#define ParamLMG_CHP3_SP10_ExtMixPercent             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP3_SP10_ExtMixPercent)))
// Profil 4 SP1 Active
#define ParamLMG_CHP4_SP1_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_Active)) & LMG_CHP4_SP1_ActiveMask))
// Profil 4 SP1 AnchorType
#define ParamLMG_CHP4_SP1_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_AnchorType)))
// Profil 4 SP1 Hour
#define ParamLMG_CHP4_SP1_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_Hour)))
// Profil 4 SP1 Minute
#define ParamLMG_CHP4_SP1_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_Minute)))
// Profil 4 SP1 OffsetMinutes
#define ParamLMG_CHP4_SP1_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP1_OffsetMinutes)))
// Profil 4 SP1 ClampMode
#define ParamLMG_CHP4_SP1_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_ClampMode)))
// Profil 4 SP1 ClampHour
#define ParamLMG_CHP4_SP1_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_ClampHour)))
// Profil 4 SP1 ClampMinute
#define ParamLMG_CHP4_SP1_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_ClampMinute)))
// Profil 4 SP1 Kelvin
#define ParamLMG_CHP4_SP1_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP1_Kelvin)))
// Profil 4 SP1 Brightness
#define ParamLMG_CHP4_SP1_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_Brightness)))
// Profil 4 SP1 ExtColorTempMode
#define ParamLMG_CHP4_SP1_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_ExtColorTempMode)))
// Profil 4 SP1 ExtMixPercent
#define ParamLMG_CHP4_SP1_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP1_ExtMixPercent)))
// Profil 4 SP2 Active
#define ParamLMG_CHP4_SP2_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_Active)) & LMG_CHP4_SP2_ActiveMask))
// Profil 4 SP2 AnchorType
#define ParamLMG_CHP4_SP2_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_AnchorType)))
// Profil 4 SP2 Hour
#define ParamLMG_CHP4_SP2_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_Hour)))
// Profil 4 SP2 Minute
#define ParamLMG_CHP4_SP2_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_Minute)))
// Profil 4 SP2 OffsetMinutes
#define ParamLMG_CHP4_SP2_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP2_OffsetMinutes)))
// Profil 4 SP2 ClampMode
#define ParamLMG_CHP4_SP2_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_ClampMode)))
// Profil 4 SP2 ClampHour
#define ParamLMG_CHP4_SP2_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_ClampHour)))
// Profil 4 SP2 ClampMinute
#define ParamLMG_CHP4_SP2_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_ClampMinute)))
// Profil 4 SP2 Kelvin
#define ParamLMG_CHP4_SP2_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP2_Kelvin)))
// Profil 4 SP2 Brightness
#define ParamLMG_CHP4_SP2_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_Brightness)))
// Profil 4 SP2 ExtColorTempMode
#define ParamLMG_CHP4_SP2_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_ExtColorTempMode)))
// Profil 4 SP2 ExtMixPercent
#define ParamLMG_CHP4_SP2_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP2_ExtMixPercent)))
// Profil 4 SP3 Active
#define ParamLMG_CHP4_SP3_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_Active)) & LMG_CHP4_SP3_ActiveMask))
// Profil 4 SP3 AnchorType
#define ParamLMG_CHP4_SP3_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_AnchorType)))
// Profil 4 SP3 Hour
#define ParamLMG_CHP4_SP3_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_Hour)))
// Profil 4 SP3 Minute
#define ParamLMG_CHP4_SP3_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_Minute)))
// Profil 4 SP3 OffsetMinutes
#define ParamLMG_CHP4_SP3_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP3_OffsetMinutes)))
// Profil 4 SP3 ClampMode
#define ParamLMG_CHP4_SP3_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_ClampMode)))
// Profil 4 SP3 ClampHour
#define ParamLMG_CHP4_SP3_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_ClampHour)))
// Profil 4 SP3 ClampMinute
#define ParamLMG_CHP4_SP3_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_ClampMinute)))
// Profil 4 SP3 Kelvin
#define ParamLMG_CHP4_SP3_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP3_Kelvin)))
// Profil 4 SP3 Brightness
#define ParamLMG_CHP4_SP3_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_Brightness)))
// Profil 4 SP3 ExtColorTempMode
#define ParamLMG_CHP4_SP3_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_ExtColorTempMode)))
// Profil 4 SP3 ExtMixPercent
#define ParamLMG_CHP4_SP3_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP3_ExtMixPercent)))
// Profil 4 SP4 Active
#define ParamLMG_CHP4_SP4_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_Active)) & LMG_CHP4_SP4_ActiveMask))
// Profil 4 SP4 AnchorType
#define ParamLMG_CHP4_SP4_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_AnchorType)))
// Profil 4 SP4 Hour
#define ParamLMG_CHP4_SP4_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_Hour)))
// Profil 4 SP4 Minute
#define ParamLMG_CHP4_SP4_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_Minute)))
// Profil 4 SP4 OffsetMinutes
#define ParamLMG_CHP4_SP4_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP4_OffsetMinutes)))
// Profil 4 SP4 ClampMode
#define ParamLMG_CHP4_SP4_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_ClampMode)))
// Profil 4 SP4 ClampHour
#define ParamLMG_CHP4_SP4_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_ClampHour)))
// Profil 4 SP4 ClampMinute
#define ParamLMG_CHP4_SP4_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_ClampMinute)))
// Profil 4 SP4 Kelvin
#define ParamLMG_CHP4_SP4_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP4_Kelvin)))
// Profil 4 SP4 Brightness
#define ParamLMG_CHP4_SP4_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_Brightness)))
// Profil 4 SP4 ExtColorTempMode
#define ParamLMG_CHP4_SP4_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_ExtColorTempMode)))
// Profil 4 SP4 ExtMixPercent
#define ParamLMG_CHP4_SP4_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP4_ExtMixPercent)))
// Profil 4 SP5 Active
#define ParamLMG_CHP4_SP5_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_Active)) & LMG_CHP4_SP5_ActiveMask))
// Profil 4 SP5 AnchorType
#define ParamLMG_CHP4_SP5_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_AnchorType)))
// Profil 4 SP5 Hour
#define ParamLMG_CHP4_SP5_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_Hour)))
// Profil 4 SP5 Minute
#define ParamLMG_CHP4_SP5_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_Minute)))
// Profil 4 SP5 OffsetMinutes
#define ParamLMG_CHP4_SP5_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP5_OffsetMinutes)))
// Profil 4 SP5 ClampMode
#define ParamLMG_CHP4_SP5_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_ClampMode)))
// Profil 4 SP5 ClampHour
#define ParamLMG_CHP4_SP5_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_ClampHour)))
// Profil 4 SP5 ClampMinute
#define ParamLMG_CHP4_SP5_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_ClampMinute)))
// Profil 4 SP5 Kelvin
#define ParamLMG_CHP4_SP5_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP5_Kelvin)))
// Profil 4 SP5 Brightness
#define ParamLMG_CHP4_SP5_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_Brightness)))
// Profil 4 SP5 ExtColorTempMode
#define ParamLMG_CHP4_SP5_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_ExtColorTempMode)))
// Profil 4 SP5 ExtMixPercent
#define ParamLMG_CHP4_SP5_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP5_ExtMixPercent)))
// Profil 4 SP6 Active
#define ParamLMG_CHP4_SP6_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_Active)) & LMG_CHP4_SP6_ActiveMask))
// Profil 4 SP6 AnchorType
#define ParamLMG_CHP4_SP6_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_AnchorType)))
// Profil 4 SP6 Hour
#define ParamLMG_CHP4_SP6_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_Hour)))
// Profil 4 SP6 Minute
#define ParamLMG_CHP4_SP6_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_Minute)))
// Profil 4 SP6 OffsetMinutes
#define ParamLMG_CHP4_SP6_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP6_OffsetMinutes)))
// Profil 4 SP6 ClampMode
#define ParamLMG_CHP4_SP6_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_ClampMode)))
// Profil 4 SP6 ClampHour
#define ParamLMG_CHP4_SP6_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_ClampHour)))
// Profil 4 SP6 ClampMinute
#define ParamLMG_CHP4_SP6_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_ClampMinute)))
// Profil 4 SP6 Kelvin
#define ParamLMG_CHP4_SP6_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP6_Kelvin)))
// Profil 4 SP6 Brightness
#define ParamLMG_CHP4_SP6_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_Brightness)))
// Profil 4 SP6 ExtColorTempMode
#define ParamLMG_CHP4_SP6_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_ExtColorTempMode)))
// Profil 4 SP6 ExtMixPercent
#define ParamLMG_CHP4_SP6_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP6_ExtMixPercent)))
// Profil 4 SP7 Active
#define ParamLMG_CHP4_SP7_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_Active)) & LMG_CHP4_SP7_ActiveMask))
// Profil 4 SP7 AnchorType
#define ParamLMG_CHP4_SP7_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_AnchorType)))
// Profil 4 SP7 Hour
#define ParamLMG_CHP4_SP7_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_Hour)))
// Profil 4 SP7 Minute
#define ParamLMG_CHP4_SP7_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_Minute)))
// Profil 4 SP7 OffsetMinutes
#define ParamLMG_CHP4_SP7_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP7_OffsetMinutes)))
// Profil 4 SP7 ClampMode
#define ParamLMG_CHP4_SP7_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_ClampMode)))
// Profil 4 SP7 ClampHour
#define ParamLMG_CHP4_SP7_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_ClampHour)))
// Profil 4 SP7 ClampMinute
#define ParamLMG_CHP4_SP7_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_ClampMinute)))
// Profil 4 SP7 Kelvin
#define ParamLMG_CHP4_SP7_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP7_Kelvin)))
// Profil 4 SP7 Brightness
#define ParamLMG_CHP4_SP7_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_Brightness)))
// Profil 4 SP7 ExtColorTempMode
#define ParamLMG_CHP4_SP7_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_ExtColorTempMode)))
// Profil 4 SP7 ExtMixPercent
#define ParamLMG_CHP4_SP7_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP7_ExtMixPercent)))
// Profil 4 SP8 Active
#define ParamLMG_CHP4_SP8_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_Active)) & LMG_CHP4_SP8_ActiveMask))
// Profil 4 SP8 AnchorType
#define ParamLMG_CHP4_SP8_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_AnchorType)))
// Profil 4 SP8 Hour
#define ParamLMG_CHP4_SP8_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_Hour)))
// Profil 4 SP8 Minute
#define ParamLMG_CHP4_SP8_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_Minute)))
// Profil 4 SP8 OffsetMinutes
#define ParamLMG_CHP4_SP8_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP8_OffsetMinutes)))
// Profil 4 SP8 ClampMode
#define ParamLMG_CHP4_SP8_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_ClampMode)))
// Profil 4 SP8 ClampHour
#define ParamLMG_CHP4_SP8_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_ClampHour)))
// Profil 4 SP8 ClampMinute
#define ParamLMG_CHP4_SP8_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_ClampMinute)))
// Profil 4 SP8 Kelvin
#define ParamLMG_CHP4_SP8_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP8_Kelvin)))
// Profil 4 SP8 Brightness
#define ParamLMG_CHP4_SP8_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_Brightness)))
// Profil 4 SP8 ExtColorTempMode
#define ParamLMG_CHP4_SP8_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_ExtColorTempMode)))
// Profil 4 SP8 ExtMixPercent
#define ParamLMG_CHP4_SP8_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP8_ExtMixPercent)))
// Profil 4 SP9 Active
#define ParamLMG_CHP4_SP9_Active                     ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_Active)) & LMG_CHP4_SP9_ActiveMask))
// Profil 4 SP9 AnchorType
#define ParamLMG_CHP4_SP9_AnchorType                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_AnchorType)))
// Profil 4 SP9 Hour
#define ParamLMG_CHP4_SP9_Hour                       (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_Hour)))
// Profil 4 SP9 Minute
#define ParamLMG_CHP4_SP9_Minute                     (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_Minute)))
// Profil 4 SP9 OffsetMinutes
#define ParamLMG_CHP4_SP9_OffsetMinutes              ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP9_OffsetMinutes)))
// Profil 4 SP9 ClampMode
#define ParamLMG_CHP4_SP9_ClampMode                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_ClampMode)))
// Profil 4 SP9 ClampHour
#define ParamLMG_CHP4_SP9_ClampHour                  (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_ClampHour)))
// Profil 4 SP9 ClampMinute
#define ParamLMG_CHP4_SP9_ClampMinute                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_ClampMinute)))
// Profil 4 SP9 Kelvin
#define ParamLMG_CHP4_SP9_Kelvin                     (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP9_Kelvin)))
// Profil 4 SP9 Brightness
#define ParamLMG_CHP4_SP9_Brightness                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_Brightness)))
// Profil 4 SP9 ExtColorTempMode
#define ParamLMG_CHP4_SP9_ExtColorTempMode           (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_ExtColorTempMode)))
// Profil 4 SP9 ExtMixPercent
#define ParamLMG_CHP4_SP9_ExtMixPercent              (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP9_ExtMixPercent)))
// Profil 4 SP10 Active
#define ParamLMG_CHP4_SP10_Active                    ((bool)(knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_Active)) & LMG_CHP4_SP10_ActiveMask))
// Profil 4 SP10 AnchorType
#define ParamLMG_CHP4_SP10_AnchorType                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_AnchorType)))
// Profil 4 SP10 Hour
#define ParamLMG_CHP4_SP10_Hour                      (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_Hour)))
// Profil 4 SP10 Minute
#define ParamLMG_CHP4_SP10_Minute                    (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_Minute)))
// Profil 4 SP10 OffsetMinutes
#define ParamLMG_CHP4_SP10_OffsetMinutes             ((int16_t)knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP10_OffsetMinutes)))
// Profil 4 SP10 ClampMode
#define ParamLMG_CHP4_SP10_ClampMode                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_ClampMode)))
// Profil 4 SP10 ClampHour
#define ParamLMG_CHP4_SP10_ClampHour                 (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_ClampHour)))
// Profil 4 SP10 ClampMinute
#define ParamLMG_CHP4_SP10_ClampMinute               (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_ClampMinute)))
// Profil 4 SP10 Kelvin
#define ParamLMG_CHP4_SP10_Kelvin                    (knx.paramWord(LMG_ParamCalcIndex(LMG_CHP4_SP10_Kelvin)))
// Profil 4 SP10 Brightness
#define ParamLMG_CHP4_SP10_Brightness                (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_Brightness)))
// Profil 4 SP10 ExtColorTempMode
#define ParamLMG_CHP4_SP10_ExtColorTempMode          (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_ExtColorTempMode)))
// Profil 4 SP10 ExtMixPercent
#define ParamLMG_CHP4_SP10_ExtMixPercent             (knx.paramByte(LMG_ParamCalcIndex(LMG_CHP4_SP10_ExtMixPercent)))

// deprecated
#define LMG_KoOffset 1500

// Communication objects per channel (multiple occurrence)
#define LMG_KoBlockOffset 1500
#define LMG_KoBlockSize 22

#define LMG_KoCalcNumber(index) (index + LMG_KoBlockOffset + _channelIndex * LMG_KoBlockSize)
#define LMG_KoCalcIndex(number) ((number >= LMG_KoCalcNumber(0) && number < LMG_KoCalcNumber(LMG_KoBlockSize)) ? (number - LMG_KoBlockOffset) % LMG_KoBlockSize : -1)
#define LMG_KoCalcChannel(number) ((number >= LMG_KoBlockOffset && number < LMG_KoBlockOffset + LMG_ChannelCount * LMG_KoBlockSize) ? (number - LMG_KoBlockOffset) / LMG_KoBlockSize : -1)

#define LMG_KoCHStatusBrightness 0
#define LMG_KoCHStatusColorTemp 1
#define LMG_KoCHLock 2
#define LMG_KoCHLockStatus 3
#define LMG_KoCHSummerActive 4
#define LMG_KoCHAmbientLux 5
#define LMG_KoCHDayNight 6
#define LMG_KoCHAdaptiveActive 7
#define LMG_KoCHStatusCombined 8
#define LMG_KoCHLockColor 9
#define LMG_KoCHLockBrightness 10
#define LMG_KoCHPreviewMinutes 11
#define LMG_KoCHPreviewColorTemp 12
#define LMG_KoCHPreviewBrightness 13
#define LMG_KoCHDayProgress 14
#define LMG_KoCHDayPhase 15
#define LMG_KoCHExtBrightnessPercent 16
#define LMG_KoCHExtBrightnessLux 17
#define LMG_KoCHExtColorTempKelvin 18
#define LMG_KoCHExtColorTempScalar 19
#define LMG_KoCHReserve1 20
#define LMG_KoCHReserve2 21

// LM %C%: Status-Soll-Helligkeit
#define KoLMG_CHStatusBrightness                  (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHStatusBrightness)))
// LM %C%: Status-Soll-Farbtemperatur
#define KoLMG_CHStatusColorTemp                   (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHStatusColorTemp)))
// LM %C%: Sperre
#define KoLMG_CHLock                              (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHLock)))
// LM %C%: Status Sperre
#define KoLMG_CHLockStatus                        (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHLockStatus)))
// LM %C%: Sommer aktiv
#define KoLMG_CHSummerActive                      (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHSummerActive)))
// LM %C%: Umgebungslicht (Lux)
#define KoLMG_CHAmbientLux                        (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHAmbientLux)))
// LM %C%: Tag/Nacht
#define KoLMG_CHDayNight                          (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHDayNight)))
// LM %C%: Adaptive Helligkeit aktiv
#define KoLMG_CHAdaptiveActive                    (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHAdaptiveActive)))
// LM %C%: Status-Soll Kombi (Tunable White)
#define KoLMG_CHStatusCombined                    (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHStatusCombined)))
// LM %C%: Sperre Farbtemperatur
#define KoLMG_CHLockColor                         (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHLockColor)))
// LM %C%: Sperre Helligkeit
#define KoLMG_CHLockBrightness                    (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHLockBrightness)))
// LM %C%: Vorausschau Minuten
#define KoLMG_CHPreviewMinutes                    (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHPreviewMinutes)))
// LM %C%: Vorausschau Farbtemperatur
#define KoLMG_CHPreviewColorTemp                  (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHPreviewColorTemp)))
// LM %C%: Vorausschau Helligkeit
#define KoLMG_CHPreviewBrightness                 (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHPreviewBrightness)))
// LM %C%: Tages-Fortschritt
#define KoLMG_CHDayProgress                       (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHDayProgress)))
// LM %C%: Tagesphase
#define KoLMG_CHDayPhase                          (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHDayPhase)))
// LM %C%: Externe Helligkeit (Prozent)
#define KoLMG_CHExtBrightnessPercent              (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHExtBrightnessPercent)))
// LM %C%: Externe Helligkeit (Lux)
#define KoLMG_CHExtBrightnessLux                  (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHExtBrightnessLux)))
// LM %C%: Externe Farbtemperatur (Kelvin)
#define KoLMG_CHExtColorTempKelvin                (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHExtColorTempKelvin)))
// LM %C%: Externe Farbtemperatur (Skalar)
#define KoLMG_CHExtColorTempScalar                (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHExtColorTempScalar)))
// reserved
#define KoLMG_CHReserve1                          (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHReserve1)))
// reserved
#define KoLMG_CHReserve2                          (knx.getGroupObject(LMG_KoCalcNumber(LMG_KoCHReserve2)))

#define NEO_HCLMaster                           24009      // 8 Bits, Bit 7-0
#define NEO_HCLApplyMode                        24010      // 3 Bits, Bit 7-5
#define     NEO_HCLApplyModeMask 0xE0
#define     NEO_HCLApplyModeShift 5
#define NEO_NeoPixelHardwareSelect              24029      // 16 Bits, Bit 15-0
#define NEO_NEONumberOfLEDStrips                24031      // 4 Bits, Bit 7-4
#define     NEO_NEONumberOfLEDStripsMask 0xF0
#define     NEO_NEONumberOfLEDStripsShift 4
#define NEO_NEONumberOfSegments                 24032      // 5 Bits, Bit 7-3
#define     NEO_NEONumberOfSegmentsMask 0xF8
#define     NEO_NEONumberOfSegmentsShift 3
#define NEO_NEOTurnOffBeforeRestart             24032      // 1 Bit, Bit 2
#define     NEO_NEOTurnOffBeforeRestartMask 0x04
#define     NEO_NEOTurnOffBeforeRestartShift 2
#define NEO_NEOUpdateSpeed                      24033      // 8 Bits, Bit 7-0
#define NEO_NEOpowerLimitGlobal                 24034      // uint16_t
#define NEO_NEOcurrentPerLED                    24036      // 7 Bits, Bit 7-1
#define     NEO_NEOcurrentPerLEDMask 0xFE
#define     NEO_NEOcurrentPerLEDShift 1
#define NEO_NEOcurrentPerChannel                24037      // uint16_t
#define NEO_NEOpowerLimitCombined               24039      // 3 Bits, Bit 7-5
#define     NEO_NEOpowerLimitCombinedMask 0xE0
#define     NEO_NEOpowerLimitCombinedShift 5
#define NEO_NEOautoBrightnessLimit              24040      // 7 Bits, Bit 7-1
#define     NEO_NEOautoBrightnessLimitMask 0xFE
#define     NEO_NEOautoBrightnessLimitShift 1
#define NEO_NEOpowerLimitThreshold              24041      // 7 Bits, Bit 7-1
#define     NEO_NEOpowerLimitThresholdMask 0xFE
#define     NEO_NEOpowerLimitThresholdShift 1
#define NEO_NEOablSlewRatePercent               24042      // 7 Bits, Bit 7-1
#define     NEO_NEOablSlewRatePercentMask 0xFE
#define     NEO_NEOablSlewRatePercentShift 1
#define NEO_NEOEMEnabled                        24043      // uint8_t
#define NEO_NEOGlobalStartupBehavior            24045      // 2 Bits, Bit 7-6
#define     NEO_NEOGlobalStartupBehaviorMask 0xC0
#define     NEO_NEOGlobalStartupBehaviorShift 6
#define NEO_NEOGlobalStartupR                   24046      // uint8_t
#define NEO_NEOGlobalStartupG                   24047      // uint8_t
#define NEO_NEOGlobalStartupB                   24048      // uint8_t
#define NEO_NEOGlobalStartupW                   24049      // uint8_t
#define NEO_NEOGlobalStartupBrightness          24050      // uint8_t
#define NEO_NEOGlobalStartupEffect              24051      // 8 Bits, Bit 7-0
#define NEO_VirtualStripPos1                    24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1Mask 0xF0
#define     NEO_VirtualStripPos1Shift 4
#define NEO_VirtualStripPos2                    24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2Mask 0x0F
#define     NEO_VirtualStripPos2Shift 0
#define NEO_VirtualStripPos3                    24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3Mask 0xF0
#define     NEO_VirtualStripPos3Shift 4
#define NEO_VirtualStripPos4                    24053      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4Mask 0x0F
#define     NEO_VirtualStripPos4Shift 0
#define NEO_VirtualStripPos5                    24054      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5Mask 0xF0
#define     NEO_VirtualStripPos5Shift 4
#define NEO_VirtualStripPos6                    24054      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos6Mask 0x0F
#define     NEO_VirtualStripPos6Shift 0
#define NEO_VirtualStripPos7                    24055      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos7Mask 0xF0
#define     NEO_VirtualStripPos7Shift 4
#define NEO_VirtualStripPos8                    24055      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos8Mask 0x0F
#define     NEO_VirtualStripPos8Shift 0
#define NEO_VirtualStripPos1_1                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_1Mask 0xF0
#define     NEO_VirtualStripPos1_1Shift 4
#define NEO_VirtualStripPos1_2                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_2Mask 0xF0
#define     NEO_VirtualStripPos1_2Shift 4
#define NEO_VirtualStripPos1_3                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_3Mask 0xF0
#define     NEO_VirtualStripPos1_3Shift 4
#define NEO_VirtualStripPos1_4                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_4Mask 0xF0
#define     NEO_VirtualStripPos1_4Shift 4
#define NEO_VirtualStripPos1_5                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_5Mask 0xF0
#define     NEO_VirtualStripPos1_5Shift 4
#define NEO_VirtualStripPos1_6                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_6Mask 0xF0
#define     NEO_VirtualStripPos1_6Shift 4
#define NEO_VirtualStripPos1_7                  24052      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos1_7Mask 0xF0
#define     NEO_VirtualStripPos1_7Shift 4
#define NEO_VirtualStripPos2_2                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_2Mask 0x0F
#define     NEO_VirtualStripPos2_2Shift 0
#define NEO_VirtualStripPos2_3                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_3Mask 0x0F
#define     NEO_VirtualStripPos2_3Shift 0
#define NEO_VirtualStripPos2_4                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_4Mask 0x0F
#define     NEO_VirtualStripPos2_4Shift 0
#define NEO_VirtualStripPos2_5                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_5Mask 0x0F
#define     NEO_VirtualStripPos2_5Shift 0
#define NEO_VirtualStripPos2_6                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_6Mask 0x0F
#define     NEO_VirtualStripPos2_6Shift 0
#define NEO_VirtualStripPos2_7                  24052      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos2_7Mask 0x0F
#define     NEO_VirtualStripPos2_7Shift 0
#define NEO_VirtualStripPos3_3                  24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_3Mask 0xF0
#define     NEO_VirtualStripPos3_3Shift 4
#define NEO_VirtualStripPos3_4                  24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_4Mask 0xF0
#define     NEO_VirtualStripPos3_4Shift 4
#define NEO_VirtualStripPos3_5                  24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_5Mask 0xF0
#define     NEO_VirtualStripPos3_5Shift 4
#define NEO_VirtualStripPos3_6                  24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_6Mask 0xF0
#define     NEO_VirtualStripPos3_6Shift 4
#define NEO_VirtualStripPos3_7                  24053      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos3_7Mask 0xF0
#define     NEO_VirtualStripPos3_7Shift 4
#define NEO_VirtualStripPos4_4                  24053      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_4Mask 0x0F
#define     NEO_VirtualStripPos4_4Shift 0
#define NEO_VirtualStripPos4_5                  24053      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_5Mask 0x0F
#define     NEO_VirtualStripPos4_5Shift 0
#define NEO_VirtualStripPos4_6                  24053      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_6Mask 0x0F
#define     NEO_VirtualStripPos4_6Shift 0
#define NEO_VirtualStripPos4_7                  24053      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos4_7Mask 0x0F
#define     NEO_VirtualStripPos4_7Shift 0
#define NEO_VirtualStripPos5_5                  24054      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5_5Mask 0xF0
#define     NEO_VirtualStripPos5_5Shift 4
#define NEO_VirtualStripPos5_6                  24054      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5_6Mask 0xF0
#define     NEO_VirtualStripPos5_6Shift 4
#define NEO_VirtualStripPos5_7                  24054      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos5_7Mask 0xF0
#define     NEO_VirtualStripPos5_7Shift 4
#define NEO_VirtualStripPos6_6                  24054      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos6_6Mask 0x0F
#define     NEO_VirtualStripPos6_6Shift 0
#define NEO_VirtualStripPos6_7                  24054      // 4 Bits, Bit 3-0
#define     NEO_VirtualStripPos6_7Mask 0x0F
#define     NEO_VirtualStripPos6_7Shift 0
#define NEO_VirtualStripPos7_7                  24055      // 4 Bits, Bit 7-4
#define     NEO_VirtualStripPos7_7Mask 0xF0
#define     NEO_VirtualStripPos7_7Shift 4
#define NEO_VirtualStripHasDuplicates           24056      // 1 Bit, Bit 7
#define     NEO_VirtualStripHasDuplicatesMask 0x80
#define     NEO_VirtualStripHasDuplicatesShift 7
#define NEO_SegmentStartEndError                24056      // 1 Bit, Bit 6
#define     NEO_SegmentStartEndErrorMask 0x40
#define     NEO_SegmentStartEndErrorShift 6
#define NEO_VirtualStripStart1                  24057      // uint16_t
#define NEO_VirtualStripStart2                  24059      // uint16_t
#define NEO_VirtualStripStart3                  24061      // uint16_t
#define NEO_VirtualStripStart4                  24063      // uint16_t
#define NEO_VirtualStripStart5                  24065      // uint16_t
#define NEO_VirtualStripStart6                  24067      // uint16_t
#define NEO_VirtualStripStart7                  24069      // uint16_t
#define NEO_VirtualStripStart8                  24071      // uint16_t
#define NEO_VirtualStripEnd1                    24073      // uint16_t
#define NEO_VirtualStripEnd2                    24075      // uint16_t
#define NEO_VirtualStripEnd3                    24077      // uint16_t
#define NEO_VirtualStripEnd4                    24079      // uint16_t
#define NEO_VirtualStripEnd5                    24081      // uint16_t
#define NEO_VirtualStripEnd6                    24083      // uint16_t
#define NEO_VirtualStripEnd7                    24085      // uint16_t
#define NEO_VirtualStripEnd8                    24087      // uint16_t
#define NEO_Strip1DataPort                      24089      // uint8_t
#define NEO_Strip2DataPort                      24090      // uint8_t
#define NEO_Strip3DataPort                      24091      // uint8_t
#define NEO_Strip4DataPort                      24092      // uint8_t
#define NEO_Strip5DataPort                      24093      // uint8_t
#define NEO_Strip6DataPort                      24094      // uint8_t
#define NEO_Strip7DataPort                      24095      // uint8_t
#define NEO_Strip8DataPort                      24096      // uint8_t
#define NEO_Strip1HasDataConflict               24097      // 1 Bit, Bit 7
#define     NEO_Strip1HasDataConflictMask 0x80
#define     NEO_Strip1HasDataConflictShift 7
#define NEO_Strip2HasDataConflict               24097      // 1 Bit, Bit 6
#define     NEO_Strip2HasDataConflictMask 0x40
#define     NEO_Strip2HasDataConflictShift 6
#define NEO_Strip3HasDataConflict               24097      // 1 Bit, Bit 5
#define     NEO_Strip3HasDataConflictMask 0x20
#define     NEO_Strip3HasDataConflictShift 5
#define NEO_Strip4HasDataConflict               24097      // 1 Bit, Bit 4
#define     NEO_Strip4HasDataConflictMask 0x10
#define     NEO_Strip4HasDataConflictShift 4
#define NEO_Strip5HasDataConflict               24097      // 1 Bit, Bit 3
#define     NEO_Strip5HasDataConflictMask 0x08
#define     NEO_Strip5HasDataConflictShift 3
#define NEO_Strip6HasDataConflict               24097      // 1 Bit, Bit 2
#define     NEO_Strip6HasDataConflictMask 0x04
#define     NEO_Strip6HasDataConflictShift 2
#define NEO_Strip7HasDataConflict               24097      // 1 Bit, Bit 1
#define     NEO_Strip7HasDataConflictMask 0x02
#define     NEO_Strip7HasDataConflictShift 1
#define NEO_Strip8HasDataConflict               24097      // 1 Bit, Bit 0
#define     NEO_Strip8HasDataConflictMask 0x01
#define     NEO_Strip8HasDataConflictShift 0
#define NEO_Strip1HasClockConflict              24098      // 1 Bit, Bit 7
#define     NEO_Strip1HasClockConflictMask 0x80
#define     NEO_Strip1HasClockConflictShift 7
#define NEO_Strip2HasClockConflict              24098      // 1 Bit, Bit 6
#define     NEO_Strip2HasClockConflictMask 0x40
#define     NEO_Strip2HasClockConflictShift 6
#define NEO_Strip3HasClockConflict              24098      // 1 Bit, Bit 5
#define     NEO_Strip3HasClockConflictMask 0x20
#define     NEO_Strip3HasClockConflictShift 5
#define NEO_Strip4HasClockConflict              24098      // 1 Bit, Bit 4
#define     NEO_Strip4HasClockConflictMask 0x10
#define     NEO_Strip4HasClockConflictShift 4
#define NEO_Strip5HasClockConflict              24098      // 1 Bit, Bit 3
#define     NEO_Strip5HasClockConflictMask 0x08
#define     NEO_Strip5HasClockConflictShift 3
#define NEO_Strip6HasClockConflict              24098      // 1 Bit, Bit 2
#define     NEO_Strip6HasClockConflictMask 0x04
#define     NEO_Strip6HasClockConflictShift 2
#define NEO_Strip7HasClockConflict              24098      // 1 Bit, Bit 1
#define     NEO_Strip7HasClockConflictMask 0x02
#define     NEO_Strip7HasClockConflictShift 1
#define NEO_Strip8HasClockConflict              24098      // 1 Bit, Bit 0
#define     NEO_Strip8HasClockConflictMask 0x01
#define     NEO_Strip8HasClockConflictShift 0
#define NEO_Strip1ClockPort                     24099      // uint8_t
#define NEO_Strip2ClockPort                     24100      // uint8_t
#define NEO_Strip3ClockPort                     24101      // uint8_t
#define NEO_Strip4ClockPort                     24102      // uint8_t
#define NEO_Strip5ClockPort                     24103      // uint8_t
#define NEO_Strip6ClockPort                     24104      // uint8_t
#define NEO_Strip7ClockPort                     24105      // uint8_t
#define NEO_Strip8ClockPort                     24106      // uint8_t
#define NEO_NEOExternalRelay1Port               24107      // uint8_t
#define NEO_NEOExternalRelay2Port               24108      // uint8_t
#define NEO_NEOExternalRelayCount               24109      // 3 Bits, Bit 7-5
#define     NEO_NEOExternalRelayCountMask 0xE0
#define     NEO_NEOExternalRelayCountShift 5
#define NEO_NEOExternalRelay3Port               24110      // uint8_t
#define NEO_NEOExternalRelay4Port               24111      // uint8_t
#define NEO_NEORelay1GPIOPortHW0                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW1                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW2                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW3                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW4                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW5                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW6                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW7                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW8                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW9                24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW10               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW11               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW12               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW13               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW14               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW15               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW16               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW17               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW18               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW19               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW20               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW21               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW22               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW23               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW24               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW25               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW26               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW27               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW28               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW29               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW30               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay1GPIOPortHW31               24107      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW0                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW1                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW2                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW3                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW4                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW5                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW6                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW7                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW8                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW9                24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW10               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW11               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW12               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW13               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW14               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW15               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW16               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW17               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW18               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW19               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW20               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW21               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW22               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW23               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW24               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW25               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW26               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW27               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW28               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW29               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW30               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay2GPIOPortHW31               24108      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW0                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW1                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW2                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW3                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW4                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW5                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW6                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW7                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW8                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW9                24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW10               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW11               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW12               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW13               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW14               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW15               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW16               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW17               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW18               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW19               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW20               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW21               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW22               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW23               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW24               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW25               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW26               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW27               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW28               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW29               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW30               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay3GPIOPortHW31               24110      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW0                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW1                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW2                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW3                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW4                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW5                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW6                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW7                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW8                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW9                24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW10               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW11               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW12               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW13               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW14               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW15               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW16               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW17               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW18               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW19               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW20               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW21               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW22               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW23               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW24               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW25               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW26               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW27               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW28               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW29               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW30               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelay4GPIOPortHW31               24111      // 8 Bits, Bit 7-0
#define NEO_NEORelayNamesReserved               24147      // char*, 40 Byte
#define     NEO_NEORelayNamesReservedLength 40
#define NEO_Relay1HasConflict                   24112      // 1 Bit, Bit 7
#define     NEO_Relay1HasConflictMask 0x80
#define     NEO_Relay1HasConflictShift 7
#define NEO_Relay2HasConflict                   24112      // 1 Bit, Bit 6
#define     NEO_Relay2HasConflictMask 0x40
#define     NEO_Relay2HasConflictShift 6
#define NEO_Relay3HasConflict                   24112      // 1 Bit, Bit 5
#define     NEO_Relay3HasConflictMask 0x20
#define     NEO_Relay3HasConflictShift 5
#define NEO_Relay4HasConflict                   24112      // 1 Bit, Bit 4
#define     NEO_Relay4HasConflictMask 0x10
#define     NEO_Relay4HasConflictShift 4
#define NEO_NEOExternalRelay1OnDelay            24113      // uint16_t
#define NEO_NEOExternalRelay1OffDelay           24115      // uint16_t
#define NEO_NEOExternalRelay2OnDelay            24117      // uint16_t
#define NEO_NEOExternalRelay2OffDelay           24119      // uint16_t
#define NEO_NEOExternalRelay3OnDelay            24121      // uint16_t
#define NEO_NEOExternalRelay3OffDelay           24123      // uint16_t
#define NEO_NEOExternalRelay4OnDelay            24125      // uint16_t
#define NEO_NEOExternalRelay4OffDelay           24127      // uint16_t
#define NEO_NEOExternalRelay1MinOffTime         24129      // uint16_t
#define NEO_NEOExternalRelay2MinOffTime         24131      // uint16_t
#define NEO_NEOExternalRelay3MinOffTime         24133      // uint16_t
#define NEO_NEOExternalRelay4MinOffTime         24135      // uint16_t
#define NEO_NEOExternalRelay1GPIO               24137      // uint8_t
#define NEO_NEOExternalRelay2GPIO               24138      // uint8_t
#define NEO_NEOExternalRelay3GPIO               24139      // uint8_t
#define NEO_NEOExternalRelay4GPIO               24140      // uint8_t
#define NEO_NEOExternalRelay1OutputLogic        24141      // 1 Bit, Bit 7
#define     NEO_NEOExternalRelay1OutputLogicMask 0x80
#define     NEO_NEOExternalRelay1OutputLogicShift 7
#define NEO_NEOExternalRelay2OutputLogic        24141      // 1 Bit, Bit 6
#define     NEO_NEOExternalRelay2OutputLogicMask 0x40
#define     NEO_NEOExternalRelay2OutputLogicShift 6
#define NEO_NEOExternalRelay3OutputLogic        24141      // 1 Bit, Bit 5
#define     NEO_NEOExternalRelay3OutputLogicMask 0x20
#define     NEO_NEOExternalRelay3OutputLogicShift 5
#define NEO_NEOExternalRelay4OutputLogic        24141      // 1 Bit, Bit 4
#define     NEO_NEOExternalRelay4OutputLogicMask 0x10
#define     NEO_NEOExternalRelay4OutputLogicShift 4
#define NEO_PendingLedTypeResetStrip            24143      // uint8_t
#define NEO_PendingLedTypeResetLedType          24144      // uint8_t
#define NEO_PendingLedTypeResetTick             24145      // uint8_t

// Lichtmanager
#define ParamNEO_HCLMaster                           (knx.paramByte(NEO_HCLMaster))
// Anwendung
#define ParamNEO_HCLApplyMode                        ((knx.paramByte(NEO_HCLApplyMode) & NEO_HCLApplyModeMask) >> NEO_HCLApplyModeShift)
// Hardware Auswahl
#define ParamNEO_NeoPixelHardwareSelect              (knx.paramWord(NEO_NeoPixelHardwareSelect))
// Anzahl der LED-Streifen
#define ParamNEO_NEONumberOfLEDStrips                ((knx.paramByte(NEO_NEONumberOfLEDStrips) & NEO_NEONumberOfLEDStripsMask) >> NEO_NEONumberOfLEDStripsShift)
// Anzahl der Segmente
#define ParamNEO_NEONumberOfSegments                 ((knx.paramByte(NEO_NEONumberOfSegments) & NEO_NEONumberOfSegmentsMask) >> NEO_NEONumberOfSegmentsShift)
// LEDs vor Upload/Neustart ausschalten
#define ParamNEO_NEOTurnOffBeforeRestart             ((bool)(knx.paramByte(NEO_NEOTurnOffBeforeRestart) & NEO_NEOTurnOffBeforeRestartMask))
// Aktualisierungsrate
#define ParamNEO_NEOUpdateSpeed                      (knx.paramByte(NEO_NEOUpdateSpeed))
// Globale Strombegrenzung (mA)
#define ParamNEO_NEOpowerLimitGlobal                 (knx.paramWord(NEO_NEOpowerLimitGlobal))
// Strom pro LED (mA)
#define ParamNEO_NEOcurrentPerLED                    ((knx.paramByte(NEO_NEOcurrentPerLED) & NEO_NEOcurrentPerLEDMask) >> NEO_NEOcurrentPerLEDShift)
// Strom pro Kanal (mA)
#define ParamNEO_NEOcurrentPerChannel                (knx.paramWord(NEO_NEOcurrentPerChannel))
// Strombegrenzung
#define ParamNEO_NEOpowerLimitCombined               (PT_NEOPowerLimitCombined)((knx.paramByte(NEO_NEOpowerLimitCombined) & NEO_NEOpowerLimitCombinedMask) >> NEO_NEOpowerLimitCombinedShift)
// Auto-Helligkeitsbegrenzung (%)
#define ParamNEO_NEOautoBrightnessLimit              ((knx.paramByte(NEO_NEOautoBrightnessLimit) & NEO_NEOautoBrightnessLimitMask) >> NEO_NEOautoBrightnessLimitShift)
// Strombegrenzung Schwellwert (%)
#define ParamNEO_NEOpowerLimitThreshold              ((knx.paramByte(NEO_NEOpowerLimitThreshold) & NEO_NEOpowerLimitThresholdMask) >> NEO_NEOpowerLimitThresholdShift)
// Anstiegszeit ABL (%)
#define ParamNEO_NEOablSlewRatePercent               ((knx.paramByte(NEO_NEOablSlewRatePercent) & NEO_NEOablSlewRatePercentMask) >> NEO_NEOablSlewRatePercentShift)
// Verfügbare Effektmanager
#define ParamNEO_NEOEMEnabled                        (knx.paramByte(NEO_NEOEMEnabled))
// Verhalten nach Neustart (Global)
#define ParamNEO_NEOGlobalStartupBehavior            ((knx.paramByte(NEO_NEOGlobalStartupBehavior) & NEO_NEOGlobalStartupBehaviorMask) >> NEO_NEOGlobalStartupBehaviorShift)
// Start-Farbe R
#define ParamNEO_NEOGlobalStartupR                   (knx.paramByte(NEO_NEOGlobalStartupR))
// Start-Farbe G
#define ParamNEO_NEOGlobalStartupG                   (knx.paramByte(NEO_NEOGlobalStartupG))
// Start-Farbe B
#define ParamNEO_NEOGlobalStartupB                   (knx.paramByte(NEO_NEOGlobalStartupB))
// Start-Farbe W
#define ParamNEO_NEOGlobalStartupW                   (knx.paramByte(NEO_NEOGlobalStartupW))
// Start-Helligkeit
#define ParamNEO_NEOGlobalStartupBrightness          (knx.paramByte(NEO_NEOGlobalStartupBrightness))
// Start-Effekt
#define ParamNEO_NEOGlobalStartupEffect              (PT_NEOEffectType)(knx.paramByte(NEO_NEOGlobalStartupEffect))
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
// Position 7
#define ParamNEO_VirtualStripPos7                    ((knx.paramByte(NEO_VirtualStripPos7) & NEO_VirtualStripPos7Mask) >> NEO_VirtualStripPos7Shift)
// Position 8
#define ParamNEO_VirtualStripPos8                    (knx.paramByte(NEO_VirtualStripPos8) & NEO_VirtualStripPos8Mask)
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
// Position 1
#define ParamNEO_VirtualStripPos1_6                  ((knx.paramByte(NEO_VirtualStripPos1_6) & NEO_VirtualStripPos1_6Mask) >> NEO_VirtualStripPos1_6Shift)
// Position 1
#define ParamNEO_VirtualStripPos1_7                  ((knx.paramByte(NEO_VirtualStripPos1_7) & NEO_VirtualStripPos1_7Mask) >> NEO_VirtualStripPos1_7Shift)
// Position 2
#define ParamNEO_VirtualStripPos2_2                  (knx.paramByte(NEO_VirtualStripPos2_2) & NEO_VirtualStripPos2_2Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_3                  (knx.paramByte(NEO_VirtualStripPos2_3) & NEO_VirtualStripPos2_3Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_4                  (knx.paramByte(NEO_VirtualStripPos2_4) & NEO_VirtualStripPos2_4Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_5                  (knx.paramByte(NEO_VirtualStripPos2_5) & NEO_VirtualStripPos2_5Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_6                  (knx.paramByte(NEO_VirtualStripPos2_6) & NEO_VirtualStripPos2_6Mask)
// Position 2
#define ParamNEO_VirtualStripPos2_7                  (knx.paramByte(NEO_VirtualStripPos2_7) & NEO_VirtualStripPos2_7Mask)
// Position 3
#define ParamNEO_VirtualStripPos3_3                  ((knx.paramByte(NEO_VirtualStripPos3_3) & NEO_VirtualStripPos3_3Mask) >> NEO_VirtualStripPos3_3Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_4                  ((knx.paramByte(NEO_VirtualStripPos3_4) & NEO_VirtualStripPos3_4Mask) >> NEO_VirtualStripPos3_4Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_5                  ((knx.paramByte(NEO_VirtualStripPos3_5) & NEO_VirtualStripPos3_5Mask) >> NEO_VirtualStripPos3_5Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_6                  ((knx.paramByte(NEO_VirtualStripPos3_6) & NEO_VirtualStripPos3_6Mask) >> NEO_VirtualStripPos3_6Shift)
// Position 3
#define ParamNEO_VirtualStripPos3_7                  ((knx.paramByte(NEO_VirtualStripPos3_7) & NEO_VirtualStripPos3_7Mask) >> NEO_VirtualStripPos3_7Shift)
// Position 4
#define ParamNEO_VirtualStripPos4_4                  (knx.paramByte(NEO_VirtualStripPos4_4) & NEO_VirtualStripPos4_4Mask)
// Position 4
#define ParamNEO_VirtualStripPos4_5                  (knx.paramByte(NEO_VirtualStripPos4_5) & NEO_VirtualStripPos4_5Mask)
// Position 4
#define ParamNEO_VirtualStripPos4_6                  (knx.paramByte(NEO_VirtualStripPos4_6) & NEO_VirtualStripPos4_6Mask)
// Position 4
#define ParamNEO_VirtualStripPos4_7                  (knx.paramByte(NEO_VirtualStripPos4_7) & NEO_VirtualStripPos4_7Mask)
// Position 5
#define ParamNEO_VirtualStripPos5_5                  ((knx.paramByte(NEO_VirtualStripPos5_5) & NEO_VirtualStripPos5_5Mask) >> NEO_VirtualStripPos5_5Shift)
// Position 5
#define ParamNEO_VirtualStripPos5_6                  ((knx.paramByte(NEO_VirtualStripPos5_6) & NEO_VirtualStripPos5_6Mask) >> NEO_VirtualStripPos5_6Shift)
// Position 5
#define ParamNEO_VirtualStripPos5_7                  ((knx.paramByte(NEO_VirtualStripPos5_7) & NEO_VirtualStripPos5_7Mask) >> NEO_VirtualStripPos5_7Shift)
// Position 6
#define ParamNEO_VirtualStripPos6_6                  (knx.paramByte(NEO_VirtualStripPos6_6) & NEO_VirtualStripPos6_6Mask)
// Position 6
#define ParamNEO_VirtualStripPos6_7                  (knx.paramByte(NEO_VirtualStripPos6_7) & NEO_VirtualStripPos6_7Mask)
// Position 7
#define ParamNEO_VirtualStripPos7_7                  ((knx.paramByte(NEO_VirtualStripPos7_7) & NEO_VirtualStripPos7_7Mask) >> NEO_VirtualStripPos7_7Shift)
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
// Start LED Position 7
#define ParamNEO_VirtualStripStart7                  (knx.paramWord(NEO_VirtualStripStart7))
// Start LED Position 8
#define ParamNEO_VirtualStripStart8                  (knx.paramWord(NEO_VirtualStripStart8))
// End LED Position 1
#define ParamNEO_VirtualStripEnd1                    (knx.paramWord(NEO_VirtualStripEnd1))
// End LED Position 2
#define ParamNEO_VirtualStripEnd2                    (knx.paramWord(NEO_VirtualStripEnd2))
// End LED Position 3
#define ParamNEO_VirtualStripEnd3                    (knx.paramWord(NEO_VirtualStripEnd3))
// End LED Position 4
#define ParamNEO_VirtualStripEnd4                    (knx.paramWord(NEO_VirtualStripEnd4))
// End LED Position 5
#define ParamNEO_VirtualStripEnd5                    (knx.paramWord(NEO_VirtualStripEnd5))
// End LED Position 6
#define ParamNEO_VirtualStripEnd6                    (knx.paramWord(NEO_VirtualStripEnd6))
// End LED Position 7
#define ParamNEO_VirtualStripEnd7                    (knx.paramWord(NEO_VirtualStripEnd7))
// End LED Position 8
#define ParamNEO_VirtualStripEnd8                    (knx.paramWord(NEO_VirtualStripEnd8))
// Strip 1 GPIO Port
#define ParamNEO_Strip1DataPort                      (knx.paramByte(NEO_Strip1DataPort))
// Strip 2 GPIO Port
#define ParamNEO_Strip2DataPort                      (knx.paramByte(NEO_Strip2DataPort))
// Strip 3 GPIO Port
#define ParamNEO_Strip3DataPort                      (knx.paramByte(NEO_Strip3DataPort))
// Strip 4 GPIO Port
#define ParamNEO_Strip4DataPort                      (knx.paramByte(NEO_Strip4DataPort))
// Strip 5 GPIO Port
#define ParamNEO_Strip5DataPort                      (knx.paramByte(NEO_Strip5DataPort))
// Strip 6 GPIO Port
#define ParamNEO_Strip6DataPort                      (knx.paramByte(NEO_Strip6DataPort))
// Strip 7 GPIO Port
#define ParamNEO_Strip7DataPort                      (knx.paramByte(NEO_Strip7DataPort))
// Strip 8 GPIO Port
#define ParamNEO_Strip8DataPort                      (knx.paramByte(NEO_Strip8DataPort))
// Strip 1 Conflict
#define ParamNEO_Strip1HasDataConflict               ((bool)(knx.paramByte(NEO_Strip1HasDataConflict) & NEO_Strip1HasDataConflictMask))
// Strip 2 Conflict
#define ParamNEO_Strip2HasDataConflict               ((bool)(knx.paramByte(NEO_Strip2HasDataConflict) & NEO_Strip2HasDataConflictMask))
// Strip 3 Conflict
#define ParamNEO_Strip3HasDataConflict               ((bool)(knx.paramByte(NEO_Strip3HasDataConflict) & NEO_Strip3HasDataConflictMask))
// Strip 4 Conflict
#define ParamNEO_Strip4HasDataConflict               ((bool)(knx.paramByte(NEO_Strip4HasDataConflict) & NEO_Strip4HasDataConflictMask))
// Strip 5 Conflict
#define ParamNEO_Strip5HasDataConflict               ((bool)(knx.paramByte(NEO_Strip5HasDataConflict) & NEO_Strip5HasDataConflictMask))
// Strip 6 Conflict
#define ParamNEO_Strip6HasDataConflict               ((bool)(knx.paramByte(NEO_Strip6HasDataConflict) & NEO_Strip6HasDataConflictMask))
// Strip 7 Conflict
#define ParamNEO_Strip7HasDataConflict               ((bool)(knx.paramByte(NEO_Strip7HasDataConflict) & NEO_Strip7HasDataConflictMask))
// Strip 8 Conflict
#define ParamNEO_Strip8HasDataConflict               ((bool)(knx.paramByte(NEO_Strip8HasDataConflict) & NEO_Strip8HasDataConflictMask))
// Strip 1 Clock Conflict
#define ParamNEO_Strip1HasClockConflict              ((bool)(knx.paramByte(NEO_Strip1HasClockConflict) & NEO_Strip1HasClockConflictMask))
// Strip 2 Clock Conflict
#define ParamNEO_Strip2HasClockConflict              ((bool)(knx.paramByte(NEO_Strip2HasClockConflict) & NEO_Strip2HasClockConflictMask))
// Strip 3 Clock Conflict
#define ParamNEO_Strip3HasClockConflict              ((bool)(knx.paramByte(NEO_Strip3HasClockConflict) & NEO_Strip3HasClockConflictMask))
// Strip 4 Clock Conflict
#define ParamNEO_Strip4HasClockConflict              ((bool)(knx.paramByte(NEO_Strip4HasClockConflict) & NEO_Strip4HasClockConflictMask))
// Strip 5 Clock Conflict
#define ParamNEO_Strip5HasClockConflict              ((bool)(knx.paramByte(NEO_Strip5HasClockConflict) & NEO_Strip5HasClockConflictMask))
// Strip 6 Clock Conflict
#define ParamNEO_Strip6HasClockConflict              ((bool)(knx.paramByte(NEO_Strip6HasClockConflict) & NEO_Strip6HasClockConflictMask))
// Strip 7 Clock Conflict
#define ParamNEO_Strip7HasClockConflict              ((bool)(knx.paramByte(NEO_Strip7HasClockConflict) & NEO_Strip7HasClockConflictMask))
// Strip 8 Clock Conflict
#define ParamNEO_Strip8HasClockConflict              ((bool)(knx.paramByte(NEO_Strip8HasClockConflict) & NEO_Strip8HasClockConflictMask))
// Strip 1 Clock GPIO Port
#define ParamNEO_Strip1ClockPort                     (knx.paramByte(NEO_Strip1ClockPort))
// Strip 2 Clock GPIO Port
#define ParamNEO_Strip2ClockPort                     (knx.paramByte(NEO_Strip2ClockPort))
// Strip 3 Clock GPIO Port
#define ParamNEO_Strip3ClockPort                     (knx.paramByte(NEO_Strip3ClockPort))
// Strip 4 Clock GPIO Port
#define ParamNEO_Strip4ClockPort                     (knx.paramByte(NEO_Strip4ClockPort))
// Strip 5 Clock GPIO Port
#define ParamNEO_Strip5ClockPort                     (knx.paramByte(NEO_Strip5ClockPort))
// Strip 6 Clock GPIO Port
#define ParamNEO_Strip6ClockPort                     (knx.paramByte(NEO_Strip6ClockPort))
// Strip 7 Clock GPIO Port
#define ParamNEO_Strip7ClockPort                     (knx.paramByte(NEO_Strip7ClockPort))
// Strip 8 Clock GPIO Port
#define ParamNEO_Strip8ClockPort                     (knx.paramByte(NEO_Strip8ClockPort))
// Relais 1 Port
#define ParamNEO_NEOExternalRelay1Port               (knx.paramByte(NEO_NEOExternalRelay1Port))
// Relais 2 Port
#define ParamNEO_NEOExternalRelay2Port               (knx.paramByte(NEO_NEOExternalRelay2Port))
// Externe Relais
#define ParamNEO_NEOExternalRelayCount               ((knx.paramByte(NEO_NEOExternalRelayCount) & NEO_NEOExternalRelayCountMask) >> NEO_NEOExternalRelayCountShift)
// Relais 3 Port
#define ParamNEO_NEOExternalRelay3Port               (knx.paramByte(NEO_NEOExternalRelay3Port))
// Relais 4 Port
#define ParamNEO_NEOExternalRelay4Port               (knx.paramByte(NEO_NEOExternalRelay4Port))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW0                (knx.paramByte(NEO_NEORelay1GPIOPortHW0))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW1                (knx.paramByte(NEO_NEORelay1GPIOPortHW1))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW2                (knx.paramByte(NEO_NEORelay1GPIOPortHW2))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW3                (knx.paramByte(NEO_NEORelay1GPIOPortHW3))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW4                (knx.paramByte(NEO_NEORelay1GPIOPortHW4))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW5                (knx.paramByte(NEO_NEORelay1GPIOPortHW5))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW6                (knx.paramByte(NEO_NEORelay1GPIOPortHW6))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW7                (knx.paramByte(NEO_NEORelay1GPIOPortHW7))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW8                (knx.paramByte(NEO_NEORelay1GPIOPortHW8))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW9                (knx.paramByte(NEO_NEORelay1GPIOPortHW9))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW10               (knx.paramByte(NEO_NEORelay1GPIOPortHW10))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW11               (knx.paramByte(NEO_NEORelay1GPIOPortHW11))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW12               (knx.paramByte(NEO_NEORelay1GPIOPortHW12))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW13               (knx.paramByte(NEO_NEORelay1GPIOPortHW13))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW14               (knx.paramByte(NEO_NEORelay1GPIOPortHW14))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW15               (knx.paramByte(NEO_NEORelay1GPIOPortHW15))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW16               (knx.paramByte(NEO_NEORelay1GPIOPortHW16))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW17               (knx.paramByte(NEO_NEORelay1GPIOPortHW17))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW18               (knx.paramByte(NEO_NEORelay1GPIOPortHW18))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW19               (knx.paramByte(NEO_NEORelay1GPIOPortHW19))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW20               (knx.paramByte(NEO_NEORelay1GPIOPortHW20))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW21               (knx.paramByte(NEO_NEORelay1GPIOPortHW21))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW22               (knx.paramByte(NEO_NEORelay1GPIOPortHW22))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW23               (knx.paramByte(NEO_NEORelay1GPIOPortHW23))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW24               (knx.paramByte(NEO_NEORelay1GPIOPortHW24))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW25               (knx.paramByte(NEO_NEORelay1GPIOPortHW25))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW26               (knx.paramByte(NEO_NEORelay1GPIOPortHW26))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW27               (knx.paramByte(NEO_NEORelay1GPIOPortHW27))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW28               (knx.paramByte(NEO_NEORelay1GPIOPortHW28))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW29               (knx.paramByte(NEO_NEORelay1GPIOPortHW29))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW30               (knx.paramByte(NEO_NEORelay1GPIOPortHW30))
// Relais 1 Port
#define ParamNEO_NEORelay1GPIOPortHW31               (knx.paramByte(NEO_NEORelay1GPIOPortHW31))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW0                (knx.paramByte(NEO_NEORelay2GPIOPortHW0))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW1                (knx.paramByte(NEO_NEORelay2GPIOPortHW1))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW2                (knx.paramByte(NEO_NEORelay2GPIOPortHW2))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW3                (knx.paramByte(NEO_NEORelay2GPIOPortHW3))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW4                (knx.paramByte(NEO_NEORelay2GPIOPortHW4))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW5                (knx.paramByte(NEO_NEORelay2GPIOPortHW5))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW6                (knx.paramByte(NEO_NEORelay2GPIOPortHW6))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW7                (knx.paramByte(NEO_NEORelay2GPIOPortHW7))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW8                (knx.paramByte(NEO_NEORelay2GPIOPortHW8))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW9                (knx.paramByte(NEO_NEORelay2GPIOPortHW9))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW10               (knx.paramByte(NEO_NEORelay2GPIOPortHW10))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW11               (knx.paramByte(NEO_NEORelay2GPIOPortHW11))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW12               (knx.paramByte(NEO_NEORelay2GPIOPortHW12))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW13               (knx.paramByte(NEO_NEORelay2GPIOPortHW13))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW14               (knx.paramByte(NEO_NEORelay2GPIOPortHW14))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW15               (knx.paramByte(NEO_NEORelay2GPIOPortHW15))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW16               (knx.paramByte(NEO_NEORelay2GPIOPortHW16))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW17               (knx.paramByte(NEO_NEORelay2GPIOPortHW17))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW18               (knx.paramByte(NEO_NEORelay2GPIOPortHW18))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW19               (knx.paramByte(NEO_NEORelay2GPIOPortHW19))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW20               (knx.paramByte(NEO_NEORelay2GPIOPortHW20))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW21               (knx.paramByte(NEO_NEORelay2GPIOPortHW21))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW22               (knx.paramByte(NEO_NEORelay2GPIOPortHW22))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW23               (knx.paramByte(NEO_NEORelay2GPIOPortHW23))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW24               (knx.paramByte(NEO_NEORelay2GPIOPortHW24))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW25               (knx.paramByte(NEO_NEORelay2GPIOPortHW25))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW26               (knx.paramByte(NEO_NEORelay2GPIOPortHW26))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW27               (knx.paramByte(NEO_NEORelay2GPIOPortHW27))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW28               (knx.paramByte(NEO_NEORelay2GPIOPortHW28))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW29               (knx.paramByte(NEO_NEORelay2GPIOPortHW29))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW30               (knx.paramByte(NEO_NEORelay2GPIOPortHW30))
// Relais 2 Port
#define ParamNEO_NEORelay2GPIOPortHW31               (knx.paramByte(NEO_NEORelay2GPIOPortHW31))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW0                (knx.paramByte(NEO_NEORelay3GPIOPortHW0))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW1                (knx.paramByte(NEO_NEORelay3GPIOPortHW1))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW2                (knx.paramByte(NEO_NEORelay3GPIOPortHW2))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW3                (knx.paramByte(NEO_NEORelay3GPIOPortHW3))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW4                (knx.paramByte(NEO_NEORelay3GPIOPortHW4))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW5                (knx.paramByte(NEO_NEORelay3GPIOPortHW5))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW6                (knx.paramByte(NEO_NEORelay3GPIOPortHW6))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW7                (knx.paramByte(NEO_NEORelay3GPIOPortHW7))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW8                (knx.paramByte(NEO_NEORelay3GPIOPortHW8))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW9                (knx.paramByte(NEO_NEORelay3GPIOPortHW9))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW10               (knx.paramByte(NEO_NEORelay3GPIOPortHW10))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW11               (knx.paramByte(NEO_NEORelay3GPIOPortHW11))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW12               (knx.paramByte(NEO_NEORelay3GPIOPortHW12))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW13               (knx.paramByte(NEO_NEORelay3GPIOPortHW13))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW14               (knx.paramByte(NEO_NEORelay3GPIOPortHW14))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW15               (knx.paramByte(NEO_NEORelay3GPIOPortHW15))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW16               (knx.paramByte(NEO_NEORelay3GPIOPortHW16))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW17               (knx.paramByte(NEO_NEORelay3GPIOPortHW17))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW18               (knx.paramByte(NEO_NEORelay3GPIOPortHW18))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW19               (knx.paramByte(NEO_NEORelay3GPIOPortHW19))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW20               (knx.paramByte(NEO_NEORelay3GPIOPortHW20))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW21               (knx.paramByte(NEO_NEORelay3GPIOPortHW21))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW22               (knx.paramByte(NEO_NEORelay3GPIOPortHW22))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW23               (knx.paramByte(NEO_NEORelay3GPIOPortHW23))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW24               (knx.paramByte(NEO_NEORelay3GPIOPortHW24))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW25               (knx.paramByte(NEO_NEORelay3GPIOPortHW25))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW26               (knx.paramByte(NEO_NEORelay3GPIOPortHW26))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW27               (knx.paramByte(NEO_NEORelay3GPIOPortHW27))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW28               (knx.paramByte(NEO_NEORelay3GPIOPortHW28))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW29               (knx.paramByte(NEO_NEORelay3GPIOPortHW29))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW30               (knx.paramByte(NEO_NEORelay3GPIOPortHW30))
// Relais 3 Port
#define ParamNEO_NEORelay3GPIOPortHW31               (knx.paramByte(NEO_NEORelay3GPIOPortHW31))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW0                (knx.paramByte(NEO_NEORelay4GPIOPortHW0))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW1                (knx.paramByte(NEO_NEORelay4GPIOPortHW1))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW2                (knx.paramByte(NEO_NEORelay4GPIOPortHW2))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW3                (knx.paramByte(NEO_NEORelay4GPIOPortHW3))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW4                (knx.paramByte(NEO_NEORelay4GPIOPortHW4))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW5                (knx.paramByte(NEO_NEORelay4GPIOPortHW5))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW6                (knx.paramByte(NEO_NEORelay4GPIOPortHW6))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW7                (knx.paramByte(NEO_NEORelay4GPIOPortHW7))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW8                (knx.paramByte(NEO_NEORelay4GPIOPortHW8))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW9                (knx.paramByte(NEO_NEORelay4GPIOPortHW9))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW10               (knx.paramByte(NEO_NEORelay4GPIOPortHW10))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW11               (knx.paramByte(NEO_NEORelay4GPIOPortHW11))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW12               (knx.paramByte(NEO_NEORelay4GPIOPortHW12))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW13               (knx.paramByte(NEO_NEORelay4GPIOPortHW13))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW14               (knx.paramByte(NEO_NEORelay4GPIOPortHW14))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW15               (knx.paramByte(NEO_NEORelay4GPIOPortHW15))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW16               (knx.paramByte(NEO_NEORelay4GPIOPortHW16))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW17               (knx.paramByte(NEO_NEORelay4GPIOPortHW17))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW18               (knx.paramByte(NEO_NEORelay4GPIOPortHW18))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW19               (knx.paramByte(NEO_NEORelay4GPIOPortHW19))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW20               (knx.paramByte(NEO_NEORelay4GPIOPortHW20))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW21               (knx.paramByte(NEO_NEORelay4GPIOPortHW21))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW22               (knx.paramByte(NEO_NEORelay4GPIOPortHW22))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW23               (knx.paramByte(NEO_NEORelay4GPIOPortHW23))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW24               (knx.paramByte(NEO_NEORelay4GPIOPortHW24))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW25               (knx.paramByte(NEO_NEORelay4GPIOPortHW25))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW26               (knx.paramByte(NEO_NEORelay4GPIOPortHW26))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW27               (knx.paramByte(NEO_NEORelay4GPIOPortHW27))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW28               (knx.paramByte(NEO_NEORelay4GPIOPortHW28))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW29               (knx.paramByte(NEO_NEORelay4GPIOPortHW29))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW30               (knx.paramByte(NEO_NEORelay4GPIOPortHW30))
// Relais 4 Port
#define ParamNEO_NEORelay4GPIOPortHW31               (knx.paramByte(NEO_NEORelay4GPIOPortHW31))
// reserved
#define ParamNEO_NEORelayNamesReserved               (knx.paramData(NEO_NEORelayNamesReserved))
#define ParamNEO_NEORelayNamesReservedStr            (knx.paramString(NEO_NEORelayNamesReserved, NEO_NEORelayNamesReservedLength))
// Relais 1 Conflict
#define ParamNEO_Relay1HasConflict                   ((bool)(knx.paramByte(NEO_Relay1HasConflict) & NEO_Relay1HasConflictMask))
// Relais 2 Conflict
#define ParamNEO_Relay2HasConflict                   ((bool)(knx.paramByte(NEO_Relay2HasConflict) & NEO_Relay2HasConflictMask))
// Relais 3 Conflict
#define ParamNEO_Relay3HasConflict                   ((bool)(knx.paramByte(NEO_Relay3HasConflict) & NEO_Relay3HasConflictMask))
// Relais 4 Conflict
#define ParamNEO_Relay4HasConflict                   ((bool)(knx.paramByte(NEO_Relay4HasConflict) & NEO_Relay4HasConflictMask))
// Einschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay1OnDelay            (knx.paramWord(NEO_NEOExternalRelay1OnDelay))
// Ausschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay1OffDelay           (knx.paramWord(NEO_NEOExternalRelay1OffDelay))
// Einschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay2OnDelay            (knx.paramWord(NEO_NEOExternalRelay2OnDelay))
// Ausschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay2OffDelay           (knx.paramWord(NEO_NEOExternalRelay2OffDelay))
// Einschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay3OnDelay            (knx.paramWord(NEO_NEOExternalRelay3OnDelay))
// Ausschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay3OffDelay           (knx.paramWord(NEO_NEOExternalRelay3OffDelay))
// Einschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay4OnDelay            (knx.paramWord(NEO_NEOExternalRelay4OnDelay))
// Ausschaltverzögerung (s)
#define ParamNEO_NEOExternalRelay4OffDelay           (knx.paramWord(NEO_NEOExternalRelay4OffDelay))
// Minimale Aus-Zeit (s)
#define ParamNEO_NEOExternalRelay1MinOffTime         (knx.paramWord(NEO_NEOExternalRelay1MinOffTime))
// Minimale Aus-Zeit (s)
#define ParamNEO_NEOExternalRelay2MinOffTime         (knx.paramWord(NEO_NEOExternalRelay2MinOffTime))
// Minimale Aus-Zeit (s)
#define ParamNEO_NEOExternalRelay3MinOffTime         (knx.paramWord(NEO_NEOExternalRelay3MinOffTime))
// Minimale Aus-Zeit (s)
#define ParamNEO_NEOExternalRelay4MinOffTime         (knx.paramWord(NEO_NEOExternalRelay4MinOffTime))
// Relais 1 GPIO
#define ParamNEO_NEOExternalRelay1GPIO               (knx.paramByte(NEO_NEOExternalRelay1GPIO))
// Relais 2 GPIO
#define ParamNEO_NEOExternalRelay2GPIO               (knx.paramByte(NEO_NEOExternalRelay2GPIO))
// Relais 3 GPIO
#define ParamNEO_NEOExternalRelay3GPIO               (knx.paramByte(NEO_NEOExternalRelay3GPIO))
// Relais 4 GPIO
#define ParamNEO_NEOExternalRelay4GPIO               (knx.paramByte(NEO_NEOExternalRelay4GPIO))
// Ausgangslogik
#define ParamNEO_NEOExternalRelay1OutputLogic        ((bool)(knx.paramByte(NEO_NEOExternalRelay1OutputLogic) & NEO_NEOExternalRelay1OutputLogicMask))
// Ausgangslogik
#define ParamNEO_NEOExternalRelay2OutputLogic        ((bool)(knx.paramByte(NEO_NEOExternalRelay2OutputLogic) & NEO_NEOExternalRelay2OutputLogicMask))
// Ausgangslogik
#define ParamNEO_NEOExternalRelay3OutputLogic        ((bool)(knx.paramByte(NEO_NEOExternalRelay3OutputLogic) & NEO_NEOExternalRelay3OutputLogicMask))
// Ausgangslogik
#define ParamNEO_NEOExternalRelay4OutputLogic        ((bool)(knx.paramByte(NEO_NEOExternalRelay4OutputLogic) & NEO_NEOExternalRelay4OutputLogicMask))
// Pending LED Type Reset Strip
#define ParamNEO_PendingLedTypeResetStrip            (knx.paramByte(NEO_PendingLedTypeResetStrip))
// Pending LED Type Reset LED Type
#define ParamNEO_PendingLedTypeResetLedType          (knx.paramByte(NEO_PendingLedTypeResetLedType))
// Pending LED Type Reset Tick
#define ParamNEO_PendingLedTypeResetTick             (knx.paramByte(NEO_PendingLedTypeResetTick))

#define NEO_KoPower 400
#define NEO_KoPowerState 401
#define NEO_KoBrightness 402
#define NEO_KoBrightnessState 403
#define NEO_KoPowerCurrentTotal 404
#define NEO_KoPowerLoadTotal 405
#define NEO_KoPowerWattsTotal 406
#define NEO_KoHCLGlobalState 407
#define NEO_KoExternalRelay1 408
#define NEO_KoExternalRelay2 409
#define NEO_KoExternalRelay3 410
#define NEO_KoExternalRelay4 411
#define NEO_KoExternalRelay1State 412
#define NEO_KoExternalRelay2State 413
#define NEO_KoExternalRelay3State 414
#define NEO_KoExternalRelay4State 415

// Alle LEDs
#define KoNEO_Power                               (knx.getGroupObject(NEO_KoPower))
// Alle LEDs Status
#define KoNEO_PowerState                          (knx.getGroupObject(NEO_KoPowerState))
// Helligkeit
#define KoNEO_Brightness                          (knx.getGroupObject(NEO_KoBrightness))
// Helligkeit Status
#define KoNEO_BrightnessState                     (knx.getGroupObject(NEO_KoBrightnessState))
// Stromverbrauch Gesamt
#define KoNEO_PowerCurrentTotal                   (knx.getGroupObject(NEO_KoPowerCurrentTotal))
// Auslastung Gesamt
#define KoNEO_PowerLoadTotal                      (knx.getGroupObject(NEO_KoPowerLoadTotal))
// Leistung Gesamt
#define KoNEO_PowerWattsTotal                     (knx.getGroupObject(NEO_KoPowerWattsTotal))
// HCL Global Zustand
#define KoNEO_HCLGlobalState                      (knx.getGroupObject(NEO_KoHCLGlobalState))
// Relais 1
#define KoNEO_ExternalRelay1                      (knx.getGroupObject(NEO_KoExternalRelay1))
// Relais 2
#define KoNEO_ExternalRelay2                      (knx.getGroupObject(NEO_KoExternalRelay2))
// Relais 3
#define KoNEO_ExternalRelay3                      (knx.getGroupObject(NEO_KoExternalRelay3))
// Relais 4
#define KoNEO_ExternalRelay4                      (knx.getGroupObject(NEO_KoExternalRelay4))
// Relais 1 Status
#define KoNEO_ExternalRelay1State                 (knx.getGroupObject(NEO_KoExternalRelay1State))
// Relais 2 Status
#define KoNEO_ExternalRelay2State                 (knx.getGroupObject(NEO_KoExternalRelay2State))
// Relais 3 Status
#define KoNEO_ExternalRelay3State                 (knx.getGroupObject(NEO_KoExternalRelay3State))
// Relais 4 Status
#define KoNEO_ExternalRelay4State                 (knx.getGroupObject(NEO_KoExternalRelay4State))

#define NEO_ChannelCount 16

// Parameter per channel
#define NEO_ParamBlockOffset 24203
#define NEO_ParamBlockSize 704
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
#define NEO_NEOSegmentTopology                  10      // 3 Bits, Bit 2-0
#define     NEO_NEOSegmentTopologyMask 0x07
#define     NEO_NEOSegmentTopologyShift 0
#define NEO_NEONEOEffectType                    11      // 8 Bits, Bit 7-0
#define NEO_NEOMatrixWidth                      12      // uint8_t
#define NEO_NEOMatrixHeight                     13      // uint8_t
#define NEO_NEOMatrixDepth                      14      // uint8_t
#define NEO_NEOSyncMode                         15      // 2 Bits, Bit 7-6
#define     NEO_NEOSyncModeMask 0xC0
#define     NEO_NEOSyncModeShift 6
#define NEO_NEOSyncOverridePolicy               15      // 1 Bit, Bit 5
#define     NEO_NEOSyncOverridePolicyMask 0x20
#define     NEO_NEOSyncOverridePolicyShift 5
#define NEO_NEOSyncTimeout                      15      // 5 Bits, Bit 4-0
#define     NEO_NEOSyncTimeoutMask 0x1F
#define     NEO_NEOSyncTimeoutShift 0
#define NEO_NEOVirtualTotalLength               16      // uint16_t
#define NEO_NEOVirtualOffset                    18      // uint16_t
#define NEO_NEOStartupEM                        21      // 5 Bits, Bit 7-3
#define     NEO_NEOStartupEMMask 0xF8
#define     NEO_NEOStartupEMShift 3
#define NEO_NEOSegmentStartupColor              24      // 24 Bits, Bit 31-8
#define     NEO_NEOSegmentStartupColorMask 0xFFFFFF00
#define     NEO_NEOSegmentStartupColorShift 8
#define NEO_NEOSegmentStartupW                  27      // uint8_t
#define NEO_NEOSegmentStartupBrightness         28      // uint8_t
#define NEO_NEOSegmentResetColorOnEffectChange  23      // 1 Bit, Bit 4
#define     NEO_NEOSegmentResetColorOnEffectChangeMask 0x10
#define     NEO_NEOSegmentResetColorOnEffectChangeShift 4
#define NEO_NEOSegmentEmStopReturnMode          29      // 2 Bits, Bit 7-6
#define     NEO_NEOSegmentEmStopReturnModeMask 0xC0
#define     NEO_NEOSegmentEmStopReturnModeShift 6
#define NEO_NEOWipeSpeed                        30      // uint8_t
#define NEO_NEOWipeDirection                    31      // 8 Bits, Bit 7-0
#define NEO_NEORainbowSpeed                     32      // uint8_t
#define NEO_NEORainbowDelta                     33      // uint8_t
#define NEO_NEORainbowSaturation                34      // uint8_t
#define NEO_NEORainbowDensity                   35      // uint8_t
#define NEO_NEORainbowMode                      36      // uint8_t
#define NEO_NEOPride2015Speed                   37      // uint8_t
#define NEO_NEOJuggleSpeed                      38      // uint8_t
#define NEO_NEOJuggleNumDots                    39      // uint8_t
#define NEO_NEOJuggleFadeSpeed                  40      // uint8_t
#define NEO_NEOJuggleHueOffset                  41      // uint8_t
#define NEO_NEOBPMBPM                           42      // uint8_t
#define NEO_NEOBPMHue                           43      // uint8_t
#define NEO_NEOCylonSpeed                       44      // uint8_t
#define NEO_NEOCylonHue                         45      // uint8_t
#define NEO_NEOCylonEyeSize                     46      // uint8_t
#define NEO_NEOCylonFadeAmount                  47      // uint8_t
#define NEO_NEOCylonDirection                   48      // 8 Bits, Bit 7-0
#define NEO_NEOCylonMode                        49      // 8 Bits, Bit 7-0
#define NEO_NEOTestPhaseDuration                50      // uint8_t
#define NEO_NEOTestMode                         51      // 8 Bits, Bit 7-0
#define NEO_NEOFireSpeed                        52      // uint8_t
#define NEO_NEOFireCooling                      53      // uint8_t
#define NEO_NEOFireSparking                     54      // uint8_t
#define NEO_NEOFireReverseDirection             55      // 1 Bit, Bit 0
#define     NEO_NEOFireReverseDirectionMask 0x01
#define     NEO_NEOFireReverseDirectionShift 0
#define NEO_NEOFireBlueFireMode                 56      // 1 Bit, Bit 0
#define     NEO_NEOFireBlueFireModeMask 0x01
#define     NEO_NEOFireBlueFireModeShift 0
#define NEO_NEOTheaterChaseSpeed                57      // uint8_t
#define NEO_NEOTheaterChaseSpacing              58      // uint8_t
#define NEO_NEOTheaterChaseDotSize              59      // uint8_t
#define NEO_NEOTheaterChaseColorMode            60      // 8 Bits, Bit 7-0
#define NEO_NEOTheaterChaseColorSpeed           61      // uint8_t
#define NEO_NEOTheaterChaseBounce               62      // 8 Bits, Bit 7-0
#define NEO_NEOSparkleSpeed                     63      // uint8_t
#define NEO_NEOSparkleFadeRate                  64      // uint8_t
#define NEO_NEOSparkleSparkleCount              65      // uint8_t
#define NEO_NEOSparkleProbability               66      // uint8_t
#define NEO_NEOSparkleWhiteOnly                 67      // 1 Bit, Bit 0
#define     NEO_NEOSparkleWhiteOnlyMask 0x01
#define     NEO_NEOSparkleWhiteOnlyShift 0
#define NEO_NEOSparkleBurstMode                 68      // 1 Bit, Bit 0
#define     NEO_NEOSparkleBurstModeMask 0x01
#define     NEO_NEOSparkleBurstModeShift 0
#define NEO_NEOSparkleMode                      69      // 8 Bits, Bit 7-0
#define NEO_NEOBreathingSpeed                   70      // uint8_t
#define NEO_NEOBreathingMinBrightness           71      // uint8_t
#define NEO_NEOBreathingCurve                   72      // uint8_t
#define NEO_NEOBreathingRainbowMode             73      // 1 Bit, Bit 0
#define     NEO_NEOBreathingRainbowModeMask 0x01
#define     NEO_NEOBreathingRainbowModeShift 0
#define NEO_NEOBreathingWaveform                74      // 8 Bits, Bit 7-0
#define NEO_NEOStrobeSpeed                      75      // uint8_t
#define NEO_NEOStrobeOnRatio                    76      // uint8_t
#define NEO_NEOStrobeMinBrightness              77      // uint8_t
#define NEO_NEOStrobeRandomTiming               78      // 1 Bit, Bit 0
#define     NEO_NEOStrobeRandomTimingMask 0x01
#define     NEO_NEOStrobeRandomTimingShift 0
#define NEO_NEOStrobeRainbowStrobe              79      // 1 Bit, Bit 0
#define     NEO_NEOStrobeRainbowStrobeMask 0x01
#define     NEO_NEOStrobeRainbowStrobeShift 0
#define NEO_NEOCometSpeed                       80      // uint8_t
#define NEO_NEOCometFadeRate                    81      // uint8_t
#define NEO_NEOCometTailLength                  82      // uint8_t
#define NEO_NEOCometBounceMode                  83      // 1 Bit, Bit 0
#define     NEO_NEOCometBounceModeMask 0x01
#define     NEO_NEOCometBounceModeShift 0
#define NEO_NEOCometRainbowMode                 84      // 1 Bit, Bit 0
#define     NEO_NEOCometRainbowModeMask 0x01
#define     NEO_NEOCometRainbowModeShift 0
#define NEO_NEOCometMode                        85      // 8 Bits, Bit 7-0
#define NEO_NEONoiseSpeed                       86      // uint8_t
#define NEO_NEONoiseScale                       87      // uint8_t
#define NEO_NEONoiseSaturation                  88      // uint8_t
#define NEO_NEONoiseHueOffset                   89      // uint8_t
#define NEO_NEONoisePalette                     90      // 8 Bits, Bit 7-0
#define NEO_NEOPaletteSpeed                     91      // uint8_t
#define NEO_NEOPalettePalette                   92      // 8 Bits, Bit 7-0
#define NEO_NEOPaletteBlend                     93      // 1 Bit, Bit 0
#define     NEO_NEOPaletteBlendMask 0x01
#define     NEO_NEOPaletteBlendShift 0
#define NEO_NEOPaletteSpacing                   94      // uint8_t
#define NEO_NEOBlitzSpeed                       95      // uint8_t
#define NEO_NEOBlitzWidth                       96      // uint8_t
#define NEO_NEOBlitzDecay                       97      // uint8_t
#define NEO_NEOBlitzHue                         98      // uint8_t
#define NEO_NEOBlitzIntensity                   99      // uint8_t
#define NEO_NEOGradientSpeed                    100      // uint8_t
#define NEO_NEOGradientStartHue                 101      // uint8_t
#define NEO_NEOGradientEndHue                   102      // uint8_t
#define NEO_NEOGradientSaturation               103      // uint8_t
#define NEO_NEOKerzeSpeed                       104      // uint8_t
#define NEO_NEOKerzeIntensity                   105      // uint8_t
#define NEO_NEOKerzeZones                       106      // uint8_t
#define NEO_NEOLaufschriftSpeed                 107      // uint8_t
#define NEO_NEOLaufschriftGap                   108      // uint8_t
#define NEO_NEOLaufschriftLoop                  109      // 1 Bit, Bit 0
#define     NEO_NEOLaufschriftLoopMask 0x01
#define     NEO_NEOLaufschriftLoopShift 0
#define NEO_NEOLaufschriftText                  110      // char*, 14 Byte
#define     NEO_NEOLaufschriftTextLength 14
#define NEO_NEOLaufschriftFont                  124      // 8 Bits, Bit 7-0
#define NEO_NEOUhr2DViewMode                    125      // 8 Bits, Bit 7-0
#define NEO_NEOUhr2DShowSeconds                 126      // 1 Bit, Bit 0
#define     NEO_NEOUhr2DShowSecondsMask 0x01
#define     NEO_NEOUhr2DShowSecondsShift 0
#define NEO_NEOUhr2DBlinkColon                  127      // 1 Bit, Bit 0
#define     NEO_NEOUhr2DBlinkColonMask 0x01
#define     NEO_NEOUhr2DBlinkColonShift 0
#define NEO_NEOUhr2DColourHue                   128      // uint8_t
#define NEO_NEOUhr2DDateMode                    129      // 8 Bits, Bit 7-0
#define NEO_NEOUhr2DDateFormat                  130      // 8 Bits, Bit 7-0
#define NEO_NEOUhr2DDateHue                     131      // uint8_t
#define NEO_NEOUhr2DSwitchSec                   132      // uint8_t
#define NEO_NEOUhr2DScrollSpeed                 133      // uint8_t
#define NEO_NEOUhr2DFont                        134      // 8 Bits, Bit 7-0
#define NEO_NEOSnake2DSpeed                     135      // uint8_t
#define NEO_NEOSnake2DHeadHue                   136      // uint8_t
#define NEO_NEOSnake2DBodyMode                  137      // 8 Bits, Bit 7-0
#define NEO_NEOSnake2DBodyHue                   138      // uint8_t
#define NEO_NEOMatrix2DSpeed                    139      // uint8_t
#define NEO_NEOMatrix2DDensity                  140      // uint8_t
#define NEO_NEOMatrix2DPalette                  141      // 8 Bits, Bit 7-0
#define NEO_NEOMatrix2DGlitch                   142      // 1 Bit, Bit 0
#define     NEO_NEOMatrix2DGlitchMask 0x01
#define     NEO_NEOMatrix2DGlitchShift 0
#define NEO_NEOTetris2DSpeed                    143      // uint8_t
#define NEO_NEOTetris2DBgBrightness             144      // uint8_t
#define NEO_NEOTetris2DGhostPiece               145      // 1 Bit, Bit 0
#define     NEO_NEOTetris2DGhostPieceMask 0x01
#define     NEO_NEOTetris2DGhostPieceShift 0
#define NEO_NEOTetris2DColorMode                146      // 8 Bits, Bit 7-0
#define NEO_NEOTetris2DFlashLines               147      // 1 Bit, Bit 0
#define     NEO_NEOTetris2DFlashLinesMask 0x01
#define     NEO_NEOTetris2DFlashLinesShift 0
#define NEO_NEOTetris2DAutoPlay                 148      // 8 Bits, Bit 7-0
#define NEO_NEOTRONSpeed                        149      // uint8_t
#define NEO_NEOTRONHue                          150      // uint8_t
#define NEO_NEOTRONGridSpacing                  151      // uint8_t
#define NEO_NEOTRONGlow                         152      // uint8_t
#define NEO_NEOStarfieldWarpSpeed               153      // uint8_t
#define NEO_NEOStarfieldWarpDensity             154      // uint8_t
#define NEO_NEOStarfieldWarpColorMode           155      // 8 Bits, Bit 7-0
#define NEO_NEOStarfieldWarpWarpPulse           156      // 1 Bit, Bit 0
#define     NEO_NEOStarfieldWarpWarpPulseMask 0x01
#define     NEO_NEOStarfieldWarpWarpPulseShift 0
#define NEO_NEOPlasmaNebulaSpeed                157      // uint8_t
#define NEO_NEOPlasmaNebulaSaturation           158      // uint8_t
#define NEO_NEOPlasmaNebulaContrast             523      // uint8_t
#define NEO_NEOPlasmaNebulaPaletteShift         524      // uint8_t
#define NEO_NEOUFOSwarmSpeed                    525      // uint8_t
#define NEO_NEOUFOSwarmCount                    526      // uint8_t
#define NEO_NEOUFOSwarmHue                      527      // uint8_t
#define NEO_NEOUFOSwarmBeam                     528      // 1 Bit, Bit 0
#define     NEO_NEOUFOSwarmBeamMask 0x01
#define     NEO_NEOUFOSwarmBeamShift 0
#define NEO_NEOGameofLife2DSpeed                529      // uint8_t
#define NEO_NEOGameofLife2DHue                  530      // uint8_t
#define NEO_NEOGameofLife2DColorMode            531      // uint8_t
#define NEO_NEOGameofLife2DSpawnDensity         532      // uint8_t
#define NEO_NEODNA2DSpeed                       533      // uint8_t
#define NEO_NEODNA2DHue                         534      // uint8_t
#define NEO_NEODNA2DTwist                       535      // uint8_t
#define NEO_NEODNA2DRungSpacing                 536      // uint8_t
#define NEO_NEOAurora2DSpeed                    537      // uint8_t
#define NEO_NEOAurora2DHue                      538      // uint8_t
#define NEO_NEOAurora2DScale                    539      // uint8_t
#define NEO_NEOAurora2DIntensity                540      // uint8_t
#define NEO_NEOLissajous2DSpeed                 541      // uint8_t
#define NEO_NEOLissajous2DHue                   542      // uint8_t
#define NEO_NEOLissajous2DFreqA                 543      // uint8_t
#define NEO_NEOLissajous2DFreqB                 544      // uint8_t
#define NEO_NEOMetaballs2DSpeed                 545      // uint8_t
#define NEO_NEOMetaballs2DHue                   546      // uint8_t
#define NEO_NEOMetaballs2DBlobCount             547      // uint8_t
#define NEO_NEOMetaballs2DContrast              548      // uint8_t
#define NEO_NEOSceneCount                       159      // 4 Bits, Bit 7-4
#define     NEO_NEOSceneCountMask 0xF0
#define     NEO_NEOSceneCountShift 4
#define NEO_NEOSceneAEffectType                 160      // 8 Bits, Bit 7-0
#define NEO_NEOSceneAPrimaryColor               161      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneAPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneAPrimaryColorShift 8
#define NEO_NEOSceneAPrimaryWW                  164      // uint8_t
#define NEO_NEOSceneAPrimaryCW                  165      // uint8_t
#define NEO_NEOSceneASecondaryColor             166      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneASecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneASecondaryColorShift 8
#define NEO_NEOSceneASecondaryWW                169      // uint8_t
#define NEO_NEOSceneASecondaryCW                170      // uint8_t
#define NEO_NEOSceneABrightness                 171      // uint8_t
#define NEO_NEOSceneASlot0                      172      // uint8_t
#define NEO_NEOSceneASlot1                      173      // uint8_t
#define NEO_NEOSceneASlot2                      174      // uint8_t
#define NEO_NEOSceneASlot3                      175      // uint8_t
#define NEO_NEOSceneASlot4                      176      // uint8_t
#define NEO_NEOSceneASlot5                      177      // uint8_t
#define NEO_NEOSceneASlot6                      178      // uint8_t
#define NEO_NEOSceneASlot7                      179      // uint8_t
#define NEO_NEOSceneASlot8                      180      // uint8_t
#define NEO_NEOSceneASlot9                      181      // uint8_t
#define NEO_NEOSceneAEffectText                 182      // char*, 14 Byte
#define     NEO_NEOSceneAEffectTextLength 14
#define NEO_NEOSceneBEffectType                 196      // 8 Bits, Bit 7-0
#define NEO_NEOSceneBPrimaryColor               197      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneBPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneBPrimaryColorShift 8
#define NEO_NEOSceneBPrimaryWW                  200      // uint8_t
#define NEO_NEOSceneBPrimaryCW                  201      // uint8_t
#define NEO_NEOSceneBSecondaryColor             202      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneBSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneBSecondaryColorShift 8
#define NEO_NEOSceneBSecondaryWW                205      // uint8_t
#define NEO_NEOSceneBSecondaryCW                206      // uint8_t
#define NEO_NEOSceneBBrightness                 207      // uint8_t
#define NEO_NEOSceneBSlot0                      208      // uint8_t
#define NEO_NEOSceneBSlot1                      209      // uint8_t
#define NEO_NEOSceneBSlot2                      210      // uint8_t
#define NEO_NEOSceneBSlot3                      211      // uint8_t
#define NEO_NEOSceneBSlot4                      212      // uint8_t
#define NEO_NEOSceneBSlot5                      213      // uint8_t
#define NEO_NEOSceneBSlot6                      214      // uint8_t
#define NEO_NEOSceneBSlot7                      215      // uint8_t
#define NEO_NEOSceneBSlot8                      216      // uint8_t
#define NEO_NEOSceneBSlot9                      217      // uint8_t
#define NEO_NEOSceneBEffectText                 218      // char*, 14 Byte
#define     NEO_NEOSceneBEffectTextLength 14
#define NEO_NEOSceneCEffectType                 232      // 8 Bits, Bit 7-0
#define NEO_NEOSceneCPrimaryColor               233      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneCPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneCPrimaryColorShift 8
#define NEO_NEOSceneCPrimaryWW                  236      // uint8_t
#define NEO_NEOSceneCPrimaryCW                  237      // uint8_t
#define NEO_NEOSceneCSecondaryColor             238      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneCSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneCSecondaryColorShift 8
#define NEO_NEOSceneCSecondaryWW                241      // uint8_t
#define NEO_NEOSceneCSecondaryCW                242      // uint8_t
#define NEO_NEOSceneCBrightness                 243      // uint8_t
#define NEO_NEOSceneCSlot0                      244      // uint8_t
#define NEO_NEOSceneCSlot1                      245      // uint8_t
#define NEO_NEOSceneCSlot2                      246      // uint8_t
#define NEO_NEOSceneCSlot3                      247      // uint8_t
#define NEO_NEOSceneCSlot4                      248      // uint8_t
#define NEO_NEOSceneCSlot5                      249      // uint8_t
#define NEO_NEOSceneCSlot6                      250      // uint8_t
#define NEO_NEOSceneCSlot7                      251      // uint8_t
#define NEO_NEOSceneCSlot8                      252      // uint8_t
#define NEO_NEOSceneCSlot9                      253      // uint8_t
#define NEO_NEOSceneCEffectText                 254      // char*, 14 Byte
#define     NEO_NEOSceneCEffectTextLength 14
#define NEO_NEOSceneDEffectType                 268      // 8 Bits, Bit 7-0
#define NEO_NEOSceneDPrimaryColor               269      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneDPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneDPrimaryColorShift 8
#define NEO_NEOSceneDPrimaryWW                  272      // uint8_t
#define NEO_NEOSceneDPrimaryCW                  273      // uint8_t
#define NEO_NEOSceneDSecondaryColor             274      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneDSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneDSecondaryColorShift 8
#define NEO_NEOSceneDSecondaryWW                277      // uint8_t
#define NEO_NEOSceneDSecondaryCW                278      // uint8_t
#define NEO_NEOSceneDBrightness                 279      // uint8_t
#define NEO_NEOSceneDSlot0                      280      // uint8_t
#define NEO_NEOSceneDSlot1                      281      // uint8_t
#define NEO_NEOSceneDSlot2                      282      // uint8_t
#define NEO_NEOSceneDSlot3                      283      // uint8_t
#define NEO_NEOSceneDSlot4                      284      // uint8_t
#define NEO_NEOSceneDSlot5                      285      // uint8_t
#define NEO_NEOSceneDSlot6                      286      // uint8_t
#define NEO_NEOSceneDSlot7                      287      // uint8_t
#define NEO_NEOSceneDSlot8                      288      // uint8_t
#define NEO_NEOSceneDSlot9                      289      // uint8_t
#define NEO_NEOSceneDEffectText                 290      // char*, 14 Byte
#define     NEO_NEOSceneDEffectTextLength 14
#define NEO_NEOSceneEEffectType                 304      // 8 Bits, Bit 7-0
#define NEO_NEOSceneEPrimaryColor               305      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneEPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneEPrimaryColorShift 8
#define NEO_NEOSceneEPrimaryWW                  308      // uint8_t
#define NEO_NEOSceneEPrimaryCW                  309      // uint8_t
#define NEO_NEOSceneESecondaryColor             310      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneESecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneESecondaryColorShift 8
#define NEO_NEOSceneESecondaryWW                313      // uint8_t
#define NEO_NEOSceneESecondaryCW                314      // uint8_t
#define NEO_NEOSceneEBrightness                 315      // uint8_t
#define NEO_NEOSceneESlot0                      316      // uint8_t
#define NEO_NEOSceneESlot1                      317      // uint8_t
#define NEO_NEOSceneESlot2                      318      // uint8_t
#define NEO_NEOSceneESlot3                      319      // uint8_t
#define NEO_NEOSceneESlot4                      320      // uint8_t
#define NEO_NEOSceneESlot5                      321      // uint8_t
#define NEO_NEOSceneESlot6                      322      // uint8_t
#define NEO_NEOSceneESlot7                      323      // uint8_t
#define NEO_NEOSceneESlot8                      324      // uint8_t
#define NEO_NEOSceneESlot9                      325      // uint8_t
#define NEO_NEOSceneEEffectText                 326      // char*, 14 Byte
#define     NEO_NEOSceneEEffectTextLength 14
#define NEO_NEOSceneFEffectType                 340      // 8 Bits, Bit 7-0
#define NEO_NEOSceneFPrimaryColor               341      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneFPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneFPrimaryColorShift 8
#define NEO_NEOSceneFPrimaryWW                  344      // uint8_t
#define NEO_NEOSceneFPrimaryCW                  345      // uint8_t
#define NEO_NEOSceneFSecondaryColor             346      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneFSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneFSecondaryColorShift 8
#define NEO_NEOSceneFSecondaryWW                349      // uint8_t
#define NEO_NEOSceneFSecondaryCW                350      // uint8_t
#define NEO_NEOSceneFBrightness                 351      // uint8_t
#define NEO_NEOSceneFSlot0                      352      // uint8_t
#define NEO_NEOSceneFSlot1                      353      // uint8_t
#define NEO_NEOSceneFSlot2                      354      // uint8_t
#define NEO_NEOSceneFSlot3                      355      // uint8_t
#define NEO_NEOSceneFSlot4                      356      // uint8_t
#define NEO_NEOSceneFSlot5                      357      // uint8_t
#define NEO_NEOSceneFSlot6                      358      // uint8_t
#define NEO_NEOSceneFSlot7                      359      // uint8_t
#define NEO_NEOSceneFSlot8                      360      // uint8_t
#define NEO_NEOSceneFSlot9                      361      // uint8_t
#define NEO_NEOSceneFEffectText                 362      // char*, 14 Byte
#define     NEO_NEOSceneFEffectTextLength 14
#define NEO_NEOSceneGEffectType                 376      // 8 Bits, Bit 7-0
#define NEO_NEOSceneGPrimaryColor               377      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneGPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneGPrimaryColorShift 8
#define NEO_NEOSceneGPrimaryWW                  380      // uint8_t
#define NEO_NEOSceneGPrimaryCW                  381      // uint8_t
#define NEO_NEOSceneGSecondaryColor             382      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneGSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneGSecondaryColorShift 8
#define NEO_NEOSceneGSecondaryWW                385      // uint8_t
#define NEO_NEOSceneGSecondaryCW                386      // uint8_t
#define NEO_NEOSceneGBrightness                 387      // uint8_t
#define NEO_NEOSceneGSlot0                      388      // uint8_t
#define NEO_NEOSceneGSlot1                      389      // uint8_t
#define NEO_NEOSceneGSlot2                      390      // uint8_t
#define NEO_NEOSceneGSlot3                      391      // uint8_t
#define NEO_NEOSceneGSlot4                      392      // uint8_t
#define NEO_NEOSceneGSlot5                      393      // uint8_t
#define NEO_NEOSceneGSlot6                      394      // uint8_t
#define NEO_NEOSceneGSlot7                      395      // uint8_t
#define NEO_NEOSceneGSlot8                      396      // uint8_t
#define NEO_NEOSceneGSlot9                      397      // uint8_t
#define NEO_NEOSceneGEffectText                 398      // char*, 14 Byte
#define     NEO_NEOSceneGEffectTextLength 14
#define NEO_NEOSceneHEffectType                 412      // 8 Bits, Bit 7-0
#define NEO_NEOSceneHPrimaryColor               413      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneHPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneHPrimaryColorShift 8
#define NEO_NEOSceneHPrimaryWW                  416      // uint8_t
#define NEO_NEOSceneHPrimaryCW                  417      // uint8_t
#define NEO_NEOSceneHSecondaryColor             418      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneHSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneHSecondaryColorShift 8
#define NEO_NEOSceneHSecondaryWW                421      // uint8_t
#define NEO_NEOSceneHSecondaryCW                422      // uint8_t
#define NEO_NEOSceneHBrightness                 423      // uint8_t
#define NEO_NEOSceneHSlot0                      424      // uint8_t
#define NEO_NEOSceneHSlot1                      425      // uint8_t
#define NEO_NEOSceneHSlot2                      426      // uint8_t
#define NEO_NEOSceneHSlot3                      427      // uint8_t
#define NEO_NEOSceneHSlot4                      428      // uint8_t
#define NEO_NEOSceneHSlot5                      429      // uint8_t
#define NEO_NEOSceneHSlot6                      430      // uint8_t
#define NEO_NEOSceneHSlot7                      431      // uint8_t
#define NEO_NEOSceneHSlot8                      432      // uint8_t
#define NEO_NEOSceneHSlot9                      433      // uint8_t
#define NEO_NEOSceneHEffectText                 434      // char*, 14 Byte
#define     NEO_NEOSceneHEffectTextLength 14
#define NEO_NEOSceneIEffectType                 448      // 8 Bits, Bit 7-0
#define NEO_NEOSceneIPrimaryColor               449      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneIPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneIPrimaryColorShift 8
#define NEO_NEOSceneIPrimaryWW                  452      // uint8_t
#define NEO_NEOSceneIPrimaryCW                  453      // uint8_t
#define NEO_NEOSceneISecondaryColor             454      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneISecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneISecondaryColorShift 8
#define NEO_NEOSceneISecondaryWW                457      // uint8_t
#define NEO_NEOSceneISecondaryCW                458      // uint8_t
#define NEO_NEOSceneIBrightness                 459      // uint8_t
#define NEO_NEOSceneISlot0                      460      // uint8_t
#define NEO_NEOSceneISlot1                      461      // uint8_t
#define NEO_NEOSceneISlot2                      462      // uint8_t
#define NEO_NEOSceneISlot3                      463      // uint8_t
#define NEO_NEOSceneISlot4                      464      // uint8_t
#define NEO_NEOSceneISlot5                      465      // uint8_t
#define NEO_NEOSceneISlot6                      466      // uint8_t
#define NEO_NEOSceneISlot7                      467      // uint8_t
#define NEO_NEOSceneISlot8                      468      // uint8_t
#define NEO_NEOSceneISlot9                      469      // uint8_t
#define NEO_NEOSceneIEffectText                 470      // char*, 14 Byte
#define     NEO_NEOSceneIEffectTextLength 14
#define NEO_NEOSceneJEffectType                 484      // 8 Bits, Bit 7-0
#define NEO_NEOSceneJPrimaryColor               485      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneJPrimaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneJPrimaryColorShift 8
#define NEO_NEOSceneJPrimaryWW                  488      // uint8_t
#define NEO_NEOSceneJPrimaryCW                  489      // uint8_t
#define NEO_NEOSceneJSecondaryColor             490      // 24 Bits, Bit 31-8
#define     NEO_NEOSceneJSecondaryColorMask 0xFFFFFF00
#define     NEO_NEOSceneJSecondaryColorShift 8
#define NEO_NEOSceneJSecondaryWW                493      // uint8_t
#define NEO_NEOSceneJSecondaryCW                494      // uint8_t
#define NEO_NEOSceneJBrightness                 495      // uint8_t
#define NEO_NEOSceneJSlot0                      496      // uint8_t
#define NEO_NEOSceneJSlot1                      497      // uint8_t
#define NEO_NEOSceneJSlot2                      498      // uint8_t
#define NEO_NEOSceneJSlot3                      499      // uint8_t
#define NEO_NEOSceneJSlot4                      500      // uint8_t
#define NEO_NEOSceneJSlot5                      501      // uint8_t
#define NEO_NEOSceneJSlot6                      502      // uint8_t
#define NEO_NEOSceneJSlot7                      503      // uint8_t
#define NEO_NEOSceneJSlot8                      504      // uint8_t
#define NEO_NEOSceneJSlot9                      505      // uint8_t
#define NEO_NEOSceneJEffectText                 506      // char*, 14 Byte
#define     NEO_NEOSceneJEffectTextLength 14
#define NEO_NEOHCLMode                          520      // 3 Bits, Bit 7-5
#define     NEO_NEOHCLModeMask 0xE0
#define     NEO_NEOHCLModeShift 5
#define NEO_NEOHCLMaster                        521      // 8 Bits, Bit 7-0
#define NEO_NEOHCLApplyMode                     522      // 3 Bits, Bit 7-5
#define     NEO_NEOHCLApplyModeMask 0xE0
#define     NEO_NEOHCLApplyModeShift 5

// Start LED
#define ParamNEO_NEOSegmentStart                     ((knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentStart)) & NEO_NEOSegmentStartMask) >> NEO_NEOSegmentStartShift)
// Ende LED
#define ParamNEO_NEOSegmentEnd                       ((knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentEnd)) & NEO_NEOSegmentEndMask) >> NEO_NEOSegmentEndShift)
// Offset
#define ParamNEO_NEOSegmentOffset                    (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentOffset)))
// Gruppierung
#define ParamNEO_NEOSegmentGrouping                  (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentGrouping)))
// Dunkelabstand
#define ParamNEO_NEOSegmentSpacing                   (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOSegmentSpacing)))
// Richtung umkehren
#define ParamNEO_NEOSegmentReverseDirection          ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentReverseDirection)) & NEO_NEOSegmentReverseDirectionMask))
// Spiegelung des Effekts
#define ParamNEO_NEOSegmentMirrorEffect              ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentMirrorEffect)) & NEO_NEOSegmentMirrorEffectMask))
// Verhalten nach Neustart
#define ParamNEO_NEOSegmentStartupBehavior           ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupBehavior)) & NEO_NEOSegmentStartupBehaviorMask) >> NEO_NEOSegmentStartupBehaviorShift)
// Topologie
#define ParamNEO_NEOSegmentTopology                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentTopology)) & NEO_NEOSegmentTopologyMask)
// Effekt
#define ParamNEO_NEONEOEffectType                    (PT_NEOEffectType)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEONEOEffectType)))
// Matrix: Breite (Spalten)
#define ParamNEO_NEOMatrixWidth                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrixWidth)))
// Matrix: Höhe (Zeilen)
#define ParamNEO_NEOMatrixHeight                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrixHeight)))
// Matrix: Tiefe (Ebenen / Kachel-Höhe)
#define ParamNEO_NEOMatrixDepth                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrixDepth)))
// Effektkette: Modus
#define ParamNEO_NEOSyncMode                         ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSyncMode)) & NEO_NEOSyncModeMask) >> NEO_NEOSyncModeShift)
// Sync-Verhalten bei lokalem Override
#define ParamNEO_NEOSyncOverridePolicy               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSyncOverridePolicy)) & NEO_NEOSyncOverridePolicyMask))
// Slave-Timeout
#define ParamNEO_NEOSyncTimeout                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSyncTimeout)) & NEO_NEOSyncTimeoutMask)
// Effektkette: Gesamtlänge (LEDs)
#define ParamNEO_NEOVirtualTotalLength               (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOVirtualTotalLength)))
// Effektkette: Mein Offset (LEDs)
#define ParamNEO_NEOVirtualOffset                    (knx.paramWord(NEO_ParamCalcIndex(NEO_NEOVirtualOffset)))
// Effektmanager bei Start
#define ParamNEO_NEOStartupEM                        ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStartupEM)) & NEO_NEOStartupEMMask) >> NEO_NEOStartupEMShift)
// Start-Farbe RGB
#define ParamNEO_NEOSegmentStartupColor              ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSegmentStartupColor)) & NEO_NEOSegmentStartupColorMask) >> NEO_NEOSegmentStartupColorShift)
// Start-Farbe W
#define ParamNEO_NEOSegmentStartupW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupW)))
// Start-Helligkeit
#define ParamNEO_NEOSegmentStartupBrightness         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentStartupBrightness)))
// Bei Effektwechsel Farbe auf Standard zurücksetzen
#define ParamNEO_NEOSegmentResetColorOnEffectChange  ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentResetColorOnEffectChange)) & NEO_NEOSegmentResetColorOnEffectChangeMask))
// Effektmanager-Stop: Rueckkehr
#define ParamNEO_NEOSegmentEmStopReturnMode          ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSegmentEmStopReturnMode)) & NEO_NEOSegmentEmStopReturnModeMask) >> NEO_NEOSegmentEmStopReturnModeShift)
// Speed (Wipe)
#define ParamNEO_NEOWipeSpeed                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOWipeSpeed)))
// Direction (Wipe)
#define ParamNEO_NEOWipeDirection                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOWipeDirection)))
// Speed (Rainbow)
#define ParamNEO_NEORainbowSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowSpeed)))
// Delta (Rainbow)
#define ParamNEO_NEORainbowDelta                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowDelta)))
// Saturation (Rainbow)
#define ParamNEO_NEORainbowSaturation                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowSaturation)))
// Density (Rainbow)
#define ParamNEO_NEORainbowDensity                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowDensity)))
// Mode (Rainbow)
#define ParamNEO_NEORainbowMode                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEORainbowMode)))
// Speed (Pride2015)
#define ParamNEO_NEOPride2015Speed                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPride2015Speed)))
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
// Direction (Cylon)
#define ParamNEO_NEOCylonDirection                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonDirection)))
// Mode (Cylon)
#define ParamNEO_NEOCylonMode                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCylonMode)))
// PhaseDuration (Test)
#define ParamNEO_NEOTestPhaseDuration                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTestPhaseDuration)))
// Mode (Test)
#define ParamNEO_NEOTestMode                         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTestMode)))
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
// ColorMode (Theater Chase)
#define ParamNEO_NEOTheaterChaseColorMode            (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseColorMode)))
// ColorSpeed (Theater Chase)
#define ParamNEO_NEOTheaterChaseColorSpeed           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseColorSpeed)))
// Bounce (Theater Chase)
#define ParamNEO_NEOTheaterChaseBounce               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTheaterChaseBounce)))
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
// Mode (Sparkle)
#define ParamNEO_NEOSparkleMode                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSparkleMode)))
// Speed (Breathing)
#define ParamNEO_NEOBreathingSpeed                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingSpeed)))
// MinBrightness (Breathing)
#define ParamNEO_NEOBreathingMinBrightness           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingMinBrightness)))
// Curve (Breathing)
#define ParamNEO_NEOBreathingCurve                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingCurve)))
// RainbowMode (Breathing)
#define ParamNEO_NEOBreathingRainbowMode             ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingRainbowMode)) & NEO_NEOBreathingRainbowModeMask))
// Waveform (Breathing)
#define ParamNEO_NEOBreathingWaveform                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBreathingWaveform)))
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
// Mode (Comet)
#define ParamNEO_NEOCometMode                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOCometMode)))
// Speed (Noise)
#define ParamNEO_NEONoiseSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseSpeed)))
// Scale (Noise)
#define ParamNEO_NEONoiseScale                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseScale)))
// Saturation (Noise)
#define ParamNEO_NEONoiseSaturation                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseSaturation)))
// HueOffset (Noise)
#define ParamNEO_NEONoiseHueOffset                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoiseHueOffset)))
// Palette (Noise)
#define ParamNEO_NEONoisePalette                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEONoisePalette)))
// Speed (Palette)
#define ParamNEO_NEOPaletteSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPaletteSpeed)))
// Palette (Palette)
#define ParamNEO_NEOPalettePalette                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPalettePalette)))
// Blend (Palette)
#define ParamNEO_NEOPaletteBlend                     ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPaletteBlend)) & NEO_NEOPaletteBlendMask))
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
// Intensity (Blitz)
#define ParamNEO_NEOBlitzIntensity                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOBlitzIntensity)))
// Speed (Gradient)
#define ParamNEO_NEOGradientSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientSpeed)))
// StartHue (Gradient)
#define ParamNEO_NEOGradientStartHue                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientStartHue)))
// EndHue (Gradient)
#define ParamNEO_NEOGradientEndHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientEndHue)))
// Saturation (Gradient)
#define ParamNEO_NEOGradientSaturation               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGradientSaturation)))
// Speed (Kerze)
#define ParamNEO_NEOKerzeSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOKerzeSpeed)))
// Intensity (Kerze)
#define ParamNEO_NEOKerzeIntensity                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOKerzeIntensity)))
// Zones (Kerze)
#define ParamNEO_NEOKerzeZones                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOKerzeZones)))
// Speed (Laufschrift)
#define ParamNEO_NEOLaufschriftSpeed                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLaufschriftSpeed)))
// Gap (Laufschrift)
#define ParamNEO_NEOLaufschriftGap                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLaufschriftGap)))
// Loop (Laufschrift)
#define ParamNEO_NEOLaufschriftLoop                  ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLaufschriftLoop)) & NEO_NEOLaufschriftLoopMask))
// Text (Laufschrift)
#define ParamNEO_NEOLaufschriftText                  (knx.paramData(NEO_ParamCalcIndex(NEO_NEOLaufschriftText)))
#define ParamNEO_NEOLaufschriftTextStr               (knx.paramString(NEO_ParamCalcIndex(NEO_NEOLaufschriftText), NEO_NEOLaufschriftTextLength))
// Font (Laufschrift)
#define ParamNEO_NEOLaufschriftFont                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLaufschriftFont)))
// ViewMode (Uhr 2D)
#define ParamNEO_NEOUhr2DViewMode                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DViewMode)))
// ShowSeconds (Uhr 2D)
#define ParamNEO_NEOUhr2DShowSeconds                 ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DShowSeconds)) & NEO_NEOUhr2DShowSecondsMask))
// BlinkColon (Uhr 2D)
#define ParamNEO_NEOUhr2DBlinkColon                  ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DBlinkColon)) & NEO_NEOUhr2DBlinkColonMask))
// ColourHue (Uhr 2D)
#define ParamNEO_NEOUhr2DColourHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DColourHue)))
// DateMode (Uhr 2D)
#define ParamNEO_NEOUhr2DDateMode                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DDateMode)))
// DateFormat (Uhr 2D)
#define ParamNEO_NEOUhr2DDateFormat                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DDateFormat)))
// DateHue (Uhr 2D)
#define ParamNEO_NEOUhr2DDateHue                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DDateHue)))
// SwitchSec (Uhr 2D)
#define ParamNEO_NEOUhr2DSwitchSec                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DSwitchSec)))
// ScrollSpeed (Uhr 2D)
#define ParamNEO_NEOUhr2DScrollSpeed                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DScrollSpeed)))
// Font (Uhr 2D)
#define ParamNEO_NEOUhr2DFont                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUhr2DFont)))
// Speed (Snake 2D)
#define ParamNEO_NEOSnake2DSpeed                     (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSnake2DSpeed)))
// HeadHue (Snake 2D)
#define ParamNEO_NEOSnake2DHeadHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSnake2DHeadHue)))
// BodyMode (Snake 2D)
#define ParamNEO_NEOSnake2DBodyMode                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSnake2DBodyMode)))
// BodyHue (Snake 2D)
#define ParamNEO_NEOSnake2DBodyHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSnake2DBodyHue)))
// Speed (Matrix 2D)
#define ParamNEO_NEOMatrix2DSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrix2DSpeed)))
// Density (Matrix 2D)
#define ParamNEO_NEOMatrix2DDensity                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrix2DDensity)))
// Palette (Matrix 2D)
#define ParamNEO_NEOMatrix2DPalette                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrix2DPalette)))
// Glitch (Matrix 2D)
#define ParamNEO_NEOMatrix2DGlitch                   ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMatrix2DGlitch)) & NEO_NEOMatrix2DGlitchMask))
// Speed (Tetris 2D)
#define ParamNEO_NEOTetris2DSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DSpeed)))
// BgBrightness (Tetris 2D)
#define ParamNEO_NEOTetris2DBgBrightness             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DBgBrightness)))
// GhostPiece (Tetris 2D)
#define ParamNEO_NEOTetris2DGhostPiece               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DGhostPiece)) & NEO_NEOTetris2DGhostPieceMask))
// ColorMode (Tetris 2D)
#define ParamNEO_NEOTetris2DColorMode                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DColorMode)))
// FlashLines (Tetris 2D)
#define ParamNEO_NEOTetris2DFlashLines               ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DFlashLines)) & NEO_NEOTetris2DFlashLinesMask))
// AutoPlay (Tetris 2D)
#define ParamNEO_NEOTetris2DAutoPlay                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTetris2DAutoPlay)))
// Speed (TRON)
#define ParamNEO_NEOTRONSpeed                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTRONSpeed)))
// Hue (TRON)
#define ParamNEO_NEOTRONHue                          (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTRONHue)))
// GridSpacing (TRON)
#define ParamNEO_NEOTRONGridSpacing                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTRONGridSpacing)))
// Glow (TRON)
#define ParamNEO_NEOTRONGlow                         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOTRONGlow)))
// Speed (Starfield Warp)
#define ParamNEO_NEOStarfieldWarpSpeed               (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStarfieldWarpSpeed)))
// Density (Starfield Warp)
#define ParamNEO_NEOStarfieldWarpDensity             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStarfieldWarpDensity)))
// ColorMode (Starfield Warp)
#define ParamNEO_NEOStarfieldWarpColorMode           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStarfieldWarpColorMode)))
// WarpPulse (Starfield Warp)
#define ParamNEO_NEOStarfieldWarpWarpPulse           ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOStarfieldWarpWarpPulse)) & NEO_NEOStarfieldWarpWarpPulseMask))
// Speed (Plasma Nebula)
#define ParamNEO_NEOPlasmaNebulaSpeed                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPlasmaNebulaSpeed)))
// Saturation (Plasma Nebula)
#define ParamNEO_NEOPlasmaNebulaSaturation           (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPlasmaNebulaSaturation)))
// Contrast (Plasma Nebula)
#define ParamNEO_NEOPlasmaNebulaContrast             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPlasmaNebulaContrast)))
// PaletteShift (Plasma Nebula)
#define ParamNEO_NEOPlasmaNebulaPaletteShift         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOPlasmaNebulaPaletteShift)))
// Speed (UFO Swarm)
#define ParamNEO_NEOUFOSwarmSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUFOSwarmSpeed)))
// Count (UFO Swarm)
#define ParamNEO_NEOUFOSwarmCount                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUFOSwarmCount)))
// Hue (UFO Swarm)
#define ParamNEO_NEOUFOSwarmHue                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUFOSwarmHue)))
// Beam (UFO Swarm)
#define ParamNEO_NEOUFOSwarmBeam                     ((bool)(knx.paramByte(NEO_ParamCalcIndex(NEO_NEOUFOSwarmBeam)) & NEO_NEOUFOSwarmBeamMask))
// Speed (Game of Life 2D)
#define ParamNEO_NEOGameofLife2DSpeed                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGameofLife2DSpeed)))
// Hue (Game of Life 2D)
#define ParamNEO_NEOGameofLife2DHue                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGameofLife2DHue)))
// ColorMode (Game of Life 2D)
#define ParamNEO_NEOGameofLife2DColorMode            (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGameofLife2DColorMode)))
// SpawnDensity (Game of Life 2D)
#define ParamNEO_NEOGameofLife2DSpawnDensity         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOGameofLife2DSpawnDensity)))
// Speed (DNA 2D)
#define ParamNEO_NEODNA2DSpeed                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEODNA2DSpeed)))
// Hue (DNA 2D)
#define ParamNEO_NEODNA2DHue                         (knx.paramByte(NEO_ParamCalcIndex(NEO_NEODNA2DHue)))
// Twist (DNA 2D)
#define ParamNEO_NEODNA2DTwist                       (knx.paramByte(NEO_ParamCalcIndex(NEO_NEODNA2DTwist)))
// RungSpacing (DNA 2D)
#define ParamNEO_NEODNA2DRungSpacing                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEODNA2DRungSpacing)))
// Speed (Aurora 2D)
#define ParamNEO_NEOAurora2DSpeed                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOAurora2DSpeed)))
// Hue (Aurora 2D)
#define ParamNEO_NEOAurora2DHue                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOAurora2DHue)))
// Scale (Aurora 2D)
#define ParamNEO_NEOAurora2DScale                    (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOAurora2DScale)))
// Intensity (Aurora 2D)
#define ParamNEO_NEOAurora2DIntensity                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOAurora2DIntensity)))
// Speed (Lissajous 2D)
#define ParamNEO_NEOLissajous2DSpeed                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLissajous2DSpeed)))
// Hue (Lissajous 2D)
#define ParamNEO_NEOLissajous2DHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLissajous2DHue)))
// FreqA (Lissajous 2D)
#define ParamNEO_NEOLissajous2DFreqA                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLissajous2DFreqA)))
// FreqB (Lissajous 2D)
#define ParamNEO_NEOLissajous2DFreqB                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOLissajous2DFreqB)))
// Speed (Metaballs 2D)
#define ParamNEO_NEOMetaballs2DSpeed                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMetaballs2DSpeed)))
// Hue (Metaballs 2D)
#define ParamNEO_NEOMetaballs2DHue                   (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMetaballs2DHue)))
// BlobCount (Metaballs 2D)
#define ParamNEO_NEOMetaballs2DBlobCount             (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMetaballs2DBlobCount)))
// Contrast (Metaballs 2D)
#define ParamNEO_NEOMetaballs2DContrast              (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOMetaballs2DContrast)))
// Anzahl Szenen
#define ParamNEO_NEOSceneCount                       ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCount)) & NEO_NEOSceneCountMask) >> NEO_NEOSceneCountShift)
// Effekt
#define ParamNEO_NEOSceneAEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneAEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneAPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneAPrimaryColor)) & NEO_NEOSceneAPrimaryColorMask) >> NEO_NEOSceneAPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneAPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneAPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneAPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneAPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneASecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneASecondaryColor)) & NEO_NEOSceneASecondaryColorMask) >> NEO_NEOSceneASecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneASecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneASecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneABrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneABrightness)))
// Parameter 1 (Szene 1)
#define ParamNEO_NEOSceneASlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot0)))
// Parameter 2 (Szene 1)
#define ParamNEO_NEOSceneASlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot1)))
// Parameter 3 (Szene 1)
#define ParamNEO_NEOSceneASlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot2)))
// Parameter 4 (Szene 1)
#define ParamNEO_NEOSceneASlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot3)))
// Parameter 5 (Szene 1)
#define ParamNEO_NEOSceneASlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot4)))
// Parameter 6 (Szene 1)
#define ParamNEO_NEOSceneASlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot5)))
// Parameter 7 (Szene 1)
#define ParamNEO_NEOSceneASlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot6)))
// Parameter 8 (Szene 1)
#define ParamNEO_NEOSceneASlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot7)))
// Parameter 9 (Szene 1)
#define ParamNEO_NEOSceneASlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot8)))
// Parameter 10 (Szene 1)
#define ParamNEO_NEOSceneASlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneASlot9)))
// Effekt-Text (Szene 1)
#define ParamNEO_NEOSceneAEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneAEffectText)))
#define ParamNEO_NEOSceneAEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneAEffectText), NEO_NEOSceneAEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneBEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneBPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneBPrimaryColor)) & NEO_NEOSceneBPrimaryColorMask) >> NEO_NEOSceneBPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneBPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneBPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneBSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneBSecondaryColor)) & NEO_NEOSceneBSecondaryColorMask) >> NEO_NEOSceneBSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneBSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneBSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneBBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBBrightness)))
// Parameter 1 (Szene 2)
#define ParamNEO_NEOSceneBSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot0)))
// Parameter 2 (Szene 2)
#define ParamNEO_NEOSceneBSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot1)))
// Parameter 3 (Szene 2)
#define ParamNEO_NEOSceneBSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot2)))
// Parameter 4 (Szene 2)
#define ParamNEO_NEOSceneBSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot3)))
// Parameter 5 (Szene 2)
#define ParamNEO_NEOSceneBSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot4)))
// Parameter 6 (Szene 2)
#define ParamNEO_NEOSceneBSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot5)))
// Parameter 7 (Szene 2)
#define ParamNEO_NEOSceneBSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot6)))
// Parameter 8 (Szene 2)
#define ParamNEO_NEOSceneBSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot7)))
// Parameter 9 (Szene 2)
#define ParamNEO_NEOSceneBSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot8)))
// Parameter 10 (Szene 2)
#define ParamNEO_NEOSceneBSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneBSlot9)))
// Effekt-Text (Szene 2)
#define ParamNEO_NEOSceneBEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneBEffectText)))
#define ParamNEO_NEOSceneBEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneBEffectText), NEO_NEOSceneBEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneCEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneCPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneCPrimaryColor)) & NEO_NEOSceneCPrimaryColorMask) >> NEO_NEOSceneCPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneCPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneCPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneCSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneCSecondaryColor)) & NEO_NEOSceneCSecondaryColorMask) >> NEO_NEOSceneCSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneCSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneCSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneCBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCBrightness)))
// Parameter 1 (Szene 3)
#define ParamNEO_NEOSceneCSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot0)))
// Parameter 2 (Szene 3)
#define ParamNEO_NEOSceneCSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot1)))
// Parameter 3 (Szene 3)
#define ParamNEO_NEOSceneCSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot2)))
// Parameter 4 (Szene 3)
#define ParamNEO_NEOSceneCSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot3)))
// Parameter 5 (Szene 3)
#define ParamNEO_NEOSceneCSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot4)))
// Parameter 6 (Szene 3)
#define ParamNEO_NEOSceneCSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot5)))
// Parameter 7 (Szene 3)
#define ParamNEO_NEOSceneCSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot6)))
// Parameter 8 (Szene 3)
#define ParamNEO_NEOSceneCSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot7)))
// Parameter 9 (Szene 3)
#define ParamNEO_NEOSceneCSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot8)))
// Parameter 10 (Szene 3)
#define ParamNEO_NEOSceneCSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneCSlot9)))
// Effekt-Text (Szene 3)
#define ParamNEO_NEOSceneCEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneCEffectText)))
#define ParamNEO_NEOSceneCEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneCEffectText), NEO_NEOSceneCEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneDEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneDPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneDPrimaryColor)) & NEO_NEOSceneDPrimaryColorMask) >> NEO_NEOSceneDPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneDPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneDPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneDSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneDSecondaryColor)) & NEO_NEOSceneDSecondaryColorMask) >> NEO_NEOSceneDSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneDSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneDSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneDBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDBrightness)))
// Parameter 1 (Szene 4)
#define ParamNEO_NEOSceneDSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot0)))
// Parameter 2 (Szene 4)
#define ParamNEO_NEOSceneDSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot1)))
// Parameter 3 (Szene 4)
#define ParamNEO_NEOSceneDSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot2)))
// Parameter 4 (Szene 4)
#define ParamNEO_NEOSceneDSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot3)))
// Parameter 5 (Szene 4)
#define ParamNEO_NEOSceneDSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot4)))
// Parameter 6 (Szene 4)
#define ParamNEO_NEOSceneDSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot5)))
// Parameter 7 (Szene 4)
#define ParamNEO_NEOSceneDSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot6)))
// Parameter 8 (Szene 4)
#define ParamNEO_NEOSceneDSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot7)))
// Parameter 9 (Szene 4)
#define ParamNEO_NEOSceneDSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot8)))
// Parameter 10 (Szene 4)
#define ParamNEO_NEOSceneDSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneDSlot9)))
// Effekt-Text (Szene 4)
#define ParamNEO_NEOSceneDEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneDEffectText)))
#define ParamNEO_NEOSceneDEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneDEffectText), NEO_NEOSceneDEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneEEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneEEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneEPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneEPrimaryColor)) & NEO_NEOSceneEPrimaryColorMask) >> NEO_NEOSceneEPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneEPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneEPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneEPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneEPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneESecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneESecondaryColor)) & NEO_NEOSceneESecondaryColorMask) >> NEO_NEOSceneESecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneESecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneESecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneEBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneEBrightness)))
// Parameter 1 (Szene 5)
#define ParamNEO_NEOSceneESlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot0)))
// Parameter 2 (Szene 5)
#define ParamNEO_NEOSceneESlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot1)))
// Parameter 3 (Szene 5)
#define ParamNEO_NEOSceneESlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot2)))
// Parameter 4 (Szene 5)
#define ParamNEO_NEOSceneESlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot3)))
// Parameter 5 (Szene 5)
#define ParamNEO_NEOSceneESlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot4)))
// Parameter 6 (Szene 5)
#define ParamNEO_NEOSceneESlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot5)))
// Parameter 7 (Szene 5)
#define ParamNEO_NEOSceneESlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot6)))
// Parameter 8 (Szene 5)
#define ParamNEO_NEOSceneESlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot7)))
// Parameter 9 (Szene 5)
#define ParamNEO_NEOSceneESlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot8)))
// Parameter 10 (Szene 5)
#define ParamNEO_NEOSceneESlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneESlot9)))
// Effekt-Text (Szene 5)
#define ParamNEO_NEOSceneEEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneEEffectText)))
#define ParamNEO_NEOSceneEEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneEEffectText), NEO_NEOSceneEEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneFEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneFPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneFPrimaryColor)) & NEO_NEOSceneFPrimaryColorMask) >> NEO_NEOSceneFPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneFPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneFPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneFSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneFSecondaryColor)) & NEO_NEOSceneFSecondaryColorMask) >> NEO_NEOSceneFSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneFSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneFSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneFBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFBrightness)))
// Parameter 1 (Szene 6)
#define ParamNEO_NEOSceneFSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot0)))
// Parameter 2 (Szene 6)
#define ParamNEO_NEOSceneFSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot1)))
// Parameter 3 (Szene 6)
#define ParamNEO_NEOSceneFSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot2)))
// Parameter 4 (Szene 6)
#define ParamNEO_NEOSceneFSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot3)))
// Parameter 5 (Szene 6)
#define ParamNEO_NEOSceneFSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot4)))
// Parameter 6 (Szene 6)
#define ParamNEO_NEOSceneFSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot5)))
// Parameter 7 (Szene 6)
#define ParamNEO_NEOSceneFSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot6)))
// Parameter 8 (Szene 6)
#define ParamNEO_NEOSceneFSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot7)))
// Parameter 9 (Szene 6)
#define ParamNEO_NEOSceneFSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot8)))
// Parameter 10 (Szene 6)
#define ParamNEO_NEOSceneFSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneFSlot9)))
// Effekt-Text (Szene 6)
#define ParamNEO_NEOSceneFEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneFEffectText)))
#define ParamNEO_NEOSceneFEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneFEffectText), NEO_NEOSceneFEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneGEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneGPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneGPrimaryColor)) & NEO_NEOSceneGPrimaryColorMask) >> NEO_NEOSceneGPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneGPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneGPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneGSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneGSecondaryColor)) & NEO_NEOSceneGSecondaryColorMask) >> NEO_NEOSceneGSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneGSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneGSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneGBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGBrightness)))
// Parameter 1 (Szene 7)
#define ParamNEO_NEOSceneGSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot0)))
// Parameter 2 (Szene 7)
#define ParamNEO_NEOSceneGSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot1)))
// Parameter 3 (Szene 7)
#define ParamNEO_NEOSceneGSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot2)))
// Parameter 4 (Szene 7)
#define ParamNEO_NEOSceneGSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot3)))
// Parameter 5 (Szene 7)
#define ParamNEO_NEOSceneGSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot4)))
// Parameter 6 (Szene 7)
#define ParamNEO_NEOSceneGSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot5)))
// Parameter 7 (Szene 7)
#define ParamNEO_NEOSceneGSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot6)))
// Parameter 8 (Szene 7)
#define ParamNEO_NEOSceneGSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot7)))
// Parameter 9 (Szene 7)
#define ParamNEO_NEOSceneGSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot8)))
// Parameter 10 (Szene 7)
#define ParamNEO_NEOSceneGSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneGSlot9)))
// Effekt-Text (Szene 7)
#define ParamNEO_NEOSceneGEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneGEffectText)))
#define ParamNEO_NEOSceneGEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneGEffectText), NEO_NEOSceneGEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneHEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneHPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneHPrimaryColor)) & NEO_NEOSceneHPrimaryColorMask) >> NEO_NEOSceneHPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneHPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneHPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneHSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneHSecondaryColor)) & NEO_NEOSceneHSecondaryColorMask) >> NEO_NEOSceneHSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneHSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneHSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneHBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHBrightness)))
// Parameter 1 (Szene 8)
#define ParamNEO_NEOSceneHSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot0)))
// Parameter 2 (Szene 8)
#define ParamNEO_NEOSceneHSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot1)))
// Parameter 3 (Szene 8)
#define ParamNEO_NEOSceneHSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot2)))
// Parameter 4 (Szene 8)
#define ParamNEO_NEOSceneHSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot3)))
// Parameter 5 (Szene 8)
#define ParamNEO_NEOSceneHSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot4)))
// Parameter 6 (Szene 8)
#define ParamNEO_NEOSceneHSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot5)))
// Parameter 7 (Szene 8)
#define ParamNEO_NEOSceneHSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot6)))
// Parameter 8 (Szene 8)
#define ParamNEO_NEOSceneHSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot7)))
// Parameter 9 (Szene 8)
#define ParamNEO_NEOSceneHSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot8)))
// Parameter 10 (Szene 8)
#define ParamNEO_NEOSceneHSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneHSlot9)))
// Effekt-Text (Szene 8)
#define ParamNEO_NEOSceneHEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneHEffectText)))
#define ParamNEO_NEOSceneHEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneHEffectText), NEO_NEOSceneHEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneIEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneIEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneIPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneIPrimaryColor)) & NEO_NEOSceneIPrimaryColorMask) >> NEO_NEOSceneIPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneIPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneIPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneIPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneIPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneISecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneISecondaryColor)) & NEO_NEOSceneISecondaryColorMask) >> NEO_NEOSceneISecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneISecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneISecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneIBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneIBrightness)))
// Parameter 1 (Szene 9)
#define ParamNEO_NEOSceneISlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot0)))
// Parameter 2 (Szene 9)
#define ParamNEO_NEOSceneISlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot1)))
// Parameter 3 (Szene 9)
#define ParamNEO_NEOSceneISlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot2)))
// Parameter 4 (Szene 9)
#define ParamNEO_NEOSceneISlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot3)))
// Parameter 5 (Szene 9)
#define ParamNEO_NEOSceneISlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot4)))
// Parameter 6 (Szene 9)
#define ParamNEO_NEOSceneISlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot5)))
// Parameter 7 (Szene 9)
#define ParamNEO_NEOSceneISlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot6)))
// Parameter 8 (Szene 9)
#define ParamNEO_NEOSceneISlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot7)))
// Parameter 9 (Szene 9)
#define ParamNEO_NEOSceneISlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot8)))
// Parameter 10 (Szene 9)
#define ParamNEO_NEOSceneISlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneISlot9)))
// Effekt-Text (Szene 9)
#define ParamNEO_NEOSceneIEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneIEffectText)))
#define ParamNEO_NEOSceneIEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneIEffectText), NEO_NEOSceneIEffectTextLength))
// Effekt
#define ParamNEO_NEOSceneJEffectType                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJEffectType)))
// Primärfarbe RGB
#define ParamNEO_NEOSceneJPrimaryColor               ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneJPrimaryColor)) & NEO_NEOSceneJPrimaryColorMask) >> NEO_NEOSceneJPrimaryColorShift)
// Primärfarbe Warmweiß
#define ParamNEO_NEOSceneJPrimaryWW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJPrimaryWW)))
// Primärfarbe Kaltweiß
#define ParamNEO_NEOSceneJPrimaryCW                  (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJPrimaryCW)))
// Sekundärfarbe RGB
#define ParamNEO_NEOSceneJSecondaryColor             ((knx.paramInt(NEO_ParamCalcIndex(NEO_NEOSceneJSecondaryColor)) & NEO_NEOSceneJSecondaryColorMask) >> NEO_NEOSceneJSecondaryColorShift)
// Sekundärfarbe Warmweiß
#define ParamNEO_NEOSceneJSecondaryWW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSecondaryWW)))
// Sekundärfarbe Kaltweiß
#define ParamNEO_NEOSceneJSecondaryCW                (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSecondaryCW)))
// Helligkeit
#define ParamNEO_NEOSceneJBrightness                 (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJBrightness)))
// Parameter 1 (Szene 10)
#define ParamNEO_NEOSceneJSlot0                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot0)))
// Parameter 2 (Szene 10)
#define ParamNEO_NEOSceneJSlot1                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot1)))
// Parameter 3 (Szene 10)
#define ParamNEO_NEOSceneJSlot2                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot2)))
// Parameter 4 (Szene 10)
#define ParamNEO_NEOSceneJSlot3                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot3)))
// Parameter 5 (Szene 10)
#define ParamNEO_NEOSceneJSlot4                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot4)))
// Parameter 6 (Szene 10)
#define ParamNEO_NEOSceneJSlot5                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot5)))
// Parameter 7 (Szene 10)
#define ParamNEO_NEOSceneJSlot6                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot6)))
// Parameter 8 (Szene 10)
#define ParamNEO_NEOSceneJSlot7                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot7)))
// Parameter 9 (Szene 10)
#define ParamNEO_NEOSceneJSlot8                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot8)))
// Parameter 10 (Szene 10)
#define ParamNEO_NEOSceneJSlot9                      (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOSceneJSlot9)))
// Effekt-Text (Szene 10)
#define ParamNEO_NEOSceneJEffectText                 (knx.paramData(NEO_ParamCalcIndex(NEO_NEOSceneJEffectText)))
#define ParamNEO_NEOSceneJEffectTextStr              (knx.paramString(NEO_ParamCalcIndex(NEO_NEOSceneJEffectText), NEO_NEOSceneJEffectTextLength))
// HCL-Quelle
#define ParamNEO_NEOHCLMode                          ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOHCLMode)) & NEO_NEOHCLModeMask) >> NEO_NEOHCLModeShift)
// Segment-Lichtmanager
#define ParamNEO_NEOHCLMaster                        (knx.paramByte(NEO_ParamCalcIndex(NEO_NEOHCLMaster)))
// Anwendung
#define ParamNEO_NEOHCLApplyMode                     ((knx.paramByte(NEO_ParamCalcIndex(NEO_NEOHCLApplyMode)) & NEO_NEOHCLApplyModeMask) >> NEO_NEOHCLApplyModeShift)

// deprecated
#define NEO_KoOffset 600

// Communication objects per channel (multiple occurrence)
#define NEO_KoBlockOffset 600
#define NEO_KoBlockSize 53

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
#define NEO_KoScene 17
#define NEO_KoSceneState 18
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
#define NEO_KoHCLState 39
#define NEO_KoCueRel 40
#define NEO_KoLock 41
#define NEO_KoEmStart 42
#define NEO_KoEmStatus 43
#define NEO_KoEmStop 44
#define NEO_KoEmCueStatus 45
#define NEO_KoEmCueSet 46
#define NEO_KoEffectText 47
#define NEO_KoEffectTextStatus 48
#define NEO_KoEffectTextAppend 49
#define NEO_KoSyncChain 50
#define NEO_KoEmPause 51
#define NEO_KoEmRunState 52

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
// Szenensteuerung
#define KoNEO_Scene                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoScene)))
// Szenensteuerung Status
#define KoNEO_SceneState                          (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSceneState)))
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
// Dimmen
#define KoNEO_BriRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoBriRel)))
// Rot dimmen
#define KoNEO_RRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRRel)))
// Grün dimmen
#define KoNEO_GRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoGRel)))
// Blau dimmen
#define KoNEO_BRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoBRel)))
// Weiß dimmen
#define KoNEO_WRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoWRel)))
// Warmweiß dimmen
#define KoNEO_WWRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoWWRel)))
// Kaltweiß dimmen
#define KoNEO_CWRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCWRel)))
// Farbton dimmen
#define KoNEO_HRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHRel)))
// Sättigung dimmen
#define KoNEO_SRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSRel)))
// Helligkeit (Value) dimmen
#define KoNEO_VRel                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoVRel)))
// Effekt schalten
#define KoNEO_FxRel                               (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoFxRel)))
// RGB dimmen
#define KoNEO_RGBRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBRel)))
// HSV dimmen
#define KoNEO_HSVRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHSVRel)))
// RGBW dimmen
#define KoNEO_RGBWRel                             (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoRGBWRel)))
// HCL Zustand
#define KoNEO_HCLState                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoHCLState)))
// Cue +/-
#define KoNEO_CueRel                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoCueRel)))
// Sperre
#define KoNEO_Lock                                (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoLock)))
// Effektmanager Start
#define KoNEO_EmStart                             (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmStart)))
// Effektmanager Status
#define KoNEO_EmStatus                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmStatus)))
// Effektmanager Stop
#define KoNEO_EmStop                              (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmStop)))
// Aktive Cue
#define KoNEO_EmCueStatus                         (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmCueStatus)))
// Cue setzen
#define KoNEO_EmCueSet                            (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmCueSet)))
// Effekt-Text
#define KoNEO_EffectText                          (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEffectText)))
// Effekt-Text Status
#define KoNEO_EffectTextStatus                    (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEffectTextStatus)))
// Effekt-Text Anhängen
#define KoNEO_EffectTextAppend                    (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEffectTextAppend)))
// Effektkette Sync
#define KoNEO_SyncChain                           (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoSyncChain)))
// Effektmanager Pause/Resume
#define KoNEO_EmPause                             (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmPause)))
// Effektmanager Zustand
#define KoNEO_EmRunState                          (knx.getGroupObject(NEO_KoCalcNumber(NEO_KoEmRunState)))

#define NEOSTRIP_ChannelCount 8

// Parameter per channel
#define NEOSTRIP_ParamBlockOffset 35467
#define NEOSTRIP_ParamBlockSize 24
#define NEOSTRIP_ParamCalcIndex(index) (index + NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize)

#define NEOSTRIP_NEOLEDType                           0      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOColourOrder                       1      // 4 Bits, Bit 7-4
#define     NEOSTRIP_NEOColourOrderMask 0xF0
#define     NEOSTRIP_NEOColourOrderShift 4
#define NEOSTRIP_NEOSwap                              1      // 3 Bits, Bit 3-1
#define     NEOSTRIP_NEOSwapMask 0x0E
#define     NEOSTRIP_NEOSwapShift 1
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
#define NEOSTRIP_NEOSPICLKMode                        5      // 1 Bit, Bit 4
#define     NEOSTRIP_NEOSPICLKModeMask 0x10
#define     NEOSTRIP_NEOSPICLKModeShift 4
#define NEOSTRIP_NEOGPIOManual                        5      // 1 Bit, Bit 3
#define     NEOSTRIP_NEOGPIOManualMask 0x08
#define     NEOSTRIP_NEOGPIOManualShift 3
#define NEOSTRIP_NEOGPIODataPortHW0                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW1                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW2                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW3                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW4                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW5                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW6                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW7                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW8                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW9                   5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW10                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW11                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW12                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW13                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW14                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW15                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW16                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW17                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW18                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW19                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW20                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW21                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW22                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW23                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW24                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW25                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW26                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW27                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW28                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW29                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW30                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIODataPortHW31                  5      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW0                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW1                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW2                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW3                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW4                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW5                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW6                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW7                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW8                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW9                  3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW10                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW11                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW12                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW13                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW14                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW15                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW16                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW17                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW18                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW19                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW20                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW21                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW22                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW23                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW24                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW25                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW26                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW27                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW28                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW29                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW30                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOGPIOClockPortHW31                 3      // 8 Bits, Bit 7-0
#define NEOSTRIP_NEOLength                            6      // 14 Bits, Bit 15-2
#define     NEOSTRIP_NEOLengthMask 0xFFFC
#define     NEOSTRIP_NEOLengthShift 2
#define NEOSTRIP_NEOSkipFirstLEDs                     8      // uint16_t
#define NEOSTRIP_NEOpowerLimitCombined               10      // 3 Bits, Bit 7-5
#define     NEOSTRIP_NEOpowerLimitCombinedMask 0xE0
#define     NEOSTRIP_NEOpowerLimitCombinedShift 5
#define NEOSTRIP_NEOTiming                           10      // 4 Bits, Bit 4-1
#define     NEOSTRIP_NEOTimingMask 0x1E
#define     NEOSTRIP_NEOTimingShift 1
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
#define NEOSTRIP_NEOColorCalibrationMaster           18      // 1 Bit, Bit 7
#define     NEOSTRIP_NEOColorCalibrationMasterMask 0x80
#define     NEOSTRIP_NEOColorCalibrationMasterShift 7
#define NEOSTRIP_NEOGammaCorrection                  18      // 1 Bit, Bit 6
#define     NEOSTRIP_NEOGammaCorrectionMask 0x40
#define     NEOSTRIP_NEOGammaCorrectionShift 6
#define NEOSTRIP_NEOGammaValue                       18      // 4 Bits, Bit 5-2
#define     NEOSTRIP_NEOGammaValueMask 0x3C
#define     NEOSTRIP_NEOGammaValueShift 2
#define NEOSTRIP_NEOVoltage                          18      // 2 Bits, Bit 1-0
#define     NEOSTRIP_NEOVoltageMask 0x03
#define     NEOSTRIP_NEOVoltageShift 0
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
#define NEOSTRIP_NEOClockGPIOManual                  23      // uint8_t

// LED Typ
#define ParamNEOSTRIP_NEOLEDType                          (PT_NEOLedType)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOLEDType)))
// LED Farbordnung
#define ParamNEOSTRIP_NEOColourOrder                      (PT_NEOColourOrder)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOColourOrder)) & NEOSTRIP_NEOColourOrderMask) >> NEOSTRIP_NEOColourOrderShift)
// Kanal-Tausch
#define ParamNEOSTRIP_NEOSwap                             (PT_NEOSwap)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSwap)) & NEOSTRIP_NEOSwapMask) >> NEOSTRIP_NEOSwapShift)
// Daten GPIO
#define ParamNEOSTRIP_NEODataGPIO                         ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEODataGPIO)) & NEOSTRIP_NEODataGPIOMask) >> NEOSTRIP_NEODataGPIOShift)
// Clock GPIO
#define ParamNEOSTRIP_NEOClockGPIO                        ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOClockGPIO)) & NEOSTRIP_NEOClockGPIOMask) >> NEOSTRIP_NEOClockGPIOShift)
// Daten GPIO
#define ParamNEOSTRIP_NEOSPIMOSIGPIO                      ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPIMOSIGPIO)) & NEOSTRIP_NEOSPIMOSIGPIOMask) >> NEOSTRIP_NEOSPIMOSIGPIOShift)
// SPI Clock
#define ParamNEOSTRIP_NEOSPICLK                           (PT_NEOSPICLK)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPICLK)) & NEOSTRIP_NEOSPICLKMask) >> NEOSTRIP_NEOSPICLKShift)
// SPI Takt
#define ParamNEOSTRIP_NEOSPICLKMode                       ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSPICLKMode)) & NEOSTRIP_NEOSPICLKModeMask))
// GPIO manuell konfigurieren
#define ParamNEOSTRIP_NEOGPIOManual                       ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOManual)) & NEOSTRIP_NEOGPIOManualMask))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW0                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW0)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW1                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW1)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW2                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW2)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW3                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW3)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW4                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW4)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW5                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW5)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW6                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW6)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW7                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW7)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW8                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW8)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW9                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW9)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW10                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW10)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW11                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW11)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW12                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW12)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW13                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW13)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW14                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW14)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW15                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW15)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW16                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW16)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW17                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW17)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW18                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW18)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW19                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW19)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW20                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW20)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW21                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW21)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW22                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW22)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW23                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW23)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW24                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW24)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW25                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW25)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW26                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW26)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW27                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW27)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW28                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW28)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW29                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW29)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW30                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW30)))
// Daten Port
#define ParamNEOSTRIP_NEOGPIODataPortHW31                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIODataPortHW31)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW0                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW0)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW1                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW1)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW2                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW2)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW3                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW3)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW4                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW4)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW5                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW5)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW6                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW6)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW7                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW7)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW8                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW8)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW9                 (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW9)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW10                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW10)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW11                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW11)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW12                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW12)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW13                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW13)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW14                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW14)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW15                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW15)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW16                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW16)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW17                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW17)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW18                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW18)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW19                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW19)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW20                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW20)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW21                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW21)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW22                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW22)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW23                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW23)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW24                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW24)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW25                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW25)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW26                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW26)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW27                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW27)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW28                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW28)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW29                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW29)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW30                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW30)))
// Clock Port
#define ParamNEOSTRIP_NEOGPIOClockPortHW31                (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGPIOClockPortHW31)))
// Anzahl an LEDs/LEDs-Gruppen
#define ParamNEOSTRIP_NEOLength                           ((knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOLength)) & NEOSTRIP_NEOLengthMask) >> NEOSTRIP_NEOLengthShift)
// Überspringe erste LEDs
#define ParamNEOSTRIP_NEOSkipFirstLEDs                    (knx.paramWord(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOSkipFirstLEDs)))
// Strombegrenzung
#define ParamNEOSTRIP_NEOpowerLimitCombined               (PT_NEOPowerLimitCombinedStrip)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOpowerLimitCombined)) & NEOSTRIP_NEOpowerLimitCombinedMask) >> NEOSTRIP_NEOpowerLimitCombinedShift)
// Timing
#define ParamNEOSTRIP_NEOTiming                           (PT_NEOTiming)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOTiming)) & NEOSTRIP_NEOTimingMask) >> NEOSTRIP_NEOTimingShift)
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
// Farbkalibrierung
#define ParamNEOSTRIP_NEOColorCalibrationMaster           ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOColorCalibrationMaster)) & NEOSTRIP_NEOColorCalibrationMasterMask))
// Gamma Korrektur (empfohlen!)
#define ParamNEOSTRIP_NEOGammaCorrection                  ((bool)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGammaCorrection)) & NEOSTRIP_NEOGammaCorrectionMask))
// Gamma Wert
#define ParamNEOSTRIP_NEOGammaValue                       (PT_NEOGammaValue)((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOGammaValue)) & NEOSTRIP_NEOGammaValueMask) >> NEOSTRIP_NEOGammaValueShift)
// Versorgungsspannung
#define ParamNEOSTRIP_NEOVoltage                          (PT_NEOVoltage)(knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOVoltage)) & NEOSTRIP_NEOVoltageMask)
// Anstiegszeit ABL (%)
#define ParamNEOSTRIP_NEOablSlewRatePercent               ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOablSlewRatePercent)) & NEOSTRIP_NEOablSlewRatePercentMask) >> NEOSTRIP_NEOablSlewRatePercentShift)
// Weißabgleich Rot (%)
#define ParamNEOSTRIP_NEOWhiteBalanceRed                  ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceRed)) & NEOSTRIP_NEOWhiteBalanceRedMask) >> NEOSTRIP_NEOWhiteBalanceRedShift)
// Weißabgleich Grün (%)
#define ParamNEOSTRIP_NEOWhiteBalanceGreen                ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceGreen)) & NEOSTRIP_NEOWhiteBalanceGreenMask) >> NEOSTRIP_NEOWhiteBalanceGreenShift)
// Weißabgleich Blau (%)
#define ParamNEOSTRIP_NEOWhiteBalanceBlue                 ((knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOWhiteBalanceBlue)) & NEOSTRIP_NEOWhiteBalanceBlueMask) >> NEOSTRIP_NEOWhiteBalanceBlueShift)
// Clock GPIO Pin
#define ParamNEOSTRIP_NEOClockGPIOManual                  (knx.paramByte(NEOSTRIP_ParamCalcIndex(NEOSTRIP_NEOClockGPIOManual)))

// deprecated
#define NEOSTRIP_KoOffset 500

// Communication objects per channel (multiple occurrence)
#define NEOSTRIP_KoBlockOffset 500
#define NEOSTRIP_KoBlockSize 8

#define NEOSTRIP_KoCalcNumber(index) (index + NEOSTRIP_KoBlockOffset + _channelIndex * NEOSTRIP_KoBlockSize)
#define NEOSTRIP_KoCalcIndex(number) ((number >= NEOSTRIP_KoCalcNumber(0) && number < NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBlockSize)) ? (number - NEOSTRIP_KoBlockOffset) % NEOSTRIP_KoBlockSize : -1)
#define NEOSTRIP_KoCalcChannel(number) ((number >= NEOSTRIP_KoBlockOffset && number < NEOSTRIP_KoBlockOffset + NEOSTRIP_ChannelCount * NEOSTRIP_KoBlockSize) ? (number - NEOSTRIP_KoBlockOffset) / NEOSTRIP_KoBlockSize : -1)

#define NEOSTRIP_KoPower 0
#define NEOSTRIP_KoPowerState 1
#define NEOSTRIP_KoBrightness 3
#define NEOSTRIP_KoBrightnessState 4
#define NEOSTRIP_KoPowerCurrent 5
#define NEOSTRIP_KoPowerLoad 6
#define NEOSTRIP_KoPowerWatts 7

// Ein
#define KoNEOSTRIP_Power                               (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPower)))
// Ein Status
#define KoNEOSTRIP_PowerState                          (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPowerState)))
// Helligkeit
#define KoNEOSTRIP_Brightness                          (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBrightness)))
// Helligkeit Status
#define KoNEOSTRIP_BrightnessState                     (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoBrightnessState)))
// Stromverbrauch
#define KoNEOSTRIP_PowerCurrent                        (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPowerCurrent)))
// Auslastung
#define KoNEOSTRIP_PowerLoad                           (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPowerLoad)))
// Leistung
#define KoNEOSTRIP_PowerWatts                          (knx.getGroupObject(NEOSTRIP_KoCalcNumber(NEOSTRIP_KoPowerWatts)))

#define NEOEM_ChannelCount 16

// Parameter per channel
#define NEOEM_ParamBlockOffset 35659
#define NEOEM_ParamBlockSize 500
#define NEOEM_ParamCalcIndex(index) (index + NEOEM_ParamBlockOffset + _channelIndex * NEOEM_ParamBlockSize)

#define NEOEM_NEOEMCueCount                       16      // uint8_t
#define NEOEM_NEOEMLoop                           17      // 1 Bit, Bit 7
#define     NEOEM_NEOEMLoopMask 0x80
#define     NEOEM_NEOEMLoopShift 7
#define NEOEM_NEOEMTextKOs                        17      // 1 Bit, Bit 6
#define     NEOEM_NEOEMTextKOsMask 0x40
#define     NEOEM_NEOEMTextKOsShift 6
#define NEOEM_NEOEMNext                           18      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMEnabled                        19      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Effect                     20      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Param0                     21      // uint8_t
#define NEOEM_NEOEMCue1Param1                     22      // uint8_t
#define NEOEM_NEOEMCue1Param2                     23      // uint8_t
#define NEOEM_NEOEMCue1Param3                     24      // uint8_t
#define NEOEM_NEOEMCue1Param4                     25      // uint8_t
#define NEOEM_NEOEMCue1Param5                     26      // uint8_t
#define NEOEM_NEOEMCue1Param6                     27      // uint8_t
#define NEOEM_NEOEMCue1Param7                     28      // uint8_t
#define NEOEM_NEOEMCue1Param8                     29      // uint8_t
#define NEOEM_NEOEMCue1Param9                     30      // uint8_t
#define NEOEM_NEOEMCue1Color                      31      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue1ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue1ColorShift 8
#define NEOEM_NEOEMCue1W                          34      // uint8_t
#define NEOEM_NEOEMCue1Brightness                 35      // uint8_t
#define NEOEM_NEOEMCue1Duration                   36      // uint16_t
#define NEOEM_NEOEMCue1Fade                       38      // uint16_t
#define NEOEM_NEOEMCue1Text                       40      // char*, 14 Byte
#define     NEOEM_NEOEMCue1TextLength 14
#define NEOEM_NEOEMCue1EffectText                 54      // char*, 14 Byte
#define     NEOEM_NEOEMCue1EffectTextLength 14
#define NEOEM_NEOEMCue1WipeSpeed                  21      // uint8_t
#define NEOEM_NEOEMCue1WipeDirection              22      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1RainbowSpeed               21      // uint8_t
#define NEOEM_NEOEMCue1RainbowDelta               22      // uint8_t
#define NEOEM_NEOEMCue1RainbowSaturation          23      // uint8_t
#define NEOEM_NEOEMCue1RainbowDensity             24      // uint8_t
#define NEOEM_NEOEMCue1RainbowMode                25      // uint8_t
#define NEOEM_NEOEMCue1Pride2015Speed             21      // uint8_t
#define NEOEM_NEOEMCue1JuggleSpeed                21      // uint8_t
#define NEOEM_NEOEMCue1JuggleNumDots              22      // uint8_t
#define NEOEM_NEOEMCue1JuggleFadeSpeed            23      // uint8_t
#define NEOEM_NEOEMCue1JuggleHueOffset            24      // uint8_t
#define NEOEM_NEOEMCue1BPMBPM                     21      // uint8_t
#define NEOEM_NEOEMCue1BPMHue                     22      // uint8_t
#define NEOEM_NEOEMCue1CylonSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1CylonHue                   22      // uint8_t
#define NEOEM_NEOEMCue1CylonEyeSize               23      // uint8_t
#define NEOEM_NEOEMCue1CylonFadeAmount            24      // uint8_t
#define NEOEM_NEOEMCue1CylonDirection             25      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1CylonMode                  26      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1TestPhaseDuration          21      // uint8_t
#define NEOEM_NEOEMCue1TestMode                   22      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1FireSpeed                  21      // uint8_t
#define NEOEM_NEOEMCue1FireCooling                22      // uint8_t
#define NEOEM_NEOEMCue1FireSparking               23      // uint8_t
#define NEOEM_NEOEMCue1FireReverseDirection       24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue1FireReverseDirectionShift 7
#define NEOEM_NEOEMCue1FireBlueFireMode           25      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue1FireBlueFireModeShift 7
#define NEOEM_NEOEMCue1TheaterChaseSpeed          21      // uint8_t
#define NEOEM_NEOEMCue1TheaterChaseSpacing        22      // uint8_t
#define NEOEM_NEOEMCue1TheaterChaseDotSize        23      // uint8_t
#define NEOEM_NEOEMCue1TheaterChaseColorMode      24      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1TheaterChaseColorSpeed     25      // uint8_t
#define NEOEM_NEOEMCue1TheaterChaseBounce         26      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1SparkleSpeed               21      // uint8_t
#define NEOEM_NEOEMCue1SparkleFadeRate            22      // uint8_t
#define NEOEM_NEOEMCue1SparkleSparkleCount        23      // uint8_t
#define NEOEM_NEOEMCue1SparkleProbability         24      // uint8_t
#define NEOEM_NEOEMCue1SparkleWhiteOnly           25      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue1SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue1SparkleBurstMode           26      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue1SparkleBurstModeShift 7
#define NEOEM_NEOEMCue1SparkleMode                27      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1BreathingSpeed             21      // uint8_t
#define NEOEM_NEOEMCue1BreathingMinBrightness     22      // uint8_t
#define NEOEM_NEOEMCue1BreathingCurve             23      // uint8_t
#define NEOEM_NEOEMCue1BreathingRainbowMode       24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue1BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue1BreathingWaveform          25      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1StrobeSpeed                21      // uint8_t
#define NEOEM_NEOEMCue1StrobeOnRatio              22      // uint8_t
#define NEOEM_NEOEMCue1StrobeMinBrightness        23      // uint8_t
#define NEOEM_NEOEMCue1StrobeRandomTiming         24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue1StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue1StrobeRainbowStrobe        25      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue1StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue1CometSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1CometFadeRate              22      // uint8_t
#define NEOEM_NEOEMCue1CometTailLength            23      // uint8_t
#define NEOEM_NEOEMCue1CometBounceMode            24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue1CometBounceModeShift 7
#define NEOEM_NEOEMCue1CometRainbowMode           25      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue1CometRainbowModeShift 7
#define NEOEM_NEOEMCue1CometMode                  26      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1NoiseSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1NoiseScale                 22      // uint8_t
#define NEOEM_NEOEMCue1NoiseSaturation            23      // uint8_t
#define NEOEM_NEOEMCue1NoiseHueOffset             24      // uint8_t
#define NEOEM_NEOEMCue1NoisePalette               25      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1PaletteSpeed               21      // uint8_t
#define NEOEM_NEOEMCue1PalettePalette             22      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1PaletteBlend               23      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue1PaletteBlendShift 7
#define NEOEM_NEOEMCue1PaletteSpacing             24      // uint8_t
#define NEOEM_NEOEMCue1BlitzSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1BlitzWidth                 22      // uint8_t
#define NEOEM_NEOEMCue1BlitzDecay                 23      // uint8_t
#define NEOEM_NEOEMCue1BlitzHue                   24      // uint8_t
#define NEOEM_NEOEMCue1BlitzIntensity             25      // uint8_t
#define NEOEM_NEOEMCue1GradientSpeed              21      // uint8_t
#define NEOEM_NEOEMCue1GradientStartHue           22      // uint8_t
#define NEOEM_NEOEMCue1GradientEndHue             23      // uint8_t
#define NEOEM_NEOEMCue1GradientSaturation         24      // uint8_t
#define NEOEM_NEOEMCue1KerzeSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1KerzeIntensity             22      // uint8_t
#define NEOEM_NEOEMCue1KerzeZones                 23      // uint8_t
#define NEOEM_NEOEMCue1LaufschriftSpeed           21      // uint8_t
#define NEOEM_NEOEMCue1LaufschriftGap             22      // uint8_t
#define NEOEM_NEOEMCue1LaufschriftLoop            23      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue1LaufschriftLoopShift 7
#define NEOEM_NEOEMCue1LaufschriftText            54      // char*, 14 Byte
#define     NEOEM_NEOEMCue1LaufschriftTextLength 14
#define NEOEM_NEOEMCue1LaufschriftFont            25      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Uhr2DViewMode              21      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Uhr2DShowSeconds           22      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue1Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue1Uhr2DBlinkColon            23      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue1Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue1Uhr2DColourHue             24      // uint8_t
#define NEOEM_NEOEMCue1Uhr2DDateMode              25      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Uhr2DDateFormat            26      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Uhr2DDateHue               27      // uint8_t
#define NEOEM_NEOEMCue1Uhr2DSwitchSec             28      // uint8_t
#define NEOEM_NEOEMCue1Uhr2DScrollSpeed           29      // uint8_t
#define NEOEM_NEOEMCue1Uhr2DFont                  30      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Snake2DSpeed               21      // uint8_t
#define NEOEM_NEOEMCue1Snake2DHeadHue             22      // uint8_t
#define NEOEM_NEOEMCue1Snake2DBodyMode            23      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Snake2DBodyHue             24      // uint8_t
#define NEOEM_NEOEMCue1Matrix2DSpeed              21      // uint8_t
#define NEOEM_NEOEMCue1Matrix2DDensity            22      // uint8_t
#define NEOEM_NEOEMCue1Matrix2DPalette            23      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Matrix2DGlitch             24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue1Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue1Tetris2DSpeed              21      // uint8_t
#define NEOEM_NEOEMCue1Tetris2DBgBrightness       22      // uint8_t
#define NEOEM_NEOEMCue1Tetris2DGhostPiece         23      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue1Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue1Tetris2DColorMode          24      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1Tetris2DFlashLines         25      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue1Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue1Tetris2DAutoPlay           26      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1TRONSpeed                  21      // uint8_t
#define NEOEM_NEOEMCue1TRONHue                    22      // uint8_t
#define NEOEM_NEOEMCue1TRONGridSpacing            23      // uint8_t
#define NEOEM_NEOEMCue1TRONGlow                   24      // uint8_t
#define NEOEM_NEOEMCue1StarfieldWarpSpeed         21      // uint8_t
#define NEOEM_NEOEMCue1StarfieldWarpDensity       22      // uint8_t
#define NEOEM_NEOEMCue1StarfieldWarpColorMode     23      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue1StarfieldWarpWarpPulse     24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue1StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue1PlasmaNebulaSpeed          21      // uint8_t
#define NEOEM_NEOEMCue1PlasmaNebulaSaturation     22      // uint8_t
#define NEOEM_NEOEMCue1PlasmaNebulaContrast       23      // uint8_t
#define NEOEM_NEOEMCue1PlasmaNebulaPaletteShift   24      // uint8_t
#define NEOEM_NEOEMCue1UFOSwarmSpeed              21      // uint8_t
#define NEOEM_NEOEMCue1UFOSwarmCount              22      // uint8_t
#define NEOEM_NEOEMCue1UFOSwarmHue                23      // uint8_t
#define NEOEM_NEOEMCue1UFOSwarmBeam               24      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue1UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue1UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue1GameofLife2DSpeed          21      // uint8_t
#define NEOEM_NEOEMCue1GameofLife2DHue            22      // uint8_t
#define NEOEM_NEOEMCue1GameofLife2DColorMode      23      // uint8_t
#define NEOEM_NEOEMCue1GameofLife2DSpawnDensity   24      // uint8_t
#define NEOEM_NEOEMCue1DNA2DSpeed                 21      // uint8_t
#define NEOEM_NEOEMCue1DNA2DHue                   22      // uint8_t
#define NEOEM_NEOEMCue1DNA2DTwist                 23      // uint8_t
#define NEOEM_NEOEMCue1DNA2DRungSpacing           24      // uint8_t
#define NEOEM_NEOEMCue1Aurora2DSpeed              21      // uint8_t
#define NEOEM_NEOEMCue1Aurora2DHue                22      // uint8_t
#define NEOEM_NEOEMCue1Aurora2DScale              23      // uint8_t
#define NEOEM_NEOEMCue1Aurora2DIntensity          24      // uint8_t
#define NEOEM_NEOEMCue1Lissajous2DSpeed           21      // uint8_t
#define NEOEM_NEOEMCue1Lissajous2DHue             22      // uint8_t
#define NEOEM_NEOEMCue1Lissajous2DFreqA           23      // uint8_t
#define NEOEM_NEOEMCue1Lissajous2DFreqB           24      // uint8_t
#define NEOEM_NEOEMCue1Metaballs2DSpeed           21      // uint8_t
#define NEOEM_NEOEMCue1Metaballs2DHue             22      // uint8_t
#define NEOEM_NEOEMCue1Metaballs2DBlobCount       23      // uint8_t
#define NEOEM_NEOEMCue1Metaballs2DContrast        24      // uint8_t
#define NEOEM_NEOEMCue2Effect                     68      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Param0                     69      // uint8_t
#define NEOEM_NEOEMCue2Param1                     70      // uint8_t
#define NEOEM_NEOEMCue2Param2                     71      // uint8_t
#define NEOEM_NEOEMCue2Param3                     72      // uint8_t
#define NEOEM_NEOEMCue2Param4                     73      // uint8_t
#define NEOEM_NEOEMCue2Param5                     74      // uint8_t
#define NEOEM_NEOEMCue2Param6                     75      // uint8_t
#define NEOEM_NEOEMCue2Param7                     76      // uint8_t
#define NEOEM_NEOEMCue2Param8                     77      // uint8_t
#define NEOEM_NEOEMCue2Param9                     78      // uint8_t
#define NEOEM_NEOEMCue2Color                      79      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue2ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue2ColorShift 8
#define NEOEM_NEOEMCue2W                          82      // uint8_t
#define NEOEM_NEOEMCue2Brightness                 83      // uint8_t
#define NEOEM_NEOEMCue2Duration                   84      // uint16_t
#define NEOEM_NEOEMCue2Fade                       86      // uint16_t
#define NEOEM_NEOEMCue2Text                       88      // char*, 14 Byte
#define     NEOEM_NEOEMCue2TextLength 14
#define NEOEM_NEOEMCue2EffectText                 102      // char*, 14 Byte
#define     NEOEM_NEOEMCue2EffectTextLength 14
#define NEOEM_NEOEMCue2WipeSpeed                  69      // uint8_t
#define NEOEM_NEOEMCue2WipeDirection              70      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2RainbowSpeed               69      // uint8_t
#define NEOEM_NEOEMCue2RainbowDelta               70      // uint8_t
#define NEOEM_NEOEMCue2RainbowSaturation          71      // uint8_t
#define NEOEM_NEOEMCue2RainbowDensity             72      // uint8_t
#define NEOEM_NEOEMCue2RainbowMode                73      // uint8_t
#define NEOEM_NEOEMCue2Pride2015Speed             69      // uint8_t
#define NEOEM_NEOEMCue2JuggleSpeed                69      // uint8_t
#define NEOEM_NEOEMCue2JuggleNumDots              70      // uint8_t
#define NEOEM_NEOEMCue2JuggleFadeSpeed            71      // uint8_t
#define NEOEM_NEOEMCue2JuggleHueOffset            72      // uint8_t
#define NEOEM_NEOEMCue2BPMBPM                     69      // uint8_t
#define NEOEM_NEOEMCue2BPMHue                     70      // uint8_t
#define NEOEM_NEOEMCue2CylonSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2CylonHue                   70      // uint8_t
#define NEOEM_NEOEMCue2CylonEyeSize               71      // uint8_t
#define NEOEM_NEOEMCue2CylonFadeAmount            72      // uint8_t
#define NEOEM_NEOEMCue2CylonDirection             73      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2CylonMode                  74      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2TestPhaseDuration          69      // uint8_t
#define NEOEM_NEOEMCue2TestMode                   70      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2FireSpeed                  69      // uint8_t
#define NEOEM_NEOEMCue2FireCooling                70      // uint8_t
#define NEOEM_NEOEMCue2FireSparking               71      // uint8_t
#define NEOEM_NEOEMCue2FireReverseDirection       72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue2FireReverseDirectionShift 7
#define NEOEM_NEOEMCue2FireBlueFireMode           73      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue2FireBlueFireModeShift 7
#define NEOEM_NEOEMCue2TheaterChaseSpeed          69      // uint8_t
#define NEOEM_NEOEMCue2TheaterChaseSpacing        70      // uint8_t
#define NEOEM_NEOEMCue2TheaterChaseDotSize        71      // uint8_t
#define NEOEM_NEOEMCue2TheaterChaseColorMode      72      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2TheaterChaseColorSpeed     73      // uint8_t
#define NEOEM_NEOEMCue2TheaterChaseBounce         74      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2SparkleSpeed               69      // uint8_t
#define NEOEM_NEOEMCue2SparkleFadeRate            70      // uint8_t
#define NEOEM_NEOEMCue2SparkleSparkleCount        71      // uint8_t
#define NEOEM_NEOEMCue2SparkleProbability         72      // uint8_t
#define NEOEM_NEOEMCue2SparkleWhiteOnly           73      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue2SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue2SparkleBurstMode           74      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue2SparkleBurstModeShift 7
#define NEOEM_NEOEMCue2SparkleMode                75      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2BreathingSpeed             69      // uint8_t
#define NEOEM_NEOEMCue2BreathingMinBrightness     70      // uint8_t
#define NEOEM_NEOEMCue2BreathingCurve             71      // uint8_t
#define NEOEM_NEOEMCue2BreathingRainbowMode       72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue2BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue2BreathingWaveform          73      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2StrobeSpeed                69      // uint8_t
#define NEOEM_NEOEMCue2StrobeOnRatio              70      // uint8_t
#define NEOEM_NEOEMCue2StrobeMinBrightness        71      // uint8_t
#define NEOEM_NEOEMCue2StrobeRandomTiming         72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue2StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue2StrobeRainbowStrobe        73      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue2StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue2CometSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2CometFadeRate              70      // uint8_t
#define NEOEM_NEOEMCue2CometTailLength            71      // uint8_t
#define NEOEM_NEOEMCue2CometBounceMode            72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue2CometBounceModeShift 7
#define NEOEM_NEOEMCue2CometRainbowMode           73      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue2CometRainbowModeShift 7
#define NEOEM_NEOEMCue2CometMode                  74      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2NoiseSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2NoiseScale                 70      // uint8_t
#define NEOEM_NEOEMCue2NoiseSaturation            71      // uint8_t
#define NEOEM_NEOEMCue2NoiseHueOffset             72      // uint8_t
#define NEOEM_NEOEMCue2NoisePalette               73      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2PaletteSpeed               69      // uint8_t
#define NEOEM_NEOEMCue2PalettePalette             70      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2PaletteBlend               71      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue2PaletteBlendShift 7
#define NEOEM_NEOEMCue2PaletteSpacing             72      // uint8_t
#define NEOEM_NEOEMCue2BlitzSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2BlitzWidth                 70      // uint8_t
#define NEOEM_NEOEMCue2BlitzDecay                 71      // uint8_t
#define NEOEM_NEOEMCue2BlitzHue                   72      // uint8_t
#define NEOEM_NEOEMCue2BlitzIntensity             73      // uint8_t
#define NEOEM_NEOEMCue2GradientSpeed              69      // uint8_t
#define NEOEM_NEOEMCue2GradientStartHue           70      // uint8_t
#define NEOEM_NEOEMCue2GradientEndHue             71      // uint8_t
#define NEOEM_NEOEMCue2GradientSaturation         72      // uint8_t
#define NEOEM_NEOEMCue2KerzeSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2KerzeIntensity             70      // uint8_t
#define NEOEM_NEOEMCue2KerzeZones                 71      // uint8_t
#define NEOEM_NEOEMCue2LaufschriftSpeed           69      // uint8_t
#define NEOEM_NEOEMCue2LaufschriftGap             70      // uint8_t
#define NEOEM_NEOEMCue2LaufschriftLoop            71      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue2LaufschriftLoopShift 7
#define NEOEM_NEOEMCue2LaufschriftText            102      // char*, 14 Byte
#define     NEOEM_NEOEMCue2LaufschriftTextLength 14
#define NEOEM_NEOEMCue2LaufschriftFont            73      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Uhr2DViewMode              69      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Uhr2DShowSeconds           70      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue2Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue2Uhr2DBlinkColon            71      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue2Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue2Uhr2DColourHue             72      // uint8_t
#define NEOEM_NEOEMCue2Uhr2DDateMode              73      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Uhr2DDateFormat            74      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Uhr2DDateHue               75      // uint8_t
#define NEOEM_NEOEMCue2Uhr2DSwitchSec             76      // uint8_t
#define NEOEM_NEOEMCue2Uhr2DScrollSpeed           77      // uint8_t
#define NEOEM_NEOEMCue2Uhr2DFont                  78      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Snake2DSpeed               69      // uint8_t
#define NEOEM_NEOEMCue2Snake2DHeadHue             70      // uint8_t
#define NEOEM_NEOEMCue2Snake2DBodyMode            71      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Snake2DBodyHue             72      // uint8_t
#define NEOEM_NEOEMCue2Matrix2DSpeed              69      // uint8_t
#define NEOEM_NEOEMCue2Matrix2DDensity            70      // uint8_t
#define NEOEM_NEOEMCue2Matrix2DPalette            71      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Matrix2DGlitch             72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue2Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue2Tetris2DSpeed              69      // uint8_t
#define NEOEM_NEOEMCue2Tetris2DBgBrightness       70      // uint8_t
#define NEOEM_NEOEMCue2Tetris2DGhostPiece         71      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue2Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue2Tetris2DColorMode          72      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2Tetris2DFlashLines         73      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue2Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue2Tetris2DAutoPlay           74      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2TRONSpeed                  69      // uint8_t
#define NEOEM_NEOEMCue2TRONHue                    70      // uint8_t
#define NEOEM_NEOEMCue2TRONGridSpacing            71      // uint8_t
#define NEOEM_NEOEMCue2TRONGlow                   72      // uint8_t
#define NEOEM_NEOEMCue2StarfieldWarpSpeed         69      // uint8_t
#define NEOEM_NEOEMCue2StarfieldWarpDensity       70      // uint8_t
#define NEOEM_NEOEMCue2StarfieldWarpColorMode     71      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue2StarfieldWarpWarpPulse     72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue2StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue2PlasmaNebulaSpeed          69      // uint8_t
#define NEOEM_NEOEMCue2PlasmaNebulaSaturation     70      // uint8_t
#define NEOEM_NEOEMCue2PlasmaNebulaContrast       71      // uint8_t
#define NEOEM_NEOEMCue2PlasmaNebulaPaletteShift   72      // uint8_t
#define NEOEM_NEOEMCue2UFOSwarmSpeed              69      // uint8_t
#define NEOEM_NEOEMCue2UFOSwarmCount              70      // uint8_t
#define NEOEM_NEOEMCue2UFOSwarmHue                71      // uint8_t
#define NEOEM_NEOEMCue2UFOSwarmBeam               72      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue2UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue2UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue2GameofLife2DSpeed          69      // uint8_t
#define NEOEM_NEOEMCue2GameofLife2DHue            70      // uint8_t
#define NEOEM_NEOEMCue2GameofLife2DColorMode      71      // uint8_t
#define NEOEM_NEOEMCue2GameofLife2DSpawnDensity   72      // uint8_t
#define NEOEM_NEOEMCue2DNA2DSpeed                 69      // uint8_t
#define NEOEM_NEOEMCue2DNA2DHue                   70      // uint8_t
#define NEOEM_NEOEMCue2DNA2DTwist                 71      // uint8_t
#define NEOEM_NEOEMCue2DNA2DRungSpacing           72      // uint8_t
#define NEOEM_NEOEMCue2Aurora2DSpeed              69      // uint8_t
#define NEOEM_NEOEMCue2Aurora2DHue                70      // uint8_t
#define NEOEM_NEOEMCue2Aurora2DScale              71      // uint8_t
#define NEOEM_NEOEMCue2Aurora2DIntensity          72      // uint8_t
#define NEOEM_NEOEMCue2Lissajous2DSpeed           69      // uint8_t
#define NEOEM_NEOEMCue2Lissajous2DHue             70      // uint8_t
#define NEOEM_NEOEMCue2Lissajous2DFreqA           71      // uint8_t
#define NEOEM_NEOEMCue2Lissajous2DFreqB           72      // uint8_t
#define NEOEM_NEOEMCue2Metaballs2DSpeed           69      // uint8_t
#define NEOEM_NEOEMCue2Metaballs2DHue             70      // uint8_t
#define NEOEM_NEOEMCue2Metaballs2DBlobCount       71      // uint8_t
#define NEOEM_NEOEMCue2Metaballs2DContrast        72      // uint8_t
#define NEOEM_NEOEMCue3Effect                     116      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Param0                     117      // uint8_t
#define NEOEM_NEOEMCue3Param1                     118      // uint8_t
#define NEOEM_NEOEMCue3Param2                     119      // uint8_t
#define NEOEM_NEOEMCue3Param3                     120      // uint8_t
#define NEOEM_NEOEMCue3Param4                     121      // uint8_t
#define NEOEM_NEOEMCue3Param5                     122      // uint8_t
#define NEOEM_NEOEMCue3Param6                     123      // uint8_t
#define NEOEM_NEOEMCue3Param7                     124      // uint8_t
#define NEOEM_NEOEMCue3Param8                     125      // uint8_t
#define NEOEM_NEOEMCue3Param9                     126      // uint8_t
#define NEOEM_NEOEMCue3Color                      127      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue3ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue3ColorShift 8
#define NEOEM_NEOEMCue3W                          130      // uint8_t
#define NEOEM_NEOEMCue3Brightness                 131      // uint8_t
#define NEOEM_NEOEMCue3Duration                   132      // uint16_t
#define NEOEM_NEOEMCue3Fade                       134      // uint16_t
#define NEOEM_NEOEMCue3Text                       136      // char*, 14 Byte
#define     NEOEM_NEOEMCue3TextLength 14
#define NEOEM_NEOEMCue3EffectText                 150      // char*, 14 Byte
#define     NEOEM_NEOEMCue3EffectTextLength 14
#define NEOEM_NEOEMCue3WipeSpeed                  117      // uint8_t
#define NEOEM_NEOEMCue3WipeDirection              118      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3RainbowSpeed               117      // uint8_t
#define NEOEM_NEOEMCue3RainbowDelta               118      // uint8_t
#define NEOEM_NEOEMCue3RainbowSaturation          119      // uint8_t
#define NEOEM_NEOEMCue3RainbowDensity             120      // uint8_t
#define NEOEM_NEOEMCue3RainbowMode                121      // uint8_t
#define NEOEM_NEOEMCue3Pride2015Speed             117      // uint8_t
#define NEOEM_NEOEMCue3JuggleSpeed                117      // uint8_t
#define NEOEM_NEOEMCue3JuggleNumDots              118      // uint8_t
#define NEOEM_NEOEMCue3JuggleFadeSpeed            119      // uint8_t
#define NEOEM_NEOEMCue3JuggleHueOffset            120      // uint8_t
#define NEOEM_NEOEMCue3BPMBPM                     117      // uint8_t
#define NEOEM_NEOEMCue3BPMHue                     118      // uint8_t
#define NEOEM_NEOEMCue3CylonSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3CylonHue                   118      // uint8_t
#define NEOEM_NEOEMCue3CylonEyeSize               119      // uint8_t
#define NEOEM_NEOEMCue3CylonFadeAmount            120      // uint8_t
#define NEOEM_NEOEMCue3CylonDirection             121      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3CylonMode                  122      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3TestPhaseDuration          117      // uint8_t
#define NEOEM_NEOEMCue3TestMode                   118      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3FireSpeed                  117      // uint8_t
#define NEOEM_NEOEMCue3FireCooling                118      // uint8_t
#define NEOEM_NEOEMCue3FireSparking               119      // uint8_t
#define NEOEM_NEOEMCue3FireReverseDirection       120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue3FireReverseDirectionShift 7
#define NEOEM_NEOEMCue3FireBlueFireMode           121      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue3FireBlueFireModeShift 7
#define NEOEM_NEOEMCue3TheaterChaseSpeed          117      // uint8_t
#define NEOEM_NEOEMCue3TheaterChaseSpacing        118      // uint8_t
#define NEOEM_NEOEMCue3TheaterChaseDotSize        119      // uint8_t
#define NEOEM_NEOEMCue3TheaterChaseColorMode      120      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3TheaterChaseColorSpeed     121      // uint8_t
#define NEOEM_NEOEMCue3TheaterChaseBounce         122      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3SparkleSpeed               117      // uint8_t
#define NEOEM_NEOEMCue3SparkleFadeRate            118      // uint8_t
#define NEOEM_NEOEMCue3SparkleSparkleCount        119      // uint8_t
#define NEOEM_NEOEMCue3SparkleProbability         120      // uint8_t
#define NEOEM_NEOEMCue3SparkleWhiteOnly           121      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue3SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue3SparkleBurstMode           122      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue3SparkleBurstModeShift 7
#define NEOEM_NEOEMCue3SparkleMode                123      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3BreathingSpeed             117      // uint8_t
#define NEOEM_NEOEMCue3BreathingMinBrightness     118      // uint8_t
#define NEOEM_NEOEMCue3BreathingCurve             119      // uint8_t
#define NEOEM_NEOEMCue3BreathingRainbowMode       120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue3BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue3BreathingWaveform          121      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3StrobeSpeed                117      // uint8_t
#define NEOEM_NEOEMCue3StrobeOnRatio              118      // uint8_t
#define NEOEM_NEOEMCue3StrobeMinBrightness        119      // uint8_t
#define NEOEM_NEOEMCue3StrobeRandomTiming         120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue3StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue3StrobeRainbowStrobe        121      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue3StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue3CometSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3CometFadeRate              118      // uint8_t
#define NEOEM_NEOEMCue3CometTailLength            119      // uint8_t
#define NEOEM_NEOEMCue3CometBounceMode            120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue3CometBounceModeShift 7
#define NEOEM_NEOEMCue3CometRainbowMode           121      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue3CometRainbowModeShift 7
#define NEOEM_NEOEMCue3CometMode                  122      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3NoiseSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3NoiseScale                 118      // uint8_t
#define NEOEM_NEOEMCue3NoiseSaturation            119      // uint8_t
#define NEOEM_NEOEMCue3NoiseHueOffset             120      // uint8_t
#define NEOEM_NEOEMCue3NoisePalette               121      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3PaletteSpeed               117      // uint8_t
#define NEOEM_NEOEMCue3PalettePalette             118      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3PaletteBlend               119      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue3PaletteBlendShift 7
#define NEOEM_NEOEMCue3PaletteSpacing             120      // uint8_t
#define NEOEM_NEOEMCue3BlitzSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3BlitzWidth                 118      // uint8_t
#define NEOEM_NEOEMCue3BlitzDecay                 119      // uint8_t
#define NEOEM_NEOEMCue3BlitzHue                   120      // uint8_t
#define NEOEM_NEOEMCue3BlitzIntensity             121      // uint8_t
#define NEOEM_NEOEMCue3GradientSpeed              117      // uint8_t
#define NEOEM_NEOEMCue3GradientStartHue           118      // uint8_t
#define NEOEM_NEOEMCue3GradientEndHue             119      // uint8_t
#define NEOEM_NEOEMCue3GradientSaturation         120      // uint8_t
#define NEOEM_NEOEMCue3KerzeSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3KerzeIntensity             118      // uint8_t
#define NEOEM_NEOEMCue3KerzeZones                 119      // uint8_t
#define NEOEM_NEOEMCue3LaufschriftSpeed           117      // uint8_t
#define NEOEM_NEOEMCue3LaufschriftGap             118      // uint8_t
#define NEOEM_NEOEMCue3LaufschriftLoop            119      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue3LaufschriftLoopShift 7
#define NEOEM_NEOEMCue3LaufschriftText            150      // char*, 14 Byte
#define     NEOEM_NEOEMCue3LaufschriftTextLength 14
#define NEOEM_NEOEMCue3LaufschriftFont            121      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Uhr2DViewMode              117      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Uhr2DShowSeconds           118      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue3Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue3Uhr2DBlinkColon            119      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue3Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue3Uhr2DColourHue             120      // uint8_t
#define NEOEM_NEOEMCue3Uhr2DDateMode              121      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Uhr2DDateFormat            122      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Uhr2DDateHue               123      // uint8_t
#define NEOEM_NEOEMCue3Uhr2DSwitchSec             124      // uint8_t
#define NEOEM_NEOEMCue3Uhr2DScrollSpeed           125      // uint8_t
#define NEOEM_NEOEMCue3Uhr2DFont                  126      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Snake2DSpeed               117      // uint8_t
#define NEOEM_NEOEMCue3Snake2DHeadHue             118      // uint8_t
#define NEOEM_NEOEMCue3Snake2DBodyMode            119      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Snake2DBodyHue             120      // uint8_t
#define NEOEM_NEOEMCue3Matrix2DSpeed              117      // uint8_t
#define NEOEM_NEOEMCue3Matrix2DDensity            118      // uint8_t
#define NEOEM_NEOEMCue3Matrix2DPalette            119      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Matrix2DGlitch             120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue3Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue3Tetris2DSpeed              117      // uint8_t
#define NEOEM_NEOEMCue3Tetris2DBgBrightness       118      // uint8_t
#define NEOEM_NEOEMCue3Tetris2DGhostPiece         119      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue3Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue3Tetris2DColorMode          120      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3Tetris2DFlashLines         121      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue3Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue3Tetris2DAutoPlay           122      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3TRONSpeed                  117      // uint8_t
#define NEOEM_NEOEMCue3TRONHue                    118      // uint8_t
#define NEOEM_NEOEMCue3TRONGridSpacing            119      // uint8_t
#define NEOEM_NEOEMCue3TRONGlow                   120      // uint8_t
#define NEOEM_NEOEMCue3StarfieldWarpSpeed         117      // uint8_t
#define NEOEM_NEOEMCue3StarfieldWarpDensity       118      // uint8_t
#define NEOEM_NEOEMCue3StarfieldWarpColorMode     119      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue3StarfieldWarpWarpPulse     120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue3StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue3PlasmaNebulaSpeed          117      // uint8_t
#define NEOEM_NEOEMCue3PlasmaNebulaSaturation     118      // uint8_t
#define NEOEM_NEOEMCue3PlasmaNebulaContrast       119      // uint8_t
#define NEOEM_NEOEMCue3PlasmaNebulaPaletteShift   120      // uint8_t
#define NEOEM_NEOEMCue3UFOSwarmSpeed              117      // uint8_t
#define NEOEM_NEOEMCue3UFOSwarmCount              118      // uint8_t
#define NEOEM_NEOEMCue3UFOSwarmHue                119      // uint8_t
#define NEOEM_NEOEMCue3UFOSwarmBeam               120      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue3UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue3UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue3GameofLife2DSpeed          117      // uint8_t
#define NEOEM_NEOEMCue3GameofLife2DHue            118      // uint8_t
#define NEOEM_NEOEMCue3GameofLife2DColorMode      119      // uint8_t
#define NEOEM_NEOEMCue3GameofLife2DSpawnDensity   120      // uint8_t
#define NEOEM_NEOEMCue3DNA2DSpeed                 117      // uint8_t
#define NEOEM_NEOEMCue3DNA2DHue                   118      // uint8_t
#define NEOEM_NEOEMCue3DNA2DTwist                 119      // uint8_t
#define NEOEM_NEOEMCue3DNA2DRungSpacing           120      // uint8_t
#define NEOEM_NEOEMCue3Aurora2DSpeed              117      // uint8_t
#define NEOEM_NEOEMCue3Aurora2DHue                118      // uint8_t
#define NEOEM_NEOEMCue3Aurora2DScale              119      // uint8_t
#define NEOEM_NEOEMCue3Aurora2DIntensity          120      // uint8_t
#define NEOEM_NEOEMCue3Lissajous2DSpeed           117      // uint8_t
#define NEOEM_NEOEMCue3Lissajous2DHue             118      // uint8_t
#define NEOEM_NEOEMCue3Lissajous2DFreqA           119      // uint8_t
#define NEOEM_NEOEMCue3Lissajous2DFreqB           120      // uint8_t
#define NEOEM_NEOEMCue3Metaballs2DSpeed           117      // uint8_t
#define NEOEM_NEOEMCue3Metaballs2DHue             118      // uint8_t
#define NEOEM_NEOEMCue3Metaballs2DBlobCount       119      // uint8_t
#define NEOEM_NEOEMCue3Metaballs2DContrast        120      // uint8_t
#define NEOEM_NEOEMCue4Effect                     164      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Param0                     165      // uint8_t
#define NEOEM_NEOEMCue4Param1                     166      // uint8_t
#define NEOEM_NEOEMCue4Param2                     167      // uint8_t
#define NEOEM_NEOEMCue4Param3                     168      // uint8_t
#define NEOEM_NEOEMCue4Param4                     169      // uint8_t
#define NEOEM_NEOEMCue4Param5                     170      // uint8_t
#define NEOEM_NEOEMCue4Param6                     171      // uint8_t
#define NEOEM_NEOEMCue4Param7                     172      // uint8_t
#define NEOEM_NEOEMCue4Param8                     173      // uint8_t
#define NEOEM_NEOEMCue4Param9                     174      // uint8_t
#define NEOEM_NEOEMCue4Color                      175      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue4ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue4ColorShift 8
#define NEOEM_NEOEMCue4W                          178      // uint8_t
#define NEOEM_NEOEMCue4Brightness                 179      // uint8_t
#define NEOEM_NEOEMCue4Duration                   180      // uint16_t
#define NEOEM_NEOEMCue4Fade                       182      // uint16_t
#define NEOEM_NEOEMCue4Text                       184      // char*, 14 Byte
#define     NEOEM_NEOEMCue4TextLength 14
#define NEOEM_NEOEMCue4EffectText                 198      // char*, 14 Byte
#define     NEOEM_NEOEMCue4EffectTextLength 14
#define NEOEM_NEOEMCue4WipeSpeed                  165      // uint8_t
#define NEOEM_NEOEMCue4WipeDirection              166      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4RainbowSpeed               165      // uint8_t
#define NEOEM_NEOEMCue4RainbowDelta               166      // uint8_t
#define NEOEM_NEOEMCue4RainbowSaturation          167      // uint8_t
#define NEOEM_NEOEMCue4RainbowDensity             168      // uint8_t
#define NEOEM_NEOEMCue4RainbowMode                169      // uint8_t
#define NEOEM_NEOEMCue4Pride2015Speed             165      // uint8_t
#define NEOEM_NEOEMCue4JuggleSpeed                165      // uint8_t
#define NEOEM_NEOEMCue4JuggleNumDots              166      // uint8_t
#define NEOEM_NEOEMCue4JuggleFadeSpeed            167      // uint8_t
#define NEOEM_NEOEMCue4JuggleHueOffset            168      // uint8_t
#define NEOEM_NEOEMCue4BPMBPM                     165      // uint8_t
#define NEOEM_NEOEMCue4BPMHue                     166      // uint8_t
#define NEOEM_NEOEMCue4CylonSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4CylonHue                   166      // uint8_t
#define NEOEM_NEOEMCue4CylonEyeSize               167      // uint8_t
#define NEOEM_NEOEMCue4CylonFadeAmount            168      // uint8_t
#define NEOEM_NEOEMCue4CylonDirection             169      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4CylonMode                  170      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4TestPhaseDuration          165      // uint8_t
#define NEOEM_NEOEMCue4TestMode                   166      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4FireSpeed                  165      // uint8_t
#define NEOEM_NEOEMCue4FireCooling                166      // uint8_t
#define NEOEM_NEOEMCue4FireSparking               167      // uint8_t
#define NEOEM_NEOEMCue4FireReverseDirection       168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue4FireReverseDirectionShift 7
#define NEOEM_NEOEMCue4FireBlueFireMode           169      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue4FireBlueFireModeShift 7
#define NEOEM_NEOEMCue4TheaterChaseSpeed          165      // uint8_t
#define NEOEM_NEOEMCue4TheaterChaseSpacing        166      // uint8_t
#define NEOEM_NEOEMCue4TheaterChaseDotSize        167      // uint8_t
#define NEOEM_NEOEMCue4TheaterChaseColorMode      168      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4TheaterChaseColorSpeed     169      // uint8_t
#define NEOEM_NEOEMCue4TheaterChaseBounce         170      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4SparkleSpeed               165      // uint8_t
#define NEOEM_NEOEMCue4SparkleFadeRate            166      // uint8_t
#define NEOEM_NEOEMCue4SparkleSparkleCount        167      // uint8_t
#define NEOEM_NEOEMCue4SparkleProbability         168      // uint8_t
#define NEOEM_NEOEMCue4SparkleWhiteOnly           169      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue4SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue4SparkleBurstMode           170      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue4SparkleBurstModeShift 7
#define NEOEM_NEOEMCue4SparkleMode                171      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4BreathingSpeed             165      // uint8_t
#define NEOEM_NEOEMCue4BreathingMinBrightness     166      // uint8_t
#define NEOEM_NEOEMCue4BreathingCurve             167      // uint8_t
#define NEOEM_NEOEMCue4BreathingRainbowMode       168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue4BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue4BreathingWaveform          169      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4StrobeSpeed                165      // uint8_t
#define NEOEM_NEOEMCue4StrobeOnRatio              166      // uint8_t
#define NEOEM_NEOEMCue4StrobeMinBrightness        167      // uint8_t
#define NEOEM_NEOEMCue4StrobeRandomTiming         168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue4StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue4StrobeRainbowStrobe        169      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue4StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue4CometSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4CometFadeRate              166      // uint8_t
#define NEOEM_NEOEMCue4CometTailLength            167      // uint8_t
#define NEOEM_NEOEMCue4CometBounceMode            168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue4CometBounceModeShift 7
#define NEOEM_NEOEMCue4CometRainbowMode           169      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue4CometRainbowModeShift 7
#define NEOEM_NEOEMCue4CometMode                  170      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4NoiseSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4NoiseScale                 166      // uint8_t
#define NEOEM_NEOEMCue4NoiseSaturation            167      // uint8_t
#define NEOEM_NEOEMCue4NoiseHueOffset             168      // uint8_t
#define NEOEM_NEOEMCue4NoisePalette               169      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4PaletteSpeed               165      // uint8_t
#define NEOEM_NEOEMCue4PalettePalette             166      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4PaletteBlend               167      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue4PaletteBlendShift 7
#define NEOEM_NEOEMCue4PaletteSpacing             168      // uint8_t
#define NEOEM_NEOEMCue4BlitzSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4BlitzWidth                 166      // uint8_t
#define NEOEM_NEOEMCue4BlitzDecay                 167      // uint8_t
#define NEOEM_NEOEMCue4BlitzHue                   168      // uint8_t
#define NEOEM_NEOEMCue4BlitzIntensity             169      // uint8_t
#define NEOEM_NEOEMCue4GradientSpeed              165      // uint8_t
#define NEOEM_NEOEMCue4GradientStartHue           166      // uint8_t
#define NEOEM_NEOEMCue4GradientEndHue             167      // uint8_t
#define NEOEM_NEOEMCue4GradientSaturation         168      // uint8_t
#define NEOEM_NEOEMCue4KerzeSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4KerzeIntensity             166      // uint8_t
#define NEOEM_NEOEMCue4KerzeZones                 167      // uint8_t
#define NEOEM_NEOEMCue4LaufschriftSpeed           165      // uint8_t
#define NEOEM_NEOEMCue4LaufschriftGap             166      // uint8_t
#define NEOEM_NEOEMCue4LaufschriftLoop            167      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue4LaufschriftLoopShift 7
#define NEOEM_NEOEMCue4LaufschriftText            198      // char*, 14 Byte
#define     NEOEM_NEOEMCue4LaufschriftTextLength 14
#define NEOEM_NEOEMCue4LaufschriftFont            169      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Uhr2DViewMode              165      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Uhr2DShowSeconds           166      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue4Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue4Uhr2DBlinkColon            167      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue4Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue4Uhr2DColourHue             168      // uint8_t
#define NEOEM_NEOEMCue4Uhr2DDateMode              169      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Uhr2DDateFormat            170      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Uhr2DDateHue               171      // uint8_t
#define NEOEM_NEOEMCue4Uhr2DSwitchSec             172      // uint8_t
#define NEOEM_NEOEMCue4Uhr2DScrollSpeed           173      // uint8_t
#define NEOEM_NEOEMCue4Uhr2DFont                  174      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Snake2DSpeed               165      // uint8_t
#define NEOEM_NEOEMCue4Snake2DHeadHue             166      // uint8_t
#define NEOEM_NEOEMCue4Snake2DBodyMode            167      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Snake2DBodyHue             168      // uint8_t
#define NEOEM_NEOEMCue4Matrix2DSpeed              165      // uint8_t
#define NEOEM_NEOEMCue4Matrix2DDensity            166      // uint8_t
#define NEOEM_NEOEMCue4Matrix2DPalette            167      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Matrix2DGlitch             168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue4Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue4Tetris2DSpeed              165      // uint8_t
#define NEOEM_NEOEMCue4Tetris2DBgBrightness       166      // uint8_t
#define NEOEM_NEOEMCue4Tetris2DGhostPiece         167      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue4Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue4Tetris2DColorMode          168      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4Tetris2DFlashLines         169      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue4Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue4Tetris2DAutoPlay           170      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4TRONSpeed                  165      // uint8_t
#define NEOEM_NEOEMCue4TRONHue                    166      // uint8_t
#define NEOEM_NEOEMCue4TRONGridSpacing            167      // uint8_t
#define NEOEM_NEOEMCue4TRONGlow                   168      // uint8_t
#define NEOEM_NEOEMCue4StarfieldWarpSpeed         165      // uint8_t
#define NEOEM_NEOEMCue4StarfieldWarpDensity       166      // uint8_t
#define NEOEM_NEOEMCue4StarfieldWarpColorMode     167      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue4StarfieldWarpWarpPulse     168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue4StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue4PlasmaNebulaSpeed          165      // uint8_t
#define NEOEM_NEOEMCue4PlasmaNebulaSaturation     166      // uint8_t
#define NEOEM_NEOEMCue4PlasmaNebulaContrast       167      // uint8_t
#define NEOEM_NEOEMCue4PlasmaNebulaPaletteShift   168      // uint8_t
#define NEOEM_NEOEMCue4UFOSwarmSpeed              165      // uint8_t
#define NEOEM_NEOEMCue4UFOSwarmCount              166      // uint8_t
#define NEOEM_NEOEMCue4UFOSwarmHue                167      // uint8_t
#define NEOEM_NEOEMCue4UFOSwarmBeam               168      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue4UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue4UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue4GameofLife2DSpeed          165      // uint8_t
#define NEOEM_NEOEMCue4GameofLife2DHue            166      // uint8_t
#define NEOEM_NEOEMCue4GameofLife2DColorMode      167      // uint8_t
#define NEOEM_NEOEMCue4GameofLife2DSpawnDensity   168      // uint8_t
#define NEOEM_NEOEMCue4DNA2DSpeed                 165      // uint8_t
#define NEOEM_NEOEMCue4DNA2DHue                   166      // uint8_t
#define NEOEM_NEOEMCue4DNA2DTwist                 167      // uint8_t
#define NEOEM_NEOEMCue4DNA2DRungSpacing           168      // uint8_t
#define NEOEM_NEOEMCue4Aurora2DSpeed              165      // uint8_t
#define NEOEM_NEOEMCue4Aurora2DHue                166      // uint8_t
#define NEOEM_NEOEMCue4Aurora2DScale              167      // uint8_t
#define NEOEM_NEOEMCue4Aurora2DIntensity          168      // uint8_t
#define NEOEM_NEOEMCue4Lissajous2DSpeed           165      // uint8_t
#define NEOEM_NEOEMCue4Lissajous2DHue             166      // uint8_t
#define NEOEM_NEOEMCue4Lissajous2DFreqA           167      // uint8_t
#define NEOEM_NEOEMCue4Lissajous2DFreqB           168      // uint8_t
#define NEOEM_NEOEMCue4Metaballs2DSpeed           165      // uint8_t
#define NEOEM_NEOEMCue4Metaballs2DHue             166      // uint8_t
#define NEOEM_NEOEMCue4Metaballs2DBlobCount       167      // uint8_t
#define NEOEM_NEOEMCue4Metaballs2DContrast        168      // uint8_t
#define NEOEM_NEOEMCue5Effect                     212      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Param0                     213      // uint8_t
#define NEOEM_NEOEMCue5Param1                     214      // uint8_t
#define NEOEM_NEOEMCue5Param2                     215      // uint8_t
#define NEOEM_NEOEMCue5Param3                     216      // uint8_t
#define NEOEM_NEOEMCue5Param4                     217      // uint8_t
#define NEOEM_NEOEMCue5Param5                     218      // uint8_t
#define NEOEM_NEOEMCue5Param6                     219      // uint8_t
#define NEOEM_NEOEMCue5Param7                     220      // uint8_t
#define NEOEM_NEOEMCue5Param8                     221      // uint8_t
#define NEOEM_NEOEMCue5Param9                     222      // uint8_t
#define NEOEM_NEOEMCue5Color                      223      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue5ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue5ColorShift 8
#define NEOEM_NEOEMCue5W                          226      // uint8_t
#define NEOEM_NEOEMCue5Brightness                 227      // uint8_t
#define NEOEM_NEOEMCue5Duration                   228      // uint16_t
#define NEOEM_NEOEMCue5Fade                       230      // uint16_t
#define NEOEM_NEOEMCue5Text                       232      // char*, 14 Byte
#define     NEOEM_NEOEMCue5TextLength 14
#define NEOEM_NEOEMCue5EffectText                 246      // char*, 14 Byte
#define     NEOEM_NEOEMCue5EffectTextLength 14
#define NEOEM_NEOEMCue5WipeSpeed                  213      // uint8_t
#define NEOEM_NEOEMCue5WipeDirection              214      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5RainbowSpeed               213      // uint8_t
#define NEOEM_NEOEMCue5RainbowDelta               214      // uint8_t
#define NEOEM_NEOEMCue5RainbowSaturation          215      // uint8_t
#define NEOEM_NEOEMCue5RainbowDensity             216      // uint8_t
#define NEOEM_NEOEMCue5RainbowMode                217      // uint8_t
#define NEOEM_NEOEMCue5Pride2015Speed             213      // uint8_t
#define NEOEM_NEOEMCue5JuggleSpeed                213      // uint8_t
#define NEOEM_NEOEMCue5JuggleNumDots              214      // uint8_t
#define NEOEM_NEOEMCue5JuggleFadeSpeed            215      // uint8_t
#define NEOEM_NEOEMCue5JuggleHueOffset            216      // uint8_t
#define NEOEM_NEOEMCue5BPMBPM                     213      // uint8_t
#define NEOEM_NEOEMCue5BPMHue                     214      // uint8_t
#define NEOEM_NEOEMCue5CylonSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5CylonHue                   214      // uint8_t
#define NEOEM_NEOEMCue5CylonEyeSize               215      // uint8_t
#define NEOEM_NEOEMCue5CylonFadeAmount            216      // uint8_t
#define NEOEM_NEOEMCue5CylonDirection             217      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5CylonMode                  218      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5TestPhaseDuration          213      // uint8_t
#define NEOEM_NEOEMCue5TestMode                   214      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5FireSpeed                  213      // uint8_t
#define NEOEM_NEOEMCue5FireCooling                214      // uint8_t
#define NEOEM_NEOEMCue5FireSparking               215      // uint8_t
#define NEOEM_NEOEMCue5FireReverseDirection       216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue5FireReverseDirectionShift 7
#define NEOEM_NEOEMCue5FireBlueFireMode           217      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue5FireBlueFireModeShift 7
#define NEOEM_NEOEMCue5TheaterChaseSpeed          213      // uint8_t
#define NEOEM_NEOEMCue5TheaterChaseSpacing        214      // uint8_t
#define NEOEM_NEOEMCue5TheaterChaseDotSize        215      // uint8_t
#define NEOEM_NEOEMCue5TheaterChaseColorMode      216      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5TheaterChaseColorSpeed     217      // uint8_t
#define NEOEM_NEOEMCue5TheaterChaseBounce         218      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5SparkleSpeed               213      // uint8_t
#define NEOEM_NEOEMCue5SparkleFadeRate            214      // uint8_t
#define NEOEM_NEOEMCue5SparkleSparkleCount        215      // uint8_t
#define NEOEM_NEOEMCue5SparkleProbability         216      // uint8_t
#define NEOEM_NEOEMCue5SparkleWhiteOnly           217      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue5SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue5SparkleBurstMode           218      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue5SparkleBurstModeShift 7
#define NEOEM_NEOEMCue5SparkleMode                219      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5BreathingSpeed             213      // uint8_t
#define NEOEM_NEOEMCue5BreathingMinBrightness     214      // uint8_t
#define NEOEM_NEOEMCue5BreathingCurve             215      // uint8_t
#define NEOEM_NEOEMCue5BreathingRainbowMode       216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue5BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue5BreathingWaveform          217      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5StrobeSpeed                213      // uint8_t
#define NEOEM_NEOEMCue5StrobeOnRatio              214      // uint8_t
#define NEOEM_NEOEMCue5StrobeMinBrightness        215      // uint8_t
#define NEOEM_NEOEMCue5StrobeRandomTiming         216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue5StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue5StrobeRainbowStrobe        217      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue5StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue5CometSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5CometFadeRate              214      // uint8_t
#define NEOEM_NEOEMCue5CometTailLength            215      // uint8_t
#define NEOEM_NEOEMCue5CometBounceMode            216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue5CometBounceModeShift 7
#define NEOEM_NEOEMCue5CometRainbowMode           217      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue5CometRainbowModeShift 7
#define NEOEM_NEOEMCue5CometMode                  218      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5NoiseSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5NoiseScale                 214      // uint8_t
#define NEOEM_NEOEMCue5NoiseSaturation            215      // uint8_t
#define NEOEM_NEOEMCue5NoiseHueOffset             216      // uint8_t
#define NEOEM_NEOEMCue5NoisePalette               217      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5PaletteSpeed               213      // uint8_t
#define NEOEM_NEOEMCue5PalettePalette             214      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5PaletteBlend               215      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue5PaletteBlendShift 7
#define NEOEM_NEOEMCue5PaletteSpacing             216      // uint8_t
#define NEOEM_NEOEMCue5BlitzSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5BlitzWidth                 214      // uint8_t
#define NEOEM_NEOEMCue5BlitzDecay                 215      // uint8_t
#define NEOEM_NEOEMCue5BlitzHue                   216      // uint8_t
#define NEOEM_NEOEMCue5BlitzIntensity             217      // uint8_t
#define NEOEM_NEOEMCue5GradientSpeed              213      // uint8_t
#define NEOEM_NEOEMCue5GradientStartHue           214      // uint8_t
#define NEOEM_NEOEMCue5GradientEndHue             215      // uint8_t
#define NEOEM_NEOEMCue5GradientSaturation         216      // uint8_t
#define NEOEM_NEOEMCue5KerzeSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5KerzeIntensity             214      // uint8_t
#define NEOEM_NEOEMCue5KerzeZones                 215      // uint8_t
#define NEOEM_NEOEMCue5LaufschriftSpeed           213      // uint8_t
#define NEOEM_NEOEMCue5LaufschriftGap             214      // uint8_t
#define NEOEM_NEOEMCue5LaufschriftLoop            215      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue5LaufschriftLoopShift 7
#define NEOEM_NEOEMCue5LaufschriftText            246      // char*, 14 Byte
#define     NEOEM_NEOEMCue5LaufschriftTextLength 14
#define NEOEM_NEOEMCue5LaufschriftFont            217      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Uhr2DViewMode              213      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Uhr2DShowSeconds           214      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue5Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue5Uhr2DBlinkColon            215      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue5Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue5Uhr2DColourHue             216      // uint8_t
#define NEOEM_NEOEMCue5Uhr2DDateMode              217      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Uhr2DDateFormat            218      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Uhr2DDateHue               219      // uint8_t
#define NEOEM_NEOEMCue5Uhr2DSwitchSec             220      // uint8_t
#define NEOEM_NEOEMCue5Uhr2DScrollSpeed           221      // uint8_t
#define NEOEM_NEOEMCue5Uhr2DFont                  222      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Snake2DSpeed               213      // uint8_t
#define NEOEM_NEOEMCue5Snake2DHeadHue             214      // uint8_t
#define NEOEM_NEOEMCue5Snake2DBodyMode            215      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Snake2DBodyHue             216      // uint8_t
#define NEOEM_NEOEMCue5Matrix2DSpeed              213      // uint8_t
#define NEOEM_NEOEMCue5Matrix2DDensity            214      // uint8_t
#define NEOEM_NEOEMCue5Matrix2DPalette            215      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Matrix2DGlitch             216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue5Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue5Tetris2DSpeed              213      // uint8_t
#define NEOEM_NEOEMCue5Tetris2DBgBrightness       214      // uint8_t
#define NEOEM_NEOEMCue5Tetris2DGhostPiece         215      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue5Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue5Tetris2DColorMode          216      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5Tetris2DFlashLines         217      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue5Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue5Tetris2DAutoPlay           218      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5TRONSpeed                  213      // uint8_t
#define NEOEM_NEOEMCue5TRONHue                    214      // uint8_t
#define NEOEM_NEOEMCue5TRONGridSpacing            215      // uint8_t
#define NEOEM_NEOEMCue5TRONGlow                   216      // uint8_t
#define NEOEM_NEOEMCue5StarfieldWarpSpeed         213      // uint8_t
#define NEOEM_NEOEMCue5StarfieldWarpDensity       214      // uint8_t
#define NEOEM_NEOEMCue5StarfieldWarpColorMode     215      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue5StarfieldWarpWarpPulse     216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue5StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue5PlasmaNebulaSpeed          213      // uint8_t
#define NEOEM_NEOEMCue5PlasmaNebulaSaturation     214      // uint8_t
#define NEOEM_NEOEMCue5PlasmaNebulaContrast       215      // uint8_t
#define NEOEM_NEOEMCue5PlasmaNebulaPaletteShift   216      // uint8_t
#define NEOEM_NEOEMCue5UFOSwarmSpeed              213      // uint8_t
#define NEOEM_NEOEMCue5UFOSwarmCount              214      // uint8_t
#define NEOEM_NEOEMCue5UFOSwarmHue                215      // uint8_t
#define NEOEM_NEOEMCue5UFOSwarmBeam               216      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue5UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue5UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue5GameofLife2DSpeed          213      // uint8_t
#define NEOEM_NEOEMCue5GameofLife2DHue            214      // uint8_t
#define NEOEM_NEOEMCue5GameofLife2DColorMode      215      // uint8_t
#define NEOEM_NEOEMCue5GameofLife2DSpawnDensity   216      // uint8_t
#define NEOEM_NEOEMCue5DNA2DSpeed                 213      // uint8_t
#define NEOEM_NEOEMCue5DNA2DHue                   214      // uint8_t
#define NEOEM_NEOEMCue5DNA2DTwist                 215      // uint8_t
#define NEOEM_NEOEMCue5DNA2DRungSpacing           216      // uint8_t
#define NEOEM_NEOEMCue5Aurora2DSpeed              213      // uint8_t
#define NEOEM_NEOEMCue5Aurora2DHue                214      // uint8_t
#define NEOEM_NEOEMCue5Aurora2DScale              215      // uint8_t
#define NEOEM_NEOEMCue5Aurora2DIntensity          216      // uint8_t
#define NEOEM_NEOEMCue5Lissajous2DSpeed           213      // uint8_t
#define NEOEM_NEOEMCue5Lissajous2DHue             214      // uint8_t
#define NEOEM_NEOEMCue5Lissajous2DFreqA           215      // uint8_t
#define NEOEM_NEOEMCue5Lissajous2DFreqB           216      // uint8_t
#define NEOEM_NEOEMCue5Metaballs2DSpeed           213      // uint8_t
#define NEOEM_NEOEMCue5Metaballs2DHue             214      // uint8_t
#define NEOEM_NEOEMCue5Metaballs2DBlobCount       215      // uint8_t
#define NEOEM_NEOEMCue5Metaballs2DContrast        216      // uint8_t
#define NEOEM_NEOEMCue6Effect                     260      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Param0                     261      // uint8_t
#define NEOEM_NEOEMCue6Param1                     262      // uint8_t
#define NEOEM_NEOEMCue6Param2                     263      // uint8_t
#define NEOEM_NEOEMCue6Param3                     264      // uint8_t
#define NEOEM_NEOEMCue6Param4                     265      // uint8_t
#define NEOEM_NEOEMCue6Param5                     266      // uint8_t
#define NEOEM_NEOEMCue6Param6                     267      // uint8_t
#define NEOEM_NEOEMCue6Param7                     268      // uint8_t
#define NEOEM_NEOEMCue6Param8                     269      // uint8_t
#define NEOEM_NEOEMCue6Param9                     270      // uint8_t
#define NEOEM_NEOEMCue6Color                      271      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue6ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue6ColorShift 8
#define NEOEM_NEOEMCue6W                          274      // uint8_t
#define NEOEM_NEOEMCue6Brightness                 275      // uint8_t
#define NEOEM_NEOEMCue6Duration                   276      // uint16_t
#define NEOEM_NEOEMCue6Fade                       278      // uint16_t
#define NEOEM_NEOEMCue6Text                       280      // char*, 14 Byte
#define     NEOEM_NEOEMCue6TextLength 14
#define NEOEM_NEOEMCue6EffectText                 294      // char*, 14 Byte
#define     NEOEM_NEOEMCue6EffectTextLength 14
#define NEOEM_NEOEMCue6WipeSpeed                  261      // uint8_t
#define NEOEM_NEOEMCue6WipeDirection              262      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6RainbowSpeed               261      // uint8_t
#define NEOEM_NEOEMCue6RainbowDelta               262      // uint8_t
#define NEOEM_NEOEMCue6RainbowSaturation          263      // uint8_t
#define NEOEM_NEOEMCue6RainbowDensity             264      // uint8_t
#define NEOEM_NEOEMCue6RainbowMode                265      // uint8_t
#define NEOEM_NEOEMCue6Pride2015Speed             261      // uint8_t
#define NEOEM_NEOEMCue6JuggleSpeed                261      // uint8_t
#define NEOEM_NEOEMCue6JuggleNumDots              262      // uint8_t
#define NEOEM_NEOEMCue6JuggleFadeSpeed            263      // uint8_t
#define NEOEM_NEOEMCue6JuggleHueOffset            264      // uint8_t
#define NEOEM_NEOEMCue6BPMBPM                     261      // uint8_t
#define NEOEM_NEOEMCue6BPMHue                     262      // uint8_t
#define NEOEM_NEOEMCue6CylonSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6CylonHue                   262      // uint8_t
#define NEOEM_NEOEMCue6CylonEyeSize               263      // uint8_t
#define NEOEM_NEOEMCue6CylonFadeAmount            264      // uint8_t
#define NEOEM_NEOEMCue6CylonDirection             265      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6CylonMode                  266      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6TestPhaseDuration          261      // uint8_t
#define NEOEM_NEOEMCue6TestMode                   262      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6FireSpeed                  261      // uint8_t
#define NEOEM_NEOEMCue6FireCooling                262      // uint8_t
#define NEOEM_NEOEMCue6FireSparking               263      // uint8_t
#define NEOEM_NEOEMCue6FireReverseDirection       264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue6FireReverseDirectionShift 7
#define NEOEM_NEOEMCue6FireBlueFireMode           265      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue6FireBlueFireModeShift 7
#define NEOEM_NEOEMCue6TheaterChaseSpeed          261      // uint8_t
#define NEOEM_NEOEMCue6TheaterChaseSpacing        262      // uint8_t
#define NEOEM_NEOEMCue6TheaterChaseDotSize        263      // uint8_t
#define NEOEM_NEOEMCue6TheaterChaseColorMode      264      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6TheaterChaseColorSpeed     265      // uint8_t
#define NEOEM_NEOEMCue6TheaterChaseBounce         266      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6SparkleSpeed               261      // uint8_t
#define NEOEM_NEOEMCue6SparkleFadeRate            262      // uint8_t
#define NEOEM_NEOEMCue6SparkleSparkleCount        263      // uint8_t
#define NEOEM_NEOEMCue6SparkleProbability         264      // uint8_t
#define NEOEM_NEOEMCue6SparkleWhiteOnly           265      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue6SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue6SparkleBurstMode           266      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue6SparkleBurstModeShift 7
#define NEOEM_NEOEMCue6SparkleMode                267      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6BreathingSpeed             261      // uint8_t
#define NEOEM_NEOEMCue6BreathingMinBrightness     262      // uint8_t
#define NEOEM_NEOEMCue6BreathingCurve             263      // uint8_t
#define NEOEM_NEOEMCue6BreathingRainbowMode       264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue6BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue6BreathingWaveform          265      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6StrobeSpeed                261      // uint8_t
#define NEOEM_NEOEMCue6StrobeOnRatio              262      // uint8_t
#define NEOEM_NEOEMCue6StrobeMinBrightness        263      // uint8_t
#define NEOEM_NEOEMCue6StrobeRandomTiming         264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue6StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue6StrobeRainbowStrobe        265      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue6StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue6CometSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6CometFadeRate              262      // uint8_t
#define NEOEM_NEOEMCue6CometTailLength            263      // uint8_t
#define NEOEM_NEOEMCue6CometBounceMode            264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue6CometBounceModeShift 7
#define NEOEM_NEOEMCue6CometRainbowMode           265      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue6CometRainbowModeShift 7
#define NEOEM_NEOEMCue6CometMode                  266      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6NoiseSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6NoiseScale                 262      // uint8_t
#define NEOEM_NEOEMCue6NoiseSaturation            263      // uint8_t
#define NEOEM_NEOEMCue6NoiseHueOffset             264      // uint8_t
#define NEOEM_NEOEMCue6NoisePalette               265      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6PaletteSpeed               261      // uint8_t
#define NEOEM_NEOEMCue6PalettePalette             262      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6PaletteBlend               263      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue6PaletteBlendShift 7
#define NEOEM_NEOEMCue6PaletteSpacing             264      // uint8_t
#define NEOEM_NEOEMCue6BlitzSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6BlitzWidth                 262      // uint8_t
#define NEOEM_NEOEMCue6BlitzDecay                 263      // uint8_t
#define NEOEM_NEOEMCue6BlitzHue                   264      // uint8_t
#define NEOEM_NEOEMCue6BlitzIntensity             265      // uint8_t
#define NEOEM_NEOEMCue6GradientSpeed              261      // uint8_t
#define NEOEM_NEOEMCue6GradientStartHue           262      // uint8_t
#define NEOEM_NEOEMCue6GradientEndHue             263      // uint8_t
#define NEOEM_NEOEMCue6GradientSaturation         264      // uint8_t
#define NEOEM_NEOEMCue6KerzeSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6KerzeIntensity             262      // uint8_t
#define NEOEM_NEOEMCue6KerzeZones                 263      // uint8_t
#define NEOEM_NEOEMCue6LaufschriftSpeed           261      // uint8_t
#define NEOEM_NEOEMCue6LaufschriftGap             262      // uint8_t
#define NEOEM_NEOEMCue6LaufschriftLoop            263      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue6LaufschriftLoopShift 7
#define NEOEM_NEOEMCue6LaufschriftText            294      // char*, 14 Byte
#define     NEOEM_NEOEMCue6LaufschriftTextLength 14
#define NEOEM_NEOEMCue6LaufschriftFont            265      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Uhr2DViewMode              261      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Uhr2DShowSeconds           262      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue6Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue6Uhr2DBlinkColon            263      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue6Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue6Uhr2DColourHue             264      // uint8_t
#define NEOEM_NEOEMCue6Uhr2DDateMode              265      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Uhr2DDateFormat            266      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Uhr2DDateHue               267      // uint8_t
#define NEOEM_NEOEMCue6Uhr2DSwitchSec             268      // uint8_t
#define NEOEM_NEOEMCue6Uhr2DScrollSpeed           269      // uint8_t
#define NEOEM_NEOEMCue6Uhr2DFont                  270      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Snake2DSpeed               261      // uint8_t
#define NEOEM_NEOEMCue6Snake2DHeadHue             262      // uint8_t
#define NEOEM_NEOEMCue6Snake2DBodyMode            263      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Snake2DBodyHue             264      // uint8_t
#define NEOEM_NEOEMCue6Matrix2DSpeed              261      // uint8_t
#define NEOEM_NEOEMCue6Matrix2DDensity            262      // uint8_t
#define NEOEM_NEOEMCue6Matrix2DPalette            263      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Matrix2DGlitch             264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue6Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue6Tetris2DSpeed              261      // uint8_t
#define NEOEM_NEOEMCue6Tetris2DBgBrightness       262      // uint8_t
#define NEOEM_NEOEMCue6Tetris2DGhostPiece         263      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue6Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue6Tetris2DColorMode          264      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6Tetris2DFlashLines         265      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue6Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue6Tetris2DAutoPlay           266      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6TRONSpeed                  261      // uint8_t
#define NEOEM_NEOEMCue6TRONHue                    262      // uint8_t
#define NEOEM_NEOEMCue6TRONGridSpacing            263      // uint8_t
#define NEOEM_NEOEMCue6TRONGlow                   264      // uint8_t
#define NEOEM_NEOEMCue6StarfieldWarpSpeed         261      // uint8_t
#define NEOEM_NEOEMCue6StarfieldWarpDensity       262      // uint8_t
#define NEOEM_NEOEMCue6StarfieldWarpColorMode     263      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue6StarfieldWarpWarpPulse     264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue6StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue6PlasmaNebulaSpeed          261      // uint8_t
#define NEOEM_NEOEMCue6PlasmaNebulaSaturation     262      // uint8_t
#define NEOEM_NEOEMCue6PlasmaNebulaContrast       263      // uint8_t
#define NEOEM_NEOEMCue6PlasmaNebulaPaletteShift   264      // uint8_t
#define NEOEM_NEOEMCue6UFOSwarmSpeed              261      // uint8_t
#define NEOEM_NEOEMCue6UFOSwarmCount              262      // uint8_t
#define NEOEM_NEOEMCue6UFOSwarmHue                263      // uint8_t
#define NEOEM_NEOEMCue6UFOSwarmBeam               264      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue6UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue6UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue6GameofLife2DSpeed          261      // uint8_t
#define NEOEM_NEOEMCue6GameofLife2DHue            262      // uint8_t
#define NEOEM_NEOEMCue6GameofLife2DColorMode      263      // uint8_t
#define NEOEM_NEOEMCue6GameofLife2DSpawnDensity   264      // uint8_t
#define NEOEM_NEOEMCue6DNA2DSpeed                 261      // uint8_t
#define NEOEM_NEOEMCue6DNA2DHue                   262      // uint8_t
#define NEOEM_NEOEMCue6DNA2DTwist                 263      // uint8_t
#define NEOEM_NEOEMCue6DNA2DRungSpacing           264      // uint8_t
#define NEOEM_NEOEMCue6Aurora2DSpeed              261      // uint8_t
#define NEOEM_NEOEMCue6Aurora2DHue                262      // uint8_t
#define NEOEM_NEOEMCue6Aurora2DScale              263      // uint8_t
#define NEOEM_NEOEMCue6Aurora2DIntensity          264      // uint8_t
#define NEOEM_NEOEMCue6Lissajous2DSpeed           261      // uint8_t
#define NEOEM_NEOEMCue6Lissajous2DHue             262      // uint8_t
#define NEOEM_NEOEMCue6Lissajous2DFreqA           263      // uint8_t
#define NEOEM_NEOEMCue6Lissajous2DFreqB           264      // uint8_t
#define NEOEM_NEOEMCue6Metaballs2DSpeed           261      // uint8_t
#define NEOEM_NEOEMCue6Metaballs2DHue             262      // uint8_t
#define NEOEM_NEOEMCue6Metaballs2DBlobCount       263      // uint8_t
#define NEOEM_NEOEMCue6Metaballs2DContrast        264      // uint8_t
#define NEOEM_NEOEMCue7Effect                     308      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Param0                     309      // uint8_t
#define NEOEM_NEOEMCue7Param1                     310      // uint8_t
#define NEOEM_NEOEMCue7Param2                     311      // uint8_t
#define NEOEM_NEOEMCue7Param3                     312      // uint8_t
#define NEOEM_NEOEMCue7Param4                     313      // uint8_t
#define NEOEM_NEOEMCue7Param5                     314      // uint8_t
#define NEOEM_NEOEMCue7Param6                     315      // uint8_t
#define NEOEM_NEOEMCue7Param7                     316      // uint8_t
#define NEOEM_NEOEMCue7Param8                     317      // uint8_t
#define NEOEM_NEOEMCue7Param9                     318      // uint8_t
#define NEOEM_NEOEMCue7Color                      319      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue7ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue7ColorShift 8
#define NEOEM_NEOEMCue7W                          322      // uint8_t
#define NEOEM_NEOEMCue7Brightness                 323      // uint8_t
#define NEOEM_NEOEMCue7Duration                   324      // uint16_t
#define NEOEM_NEOEMCue7Fade                       326      // uint16_t
#define NEOEM_NEOEMCue7Text                       328      // char*, 14 Byte
#define     NEOEM_NEOEMCue7TextLength 14
#define NEOEM_NEOEMCue7EffectText                 342      // char*, 14 Byte
#define     NEOEM_NEOEMCue7EffectTextLength 14
#define NEOEM_NEOEMCue7WipeSpeed                  309      // uint8_t
#define NEOEM_NEOEMCue7WipeDirection              310      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7RainbowSpeed               309      // uint8_t
#define NEOEM_NEOEMCue7RainbowDelta               310      // uint8_t
#define NEOEM_NEOEMCue7RainbowSaturation          311      // uint8_t
#define NEOEM_NEOEMCue7RainbowDensity             312      // uint8_t
#define NEOEM_NEOEMCue7RainbowMode                313      // uint8_t
#define NEOEM_NEOEMCue7Pride2015Speed             309      // uint8_t
#define NEOEM_NEOEMCue7JuggleSpeed                309      // uint8_t
#define NEOEM_NEOEMCue7JuggleNumDots              310      // uint8_t
#define NEOEM_NEOEMCue7JuggleFadeSpeed            311      // uint8_t
#define NEOEM_NEOEMCue7JuggleHueOffset            312      // uint8_t
#define NEOEM_NEOEMCue7BPMBPM                     309      // uint8_t
#define NEOEM_NEOEMCue7BPMHue                     310      // uint8_t
#define NEOEM_NEOEMCue7CylonSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7CylonHue                   310      // uint8_t
#define NEOEM_NEOEMCue7CylonEyeSize               311      // uint8_t
#define NEOEM_NEOEMCue7CylonFadeAmount            312      // uint8_t
#define NEOEM_NEOEMCue7CylonDirection             313      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7CylonMode                  314      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7TestPhaseDuration          309      // uint8_t
#define NEOEM_NEOEMCue7TestMode                   310      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7FireSpeed                  309      // uint8_t
#define NEOEM_NEOEMCue7FireCooling                310      // uint8_t
#define NEOEM_NEOEMCue7FireSparking               311      // uint8_t
#define NEOEM_NEOEMCue7FireReverseDirection       312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue7FireReverseDirectionShift 7
#define NEOEM_NEOEMCue7FireBlueFireMode           313      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue7FireBlueFireModeShift 7
#define NEOEM_NEOEMCue7TheaterChaseSpeed          309      // uint8_t
#define NEOEM_NEOEMCue7TheaterChaseSpacing        310      // uint8_t
#define NEOEM_NEOEMCue7TheaterChaseDotSize        311      // uint8_t
#define NEOEM_NEOEMCue7TheaterChaseColorMode      312      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7TheaterChaseColorSpeed     313      // uint8_t
#define NEOEM_NEOEMCue7TheaterChaseBounce         314      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7SparkleSpeed               309      // uint8_t
#define NEOEM_NEOEMCue7SparkleFadeRate            310      // uint8_t
#define NEOEM_NEOEMCue7SparkleSparkleCount        311      // uint8_t
#define NEOEM_NEOEMCue7SparkleProbability         312      // uint8_t
#define NEOEM_NEOEMCue7SparkleWhiteOnly           313      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue7SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue7SparkleBurstMode           314      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue7SparkleBurstModeShift 7
#define NEOEM_NEOEMCue7SparkleMode                315      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7BreathingSpeed             309      // uint8_t
#define NEOEM_NEOEMCue7BreathingMinBrightness     310      // uint8_t
#define NEOEM_NEOEMCue7BreathingCurve             311      // uint8_t
#define NEOEM_NEOEMCue7BreathingRainbowMode       312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue7BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue7BreathingWaveform          313      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7StrobeSpeed                309      // uint8_t
#define NEOEM_NEOEMCue7StrobeOnRatio              310      // uint8_t
#define NEOEM_NEOEMCue7StrobeMinBrightness        311      // uint8_t
#define NEOEM_NEOEMCue7StrobeRandomTiming         312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue7StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue7StrobeRainbowStrobe        313      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue7StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue7CometSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7CometFadeRate              310      // uint8_t
#define NEOEM_NEOEMCue7CometTailLength            311      // uint8_t
#define NEOEM_NEOEMCue7CometBounceMode            312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue7CometBounceModeShift 7
#define NEOEM_NEOEMCue7CometRainbowMode           313      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue7CometRainbowModeShift 7
#define NEOEM_NEOEMCue7CometMode                  314      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7NoiseSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7NoiseScale                 310      // uint8_t
#define NEOEM_NEOEMCue7NoiseSaturation            311      // uint8_t
#define NEOEM_NEOEMCue7NoiseHueOffset             312      // uint8_t
#define NEOEM_NEOEMCue7NoisePalette               313      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7PaletteSpeed               309      // uint8_t
#define NEOEM_NEOEMCue7PalettePalette             310      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7PaletteBlend               311      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue7PaletteBlendShift 7
#define NEOEM_NEOEMCue7PaletteSpacing             312      // uint8_t
#define NEOEM_NEOEMCue7BlitzSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7BlitzWidth                 310      // uint8_t
#define NEOEM_NEOEMCue7BlitzDecay                 311      // uint8_t
#define NEOEM_NEOEMCue7BlitzHue                   312      // uint8_t
#define NEOEM_NEOEMCue7BlitzIntensity             313      // uint8_t
#define NEOEM_NEOEMCue7GradientSpeed              309      // uint8_t
#define NEOEM_NEOEMCue7GradientStartHue           310      // uint8_t
#define NEOEM_NEOEMCue7GradientEndHue             311      // uint8_t
#define NEOEM_NEOEMCue7GradientSaturation         312      // uint8_t
#define NEOEM_NEOEMCue7KerzeSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7KerzeIntensity             310      // uint8_t
#define NEOEM_NEOEMCue7KerzeZones                 311      // uint8_t
#define NEOEM_NEOEMCue7LaufschriftSpeed           309      // uint8_t
#define NEOEM_NEOEMCue7LaufschriftGap             310      // uint8_t
#define NEOEM_NEOEMCue7LaufschriftLoop            311      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue7LaufschriftLoopShift 7
#define NEOEM_NEOEMCue7LaufschriftText            342      // char*, 14 Byte
#define     NEOEM_NEOEMCue7LaufschriftTextLength 14
#define NEOEM_NEOEMCue7LaufschriftFont            313      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Uhr2DViewMode              309      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Uhr2DShowSeconds           310      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue7Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue7Uhr2DBlinkColon            311      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue7Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue7Uhr2DColourHue             312      // uint8_t
#define NEOEM_NEOEMCue7Uhr2DDateMode              313      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Uhr2DDateFormat            314      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Uhr2DDateHue               315      // uint8_t
#define NEOEM_NEOEMCue7Uhr2DSwitchSec             316      // uint8_t
#define NEOEM_NEOEMCue7Uhr2DScrollSpeed           317      // uint8_t
#define NEOEM_NEOEMCue7Uhr2DFont                  318      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Snake2DSpeed               309      // uint8_t
#define NEOEM_NEOEMCue7Snake2DHeadHue             310      // uint8_t
#define NEOEM_NEOEMCue7Snake2DBodyMode            311      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Snake2DBodyHue             312      // uint8_t
#define NEOEM_NEOEMCue7Matrix2DSpeed              309      // uint8_t
#define NEOEM_NEOEMCue7Matrix2DDensity            310      // uint8_t
#define NEOEM_NEOEMCue7Matrix2DPalette            311      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Matrix2DGlitch             312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue7Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue7Tetris2DSpeed              309      // uint8_t
#define NEOEM_NEOEMCue7Tetris2DBgBrightness       310      // uint8_t
#define NEOEM_NEOEMCue7Tetris2DGhostPiece         311      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue7Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue7Tetris2DColorMode          312      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7Tetris2DFlashLines         313      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue7Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue7Tetris2DAutoPlay           314      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7TRONSpeed                  309      // uint8_t
#define NEOEM_NEOEMCue7TRONHue                    310      // uint8_t
#define NEOEM_NEOEMCue7TRONGridSpacing            311      // uint8_t
#define NEOEM_NEOEMCue7TRONGlow                   312      // uint8_t
#define NEOEM_NEOEMCue7StarfieldWarpSpeed         309      // uint8_t
#define NEOEM_NEOEMCue7StarfieldWarpDensity       310      // uint8_t
#define NEOEM_NEOEMCue7StarfieldWarpColorMode     311      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue7StarfieldWarpWarpPulse     312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue7StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue7PlasmaNebulaSpeed          309      // uint8_t
#define NEOEM_NEOEMCue7PlasmaNebulaSaturation     310      // uint8_t
#define NEOEM_NEOEMCue7PlasmaNebulaContrast       311      // uint8_t
#define NEOEM_NEOEMCue7PlasmaNebulaPaletteShift   312      // uint8_t
#define NEOEM_NEOEMCue7UFOSwarmSpeed              309      // uint8_t
#define NEOEM_NEOEMCue7UFOSwarmCount              310      // uint8_t
#define NEOEM_NEOEMCue7UFOSwarmHue                311      // uint8_t
#define NEOEM_NEOEMCue7UFOSwarmBeam               312      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue7UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue7UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue7GameofLife2DSpeed          309      // uint8_t
#define NEOEM_NEOEMCue7GameofLife2DHue            310      // uint8_t
#define NEOEM_NEOEMCue7GameofLife2DColorMode      311      // uint8_t
#define NEOEM_NEOEMCue7GameofLife2DSpawnDensity   312      // uint8_t
#define NEOEM_NEOEMCue7DNA2DSpeed                 309      // uint8_t
#define NEOEM_NEOEMCue7DNA2DHue                   310      // uint8_t
#define NEOEM_NEOEMCue7DNA2DTwist                 311      // uint8_t
#define NEOEM_NEOEMCue7DNA2DRungSpacing           312      // uint8_t
#define NEOEM_NEOEMCue7Aurora2DSpeed              309      // uint8_t
#define NEOEM_NEOEMCue7Aurora2DHue                310      // uint8_t
#define NEOEM_NEOEMCue7Aurora2DScale              311      // uint8_t
#define NEOEM_NEOEMCue7Aurora2DIntensity          312      // uint8_t
#define NEOEM_NEOEMCue7Lissajous2DSpeed           309      // uint8_t
#define NEOEM_NEOEMCue7Lissajous2DHue             310      // uint8_t
#define NEOEM_NEOEMCue7Lissajous2DFreqA           311      // uint8_t
#define NEOEM_NEOEMCue7Lissajous2DFreqB           312      // uint8_t
#define NEOEM_NEOEMCue7Metaballs2DSpeed           309      // uint8_t
#define NEOEM_NEOEMCue7Metaballs2DHue             310      // uint8_t
#define NEOEM_NEOEMCue7Metaballs2DBlobCount       311      // uint8_t
#define NEOEM_NEOEMCue7Metaballs2DContrast        312      // uint8_t
#define NEOEM_NEOEMCue8Effect                     356      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Param0                     357      // uint8_t
#define NEOEM_NEOEMCue8Param1                     358      // uint8_t
#define NEOEM_NEOEMCue8Param2                     359      // uint8_t
#define NEOEM_NEOEMCue8Param3                     360      // uint8_t
#define NEOEM_NEOEMCue8Param4                     361      // uint8_t
#define NEOEM_NEOEMCue8Param5                     362      // uint8_t
#define NEOEM_NEOEMCue8Param6                     363      // uint8_t
#define NEOEM_NEOEMCue8Param7                     364      // uint8_t
#define NEOEM_NEOEMCue8Param8                     365      // uint8_t
#define NEOEM_NEOEMCue8Param9                     366      // uint8_t
#define NEOEM_NEOEMCue8Color                      367      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue8ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue8ColorShift 8
#define NEOEM_NEOEMCue8W                          370      // uint8_t
#define NEOEM_NEOEMCue8Brightness                 371      // uint8_t
#define NEOEM_NEOEMCue8Duration                   372      // uint16_t
#define NEOEM_NEOEMCue8Fade                       374      // uint16_t
#define NEOEM_NEOEMCue8Text                       376      // char*, 14 Byte
#define     NEOEM_NEOEMCue8TextLength 14
#define NEOEM_NEOEMCue8EffectText                 390      // char*, 14 Byte
#define     NEOEM_NEOEMCue8EffectTextLength 14
#define NEOEM_NEOEMCue8WipeSpeed                  357      // uint8_t
#define NEOEM_NEOEMCue8WipeDirection              358      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8RainbowSpeed               357      // uint8_t
#define NEOEM_NEOEMCue8RainbowDelta               358      // uint8_t
#define NEOEM_NEOEMCue8RainbowSaturation          359      // uint8_t
#define NEOEM_NEOEMCue8RainbowDensity             360      // uint8_t
#define NEOEM_NEOEMCue8RainbowMode                361      // uint8_t
#define NEOEM_NEOEMCue8Pride2015Speed             357      // uint8_t
#define NEOEM_NEOEMCue8JuggleSpeed                357      // uint8_t
#define NEOEM_NEOEMCue8JuggleNumDots              358      // uint8_t
#define NEOEM_NEOEMCue8JuggleFadeSpeed            359      // uint8_t
#define NEOEM_NEOEMCue8JuggleHueOffset            360      // uint8_t
#define NEOEM_NEOEMCue8BPMBPM                     357      // uint8_t
#define NEOEM_NEOEMCue8BPMHue                     358      // uint8_t
#define NEOEM_NEOEMCue8CylonSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8CylonHue                   358      // uint8_t
#define NEOEM_NEOEMCue8CylonEyeSize               359      // uint8_t
#define NEOEM_NEOEMCue8CylonFadeAmount            360      // uint8_t
#define NEOEM_NEOEMCue8CylonDirection             361      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8CylonMode                  362      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8TestPhaseDuration          357      // uint8_t
#define NEOEM_NEOEMCue8TestMode                   358      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8FireSpeed                  357      // uint8_t
#define NEOEM_NEOEMCue8FireCooling                358      // uint8_t
#define NEOEM_NEOEMCue8FireSparking               359      // uint8_t
#define NEOEM_NEOEMCue8FireReverseDirection       360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue8FireReverseDirectionShift 7
#define NEOEM_NEOEMCue8FireBlueFireMode           361      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue8FireBlueFireModeShift 7
#define NEOEM_NEOEMCue8TheaterChaseSpeed          357      // uint8_t
#define NEOEM_NEOEMCue8TheaterChaseSpacing        358      // uint8_t
#define NEOEM_NEOEMCue8TheaterChaseDotSize        359      // uint8_t
#define NEOEM_NEOEMCue8TheaterChaseColorMode      360      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8TheaterChaseColorSpeed     361      // uint8_t
#define NEOEM_NEOEMCue8TheaterChaseBounce         362      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8SparkleSpeed               357      // uint8_t
#define NEOEM_NEOEMCue8SparkleFadeRate            358      // uint8_t
#define NEOEM_NEOEMCue8SparkleSparkleCount        359      // uint8_t
#define NEOEM_NEOEMCue8SparkleProbability         360      // uint8_t
#define NEOEM_NEOEMCue8SparkleWhiteOnly           361      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue8SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue8SparkleBurstMode           362      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue8SparkleBurstModeShift 7
#define NEOEM_NEOEMCue8SparkleMode                363      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8BreathingSpeed             357      // uint8_t
#define NEOEM_NEOEMCue8BreathingMinBrightness     358      // uint8_t
#define NEOEM_NEOEMCue8BreathingCurve             359      // uint8_t
#define NEOEM_NEOEMCue8BreathingRainbowMode       360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue8BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue8BreathingWaveform          361      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8StrobeSpeed                357      // uint8_t
#define NEOEM_NEOEMCue8StrobeOnRatio              358      // uint8_t
#define NEOEM_NEOEMCue8StrobeMinBrightness        359      // uint8_t
#define NEOEM_NEOEMCue8StrobeRandomTiming         360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue8StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue8StrobeRainbowStrobe        361      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue8StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue8CometSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8CometFadeRate              358      // uint8_t
#define NEOEM_NEOEMCue8CometTailLength            359      // uint8_t
#define NEOEM_NEOEMCue8CometBounceMode            360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue8CometBounceModeShift 7
#define NEOEM_NEOEMCue8CometRainbowMode           361      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue8CometRainbowModeShift 7
#define NEOEM_NEOEMCue8CometMode                  362      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8NoiseSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8NoiseScale                 358      // uint8_t
#define NEOEM_NEOEMCue8NoiseSaturation            359      // uint8_t
#define NEOEM_NEOEMCue8NoiseHueOffset             360      // uint8_t
#define NEOEM_NEOEMCue8NoisePalette               361      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8PaletteSpeed               357      // uint8_t
#define NEOEM_NEOEMCue8PalettePalette             358      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8PaletteBlend               359      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue8PaletteBlendShift 7
#define NEOEM_NEOEMCue8PaletteSpacing             360      // uint8_t
#define NEOEM_NEOEMCue8BlitzSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8BlitzWidth                 358      // uint8_t
#define NEOEM_NEOEMCue8BlitzDecay                 359      // uint8_t
#define NEOEM_NEOEMCue8BlitzHue                   360      // uint8_t
#define NEOEM_NEOEMCue8BlitzIntensity             361      // uint8_t
#define NEOEM_NEOEMCue8GradientSpeed              357      // uint8_t
#define NEOEM_NEOEMCue8GradientStartHue           358      // uint8_t
#define NEOEM_NEOEMCue8GradientEndHue             359      // uint8_t
#define NEOEM_NEOEMCue8GradientSaturation         360      // uint8_t
#define NEOEM_NEOEMCue8KerzeSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8KerzeIntensity             358      // uint8_t
#define NEOEM_NEOEMCue8KerzeZones                 359      // uint8_t
#define NEOEM_NEOEMCue8LaufschriftSpeed           357      // uint8_t
#define NEOEM_NEOEMCue8LaufschriftGap             358      // uint8_t
#define NEOEM_NEOEMCue8LaufschriftLoop            359      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue8LaufschriftLoopShift 7
#define NEOEM_NEOEMCue8LaufschriftText            390      // char*, 14 Byte
#define     NEOEM_NEOEMCue8LaufschriftTextLength 14
#define NEOEM_NEOEMCue8LaufschriftFont            361      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Uhr2DViewMode              357      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Uhr2DShowSeconds           358      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue8Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue8Uhr2DBlinkColon            359      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue8Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue8Uhr2DColourHue             360      // uint8_t
#define NEOEM_NEOEMCue8Uhr2DDateMode              361      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Uhr2DDateFormat            362      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Uhr2DDateHue               363      // uint8_t
#define NEOEM_NEOEMCue8Uhr2DSwitchSec             364      // uint8_t
#define NEOEM_NEOEMCue8Uhr2DScrollSpeed           365      // uint8_t
#define NEOEM_NEOEMCue8Uhr2DFont                  366      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Snake2DSpeed               357      // uint8_t
#define NEOEM_NEOEMCue8Snake2DHeadHue             358      // uint8_t
#define NEOEM_NEOEMCue8Snake2DBodyMode            359      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Snake2DBodyHue             360      // uint8_t
#define NEOEM_NEOEMCue8Matrix2DSpeed              357      // uint8_t
#define NEOEM_NEOEMCue8Matrix2DDensity            358      // uint8_t
#define NEOEM_NEOEMCue8Matrix2DPalette            359      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Matrix2DGlitch             360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue8Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue8Tetris2DSpeed              357      // uint8_t
#define NEOEM_NEOEMCue8Tetris2DBgBrightness       358      // uint8_t
#define NEOEM_NEOEMCue8Tetris2DGhostPiece         359      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue8Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue8Tetris2DColorMode          360      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8Tetris2DFlashLines         361      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue8Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue8Tetris2DAutoPlay           362      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8TRONSpeed                  357      // uint8_t
#define NEOEM_NEOEMCue8TRONHue                    358      // uint8_t
#define NEOEM_NEOEMCue8TRONGridSpacing            359      // uint8_t
#define NEOEM_NEOEMCue8TRONGlow                   360      // uint8_t
#define NEOEM_NEOEMCue8StarfieldWarpSpeed         357      // uint8_t
#define NEOEM_NEOEMCue8StarfieldWarpDensity       358      // uint8_t
#define NEOEM_NEOEMCue8StarfieldWarpColorMode     359      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue8StarfieldWarpWarpPulse     360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue8StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue8PlasmaNebulaSpeed          357      // uint8_t
#define NEOEM_NEOEMCue8PlasmaNebulaSaturation     358      // uint8_t
#define NEOEM_NEOEMCue8PlasmaNebulaContrast       359      // uint8_t
#define NEOEM_NEOEMCue8PlasmaNebulaPaletteShift   360      // uint8_t
#define NEOEM_NEOEMCue8UFOSwarmSpeed              357      // uint8_t
#define NEOEM_NEOEMCue8UFOSwarmCount              358      // uint8_t
#define NEOEM_NEOEMCue8UFOSwarmHue                359      // uint8_t
#define NEOEM_NEOEMCue8UFOSwarmBeam               360      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue8UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue8UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue8GameofLife2DSpeed          357      // uint8_t
#define NEOEM_NEOEMCue8GameofLife2DHue            358      // uint8_t
#define NEOEM_NEOEMCue8GameofLife2DColorMode      359      // uint8_t
#define NEOEM_NEOEMCue8GameofLife2DSpawnDensity   360      // uint8_t
#define NEOEM_NEOEMCue8DNA2DSpeed                 357      // uint8_t
#define NEOEM_NEOEMCue8DNA2DHue                   358      // uint8_t
#define NEOEM_NEOEMCue8DNA2DTwist                 359      // uint8_t
#define NEOEM_NEOEMCue8DNA2DRungSpacing           360      // uint8_t
#define NEOEM_NEOEMCue8Aurora2DSpeed              357      // uint8_t
#define NEOEM_NEOEMCue8Aurora2DHue                358      // uint8_t
#define NEOEM_NEOEMCue8Aurora2DScale              359      // uint8_t
#define NEOEM_NEOEMCue8Aurora2DIntensity          360      // uint8_t
#define NEOEM_NEOEMCue8Lissajous2DSpeed           357      // uint8_t
#define NEOEM_NEOEMCue8Lissajous2DHue             358      // uint8_t
#define NEOEM_NEOEMCue8Lissajous2DFreqA           359      // uint8_t
#define NEOEM_NEOEMCue8Lissajous2DFreqB           360      // uint8_t
#define NEOEM_NEOEMCue8Metaballs2DSpeed           357      // uint8_t
#define NEOEM_NEOEMCue8Metaballs2DHue             358      // uint8_t
#define NEOEM_NEOEMCue8Metaballs2DBlobCount       359      // uint8_t
#define NEOEM_NEOEMCue8Metaballs2DContrast        360      // uint8_t
#define NEOEM_NEOEMCue9Effect                     404      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Param0                     405      // uint8_t
#define NEOEM_NEOEMCue9Param1                     406      // uint8_t
#define NEOEM_NEOEMCue9Param2                     407      // uint8_t
#define NEOEM_NEOEMCue9Param3                     408      // uint8_t
#define NEOEM_NEOEMCue9Param4                     409      // uint8_t
#define NEOEM_NEOEMCue9Param5                     410      // uint8_t
#define NEOEM_NEOEMCue9Param6                     411      // uint8_t
#define NEOEM_NEOEMCue9Param7                     412      // uint8_t
#define NEOEM_NEOEMCue9Param8                     413      // uint8_t
#define NEOEM_NEOEMCue9Param9                     414      // uint8_t
#define NEOEM_NEOEMCue9Color                      415      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue9ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue9ColorShift 8
#define NEOEM_NEOEMCue9W                          418      // uint8_t
#define NEOEM_NEOEMCue9Brightness                 419      // uint8_t
#define NEOEM_NEOEMCue9Duration                   420      // uint16_t
#define NEOEM_NEOEMCue9Fade                       422      // uint16_t
#define NEOEM_NEOEMCue9Text                       424      // char*, 14 Byte
#define     NEOEM_NEOEMCue9TextLength 14
#define NEOEM_NEOEMCue9EffectText                 438      // char*, 14 Byte
#define     NEOEM_NEOEMCue9EffectTextLength 14
#define NEOEM_NEOEMCue9WipeSpeed                  405      // uint8_t
#define NEOEM_NEOEMCue9WipeDirection              406      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9RainbowSpeed               405      // uint8_t
#define NEOEM_NEOEMCue9RainbowDelta               406      // uint8_t
#define NEOEM_NEOEMCue9RainbowSaturation          407      // uint8_t
#define NEOEM_NEOEMCue9RainbowDensity             408      // uint8_t
#define NEOEM_NEOEMCue9RainbowMode                409      // uint8_t
#define NEOEM_NEOEMCue9Pride2015Speed             405      // uint8_t
#define NEOEM_NEOEMCue9JuggleSpeed                405      // uint8_t
#define NEOEM_NEOEMCue9JuggleNumDots              406      // uint8_t
#define NEOEM_NEOEMCue9JuggleFadeSpeed            407      // uint8_t
#define NEOEM_NEOEMCue9JuggleHueOffset            408      // uint8_t
#define NEOEM_NEOEMCue9BPMBPM                     405      // uint8_t
#define NEOEM_NEOEMCue9BPMHue                     406      // uint8_t
#define NEOEM_NEOEMCue9CylonSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9CylonHue                   406      // uint8_t
#define NEOEM_NEOEMCue9CylonEyeSize               407      // uint8_t
#define NEOEM_NEOEMCue9CylonFadeAmount            408      // uint8_t
#define NEOEM_NEOEMCue9CylonDirection             409      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9CylonMode                  410      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9TestPhaseDuration          405      // uint8_t
#define NEOEM_NEOEMCue9TestMode                   406      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9FireSpeed                  405      // uint8_t
#define NEOEM_NEOEMCue9FireCooling                406      // uint8_t
#define NEOEM_NEOEMCue9FireSparking               407      // uint8_t
#define NEOEM_NEOEMCue9FireReverseDirection       408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue9FireReverseDirectionShift 7
#define NEOEM_NEOEMCue9FireBlueFireMode           409      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue9FireBlueFireModeShift 7
#define NEOEM_NEOEMCue9TheaterChaseSpeed          405      // uint8_t
#define NEOEM_NEOEMCue9TheaterChaseSpacing        406      // uint8_t
#define NEOEM_NEOEMCue9TheaterChaseDotSize        407      // uint8_t
#define NEOEM_NEOEMCue9TheaterChaseColorMode      408      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9TheaterChaseColorSpeed     409      // uint8_t
#define NEOEM_NEOEMCue9TheaterChaseBounce         410      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9SparkleSpeed               405      // uint8_t
#define NEOEM_NEOEMCue9SparkleFadeRate            406      // uint8_t
#define NEOEM_NEOEMCue9SparkleSparkleCount        407      // uint8_t
#define NEOEM_NEOEMCue9SparkleProbability         408      // uint8_t
#define NEOEM_NEOEMCue9SparkleWhiteOnly           409      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue9SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue9SparkleBurstMode           410      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue9SparkleBurstModeShift 7
#define NEOEM_NEOEMCue9SparkleMode                411      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9BreathingSpeed             405      // uint8_t
#define NEOEM_NEOEMCue9BreathingMinBrightness     406      // uint8_t
#define NEOEM_NEOEMCue9BreathingCurve             407      // uint8_t
#define NEOEM_NEOEMCue9BreathingRainbowMode       408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue9BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue9BreathingWaveform          409      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9StrobeSpeed                405      // uint8_t
#define NEOEM_NEOEMCue9StrobeOnRatio              406      // uint8_t
#define NEOEM_NEOEMCue9StrobeMinBrightness        407      // uint8_t
#define NEOEM_NEOEMCue9StrobeRandomTiming         408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue9StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue9StrobeRainbowStrobe        409      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue9StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue9CometSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9CometFadeRate              406      // uint8_t
#define NEOEM_NEOEMCue9CometTailLength            407      // uint8_t
#define NEOEM_NEOEMCue9CometBounceMode            408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue9CometBounceModeShift 7
#define NEOEM_NEOEMCue9CometRainbowMode           409      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue9CometRainbowModeShift 7
#define NEOEM_NEOEMCue9CometMode                  410      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9NoiseSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9NoiseScale                 406      // uint8_t
#define NEOEM_NEOEMCue9NoiseSaturation            407      // uint8_t
#define NEOEM_NEOEMCue9NoiseHueOffset             408      // uint8_t
#define NEOEM_NEOEMCue9NoisePalette               409      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9PaletteSpeed               405      // uint8_t
#define NEOEM_NEOEMCue9PalettePalette             406      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9PaletteBlend               407      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue9PaletteBlendShift 7
#define NEOEM_NEOEMCue9PaletteSpacing             408      // uint8_t
#define NEOEM_NEOEMCue9BlitzSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9BlitzWidth                 406      // uint8_t
#define NEOEM_NEOEMCue9BlitzDecay                 407      // uint8_t
#define NEOEM_NEOEMCue9BlitzHue                   408      // uint8_t
#define NEOEM_NEOEMCue9BlitzIntensity             409      // uint8_t
#define NEOEM_NEOEMCue9GradientSpeed              405      // uint8_t
#define NEOEM_NEOEMCue9GradientStartHue           406      // uint8_t
#define NEOEM_NEOEMCue9GradientEndHue             407      // uint8_t
#define NEOEM_NEOEMCue9GradientSaturation         408      // uint8_t
#define NEOEM_NEOEMCue9KerzeSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9KerzeIntensity             406      // uint8_t
#define NEOEM_NEOEMCue9KerzeZones                 407      // uint8_t
#define NEOEM_NEOEMCue9LaufschriftSpeed           405      // uint8_t
#define NEOEM_NEOEMCue9LaufschriftGap             406      // uint8_t
#define NEOEM_NEOEMCue9LaufschriftLoop            407      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue9LaufschriftLoopShift 7
#define NEOEM_NEOEMCue9LaufschriftText            438      // char*, 14 Byte
#define     NEOEM_NEOEMCue9LaufschriftTextLength 14
#define NEOEM_NEOEMCue9LaufschriftFont            409      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Uhr2DViewMode              405      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Uhr2DShowSeconds           406      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue9Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue9Uhr2DBlinkColon            407      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue9Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue9Uhr2DColourHue             408      // uint8_t
#define NEOEM_NEOEMCue9Uhr2DDateMode              409      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Uhr2DDateFormat            410      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Uhr2DDateHue               411      // uint8_t
#define NEOEM_NEOEMCue9Uhr2DSwitchSec             412      // uint8_t
#define NEOEM_NEOEMCue9Uhr2DScrollSpeed           413      // uint8_t
#define NEOEM_NEOEMCue9Uhr2DFont                  414      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Snake2DSpeed               405      // uint8_t
#define NEOEM_NEOEMCue9Snake2DHeadHue             406      // uint8_t
#define NEOEM_NEOEMCue9Snake2DBodyMode            407      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Snake2DBodyHue             408      // uint8_t
#define NEOEM_NEOEMCue9Matrix2DSpeed              405      // uint8_t
#define NEOEM_NEOEMCue9Matrix2DDensity            406      // uint8_t
#define NEOEM_NEOEMCue9Matrix2DPalette            407      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Matrix2DGlitch             408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue9Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue9Tetris2DSpeed              405      // uint8_t
#define NEOEM_NEOEMCue9Tetris2DBgBrightness       406      // uint8_t
#define NEOEM_NEOEMCue9Tetris2DGhostPiece         407      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue9Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue9Tetris2DColorMode          408      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9Tetris2DFlashLines         409      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue9Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue9Tetris2DAutoPlay           410      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9TRONSpeed                  405      // uint8_t
#define NEOEM_NEOEMCue9TRONHue                    406      // uint8_t
#define NEOEM_NEOEMCue9TRONGridSpacing            407      // uint8_t
#define NEOEM_NEOEMCue9TRONGlow                   408      // uint8_t
#define NEOEM_NEOEMCue9StarfieldWarpSpeed         405      // uint8_t
#define NEOEM_NEOEMCue9StarfieldWarpDensity       406      // uint8_t
#define NEOEM_NEOEMCue9StarfieldWarpColorMode     407      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue9StarfieldWarpWarpPulse     408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue9StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue9PlasmaNebulaSpeed          405      // uint8_t
#define NEOEM_NEOEMCue9PlasmaNebulaSaturation     406      // uint8_t
#define NEOEM_NEOEMCue9PlasmaNebulaContrast       407      // uint8_t
#define NEOEM_NEOEMCue9PlasmaNebulaPaletteShift   408      // uint8_t
#define NEOEM_NEOEMCue9UFOSwarmSpeed              405      // uint8_t
#define NEOEM_NEOEMCue9UFOSwarmCount              406      // uint8_t
#define NEOEM_NEOEMCue9UFOSwarmHue                407      // uint8_t
#define NEOEM_NEOEMCue9UFOSwarmBeam               408      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue9UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue9UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue9GameofLife2DSpeed          405      // uint8_t
#define NEOEM_NEOEMCue9GameofLife2DHue            406      // uint8_t
#define NEOEM_NEOEMCue9GameofLife2DColorMode      407      // uint8_t
#define NEOEM_NEOEMCue9GameofLife2DSpawnDensity   408      // uint8_t
#define NEOEM_NEOEMCue9DNA2DSpeed                 405      // uint8_t
#define NEOEM_NEOEMCue9DNA2DHue                   406      // uint8_t
#define NEOEM_NEOEMCue9DNA2DTwist                 407      // uint8_t
#define NEOEM_NEOEMCue9DNA2DRungSpacing           408      // uint8_t
#define NEOEM_NEOEMCue9Aurora2DSpeed              405      // uint8_t
#define NEOEM_NEOEMCue9Aurora2DHue                406      // uint8_t
#define NEOEM_NEOEMCue9Aurora2DScale              407      // uint8_t
#define NEOEM_NEOEMCue9Aurora2DIntensity          408      // uint8_t
#define NEOEM_NEOEMCue9Lissajous2DSpeed           405      // uint8_t
#define NEOEM_NEOEMCue9Lissajous2DHue             406      // uint8_t
#define NEOEM_NEOEMCue9Lissajous2DFreqA           407      // uint8_t
#define NEOEM_NEOEMCue9Lissajous2DFreqB           408      // uint8_t
#define NEOEM_NEOEMCue9Metaballs2DSpeed           405      // uint8_t
#define NEOEM_NEOEMCue9Metaballs2DHue             406      // uint8_t
#define NEOEM_NEOEMCue9Metaballs2DBlobCount       407      // uint8_t
#define NEOEM_NEOEMCue9Metaballs2DContrast        408      // uint8_t
#define NEOEM_NEOEMCue10Effect                    452      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Param0                    453      // uint8_t
#define NEOEM_NEOEMCue10Param1                    454      // uint8_t
#define NEOEM_NEOEMCue10Param2                    455      // uint8_t
#define NEOEM_NEOEMCue10Param3                    456      // uint8_t
#define NEOEM_NEOEMCue10Param4                    457      // uint8_t
#define NEOEM_NEOEMCue10Param5                    458      // uint8_t
#define NEOEM_NEOEMCue10Param6                    459      // uint8_t
#define NEOEM_NEOEMCue10Param7                    460      // uint8_t
#define NEOEM_NEOEMCue10Param8                    461      // uint8_t
#define NEOEM_NEOEMCue10Param9                    462      // uint8_t
#define NEOEM_NEOEMCue10Color                     463      // 24 Bits, Bit 31-8
#define     NEOEM_NEOEMCue10ColorMask 0xFFFFFF00
#define     NEOEM_NEOEMCue10ColorShift 8
#define NEOEM_NEOEMCue10W                         466      // uint8_t
#define NEOEM_NEOEMCue10Brightness                467      // uint8_t
#define NEOEM_NEOEMCue10Duration                  468      // uint16_t
#define NEOEM_NEOEMCue10Fade                      470      // uint16_t
#define NEOEM_NEOEMCue10Text                      472      // char*, 14 Byte
#define     NEOEM_NEOEMCue10TextLength 14
#define NEOEM_NEOEMCue10EffectText                486      // char*, 14 Byte
#define     NEOEM_NEOEMCue10EffectTextLength 14
#define NEOEM_NEOEMCue10WipeSpeed                 453      // uint8_t
#define NEOEM_NEOEMCue10WipeDirection             454      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10RainbowSpeed              453      // uint8_t
#define NEOEM_NEOEMCue10RainbowDelta              454      // uint8_t
#define NEOEM_NEOEMCue10RainbowSaturation         455      // uint8_t
#define NEOEM_NEOEMCue10RainbowDensity            456      // uint8_t
#define NEOEM_NEOEMCue10RainbowMode               457      // uint8_t
#define NEOEM_NEOEMCue10Pride2015Speed            453      // uint8_t
#define NEOEM_NEOEMCue10JuggleSpeed               453      // uint8_t
#define NEOEM_NEOEMCue10JuggleNumDots             454      // uint8_t
#define NEOEM_NEOEMCue10JuggleFadeSpeed           455      // uint8_t
#define NEOEM_NEOEMCue10JuggleHueOffset           456      // uint8_t
#define NEOEM_NEOEMCue10BPMBPM                    453      // uint8_t
#define NEOEM_NEOEMCue10BPMHue                    454      // uint8_t
#define NEOEM_NEOEMCue10CylonSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10CylonHue                  454      // uint8_t
#define NEOEM_NEOEMCue10CylonEyeSize              455      // uint8_t
#define NEOEM_NEOEMCue10CylonFadeAmount           456      // uint8_t
#define NEOEM_NEOEMCue10CylonDirection            457      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10CylonMode                 458      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10TestPhaseDuration         453      // uint8_t
#define NEOEM_NEOEMCue10TestMode                  454      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10FireSpeed                 453      // uint8_t
#define NEOEM_NEOEMCue10FireCooling               454      // uint8_t
#define NEOEM_NEOEMCue10FireSparking              455      // uint8_t
#define NEOEM_NEOEMCue10FireReverseDirection      456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10FireReverseDirectionMask 0x80
#define     NEOEM_NEOEMCue10FireReverseDirectionShift 7
#define NEOEM_NEOEMCue10FireBlueFireMode          457      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10FireBlueFireModeMask 0x80
#define     NEOEM_NEOEMCue10FireBlueFireModeShift 7
#define NEOEM_NEOEMCue10TheaterChaseSpeed         453      // uint8_t
#define NEOEM_NEOEMCue10TheaterChaseSpacing       454      // uint8_t
#define NEOEM_NEOEMCue10TheaterChaseDotSize       455      // uint8_t
#define NEOEM_NEOEMCue10TheaterChaseColorMode     456      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10TheaterChaseColorSpeed    457      // uint8_t
#define NEOEM_NEOEMCue10TheaterChaseBounce        458      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10SparkleSpeed              453      // uint8_t
#define NEOEM_NEOEMCue10SparkleFadeRate           454      // uint8_t
#define NEOEM_NEOEMCue10SparkleSparkleCount       455      // uint8_t
#define NEOEM_NEOEMCue10SparkleProbability        456      // uint8_t
#define NEOEM_NEOEMCue10SparkleWhiteOnly          457      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10SparkleWhiteOnlyMask 0x80
#define     NEOEM_NEOEMCue10SparkleWhiteOnlyShift 7
#define NEOEM_NEOEMCue10SparkleBurstMode          458      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10SparkleBurstModeMask 0x80
#define     NEOEM_NEOEMCue10SparkleBurstModeShift 7
#define NEOEM_NEOEMCue10SparkleMode               459      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10BreathingSpeed            453      // uint8_t
#define NEOEM_NEOEMCue10BreathingMinBrightness    454      // uint8_t
#define NEOEM_NEOEMCue10BreathingCurve            455      // uint8_t
#define NEOEM_NEOEMCue10BreathingRainbowMode      456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10BreathingRainbowModeMask 0x80
#define     NEOEM_NEOEMCue10BreathingRainbowModeShift 7
#define NEOEM_NEOEMCue10BreathingWaveform         457      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10StrobeSpeed               453      // uint8_t
#define NEOEM_NEOEMCue10StrobeOnRatio             454      // uint8_t
#define NEOEM_NEOEMCue10StrobeMinBrightness       455      // uint8_t
#define NEOEM_NEOEMCue10StrobeRandomTiming        456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10StrobeRandomTimingMask 0x80
#define     NEOEM_NEOEMCue10StrobeRandomTimingShift 7
#define NEOEM_NEOEMCue10StrobeRainbowStrobe       457      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10StrobeRainbowStrobeMask 0x80
#define     NEOEM_NEOEMCue10StrobeRainbowStrobeShift 7
#define NEOEM_NEOEMCue10CometSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10CometFadeRate             454      // uint8_t
#define NEOEM_NEOEMCue10CometTailLength           455      // uint8_t
#define NEOEM_NEOEMCue10CometBounceMode           456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10CometBounceModeMask 0x80
#define     NEOEM_NEOEMCue10CometBounceModeShift 7
#define NEOEM_NEOEMCue10CometRainbowMode          457      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10CometRainbowModeMask 0x80
#define     NEOEM_NEOEMCue10CometRainbowModeShift 7
#define NEOEM_NEOEMCue10CometMode                 458      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10NoiseSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10NoiseScale                454      // uint8_t
#define NEOEM_NEOEMCue10NoiseSaturation           455      // uint8_t
#define NEOEM_NEOEMCue10NoiseHueOffset            456      // uint8_t
#define NEOEM_NEOEMCue10NoisePalette              457      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10PaletteSpeed              453      // uint8_t
#define NEOEM_NEOEMCue10PalettePalette            454      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10PaletteBlend              455      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10PaletteBlendMask 0x80
#define     NEOEM_NEOEMCue10PaletteBlendShift 7
#define NEOEM_NEOEMCue10PaletteSpacing            456      // uint8_t
#define NEOEM_NEOEMCue10BlitzSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10BlitzWidth                454      // uint8_t
#define NEOEM_NEOEMCue10BlitzDecay                455      // uint8_t
#define NEOEM_NEOEMCue10BlitzHue                  456      // uint8_t
#define NEOEM_NEOEMCue10BlitzIntensity            457      // uint8_t
#define NEOEM_NEOEMCue10GradientSpeed             453      // uint8_t
#define NEOEM_NEOEMCue10GradientStartHue          454      // uint8_t
#define NEOEM_NEOEMCue10GradientEndHue            455      // uint8_t
#define NEOEM_NEOEMCue10GradientSaturation        456      // uint8_t
#define NEOEM_NEOEMCue10KerzeSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10KerzeIntensity            454      // uint8_t
#define NEOEM_NEOEMCue10KerzeZones                455      // uint8_t
#define NEOEM_NEOEMCue10LaufschriftSpeed          453      // uint8_t
#define NEOEM_NEOEMCue10LaufschriftGap            454      // uint8_t
#define NEOEM_NEOEMCue10LaufschriftLoop           455      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10LaufschriftLoopMask 0x80
#define     NEOEM_NEOEMCue10LaufschriftLoopShift 7
#define NEOEM_NEOEMCue10LaufschriftText           486      // char*, 14 Byte
#define     NEOEM_NEOEMCue10LaufschriftTextLength 14
#define NEOEM_NEOEMCue10LaufschriftFont           457      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Uhr2DViewMode             453      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Uhr2DShowSeconds          454      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10Uhr2DShowSecondsMask 0x80
#define     NEOEM_NEOEMCue10Uhr2DShowSecondsShift 7
#define NEOEM_NEOEMCue10Uhr2DBlinkColon           455      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10Uhr2DBlinkColonMask 0x80
#define     NEOEM_NEOEMCue10Uhr2DBlinkColonShift 7
#define NEOEM_NEOEMCue10Uhr2DColourHue            456      // uint8_t
#define NEOEM_NEOEMCue10Uhr2DDateMode             457      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Uhr2DDateFormat           458      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Uhr2DDateHue              459      // uint8_t
#define NEOEM_NEOEMCue10Uhr2DSwitchSec            460      // uint8_t
#define NEOEM_NEOEMCue10Uhr2DScrollSpeed          461      // uint8_t
#define NEOEM_NEOEMCue10Uhr2DFont                 462      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Snake2DSpeed              453      // uint8_t
#define NEOEM_NEOEMCue10Snake2DHeadHue            454      // uint8_t
#define NEOEM_NEOEMCue10Snake2DBodyMode           455      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Snake2DBodyHue            456      // uint8_t
#define NEOEM_NEOEMCue10Matrix2DSpeed             453      // uint8_t
#define NEOEM_NEOEMCue10Matrix2DDensity           454      // uint8_t
#define NEOEM_NEOEMCue10Matrix2DPalette           455      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Matrix2DGlitch            456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10Matrix2DGlitchMask 0x80
#define     NEOEM_NEOEMCue10Matrix2DGlitchShift 7
#define NEOEM_NEOEMCue10Tetris2DSpeed             453      // uint8_t
#define NEOEM_NEOEMCue10Tetris2DBgBrightness      454      // uint8_t
#define NEOEM_NEOEMCue10Tetris2DGhostPiece        455      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10Tetris2DGhostPieceMask 0x80
#define     NEOEM_NEOEMCue10Tetris2DGhostPieceShift 7
#define NEOEM_NEOEMCue10Tetris2DColorMode         456      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10Tetris2DFlashLines        457      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10Tetris2DFlashLinesMask 0x80
#define     NEOEM_NEOEMCue10Tetris2DFlashLinesShift 7
#define NEOEM_NEOEMCue10Tetris2DAutoPlay          458      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10TRONSpeed                 453      // uint8_t
#define NEOEM_NEOEMCue10TRONHue                   454      // uint8_t
#define NEOEM_NEOEMCue10TRONGridSpacing           455      // uint8_t
#define NEOEM_NEOEMCue10TRONGlow                  456      // uint8_t
#define NEOEM_NEOEMCue10StarfieldWarpSpeed        453      // uint8_t
#define NEOEM_NEOEMCue10StarfieldWarpDensity      454      // uint8_t
#define NEOEM_NEOEMCue10StarfieldWarpColorMode    455      // 8 Bits, Bit 7-0
#define NEOEM_NEOEMCue10StarfieldWarpWarpPulse    456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10StarfieldWarpWarpPulseMask 0x80
#define     NEOEM_NEOEMCue10StarfieldWarpWarpPulseShift 7
#define NEOEM_NEOEMCue10PlasmaNebulaSpeed         453      // uint8_t
#define NEOEM_NEOEMCue10PlasmaNebulaSaturation    454      // uint8_t
#define NEOEM_NEOEMCue10PlasmaNebulaContrast      455      // uint8_t
#define NEOEM_NEOEMCue10PlasmaNebulaPaletteShift  456      // uint8_t
#define NEOEM_NEOEMCue10UFOSwarmSpeed             453      // uint8_t
#define NEOEM_NEOEMCue10UFOSwarmCount             454      // uint8_t
#define NEOEM_NEOEMCue10UFOSwarmHue               455      // uint8_t
#define NEOEM_NEOEMCue10UFOSwarmBeam              456      // 1 Bit, Bit 7
#define     NEOEM_NEOEMCue10UFOSwarmBeamMask 0x80
#define     NEOEM_NEOEMCue10UFOSwarmBeamShift 7
#define NEOEM_NEOEMCue10GameofLife2DSpeed         453      // uint8_t
#define NEOEM_NEOEMCue10GameofLife2DHue           454      // uint8_t
#define NEOEM_NEOEMCue10GameofLife2DColorMode     455      // uint8_t
#define NEOEM_NEOEMCue10GameofLife2DSpawnDensity  456      // uint8_t
#define NEOEM_NEOEMCue10DNA2DSpeed                453      // uint8_t
#define NEOEM_NEOEMCue10DNA2DHue                  454      // uint8_t
#define NEOEM_NEOEMCue10DNA2DTwist                455      // uint8_t
#define NEOEM_NEOEMCue10DNA2DRungSpacing          456      // uint8_t
#define NEOEM_NEOEMCue10Aurora2DSpeed             453      // uint8_t
#define NEOEM_NEOEMCue10Aurora2DHue               454      // uint8_t
#define NEOEM_NEOEMCue10Aurora2DScale             455      // uint8_t
#define NEOEM_NEOEMCue10Aurora2DIntensity         456      // uint8_t
#define NEOEM_NEOEMCue10Lissajous2DSpeed          453      // uint8_t
#define NEOEM_NEOEMCue10Lissajous2DHue            454      // uint8_t
#define NEOEM_NEOEMCue10Lissajous2DFreqA          455      // uint8_t
#define NEOEM_NEOEMCue10Lissajous2DFreqB          456      // uint8_t
#define NEOEM_NEOEMCue10Metaballs2DSpeed          453      // uint8_t
#define NEOEM_NEOEMCue10Metaballs2DHue            454      // uint8_t
#define NEOEM_NEOEMCue10Metaballs2DBlobCount      455      // uint8_t
#define NEOEM_NEOEMCue10Metaballs2DContrast       456      // uint8_t

// Anzahl aktiver Cues
#define ParamNEOEM_NEOEMCueCount                       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCueCount)))
// Loop
#define ParamNEOEM_NEOEMLoop                           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMLoop)) & NEOEM_NEOEMLoopMask))
// Effekt-Text KOs aktivieren
#define ParamNEOEM_NEOEMTextKOs                        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMTextKOs)) & NEOEM_NEOEMTextKOsMask))
// Wenn fertig → Effektmanager
#define ParamNEOEM_NEOEMNext                           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMNext)))
// Zustand
#define ParamNEOEM_NEOEMEnabled                        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMEnabled)))
// Effekt
#define ParamNEOEM_NEOEMCue1Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue1Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue1Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue1Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue1Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue1Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue1Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue1Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue1Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue1Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue1Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue1Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Color)) & NEOEM_NEOEMCue1ColorMask) >> NEOEM_NEOEMCue1ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue1W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue1Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue1Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue1Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue1Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Text)))
#define ParamNEOEM_NEOEMCue1TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Text), NEOEM_NEOEMCue1TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue1EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1EffectText)))
#define ParamNEOEM_NEOEMCue1EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1EffectText), NEOEM_NEOEMCue1EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue1WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue1WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue1RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue1RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue1RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue1RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue1RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue1Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue1JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue1JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue1JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue1JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue1BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue1BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue1CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue1CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue1CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue1CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue1CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue1TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue1TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue1FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue1FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue1FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue1FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1FireReverseDirection)) & NEOEM_NEOEMCue1FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue1FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1FireBlueFireMode)) & NEOEM_NEOEMCue1FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue1TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue1TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue1TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue1TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue1TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue1TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue1SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue1SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue1SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue1SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue1SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleWhiteOnly)) & NEOEM_NEOEMCue1SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue1SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleBurstMode)) & NEOEM_NEOEMCue1SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue1SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue1BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue1BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue1BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue1BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BreathingRainbowMode)) & NEOEM_NEOEMCue1BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue1BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue1StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue1StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue1StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue1StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StrobeRandomTiming)) & NEOEM_NEOEMCue1StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue1StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StrobeRainbowStrobe)) & NEOEM_NEOEMCue1StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue1CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue1CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue1CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue1CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometBounceMode)) & NEOEM_NEOEMCue1CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue1CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometRainbowMode)) & NEOEM_NEOEMCue1CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue1CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue1NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue1NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue1NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue1NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue1NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue1PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue1PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue1PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PaletteBlend)) & NEOEM_NEOEMCue1PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue1PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue1BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue1BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue1BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue1BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue1BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue1GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue1GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue1GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue1GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue1KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue1KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue1KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue1LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue1LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue1LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftLoop)) & NEOEM_NEOEMCue1LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue1LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftText)))
#define ParamNEOEM_NEOEMCue1LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftText), NEOEM_NEOEMCue1LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue1LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue1Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue1Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DShowSeconds)) & NEOEM_NEOEMCue1Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue1Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DBlinkColon)) & NEOEM_NEOEMCue1Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue1Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue1Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue1Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue1Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue1Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue1Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue1Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue1Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue1Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue1Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue1Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue1Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue1Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue1Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue1Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Matrix2DGlitch)) & NEOEM_NEOEMCue1Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue1Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue1Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue1Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DGhostPiece)) & NEOEM_NEOEMCue1Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue1Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue1Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DFlashLines)) & NEOEM_NEOEMCue1Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue1Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue1TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue1TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue1TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue1StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue1StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue1StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue1StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue1StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue1PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue1PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue1PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue1PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue1UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue1UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue1UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue1UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1UFOSwarmBeam)) & NEOEM_NEOEMCue1UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue1GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue1GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue1GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue1DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue1DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue1DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue1Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue1Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue1Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue1Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue1Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue1Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue1Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue1Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue1Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue1Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue1Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue2Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue2Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue2Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue2Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue2Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue2Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue2Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue2Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue2Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue2Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue2Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue2Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Color)) & NEOEM_NEOEMCue2ColorMask) >> NEOEM_NEOEMCue2ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue2W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue2Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue2Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue2Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue2Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Text)))
#define ParamNEOEM_NEOEMCue2TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Text), NEOEM_NEOEMCue2TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue2EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2EffectText)))
#define ParamNEOEM_NEOEMCue2EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2EffectText), NEOEM_NEOEMCue2EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue2WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue2WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue2RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue2RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue2RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue2RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue2RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue2Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue2JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue2JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue2JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue2JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue2BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue2BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue2CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue2CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue2CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue2CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue2CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue2TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue2TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue2FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue2FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue2FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue2FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2FireReverseDirection)) & NEOEM_NEOEMCue2FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue2FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2FireBlueFireMode)) & NEOEM_NEOEMCue2FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue2TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue2TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue2TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue2TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue2TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue2TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue2SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue2SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue2SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue2SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue2SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleWhiteOnly)) & NEOEM_NEOEMCue2SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue2SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleBurstMode)) & NEOEM_NEOEMCue2SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue2SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue2BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue2BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue2BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue2BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BreathingRainbowMode)) & NEOEM_NEOEMCue2BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue2BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue2StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue2StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue2StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue2StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StrobeRandomTiming)) & NEOEM_NEOEMCue2StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue2StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StrobeRainbowStrobe)) & NEOEM_NEOEMCue2StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue2CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue2CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue2CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue2CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometBounceMode)) & NEOEM_NEOEMCue2CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue2CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometRainbowMode)) & NEOEM_NEOEMCue2CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue2CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue2NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue2NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue2NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue2NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue2NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue2PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue2PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue2PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PaletteBlend)) & NEOEM_NEOEMCue2PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue2PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue2BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue2BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue2BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue2BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue2BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue2GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue2GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue2GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue2GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue2KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue2KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue2KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue2LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue2LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue2LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftLoop)) & NEOEM_NEOEMCue2LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue2LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftText)))
#define ParamNEOEM_NEOEMCue2LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftText), NEOEM_NEOEMCue2LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue2LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue2Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue2Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DShowSeconds)) & NEOEM_NEOEMCue2Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue2Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DBlinkColon)) & NEOEM_NEOEMCue2Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue2Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue2Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue2Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue2Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue2Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue2Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue2Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue2Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue2Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue2Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue2Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue2Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue2Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue2Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue2Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Matrix2DGlitch)) & NEOEM_NEOEMCue2Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue2Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue2Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue2Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DGhostPiece)) & NEOEM_NEOEMCue2Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue2Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue2Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DFlashLines)) & NEOEM_NEOEMCue2Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue2Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue2TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue2TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue2TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue2StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue2StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue2StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue2StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue2StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue2PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue2PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue2PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue2PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue2UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue2UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue2UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue2UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2UFOSwarmBeam)) & NEOEM_NEOEMCue2UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue2GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue2GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue2GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue2DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue2DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue2DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue2Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue2Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue2Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue2Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue2Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue2Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue2Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue2Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue2Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue2Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue2Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue3Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue3Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue3Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue3Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue3Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue3Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue3Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue3Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue3Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue3Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue3Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue3Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Color)) & NEOEM_NEOEMCue3ColorMask) >> NEOEM_NEOEMCue3ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue3W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue3Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue3Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue3Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue3Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Text)))
#define ParamNEOEM_NEOEMCue3TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Text), NEOEM_NEOEMCue3TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue3EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3EffectText)))
#define ParamNEOEM_NEOEMCue3EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3EffectText), NEOEM_NEOEMCue3EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue3WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue3WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue3RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue3RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue3RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue3RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue3RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue3Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue3JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue3JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue3JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue3JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue3BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue3BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue3CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue3CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue3CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue3CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue3CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue3TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue3TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue3FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue3FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue3FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue3FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3FireReverseDirection)) & NEOEM_NEOEMCue3FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue3FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3FireBlueFireMode)) & NEOEM_NEOEMCue3FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue3TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue3TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue3TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue3TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue3TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue3TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue3SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue3SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue3SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue3SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue3SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleWhiteOnly)) & NEOEM_NEOEMCue3SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue3SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleBurstMode)) & NEOEM_NEOEMCue3SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue3SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue3BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue3BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue3BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue3BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BreathingRainbowMode)) & NEOEM_NEOEMCue3BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue3BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue3StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue3StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue3StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue3StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StrobeRandomTiming)) & NEOEM_NEOEMCue3StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue3StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StrobeRainbowStrobe)) & NEOEM_NEOEMCue3StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue3CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue3CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue3CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue3CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometBounceMode)) & NEOEM_NEOEMCue3CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue3CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometRainbowMode)) & NEOEM_NEOEMCue3CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue3CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue3NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue3NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue3NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue3NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue3NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue3PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue3PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue3PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PaletteBlend)) & NEOEM_NEOEMCue3PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue3PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue3BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue3BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue3BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue3BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue3BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue3GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue3GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue3GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue3GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue3KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue3KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue3KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue3LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue3LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue3LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftLoop)) & NEOEM_NEOEMCue3LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue3LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftText)))
#define ParamNEOEM_NEOEMCue3LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftText), NEOEM_NEOEMCue3LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue3LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue3Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue3Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DShowSeconds)) & NEOEM_NEOEMCue3Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue3Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DBlinkColon)) & NEOEM_NEOEMCue3Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue3Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue3Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue3Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue3Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue3Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue3Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue3Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue3Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue3Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue3Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue3Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue3Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue3Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue3Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue3Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Matrix2DGlitch)) & NEOEM_NEOEMCue3Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue3Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue3Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue3Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DGhostPiece)) & NEOEM_NEOEMCue3Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue3Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue3Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DFlashLines)) & NEOEM_NEOEMCue3Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue3Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue3TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue3TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue3TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue3StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue3StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue3StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue3StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue3StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue3PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue3PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue3PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue3PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue3UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue3UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue3UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue3UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3UFOSwarmBeam)) & NEOEM_NEOEMCue3UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue3GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue3GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue3GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue3DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue3DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue3DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue3Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue3Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue3Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue3Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue3Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue3Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue3Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue3Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue3Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue3Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue3Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue4Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue4Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue4Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue4Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue4Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue4Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue4Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue4Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue4Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue4Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue4Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue4Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Color)) & NEOEM_NEOEMCue4ColorMask) >> NEOEM_NEOEMCue4ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue4W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue4Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue4Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue4Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue4Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Text)))
#define ParamNEOEM_NEOEMCue4TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Text), NEOEM_NEOEMCue4TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue4EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4EffectText)))
#define ParamNEOEM_NEOEMCue4EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4EffectText), NEOEM_NEOEMCue4EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue4WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue4WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue4RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue4RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue4RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue4RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue4RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue4Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue4JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue4JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue4JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue4JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue4BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue4BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue4CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue4CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue4CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue4CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue4CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue4TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue4TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue4FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue4FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue4FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue4FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4FireReverseDirection)) & NEOEM_NEOEMCue4FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue4FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4FireBlueFireMode)) & NEOEM_NEOEMCue4FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue4TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue4TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue4TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue4TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue4TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue4TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue4SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue4SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue4SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue4SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue4SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleWhiteOnly)) & NEOEM_NEOEMCue4SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue4SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleBurstMode)) & NEOEM_NEOEMCue4SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue4SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue4BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue4BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue4BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue4BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BreathingRainbowMode)) & NEOEM_NEOEMCue4BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue4BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue4StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue4StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue4StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue4StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StrobeRandomTiming)) & NEOEM_NEOEMCue4StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue4StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StrobeRainbowStrobe)) & NEOEM_NEOEMCue4StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue4CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue4CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue4CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue4CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometBounceMode)) & NEOEM_NEOEMCue4CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue4CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometRainbowMode)) & NEOEM_NEOEMCue4CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue4CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue4NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue4NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue4NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue4NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue4NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue4PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue4PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue4PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PaletteBlend)) & NEOEM_NEOEMCue4PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue4PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue4BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue4BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue4BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue4BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue4BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue4GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue4GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue4GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue4GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue4KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue4KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue4KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue4LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue4LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue4LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftLoop)) & NEOEM_NEOEMCue4LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue4LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftText)))
#define ParamNEOEM_NEOEMCue4LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftText), NEOEM_NEOEMCue4LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue4LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue4Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue4Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DShowSeconds)) & NEOEM_NEOEMCue4Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue4Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DBlinkColon)) & NEOEM_NEOEMCue4Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue4Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue4Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue4Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue4Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue4Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue4Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue4Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue4Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue4Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue4Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue4Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue4Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue4Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue4Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue4Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Matrix2DGlitch)) & NEOEM_NEOEMCue4Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue4Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue4Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue4Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DGhostPiece)) & NEOEM_NEOEMCue4Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue4Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue4Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DFlashLines)) & NEOEM_NEOEMCue4Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue4Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue4TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue4TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue4TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue4StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue4StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue4StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue4StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue4StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue4PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue4PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue4PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue4PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue4UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue4UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue4UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue4UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4UFOSwarmBeam)) & NEOEM_NEOEMCue4UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue4GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue4GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue4GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue4DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue4DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue4DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue4Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue4Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue4Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue4Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue4Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue4Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue4Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue4Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue4Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue4Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue4Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue5Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue5Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue5Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue5Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue5Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue5Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue5Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue5Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue5Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue5Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue5Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue5Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Color)) & NEOEM_NEOEMCue5ColorMask) >> NEOEM_NEOEMCue5ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue5W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue5Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue5Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue5Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue5Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Text)))
#define ParamNEOEM_NEOEMCue5TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Text), NEOEM_NEOEMCue5TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue5EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5EffectText)))
#define ParamNEOEM_NEOEMCue5EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5EffectText), NEOEM_NEOEMCue5EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue5WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue5WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue5RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue5RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue5RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue5RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue5RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue5Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue5JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue5JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue5JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue5JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue5BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue5BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue5CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue5CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue5CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue5CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue5CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue5TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue5TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue5FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue5FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue5FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue5FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5FireReverseDirection)) & NEOEM_NEOEMCue5FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue5FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5FireBlueFireMode)) & NEOEM_NEOEMCue5FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue5TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue5TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue5TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue5TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue5TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue5TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue5SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue5SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue5SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue5SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue5SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleWhiteOnly)) & NEOEM_NEOEMCue5SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue5SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleBurstMode)) & NEOEM_NEOEMCue5SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue5SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue5BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue5BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue5BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue5BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BreathingRainbowMode)) & NEOEM_NEOEMCue5BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue5BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue5StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue5StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue5StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue5StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StrobeRandomTiming)) & NEOEM_NEOEMCue5StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue5StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StrobeRainbowStrobe)) & NEOEM_NEOEMCue5StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue5CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue5CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue5CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue5CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometBounceMode)) & NEOEM_NEOEMCue5CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue5CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometRainbowMode)) & NEOEM_NEOEMCue5CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue5CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue5NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue5NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue5NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue5NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue5NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue5PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue5PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue5PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PaletteBlend)) & NEOEM_NEOEMCue5PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue5PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue5BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue5BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue5BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue5BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue5BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue5GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue5GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue5GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue5GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue5KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue5KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue5KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue5LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue5LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue5LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftLoop)) & NEOEM_NEOEMCue5LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue5LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftText)))
#define ParamNEOEM_NEOEMCue5LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftText), NEOEM_NEOEMCue5LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue5LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue5Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue5Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DShowSeconds)) & NEOEM_NEOEMCue5Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue5Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DBlinkColon)) & NEOEM_NEOEMCue5Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue5Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue5Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue5Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue5Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue5Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue5Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue5Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue5Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue5Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue5Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue5Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue5Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue5Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue5Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue5Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Matrix2DGlitch)) & NEOEM_NEOEMCue5Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue5Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue5Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue5Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DGhostPiece)) & NEOEM_NEOEMCue5Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue5Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue5Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DFlashLines)) & NEOEM_NEOEMCue5Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue5Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue5TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue5TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue5TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue5StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue5StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue5StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue5StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue5StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue5PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue5PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue5PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue5PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue5UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue5UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue5UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue5UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5UFOSwarmBeam)) & NEOEM_NEOEMCue5UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue5GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue5GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue5GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue5DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue5DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue5DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue5Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue5Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue5Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue5Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue5Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue5Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue5Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue5Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue5Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue5Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue5Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue6Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue6Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue6Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue6Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue6Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue6Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue6Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue6Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue6Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue6Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue6Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue6Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Color)) & NEOEM_NEOEMCue6ColorMask) >> NEOEM_NEOEMCue6ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue6W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue6Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue6Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue6Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue6Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Text)))
#define ParamNEOEM_NEOEMCue6TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Text), NEOEM_NEOEMCue6TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue6EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6EffectText)))
#define ParamNEOEM_NEOEMCue6EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6EffectText), NEOEM_NEOEMCue6EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue6WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue6WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue6RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue6RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue6RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue6RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue6RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue6Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue6JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue6JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue6JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue6JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue6BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue6BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue6CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue6CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue6CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue6CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue6CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue6TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue6TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue6FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue6FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue6FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue6FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6FireReverseDirection)) & NEOEM_NEOEMCue6FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue6FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6FireBlueFireMode)) & NEOEM_NEOEMCue6FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue6TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue6TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue6TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue6TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue6TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue6TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue6SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue6SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue6SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue6SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue6SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleWhiteOnly)) & NEOEM_NEOEMCue6SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue6SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleBurstMode)) & NEOEM_NEOEMCue6SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue6SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue6BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue6BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue6BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue6BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BreathingRainbowMode)) & NEOEM_NEOEMCue6BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue6BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue6StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue6StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue6StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue6StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StrobeRandomTiming)) & NEOEM_NEOEMCue6StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue6StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StrobeRainbowStrobe)) & NEOEM_NEOEMCue6StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue6CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue6CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue6CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue6CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometBounceMode)) & NEOEM_NEOEMCue6CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue6CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometRainbowMode)) & NEOEM_NEOEMCue6CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue6CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue6NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue6NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue6NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue6NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue6NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue6PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue6PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue6PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PaletteBlend)) & NEOEM_NEOEMCue6PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue6PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue6BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue6BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue6BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue6BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue6BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue6GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue6GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue6GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue6GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue6KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue6KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue6KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue6LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue6LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue6LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftLoop)) & NEOEM_NEOEMCue6LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue6LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftText)))
#define ParamNEOEM_NEOEMCue6LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftText), NEOEM_NEOEMCue6LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue6LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue6Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue6Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DShowSeconds)) & NEOEM_NEOEMCue6Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue6Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DBlinkColon)) & NEOEM_NEOEMCue6Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue6Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue6Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue6Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue6Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue6Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue6Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue6Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue6Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue6Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue6Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue6Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue6Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue6Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue6Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue6Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Matrix2DGlitch)) & NEOEM_NEOEMCue6Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue6Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue6Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue6Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DGhostPiece)) & NEOEM_NEOEMCue6Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue6Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue6Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DFlashLines)) & NEOEM_NEOEMCue6Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue6Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue6TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue6TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue6TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue6StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue6StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue6StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue6StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue6StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue6PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue6PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue6PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue6PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue6UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue6UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue6UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue6UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6UFOSwarmBeam)) & NEOEM_NEOEMCue6UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue6GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue6GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue6GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue6DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue6DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue6DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue6Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue6Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue6Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue6Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue6Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue6Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue6Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue6Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue6Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue6Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue6Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue7Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue7Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue7Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue7Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue7Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue7Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue7Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue7Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue7Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue7Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue7Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue7Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Color)) & NEOEM_NEOEMCue7ColorMask) >> NEOEM_NEOEMCue7ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue7W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue7Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue7Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue7Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue7Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Text)))
#define ParamNEOEM_NEOEMCue7TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Text), NEOEM_NEOEMCue7TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue7EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7EffectText)))
#define ParamNEOEM_NEOEMCue7EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7EffectText), NEOEM_NEOEMCue7EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue7WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue7WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue7RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue7RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue7RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue7RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue7RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue7Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue7JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue7JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue7JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue7JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue7BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue7BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue7CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue7CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue7CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue7CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue7CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue7TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue7TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue7FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue7FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue7FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue7FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7FireReverseDirection)) & NEOEM_NEOEMCue7FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue7FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7FireBlueFireMode)) & NEOEM_NEOEMCue7FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue7TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue7TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue7TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue7TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue7TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue7TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue7SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue7SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue7SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue7SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue7SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleWhiteOnly)) & NEOEM_NEOEMCue7SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue7SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleBurstMode)) & NEOEM_NEOEMCue7SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue7SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue7BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue7BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue7BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue7BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BreathingRainbowMode)) & NEOEM_NEOEMCue7BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue7BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue7StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue7StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue7StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue7StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StrobeRandomTiming)) & NEOEM_NEOEMCue7StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue7StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StrobeRainbowStrobe)) & NEOEM_NEOEMCue7StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue7CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue7CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue7CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue7CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometBounceMode)) & NEOEM_NEOEMCue7CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue7CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometRainbowMode)) & NEOEM_NEOEMCue7CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue7CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue7NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue7NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue7NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue7NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue7NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue7PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue7PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue7PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PaletteBlend)) & NEOEM_NEOEMCue7PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue7PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue7BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue7BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue7BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue7BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue7BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue7GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue7GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue7GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue7GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue7KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue7KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue7KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue7LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue7LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue7LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftLoop)) & NEOEM_NEOEMCue7LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue7LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftText)))
#define ParamNEOEM_NEOEMCue7LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftText), NEOEM_NEOEMCue7LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue7LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue7Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue7Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DShowSeconds)) & NEOEM_NEOEMCue7Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue7Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DBlinkColon)) & NEOEM_NEOEMCue7Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue7Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue7Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue7Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue7Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue7Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue7Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue7Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue7Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue7Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue7Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue7Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue7Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue7Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue7Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue7Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Matrix2DGlitch)) & NEOEM_NEOEMCue7Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue7Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue7Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue7Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DGhostPiece)) & NEOEM_NEOEMCue7Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue7Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue7Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DFlashLines)) & NEOEM_NEOEMCue7Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue7Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue7TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue7TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue7TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue7StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue7StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue7StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue7StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue7StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue7PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue7PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue7PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue7PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue7UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue7UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue7UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue7UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7UFOSwarmBeam)) & NEOEM_NEOEMCue7UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue7GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue7GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue7GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue7DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue7DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue7DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue7Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue7Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue7Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue7Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue7Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue7Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue7Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue7Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue7Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue7Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue7Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue8Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue8Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue8Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue8Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue8Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue8Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue8Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue8Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue8Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue8Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue8Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue8Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Color)) & NEOEM_NEOEMCue8ColorMask) >> NEOEM_NEOEMCue8ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue8W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue8Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue8Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue8Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue8Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Text)))
#define ParamNEOEM_NEOEMCue8TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Text), NEOEM_NEOEMCue8TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue8EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8EffectText)))
#define ParamNEOEM_NEOEMCue8EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8EffectText), NEOEM_NEOEMCue8EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue8WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue8WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue8RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue8RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue8RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue8RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue8RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue8Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue8JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue8JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue8JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue8JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue8BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue8BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue8CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue8CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue8CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue8CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue8CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue8TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue8TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue8FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue8FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue8FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue8FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8FireReverseDirection)) & NEOEM_NEOEMCue8FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue8FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8FireBlueFireMode)) & NEOEM_NEOEMCue8FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue8TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue8TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue8TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue8TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue8TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue8TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue8SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue8SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue8SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue8SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue8SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleWhiteOnly)) & NEOEM_NEOEMCue8SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue8SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleBurstMode)) & NEOEM_NEOEMCue8SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue8SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue8BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue8BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue8BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue8BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BreathingRainbowMode)) & NEOEM_NEOEMCue8BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue8BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue8StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue8StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue8StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue8StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StrobeRandomTiming)) & NEOEM_NEOEMCue8StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue8StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StrobeRainbowStrobe)) & NEOEM_NEOEMCue8StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue8CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue8CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue8CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue8CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometBounceMode)) & NEOEM_NEOEMCue8CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue8CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometRainbowMode)) & NEOEM_NEOEMCue8CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue8CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue8NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue8NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue8NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue8NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue8NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue8PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue8PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue8PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PaletteBlend)) & NEOEM_NEOEMCue8PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue8PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue8BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue8BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue8BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue8BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue8BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue8GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue8GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue8GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue8GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue8KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue8KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue8KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue8LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue8LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue8LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftLoop)) & NEOEM_NEOEMCue8LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue8LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftText)))
#define ParamNEOEM_NEOEMCue8LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftText), NEOEM_NEOEMCue8LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue8LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue8Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue8Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DShowSeconds)) & NEOEM_NEOEMCue8Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue8Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DBlinkColon)) & NEOEM_NEOEMCue8Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue8Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue8Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue8Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue8Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue8Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue8Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue8Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue8Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue8Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue8Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue8Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue8Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue8Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue8Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue8Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Matrix2DGlitch)) & NEOEM_NEOEMCue8Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue8Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue8Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue8Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DGhostPiece)) & NEOEM_NEOEMCue8Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue8Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue8Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DFlashLines)) & NEOEM_NEOEMCue8Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue8Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue8TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue8TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue8TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue8StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue8StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue8StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue8StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue8StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue8PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue8PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue8PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue8PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue8UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue8UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue8UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue8UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8UFOSwarmBeam)) & NEOEM_NEOEMCue8UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue8GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue8GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue8GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue8DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue8DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue8DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue8Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue8Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue8Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue8Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue8Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue8Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue8Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue8Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue8Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue8Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue8Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue9Effect                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue9Param0                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue9Param1                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue9Param2                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue9Param3                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue9Param4                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue9Param5                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue9Param6                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue9Param7                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue9Param8                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue9Param9                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue9Color                      ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Color)) & NEOEM_NEOEMCue9ColorMask) >> NEOEM_NEOEMCue9ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue9W                          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue9Brightness                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue9Duration                   (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue9Fade                       (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue9Text                       (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Text)))
#define ParamNEOEM_NEOEMCue9TextStr                    (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Text), NEOEM_NEOEMCue9TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue9EffectText                 (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9EffectText)))
#define ParamNEOEM_NEOEMCue9EffectTextStr              (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9EffectText), NEOEM_NEOEMCue9EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue9WipeSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue9WipeDirection              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue9RainbowSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue9RainbowDelta               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue9RainbowSaturation          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue9RainbowDensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue9RainbowMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue9Pride2015Speed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue9JuggleSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue9JuggleNumDots              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue9JuggleFadeSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue9JuggleHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue9BPMBPM                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue9BPMHue                     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue9CylonSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9CylonHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue9CylonEyeSize               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue9CylonFadeAmount            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue9CylonDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue9CylonMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue9TestPhaseDuration          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue9TestMode                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue9FireSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue9FireCooling                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue9FireSparking               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue9FireReverseDirection       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9FireReverseDirection)) & NEOEM_NEOEMCue9FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue9FireBlueFireMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9FireBlueFireMode)) & NEOEM_NEOEMCue9FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue9TheaterChaseSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue9TheaterChaseSpacing        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue9TheaterChaseDotSize        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue9TheaterChaseColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue9TheaterChaseColorSpeed     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue9TheaterChaseBounce         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue9SparkleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue9SparkleFadeRate            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue9SparkleSparkleCount        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue9SparkleProbability         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue9SparkleWhiteOnly           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleWhiteOnly)) & NEOEM_NEOEMCue9SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue9SparkleBurstMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleBurstMode)) & NEOEM_NEOEMCue9SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue9SparkleMode                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue9BreathingSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue9BreathingMinBrightness     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue9BreathingCurve             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue9BreathingRainbowMode       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BreathingRainbowMode)) & NEOEM_NEOEMCue9BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue9BreathingWaveform          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue9StrobeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue9StrobeOnRatio              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue9StrobeMinBrightness        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue9StrobeRandomTiming         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StrobeRandomTiming)) & NEOEM_NEOEMCue9StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue9StrobeRainbowStrobe        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StrobeRainbowStrobe)) & NEOEM_NEOEMCue9StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue9CometSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue9CometFadeRate              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue9CometTailLength            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue9CometBounceMode            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometBounceMode)) & NEOEM_NEOEMCue9CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue9CometRainbowMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometRainbowMode)) & NEOEM_NEOEMCue9CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue9CometMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue9NoiseSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue9NoiseScale                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue9NoiseSaturation            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue9NoiseHueOffset             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue9NoisePalette               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue9PaletteSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue9PalettePalette             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue9PaletteBlend               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PaletteBlend)) & NEOEM_NEOEMCue9PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue9PaletteSpacing             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue9BlitzSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue9BlitzWidth                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue9BlitzDecay                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue9BlitzHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue9BlitzIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue9GradientSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue9GradientStartHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue9GradientEndHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue9GradientSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue9KerzeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue9KerzeIntensity             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue9KerzeZones                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue9LaufschriftSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue9LaufschriftGap             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue9LaufschriftLoop            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftLoop)) & NEOEM_NEOEMCue9LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue9LaufschriftText            (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftText)))
#define ParamNEOEM_NEOEMCue9LaufschriftTextStr         (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftText), NEOEM_NEOEMCue9LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue9LaufschriftFont            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue9Uhr2DViewMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue9Uhr2DShowSeconds           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DShowSeconds)) & NEOEM_NEOEMCue9Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue9Uhr2DBlinkColon            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DBlinkColon)) & NEOEM_NEOEMCue9Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue9Uhr2DColourHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue9Uhr2DDateMode              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue9Uhr2DDateFormat            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue9Uhr2DDateHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue9Uhr2DSwitchSec             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue9Uhr2DScrollSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue9Uhr2DFont                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue9Snake2DSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue9Snake2DHeadHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue9Snake2DBodyMode            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue9Snake2DBodyHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue9Matrix2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue9Matrix2DDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue9Matrix2DPalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue9Matrix2DGlitch             ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Matrix2DGlitch)) & NEOEM_NEOEMCue9Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue9Tetris2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue9Tetris2DBgBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue9Tetris2DGhostPiece         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DGhostPiece)) & NEOEM_NEOEMCue9Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue9Tetris2DColorMode          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue9Tetris2DFlashLines         ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DFlashLines)) & NEOEM_NEOEMCue9Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue9Tetris2DAutoPlay           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue9TRONSpeed                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9TRONHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue9TRONGridSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue9TRONGlow                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue9StarfieldWarpSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue9StarfieldWarpDensity       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue9StarfieldWarpColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue9StarfieldWarpWarpPulse     ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue9StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue9PlasmaNebulaSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue9PlasmaNebulaSaturation     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue9PlasmaNebulaContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue9PlasmaNebulaPaletteShift   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue9UFOSwarmSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue9UFOSwarmCount              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue9UFOSwarmHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue9UFOSwarmBeam               ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9UFOSwarmBeam)) & NEOEM_NEOEMCue9UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue9GameofLife2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9GameofLife2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue9GameofLife2DColorMode      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue9GameofLife2DSpawnDensity   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue9DNA2DSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9DNA2DHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue9DNA2DTwist                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue9DNA2DRungSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue9Aurora2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9Aurora2DHue                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue9Aurora2DScale              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue9Aurora2DIntensity          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue9Lissajous2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9Lissajous2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue9Lissajous2DFreqA           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue9Lissajous2DFreqB           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue9Metaballs2DSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue9Metaballs2DHue             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue9Metaballs2DBlobCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue9Metaballs2DContrast        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue9Metaballs2DContrast)))
// Effekt
#define ParamNEOEM_NEOEMCue10Effect                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Effect)))
// Parameter 0
#define ParamNEOEM_NEOEMCue10Param0                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param0)))
// Parameter 1
#define ParamNEOEM_NEOEMCue10Param1                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param1)))
// Parameter 2
#define ParamNEOEM_NEOEMCue10Param2                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param2)))
// Parameter 3
#define ParamNEOEM_NEOEMCue10Param3                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param3)))
// Parameter 4
#define ParamNEOEM_NEOEMCue10Param4                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param4)))
// Parameter 5
#define ParamNEOEM_NEOEMCue10Param5                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param5)))
// Parameter 6
#define ParamNEOEM_NEOEMCue10Param6                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param6)))
// Parameter 7
#define ParamNEOEM_NEOEMCue10Param7                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param7)))
// Parameter 8
#define ParamNEOEM_NEOEMCue10Param8                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param8)))
// Parameter 9
#define ParamNEOEM_NEOEMCue10Param9                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Param9)))
// Farbe RGB
#define ParamNEOEM_NEOEMCue10Color                     ((knx.paramInt(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Color)) & NEOEM_NEOEMCue10ColorMask) >> NEOEM_NEOEMCue10ColorShift)
// Weißkanal W
#define ParamNEOEM_NEOEMCue10W                         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10W)))
// Helligkeit
#define ParamNEOEM_NEOEMCue10Brightness                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Brightness)))
// Dauer in Sekunden
#define ParamNEOEM_NEOEMCue10Duration                  (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Duration)))
// Übergangszeit ms
#define ParamNEOEM_NEOEMCue10Fade                      (knx.paramWord(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Fade)))
// Beschreibung
#define ParamNEOEM_NEOEMCue10Text                      (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Text)))
#define ParamNEOEM_NEOEMCue10TextStr                   (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Text), NEOEM_NEOEMCue10TextLength))
// Effekt-Text
#define ParamNEOEM_NEOEMCue10EffectText                (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10EffectText)))
#define ParamNEOEM_NEOEMCue10EffectTextStr             (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10EffectText), NEOEM_NEOEMCue10EffectTextLength))
// Speed
#define ParamNEOEM_NEOEMCue10WipeSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10WipeSpeed)))
// Direction
#define ParamNEOEM_NEOEMCue10WipeDirection             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10WipeDirection)))
// Speed
#define ParamNEOEM_NEOEMCue10RainbowSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10RainbowSpeed)))
// Delta
#define ParamNEOEM_NEOEMCue10RainbowDelta              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10RainbowDelta)))
// Saturation
#define ParamNEOEM_NEOEMCue10RainbowSaturation         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10RainbowSaturation)))
// Density
#define ParamNEOEM_NEOEMCue10RainbowDensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10RainbowDensity)))
// Mode
#define ParamNEOEM_NEOEMCue10RainbowMode               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10RainbowMode)))
// Speed
#define ParamNEOEM_NEOEMCue10Pride2015Speed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Pride2015Speed)))
// Speed
#define ParamNEOEM_NEOEMCue10JuggleSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10JuggleSpeed)))
// NumDots
#define ParamNEOEM_NEOEMCue10JuggleNumDots             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10JuggleNumDots)))
// FadeSpeed
#define ParamNEOEM_NEOEMCue10JuggleFadeSpeed           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10JuggleFadeSpeed)))
// HueOffset
#define ParamNEOEM_NEOEMCue10JuggleHueOffset           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10JuggleHueOffset)))
// BPM
#define ParamNEOEM_NEOEMCue10BPMBPM                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BPMBPM)))
// Hue
#define ParamNEOEM_NEOEMCue10BPMHue                    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BPMHue)))
// Speed
#define ParamNEOEM_NEOEMCue10CylonSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10CylonHue                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonHue)))
// EyeSize
#define ParamNEOEM_NEOEMCue10CylonEyeSize              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonEyeSize)))
// FadeAmount
#define ParamNEOEM_NEOEMCue10CylonFadeAmount           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonFadeAmount)))
// Direction
#define ParamNEOEM_NEOEMCue10CylonDirection            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonDirection)))
// Mode
#define ParamNEOEM_NEOEMCue10CylonMode                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CylonMode)))
// PhaseDuration
#define ParamNEOEM_NEOEMCue10TestPhaseDuration         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TestPhaseDuration)))
// Mode
#define ParamNEOEM_NEOEMCue10TestMode                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TestMode)))
// Speed
#define ParamNEOEM_NEOEMCue10FireSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10FireSpeed)))
// Cooling
#define ParamNEOEM_NEOEMCue10FireCooling               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10FireCooling)))
// Sparking
#define ParamNEOEM_NEOEMCue10FireSparking              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10FireSparking)))
// ReverseDirection
#define ParamNEOEM_NEOEMCue10FireReverseDirection      ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10FireReverseDirection)) & NEOEM_NEOEMCue10FireReverseDirectionMask))
// BlueFireMode
#define ParamNEOEM_NEOEMCue10FireBlueFireMode          ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10FireBlueFireMode)) & NEOEM_NEOEMCue10FireBlueFireModeMask))
// Speed
#define ParamNEOEM_NEOEMCue10TheaterChaseSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseSpeed)))
// Spacing
#define ParamNEOEM_NEOEMCue10TheaterChaseSpacing       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseSpacing)))
// DotSize
#define ParamNEOEM_NEOEMCue10TheaterChaseDotSize       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseDotSize)))
// ColorMode
#define ParamNEOEM_NEOEMCue10TheaterChaseColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseColorMode)))
// ColorSpeed
#define ParamNEOEM_NEOEMCue10TheaterChaseColorSpeed    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseColorSpeed)))
// Bounce
#define ParamNEOEM_NEOEMCue10TheaterChaseBounce        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TheaterChaseBounce)))
// Speed
#define ParamNEOEM_NEOEMCue10SparkleSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue10SparkleFadeRate           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleFadeRate)))
// SparkleCount
#define ParamNEOEM_NEOEMCue10SparkleSparkleCount       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleSparkleCount)))
// Probability
#define ParamNEOEM_NEOEMCue10SparkleProbability        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleProbability)))
// WhiteOnly
#define ParamNEOEM_NEOEMCue10SparkleWhiteOnly          ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleWhiteOnly)) & NEOEM_NEOEMCue10SparkleWhiteOnlyMask))
// BurstMode
#define ParamNEOEM_NEOEMCue10SparkleBurstMode          ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleBurstMode)) & NEOEM_NEOEMCue10SparkleBurstModeMask))
// Mode
#define ParamNEOEM_NEOEMCue10SparkleMode               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10SparkleMode)))
// Speed
#define ParamNEOEM_NEOEMCue10BreathingSpeed            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BreathingSpeed)))
// MinBrightness
#define ParamNEOEM_NEOEMCue10BreathingMinBrightness    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BreathingMinBrightness)))
// Curve
#define ParamNEOEM_NEOEMCue10BreathingCurve            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BreathingCurve)))
// RainbowMode
#define ParamNEOEM_NEOEMCue10BreathingRainbowMode      ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BreathingRainbowMode)) & NEOEM_NEOEMCue10BreathingRainbowModeMask))
// Waveform
#define ParamNEOEM_NEOEMCue10BreathingWaveform         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BreathingWaveform)))
// Speed
#define ParamNEOEM_NEOEMCue10StrobeSpeed               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StrobeSpeed)))
// OnRatio
#define ParamNEOEM_NEOEMCue10StrobeOnRatio             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StrobeOnRatio)))
// MinBrightness
#define ParamNEOEM_NEOEMCue10StrobeMinBrightness       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StrobeMinBrightness)))
// RandomTiming
#define ParamNEOEM_NEOEMCue10StrobeRandomTiming        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StrobeRandomTiming)) & NEOEM_NEOEMCue10StrobeRandomTimingMask))
// RainbowStrobe
#define ParamNEOEM_NEOEMCue10StrobeRainbowStrobe       ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StrobeRainbowStrobe)) & NEOEM_NEOEMCue10StrobeRainbowStrobeMask))
// Speed
#define ParamNEOEM_NEOEMCue10CometSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometSpeed)))
// FadeRate
#define ParamNEOEM_NEOEMCue10CometFadeRate             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometFadeRate)))
// TailLength
#define ParamNEOEM_NEOEMCue10CometTailLength           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometTailLength)))
// BounceMode
#define ParamNEOEM_NEOEMCue10CometBounceMode           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometBounceMode)) & NEOEM_NEOEMCue10CometBounceModeMask))
// RainbowMode
#define ParamNEOEM_NEOEMCue10CometRainbowMode          ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometRainbowMode)) & NEOEM_NEOEMCue10CometRainbowModeMask))
// Mode
#define ParamNEOEM_NEOEMCue10CometMode                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10CometMode)))
// Speed
#define ParamNEOEM_NEOEMCue10NoiseSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10NoiseSpeed)))
// Scale
#define ParamNEOEM_NEOEMCue10NoiseScale                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10NoiseScale)))
// Saturation
#define ParamNEOEM_NEOEMCue10NoiseSaturation           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10NoiseSaturation)))
// HueOffset
#define ParamNEOEM_NEOEMCue10NoiseHueOffset            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10NoiseHueOffset)))
// Palette
#define ParamNEOEM_NEOEMCue10NoisePalette              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10NoisePalette)))
// Speed
#define ParamNEOEM_NEOEMCue10PaletteSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PaletteSpeed)))
// Palette
#define ParamNEOEM_NEOEMCue10PalettePalette            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PalettePalette)))
// Blend
#define ParamNEOEM_NEOEMCue10PaletteBlend              ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PaletteBlend)) & NEOEM_NEOEMCue10PaletteBlendMask))
// Spacing
#define ParamNEOEM_NEOEMCue10PaletteSpacing            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PaletteSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue10BlitzSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BlitzSpeed)))
// Width
#define ParamNEOEM_NEOEMCue10BlitzWidth                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BlitzWidth)))
// Decay
#define ParamNEOEM_NEOEMCue10BlitzDecay                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BlitzDecay)))
// Hue
#define ParamNEOEM_NEOEMCue10BlitzHue                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BlitzHue)))
// Intensity
#define ParamNEOEM_NEOEMCue10BlitzIntensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10BlitzIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue10GradientSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GradientSpeed)))
// StartHue
#define ParamNEOEM_NEOEMCue10GradientStartHue          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GradientStartHue)))
// EndHue
#define ParamNEOEM_NEOEMCue10GradientEndHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GradientEndHue)))
// Saturation
#define ParamNEOEM_NEOEMCue10GradientSaturation        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GradientSaturation)))
// Speed
#define ParamNEOEM_NEOEMCue10KerzeSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10KerzeSpeed)))
// Intensity
#define ParamNEOEM_NEOEMCue10KerzeIntensity            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10KerzeIntensity)))
// Zones
#define ParamNEOEM_NEOEMCue10KerzeZones                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10KerzeZones)))
// Speed
#define ParamNEOEM_NEOEMCue10LaufschriftSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftSpeed)))
// Gap
#define ParamNEOEM_NEOEMCue10LaufschriftGap            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftGap)))
// Loop
#define ParamNEOEM_NEOEMCue10LaufschriftLoop           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftLoop)) & NEOEM_NEOEMCue10LaufschriftLoopMask))
// Laufschrift-Text
#define ParamNEOEM_NEOEMCue10LaufschriftText           (knx.paramData(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftText)))
#define ParamNEOEM_NEOEMCue10LaufschriftTextStr        (knx.paramString(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftText), NEOEM_NEOEMCue10LaufschriftTextLength))
// Font
#define ParamNEOEM_NEOEMCue10LaufschriftFont           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10LaufschriftFont)))
// ViewMode
#define ParamNEOEM_NEOEMCue10Uhr2DViewMode             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DViewMode)))
// ShowSeconds
#define ParamNEOEM_NEOEMCue10Uhr2DShowSeconds          ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DShowSeconds)) & NEOEM_NEOEMCue10Uhr2DShowSecondsMask))
// BlinkColon
#define ParamNEOEM_NEOEMCue10Uhr2DBlinkColon           ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DBlinkColon)) & NEOEM_NEOEMCue10Uhr2DBlinkColonMask))
// ColourHue
#define ParamNEOEM_NEOEMCue10Uhr2DColourHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DColourHue)))
// DateMode
#define ParamNEOEM_NEOEMCue10Uhr2DDateMode             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DDateMode)))
// DateFormat
#define ParamNEOEM_NEOEMCue10Uhr2DDateFormat           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DDateFormat)))
// DateHue
#define ParamNEOEM_NEOEMCue10Uhr2DDateHue              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DDateHue)))
// SwitchSec
#define ParamNEOEM_NEOEMCue10Uhr2DSwitchSec            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DSwitchSec)))
// ScrollSpeed
#define ParamNEOEM_NEOEMCue10Uhr2DScrollSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DScrollSpeed)))
// Font
#define ParamNEOEM_NEOEMCue10Uhr2DFont                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Uhr2DFont)))
// Speed
#define ParamNEOEM_NEOEMCue10Snake2DSpeed              (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Snake2DSpeed)))
// HeadHue
#define ParamNEOEM_NEOEMCue10Snake2DHeadHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Snake2DHeadHue)))
// BodyMode
#define ParamNEOEM_NEOEMCue10Snake2DBodyMode           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Snake2DBodyMode)))
// BodyHue
#define ParamNEOEM_NEOEMCue10Snake2DBodyHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Snake2DBodyHue)))
// Speed
#define ParamNEOEM_NEOEMCue10Matrix2DSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Matrix2DSpeed)))
// Density
#define ParamNEOEM_NEOEMCue10Matrix2DDensity           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Matrix2DDensity)))
// Palette
#define ParamNEOEM_NEOEMCue10Matrix2DPalette           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Matrix2DPalette)))
// Glitch
#define ParamNEOEM_NEOEMCue10Matrix2DGlitch            ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Matrix2DGlitch)) & NEOEM_NEOEMCue10Matrix2DGlitchMask))
// Speed
#define ParamNEOEM_NEOEMCue10Tetris2DSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DSpeed)))
// BgBrightness
#define ParamNEOEM_NEOEMCue10Tetris2DBgBrightness      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DBgBrightness)))
// GhostPiece
#define ParamNEOEM_NEOEMCue10Tetris2DGhostPiece        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DGhostPiece)) & NEOEM_NEOEMCue10Tetris2DGhostPieceMask))
// ColorMode
#define ParamNEOEM_NEOEMCue10Tetris2DColorMode         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DColorMode)))
// FlashLines
#define ParamNEOEM_NEOEMCue10Tetris2DFlashLines        ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DFlashLines)) & NEOEM_NEOEMCue10Tetris2DFlashLinesMask))
// AutoPlay
#define ParamNEOEM_NEOEMCue10Tetris2DAutoPlay          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Tetris2DAutoPlay)))
// Speed
#define ParamNEOEM_NEOEMCue10TRONSpeed                 (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TRONSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10TRONHue                   (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TRONHue)))
// GridSpacing
#define ParamNEOEM_NEOEMCue10TRONGridSpacing           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TRONGridSpacing)))
// Glow
#define ParamNEOEM_NEOEMCue10TRONGlow                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10TRONGlow)))
// Speed
#define ParamNEOEM_NEOEMCue10StarfieldWarpSpeed        (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StarfieldWarpSpeed)))
// Density
#define ParamNEOEM_NEOEMCue10StarfieldWarpDensity      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StarfieldWarpDensity)))
// ColorMode
#define ParamNEOEM_NEOEMCue10StarfieldWarpColorMode    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StarfieldWarpColorMode)))
// WarpPulse
#define ParamNEOEM_NEOEMCue10StarfieldWarpWarpPulse    ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10StarfieldWarpWarpPulse)) & NEOEM_NEOEMCue10StarfieldWarpWarpPulseMask))
// Speed
#define ParamNEOEM_NEOEMCue10PlasmaNebulaSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PlasmaNebulaSpeed)))
// Saturation
#define ParamNEOEM_NEOEMCue10PlasmaNebulaSaturation    (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PlasmaNebulaSaturation)))
// Contrast
#define ParamNEOEM_NEOEMCue10PlasmaNebulaContrast      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PlasmaNebulaContrast)))
// PaletteShift
#define ParamNEOEM_NEOEMCue10PlasmaNebulaPaletteShift  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10PlasmaNebulaPaletteShift)))
// Speed
#define ParamNEOEM_NEOEMCue10UFOSwarmSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10UFOSwarmSpeed)))
// Count
#define ParamNEOEM_NEOEMCue10UFOSwarmCount             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10UFOSwarmCount)))
// Hue
#define ParamNEOEM_NEOEMCue10UFOSwarmHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10UFOSwarmHue)))
// Beam
#define ParamNEOEM_NEOEMCue10UFOSwarmBeam              ((bool)(knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10UFOSwarmBeam)) & NEOEM_NEOEMCue10UFOSwarmBeamMask))
// Speed
#define ParamNEOEM_NEOEMCue10GameofLife2DSpeed         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GameofLife2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10GameofLife2DHue           (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GameofLife2DHue)))
// ColorMode
#define ParamNEOEM_NEOEMCue10GameofLife2DColorMode     (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GameofLife2DColorMode)))
// SpawnDensity
#define ParamNEOEM_NEOEMCue10GameofLife2DSpawnDensity  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10GameofLife2DSpawnDensity)))
// Speed
#define ParamNEOEM_NEOEMCue10DNA2DSpeed                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10DNA2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10DNA2DHue                  (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10DNA2DHue)))
// Twist
#define ParamNEOEM_NEOEMCue10DNA2DTwist                (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10DNA2DTwist)))
// RungSpacing
#define ParamNEOEM_NEOEMCue10DNA2DRungSpacing          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10DNA2DRungSpacing)))
// Speed
#define ParamNEOEM_NEOEMCue10Aurora2DSpeed             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Aurora2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10Aurora2DHue               (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Aurora2DHue)))
// Scale
#define ParamNEOEM_NEOEMCue10Aurora2DScale             (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Aurora2DScale)))
// Intensity
#define ParamNEOEM_NEOEMCue10Aurora2DIntensity         (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Aurora2DIntensity)))
// Speed
#define ParamNEOEM_NEOEMCue10Lissajous2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Lissajous2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10Lissajous2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Lissajous2DHue)))
// FreqA
#define ParamNEOEM_NEOEMCue10Lissajous2DFreqA          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Lissajous2DFreqA)))
// FreqB
#define ParamNEOEM_NEOEMCue10Lissajous2DFreqB          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Lissajous2DFreqB)))
// Speed
#define ParamNEOEM_NEOEMCue10Metaballs2DSpeed          (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Metaballs2DSpeed)))
// Hue
#define ParamNEOEM_NEOEMCue10Metaballs2DHue            (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Metaballs2DHue)))
// BlobCount
#define ParamNEOEM_NEOEMCue10Metaballs2DBlobCount      (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Metaballs2DBlobCount)))
// Contrast
#define ParamNEOEM_NEOEMCue10Metaballs2DContrast       (knx.paramByte(NEOEM_ParamCalcIndex(NEOEM_NEOEMCue10Metaballs2DContrast)))


// enumeration types
enum class PT_Logic
{
    AUS = 0,
    UND = 1,
    ODER = 2,
    EXOR = 3,
    TOR = 4,
    SCHALTER = 6,
    ZEITSCHALTUHR = 5
};

enum class PT_Calculate
{
    Invalid = 0,
    Valid = 1
};

enum class PT_GateTrigger
{
    None = 0,
    Off = 1,
    On = 2,
    Input = 3
};

enum class PT_LockTrigger
{
    None = 0,
    Off = 1,
    On = 2,
    Value = 3
};

enum class PT_LockResetQueue
{
    None = 0,
    ResetAfterLock = 1,
    ResetAfterUnlock = 2
};

enum class PT_InputEnable
{
    Inactive = 0,
    ActiveNormal = 1,
    ActiveInverted = 2
};

enum class PT_InputConv
{
    Wertintervall = 0,
    Differenzintervall = 1,
    Hysterese = 2,
    Differenzhysterese = 3,
    Einzelwerte = 4,
    Konstante = 5,
    Eingangswert = 6,
    Trigger = 7
};

enum class PT_LogicDpt
{
    DPT_1 = 0,
    DPT_2 = 1,
    DPT_3 = 17,
    DPT_5 = 2,
    DPT_5001 = 3,
    DPT_6 = 4,
    DPT_7 = 5,
    DPT_8 = 6,
    DPT_9 = 7,
    DPT_12 = 13,
    DPT_13 = 14,
    DPT_14 = 15,
    DPT_16 = 8,
    DPT_17 = 9,
    DPT_232 = 10
};

enum class PT_InputDefault
{
    None = 0,
    Bus = 1,
    Off = 2,
    On = 3
};

enum class PT_OnOffRepeat
{
    Verzoegerung_bleibt_bestehen = 0,
    Verzoegerung_wird_verlaengert = 1,
    Sofort_schalten_ohne_Verzoegerung = 2
};

enum class PT_OnOffReset
{
    Verzoegerung_bleibt_bestehen = 0,
    Verzoegerung_beenden_ohne_zu_schalten = 1
};

enum class PT_OutputFilter
{
    Alle_Wiederholungen_durchlassen = 0,
    Nur_EIN_Wiederholungen_durchlassen = 1,
    Nur_AUS_Wiederholungen_durchlassen = 2,
    Keine_Wiederholungen_durchlassen = 3
};

enum class PT_SendOnChange
{
    Alle_Werte_senden = 0,
    Nur_geaenderte_Werte_senden = 1
};

enum class PT_OutputSend
{
    None = 0,
    Constant = 1,
    ValueInput1 = 2,
    ValueInput2 = 3,
    OtherKo = 9,
    Function = 8,
    ReadRequest = 4,
    RestartDevice = 5,
    StatusLed = 7
};

enum class PT_YearDay
{
    Tagesschaltuhr = 0,
    Jahresschaltuhr = 1,
    Tagesschaltuhr_verbunden = 2,
    Jahresschaltuhr_verbunden = 3
};

enum class PT_Holiday
{
    Feiertage_nicht_beachten = 0,
    An_Feiertagen_nicht_schalten = 1,
    Nur_an_Feiertagen_schalten = 2,
    Feiertage_wie_Sonntage_behandeln = 3
};

enum class PT_Vacation
{
    Urlaub_nicht_beachten = 0,
    Bei_Urlaub_nicht_schalten = 1,
    Nur_bei_Urlaub_schalten = 2,
    Urlaub_wie_Sonntag_behandeln = 3
};

enum class PT_DuskDawn
{
    Inactive = 0,
    PointInTime = 1,
    Sunrise_Plus = 4,
    Sunrise_Minus = 5,
    Sunrise_Earliest = 6,
    Sunrise_Latest = 7,
    Sunrise_DegreeUp = 12,
    Sunrise_DegreeDown = 14,
    Sunset_Plus = 8,
    Sunset_Minus = 9,
    Sunset_Earliest = 10,
    Sunset_Latest = 11,
    Sunset_DegreeUp = 13,
    Sunset_DegreeDown = 15
};

enum class PT_KORelInput
{
    None = 0,
    Absolute = 1,
    Relative = 2,
    Bitmask = 3
};

enum class PT_StatusLedEffect
{
    Aus = 0,
    Ein = 1,
    Blinken = 2,
    Pulsieren = 3,
    Aufblitzen = 4
};

enum class PT_InternalInputType
{
    Anderen_Logikkanal = 0,
    Statuskanal = 1
};

enum class PT_NEOEffectType
{
    Solid = 0,
    Wipe = 1,
    Rainbow = 2,
    Pride = 3,
    Juggle = 4,
    BPM = 5,
    Cylon = 6,
    RGBWTest = 7,
    Fire = 8,
    TheaterChase = 9,
    Sparkle = 10,
    Breathing = 11,
    Strobe = 12,
    Comet = 13,
    Noise = 14,
    Palette = 15,
    Lightning = 16,
    Gradient = 17,
    Candle = 18,
    ScrollText = 19,
    Tron2D = 24,
    StarfieldWarp2D = 25,
    PlasmaNebula2D = 26,
    UfoSwarm2D = 27,
    Clock2D = 20,
    Snake2D = 21,
    Matrix2D = 22,
    Tetris2D = 23,
    GameOfLife2D = 28,
    DNA2D = 29,
    Aurora2D = 30,
    Lissajous2D = 31,
    Metaballs2D = 32
};

enum class PT_NEOLedType
{
    WS2812B = 0,
    WS2805_RGBCCT = 1,
    WS2811 = 2,
    WS2813 = 3,
    SK6812 = 4,
    APA102 = 5,
    SK9822 = 6,
    WS281x = 7,
    SK6812_WS2814_RGBW = 8,
    TM1814 = 9,
    WS2812_400kHz = 10,
    TM1829 = 11,
    UCS8903 = 12,
    APA106_PL9823 = 13,
    TM1914 = 14,
    FW1906 = 15,
    UCS8904 = 16,
    WS2805_RGBCW = 17,
    SM16825 = 18,
    WS2811_WHITE = 19,
    WS281x_WWA = 20,
    WS2801 = 21,
    LPD8806 = 22,
    LPD6803 = 23,
    P9813 = 24,
    APA102_Clone = 25,
    SK6812_RGBCCT_5ch = 30,
    WS2814_RGBCCT_5ch = 31,
    CUSTOM = 99
};

enum class PT_NEOColourOrder
{
    GRB = 0,
    RGB = 1,
    BRG = 2,
    RBG = 3,
    BGR = 4,
    GBR = 5,
    RGBW = 6,
    GRBW = 7,
    RGBCCT = 8,
    GRBCCT = 9,
    RGBCTW = 10,
    GRBCTW = 11
};

enum class PT_NEOVoltage
{
    V5 = 0,
    V12 = 1,
    V24 = 2
};

enum class PT_NEOSwap
{
    None = 0,
    WandB = 1,
    WandG = 2,
    WandR = 3,
    WWandCW = 4
};

enum class PT_NEOTiming
{
    AUTO = 0,
    AUTO_LEGACY = 1,
    SLOW_20PCT = 2,
    SLOW_15PCT = 3,
    SLOW_10PCT = 4,
    SLOW_5PCT = 5,
    FAST_5PCT = 6,
    FAST_10PCT = 7,
    FAST_15PCT = 8,
    FAST_20PCT = 9,
    FAST_25PCT = 10
};

enum class PT_NEOPowerLimitType
{
    Global = 0,
    PerChannel = 1,
    PerLED = 2
};

enum class PT_NEOPowerLimitCombined
{
    Disabled = 0,
    FixedValue = 1,
    PerLED = 2
};

enum class PT_NEOPowerLimitCombinedStrip
{
    Disabled = 0,
    UseGlobal = 1,
    CustomFixed = 2,
    CustomPerLED = 3
};

enum class PT_NEOPowerLimit
{
    GlobalLimit = 0,
    PerLED = 1,
    PerChannel = 2
};

enum class PT_NEOSPICLK
{
    MHz1 = 0,
    MHz2 = 1,
    MHz4 = 2,
    MHz8 = 3,
    MHz12 = 4,
    MHz16 = 5,
    MHz20 = 6
};

enum class PT_NEOGammaValue
{
    Gamma_1_2 = 0,
    Gamma_1_3 = 1,
    Gamma_1_4 = 2,
    Gamma_1_5 = 3,
    Gamma_1_6 = 4,
    Gamma_1_7 = 5,
    Gamma_1_8 = 6,
    Gamma_1_9 = 7,
    Gamma_2_0 = 8,
    Gamma_2_1 = 9,
    Gamma_2_2 = 10,
    Gamma_2_3 = 11,
    Gamma_2_4 = 12,
    Gamma_2_5 = 13,
    Gamma_2_6 = 14,
    Gamma_2_7 = 15
};



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
