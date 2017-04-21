%% loading and merging measurements
names = dir('efi/exper*');
for k =1:length(names)
    temp=load(['efi/' names(k).name]);
    efi_down{k}=temp;
end

names = dir('normal/exper*');

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

for i = 1:length(efi_down)
    efi_down{i}(efi_down{i}(:,1)==0,:)=[];
end

k=1;
for i = 1:length(efi_down)
    e =size(efi_down{i},1);
    n =size(normal_down{i},1);
    if(e==n)
        down{k}=[efi_down{i} normal_down{i}];
        k=k+1;
    end
end

%Appending throughput change to the measurements
for i = 1:length(down)
    down{i} = [down{i} down{i}(:,5)-down{i}(:,8)];
end

%Calculate average throughput per cluster
for i = 1:length(down)
    throughput_change{i}=[down{i}(:,1) down{i}(:,2) down{i}(:,9)];
    throughput{i} = [down{i}(:,1) down{i}(:,2) down{i}(:,5) down{i}(:,8)];
    [u,~,idx]=unique(throughput_change{i}(:,1));
    density_th_change{i}=[u,accumarray(idx,throughput_change{i}(:,3),[],@mean)];
    [u,~,idx]=unique(throughput_change{i}(:,2));
    res_th_change{i}=[u,accumarray(idx,throughput_change{i}(:,3),[],@mean)];
    [u,~,idx]=unique(throughput{i}(:,1));
    density_th_efi{i}=[u,accumarray(idx,throughput{i}(:,3),[],@mean)];
    density_th_normal{i}=[u,accumarray(idx,throughput{i}(:,4),[],@mean)];
    [u,~,idx]=unique(throughput{i}(:,2));
    res_th_efi{i}=[u,accumarray(idx,throughput{i}(:,3),[],@mean)];
    res_th_normal{i}=[u,accumarray(idx,throughput{i}(:,4),[],@mean)];
end

%% Convert cluster IDs to K
for i=1:length(density_th_change)
    tmp1=density_th_change{i};
    tmp2=res_th_change{i};
    for j=1:length(tmp1(:,1))
        tmp1(j,1)=length(find(throughput_change{i}(:,1)==tmp1(j,1)));
        tmp1(j,2)=tmp1(j,2)/tmp1(j,1);
    end
    for j=1:length(tmp2(:,1))
        c=length(find(throughput_change{i}(:,2)==tmp2(j,1)));
        tmp2(j,2)=tmp2(j,2)/c;
    end
    density_th_change{i}=tmp1;
    res_th_change{i}=tmp2;
    
    tmp1=density_th_efi{i};
    tmp2=res_th_efi{i};
    for j=1:length(tmp1(:,1))
        tmp1(j,1)=length(find(throughput{i}(:,1)==tmp1(j,1)));
        tmp1(j,2)=tmp1(j,2)/tmp1(j,1);
    end
    for j=1:length(tmp2(:,1))
        c=length(find(throughput{i}(:,2)==tmp2(j,1)));
        tmp2(j,2)=tmp2(j,2)/c;
    end
    density_th_efi{i}=tmp1;
    res_th_efi{i}=tmp2;
    
    tmp1=density_th_normal{i};
    tmp2=res_th_normal{i};
    for j=1:length(tmp1(:,1))
        tmp1(j,1)=length(find(throughput{i}(:,1)==tmp1(j,1)));
        tmp1(j,2)=tmp1(j,2)/tmp1(j,1);
    end
    for j=1:length(tmp2(:,1))
        c=length(find(throughput{i}(:,2)==tmp2(j,1)));
        tmp2(j,2)=tmp2(j,2)/c;
    end
    density_th_normal{i}=tmp1;
    res_th_normal{i}=tmp2;
end

%Average all equal Ks
table1 =[];
table2 =[];
for i = 1:length(density_th_change)
    tmp1=density_th_change{i};
    tmp2=res_th_change{i};
    [u,~,idx]=unique(tmp1(:,1));
    tmp1=[u,accumarray(idx,tmp1(:,2),[],@mean)];
    density_th_change{i}=tmp1;
    table1 = [table1;tmp1];
    
    [u,~,idx]=unique(tmp2(:,1));
    tmp2=[u,accumarray(idx,tmp2(:,2),[],@mean)];
    res_th_change{i}=tmp2;
    table2 = [table2;tmp2];
end
clear density_th_change;
clear res_th_change;
for i = 1:length(table1)
   [u,~,idx]=unique(table1(:,1));
   density_th_change=[u,accumarray(idx,table1(:,2),[],@mean)];
   [u,~,idx]=unique(table2(:,1));
   res_th_change=[u,accumarray(idx,table2(:,2),[],@mean)];
end


