function pic = Read_Data1(pic_num)

if nargin < 1
    pic_num = 1365;
end
rec_size = 400;
magnification = 3;
full_size = rec_size * magnification;
bar_loc = [160,70,400,20];
bar_loc_fig = [full_size - bar_loc(2) - bar_loc(4) + 2, full_size - bar_loc(2) + 2, bar_loc(1), bar_loc(1) + bar_loc(3)];

hObject = 1;
pic_num = num2str(pic_num);
path = '..\cache_data\';
out_path = strcat('pic_data\FOLDER', pic_num, '.tiff');
file = fopen(strcat(path,'worm_range\',pic_num),'rb');
range = fread(file,4,'int');
fclose(file);

size = [range(4)-range(3), range(2)-range(1)];
loc_base = floor(([rec_size, rec_size] - size)/2) - 1;
orig_base = [range(3), range(1)] - loc_base;
fig_rect = [0,0,full_size, full_size];

figure(hObject);
set(hObject, 'units', 'pixel', 'position', fig_rect, 'resize', 'off');
movegui(hObject, 'center');
axes1 = axes('parent', 1, 'tag', 'outer_axes', 'position', [0,0,1,1]);

orig_fig = imread(strcat('..\worm_pic\', pic_num, '.tiff'));
set(hObject, 'currentaxes', axes1);
imagesc(orig_fig);
Rect_Plot([orig_base, rec_size, rec_size],'r-'), colormap gray;
Rect_Plot([range(3),range(1),size], 'g-');
Frame_Save(1, strrep(out_path, 'FOLDER', '0_orig_fig'), axes1);
delete(get(1,'children'))

binary_raw = Get_Mat(strcat(path, 'binary_raw\', pic_num), [512,512], 'uint8');
axes1 = Show_Background(1, binary_raw, rec_size * magnification);
Rect_Plot([orig_base, rec_size, rec_size],'r-'), colormap gray;
Rect_Plot([range(3),range(1),size], 'g-');
Frame_Save(1, strrep(out_path, 'FOLDER', '1_binary_raw'), axes1);
delete(axes1);

background = orig_fig(orig_base(1):orig_base(1)+rec_size-1, orig_base(2):orig_base(2)+rec_size-1);
inner_loc = [loc_base(2)+1, loc_base(1)+1, size(2), size(1)];
inner_loc_normalize = inner_loc / rec_size;
inner_loc = inner_loc * magnification;
inner_loc_fig = [inner_loc(2)+4,inner_loc(2)+inner_loc(4)+3, inner_loc(1)+1,inner_loc(1)+inner_loc(3)];

[axes1, axes2] = Show_Background(hObject, background, rec_size * magnification, inner_loc_normalize);
binary_denoised = Get_Mat(strcat(path, 'binary_denoised\', pic_num), size, 'uint8');
imagesc(binary_denoised), colormap gray;
Frame_Save(hObject, strrep(out_path, 'FOLDER', '2_binary_denoised'), axes1);
delete(get(axes2, 'children'));

dist_mat = Get_Mat(strcat(path, 'dist_mat\', pic_num), size, 'single');
set(hObject, 'currentaxes', axes2);
imagesc(dist_mat), colormap default, hold on;
bar = colorbar('location','southoutside', 'units','pixel','position',bar_loc,'fontsize',24);
set(axes2, 'position', inner_loc_normalize);
axis off;
colormap default;
set(hObject, 'currentaxes', axes1);
color_frame = getframe();
colormap gray;
gray_frame = getframe();
gray_frame.cdata(bar_loc_fig(1):bar_loc_fig(2), bar_loc_fig(3):bar_loc_fig(4), :) =...
    color_frame.cdata(bar_loc_fig(1):bar_loc_fig(2), bar_loc_fig(3):bar_loc_fig(4), :);
gray_frame.cdata(inner_loc_fig(1):inner_loc_fig(2), inner_loc_fig(3):inner_loc_fig(4), :) =...
    color_frame.cdata(inner_loc_fig(1):inner_loc_fig(2), inner_loc_fig(3):inner_loc_fig(4), :);
imwrite(gray_frame.cdata, strrep(out_path, 'FOLDER', '3_dist_mat'));
delete([get(axes2, 'children'), bar]);

lap_mat = abs(Get_Mat(strcat(path, 'lap_mat\', pic_num), size, 'double'));
set(hObject, 'currentaxes', axes2);
imagesc(lap_mat);
bar = colorbar('location','southoutside', 'units','pixel','position',bar_loc,'fontsize',24);
set(axes2, 'position', inner_loc_normalize);
axis off;
colormap default;
set(hObject, 'currentaxes', axes1);
color_frame = getframe();
colormap gray;
gray_frame = getframe();
gray_frame.cdata(bar_loc_fig(1):bar_loc_fig(2), bar_loc_fig(3):bar_loc_fig(4), :) =...
    color_frame.cdata(bar_loc_fig(1):bar_loc_fig(2), bar_loc_fig(3):bar_loc_fig(4), :);
gray_frame.cdata(inner_loc_fig(1):inner_loc_fig(2), inner_loc_fig(3):inner_loc_fig(4), :) =...
    color_frame.cdata(inner_loc_fig(1):inner_loc_fig(2), inner_loc_fig(3):inner_loc_fig(4), :);
imwrite(gray_frame.cdata, strrep(out_path, 'FOLDER', '4_lap_mat'));
delete([get(axes2, 'children'), bar]);

candidate = Read_Points(strcat(path,'candidate_points\', pic_num), 'int');
set(hObject, 'currentaxes', axes2);
imagesc(binary_denoised), colormap gray, hold on;
plot(candidate.cood(:,2), candidate.cood(:,1),'b.');
Frame_Save(hObject, strrep(out_path, 'FOLDER', '5_candidate'), axes1);
delete(get(axes2, 'children'));

graph_unpruned = Read_Graph(strcat(path,'graph_unpruned\',pic_num));
set(hObject, 'currentaxes', axes2);
imagesc(binary_denoised), colormap gray, hold on;
draw_skeleton(graph_unpruned.node);
Frame_Save(hObject, strrep(out_path, 'FOLDER', '6_graph_unpruned'), axes1);
delete(get(axes2, 'children'));

graph_pruned = Read_Graph(strcat(path,'graph_pruned\',pic_num));
set(hObject, 'currentaxes', axes2);
imagesc(binary_denoised), colormap gray, hold on;
draw_skeleton(graph_pruned.node);
Frame_Save(hObject, strrep(out_path, 'FOLDER', '7_graph_pruned'), axes1);
delete(get(axes2, 'children'));

backbone_unsmoothed = Read_Points(strcat(path,'backbone_unsmoothed\', pic_num),'double');
set(hObject, 'currentaxes', axes2);
imagesc(binary_denoised), colormap gray, hold on;
plot(backbone_unsmoothed.cood(:,2), backbone_unsmoothed.cood(:,1),'b-');
Frame_Save(hObject, strrep(out_path, 'FOLDER', '8_backbone_unsmoothed'), axes1);
delete(get(axes2, 'children'));

backbone_smoothed = Read_Points(strcat(path,'backbone_smoothed\', pic_num),'double');
set(hObject, 'currentaxes', axes2);
imagesc(binary_denoised), colormap gray, hold on;
plot(backbone_smoothed.cood(:,2), backbone_smoothed.cood(:,1),'b-');
plot(backbone_smoothed.cood(1:10:end,2), backbone_smoothed.cood(1:10:end,1),'b.','MarkerSize',5);
Frame_Save(hObject, strrep(out_path, 'FOLDER', '9_backbone_smoothed'), axes1);
close all;

pic = struct('size', size, 'orig_fig', orig_fig, 'binary_raw', binary_raw, 'binary_denoised', binary_denoised,...
    'dist_mat', dist_mat, 'candidate', candidate, 'graph_unpruned', graph_unpruned, 'lap_mat', lap_mat,...
    'graph_pruned', graph_pruned, 'backbone_unsmoothed', backbone_unsmoothed, 'backbone_smoothed', backbone_smoothed);
end

function Saveas_Center(h, save_file, fig_type, size)
size = size * 4;
saveas(h, save_file, fig_type);
end