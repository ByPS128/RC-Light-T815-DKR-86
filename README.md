# RC-Light Controller for T815-DKR-86

## Úvod

RC-Light Controller je sofistikovaný systém pro ovládání osvětlení RC modelů, specificky navržený pro model Tatra 815 1986 DAKAR v měřítku 1:13. Tento projekt kombinuje hardwarové a softwarové řešení pro vytvoření realistického a plně funkčního osvětlovacího systému pro RC modely.

## Osnova

1. [Přehled funkcí](#přehled-funkcí)
2. [Architektura systému](#architektura-systému)
3. [Hlavní komponenty](#hlavní-komponenty)
4. [Konfigurace a přizpůsobení](#konfigurace-a-přizpůsobení)
5. [Optimalizace a efektivita](#optimalizace-a-efektivita)
6. [Hardwarové zapojení](#hardwarové-zapojení)
7. [Instalace a použití](#instalace-a-použití)
8. [Struktura projektu a popis souborů](#struktura-projektu-a-popis-souborů)

## Přehled funkcí

- Několik režimů osvětlení včetně denního světla, dlouhého světla a plného osvětlení
- Dynamické ovládání jasu
- Realistické brzdové a zpětné světlo
- Kalibrace RC kanálů pro přesné ovládání
- Detekce a indikace ztráty signálu
- Programovatelné blikací sekvence pro různé stavy systému

## Architektura systému

Systém je postaven na platformě Arduino a využívá objektově orientovaný přístup pro modulární a rozšiřitelný design. Klíčové třídy zahrnují:

- `MainApp`: Centrální řídící třída koordinující všechny komponenty
- `LightsController`: Spravuje různé režimy osvětlení
- `RCChannel`: Zpracovává vstupní signály z RC přijímače
- `CalibrationManager`: Zajišťuje přesnou kalibraci RC kanálů
- `LedBlinker`: Poskytuje flexibilní rozhraní pro programovatelné LED sekvence

## Hlavní komponenty

### MainApp

Tato třída slouží jako hlavní vstupní bod aplikace. Inicializuje všechny ostatní komponenty, zpracovává vstupy z tlačítek a koordinuje celkové chování systému.

### LightsController

Spravuje různé režimy osvětlení a ovládá jednotlivé LED diody. Podporuje plynulé přechody mezi režimy a reaguje na brzdění a couvání.

### RCChannel

Zpracovává vstupní PWM signály z RC přijímače, poskytuje kalibrované hodnoty pro přesné ovládání.

### CalibrationManager

Umožňuje kalibraci RC kanálů pro optimální výkon. Ukládá kalibrační data do EEPROM pro trvalé uchování.

### LedBlinker

Flexibilní systém pro vytváření komplexních blikacích sekvencí, používaný pro indikaci různých stavů systému.

## Konfigurace a přizpůsobení

Systém lze snadno přizpůsobit úpravou konstant v souboru `AppConstants.h`. Klíčové konfigurovatelné parametry zahrnují:

- Mapování pinů pro různé funkce
- Časování a prahové hodnoty pro detekci tlačítek
- Kalibrační konstanty pro RC signály

Pro přizpůsobení chování osvětlení lze upravit třídu `LightsController`, zejména matici `LIGHT_MATRIX`, která definuje různé režimy osvětlení.

## Optimalizace a efektivita

Projekt využívá několik technik pro optimalizaci využití paměti a výkonu:

- Použití `PROGMEM` pro ukládání konstant ve flash paměti
- Efektivní využití EEPROM pro ukládání kalibračních dat
- Optimalizované časování v hlavní smyčce pro snížení spotřeby energie
- Modulární design umožňující snadné vypnutí nepoužívaných funkcí

## Hardwarové zapojení

Součástí projektu je schéma zapojení vytvořené v aplikaci KiCad, které poskytuje detailní pohled na hardwarovou konfiguraci systému. Schéma zahrnuje zapojení Arduino Nano, RC přijímače, LED diod a dalších komponent.

## Instalace a použití

1. Nahrajte kód do Arduino Nano
2. Připojte hardware podle poskytnutého schématu
3. Proveďte kalibraci RC kanálů pomocí vestavěného kalibračního režimu
4. Systém je připraven k použití - ovládejte osvětlení pomocí RC vysílače

Pro detailní instrukce k instalaci a použití viz [dokumentace](./docs/USAGE.md).

---

Tento projekt představuje komplexní a profesionální řešení pro ovládání osvětlení RC modelů. Díky modulárnímu designu, efektivní správě paměti a flexibilní konfiguraci je RC-Light Controller ideální platformou pro modeláře hledající realistické osvětlení pro své RC modely.

## Struktura projektu a popis souborů

### RC-Light-T815-DKR-86.ino
Hlavní soubor projektu Arduino. Obsahuje funkce `setup()` a `loop()`, které inicializují a řídí celou aplikaci. Vytváří instanci `MainApp` a volá její metody pro inicializaci a aktualizaci.

### MainApp.h a MainApp.cpp
Definuje třídu `MainApp`, která slouží jako centrální řídící jednotka celé aplikace. Koordinuje všechny ostatní komponenty, zpracovává vstupy z tlačítek a řídí celkové chování systému. Implementuje rozhraní pro zpracování událostí tlačítek a LED blikačů.

### RCChannel.h a RCChannel.cpp
Třída `RCChannel` reprezentuje jeden kanál RC přijímače. Zpracovává vstupní PWM signály, provádí kalibraci a poskytuje metody pro získání aktuálních hodnot kanálu. Je klíčová pro přesné ovládání modelu.

### CalibrationManager.h a CalibrationManager.cpp
`CalibrationManager` spravuje proces kalibrace RC kanálů. Umožňuje ukládání a načítání kalibračních dat z EEPROM, což zajišťuje, že kalibrace zůstane zachována i po vypnutí napájení.

### LedBlinker.h a LedBlinker.cpp
Třída `LedBlinker` poskytuje flexibilní systém pro vytváření různých sekvencí blikání LED. Používá se pro indikaci různých stavů systému, jako je kalibrace nebo ztráta signálu.

### DigitalPullUpButton.h a DigitalPullUpButton.cpp
`DigitalPullUpButton` je specializovaná třída pro práci s digitálními tlačítky využívajícími interní pull-up rezistory Arduina. Dědí od `ButtonBase` a implementuje specifickou logiku pro detekci stisknutí tlačítka.

### ButtonBase.cpp
Implementace základní třídy `ButtonBase`, která poskytuje obecnou funkcionalitu pro zpracování událostí tlačítek, včetně detekce různých typů stisků (krátký, dlouhý, dvojitý).

### AppConstants.h
Soubor obsahující důležité konstanty používané v celé aplikaci. Definuje mapování pinů, adresy EEPROM, limity pro validaci RC signálu a další konfigurační hodnoty. Centralizace těchto konstant usnadňuje údržbu a přizpůsobení aplikace.

### LICENSE
Soubor obsahující licenční podmínky projektu (MIT License), který definuje, jak může být software používán, modifikován a distribuován.

### README.md
Hlavní dokumentační soubor projektu, poskytující přehled funkcí, instrukce pro instalaci a použití, a další důležité informace pro uživatele a vývojáře.

Každý z těchto souborů hraje důležitou roli v celkové architektuře projektu, přispívá k jeho modularitě, rozšiřitelnosti a snadné údržbě. Společně vytvářejí robustní systém pro ovládání osvětlení RC modelů s pokročilými funkcemi jako je kalibrace, detekce ztráty signálu a flexibilní programování světelných sekvencí.