table1 =[];
table2 =[];
for i = 1:length(density_th_efi)
    tmp1=density_th_efi{i};
    tmp2=res_th_efi{i};
    [u,~,idx]=unique(tmp1(:,1));
    tmp1=[u,accumarray(idx,tmp1(:,2),[],@mean)];
    density_th_efi{i}=tmp1;
    table1 = [table1;tmp1];
    
    [u,~,idx]=unique(tmp2(:,1));
    tmp2=[u,accumarray(idx,tmp2(:,2),[],@mean)];
    res_th_efi{i}=tmp2;
    table2 = [table2;tmp2];
end
clear density_th_efi;
clear res_th_efi;
for i = 1:length(table1)
   [u,~,idx]=unique(table1(:,1));
   density_th_efi=[u,accumarray(idx,table1(:,2),[],@mean)];
   [u,~,idx]=unique(table2(:,1));
   res_th_efi=[u,accumarray(idx,table2(:,2),[],@mean)];
end


table1 =[];
table2 =[];
for i = 1:length(density_th_normal)
    tmp1=density_th_normal{i};
    tmp2=res_th_normal{i};
    [u,~,idx]=unique(tmp1(:,1));
    tmp1=[u,accumarray(idx,tmp1(:,2),[],@mean)];
    density_th_normal{i}=tmp1;
    table1 = [table1;tmp1];
    
    [u,~,idx]=unique(tmp2(:,1));
    tmp2=[u,accumarray(idx,tmp2(:,2),[],@mean)];
    res_th_normal{i}=tmp2;
    table2 = [table2;tmp2];
end
clear density_th_normal;
clear res_th_normal;
for i = 1:length(table1)
   [u,~,idx]=unique(table1(:,1));
   density_th_normal=[u,accumarray(idx,table1(:,2),[],@mean)];
   [u,~,idx]=unique(table2(:,1));
   res_th_normal=[u,accumarray(idx,table2(:,2),[],@mean)];
end

%% Plot results
% fig_res_th_change = figure;
% bar(res_th_change(:,1),res_th_change(:,2));
% xlabel('Resources rate');
% ylabel('Throughput change');
% title('Change in throughput from WiFi to EFi vs Resources');
% 
% fig_density_th_change = figure;
% bar(density_th_change(:,1),density_th_change(:,2));
% xlabel('Number of Clients per Cluster')
% ylabel('Throughput change');
% title('Change in throughput from WiFi to EFi vs Clients per Cluster');
% 
% saveas(fig_res_th_change, 'throughput_change_density.fig', 'fig');
% saveas(fig_res_th_change, 'throughput_change_density.png', 'png');
% 
% saveas(fig_density_th_change, 'throughput_change_resrate.fig', 'fig');
% saveas(fig_density_th_change, 'throughput_change_resrate.png', 'png');

% fig_res_th_all = figure;
% plot(res_th_efi(:,1),res_th_efi(:,2), 'DisplayName', 'EFI with 20 nodes');
% hold on;
% plot(res_th_normal(:,1),res_th_normal(:,2), 'DisplayName', 'WIFI with 20 nodes');
% xlabel('Resources rate');
% ylabel('Throughput (MBytes/s');
% title('EFI and WIFI average Throughput per Resource rate');
% legend('show');

fig_density_th_all = figure;
% plot(density_th_efi(:,1),density_th_efi(:,2), 'DisplayName', 'EFI with 20 nodes');
% hold on;
% plot(density_th_normal(:,1),density_th_normal(:,2), 'DisplayName', 'WIFI with 20 nodes');
bar([density_th_efi(:,2) density_th_normal(:,2)]);
xlabel('Number of Clients per Cluster (K)')
ylabel('Throughput (MBytes/s)');
title('EFI and WIFI average Throughput per K');
xticks([1 2 3 4 5 6 7]);
xticklabels({'K=1 EFi','K=1 WiFi'});
% legend('show');

% saveas(fig_res_th_all, 'throughput_all_resrate.fig', 'fig');
% saveas(fig_res_th_all, 'throughput_all_resrate.png', 'png');

saveas(fig_density_th_all, 'throughput_all_density.fig', 'fig');
saveas(fig_density_th_all, 'throughput_all_density.png', 'png');



%% Process Group Owners Measurements
load efi/relay_measurements.txt;
relay_measurements(:,3)=[]; %% Irrelevant data
relay_measurements(:,3)=[]; %% Irrelevant data

[f,cdf] = ecdf(relay_measurements(:,2));
avg_qw_fig=figure;
plot(cdf,f);
title('ECDF of GO Avg Queue Wait time');
xlabel('Avg Queue Wait time (s)');
ylabel('Probability');

[f,cdf] = ecdf(relay_measurements(:,3));
avg_th_fig=figure;
plot(cdf,f);
title('ECDF of GO Downlink throughput');
xlabel('Downlink throughput (MB/s)');
ylabel('Probability');

saveas(avg_th_fig, 'ecdf_go_throughput.png', 'png');
saveas(avg_th_fig, 'ecdf_go_throughput.fig', 'fig');

saveas(avg_qw_fig, 'ecdf_go_avg_queue_wait.png', 'png');
saveas(avg_qw_fig, 'ecdf_go_avg_queue_wait.fig', 'fig');