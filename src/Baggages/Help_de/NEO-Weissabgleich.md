### Weißabgleich

Korrigiert Farbabweichungen der LEDs um ein neutrales Weiß zu erreichen. Verschiedene LED-Chargen können leicht unterschiedliche Farbtemperaturen haben.

**Problem ohne Weißabgleich:**
- Weiß (255,255,255) erscheint bläulich, gelblich oder rötlich
- Inkonsistente Farben zwischen verschiedenen LED-Streifen
- Sichtbare Farbunterschiede bei längeren Installationen

**Funktionsweise:**
- Individuelle Multiplikatoren für Rot, Grün und Blau
- Automatische Skalierung auf den schwächsten Kanal
- Erhalt der maximalen Helligkeit möglich

**Kalibrierung:**
1. LEDs auf Weiß (255,255,255) setzen
2. Farbstich visuell bewerten
3. Entsprechende Kanäle reduzieren:
   - **Bläulich**: Blau-Anteil reduzieren
   - **Gelblich**: Rot und Grün reduzieren  
   - **Rötlich**: Rot-Anteil reduzieren

**WLED-Kompatibilität:**
Verwendet WLED-Standard Weißabgleich-Algorithmen für professionelle Ergebnisse.

