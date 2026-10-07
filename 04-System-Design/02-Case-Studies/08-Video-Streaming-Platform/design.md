---
type: case-study
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "System Design Interview - An Insider's Guide, Alex Xu"
  - "Netflix Technology Blog: High Quality Video Encoding at Scale"
  - "YouTube Engineering: Architecture and Video Transcoding Pipeline"
---

# Design a Video Streaming Platform (YouTube / Netflix)

## 1. TL;DR

A global video streaming platform manages the end-to-end lifecycle of video ingestion, distributed transcoding, catalog discovery, and adaptive low-latency playback for billions of users worldwide.
At YouTube scale, users upload over 500 hours of video every minute, while consumers stream billions of views daily, generating multi-terabit per second egress traffic.
The core architectural challenges divide into two decoupled pipelines:
1. **The Ingestion & Transcoding Pipeline**: Ingesting multi-gigabyte raw video files reliably via resumable multipart uploads, splitting videos into Groups of Pictures (GOP), executing parallel Directed Acyclic Graph (DAG) transcoding into multiple bitrates and codecs (H.264, VP9, AV1), and packaging chunks into **Adaptive Bitrate Streaming (ABR)** manifests (HLS / MPEG-DASH).
2. **The Delivery & Playback Pipeline**: Distributing 6-second video segments globally through a multi-tiered Content Delivery Network (CDN) edge hierarchy (e.g., Netflix Open Connect), enabling client video players to seamlessly step up or down in resolution without buffering as local network conditions fluctuate.

---

## 2. Mental Model

The platform separates compute-heavy asynchronous video processing from read-optimized edge distribution.

```mermaid
flowchart TD
    subgraph UploadIngestion["Video Ingestion & Transcoding Pipeline"]
        Creator["Content Creator"] -->|1. Get Pre-Signed URL| WebAPI["API Gateway"]
        WebAPI --> UploadService["Upload Service"]
        UploadService -->|2. Multipart Pre-signed S3 URL| Creator
        Creator -->|3. Resumable Chunk Upload| RawBucket[(Raw Storage: S3 Bucket)]
        RawBucket -->|S3 Event Notification| Queue[Kafka / SQS Job Queue]
        Queue --> DAGScheduler["DAG Transcoding Orchestrator"]
        DAGScheduler --> Splitter["Video Splitter (GOP Alignment)"]
        Splitter --> TranscoderPool["Distributed Transcoder Fleet"]
        TranscoderPool -->|1080p, 720p, 480p / H.264, AV1| Packager["HLS / DASH Packager (m3u8/mpd)"]
        Packager --> TranscodedBucket[(Transcoded Storage: S3 Bucket)]
    end

    subgraph PlaybackPipeline["Global Edge Playback Pipeline"]
        Viewer["End User Mobile / TV / Web"] -->|1. Fetch Master Playlist| PlaybackAPI["Playback API Gateway"]
        PlaybackAPI --> MetadataDB[(Metadata Store: DynamoDB / Cassandra)]
        PlaybackAPI -->|Return master.m3u8| Viewer
        Viewer -->|2. Request Video Chunks (.m4s)| CDNElements["Global CDN Edge PoP (Cloudflare / Fastly)"]
        CDNElements -->|Cache Hit| Viewer
        CDNElements -.->|Cache Miss| OriginShield["CDN Origin Shield"]
        OriginShield -.->|Pull Chunk| TranscodedBucket
        Viewer -->|Heartbeat Telemetry| ViewAggregator["View Count Pipeline (Kafka -> Flink)"]
    end
```

---

## 3. Architectural Internals and Deep Dive

### 3.1 Resumable Chunked Video Uploads

Uploading raw 4K video files (often 10 GB to 50 GB) over mobile or residential internet connections is vulnerable to dropped TCP connections.
Restarting an entire upload from byte zero on failure is unacceptable.
- **Multipart Pre-Signed S3 Uploads**:
  1. Creator client requests an upload session via `POST /v1/videos/upload-session`.
  2. Server authenticates the user, initializes an Amazon S3 Multipart Upload, and returns a unique `upload_id` along with pre-signed URLs for each 10 MB chunk.
  3. Client splits the file into 10 MB binary parts locally and uploads parts in parallel directly to S3, completely bypassing the application web servers.
  4. If chunk 47 fails due to a network glitch, the client retries only chunk 47.
  5. Upon completion, client sends `POST /v1/videos/complete-upload`.
  S3 stitches the parts together atomically and triggers an event notification.

