function Plot_Cand(num,color,data_prefix)

% img_file_prefix = '..\worm_pic\';
% img_file_prefix = '..\..\Elegan_Images\Group3\Centroid\image_';
img_file_prefix = '..\..\image1_tiff\image_';
if nargin<2
    color = 'r.';
end
if nargin<3
    data_prefix = '';
end
img_file_name = strcat(img_file_prefix,num2str(abs(num)),'.tiff');
file_name = strcat('..\cache_data\candidate_points\',data_prefix,num2str(num));

if exist(file_name,'file') && exist(img_file_name,'file')
    points = Read_Points(file_name,'int');
    raw_img = imread(img_file_name);
    imagesc(raw_img);
    hold on;
    plot(points(:,2),points(:,1),color,'MarkerSize',4);
    axis equal;
    colormap gray;
end

end