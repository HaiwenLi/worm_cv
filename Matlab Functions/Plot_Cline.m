function Plot_Cline(num,color)

if nargin<2
    color = 'w-';
end
if num>0
    points = Read_Points(strcat('..\cache_data\backbone_smoothed\',num2str(num)),'double');
else
    points = Read_Points(strcat('..\cache_data\backbone_unsmoothed\',num2str(-num)),'double');
end
raw_img = imread(strcat('..\worm_pic\',num2str(abs(num)),'.tiff'));
imagesc(raw_img);
hold on;
plot(points(:,2),points(:,1),color);
axis equal;
colormap gray;

end