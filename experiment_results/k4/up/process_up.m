%% loading and merging measurements
names = dir('efi/xx*');
for k =1:length(names)
    temp=load(['efi/' names(k).name]);
    temp = sortrows(temp,2);
    efi_up{k}=temp;
end


% Removing measurements for clients connect directly to AP
for i = 1:length(efi_up)
    efi_up{i}(efi_up{i}(:,1)==0,:)=[];
end

names = dir('normal/xx*');

%% Filtering out clients connected directly to AP
for k =1:length(names)
    temp=load(['normal/' names(k).name]);
    temp = sortrows(temp,2);
    normal_up{k}=temp;
end

for i=1:length(normal_up)
    keep=[];
    for j=1:length(normal_up{i}(:,1))
        if ismember(normal_up{i}(j,2), efi_up{i}(:,2))
            keep=[keep; normal_up{i}(j,2)];
        end
    end
    del = [];
    for j=1:length(normal_up{i}(:,1))
        if ~ismember(normal_up{i}(j,2), keep)
            del=[del;normal_up{i}(j,2)];
        end
    end
    
    for j=1:length(del)
        normal_up{i}(normal_up{i}(:,2)==del(j),:)=[];
    end
    
    del = [];
    for j=1:length(efi_up{i}(:,1))
        if ~ismember(efi_up{i}(j,2), keep)
            del=[del;efi_up{i}(j,2)];
        end
    end
    
    for j=1:length(del)
        efi_up{i}(efi_up{i}(:,2)==del(j),:)=[];
    end

end

qwt=[];
for i=1:length(efi_up)
    [u,~,idx]=unique(efi_up{i}(:,1));
    for j=1:length(u)
        u(j,1)=sum(u(j,1)==efi_up{i}(:,1));
    end
    tmp = [u ,accumarray(idx,efi_up{i}(:,4),[],@mean), accumarray(idx,normal_up{i}(:,4),[],@mean)];
    [u,~,idx]=unique(tmp(:,1));
    tmp= [u, accumarray(idx,tmp(:,2),[],@mean), accumarray(idx,tmp(:,3),[],@mean)];
    qwt =[qwt; tmp];
end

[u,~,idx]=unique(qwt(:,1));
qwt_err=[u,accumarray(idx,qwt(:,2),[],@std), accumarray(idx,qwt(:,3),[],@std)];
    for j=1:length(u)
        l = sum(u(j,1)==qwt(:,1));
        qwt_err(:,2)=qwt_err(:,2)./l;
        qwt_err(:,3)=qwt_err(:,2)./l;
    end
qwt=[u,accumarray(idx,qwt(:,2),[],@mean), accumarray(idx,qwt(:,3),[],@mean), accumarray(idx,qwt(:,1),[],@sum)];

efi_qwt=[];
for i =1:length(efi_up)
    efi_qwt = [efi_qwt;efi_up{i}(:,4)];
end

normal_qwt=[];
for i =1:length(normal_up)
    normal_qwt = [normal_qwt;normal_up{i}(:,4)];
end

avg_qwt_all = figure;
colormap([1 1 1]);
b = barwitherr(qwt_err(:,2:3), qwt(:,2:3));
% set(gca, 'XTickLabel', {'5%','10%','15%','20%'});
title('EFI vs WIFI average Queue Wait time');
ylim([0 0.07]);
hatchfill2(b(1),'single');
hatchfill2(b(2),'fill');
xlabel('Cluster Size (K)');
ylabel('Average queue wait (s)');
% set(gca,'XTickLabel',{'5%','10%','15%','20%'})
[~,c,~,~]=legendflex([b(1) b(2)], {'EFI','WIFI'}, 'anchor', {'ne','ne'});
hatchfill2(c(3),'single');
hatchfill2(c(4),'fill');
saveas(avg_qwt_all,'avg_qwt_k4.fig','fig');
saveas(avg_qwt_all,'avg_qwt_k4.png','png');

avg_qwt_ecdf = figure;
title('EFI vs WIFI average Queue Wait time ECDF')
hold on;

[f,cdf] = ecdf(normal_qwt);
plot(cdf,f, 'DisplayName', 'WIFI');
[f,cdf] = ecdf(efi_qwt);
plot(cdf,f, 'DisplayName', 'EFI');
legend show;
saveas(avg_qwt_ecdf,'avg_qwt_ecdf.fig','fig');
saveas(avg_qwt_ecdf,'avg_qwt_ecdf.png','png');
