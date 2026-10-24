### Schaltverzögerung

Verzögert das Schalten des Relais um die angegebene Zeit in Sekunden (0 = sofort).

**Einschaltverzögerung:** Wartezeit, bevor das Relais nach einem EIN-Telegramm anzieht. Läuft noch die Mindest-Auszeit, wartet das Relais, bis auch diese abgelaufen ist.

**Ausschaltverzögerung:** Wartezeit, bevor das Relais nach einem AUS-Telegramm abfällt.

Die Verzögerungen beziehen sich nur auf das Relais-Objekt. Das Relais ist nicht an den Ein/Aus-Zustand der LEDs gekoppelt; soll ein Netzteil zusammen mit den LEDs schalten, muss das Relais-Objekt dafür ein eigenes Telegramm erhalten (z. B. dieselbe Gruppenadresse wie das Schaltobjekt). Ein neues Telegramm während einer laufenden Verzögerung ersetzt das vorherige.