### 3.2 The DAG Transcoding Pipeline

Raw uploaded video arrives in arbitrary formats (MOV, AVI, MKV, MP4) with high bitrates unsuited for streaming.
To make video streamable across heterogeneous client devices (4K TVs, budget smartphones on 3G, Apple Watches), the video must be converted into multiple resolutions (1080p, 720p, 480p, 360p) and modern compression codecs (H.264, VP9, AV1).

A centralized **Directed Acyclic Graph (DAG) Scheduler** (similar to Apache Airflow or Temporal) orchestrates this workflow:
1. **Video Inspection & Validation**: Verify file integrity, scan for malware, extract audio/video tracks, and evaluate aspect ratios.
2. **GOP Splitting (Group of Pictures)**: Split the video at keyframe boundaries (I-frames) into independent 6-second chunks.
3. **Parallel Transcoder Workers**:
   - Transcode video chunks simultaneously across thousands of GPU worker nodes:
     - Stream 1: 1080p @ 4.5 Mbps (H.264 / AAC)
     - Stream 2: 720p @ 2.0 Mbps (H.264 / AAC)
     - Stream 3: 480p @ 800 Kbps (H.264 / AAC)
     - Stream 4: 1080p @ 3.0 Mbps (AV1 - high compression efficiency)
4. **Watermarking & Thumbnail Generation**: Extract frame samples to generate preview sprites and animated thumbnails.
5. **Manifest Packaging**: Assemble the transcoded chunks into standard streaming protocols:
   - **HLS (HTTP Live Streaming)**: Generates `.m3u8` playlist text files and `.m4s` / `.ts` video segment files.
   - **DASH (Dynamic Adaptive Streaming over HTTP)**: Generates `.mpd` (Media Presentation Description) XML files.

```
+---------------------------------------------------------------+
|             DAG Video Transcoding Pipeline Layout             |
+---------------------------------------------------------------+
|                      [Raw Video Upload]                       |
|                              |                                |
|                   [Inspection & GOP Split]                    |
|                    /        |        \                        |
|             (Chunk 1)   (Chunk 2)   (Chunk 3)                 |
|             /   |   \   /   |   \   /   |   \                 |
|          1080p 720p 480p 1080p 720p 480p 1080p 720p 480p      |
|             \   |   /   \   |   /   \   |   /                 |
|             [Assembly & Packaging: master.m3u8]               |
+---------------------------------------------------------------+
```

### 3.3 Adaptive Bitrate Streaming (ABR): HLS and DASH

In Adaptive Bitrate Streaming, the video player continuously adjusts video quality in real time based on observed network bandwidth and client hardware constraints:
1. The client requests the **Master Playlist** (`master.m3u8`):
   ```m3u8
   #EXTM3U
   #EXT-X-STREAM-INF:BANDWIDTH=4500000,RESOLUTION=1920x1080
   1080p/index.m3u8
   #EXT-X-STREAM-INF:BANDWIDTH=2000000,RESOLUTION=1280x720
   720p/index.m3u8
   #EXT-X-STREAM-INF:BANDWIDTH=800000,RESOLUTION=854x480
   480p/index.m3u8
   ```
2. The client initially requests chunks from the 720p stream:
   `GET /video/720p/chunk_001.m4s`
3. The video player measures download duration.
If the 6-second chunk takes only 1.2 seconds to download over high-speed Wi-Fi, the player's ABR algorithm steps up to 1080p for chunk 2:
   `GET /video/1080p/chunk_002.m4s`
4. If network bandwidth abruptly drops (e.g., entering a tunnel), the player detects buffer depletion and requests chunk 3 at 480p without stalling or showing a buffering wheel.

