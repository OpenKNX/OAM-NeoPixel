# Effektmanager – Zustand

Legt fest, wie dieser Effektmanager-Kanal behandelt wird. Drei Zustände stehen zur Auswahl:

**Aktiv**

Der Kanal ist konfiguriert und voll funktionsfähig. Er führt seine Funktion (Effekt-Wiedergabe) aus; Kommunikationsobjekte und Gruppenadressen sind aktiv.

**Deaktiviert**

Der Kanal ist abgeschaltet. Typ, Parameter und Kommunikationsobjekte werden entfernt; verknüpfte Gruppenadressen gehen verloren (die ETS warnt beim Entfernen).

**Suspendiert**

Der Kanal bleibt vollständig konfiguriert: Typ, Parameter, Kommunikationsobjekte und Gruppenadressen-Verknüpfungen bleiben erhalten und sichtbar. Der Kanal führt seine Funktion aber nicht aus (er rendert nichts). So lässt sich ein Kanal vorübergehend stilllegen, ohne Konfiguration oder Verknüpfungen zu verlieren – z. B. bei Inbetriebnahme, Umbauten oder saisonal.

Hinweis: Im Gerätebaum wird Deaktiviert mit ⛔ und Suspendiert mit ⏸ vor dem Kanalnamen markiert; Aktiv erhält keinen Marker.
