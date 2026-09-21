---
tags: [security/offensive, security/defensive, reverse-engineering, malware, rootkits, polish, type/index]
aliases: [Polish Whitepapers Index, Polskie Whitepapery Techniczne, Polish Papers Catalog]
status: evergreen
created: 2026-09-17
type: paper
track: [distinguished, sde]
level:
last_reviewed:
sources: []
---

# Polish Directory (pl) - Polskie Whitepapery Techniczne

> [!summary]
> Kompletny katalog **31 polskich publikacji i opracowań technicznych (~112 MB)** dedykowanych inżynierii wstecznej (reverse engineering), tworzeniu shellcode'ów, analizie malware i rootkitów jądra Linux, bezpieczeństwu aplikacji WWW, Mandatory Access Control (SELinux) oraz audytowi i administracji baz danych PostgreSQL. Całość została zweryfikowana raportem [[../ClamAV-Audit-Report|ClamAV]].

---

## 1. Inżynieria Wsteczna i Analiza Kodu Wykonywalnego (Reverse Engineering)

| Tytuł Opracowania | Autor(zy) | Rok | Zakres Techniczny | Przewodnik Szczegółowy |
| :--- | :--- | :--- | :--- | :--- |
| **Szperając w nagłówkach, czyli wstęp do reverse engineeringu** | Wojciech Warpechowski | 2005 | Formaty PE/ELF, nagłówki sekcji, tablice importów/eksportów | [[../07-Polish-Technical-Papers-pl/Inzynieria-Wsteczna-i-Analiza-Kodu#1-szperajac-w-naglowkach-wojciech-warpechowski-2005\|Inżynieria Wsteczna]] |
| **Reverse engineering - analiza dynamiczna kodu wykonywalnego ELF** | Marek Janiczek | - | Śledzenie procesów, pułapki debugera, deasemblacja x86 w Linuksie | [[../07-Polish-Technical-Papers-pl/Inzynieria-Wsteczna-i-Analiza-Kodu#2-reverse-engineering-analiza-dynamiczna-elf-marek-janiczek\|Inżynieria Wsteczna]] |
| **Tryb chroniony mikroprocesorów x86** | Andrzej Stasiak | - | Deskryptory GDT/LDT, selektory segmentów, stronicowanie, pierścienie ochrony | [[../07-Polish-Technical-Papers-pl/Inzynieria-Wsteczna-i-Analiza-Kodu#3-tryb-chroniony-mikroprocesorow-x86-andrzej-stasiak\|Inżynieria Wsteczna]] |
| **Ewolucja Kodów Powłoki** | Itzik Kotler | - | Shellcode assembly, unikanie znaków null (`\x00`), optymalizacja rozmiaru | [[../07-Polish-Technical-Papers-pl/Inzynieria-Wsteczna-i-Analiza-Kodu#4-ewolucja-kodow-powloki-itzik-kotler\|Inżynieria Wsteczna]] |
| **Tworzenie polimorficznego szelkodu** | Michał Piotrowski | - | Enkodery XOR/ROL, mutacje instrukcji, omijanie detekcji sygnaturowej IDS | [[../07-Polish-Technical-Papers-pl/Inzynieria-Wsteczna-i-Analiza-Kodu#5-tworzenie-polimorficznego-szelkodu-michal-piotrowski\|Inżynieria Wsteczna]] |

---

## 2. Malware, Rootkity i Zaawansowane Zagrożenia (Malware & Rootkits)

| Tytuł Opracowania | Autor(zy) | Rok | Zakres Techniczny | Przewodnik Szczegółowy |
| :--- | :--- | :--- | :--- | :--- |
| **Hakowanie aplikacji: Rootkity i Ptrace** | Stefan Klaas | - | Wstrzykiwanie kodu przez `ptrace(PTRACE_POKETEXT)`, podmienianie tablicy syscalli | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#1-hakowanie-aplikacji-rootkity-i-ptrace-stefan-klaas\|Malware & Rootkity]] |
| **Własny rootkit w GNU/Linuksie** | Mariusz Burdach | - | Moduły LKM, ukrywanie procesów w `/proc`, modyfikacja `sys_call_table` | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#2-wlasny-rootkit-w-gnulinuksie-mariusz-burdach\|Malware & Rootkity]] |
| **Praktyczna aplikacja do analizy Malware** | Rubén Santamarta | - | Środowiska sandbox, hooking API, analiza behawioralna próbek | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#3-praktyczna-aplikacja-do-analizy-malware-rubén-santamarta\|Malware & Rootkity]] |
| **Robaki sieciowe** | Opracowanie Techniczne | - | Mechanizmy samoreplikacji, skanowanie podsieci, payloady sieciowe | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#4-robaki-sieciowe-analiza-wektorow-rozprzestrzeniania\|Malware & Rootkity]] |
| **Rodzaje wirusów komputerowych** | Bartłomiej Rudzki, Piotr Modzelewski | - | Klasyfikacja: boot-sektorowe, pasożytnicze, polimorficzne, makrowirusy | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#5-rodzaje-wirusow-komputerowych-rudzki--modzelewski\|Malware & Rootkity]] |
| **Wirusy** | Opracowanie Akademickie | - | Anatomia infekcji plików binarnych, techniki stealth, heurystyka AV | [[../07-Polish-Technical-Papers-pl/Malware-Rootkity-i-Zagrozenia#6-wirusy-mechanika-infekcji-i-techniki-stealth\|Malware & Rootkity]] |

---

## 3. Bezpieczeństwo Aplikacji WWW i Baz Danych (Web & Database Security)

| Tytuł Opracowania | Autor(zy) | Rok | Zakres Techniczny | Przewodnik Szczegółowy |
| :--- | :--- | :--- | :--- | :--- |
| **Bezpieczeństwo aplikacji WWW (2015)** | Michał Sajdak (Niebezpiecznik / Sekurak) | 2015 | Podatności XSS, CSRF, Clickjacking, mechanizmy autoryzacji | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#1-cykl-bezpieczenstwo-aplikacji-www-michal-sajdak-2015-2017\|Web Security]] |
| **Bezpieczeństwo aplikacji WWW: Podatności uploadu** | Michał Sajdak | 2016 | Omijanie weryfikacji MIME, rozszerzeń podwójnych, PHP execution | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#1-cykl-bezpieczenstwo-aplikacji-www-michal-sajdak-2015-2017\|Web Security]] |
| **Bezpieczeństwo aplikacji WWW (2016)** | Michał Sajdak | 2016 | Nowe wektory ataków na sesje, parsowanie JSON, CORS issues | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#1-cykl-bezpieczenstwo-aplikacji-www-michal-sajdak-2015-2017\|Web Security]] |
| **Bezpieczeństwo aplikacji WWW (2017)** | Michał Sajdak | 2017 | Nowoczesne standardy obrony (CSP, HSTS, Secure/HttpOnly cookies) | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#1-cykl-bezpieczenstwo-aplikacji-www-michal-sajdak-2015-2017\|Web Security]] |
| **Aplikacje webowe na celowniku** | Leszek Miś | - | Metodyka audytu bezpieczeństwa, rekonesans, testy penetracyjne aplikacji | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#2-aplikacje-webowe-na-celowniku-leszek-mis\|Web Security]] |
| **Metody włamań: SQL injection** | Bogusław Kluge, Karina Łuksza, Ewa Mąkosa | - | Teoria wstrzykiwania SQL, techniki UNION-based, blind SQLi | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#3-metody-wlaman-do-systemow-sql-injection-kluge-luksza-makosa\|Web Security]] |
| **Ochrona aplikacji poprzez kodowanie defensywne** | Kenny Kerr | - | Zasady bezpiecznego programowania, walidacja danych wejściowych | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#4-ochrona-aplikacji-poprzez-kodowanie-defensywne-kenny-kerr\|Web Security]] |
| **Obrona przed Fingerprinting warstwy aplikacji** | Opracowanie / Tutorial | - | Ukrywanie nagłówków `Server`, banner grabbing, unikanie enumeracji technologii | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#5-obrona-przed-fingerprinting-warstwy-aplikacji\|Web Security]] |
| **Niebezpieczne Google - wyszukiwanie poufnych informacji** | Michał Piotrowski | - | Google Dorking, odnajdywanie paneli administracyjnych, leaków baz i haseł | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Web-i-Baz-Danych#6-niebezpieczne-google-wyszukiwanie-informacji-michal-piotrowski\|Web Security]] |

---

## 4. Bezpieczeństwo Systemów, Sieci i Administracja (Systems & Network Security)

| Tytuł Opracowania | Autor(zy) | Rok | Zakres Techniczny | Przewodnik Szczegółowy |
| :--- | :--- | :--- | :--- | :--- |
| **SELinux** | Robert Jaroszuk | 2011 | Architektura Flask, reguły polityk Type Enforcement, konteksty plików | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#1-selinux-i-obowiazkowa-kontrola-dostepu-jaroszuk-brodecki-sasak\|Systemy & Sieci]] |
| **Obowiązkowa kontrola dostępu w systemie Linux** | Bartosz Brodecki, Piotr Sasak | 2007 | Porównanie MAC (SELinux, AppArmor, SMACK) vs tradycyjne DAC | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#1-selinux-i-obowiazkowa-kontrola-dostepu-jaroszuk-brodecki-sasak\|Systemy & Sieci]] |
| **Przegląd nowych mechanizmów bezpieczeństwa w RHEL6**| Leszek Miś | 2011 | Nowości w kernelu Red Hat Enterprise Linux 6, hardening, cgroups | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#2-przeglad-nowych-mechanizmow-bezpieczenstwa-w-rhel6-leszek-mis\|Systemy & Sieci]] |
| **Obrona przed atakami typu odmowa usługi (DoS)** | Marcin Żurakowski | 2004 | SYN flood, Smurf attack, techniki filtrowania pakietów, limitowanie pasma | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#3-obrona-przed-atakami-typu-odmowa-uslugi-dos-marcin-zurakowski\|Systemy & Sieci]] |
| **Karty elektroniczne w PKI - znane ataki i obrona** | Adam Augustyn | 2005 | Smart cards, kryptografia asymetryczna, ataki side-channel (DPA/SPA) | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#4-karty-elektroniczne-w-pki-adam-augustyn-2005\|Systemy & Sieci]] |
| **Prywatne wojny w sieci: poddaj się, okop lub walcz**| Opracowanie Strategiczne | - | Psychologia bezpieczeństwa, higiena cyfrowa, modelowanie zagrożeń | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#5-prywatne-wojny-w-sieci-i-strategie-obrony\|Systemy & Sieci]] |
| **Struktury systemów operacyjnych** | Przegląd Akademicki | - | Jądra monolityczne, mikrojądra, zarządzanie pamięcią wirtualną | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#6-struktury-systemow-operacyjnych-i-gentoo-linux\|Systemy & Sieci]] |
| **Gentoo Linux Instrukcja instalacji i konfiguracji**| Poradnik Systemowy | - | Kompilacja jądra ze źródeł, flagi CFLAGS/CXXFLAGS, minimalny system | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#6-struktury-systemow-operacyjnych-i-gentoo-linux\|Systemy & Sieci]] |
| **Podstawy administracji PostgreSQL: DCL, role, uprawnienia, kopie**| Antoni Ligęza, Marcin Szpyrka | - | Bezpieczeństwo PostgreSQL, `GRANT`/`REVOKE`, pg_dump, zarządzanie rolami | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#7-podstawy-administracji-postgresql-dcl-ligeza--szpyrka\|Systemy & Sieci]] |
| **IT Security Services** | Mateusz Kocielski | 2015 | Architektura usług bezpieczeństwa, audyty podatności, zarządzanie ryzykiem | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#8-it-security-services-oraz-administrowanie-sieciami-kocielski--kobus\|Systemy & Sieci]] |
| **Administrowanie sieciami lokalnymi i serwerami** | Jacek Kobus | 2016 | Konfiguracja usług sieciowych (DNS, DHCP, Samba), routing, monitorowanie | [[../07-Polish-Technical-Papers-pl/Bezpieczenstwo-Systemow-i-Sieci#8-it-security-services-oraz-administrowanie-sieciami-kocielski--kobus\|Systemy & Sieci]] |

---

## Powiązane Zasoby
- [[../README|Główny Indeks Whitepaperów (MOC)]]
- [[../ClamAV-Audit-Report|Raport Audytu Bezpieczeństwa ClamAV]]
- [[../base/Index-Base-English-Whitepapers|Katalog Whitepaperów Angielskich (Base)]]
