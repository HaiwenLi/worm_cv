function exist1 = Plot_Cline(num,color,data_prefix)

% img_file_prefix = '..\worm_pic\';
% img_file_prefix = '..\..\Elegan_Images\Group3\Roi\image_';
img_file_prefix = '..\..\image2_tiff\image_';
if nargin<2
    color = 'w-';
end
if nargin<3
    data_prefix = '';
end
if num>0
    file_name = strcat('..\cache_data\backbone_smoothed\',data_prefix, num2str(num));
else
    file_name = strcat('..\cache_data\backbone_unsmoothed\',data_prefix, num2str(-num));
end
figure(1);
axe1 = get(1,'children');
img_file_name = strcat(img_file_prefix,num2str(abs(num)),'.tiff');
exist1 = exist(file_name,'file') && exist(img_file_name,'file');
if exist1
%     raw_img = imread(img_file_name);
    points = Read_Points(file_name,'double');
%     imagesc(raw_img);
    hold on;
    delete(get(axe1,'children'));
    set(axe1,'XLim',[0,512],'YLim',[0,512]);
    plot(points(:,2),points(:,1),color);
    hold on;
    plot(points(1,2),points(1,1),'ro');
    axis equal;
    colormap gray;
end

end