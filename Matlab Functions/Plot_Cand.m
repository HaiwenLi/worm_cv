function Plot_Cand(num,color)

if nargin<2
    color = 'r.';
end
points = Read_Points(strcat('..\cache_data\candidate_points\',num2str(num)),'int');
raw_img = imread(strcat('..\worm_pic\',num2str(num),'.tiff'));
imagesc(raw_img);
hold on;
plot(points(:,2),points(:,1),color,'MarkerSize',4);
axis equal;
colormap gray;

end