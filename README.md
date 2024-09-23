# Temperaturkontrolsystem med ESP8266

Dette projekt implementerer et temperaturkontrolsystem ved hjælp af en ESP8266-mikrocontroller. Systemet kan måle og vise den aktuelle temperatur, lade brugeren indstille en ønsket temperatur og styre en varmekilde via et relæ.

## Hardwarekomponenter

- ESP8266 (D1 Mini Lite)
- Dallas DS18B20 temperatursensor
- OLED-display (SSD1306, 128x64 pixels)
- 3 trykknapper (Op, Ned, Tænd/Sluk)
- Relæ til at styre varmekilden
- Diverse modstande og ledninger

## Softwarebiblioteker

Projektet bruger følgende biblioteker:
- OneWire: Til kommunikation med DS18B20-sensoren
- DallasTemperature: Til at læse temperaturen fra DS18B20-sensoren
- U8g2: Til at styre OLED-displayet
- Bounce2: Til at håndtere knappernes debouncing

## Funktionalitet

1. **Temperaturmåling**: Systemet måler kontinuerligt temperaturen ved hjælp af DS18B20-sensoren.

2. **Temperaturvisning**: Den aktuelle temperatur og den indstillede måltemperatur vises på OLED-displayet.

3. **Temperaturkontrol**: 
   - Brugeren kan justere måltemperaturen ved hjælp af "Op" og "Ned" knapperne.
   - Systemet tænder og slukker for varmekilden via relæet for at opretholde den ønskede temperatur.

4. **Tænd/Sluk-funktion**: En dedikeret knap bruges til at tænde og slukke for hele systemet (softwarebaseret).

5. **Statusvisning**: Displayet viser systemets aktuelle status (ON, Standby, eller OFF).

6. **Hukommelse**: Systemet gemmer den indstillede temperatur og on/off-status i EEPROM, så indstillingerne bevares selv efter en strømafbrydelse.

## Pinout

- DALLAS_PIN: 4 (DS18B20 temperatursensor)
- BUTTON_UP_PIN: 6
- BUTTON_ONOFF_PIN: 7
- BUTTON_DOWN_PIN: 8
- RELAY_PIN: 5 (D1 på ESP8266)

## Kontrollogik

Systemet bruger en simpel hysterese-baseret kontrollogik:
- Hvis den aktuelle temperatur er mere end 2°C under måltemperaturen, tændes varmekilden.
- Hvis den aktuelle temperatur er mere end 1°C over måltemperaturen, slukkes varmekilden.
- Mellem disse to grænser forbliver varmekildestatus uændret for at undgå hyppig skift.

## Energibesparelse

For at spare energi og forlænge levetiden af OLED-displayet, opdateres displayet kun når der er ændringer i temperatur, indstillinger eller systemstatus.

## Opsætning og brug

1. Tilslut hardwarekomponenterne som beskrevet i pinout-sektionen.
2. Upload koden til ESP8266 ved hjælp af PlatformIO eller Arduino IDE.
3. Når systemet starter op, vil det vise "Initializing..." på displayet.
4. Brug "Op" og "Ned" knapperne til at justere måltemperaturen.
5. Brug "Tænd/Sluk" knappen til at aktivere eller deaktivere temperaturkontrollen.

## Fremtidige forbedringer

- Implementere WiFi-forbindelse for fjernbetjening og -overvågning.
- Tilføje en PID-controller for mere præcis temperaturkontrol.
- Implementere datalogning og grafer for temperaturhistorik.

## Bidrag

Føl dig fri til at forke dette projekt og bidrage med forbedringer. Pull requests er velkomne!

## Licens

Dette projekt er udgivet under MIT-licensen. Se LICENSE-filen for detaljer.
