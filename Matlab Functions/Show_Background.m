function [axes1, axes2] = Show_Background(hObject, background, size, inner_rect)
figure(hObject);
set(hObject, 'units', 'pixel', 'position', [0,0,size, size], 'resize', 'off');
movegui(hObject, 'center');
axes1 = axes('position', [0,0,1,1]);
set(hObject, 'currentaxes', axes1);
imagesc(background);
if nargin < 4 && nargout < 2
    return;
end
axes2 = axes('units', 'normalize','position', inner_rect);
set(axes2, 'ydir', 'reverse');
set(hObject, 'currentaxes', axes2);
end