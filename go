#!/bin/bash

echo "Making and uploading to device"

make

# make sure you change the address to the device's address
scp bin/apollo-pi apollo@apollo-pi.zapto.org:/home/apollo/apollo-pi