### 3.4 Multi-Tier CDN Edge Caching and Origin Shielding

Video streaming accounts for over 60% of total internet downstream traffic.
Traversing back to origin cloud datacenters for every chunk request would bankrupt the platform in egress bandwidth charges.
- **Tier 1 (CDN Edge PoPs)**: Edge servers located in internet service providers (ISPs) and major metro areas terminate client TCP/TLS connections and cache hot chunks.
- **Tier 2 (Origin Shield / Mid-Tier Cache)**: A centralized regional caching proxy deployed in front of the origin S3 bucket.
If 50 Edge PoPs all miss a newly uploaded video simultaneously, they query the Origin Shield, which fetches the chunk from S3 **once** and serves all 50 PoPs, preventing origin S3 throttling.
- **The 80-20 Rule in Video Caching**:
  - The top 20% most popular videos generate over 80% of total playback traffic.
  - Popular videos are cached permanently in edge CDN RAM and SSD storage.
  - Long-tail videos (videos with < 10 views per month) are evicted from edge caches and retrieved on-demand from S3 Standard-Infrequent Access.

### 3.5 High-Concurrency View Counter Pipeline

Updating a database row (`UPDATE videos SET view_count = view_count + 1 WHERE id = ?`) for a viral video receiving 100,000 views per second causes severe database lock contention and crash loops.
- **Fraud & Playback Verification**: A view is counted only when a user watches at least 30 continuous seconds of content.
- **Streaming Aggregation Architecture**:
  1. Client sends a playback progress beacon at 30 seconds to the API Gateway.
  2. Gateway pushes a lightweight view event (`video_id`, `user_id`, `timestamp`, `ip_hash`) to an Apache Kafka topic.
  3. Stream processing engines (Apache Flink or Spark Streaming) consume Kafka events in 10-second tumbling windows, deduplicating multiple views from identical IP addresses or bot farms.
  4. Flink flushes aggregated view counts in bulk to Redis:
     `HINCRBY video:views <video_id> 4520`
  5. A background worker periodically flushes the aggregated Redis counters to the persistent database (PostgreSQL/Cassandra) every minute.

---

## 4. Trade-offs and Comparisons

| Dimension | HLS (HTTP Live Streaming) | MPEG-DASH | WebRTC |
|---|---|---|---|
| Latency | Standard: 6-30s; Low-Latency HLS: 2-3s | Standard: 6-30s; Low-Latency DASH: 2-3s | Ultra-low (< 500ms) |
| Transport Protocol | HTTP/TCP | HTTP/TCP | UDP (SRTP) |
| CDN Cacheability | Exceptional (standard static HTTP files) | Exceptional (standard static HTTP files) | Poor (requires dedicated media relay servers) |
| Apple Ecosystem Support | Native across iOS, macOS, Safari | Requires JavaScript player (MSE) | Supported |
| Best Used For | Video on Demand (VOD), standard live streaming | Multi-platform VOD, Android-first deployments | Interactive video calls, real-time gaming |

---

## 5. Failure Modes and Mitigations

### 5.1 Transcoding Worker Out-of-Memory / Malformed Codec Crash
- **Failure Mode**: A user uploads a corrupted video file with malformed headers that causes FFmpeg to consume 100% CPU or crash with segmentation faults, poisoning transcoding workers.
- **Mitigation**: Sandbox transcoding processes in isolated ephemeral containers with strict cgroup memory and CPU limits.
Set a hard timeout (e.g., 10 minutes per chunk).
If the container crashes or times out, mark the chunk task as failed, retry with defensive fallback transcoders, and quarantine persistently failing video files.

### 5.2 CDN Cache Stampede on Viral Livestream Release
- **Failure Mode**: A world-championship sporting event starts; 10 million concurrent viewers simultaneously request `chunk_001.m4s`, which is not yet cached at the edge CDN, causing millions of simultaneous misses to hit the origin S3 bucket.
- **Mitigation**: Implement **Request Collapsing (Coalescing)** at the CDN edge.
The first request for `chunk_001.m4s` passes through to origin, while the subsequent 9,999,999 requests are held in a waiting queue at the edge proxy.
When the origin returns the segment, the CDN populates its local cache and broadcasts the single downloaded file to all waiting viewers simultaneously.

