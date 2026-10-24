# espVT — ESP32-S3 ISOBUS Virtual Terminal

> **Status:** experimental, unofficial and not ISO 11783 certified. Do not use
> it as the sole control or safety device for agricultural machinery.

Projekt terminala dostępowego ISOBUS/UT na płytkach Waveshare
ESP32-S3-Touch-LCD-7 oraz ESP32-S3-Touch-LCD-7B z AgIsoStack++.

Repozytorium zawiera również firmware osobnego panelu przycisków AUX-N dla
ESP32U w katalogu `auxn_buttons_esp32`.

![Terminal espVT w obudowie](docs/images/espvt-hardware.jpg)

## Cel

Docelowo ta aplikacja ma dzialac jako Virtual Terminal Server ISO 11783-6:

- odbior danych ISOBUS po CAN/TWAI,
- obsluga address claim przez AgIsoStack++,
- przyjmowanie i buforowanie object pooli IOP od ECU,
- AUX-N Type 2: wykrywanie urzadzen wejscia i wysylanie przypisan funkcji,
- renderowanie aktualnego working setu na LCD zależnym od wariantu płytki,
- wysylanie zdarzen dotyku/przyciskow jako komunikaty VT do ECU,
- zapis wersji object pooli w pamieci flash/NVS.

## Sprzet

Obsługiwane warianty płytki:

- **ESP32-S3-Touch-LCD-7B (domyślny):** LCD RGB 1024×600, GT911, ekspander
  CH32V003 pod I²C `0x24`. Zachowuje dotychczasowe piny, timingi i ustawienia.
- **ESP32-S3-Touch-LCD-7:** LCD RGB 800×480, GT911 i ekspander CH422G.
  Wybór wariantu znajduje się w `idf.py menuconfig` → `Wariant płytki ESP32-S3`.

Wspólne połączenia CAN/TWAI to TX GPIO20 i RX GPIO19, a I²C to SDA GPIO8 i
SCL GPIO9. Na LCD-7 sygnały panelu RGB w kolejności GPIO `B0..B4, G0..G5,
R0..R4` to `14, 38, 18, 17, 10, 39, 0, 45, 48, 47, 21, 1, 2, 42, 41, 40`;
pozostałe sygnały to DE GPIO5, VSYNC GPIO3, HSYNC GPIO46 i PCLK GPIO7.
GT911 ma IRQ GPIO4, domyślny adres I²C `0x5D` (adres alternatywny `0x14`) i
reset z EXIO1 ekspandera. Na LCD-7 CH422G steruje także podświetleniem (EXIO2),
resetem LCD (EXIO3), wyborem USB/CAN (EXIO5; stan wysoki wybiera CAN) i
zasilaniem LCD (EXIO6). CH422G używa oddzielnych adresów poleceń I²C, m.in.
WR-SET `0x24` i WR-IO `0x38`; nie jest zgodny z rejestrowym protokołem CH32V003.

Waveshare podaje dla LCD-7 panel ST7262 800×480 i timingi RGB: PCLK 16 MHz,
HSYNC 4/8/8, VSYNC 4/8/8 (szerokość impulsu/tylny/przedni porch) oraz aktywny
zboczem opadającym PCLK. Na płytce jest również RS-485: RX GPIO15 i TX GPIO16.
Nie jest to RS-232; odbiornik RS-232 wymaga zewnętrznego transceivera MAX3232
i pinów UART wybranych w konfiguracji firmware. Nie łącz napięć RS-232
bezpośrednio z ESP32.

## Obudowa do druku 3D

Aktualny model obudowy znajduje się w pliku
`ESP32-S3-Touch-LCD-7B_V42_VESA_RAM_M5_SY8205_RS232_43MM_PETG.stl`.
Model jest przygotowany dla wariantu z mocowaniem VESA/RAM, śrubami M5,
zasilaniem SY8205 i złączem RS-232; przewidziany materiał to PETG.

## Struktura

- `main/board_pins.hpp` - stałe sprzętowe zależne od wariantu LCD-7/7B.
- `main/app_main.cpp` - punkt startowy aplikacji.
- `main/board_support.*` - inicjalizacja I2C, ekspandera IO i trybu CAN.
- `main/can_monitor.*` - uruchomienie TWAI przez AgIsoStack++.
- `main/lcd_panel.*` - sterownik RGB565 panelu z dwoma buforami w PSRAM.
- `main/touch_gt911.*` - inicjalizacja i diagnostyka dotyku GT911.
- `main/isobus_terminal_server.*` - serwer VT AgIsoStack++, w tym obiekty AUX-N.
- `main/vt_scene_renderer.*` - renderer podstawowych obiektow VT i mapa obszarow dotyku.
- `main/lvgl_terminal_view.*` - ekran diagnostyczny uruchamiany przed zaladowaniem working setu.
- `main/logo_image.*` - wyswietlanie zmniejszonego logo VTISOBUS/FarmNavi na ekranie startowym i oczekiwania.
- `main/object_pool_store.*` - zapis/odczyt object pooli w flash.
- `tools/bmp_to_rgb565.py` - automatyczne kadrowanie i konwersja `logo.png` do RGB565 podczas buildu.
- `main/gnss_um982.*` - opcjonalny odbior NMEA z UM982 albo portu RS-232 oraz NTRIP/RTCM dla wariantu RTK.

