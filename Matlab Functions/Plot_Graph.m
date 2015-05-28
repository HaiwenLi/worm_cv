function Plot_Graph(num,color)

if nargin<2
    color = 'b';
end
if num<0
    graph = Read_Graph(strcat('..\cache_data\graph_unpruned\',num2str(-num)));
else
    graph = Read_Graph(strcat('..\cache_data\graph_pruned\',num2str(num)));
end
raw_img = imread(strcat('..\worm_pic\',num2str(abs(num)),'.tiff'));
imagesc(raw_img);
hold on;
draw_skeleton(graph, color);
axis equal;
colormap gray;

end