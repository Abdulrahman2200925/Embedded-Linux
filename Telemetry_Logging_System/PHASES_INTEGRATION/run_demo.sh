#!/bin/bash
# =============================================================================
# run_demo.sh — OmniMetron Telemetry System Demo Launcher
#
# Launches everything in one command:
#   Terminal 1 → C++ telemetry app
#   Terminal 2 → Python dashboard server
#   Browser    → http://localhost:5000  (auto-opens after 3s)
#
# Usage:
#   ./run_demo.sh              # full demo (CPU + RAM + dashboard)
#   ./run_demo.sh --socket     # also opens terminal for demo_socket.sh
#   ./run_demo.sh --stop       # kill all demo processes
#
# RPi3 (gpu_someip): connect manually via SSH before running this script.
# =============================================================================

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_BIN="$PROJECT_DIR/build/app/app"
DASHBOARD_PY="$PROJECT_DIR/dashboard.py"
SOCKET_SCRIPT="$PROJECT_DIR/demo_socket.sh"
PID_FILE="$PROJECT_DIR/app.pid"
LOG_FILE="$PROJECT_DIR/output/app.log"

# ── Colors ────────────────────────────────────────────────────
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# ── Stop mode ─────────────────────────────────────────────────
if [[ "$1" == "--stop" ]]; then
    echo -e "${YELLOW}Stopping all demo processes...${NC}"

    # Kill C++ app via PID file
    if [[ -f "$PID_FILE" ]]; then
        PID=$(cat "$PID_FILE")
        kill "$PID" 2>/dev/null && echo -e "${GREEN}✓ C++ app stopped (PID $PID)${NC}"
        rm -f "$PID_FILE"
    fi

    # Kill dashboard
    pkill -f "python3 dashboard.py" 2>/dev/null && echo -e "${GREEN}✓ Dashboard stopped${NC}"

    # Kill socket demo
    pkill -f "demo_socket.sh" 2>/dev/null && echo -e "${GREEN}✓ Socket demo stopped${NC}"

    # Clean up temp files
    rm -f /tmp/dashboard_socket_hint.txt

    echo -e "${GREEN}All stopped.${NC}"
    exit 0
fi

# ── Checks ────────────────────────────────────────────────────
echo -e "${CYAN}"
echo "╔══════════════════════════════════════════╗"
echo "║     OmniMetron Telemetry Dashboard       ║"
echo "║     Demo Launcher                        ║"
echo "╚══════════════════════════════════════════╝"
echo -e "${NC}"

# Check app binary exists
if [[ ! -f "$APP_BIN" ]]; then
    echo -e "${RED}✗ App binary not found: $APP_BIN${NC}"
    echo "  Run: cmake --build build first"
    exit 1
fi

# Check dashboard.py exists
if [[ ! -f "$DASHBOARD_PY" ]]; then
    echo -e "${RED}✗ Dashboard not found: $DASHBOARD_PY${NC}"
    exit 1
fi

# Create output dir if needed
mkdir -p "$PROJECT_DIR/output"

# Clear old log so dashboard starts fresh
> "$LOG_FILE"

# Kill any previous instances
pkill -f "$(basename $APP_BIN)" 2>/dev/null
pkill -f "python3 dashboard.py"  2>/dev/null
sleep 1

# ── Launch C++ app ────────────────────────────────────────────
echo -e "${GREEN}[1/3] Launching C++ telemetry app...${NC}"
gnome-terminal \
    --title="OmniMetron — C++ App" \
    -- bash -c "
        cd '$PROJECT_DIR'
        echo '=== OmniMetron C++ Telemetry App ==='
        '$APP_BIN'
        echo '--- App exited. Press Enter to close. ---'
        read
    " &

sleep 2   # give the app time to start and write app.pid

# ── Launch dashboard ──────────────────────────────────────────
echo -e "${GREEN}[2/3] Launching Python dashboard...${NC}"
gnome-terminal \
    --title="OmniMetron — Dashboard" \
    -- bash -c "
        cd '$PROJECT_DIR'
        echo '=== OmniMetron Dashboard Server ==='
        source venv/bin/activate
        python3 dashboard.py
        echo '--- Dashboard exited. Press Enter to close. ---'
        read
    " &

sleep 2   # give Flask time to start on port 5000

# ── Launch socket demo terminal (optional) ────────────────────
if [[ "$1" == "--socket" ]]; then
    echo -e "${GREEN}[+] Launching socket demo terminal...${NC}"
    gnome-terminal \
        --title="OmniMetron — Socket Demo" \
        -- bash -c "
            cd '$PROJECT_DIR'
            echo '=== Socket Demo ==='
            echo 'Enable SOCKET in the dashboard first, then press Enter...'
            read
            chmod +x demo_socket.sh
            ./demo_socket.sh
            echo '--- Socket demo ended. Press Enter to close. ---'
            read
        " &
fi

# ── Open browser ──────────────────────────────────────────────
echo -e "${GREEN}[3/3] Opening browser in 3 seconds...${NC}"
sleep 3
xdg-open http://localhost:5000 2>/dev/null || \
    open http://localhost:5000 2>/dev/null || \
    echo -e "${YELLOW}Open manually: http://localhost:5000${NC}"

# ── Done ──────────────────────────────────────────────────────
echo ""
echo -e "${CYAN}══════════════════════════════════════════${NC}"
echo -e "${GREEN}  Demo running!${NC}"
echo -e "  Dashboard: ${CYAN}http://localhost:5000${NC}"
echo ""
echo -e "  To stop everything:  ${YELLOW}./run_demo.sh --stop${NC}"
if [[ "$1" != "--socket" ]]; then
echo -e "  To add socket demo:  ${YELLOW}./run_demo.sh --socket${NC}"
fi
echo -e "${CYAN}══════════════════════════════════════════${NC}"
