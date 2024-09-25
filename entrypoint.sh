#!/bin/bash
set -ex

# Default values for environment variables if not set
export SEED_HOSTNAME=${SEED_HOSTNAME}
export NODE_HOSTNAME=${NODE_HOSTNAME}
export EMAIL=${EMAIL}
export THREADS=${THREADS}
export ADDRESS=${ADDRESS:-"0.0.0.0"}
export PORT=${PORT:-"53"}
export NETWORK=${NETWORK:-"mainnet"}  # Default to "mainnet" unless otherwise set

# Extra arguments for testnet
extra_args=""
if [[ "$NETWORK" == "testnet" ]]; then
  export extra_args="--testnet"
fi

# If the first argument starts with a hyphen (-), consider it an argument for the dnsseed binary
if [[ "${1:0:1}" == "-" ]]; then
  exec /home/dnsseed/dnsseed "$@"
fi

# Run the dnsseed binary with default arguments and extra_args, or run another command if provided
if [ "$1" == "/home/dnsseed/dnsseed" ]; then
  exec /home/dnsseed/dnsseed -h "$SEED_HOSTNAME" \
    -n "$NODE_HOSTNAME" \
    -a "$ADDRESS" \
    -p "$PORT" \
    -m "$EMAIL" \
    -t "$THREADS" "$extra_args"
else
  # If another command is passed, run that instead
  exec "$@"
fi
