function Plot_Graph(num,color,data_prefix)

close all;
if nargin<2
    color = 'b';
end
if nargin<3
    data_prefix = '';
end
graph = Read_Graph(strcat('..\cache_data\graph_unpruned\',data_prefix,num2str(num)));
% raw_img = imread(strcat('..\worm_pic\',num2str(abs(num)),'.tiff'));
% imagesc(raw_img);
hold on;
draw_skeleton(graph, color);
axis equal;
colormap gray;

end