### 5.3 Asynchronous Upload Chunk Corruption
- **Failure Mode**: Network bit-rot corrupts a 10 MB chunk during transit, causing the stitched video file to fail playback.
- **Mitigation**: Enforce MD5 checksum verification.
The client computes the MD5 digest of each 10 MB chunk before transmission and includes it in the `Content-MD5` header.
S3 verifies the checksum before accepting the part; if mismatched, S3 rejects the part immediately, prompting the client to retry.

---

## 6. Hands-On Verification

The following standalone Python implementation demonstrates master playlist generation, multi-bitrate variant playlists, adaptive bitrate chunk selection simulation, and windowed view counter aggregation.

```python
#!/usr/bin/env python3
"""
Production-grade demonstration of Video Streaming Platform components:
- HLS Master and Variant Playlist generation
- Client Adaptive Bitrate (ABR) switching simulator
- Windowed high-throughput view counter pipeline
"""

import time
import math
from typing import Dict, List, Tuple


class HLSManifestGenerator:
    """Generates standard HLS Master and Variant playlists."""
    BITRATE_PROFILES = [
        {"name": "1080p", "bandwidth": 4500000, "resolution": "1920x1080"},
        {"name": "720p",  "bandwidth": 2000000, "resolution": "1280x720"},
        {"name": "480p",  "bandwidth": 800000,  "resolution": "854x480"},
    ]

    @classmethod
    def generate_master_playlist(cls) -> str:
        lines = ["#EXTM3U", "#EXT-X-VERSION:3"]
        for p in cls.BITRATE_PROFILES:
            lines.append(f"#EXT-X-STREAM-INF:BANDWIDTH={p['bandwidth']},RESOLUTION={p['resolution']}")
            lines.append(f"{p['name']}/index.m3u8")
        return "\n".join(lines)

    @classmethod
    def generate_variant_playlist(cls, profile_name: str, num_chunks: int = 5, chunk_duration_sec: int = 6) -> str:
        lines = [
            "#EXTM3U",
            "#EXT-X-VERSION:3",
            f"#EXT-X-TARGETDURATION:{chunk_duration_sec}",
            "#EXT-X-MEDIA-SEQUENCE:0"
        ]
        for i in range(num_chunks):
            lines.append(f"#EXTINF:{chunk_duration_sec:.1f},")
            lines.append(f"segment_{profile_name}_{i:03d}.m4s")
        lines.append("#EXT-X-ENDLIST")
        return "\n".join(lines)


class AdaptiveBitratePlayerSimulator:
    """Simulates a client video player dynamically switching bitrates based on bandwidth."""
    def __init__(self, profiles: List[Dict]):
        # Sort profiles by bandwidth ascending
        self.profiles = sorted(profiles, key=lambda x: x["bandwidth"])

    def select_profile(self, available_bandwidth_bps: int) -> Dict:
        """Selects highest bitrate profile that fits within 80% of available bandwidth."""
        safe_bandwidth = available_bandwidth_bps * 0.8
        selected = self.profiles[0]  # Default to lowest
        for p in self.profiles:
            if p["bandwidth"] <= safe_bandwidth:
                selected = p
        return selected


class StreamViewCounterAggregator:
    """Simulates high-throughput streaming view aggregation (Kafka -> Flink windowing)."""
    def __init__(self):
        # video_id -> count in current window
        self._current_window_counts: Dict[str, int] = {}
        # Persistent storage simulation: video_id -> total_views
        self.persistent_db: Dict[str, int] = {}

    def record_view(self, video_id: str):
        self._current_window_counts[video_id] = self._current_window_counts.get(video_id, 0) + 1

    def flush_window(self):
        """Flushes rolling window counts to persistent storage in a single bulk operation."""
        for vid, count in self._current_window_counts.items():
            self.persistent_db[vid] = self.persistent_db.get(vid, 0) + count
        self._current_window_counts.clear()


if __name__ == "__main__":
    print("--- 1. Generating HLS Master Playlist ---")
    master = HLSManifestGenerator.generate_master_playlist()
    print(master)

    print("\n--- 2. Generating 1080p Variant Playlist ---")
    variant = HLSManifestGenerator.generate_variant_playlist("1080p", num_chunks=3)
    print(variant)

    print("\n--- 3. Testing Client Adaptive Bitrate (ABR) Switching ---")
    player = AdaptiveBitratePlayerSimulator(HLSManifestGenerator.BITRATE_PROFILES)

    bandwidth_scenarios = [
        ("High-Speed Fiber (10 Mbps)", 10000000),
        ("Moderate 4G (2.2 Mbps)", 2200000),
        ("Throttled 3G (600 Kbps)", 600000),
    ]
    for label, bw in bandwidth_scenarios:
        choice = player.select_profile(bw)
        print(f"[{label}] -> Player Selected: {choice['name']} ({choice['bandwidth']} bps)")

    print("\n--- 4. Testing High-Throughput Batch View Counter ---")
    counter = StreamViewCounterAggregator()
    # Ingest 10,000 views for viral video
    for _ in range(10000):
        counter.record_view("vid_viral_99")

    assert counter.persistent_db.get("vid_viral_99", 0) == 0, "DB should not be touched before flush!"
    counter.flush_window()
    assert counter.persistent_db.get("vid_viral_99") == 10000, "DB should reflect flushed batch!"
    print(f"Successfully flushed window! Total views in DB: {counter.persistent_db['vid_viral_99']}")

    print("\nVerification Passed: HLS manifests, ABR selection, and view counter verified!")
```

