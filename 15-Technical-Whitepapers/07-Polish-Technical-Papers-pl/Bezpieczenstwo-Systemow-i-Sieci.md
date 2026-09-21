---
tags: [security/defensive, selinux, rhel6, dos, pki, postgresql, linux/administration, polish]
aliases: [Bezpieczeństwo Systemów i Sieci, SELinux Obowiązkowa Kontrola Dostępu, RHEL6 Bezpieczeństwo, Administracja PostgreSQL]
status: evergreen
created: 2026-09-17
type: paper
track: [distinguished, sde]
level:
last_reviewed:
sources: []
---

# Bezpieczeństwo Systemów, Sieci i Administracja

> [!summary]
> Opracowanie analizujące polskie publikacje z zakresu bezpieczeństwa infrastruktury serwerowej i sieciowej: obowiązkowa kontrola dostępu SELinux (Jaroszuk, Brodecki, Sasak), innowacje bezpieczeństwa w RHEL6 (Leszek Miś), obrona przed atakami Denial of Service (Marcin Żurakowski), bezpieczeństwo kart kryptograficznych PKI (Adam Augustyn) oraz administracja rolami i uprawnieniami w bazie PostgreSQL (Ligęza, Szpyrka).

---

## 1. SELinux i Obowiązkowa Kontrola Dostępu (Jaroszuk; Brodecki & Sasak)

### Porównanie Modeli Kontroli Dostępu
Brodecki i Sasak analizują ewolucję mechanizmów ochrony w systemie GNU/Linux:
- **Discretionary Access Control (DAC)**: Oparty na uprawnieniach użytkowników (`rwxr-xr-x`). Jeśli serwer Apache działa z uprawnieniami użytkownika `apache`, a ten użytkownik posiada dostęp do odczytu wrażliwych katalogów, błąd w skrypcie CGI ujawnia te pliki.
- **Mandatory Access Control (MAC)**: Architektura oparta na etykietach bezpieczeństwa. Jądro systemu wymusza politykę globalną niezależnie od decyzji użytkownika.

### Architektura Flask w SELinux (Robert Jaroszuk)
Jaroszuk szczegółowo wyjaśnia implementację architektury Flask w SELinux:
- **Kontekst Bezpieczeństwa**: Format `użytkownik:rola:typ:poziom` (np. `system_u:system_r:httpd_t:s0`).
- **Type Enforcement (TE)**: Kluczowy komponent SELinux. Definiuje reguły zezwalające, np.:
  ```text
  allow httpd_t httpd_sys_content_t : file { read getattr open };
  ```
  Oznacza to, że proces o domenie `httpd_t` może odczytywać wyłącznie pliki oznaczone typem `httpd_sys_content_t`. Nawet jeśli atakujący zdobędzie shella jako `root` w procesie `httpd_t`, SELinux zablokuje próbę odczytu `/etc/shadow` (typ `shadow_t`), wywołując odmowę AVC (Access Vector Cache).

---

## 2. Nowe Mechanizmy Bezpieczeństwa w RHEL6 (Leszek Miś, 2011)

### Ewolucja Bezpieczeństwa w Dystrybucjach Enterprise
Leszek Miś dokumentuje kluczowe innowacje wprowadzone w dystrybucji Red Hat Enterprise Linux 6:
1. **Wprowadzenie Cgroups (Control Groups)**: Sprzętowa kontrola i limitowanie zasobów (CPU, RAM, I/O) na poziomie grup procesów, stanowiąca fundament późniejszej rewolucji kontenerowej (LXC/Docker).
2. **Udoskonalenia Podsystemu Audytu (`auditd`)**: Śledzenie wywołań systemowych w czasie rzeczywistym z gwarancją niezmienności logów.
3. **Kompilacja Pakietów ze Wsparciem PIE i RELRO**:
   - `Position Independent Executable (PIE)`: Pełna randomizacja adresu bazowego programu binarnego w pamięci RAM.
   - `Full RELRO`: Ustawienie sekcji GOT (Global Offset Table) w tryb tylko do odczytu po załadowaniu bibliotek dynamicznych, uniemożliwiające ataki typu GOT Overwrite.

