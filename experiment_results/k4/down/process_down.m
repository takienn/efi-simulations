%% loading and merging measurements
names = dir('efi/exper*');
for k =1:length(names)
    temp=load(['efi/' names(k).name]);
    efi_down{k}=temp;
end

names = dir('normal/exper*');

%% Filtering out clients connected directly to AP
for k =1:length(names)
    ap_clients=efi_down{k}(efi_down{k}(:,1)==0,:);
    
    temp=load(['normal/' names(k).name]);
    for i=1:size(efi_down{k},1)
        temp(find(temp(:,3)==efi_down{k}(i,1)),:)=[];
    end
    for i=1:size(ap_clients,1)
        temp(find(temp(:,3)==ap_clients(i,3)),:)=[];
    end
    temp(:,1)=[];
    temp(:,1)=[];
    normal_down{k}=temp;
end

%% Removing measurements for clients connect directly to AP
for i = 1:length(efi_down)
    efi_down{i}(efi_down{i}(:,1)==0,:)=[];
end

%% Ensuring measurements consistency between EFI and WIFI simulations
k=1;
for i = 1:length(efi_down)
    e =size(efi_down{i},1);
    n =size(normal_down{i},1);
    if(e==n)
        down{k}=[efi_down{i} normal_down{i}];
        k=k+1;
    end
end

%% Appending throughput change to the measurements
all_throughput=[];
for i = 1:length(down)
    down{i} = [down{i} down{i}(:,5)-down{i}(:,8)];
    all_throughput=[all_throughput;(down{i}(:,5)-down{i}(:,8))./down{i}(:,8)];
end

clear throughput_change density_th_change density_th_efi density_th_normal;
%% Calculate average throughput per cluster
for i = 1:length(down)
    throughput_change{i}=[down{i}(:,1) down{i}(:,2) down{i}(:,9)];
    throughput{i} = [down{i}(:,1) down{i}(:,2) down{i}(:,5) down{i}(:,8) down{i}(:,4) down{i}(:,7)];
    [u,~,idx]=unique(throughput_change{i}(:,1));
    density_th_change{i}=[u,accumarray(idx,throughput_change{i}(:,3),[],@mean)];
%     density_th_change{i}=[density_th_change{i},accumarray(idx,throughput_change{i}(:,3),[],@var)];
    
    [u,~,idx]=unique(throughput{i}(:,1));
    density_th_efi{i}=[u,accumarray(idx,throughput{i}(:,3),[],@mean)];
%     density_th_efi{i}=[density_th_efi{i},accumarray(idx,throughput{i}(:,3),[],@var)];
    
    density_th_normal{i}=[u,accumarray(idx,throughput{i}(:,4),[],@mean)];
%     density_th_normal{i}=[density_th_normal{i},accumarray(idx,throughput{i}(:,4),[],@var)];
% 
%     density_psr_efi{i}=[u,accumarray(idx,throughput{i}(:,5),[],@mean)];
%     density_psr_normal{i}=[u,accumarray(idx,throughput{i}(:,6),[],@mean)];

end

%% Convert cluster IDs to K
for i=1:length(density_th_change)
    tmp=density_th_change{i};
    for j=1:length(tmp(:,1))
        tmp(j,1)=length(find(throughput_change{i}(:,1)==tmp(j,1)));
        tmp(j,2)=tmp(j,2)/tmp(j,1);
    end
    density_th_change{i}=tmp;
    
    tmp=density_th_efi{i};
    for j=1:length(tmp(:,1))
        tmp(j,1)=length(find(throughput{i}(:,1)==tmp(j,1)));
        tmp(j,2)=tmp(j,2)/tmp(j,1);
    end
    density_th_efi{i}=tmp;
    
    tmp=density_th_normal{i};
    for j=1:length(tmp(:,1))
        tmp(j,1)=length(find(throughput{i}(:,1)==tmp(j,1)));
        tmp(j,2)=tmp(j,2)/tmp(j,1);
    end
    density_th_normal{i}=tmp;
    
