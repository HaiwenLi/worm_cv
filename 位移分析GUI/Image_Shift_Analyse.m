function varargout = Image_Shift_Analyse(varargin)
%FIG M-file for Fig.fig
%      FIG, by itself, creates a new FIG or raises the existing
%      singleton*.
%
%      H = FIG returns the handle to a new FIG or the handle to
%      the existing singleton*.
%
%      FIG('Property','Value',...) creates a new FIG using the
%      given property value pairs. Unrecognized properties are passed via
%      varargin to Image_Shift_Analyse_OpeningFcn.  This calling syntax produces a
%      warning when there is an existing singleton*.
%
%      FIG('CALLBACK') and FIG('CALLBACK',hObject,...) call the
%      local function named CALLBACK in FIG.M with the given input
%      arguments.
%4
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help Fig

% Last Modified by GUIDE v2.5 14-Jul-2014 18:52:40

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @Image_Shift_Analyse_OpeningFcn, ...
                   'gui_OutputFcn',  @Image_Shift_Analyse_OutputFcn, ...
                   'gui_LayoutFcn',  [], ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

% --- Executes just before Fig is made visible.
function Image_Shift_Analyse_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
global handle_group;
global L_data;
global R_data;
global main_data;
global history_data;
global CONST_PARA;
global path;
handles.output = hObject;

clc;
format short g;