---

## 3. Obrona przed Atakami Denial of Service (Marcin Żurakowski, 2004)

### Metodyka Neutralizacji Ataków DoS
Żurakowski klasyfikuje ataki sieciowe na poziomie warstwy 3 i 4 modelu OSI:
- **SYN Flood**: Wyczerpanie kolejki połączeń półotwartych (`backlog`). Obrona: Aktywacja mechanizmu `syncookies` w jądrze (`sysctl -w net.ipv4.tcp_syncookies=1`).
- **Smurf Attack**: Wysyłanie pakietów ICMP Echo Request na adres rozgłoszeniowy podsieci (Broadcast) ze sfałszowanym adresem IP ofiary jako nadawcy. Obrona: Wyłączenie odpowiedzi na broadcasty (`net.ipv4.icmp_echo_ignore_broadcasts = 1`).
- **Limitowanie Połączeń w Iptables**:
  ```bash
  # Ograniczenie liczby nowych połączeń TCP na port 80 do 20 na minutę z jednego IP:
  iptables -A INPUT -p tcp --dport 80 -m state --state NEW -m recent --set
  iptables -A INPUT -p tcp --dport 80 -m state --state NEW -m recent --update --seconds 60 --hitcount 20 -j DROP
  ```

---

## 4. Karty Elektroniczne w PKI (Adam Augustyn, 2005)

### Bezpieczeństwo Fizycznych Modułów Kryptograficznych
Adam Augustyn bada wektory ataków na inteligentne karty chipowe (Smart Cards) w infrastrukturze klucza publicznego (PKI):
- **Klucz Prywatny**: Przechowywany w chronionej pamięci EEPROM karty, nigdy nie opuszcza fizycznego układu scalonego. Wszystkie operacje podpisu i deszyfrowania RSA odbywają się wewnątrz procesora karty.
- **Ataki Kanałem Bocznym (Side-Channel Attacks)**:
  - *DPA (Differential Power Analysis)*: Analiza mikrofluktuacji poboru prądu przez procesor karty podczas operacji kryptograficznych ujawnia bity klucza prywatnego.
  - *Ataki Błędami (Fault Injection)*: Celowe zakłócenia zasilania lub naświetlanie układu promieniem lasera w celu wymuszenia błędu w algorytmie weryfikacji kodu PIN.

---

## 5. Administracja PostgreSQL: DCL, Role i Uprawnienia (Ligęza & Szpyrka)

### Bezpieczeństwo i Zarządzanie Uprawnieniami w Bazie Danych
Antoni Ligęza i Marcin Szpyrka omawiają architekturę Data Control Language (DCL) w PostgreSQL:
- **Model Ról**: PostgreSQL unifikuje pojęcia użytkowników i grup w pojedynczą abstrakcję: `ROLE`.
- **Zasada Separacji Przywilejów**:
  ```sql
  -- Utworzenie bezpiecznej roli tylko do odczytu (Audytor)
  CREATE ROLE auditor WITH LOGIN PASSWORD 'TylkoOdczyt2026!';
  GRANT CONNECT ON DATABASE finanse_db TO auditor;
  GRANT USAGE ON SCHEMA public TO auditor;
  GRANT SELECT ON ALL TABLES IN SCHEMA public TO auditor;
  ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT SELECT ON TABLES TO auditor;
  ```
- **Kopie Zapasowe i Odzyskiwanie**: Wykorzystanie narzędzi `pg_dump` do zrzutów logicznych oraz archiwizacji logów transakcyjnych WAL (Write-Ahead Logging) w celu odzyskiwania stanu bazy do określonego punktu w czasie (Point-in-Time Recovery - PITR).

---

## Powiązane Opracowania
- [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening|Linux Operating System Hardening]]
- [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec|Application and Infrastructure Security]]
- [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS|Network Diagnostics and DDoS]]
- [[../README|Główny Indeks Whitepaperów]]