## AgIsoStack++

Zmodyfikowana wersja AgIsoStack++ jest dołączona w
`third_party/AgIsoStack-plus-plus`, aby kompilacja była powtarzalna. Informacja
o bazowym commicie upstream i zakresie lokalnych zmian znajduje się w pliku
`third_party/AgIsoStack-plus-plus/ESPVT_UPSTREAM.md`. Zachowana jest również
oryginalna licencja MIT biblioteki.

Adapter jest domyslnie wlaczony. Przed podlaczeniem do prawdziwej magistrali
ustaw przydzielony kod producenta oraz unikalny numer terminala:

```sh
idf.py menuconfig
```

i ustaw `ISOVT_MANUFACTURER_CODE` i `ISOVT_IDENTITY_NUMBER`.

## Budowanie

Wymagane jest ESP-IDF z obsluga ESP32-S3.

Projekt ma domyslnie `CONFIG_APP_NO_BLOBS=y`, aby nie linkowac binarnych
bibliotek Wi-Fi/Bluetooth. Terminal korzysta z CAN/TWAI, I2C i SPIFFS, wiec ta
konfiguracja ogranicza niepotrzebne zaleznosci platformy.

```sh
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

Powyższe polecenia używają domyślnego wariantu 7B. Dla ESP32-S3-Touch-LCD-7
użyj osobnego katalogu budowania i dołącz ustawienia wariantu (8 MB flash,
PSRAM 80 MHz oraz tablicę partycji mieszczącą się w pamięci 8 MB):

```sh
idf.py -B build-lcd7 \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.lcd7.defaults" build
```

Można również wybrać płytkę w `idf.py menuconfig`; dla LCD-7 rozmiar obszaru VT
jest stały `480×480` i rysowany 1:1. Opcja `600×600` pozostaje dostępna tylko
dla 7B.

## Jeden plik firmware do release

Po zbudowaniu wariantu RS-232 można utworzyć kompletny obraz zawierający
bootloader, tablicę partycji i aplikację:

```sh
cmake --build build-rs232 --target merge-bin
```

Powstanie plik `build-rs232/merged-binary.bin`, gotowy do wgrania od adresu
`0x0`:

```sh
esptool.py --chip esp32s3 --port PORT write_flash 0x0 merged-binary.bin
```

Do GitHub Release należy dołączyć ten jeden plik, nazwany na przykład
`espVT-v0.1.0-RS232.bin`. Obraz jest przeznaczony dla pamięci flash 16 MB,
trybu DIO i częstotliwości 80 MHz.

Build zostal zweryfikowany na ESP-IDF 5.5 dla ESP32-S3. Firmware inicjalizuje
panel RGB wybrany w Kconfig, pokazuje logo startowe
i ekran oczekiwania na working set maszyny,
i sprawdza kontroler dotyku GT911 po I2C. Po zaladowaniu IOP renderer obsluguje
DataMask/AlarmMask, Container, SoftKeyMask i Key, Button, Input Boolean/String/Number,
Output String/Number, Output Rectangle oraz Output Line. Dotyk wysyla aktywacje przyciskow i klawiszy
programowych albo ustawia fokus pola wejscia.
Pole Input Boolean przelacza lokalnie stan `ON/OFF` i wysyla do ECU nowa wartosc.

## Rozmiar obszaru VT

Na wariancie 7B menu terminala pozwala wybrać logiczny obszar roboczy `480×480`
albo `600×600`. Oba tryby wykorzystują pełną wysokość 600 pikseli wyświetlacza:

- `480x480` jest domyslny i jest skalowany na LCD do `600x600`, co zapewnia
  zgodnosc z pulami zaprojektowanymi na standardowy obszar 480 pikseli,
- `600x600` jest rysowany 1:1 i jest zglaszany ECU w odpowiedzi VT Get Hardware.

Na wariancie 7 ekran ma 800×480 pikseli. Obszar VT `480×480` jest rysowany
1:1, a interfejs diagnostyczny i menu są dopasowane do rozdzielczości. Opcja
`600×600` jest ukryta i nie jest zgłaszana ECU.

Zmiana rozmiaru usuwa zapisane pule obiektow i wymaga ponownego uruchomienia
terminala. ECU z AgIsoStack++, ktore ma pule zaprojektowana dla `480x480`, powinno
po wywolaniu `set_object_pool(...)` wlaczyc skalowanie:

```cpp
virtualTerminalClient.set_object_pool_scaling(0, 480, 60);
```

Bez skalowania po stronie ECU stara pula 480 moze zajac tylko czesc logicznego
obszaru 600. Rozmiar 480 pozostaje wiec bezpiecznym ustawieniem dla nieznanych maszyn.

## Warianty GNSS

Wariant `RTK` z UM982 odbiera NMEA `GGA` i `RMC` po UART, pokazuje jakosc fixu na
ekranie oczekiwania i przekazuje RTCM z NTRIP do odbiornika. Wariant `Serial` odbiera
te same dane NMEA przez MAX3232 i port RS-232. Oba warianty udostepniaja bramke jakosci
pozycji dla przyszlej automatyki sekcji, jezeli odbiornik raportuje swiezy RTK.

Biezacy `sdkconfig` ma wlaczony RS-232 (UART1, RX GPIO11, 57600 bit/s), tak samo
jak `sdkconfig.rs232`. Zachowaj te ustawienia przy zwyklym `idf.py build flash`:
wybor `ISOVT_GNSS_SOURCE_NONE` wylacza odbior GPS i predkosc wysylana po ISOBUS.
Osobny wariant RS-232 mozna zbudowac ponizszym poleceniem; przy wgrywaniu nalezy
uzyc tego samego katalogu i konfiguracji (zastap `build` przez `-p PORT flash`):

```sh
idf.py -B build-rs232 \
  -D SDKCONFIG="$PWD/sdkconfig.rs232" \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.rs232.defaults" build