path = struct('fig_path', '', 'default_path', '');
path.default_path = mfilename('fullpath');
i = findstr(path.default_path, '\');
path.default_path = path.default_path(1:i(end));

handle_group = guidata(hObject);
L_data = struct('fig',[],'obj',[]);
R_data = struct('fig',[],'ROI',[]);
main_data = struct('rect_size',[40,30], 'history_num',0, 'history_now',0);
history_data = cell(1000, 1);
CONST_PARA = struct('RECT_SIZE_MAX', 80, 'RECT_SIZE_MIN', 20, 'RECT_PADDING', 100,...
    'BEST_RECT_NUM', 15, 'CORR_MAX_THRES', 0.95, 'CORR_MAX_THRES_LOW', 0.8, 'MATCH_PRECISION', 0.001);

set(handle_group.obj_select,'enable','off');
set(handle_group.ROI_select,'enable','off');
set(handle_group.rect_width,'string',num2str(main_data.rect_size(1)));
set(handle_group.rect_height,'string',num2str(main_data.rect_size(2)));
hold(handle_group.L_axe, 'on');
hold(handle_group.R_axe, 'on');
Check_Start();
Check_BackNext();
Set_Shift();
Set_obj();
Set_ROI();
axes(handle_group.L_axe);
axis off;
axes(handle_group.R_axe);
axis off;
movegui(hObject,'center');
% Update handles structure
guidata(hObject, handles);

%=================================SET FUNC=================================
function Set_rect_width(hObject, eventdata, handles)
global main_data;
global CONST_PARA;
width = floor(str2double(get(hObject, 'string')));
width = max(width, CONST_PARA.RECT_SIZE_MIN);
width = min(width, CONST_PARA.RECT_SIZE_MAX);
main_data.rect_size(1) = width;
set(hObject, 'string', num2str(width));

function Set_rect_height(hObject, eventdata, handles)
global main_data;
global CONST_PARA;
height = floor(str2double(get(hObject, 'string')));
height = max(height, CONST_PARA.RECT_SIZE_MIN);
height = min(height, CONST_PARA.RECT_SIZE_MAX);
main_data.rect_size(2) = height;
set(hObject, 'string', num2str(height));

function Set_Shift(shift)
global handle_group;
global main_data;
if (nargin == 0 || isempty(shift))
    main_data.shift = [];
    shift_string = 'N/A';
    set(handle_group.Show_Detail, 'enable', 'off');
else
    main_data.shift = shift;
    shift_string = sprintf('%6.2f , %6.2f',shift);
    set(handle_group.Show_Detail, 'enable', 'on');
end
set(handle_group.fig_shift, 'string', shift_string);

function Set_obj(obj_rect)
global handle_group;
global L_data;
if (nargin == 0)
    L_data.obj = [];
    obj_string = 'N/A';
    size_string = 'N/A';
else
    L_data.obj = obj_rect;
    left_top_point = obj_rect(1:2);
    obj_size = obj_rect(3:4);
    obj_string = sprintf('%4.0f,%4.0f',left_top_point);
    size_string = sprintf('%4.0f,%4.0f',obj_size);
end
set(handle_group.obj_cood, 'string', obj_string);
set(handle_group.obj_size, 'string', size_string);

function Set_ROI(ROI_rect)
global handle_group;
global R_data;
if (nargin == 0)
    R_data.ROI = [];
    ROI_string = 'N/A';
    size_string = 'N/A';
else
    R_data.ROI = ROI_rect;
    left_top_point = ROI_rect(1:2);
    ROI_size = ROI_rect(3:4);
    ROI_string = sprintf('%4.0f,%4.0f',left_top_point);
    size_string = sprintf('%4.0f,%4.0f',ROI_size);
end
set(handle_group.ROI_cood, 'string', ROI_string);
set(handle_group.ROI_size, 'string', size_string);

function Set_L_fig(L_fig)
global handle_group;
global L_data;
L_data.fig = L_fig;
L_data.obj = [];
delete(get(handle_group.L_axe, 'children'));
imshow(L_data.fig, 'parent', handle_group.L_axe);
set(handle_group.obj_select,'enable','on');
Set_obj();

function Set_R_fig(R_fig)
global handle_group;
global R_data;
R_data.fig = R_fig;
R_data.ROI = [];
delete(get(handle_group.R_axe, 'children'));
imshow(R_data.fig, 'parent', handle_group.R_axe);
set(handle_group.ROI_select,'enable','on');
Set_ROI();

function Check_Start()
global handle_group;
global L_data;
global R_data;
enable_to_start = ~(isempty(L_data.fig) || isempty(L_data.obj) ||...
        isempty(R_data.fig) || isempty(R_data.ROI));
if enable_to_start
    set(handle_group.start, 'enable', 'on');
else
    set(handle_group.start, 'enable', 'off');
end

function Check_BackNext()
global handle_group;
global main_data;
if (main_data.history_now >= main_data.history_num)
    set(handle_group.next, 'enable', 'off');
else
    set(handle_group.next, 'enable', 'on');
end
if (main_data.history_now <= 1)
    set(handle_group.back, 'enable', 'off');
else
    set(handle_group.back, 'enable', 'on');
end

%===============================TOOL FUNC==================================
function rect_mat = Get_Mat_By_Rect(mat, rect)
rect = floor(rect);
rect_mat = mat(rect(2):rect(2)+rect(4)-1, rect(1):rect(1)+rect(3)-1);

function rect_inside = Limit_Rect_In_BoxRect(rect, box_rect)
left_top_point  = rect(1:2);
left_top_box    = box_rect(1:2);
right_bottom_point  = rect(1:2)     + rect(3:4);
right_bottom_box    = box_rect(1:2) + box_rect(3:4);
left_top_point      = max(left_top_point    , left_top_box);
right_bottom_point  = min(right_bottom_point, right_bottom_box);
rect_inside = [left_top_point, right_bottom_point - left_top_point];

function Rect_Plot(hObject, rect, color)
hold on;
left = rect(1);
right = rect(1) + rect(3);
top = rect(2);
bottom = rect(2) + rect(4);
X_line = [left, right, right, left, left];
Y_line = [top, top, bottom, bottom, top];
plot(hObject, X_line, Y_line, color);

function Delete_Lines(hObject)
graph_obj = get(hObject, 'children');
for i = 1:length(graph_obj)
    if ~strcmp(get(graph_obj(i),'type'),'image')
        delete(graph_obj(i));
    end
end

function rect_sum_mat = Rect_Sum_Calc(init_mat, rect_size)
[init_row, init_col] = size(init_mat);
rect_row = rect_size(2);
rect_col = rect_size(1);
tic;
col_diff = init_mat(:, rect_col+1:end) - init_mat(:, 1:init_col-rect_col);
diff_with_first_col = cumsum(col_diff, 2);
first_col = sum(init_mat(:,1:rect_col), 2);
col_sum_mat = [first_col, diff_with_first_col + repmat(first_col, 1, init_col-rect_col)];

row_diff = col_sum_mat(rect_row+1:end, :) - col_sum_mat(1:init_row-rect_row, :);
diff_with_first_row = cumsum(row_diff, 1);
first_row = sum(col_sum_mat(1:rect_row, :), 1);
rect_sum_mat = [first_row; diff_with_first_row + repmat(first_row, init_row-rect_row, 1)];

function best_rect = Find_Best_Rect(fig_mat, range_rect)
global main_data;
global CONST_PARA;
best_rect = zeros(CONST_PARA.BEST_RECT_NUM,4);
rect = Get_Mat_By_Rect(double(fig_mat), range_rect);
rect_mean = mean(mean(rect));
rect_del2_abs = abs(del2(double(rect > rect_mean)));
rect_sum_mat = Rect_Sum_Calc(rect_del2_abs, main_data.rect_size);
for i = 1:CONST_PARA.BEST_RECT_NUM
    max_rect_sum = max(max(rect_sum_mat));
    if (max_rect_sum <= 0)
        best_rect = best_rect(1:i, :);
        [temp, order] = sort(best_rect(1:i, 2) * max(best_rect(1:i, 1)) + best_rect(1:i, 1));
        best_rect = best_rect(order, :);
        return;
    end
    [row,col] = find(rect_sum_mat == max_rect_sum, 1);
    best_rect(i,:) = [col+range_rect(1)-1, row+range_rect(2)-1,main_data.rect_size];
    delete_area_lefttop = max([col,row]-CONST_PARA.RECT_PADDING, [1,1]);
    delete_area_rightbottom = min([col,row]+main_data.rect_size + CONST_PARA.RECT_PADDING-1, range_rect(3:4) - main_data.rect_size + 1);
    rect_sum_mat(delete_area_lefttop(2):delete_area_rightbottom(2), delete_area_lefttop(1):delete_area_rightbottom(1)) = 0;
end
[temp, order] = sort(best_rect(:, 2) * max(best_rect(:, 1)) + best_rect(:, 1));
best_rect = best_rect(order, :);

function Show_History_Data()
global handle_group;
global history_data;
global main_data;
data = history_data{main_data.history_now};
Set_L_fig(data.L_fig);
Set_R_fig(data.R_fig);
for i = 1:length(data.shift_detail)
    Rect_Plot(handle_group.L_axe, data.L_rect(i, :), 'g-');
    text(data.L_rect(i,1)-30, data.L_rect(i,2)-30, strcat('Rect ', num2str(i)), 'parent', handle_group.L_axe);
    Rect_Plot(handle_group.R_axe, data.R_rect(i, :), 'g-');
    text(data.R_rect(i,1)-30, data.R_rect(i,2)-30, strcat('Rect ', num2str(i)), 'parent', handle_group.R_axe);
end
Set_Shift(data.shift);
Check_Start();
Check_BackNext();

%==============================CONTROL FUNC================================
function fig_mat = Select_Fig_File()
global path;
if ~isempty(path.fig_path)
    cd(path.fig_path);
end
msg = '请选择图片文件';
[fig_name,path.fig_path] = uigetfile({...
    '*.tif;*.tiff;*.jpg;*.png;*.bmp;*.*',msg;...
    '*.tif','(*.tif)TIF图像';...
    '*.tiff','(*.tiff)TIFF图像';...
    '*.jpg','(*.jpg)JPG图像';...
    '*.png','(*.png)PNG图像';...
    '*.bmp','(*.bmp)BMP图像';...
    '*.*','(*.*)其他格式图像'},msg);
cd(path.default_path);
try
    fig_mat = imread(strcat(path.fig_path, fig_name));
catch
    warndlg('图片读取错误！','文件读取错误','modal');
    fig_mat = [];
    return;
end

function Select_L_Fig(hObject, eventdata, handles)
fig_mat = Select_Fig_File();
if ~isempty(fig_mat)
    Set_L_fig(fig_mat)
    Check_Start();
    Set_Shift();
end

function Select_R_Fig(hObject, eventdata, handles)
fig_mat = Select_Fig_File();
if ~isempty(fig_mat)
    Set_R_fig(fig_mat);
    Check_Start();
    Set_Shift();
end

function rect_inside = Get_Rect(rect_box)
global CONST_PARA;
axis on;
grid on;
input_rect = getrect();
grid off;
axis off;
rect_inside = Limit_Rect_In_BoxRect(input_rect, rect_box);
if (min(rect_inside(3:4)) < CONST_PARA.RECT_SIZE_MIN)
    rect_inside = [];
    str = num2str(CONST_PARA.RECT_SIZE_MIN);
    warndlg(strcat('选中区域在图像内部分需大于',str,'*',str), '错误的物体坐标', 'modal');
end

function Select_obj(hObject, eventdata, handles)
global handle_group;
global L_data;
fig_box = [1, 1, size(L_data.fig, 2), size(L_data.fig, 1)];
set(handle_group.L_axe, 'YAxisLocation','right');
set(handle_group.Fig,'currentaxes',handle_group.L_axe);
Delete_Lines(handle_group.L_axe);
inside_part_rect = Get_Rect(fig_box);
if (~isempty(inside_part_rect))
    Set_obj(inside_part_rect);
    Check_Start();
end
Rect_Plot(handle_group.L_axe, L_data.obj, 'r-');

function Select_ROI(hObject, eventdata, handles)
global handle_group;
global R_data;
fig_box = [1, 1, size(R_data.fig, 2), size(R_data.fig, 1)];
set(handle_group.R_axe, 'YAxisLocation','left');
set(handle_group.Fig,'currentaxes',handle_group.R_axe);
Delete_Lines(handle_group.R_axe);
inside_part_rect = Get_Rect(fig_box);
if (~isempty(inside_part_rect))
    Set_ROI(inside_part_rect);
    Check_Start();
end
Rect_Plot(handle_group.R_axe, R_data.ROI, 'b-');

function next_Callback(hObject, eventdata, handles)
global main_data;
main_data.history_now = main_data.history_now + 1;
Show_History_Data();

function back_Callback(hObject, eventdata, handles)
global main_data;
main_data.history_now = main_data.history_now - 1;
Show_History_Data();

function Show_Detail_Callback(hObject, eventdata, handles)
global main_data;
global history_data;
data = history_data{main_data.history_now};
Show_History_Data();
new_figure = figure;
set(new_figure,'name','Rect Detail Information','units','pixels', 'position', [0,0,600,560],...
    'NumberTitle','off','MenuBar','none','WindowStyle','normal');
row = length(data.corr_max_detail);
row_name = cell(row, 1);
table_data = cell(row, 4);
for i = 1:row
    row_name{i} = sprintf('Rect %d :', i);
    table_data{i,1} = sprintf('%6.2f , %6.2f', data.shift_detail(i, :));
    table_data{i,2} = sprintf('%4.0f,%4.0f;%4.0f,%4.0f', data.L_rect(i, :));
    table_data{i,3} = sprintf('%4.0f,%4.0f;%4.0f,%4.0f', data.R_rect(i, :));
    table_data{i,4} = data.corr_max_detail(i);
end
column_name = {'          Shift          ','          Left Rect          ', '          Right Rect          ', 'Corr Max'};
column_editable = [false, false, false, false];
uitable('parent', new_figure,'fontsize',10, 'data', table_data, 'columnname', column_name,'rowname', row_name,...
    'columneditable', column_editable, 'units', 'normalized', 'position',[0,0,1,1]);
movegui(new_figure,'center');


% ==============================MAIN FUNC==================================
function [match_rect, corr_max, time] = Match(raw_image, seg_image)
global CONST_PARA;
raw_size = size(raw_image);
seg_size = size(seg_image);
corr_size = raw_size - seg_size + 1;
corr_matrix = zeros(corr_size);

segment_image_mean=sum(sum(seg_image))/(seg_size(1) * seg_size(2));
seg_image=seg_image-segment_image_mean;
segment_image_norm= norm(seg_image,'fro');
for i = 1 : corr_size(1)
    for j = 1 : corr_size(2)
        raw_seg = raw_image(i:i+seg_size(1)-1,j:j+seg_size(2)-1);
        raw_seg_mean = sum(sum(raw_seg))/(seg_size(1)*seg_size(2));
        raw_seg=raw_seg-raw_seg_mean;
        raw_seg_norm = norm(raw_seg,'fro');
        corr_matrix(i,j) = sum(sum(raw_seg.*seg_image))/(raw_seg_norm*segment_image_norm);
    end
end
corr_max = max(max(corr_matrix));
[match_rect(2),match_rect(1)] = find(corr_matrix == corr_max);
if (corr_max < CONST_PARA.CORR_MAX_THRES_LOW)
    return;
end

left = max(match_rect(1) - 1, 1);
right = min(match_rect(1) + 1, size(corr_matrix, 2));
top = max(match_rect(2) - 1, 1);
bottom = max(match_rect(2) + 1, size(corr_matrix, 1));
step = 1;
corr_local = corr_matrix(top:bottom, left:right);
[X,Y] = meshgrid(left:right, top:bottom);
while(step > CONST_PARA.MATCH_PRECISION)
    step = step/2;
    [X2,Y2] = meshgrid(match_rect(1)-2*step : step : match_rect(1)+2*step ,...
                       match_rect(2)-2*step : step : match_rect(2)+2*step);
    corr_interp = interp2(X,Y,corr_local,X2,Y2,'spline');
    corr_max = max(max(corr_interp));
    [subX,subY] = find(corr_interp == corr_max);
    match_rect = [X2(subX,subY), Y2(subX,subY)];
    Xleft = max(1, subX-1);
    Xright = min(5, subX+1);
    Yleft = max(1, subY-1);
    Yright = min(5, subY+1);
    X = X2(Xleft:Xright, Yleft:Yright);
    Y = Y2(Xleft:Xright, Yleft:Yright);
    corr_local = corr_interp(Xleft:Xright, Yleft:Yright);
end

function start_Callback(hObject, eventdata, handles)
global L_data;
global R_data;
global main_data;
global history_data;
global CONST_PARA;
L_size = [size(L_data.fig, 2), size(L_data.fig, 1)];
R_size = [size(R_data.fig, 2), size(R_data.fig, 1)];
if (min(R_data.ROI(3:4) - L_data.obj(3:4)) < 0)
    warndlg({'ROI区域小于object区域！','ROI图像需包含对应的object图块！'},'匹配区域选取错误！','modal');
    return;
end

obj_block = Get_Mat_By_Rect(double(L_data.fig), L_data.obj);
ROI_block = Get_Mat_By_Rect(double(R_data.fig), R_data.ROI);
scaling = round(min(L_data.obj(3)/main_data.rect_size(1), L_data.obj(4)/main_data.rect_size(2)));
obj_block = obj_block(1:scaling:end, 1:scaling:end);
ROI_block = ROI_block(1:scaling:end, 1:scaling:end);

[rough_shift, corr_max] = Match(ROI_block, obj_block);
if (corr_max < CONST_PARA.CORR_MAX_THRES_LOW)
    warndlg({strcat('相关系数为：',sprintf('%.2f',corr_max),' ,未达标准！'),...
        '请确定右侧蓝色框线完整包含左侧红色框线区域内的图像！'},'匹配错误','modal');
    return;
end
rough_shift = floor(rough_shift * scaling - L_data.obj(1:2) + R_data.ROI(1:2));

fig_R_cood_in_fig_L = [[1,1] - rough_shift + CONST_PARA.RECT_PADDING, R_size - 2 * CONST_PARA.RECT_PADDING];
coincide_rect = Limit_Rect_In_BoxRect(fig_R_cood_in_fig_L, [1, 1, L_size]);

L_rect = Find_Best_Rect(L_data.fig, coincide_rect);
shift_detail = zeros(size(L_rect, 1), 2);
corr_max_detail = zeros(size(L_rect, 1), 1);
promising_rect_R = zeros(1,4);
for i = 1:size(L_rect, 1)
    current_rect_L = L_rect(i, :);
    promising_rect_R(1:2) = current_rect_L(1:2) + rough_shift - CONST_PARA.RECT_PADDING/2;
    promising_rect_R(3:4) = current_rect_L(3:4) + CONST_PARA.RECT_PADDING;
    obj_block = Get_Mat_By_Rect(double(L_data.fig), current_rect_L);
    ROI_block = Get_Mat_By_Rect(double(R_data.fig), promising_rect_R);
    [shift_detail(i, :), corr_max_detail(i)] = Match(ROI_block, obj_block);
    shift_detail(i, :) = shift_detail(i, :) - CONST_PARA.RECT_PADDING/2 + rough_shift;
end
item_delete = logical(corr_max_detail < CONST_PARA.CORR_MAX_THRES);
shift_detail(item_delete, :) = [];
corr_max_detail(item_delete, :) = [];
L_rect(item_delete, :) = [];
R_rect(:, 3:4) = L_rect(:, 3:4);
R_rect(:, 1:2) = L_rect(:, 1:2)+ shift_detail;
shift = mean(shift_detail, 1);

main_data.history_num = main_data.history_num + 1;
main_data.history_now = main_data.history_num;
history_data{main_data.history_now} =...
    struct('L_fig'  , L_data.fig, 'R_fig'   , R_data.fig,...
           'L_rect' , L_rect    , 'R_rect'  , R_rect    ,...
           'shift_detail'   , shift_detail,...
           'corr_max_detail', corr_max_detail,...
           'shift', shift);
Show_History_Data();


% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % %
function rect_width_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function rect_height_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function varargout = Image_Shift_Analyse_OutputFcn(hObject, eventdata, handles)
varargout{1} = handles.output;


% --- Executes during object creation, after setting all properties.
function Fig_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Fig (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