%     tmp=density_psr_efi{i};
%     for j=1:length(tmp(:,1))
%         tmp(j,1)=length(find(throughput{i}(:,1)==tmp(j,1)));
%         tmp(j,2)=tmp(j,2)/tmp(j,1);
%     end
%     density_psr_efi{i}=tmp;
%     
%     tmp=density_psr_normal{i};
%     for j=1:length(tmp(:,1))
%         tmp(j,1)=length(find(throughput{i}(:,1)==tmp(j,1)));
%         tmp(j,2)=tmp(j,2)/tmp(j,1);
%     end
%     density_psr_normal{i}=tmp;
end

%% Average all equal Ks
clear table tmp;
table =[];
for i = 1:length(density_th_change)
    [u,~,idx]=unique(density_th_change{i}(:,1));
    tmp=[u,accumarray(idx,density_th_change{i}(:,2),[],@mean)];
    tmp=[tmp,accumarray(idx,density_th_change{i}(:,2),[],@var)]; %% variance
    density_th_change{i}=tmp;
    table = [table;tmp];
end
clear density_th_change;
for i = 1:length(table)
   [u,~,idx]=unique(table(:,1));
   density_th_change=[u,accumarray(idx,table(:,2),[],@mean)];
   density_th_change=[density_th_change,accumarray(idx,table(:,3),[],@mean)]; %% Average of variances
end
density_th_change(:,3) = sqrt(density_th_change(:,3)) / length(density_th_change(:,3)); %% Standard error

clear table tmp;
table =[];
for i = 1:length(density_th_efi)
    [u,~,idx]=unique(density_th_efi{i}(:,1));
    tmp=[u,accumarray(idx,density_th_efi{i}(:,2),[],@mean)];
    tmp=[tmp,accumarray(idx,density_th_efi{i}(:,2),[],@var)]; %% variance
    density_th_efi{i}=tmp;
    table = [table;tmp];
end
clear density_th_efi;
for i = 1:length(table)
   [u,~,idx]=unique(table(:,1));
   density_th_efi=[u,accumarray(idx,table(:,2),[],@mean)];
   density_th_efi=[density_th_efi,accumarray(idx,table(:,3),[],@mean)]; %% Average of variances
end
density_th_efi(:,3) = sqrt(density_th_efi(:,3)) / length(density_th_efi(:,3)); %% Standard error

clear table tmp;
table =[];
for i = 1:length(density_th_normal)
    [u,~,idx]=unique(density_th_normal{i}(:,1));
    tmp=[u,accumarray(idx,density_th_normal{i}(:,2),[],@mean)];
    tmp=[tmp,accumarray(idx,density_th_normal{i}(:,2),[],@var)]; %% variance
    density_th_normal{i}=tmp;
    table = [table;tmp];
end
clear density_th_normal;
for i = 1:length(table)
   [u,~,idx]=unique(table(:,1));
   density_th_normal=[u,accumarray(idx,table(:,2),[],@mean)];
   density_th_normal=[density_th_normal,accumarray(idx,table(:,3),[],@mean)]; %% Average of variances
end
density_th_normal(:,3) = sqrt(density_th_normal(:,3)) / length(density_th_normal(:,3)); %% Standard error

% table1 =[];
% for i = 1:length(density_psr_efi)
%     tmp=density_psr_efi{i};
%     [u,~,idx]=unique(tmp(:,1));
%     tmp=[u,accumarray(idx,tmp(:,2),[],@mean)];
%     density_psr_efi{i}=tmp;
%     table1 = [table1;tmp];
% end
% clear density_psr_efi;
% for i = 1:length(table1)
%    [u,~,idx]=unique(table1(:,1));
%    density_psr_efi=[u,accumarray(idx,table1(:,2),[],@mean)];
% end
% 
% table1 =[];
% for i = 1:length(density_psr_normal)
%     tmp=density_psr_normal{i};
%     [u,~,idx]=unique(tmp(:,1));
%     tmp=[u,accumarray(idx,tmp(:,2),[],@mean)];
%     density_psr_normal{i}=tmp;
%     table1 = [table1;tmp];
% end
% clear density_psr_normal;
% for i = 1:length(table1)
%    [u,~,idx]=unique(table1(:,1));
%    density_psr_normal=[u,accumarray(idx,table1(:,2),[],@mean)];
% end
%% Plot results

