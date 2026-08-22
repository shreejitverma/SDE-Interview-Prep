---
tags: [trading/networking, trading/hardware, type/concept]
aliases: [Colocation, Physical Layer, Fiber Optics, Microwave Networks, Propagation Delay, Hollow-Core Fiber, Golden Triangle]
status: evergreen
module: 06
created: 2026-08-22
---

> [!summary]
> In high-frequency trading, latency is fundamentally governed by the physics of electromagnetic propagation. While light travels through standard silica optical fiber at $\approx 4.89\text{ ns/meter}$ ($c / 1.47$), microwave and millimeter-wave wireless networks propagate through air at the speed of light in a vacuum ($\approx 3.33\text{ ns/meter}$), cutting Chicago-to-New Jersey transit time from 14.5ms down to 7.85ms.

---

## Why it matters
Software optimizations (kernel bypass, lock-free rings, C++ assembly tuning) operate in the **10 to 500 nanosecond domain**.

However:
- Moving a server cabinet **20 meters** farther from the exchange matching engine room adds **100 nanoseconds of optical fiber propagation delay** ($20\text{ m} \times 4.89\text{ ns/m}$).
- Running cross-market arbitrage between CME futures (Aurora, IL) and NASDAQ equities (Carteret, NJ) over fiber takes **14.5 milliseconds**, whereas an ultra-short-path microwave network takes **7.85 milliseconds**—a staggering **6,650,000-nanosecond advantage** that renders fiber completely obsolete for cross-market price signals.

Understanding the physical layer—colocation data centers, optical fiber refraction, hollow-core glass, and wireless RF—is mandatory for global trading architecture.

```mermaid
flowchart LR
    subgraph ChicagoArea ["Chicago (CME Aurora Data Center)"]
        CME[CME E-mini Futures Engine]
    end

    subgraph LongHaulPaths ["Chicago to New Jersey Transits (~730 miles straight line)"]
        FIBER["Silica Fiber Cable (Glass n=1.47):\n~14.50 ms RTT (4.89 ns/m)"]
        MW["Microwave / RF Towers (Air n=1.0003):\n~7.85 ms RTT (3.33 ns/m)"]
    end

    subgraph NJGoldenTriangle ["New Jersey Golden Triangle (Equities Exchanges)"]
        CARTERET["NASDAQ (Carteret, NJ)"]
        MAHWAH["NYSE (Mahwah, NJ)"]
        SECAUCUS["Cboe / BATS (Secaucus NY4)"]
    end

    CME -->|Slow High-Bandwidth Backup| FIBER
    CME ==>|Ultra-Low-Latency Signal Path (2x Faster)| MW
    FIBER --> NJGoldenTriangle
    MW --> NJGoldenTriangle
```

---

## Mechanism

### 1. The Physics of Optical and Wireless Propagation

| Transmission Medium | Index of Refraction ($n$) | Velocity of Propagation | Delay per Meter | Chicago $\leftrightarrow$ NJ One-Way (730 mi) |
| :--- | :--- | :--- | :--- | :--- |
| **Vacuum ($c$)** | $n = 1.0000$ | $299,792\text{ km/s}$ | **$3.335\text{ ns/m}$** | **$3.92\text{ ms}$ (Theoretical Minimum)** |
| **Air (Microwave / RF)**| $n = 1.0003$ | $299,700\text{ km/s}$ | **$3.336\text{ ns/m}$** | **$3.93\text{ ms}$ (Actual RTT $\approx 7.85\text{ ms}$)**|
| **Hollow-Core Fiber** | $n \approx 1.00$ | $\approx 299,000\text{ km/s}$ | **$3.340\text{ ns/m}$** | $\approx 3.95\text{ ms}$ |
| **Silica Glass Fiber** | $n \approx 1.468$ | $204,218\text{ km/s}$ | **$4.896\text{ ns/m}$** | **$7.25\text{ ms}$ (Actual RTT $\approx 14.50\text{ ms}$)**|

$$\text{Propagation Latency} = \frac{\text{Distance}}{c / n} = \text{Distance} \times \left(\frac{n}{c}\right)$$

