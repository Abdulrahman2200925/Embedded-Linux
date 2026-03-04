#!/bin/bash
# demo_socket.sh
# Sends telemetry values through the socket source.
#
# FIX: Writes each value to /tmp/dashboard_socket_hint.txt just before
# piping it to nc. dashboard.py reads this file to distinguish socket
# values from SomeIP values (both log as [GPU] in the C++ app).

SOCKET_PATH="/tmp/telemetry.sock"
HINT_FILE="/tmp/dashboard_socket_hint.txt"

echo "========================================"
echo "  TelemetrySystem Socket Demo"
echo "========================================"

{
    for value in 10.0 25.5 45.0 60.0 79.9; do
        echo "Sending INFO: $value%"  >&2
        echo "$value" > "$HINT_FILE"   # hint for dashboard routing
        echo "$value"
        sleep 2
    done

    for value in 80.1 85.0 88.5 92.0 94.9; do
        echo "Sending WARNING: $value%"  >&2
        echo "$value" > "$HINT_FILE"
        echo "$value"
        sleep 2
    done

    for value in 95.1 97.0 98.5 99.9; do
        echo "Sending CRITICAL: $value%"  >&2
        echo "$value" > "$HINT_FILE"
        echo "$value"
        sleep 2
    done

    for value in 60.0 40.0 20.0; do
        echo "Sending INFO (recovery): $value%"  >&2
        echo "$value" > "$HINT_FILE"
        echo "$value"
        sleep 2
    done

} | nc -U $SOCKET_PATH

# Clear hint file when done
rm -f "$HINT_FILE"

echo "Demo Complete!"
