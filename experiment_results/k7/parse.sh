#!/usr/bin/bash
sed -i '/^\s*$/d' $1
sed -i 's/Downlink\ Cluster\ //g' $1
sed -i 's/Uplink\ Cluster\ //g' $1
sed -i '/RelayID/ {N; d}' $1
sed -i '/ClientID/d' $1
sed -i "s/^/`head --lines=1 $1`\t/" $1
sed -i '1d' $1