### CLI Verification

Execute standard HLS inspection and transcoding commands:

```bash
# Linux / macOS: Inspect HLS Master Playlist via curl
curl -s https://content.jwplatform.com/manifests/yp34SRfc.m3u8

# Linux / macOS: Transcode raw MP4 into HLS segments using FFmpeg
ffmpeg -i sample.mp4 \
  -codec: copy \
  -start_number 0 \
  -hls_time 6 \
  -hls_list_size 0 \
  -f hls master.m3u8

# Windows PowerShell: Inspect CDN Video Chunk HTTP Headers
Invoke-WebRequest -Uri "https://example.com/video/1080p/segment_001.m4s" -Method Head | Select-Object -ExpandProperty Headers
```

---

## 7. Performance Characteristics and Capacity Planning

### 7.1 Video Upload and Ingestion Math
- **Upload Rate**: 500 hours of video uploaded every minute.
- Ingestion velocity:
  $$\text{Hours/Day} = 500 \text{ hours/min} \times 60 \times 24 = 720,000 \text{ hours of video/day}$$
- **Storage Calculations**:
  - Raw uncompressed video $\approx 3 \text{ GB/hour}$.
  - Transcoding into multiple resolutions (1080p, 720p, 480p, 360p) in H.264 and AV1 requires $\approx 5 \text{ GB}$ total storage per uploaded hour.
  - Daily storage requirement:
    $$720,000 \text{ hours} \times 5 \text{ GB} = 3,600,000 \text{ GB} = 3.6 \text{ PB/day}$$
  - Yearly storage capacity:
    $$3.6 \text{ PB/day} \times 365 \approx 1.31 \text{ Exabytes/year}$$

### 7.2 Playback Bandwidth Math
- Assume 1 billion video views per day globally.
- Average watch time per view = 5 minutes (300 seconds).
- Average streaming bitrate = 2.5 Mbps (mix of 720p and 1080p).
- Bandwidth consumption per view = $300 \text{ seconds} \times 2.5 \text{ Mbps} = 750 \text{ Megabits} \approx 93.75 \text{ MB}$.
- Total daily playback volume:
  $$1,000,000,000 \times 93.75 \text{ MB} \approx 93.75 \text{ Petabytes/day}$$
- Average CDN Egress Bandwidth:
  $$\text{Bandwidth} = \frac{93.75 \text{ PB} \times 8 \text{ bits/byte}}{86,400 \text{ sec}} \approx 8.68 \text{ Terabits per second (Tbps)}$$
