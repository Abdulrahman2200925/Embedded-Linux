#!/usr/bin/env python3
"""
TelemetrySystem Dashboard Server
Reads output/app.log, parses log messages, serves live data via SSE
Controls app via SIGHUP for config reload

═══════════════════════════════════════════════════════════════
BUG FIXES in this version
═══════════════════════════════════════════════════════════════

FIX 1 — Socket data appears in GPU/SomeIP chart instead of Socket chart
  ROOT CAUSE:
    SocketTelemetrySourceImpl and SomeIPTelemetrySourceImpl both use
    GpuPolicy → both produce log lines tagged [GPU].
    Dashboard sees [GPU] → routes everything to the GPU chart.

  SOLUTION (zero C++ changes):
    demo_socket.sh writes each value to /tmp/dashboard_socket_hint.txt
    just before piping it into the socket.
    dashboard.py reads this hint file when a [GPU] entry arrives.
    If the log value matches the hint (±0.6) and the hint is fresh
    (< 5 seconds old) → route to 'socket' chart.
    Otherwise → route to 'gpu' chart.

    Required change to demo_socket.sh — add one line per value:
      echo "$value" > /tmp/dashboard_socket_hint.txt
      echo "$value"
    (both lines inside the pipe block)

FIX 2 — Socket source blocks app when enabled
  ROOT CAUSE:
    SocketTelemetrySourceImpl::openSource() waits for a client to
    connect to /tmp/telemetry.sock before returning. When socket is
    toggled on in the dashboard and SIGHUP fires, the app rebuilds
    and the socket reader thread blocks until demo_socket.sh connects.

  SOLUTION (zero C++ changes):
    This is expected behavior — the socket source is a server waiting
    for a producer. Run demo_socket.sh AFTER enabling socket in the
    dashboard. The dashboard now adds a 'chart' field to each SSE
    event so the frontend can route correctly once data flows.
"""

import os
import json
import signal
import time
import re
from pathlib import Path
from flask import Flask, Response, jsonify, request, send_from_directory

app = Flask(__name__, static_folder='dashboard')

# ── Paths ──────────────────────────────────────────────────────
BASE_DIR         = Path(__file__).parent
LOG_FILE         = BASE_DIR / "output" / "app.log"
CONFIG_FILE      = BASE_DIR / "config" / "app_config.json"
PID_FILE         = BASE_DIR / "app.pid"
SOCKET_HINT_FILE = Path("/tmp/dashboard_socket_hint.txt")


# ── Socket hint resolver ───────────────────────────────────────
def _is_socket_value(value: float) -> bool:
    """
    Returns True if this value was recently sent via the socket source.
    demo_socket.sh writes the value to SOCKET_HINT_FILE before piping
    it to nc. We match on value (±0.6) and file freshness (< 5s).
    """
    try:
        if not SOCKET_HINT_FILE.exists():
            return False
        age = time.time() - SOCKET_HINT_FILE.stat().st_mtime
        if age > 5.0:
            return False
        hint_val = float(SOCKET_HINT_FILE.read_text().strip())
        return abs(hint_val - value) < 0.6
    except Exception:
        return False


# ── Log Parser ─────────────────────────────────────────────────
def parse_log_line(line):
    """
    Parse: [GPU],[2026-02-23 00:53:25],[Telemetry],[INFO],[GPU usage at 42.39%]
    Returns dict with 'chart' field for frontend routing, or None.

    chart values:
      'cpu'    → CPU Usage chart
      'ram'    → RAM Usage chart
      'gpu'    → GPU / SomeIP Temperature chart
      'socket' → Socket Input chart
    """
    line = line.strip()
    if not line or not line.startswith('['):
        return None

    try:
        parts = line.strip('[]').split('],[')
        if len(parts) != 5:
            return None

        source    = parts[0]
        timestamp = parts[1]
        context   = parts[2]
        severity  = parts[3]
        message   = parts[4]

        match = re.search(r'(\d+\.?\d*)', message)
        value = float(match.group(1)) if match else 0.0

        # Default: route by source name
        chart = source.lower()   # 'cpu', 'ram', 'gpu'

        # FIX 1: Both SomeIP and Socket tag as [GPU].
        # Use hint file to correctly route socket values.
        if source == 'GPU' and _is_socket_value(value):
            chart = 'socket'

        return {
            'source':    source,
            'timestamp': timestamp,
            'context':   context,
            'severity':  severity,
            'message':   message,
            'value':     value,
            'chart':     chart,
        }
    except Exception:
        return None


def read_recent_logs(n=200):
    if not LOG_FILE.exists():
        return []
    try:
        with open(LOG_FILE, 'r') as f:
            lines = f.readlines()
        parsed = []
        for line in lines[-n:]:
            entry = parse_log_line(line)
            if entry:
                parsed.append(entry)
        return parsed
    except Exception:
        return []


# ── Routes ─────────────────────────────────────────────────────

@app.route('/')
def index():
    return send_from_directory('dashboard', 'index.html')


@app.route('/api/logs')
def get_logs():
    return jsonify(read_recent_logs(200))


@app.route('/api/config', methods=['GET'])
def get_config():
    try:
        with open(CONFIG_FILE, 'r') as f:
            return jsonify(json.load(f))
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/config', methods=['POST'])
def set_config():
    """Update config and send SIGHUP to C++ app for live reload."""
    try:
        new_config = request.get_json()
        with open(CONFIG_FILE, 'w') as f:
            json.dump(new_config, f, indent=4)

        if PID_FILE.exists():
            with open(PID_FILE, 'r') as f:
                pid = int(f.read().strip())
            os.kill(pid, signal.SIGHUP)
            return jsonify({'status': 'reloaded', 'pid': pid})
        else:
            return jsonify({'status': 'config_saved', 'warning': 'app not running'})

    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/status')
def get_status():
    running = PID_FILE.exists()
    pid = None
    if running:
        try:
            with open(PID_FILE, 'r') as f:
                pid = int(f.read().strip())
            os.kill(pid, 0)
        except (ProcessLookupError, ValueError):
            running = False
            pid = None
    return jsonify({'running': running, 'pid': pid})


@app.route('/api/stream')
def stream():
    """
    Server-Sent Events — pushes new log lines to browser in real time.
    Each event includes 'chart' so frontend routes to the correct chart.
    """
    def generate():
        last_size = 0
        while True:
            try:
                if LOG_FILE.exists():
                    current_size = LOG_FILE.stat().st_size
                    if current_size > last_size:
                        with open(LOG_FILE, 'r') as f:
                            f.seek(last_size)
                            new_lines = f.readlines()
                        last_size = current_size
                        for line in new_lines:
                            entry = parse_log_line(line)
                            if entry:
                                yield f"data: {json.dumps(entry)}\n\n"
                    elif current_size < last_size:
                        last_size = 0   # app restarted, file truncated
            except Exception:
                pass
            time.sleep(0.3)

    return Response(generate(), mimetype='text/event-stream',
                    headers={'Cache-Control': 'no-cache',
                             'X-Accel-Buffering': 'no'})


if __name__ == '__main__':
    print("=" * 50)
    print("  TelemetrySystem Dashboard")
    print("  http://localhost:5000")
    print("=" * 50)
    app.run(debug=False, threaded=True, port=5000)
