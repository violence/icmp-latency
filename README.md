# icmp-latency

A minimal **CLI utility** for Windows that measures latency to a specified IP address using ICMP echo requests. 

---

## Usage

```text
ping.exe <IP> [-c <count>] [-i <interval_ms>] [-w <timeout_ms>]
```

### Arguments

- `<IP>` – Target IP address to ping. **Required**.

### Optional Flags

- `-c <count>` – Amount of ping requests to send.
  * **Default:** `1`
- `-i <interval_ms>` – Interval between pings in milliseconds.
  * **Default:** `1000`
- `-w <timeout_ms>` – Timeout for each ping in milliseconds.
  * **Default:** `5000`

---

## Examples

Ping a server once (default):

```bash
ping.exe 8.8.8.8
```

Ping a server 5 times, 2 seconds apart:

```bash
ping.exe 8.8.8.8 -c 5 -i 2000
```

Ping with a timeout of 1 second:

```bash
ping.exe 8.8.8.8 -w 1000
```