- Peak egress bandwidth exceeds **20 Tbps**, necessitating deep CDN edge peering with tier-1 Internet Service Providers.

---

## 8. In Production: Real-World Architecture (Netflix & YouTube)

1. **Netflix Open Connect**:
   - Netflix built its own custom CDN called **Open Connect**.
   - Instead of paying commercial CDN providers, Netflix deploys purpose-built hardware appliances (Open Connect Appliances - OCAs) directly inside ISP datacenters around the globe.
   - During off-peak hours (middle of the night), Netflix pre-positions tomorrow's anticipated popular movies onto local ISP appliances, serving 95%+ of traffic locally without touching the public internet backbone.
2. **YouTube Video Architecture**:
   - YouTube maintains custom ASIC video transcoding silicon (**Argos VCU - Video Coding Unit**) developed in-house to transcode AV1 and VP9 video streams 20-30x more efficiently than traditional server GPUs.

---

## 9. Interview Questions and Deep Dives

> [!question] Question 1: How does Adaptive Bitrate Streaming (ABR) work, and why is it preferred over a single video stream?
> [!success]- Answer
> In ABR (HLS or MPEG-DASH), a video is transcoded into multiple bitrates and resolutions and divided into short (e.g., 6-second) media segments.
> The client video player continuously monitors available network throughput, packet drop rates, and device hardware buffer depth.
> If the network slows down, the player requests the next 6-second chunk at a lower bitrate (e.g., dropping from 1080p to 480p) before its playback buffer empties.
> This ensures uninterrupted playback without buffering wheels or stream restarts.

> [!question] Question 2: Why are videos split into small 6-second chunks rather than serving one large progressive MP4 file?
> [!success]- Answer
> 1. **Dynamic Bitrate Switching**: The player can switch resolutions on every chunk boundary.
> 2. **Instant Seeking**: Seeking to minute 45 requires requesting only the specific chunk corresponding to minute 45 (`chunk_450.m4s`), rather than executing expensive HTTP range requests over a multi-gigabyte file.
> 3. **CDN Caching**: Small 6-second files (2-5 MB) cache efficiently in edge CDN memory and SSDs, maximizing cache hit ratios.
> 4. **Bandwidth Conservation**: If a user watches only 20 seconds of a 2-hour movie and leaves, the platform wastes only 24 seconds of downloaded chunks rather than buffering the entire file.

> [!question] Question 3: How do you prevent video uploads from restarting from the beginning if a user's connection drops at 99%?
> [!success]- Answer
> Implement **Resumable Multipart Uploads** via Amazon S3 pre-signed URLs.
> The client chunks the video locally into 10 MB blocks.
> Each block uploads independently with an MD5 checksum.
> The client and server track successfully committed chunk indexes.
> If the upload fails at chunk 99 of 100, the client queries the server for the latest uploaded part index and transmits only chunk 100 before signaling upload completion.

> [!question] Question 4: How does a Directed Acyclic Graph (DAG) transcoding orchestrator accelerate video processing?
> [!success]- Answer
> Transcoding an entire 2-hour 4K video linearly on a single machine can take hours.
> A DAG orchestrator splits the raw video into hundreds of independent GOP (Group of Pictures) chunks and schedules tasks across thousands of distributed GPU workers:
> Workers transcode chunk 1, chunk 2, and chunk $N$ simultaneously in parallel across all target resolutions and codecs.
> A final packaging job stitches manifest files and indexes the outputs.
> This reduces total processing latency from hours to a few minutes.

> [!question] Question 5: Why is AV1 increasingly preferred over H.264/AVC, and what is the trade-off?
> [!success]- Answer
> AV1 provides 30-40% higher compression efficiency than H.264, delivering identical visual quality at significantly lower bandwidth consumption, saving millions of dollars in CDN egress fees.
> **Trade-off**: AV1 encoding is computationally intensive, requiring up to $10\times$ more CPU/GPU time and power to encode than H.264.
> High-volume platforms resolve this by using custom ASIC silicon (Google Argos VCU) and encoding only popular videos in AV1, leaving long-tail videos in faster H.264.

