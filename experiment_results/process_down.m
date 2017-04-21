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

for i = 1:length(down)
    down{i} = [down{i} down{i}(:,5)-down{i}(:,8)];
end

for i = 1:length(down)
    throughput_change{i}=[down{i}(:,1) down{i}(:,2) down{i}(:,9)];
    [u,~,idx]=unique(throughput_change{i}(:,1));
    density_th{i}=[u,accumarray(idx,throughput_change{i}(:,3),[],@mean)];
    [u,~,idx]=unique(throughput_change{i}(:,2));
    res_th{i}=[u,accumarray(idx,throughput_change{i}(:,3),[],@mean)];
end

for i=1:length(density_th)
    tmp1=density_th{i};
    tmp2=res_th{i};
    for j=1:length(tmp1(:,1))
        tmp1(j,1)=length(find(throughput_change{i}(:,1)==tmp1(j,1)));
        tmp1(j,2)=tmp1(j,2)/tmp1(j,1);
        
        c=length(find(throughput_change{i}(:,2)==tmp2(j,1)));
        tmp2(j,2)=tmp2(j,2)/c;
    end
    density_th{i}=tmp1;
    res_th{i}=tmp2;
end
table1 =[];
table2 =[];
for i = 1:length(density_th)
    tmp1=density_th{i};
    tmp2=res_th{i};
    [u,~,idx]=unique(tmp1(:,1));
    tmp1=[u,accumarray(idx,tmp1(:,2),[],@mean)];
    density_th{i}=tmp1;
    table1 = [table1;tmp1];
    
    [u,~,idx]=unique(tmp2(:,1));
    tmp2=[u,accumarray(idx,tmp2(:,2),[],@mean)];
    res_th{i}=tmp2;
    table2 = [table2;tmp2];
end

clear density_th;
clear res_th;
for i = 1:length(table1)
   [u,~,idx]=unique(table1(:,1));
   density_th=[u,accumarray(idx,table1(:,2),[],@mean)];
   [u,~,idx]=unique(table2(:,1));
   res_th=[u,accumarray(idx,table2(:,2),[],@mean)];
end

figure;
bar(res_th(:,1),res_th(:,2));
xlabel('Resources rate');
ylabel('Throughput change');
title('Change in throughput from WiFi to EFi vs Resources');

figure;
bar(density_th(:,1),density_th(:,2));
xlabel('Number of Clients per Cluster')
ylabel('Throughput change');
title('Change in throughput from WiFi to EFi vs Clients per Cluster');

