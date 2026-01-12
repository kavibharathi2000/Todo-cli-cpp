#!/bin/bash

# Variables
DB_FILE="$HOME/.todo/task.db"
SQL_FILE="setup.sql"
SRC_FILE="main.cpp"
BIN_NAME="todo"
INSTALL_DIR="/usr/local/bin"

echo "[*] Starting build and setup process..."

# Step 1: Create ~/.todo directory
mkdir -p "$(dirname "$DB_FILE")"

# Step 2: Remove old database if exists
if [ -f "$DB_FILE" ]; then
    echo "[-] Removing old database file: $DB_FILE"
    rm "$DB_FILE"
fi

# Step 3: Run setup.sql to initialize schema
if [ -f "$SQL_FILE" ]; then
    echo "[*] Creating new database: $DB_FILE"
    sqlite3 "$DB_FILE" < "$SQL_FILE"
    echo "[*] Database setup complete!"
else
    echo "[-] Setup SQL file not found: $SQL_FILE"
    exit 1
fi

# Step 4: Compile the C++ source
if [ -f "$SRC_FILE" ]; then
    echo "[*] Compiling $SRC_FILE..."
    g++ "$SRC_FILE" -o "$BIN_NAME" -lsqlite3
    echo "[*] Compilation complete!"
else
    echo "[-] Source file not found: $SRC_FILE"
    exit 1
fi

# Step 5: Move binary to /usr/local/bin
echo "[*] Installing binary to $INSTALL_DIR..."
sudo mv "$BIN_NAME" "$INSTALL_DIR/"
echo "[*] Installed as $BIN_NAME"

echo "[*] Setup finished. You can now run:"
echo "    $BIN_NAME list"
echo "    $BIN_NAME add \"taskname\" [status]"
echo "    $BIN_NAME update \"taskname\" [status]"
echo "    $BIN_NAME delete \"taskname\""
