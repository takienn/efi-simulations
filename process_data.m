load('throughput_down.mat');
total=[];
for i=1:length(clients_down_efi_stats(:,1))
    normal=clients_down_normal_stats(clients_down_normal_stats==clients_down_efi_stats(i,1))
    if ~isempty(normal)
        total=[total; normal];
    end
end