# Projektplan: Styreenhed til håndklædetørrer

## Formål
At udvikle en styreenhed til en 230V håndklædetørrer ved hjælp af en ESP8266 Wemos D1 mini.

## Hardware komponenter
- ESP8266 Wemos D1 mini
- Relæ til 230V styring
- 3 knapper
- Dallas temperatursensor
- OLED display (SSD1306)

## Pin konfiguration
- Dallas sensor: Pin 4
- Knap 1 (Temperatur op): Pin 6
- Knap 2 (Soft on/off): Pin 7
- Knap 3 (Temperatur ned): Pin 8

## Funktionalitet
1. Temperaturkontrol:
   - Knap 1: Øg set-temperatur
   - Knap 3: Sænk set-temperatur
   - Relæ aktiveres 2°C under set-temperatur
   - Relæ deaktiveres 1°C over set-temperatur

2. Soft on/off:
   - Knap 2: Tænd/sluk funktion
   - Tilsidesætter temperaturstyring når slukket

3. Display:
   - Viser tænd/sluk status
   - Viser set-temperatur
   - Viser aktuel temperatur

4. Knap-håndtering:
   - Implementer debounce for alle knapper

## Software komponenter
1. Initialisering af hardware (pins, sensorer, display)
2. Knap-læsning med debounce
3. Temperaturmåling og -styring
4. Relæ-kontrol
5. Display-opdatering
6. Hovedloop med tilstandshåndtering

## Udviklingstrin
1. Opsætning af hardware og grundlæggende pin-konfiguration
2. Implementering af knap-læsning med debounce
3. Integration af Dallas temperatursensor
4. Implementering af temperaturstyringslogik
5. Tilføjelse af relæ-kontrol
6. Opsætning og integration af OLED-display
7. Implementering af soft on/off funktionalitet
8. Test og fejlfinding
9. Optimering og finpudsning

## Sikkerhedsovervejelser
- Sikker håndtering af 230V gennem relæet
- Beskyttelse mod overophedning
- Fejlhåndtering for sensorfejl

## Fremtidige udvidelsesmuligheder
- WiFi-integration for fjernbetjening
- Tidsbaseret styring
- Energiforbrugsovervågning
