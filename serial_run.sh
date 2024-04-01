#out/objects/./serial_compressor
#out/objects/./serial_decompressor

#!/bin/bash

# Check if the number of arguments provided is not equal to 1
if [ "$#" -ne 1 ]; then
    # If not, run both commands
    echo "Running serial_compressor..."
    out/objects/serial_compressor

    echo "Running serial_decompressor..."
    out/objects/serial_decompressor

    exit 0
fi

# Assign the first argument to the variable 'command'
command="$1"

# Check if the command is 'compressor'
if [ "$command" = "compressor" ]; then
    # If it is, run serial_compressor
    echo "Running serial_compressor..."
    out/objects/serial_compressor
# Check if the command is 'decompressor'
elif [ "$command" = "decompressor" ]; then
    # If it is, run serial_decompressor
    echo "Running serial_decompressor..."
    out/objects/serial_decompressor
else
    # If the command is neither 'compressor' nor 'decompressor', display an error message
    echo "Error: Invalid command. Please specify 'compressor' or 'decompressor'."
    exit 1
fi
