# HCL-Kurve (Human Centric Lighting)

Steuert die automatische Farbtemperatur-Anpassung für dieses Segment basierend auf Tageszeit oder Sonnenstand.

**Funktion:**
HCL (Human Centric Lighting) passt die Farbtemperatur automatisch an, um den natürlichen Tagesverlauf zu simulieren:

- **Morgens/Abends:** Warmes Licht (niedrige Kelvin-Werte, z.B. 2700K)
- **Mittags:** Kaltweißes Licht (hohe Kelvin-Werte, z.B. 6500K)

**Modi:**

- **Deaktiviert:** Keine HCL-Kurve für dieses Segment
- **Global verwenden:** Nutzt die globalen HCL-Einstellungen aus der Hauptkonfiguration
- **Benutzerdefiniert:** Segment-spezifische HCL-Konfiguration mit eigenen Parametern

**Benutzerdefinierte Einstellungen:**
Wenn "Benutzerdefiniert" gewählt:

- **Kurventyp:** Feste Zeiten oder Sonnenstand-basiert
- **Farbtemperatur:** Min/Max Kelvin-Bereich (2000-6500K)
- **Stärke:** HCL-Effekt-Intensität (0-100%)
- **Anwendungsmodus:** Alle Farben, nur Weiß, oder nur niedrig-gesättigte Farben

**Empfehlung:**

- **Wohnräume:** Benutzerdefiniert mit Sonnenstand für natürlichen Tagesverlauf
- **Büros:** Global mit festen Zeiten (6:00-22:00)
- **Effekt-Segmente:** Deaktiviert für volle Farbkontrolle