```

Domyslne polaczenie: `GPIO11 <- RXD` po stronie TTL uzytego konwertera MAX3232 oraz
wspolna masa. W tym module pin opisany `RXD` jest wyjsciem danych odebranych z DB9;
jego `TXD` nie jest wyjsciem do ESP. W wariancie z dzwiekiem port jest tylko
odbiorczy. Nie wolno laczyc poziomow RS-232 bezposrednio z ESP32. Profil nawigacyjny
uzywa 57600 bit/s (8N1).

## Dzwieki VT

Terminal obsluguje priorytety dzwieku obiektow Alarm Mask oraz komendy Control Audio
Signal i Set Audio Volume. Plytka nie ma wbudowanego wzmacniacza. Domyslnym modulem
jest monofoniczny wzmacniacz I2S `MAX98357A`: `BCLK=GPIO6`, `LRC=GPIO12`,
`DIN=GPIO13`, `VIN=5V`, wspolna masa. Glosnik 4 ohm podlacza sie wylacznie pomiedzy
zaciski `SPK+` i `SPK-` modulu. Nie wolno laczyc zadnego zacisku glosnika z masa.
GPIO12/13 sa wspoldzielone z karta SD, dlatego karta SD jest w tym wariancie
niedostepna. Piny i poziomy glosnosci sa konfigurowalne w `menuconfig`.

Przed automatyka sekcji wybierz w `menuconfig` minimalna jakosc GNSS. Domyslnie jest
to `RTK FIX`; `RTK FLOAT` mozna dopuscic dopiero po walidacji w polu dla konkretnej
maszyny i dopuszczalnego nakladania. Szczegoly podlaczenia i konfiguracji sa w
`GNSS_NTRIP.md`.

## Kolejne kroki techniczne

1. Potwierdzic, ze EXIO5 przelacza plytke w tryb CAN.
2. Zmienic domyslny kod producenta i numer terminala przed pierwszym address claim.
3. Dodac brakujace obiekty renderera: PictureGraphic, GraphicsContext, Line/Ellipse/Polygon,
   Input List oraz pelna edycje liczb i tekstu.
4. Dodac ekran przypisan AUX-N i przetestowac przypisanie z prawdziwym kontrolerem AUX.
5. Przetestowac z ECU wysylajacym prosty IOP, a dopiero potem z prawdziwa maszyna.

## Zrodla projektowe

- Waveshare ESP32-S3-Touch-LCD-7, dokumentacja:
  https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7
- Oficjalna konfiguracja Waveshare panelu RGB, GT911 i CH422G:
  https://github.com/waveshareteam/ESP32-S3-Touch-LCD-7/blob/main/examples/Arduino/libraries/ESP32_Display_Panel/src/board/supported/waveshare/BOARD_WAVESHARE_ESP32_S3_TOUCH_LCD_7.h
- Oficjalny sterownik CH422G, adresy poleceń I²C:
  https://github.com/esp-arduino-libs/ESP32_IO_Expander/blob/master/src/port/esp_io_expander_ch422g.c
- Przykład Waveshare RS-485 (RX GPIO15, TX GPIO16):
  https://github.com/waveshareteam/ESP32-S3-Touch-LCD-7/blob/main/examples/Arduino/examples/02_RS485_Test/02_RS485_Test.ino
- Waveshare ESP32-S3-Touch-LCD-7B: https://docs.waveshare.com/ESP32-S3-Touch-LCD-7B
- AgIsoStack++: https://agisostack.com
- AgIsoStack++ installation: https://agisostack-plus-plus.readthedocs.io/en/latest/Installation.html
- AgIsoStack++ VT API: https://agisostack-plus-plus.readthedocs.io/en/latest/api/virtual%20terminal/index.html

## Uruchomienie na plytce

Kolejnosc pierwszego testu i oczekiwane komunikaty sa opisane w `BRINGUP.md`.
