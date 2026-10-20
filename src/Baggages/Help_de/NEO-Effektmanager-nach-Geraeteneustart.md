# Effektmanager nach Geräteneustart

Startet den ausgewählten Effektmanager nach dem OpenKNX-Startup-Delay immer frisch ab Cue 1.

Der Effektmanager hat Vorrang vor einem aus dem Flash wiederhergestellten Segmentzustand. Ein Effektmanager startet auch auf einem Segment, das nach dem Neustart aus ist („Aus“ oder ausgeschaltet gespeichert), und schaltet es dabei ein; sein Ende oder Stop schaltet es wieder aus. Ein Effektmanager mit Loop bleibt an. Mit **Keiner** bleibt das bisherige Wiederherstellungsverhalten unverändert.