fig_density_th_change = figure;
bar(density_th_change(:,1),density_th_change(:,2));
hold on;
errorbar(density_th_change(:,1),density_th_change(:,2), density_th_change(:,3), 's');
xlabel('Number of WD Clients');
set(gca,'XTickLabel',{'5%','10%','15%','20%'})
ylabel('Throughput change (%)');
title('Change in throughput from WiFi to EFi vs Clients per Cluster');

saveas(fig_density_th_change, 'throughput_change_density.fig', 'fig');
saveas(fig_density_th_change, 'throughput_change_density.png', 'png');

% fig_density_psr_all = figure;
% hold on;
% plot(density_psr_efi(:,1),density_psr_efi(:,2), 'DisplayName', 'EFI with 20 nodes');
% plot(density_psr_normal(:,1),density_psr_normal(:,2), 'DisplayName', 'WIFI with 20 nodes');
% xlabel('K');
% ylabel('PSR');
% title('EFI and WIFI average PSR per K');
% legend show;
% saveas(fig_density_psr_all, 'psr_all_density.fig', 'fig');
% saveas(fig_density_psr_all, 'psr_all_density.png', 'png');

fig_density_th_all = figure;
density_th_all = [density_th_normal(:,2), density_th_efi(:,2)];
density_th_all_err = [density_th_normal(:,3), density_th_efi(:,3)];
b = barwitherr(density_th_all_err, density_th_all);
xlabel('Number of WD Clients');
set(gca,'XTickLabel',{'5%','10%','15%','20%'})
legend('WIFI', 'EFI');
ylabel('Throughput (MBytes/s)');
title('EFI and WIFI average Throughput per K');
xticks([1 2 3 4 5 6 7]);
legend('show');

saveas(fig_density_th_all, 'throughput_all_density.fig', 'fig');
saveas(fig_density_th_all, 'throughput_all_density.png', 'png');

% [f,cdf] = ecdf(all_throughput);
% ecdf_th_change_all = figure;
% plot(cdf, f);
% title('ECDF of throughput change in EFI from WIFI network-wide');
% xlabel('throughput change (%)');
% ylabel('Probability');
% 
% saveas(ecdf_th_change_all, 'ecdf_th_change_all.png', 'png');
% saveas(ecdf_th_change_all, 'ecdf_th_change_all.fig', 'fig');


%% Process Group Owners Measurements
% load efi/relay_measurements.txt;
% relay_measurements(:,3)=[]; %% Irrelevant data
% relay_measurements(:,3)=[]; %% Irrelevant data
% 
% [f,cdf] = ecdf(relay_measurements(:,2));
% avg_qw_fig=figure;
% plot(cdf,f);
% title('ECDF of GO Avg Queue Wait time');
% xlabel('Avg Queue Wait time (s)');
% ylabel('Probability');
% 
% [f,cdf] = ecdf(relay_measurements(:,3));
% avg_th_fig=figure;
% plot(cdf,f);
% title('ECDF of GO Downlink throughput');
% xlabel('Downlink throughput (MB/s)');
% ylabel('Probability');
% 
% saveas(avg_th_fig, 'ecdf_go_throughput.png', 'png');
% saveas(avg_th_fig, 'ecdf_go_throughput.fig', 'fig');
% 
% saveas(avg_qw_fig, 'ecdf_go_avg_queue_wait.png', 'png');
% saveas(avg_qw_fig, 'ecdf_go_avg_queue_wait.fig', 'fig');