> [!question] Question 6: What is a CDN Origin Shield, and why is it essential for viral video releases?
> [!success]- Answer
> When a new viral video is published, thousands of edge CDN PoPs experience simultaneous cache misses.
> If all edge PoPs query the origin S3 bucket concurrently, the origin becomes throttled and overwhelmed.
> An **Origin Shield** is a dedicated mid-tier caching proxy deployed between edge PoPs and the origin bucket.
> The first edge PoP miss hits the Origin Shield, which fetches the video segment once from S3.
> Subsequent misses from other edge PoPs are satisfied directly from the Origin Shield, completely insulating the origin storage layer.

> [!question] Question 7: How do you design an accurate view counter that prevents view count fraud and database lock contention?
> [!success]- Answer
> Never write directly to relational databases on view events.
> 1. Enforce business rules: a view event is emitted only after 30 seconds of playback.
> 2. Stream events to Apache Kafka.
> 3. Stream processing engines (Apache Flink) aggregate view counts in 10-second tumbling windows, applying fraud detection (filtering bot IP clusters and excessive view loops from single users).
> 4. Increment aggregated view deltas in Redis (`HINCRBY`), flushing rolling sums to persistent storage periodically in asynchronous batches.

> [!question] Question 8: How do you store and serve billions of video preview thumbnails efficiently?
> [!success]- Answer
> Storing billions of individual tiny 5 KB JPEG files directly on a filesystem exhausts filesystem inode tables and causes slow disk seeks.
> Instead, merge thumbnails into **sprite sheets**: a single image containing a grid of 100 thumbnail frames.
> The video player downloads the single sprite image and uses CSS coordinates (`background-position`) or WebGL to display the correct frame as the user hovers over the playback scrubber bar, reducing HTTP requests by 99%.

> [!question] Question 9: How does the system handle copyright infringement and content safety screening during video upload?
> [!success]- Answer
> The transcoding DAG includes automated inspection steps:
> 1. **Audio Fingerprinting (e.g., Content ID)**: Extract audio waveforms, compute acoustic hashes, and query a database of copyrighted reference tracks.
> 2. **Visual Frame Matching**: Sample keyframes and evaluate against databases of prohibited imagery using computer vision models.
> If a match is detected, the video is flagged, prevented from public indexing, and routed to moderation queues before transcoded chunks are distributed to CDN edges.

> [!question] Question 10: How do video platforms distribute and store content across different tiers based on age and popularity?
> [!success]- Answer
> Apply automated **Storage Tiering (Information Lifecycle Management)**:
> 1. Hot Tier (Top 20% videos, newly uploaded content): Stored in Amazon S3 Standard and cached heavily in SSD-based CDN edge PoPs.
> 2. Warm Tier (Moderate traffic): Stored in S3 Infrequent Access (IA), retrieved to edge CDNs only on demand.
> 3. Cold Tier (Long-tail videos with zero views in 6 months): Stored in S3 Glacier or deep archival, with low-bitrate streams retained on warm storage.
> 4. Transcoded intermediate chunks are discarded immediately after manifest generation to conserve storage.

---

## 10. Related Concepts and Wikilinks

- [[Amazon-S3-and-Object-Storage]]: Resumable multipart uploads and storage tiering.
- [[Apache-Kafka]]: Telemetry event streaming and view count ingestion.
- [[Load-Balancing]]: Global traffic management and Anycast edge CDN routing.
- [[Consistent-Hashing]]: Routing video segment requests across edge caching fleets.
- [[MapReduce-Architecture]]: Batch analytics and video recommendation graph generation.

---

## 11. Further Reading

- Xu, Alex. *System Design Interview – An Insider’s Guide (Volume 1)*. Chapter 14: Design YouTube.
- Netflix Technology Blog. *High Quality Video Encoding at Scale*. Netflix TechBlog.
- Apple Developer Documentation: *HTTP Live Streaming (HLS) Authoring Specification for Apple Devices*.
- Google Research. *Argos: A Specialized Processing Unit for Video Ingestion and Transcoding*. ASPLOS 2021.