### 2. The New Jersey "Golden Triangle" Colocation Centers
All US equity and options exchanges are concentrated in three primary data centers in New Jersey:
1. **Carteret, NJ**: Hosts **NASDAQ** and direct feeds.
2. **Mahwah, NJ**: Hosts **NYSE / ICE** matching engines.
3. **Secaucus, NJ (Equinix NY4)**: Hosts **Cboe, BATS, Direct Edge, EDGX**, and wholesale FX venues.

*Inter-facility transit*: Carteret to Secaucus is $\approx 15\text{ miles} \implies \approx 200\text{ µs RTT}$ over short-path optical fiber.

### 3. Transceiver Physical Media Comparison
- **Direct Attach Copper (DAC / Twinax)**: Passive copper cables (<3 meters). Zero optical conversion latency (**<1 ns**). Used exclusively for Intra-Rack server-to-ToR switch connections.
- **Short-Range Multi-Mode Fiber (SR / 850nm)**: Multi-mode optical fiber (<100 meters). Transceiver delay $\approx 2–5\text{ ns}$.
- **Long-Range Single-Mode Fiber (LR / 1310nm)**: Single-mode fiber for data-center cross-connects and metro spans. Minimal dispersion across kilometers.

---

## In Practice

### Calculating Nanosecond Latency Penalties for Physical Cable Runs

```cpp
#include <iostream>
#include <iomanip>

constexpr double SPEED_OF_LIGHT_VACUUM = 299'792'458.0; // m/s
constexpr double REFRACTIVE_INDEX_SILICA = 1.4682;       // Standard Corning SMF-28
constexpr double REFRACTIVE_INDEX_HOLLOW_CORE = 1.00;    // Air-core fiber
constexpr double REFRACTIVE_INDEX_AIR = 1.0003;          // Microwave RF

// Calculate propagation delay in nanoseconds
inline double calculate_cable_delay_ns(double distance_meters, double refractive_index) noexcept {
    double speed_in_medium = SPEED_OF_LIGHT_VACUUM / refractive_index;
    double time_seconds = distance_meters / speed_in_medium;
    return time_seconds * 1e9;
}

int main() {
    std::cout << "Physical Cable Propagation Latency Breakdown:\n";
    std::cout << "--------------------------------------------------\n";

    double patch_cable_10m = calculate_cable_delay_ns(10.0, REFRACTIVE_INDEX_SILICA);
    double rack_cross_connect_50m = calculate_cable_delay_ns(50.0, REFRACTIVE_INDEX_SILICA);
    double hollow_core_50m = calculate_cable_delay_ns(50.0, REFRACTIVE_INDEX_HOLLOW_CORE);

    std::cout << " 10-meter Silica Patch Cable:   " << std::fixed << std::setprecision(2) << patch_cable_10m << " ns\n";
    std::cout << " 50-meter Equalized Colo Spool: " << rack_cross_connect_50m << " ns\n";
    std::cout << " 50-meter Hollow-Core Fiber:    " << hollow_core_50m << " ns\n";
    std::cout << " Hollow-Core Latency Savings:   " << (rack_cross_connect_50m - hollow_core_50m) << " ns (31.8% faster)\n";
    return 0;
}
```

---

## Numbers

| Route / Medium | Technology | Round-Trip Time (RTT) | Bandwidth | Weather Dependency |
| :--- | :--- | :--- | :--- | :--- |
| **Chicago $\leftrightarrow$ NJ (Microwave)** | Short-Path Wireless RF | **~7.85 ms** | 100 Mbps – 1 Gbps | Degrades during heavy rain (rain fade) |
| **Chicago $\leftrightarrow$ NJ (Fiber)** | Underground Silica Fiber | **~14.50 ms** | 100 Gbps+ | 99.999% Reliability |
| **Carteret $\leftrightarrow$ Mahwah (Metro)** | Dark Fiber (Single Mode) | **~380 µs** | 10G / 25G | 100% Reliable |
| **Intra-Colo 50m Cross-Connect** | Silica Optical Fiber | **~245 ns (One-Way)** | 25G / 100G | 100% Reliable |
| **Intra-Rack Server-to-Switch** | Direct Attach Copper (DAC)| **<1 ns** | 25G SFP28 | 100% Reliable |

