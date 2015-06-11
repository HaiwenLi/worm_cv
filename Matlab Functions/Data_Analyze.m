function [cline,roi] = Data_Analyze(pic_start, pic_end, data_prefix)

if nargin<3
    data_prefix = '';
end
cline = zeros(pic_end-pic_start+1,101,2);
exists = ones(pic_end-pic_start+1,1);
for num=pic_start:pic_end
    file_name = strcat('..\cache_data\backbone_smoothed\',data_prefix, num2str(num));
    if exist(file_name,'file')
        Read_Points(file_name,'double');
        cline(num-pic_start+1,:,:) = Read_Points(file_name,'double');
    else
        exists(num-pic_start+1)=0;
    end
end
cline = cline(exists==1,:,:);
roi = [cline(:,31,1),cline(:,31,2)];
end