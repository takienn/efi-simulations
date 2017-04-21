#!/usr/bin/bash
mkdir efi normal;
cd efi;
csplit ../efi_down_1s_$1.log "/--------- Running new experiment ---------/" {*};
rm xx00;
sed -i '/exper/d' *
sed -i '/^\s*$/d' *
sed -i '/Avg/ {N; d}' *
sed -i '/Rel/d' *
sed -i '/Dow/d' *
cd ../normal;
csplit ../normal_down_1s_$1.log "/--------- Running new experiment ---------/" {*};
rm xx00;
sed -i '/exper/d' *
sed -i '/^\s*$/d' *
sed -i '/Avg/ {N; d}' *
sed -i '/Rel/d' *
sed -i '/Dow/d' *