---

## Trade-offs

| Transmission Medium | Latency Advantage | Reliability / Bandwidth Cost |
| :--- | :--- | :--- |
| **Microwave Wireless (Air)** | **2x Faster than fiber (7.85ms vs 14.5ms)**. | Vulnerable to rain fade; low bandwidth; high tower lease costs. |
| **Hollow-Core Optical Fiber** | 30% faster than standard fiber in data centers. | Fragile manufacturing; significantly higher cost per meter. |
| **Standard Silica Dark Fiber** | Maximum bandwidth; 99.999% weather-independent uptime. | Light travels 30% slower than through air. |

---

> [!warning] Gotchas
> 1. **The Fiber Bend Radius Attenuation Trap**: Bending a high-speed optical patch cable tighter than its rated minimum bend radius (typically <30mm) causes optical signal attenuation and photon leakage, resulting in physical-layer bit errors and dropped packets at the receiver PHY.
> 2. **Rain Fade Microwave Failover Jitter**: When a thunderstorm strikes the Midwest, microwave links drop out due to atmospheric water absorption. Trading systems must instantly failover to backup optical fiber; algorithms unaware of the failover will miscalculate cross-market price lead-lag times and lose trades to competitors with better microwave towers.

---

## Lab
**Objective**: Calculate the exact physical propagation delay across your colocation cross-connect topology, compare standard silica fiber vs hollow-core fiber vs direct attach copper (DAC), and build an automated microwave-to-fiber failover simulator.

**Success Criteria**:
1. Compute the physical propagation delay for 50m, 100m, and 500m fiber runs down to 0.1 nanoseconds.
2. Build a C++ failover detector that monitors microwave link packet loss and switches to backup fiber in <5 microseconds.

---

> [!question]- Self-test
> 1. **Why does light travel approximately 30% slower through standard optical fiber than through air or a vacuum?**
>    *Answer*: The speed of electromagnetic propagation in a medium is determined by the medium's refractive index ($v = \frac{c}{n}$). Standard optical fiber is constructed from fused silica glass, which has a refractive index of $n \approx 1.468$, slowing light to $\approx 204,000\text{ km/s}$ ($4.89\text{ ns/meter}$). Air has a refractive index of $n \approx 1.0003$, allowing microwave radio waves to travel at nearly the speed of light in a vacuum ($\approx 300,000\text{ km/s}$ / $3.33\text{ ns/meter}$).
> 2. **What is the "Golden Triangle" in US equity market structure and what data centers comprise it?**
>    *Answer*: The Golden Triangle refers to the three primary data centers in Northern New Jersey where all major US equity and options matching engines are hosted: (1) **Carteret, NJ** (NASDAQ); (2) **Mahwah, NJ** (NYSE / ICE); and (3) **Secaucus, NJ / Equinix NY4** (Cboe, BATS, EDGX, and institutional FX platforms).
> 3. **Why do high-frequency trading firms use Direct Attach Copper (DAC) cables instead of optical fiber transceivers inside the same server rack?**
>    *Answer*: Direct Attach Copper (DAC) cables transmit electrical signals directly from the NIC to the switch ASIC without converting electrical signals into optical photons and back again. This eliminates the optical transceiver (PHY/SerDes) optical-to-electrical conversion delay, reducing latency from 2–5 nanoseconds down to **<1 nanosecond**.

---

## Related
- [[06 - Networking/Network Interface Card Architecture]]
- [[06 - Networking/Switch Architectures in Trading]]
- [[07 - Time & Measurement/Latency Numbers Every Trading Engineer Knows]]
- [[07 - Time & Measurement/One-Way Latency vs Round-Trip Time Measurement]]
- [[06 - Networking/MOC - 06 Networking]]

## Sources
- [[Sources/Flash Boys by Michael Lewis (Spread Networks Fiber History)]]
- [[Sources/Corning SMF-28 Ultra Optical Fiber Specification]]
- [[Sources/How to Build an Exchange by Jane Street]]
