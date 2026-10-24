# Pierwsze uruchomienie plytki

## Warianty płytki

Firmware domyślnie obsługuje ESP32-S3-Touch-LCD-7B (1024×600, CH32V003 I/O,
I²C `0x24`). Wariant ESP32-S3-Touch-LCD-7 (800×480, CH422G) wybierz w
`idf.py menuconfig` albo buduj z `sdkconfig.lcd7.defaults`:

```sh
idf.py -B build-lcd7 \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.lcd7.defaults" build
```

W LCD-7 panel ma rozdzielczość 800×480, PCLK 16 MHz i timingi HSYNC/VSYNC
4/8/8. Linie RGB w kolejności `B0..B4, G0..G5, R0..R4` są na GPIO
`14,38,18,17,10,39,0,45,48,47,21,1,2,42,41,40`; DE=GPIO5, VSYNC=GPIO3,
HSYNC=GPIO46, PCLK=GPIO7. GT911: SDA=GPIO8, SCL=GPIO9, IRQ=GPIO4, adres
domyślny `0x5D`, reset=CH422G EXIO1. Pozostałe wyjścia CH422G: EXIO2
podświetlenie, EXIO3 reset LCD, EXIO5 USB/CAN (wysoki=CAN), EXIO6 zasilanie
LCD. CAN: TX=GPIO20, RX=GPIO19. Płytka ma RS-485 RX=GPIO15/TX=GPIO16, nie
RS-232; dla RS-232 potrzebny jest zewnętrzny MAX3232 i wolne GPIO UART.

Konfiguracja panelu i pinów pochodzi z oficjalnego Waveshare:
[dokumentacja](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7) oraz
[konfiguracja panelu RGB/GT911/CH422G](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-7/blob/main/examples/Arduino/libraries/ESP32_Display_Panel/src/board/supported/waveshare/BOARD_WAVESHARE_ESP32_S3_TOUCH_LCD_7.h).
Obsługa wyjść CH422G korzysta z adresów poleceń WR-SET `0x24` i WR-IO `0x38`
opisanych w [oficjalnym sterowniku](https://github.com/esp-arduino-libs/ESP32_IO_Expander/blob/master/src/port/esp_io_expander_ch422g.c).

## Przed podlaczeniem do magistrali

1. Ustaw prawdziwy kod producenta i unikalny numer terminala w `idf.py menuconfig`:
   `ISOVT_MANUFACTURER_CODE` i `ISOVT_IDENTITY_NUMBER`.
2. Wgraj aktualny firmware z katalogu projektu.
3. Otworz monitor portu szeregowego.

Nie podlaczaj terminala od razu do maszyny roboczej. Pierwszy test CAN wykonaj na
stanowisku 250 kbit/s z poprawna terminacja 120 omow na obu koncach magistrali.

## Co powinno pojawic sie po starcie

Na LCD:

- logo VTISOBUS by FarmNavi,
- u gory ekran stanu `ISOBUS ACCESS TERMINAL`,
- po uruchomieniu CAN: `CAN READY`,
- komunikat `WAITING FOR MACHINE CONNECTION`, az do otrzymania working setu.

W logu powinny pojawic sie wpisy o inicjalizacji LCD RGB, identyfikatorze GT911,
uruchomieniu CAN/TWAI i adresie terminala VT.

## Test dotyku

Dotknij ekranu poza aktywnym working setem. Pole `STATUS` powinno pokazac
wspolrzedne dotkniecia. To potwierdza komunikacje z GT911 oraz orientacje osi.

Na LCD-7 obszar VT `480×480` powinien być wyświetlany bez skalowania. Wariant
ten nie udostępnia ustawienia obszaru `600×600`.

## Test ISOBUS

Po przeslaniu poprawnego object poola przez ECU terminal sam odrysuje aktywny
DataMask. Aktualnie obslugiwane sa podstawowe maski, kontenery, klawisze
programowe, przyciski, pola bool/string/liczbowe, teksty, liczby i prostokaty.

Dotkniecie przycisku wysyla do ECU pojedyncze `pressed` i `released`. Dotkniecie
pola wejscia wysyla komunikat ustawienia fokusu; pole logiczne przelacza wartosc
`ON/OFF`. Pelny edytor tekstu i liczb oraz
grafika/picture graphic sa kolejnym etapem.

## Test AUX-N

Po obecnosci urzadzenia AUX-N Type 2 log powinien zglosic wykrycie jego NAME oraz
kodu modelu. Warstwa protokolowa ma juz wysylanie przypisan i odpowiedzi; ekran
konfiguracji przypisan zostanie dodany po potwierdzeniu komunikacji z prawdziwym
kontrolerem.

## Weryfikacja sprzętowa

Konfiguracja pinów i timingów jest zweryfikowana względem dokumentacji oraz
przykładów Waveshare, ale inicjalizacja LCD-7/CH422G, dotyk, wybór USB/CAN,
CAN/TWAI i rzeczywiste wyświetlanie trzeba potwierdzić na fizycznej płytce.
Szczególnie sprawdź po uruchomieniu podświetlenie, reset LCD/GT911 oraz
komunikację CAN po przełączeniu EXIO5. Sama kompilacja nie potwierdza działania
elektrycznego.
