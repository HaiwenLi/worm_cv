function Frame_Save(hObject, path, axes)
all_axe = get(hObject, 'children');
for i = 1:length(all_axe)
    if ~strcmp(get(all_axe(i),'tag'), 'Colorbar');
        axis(all_axe(i),'off');
    end
end
set(hObject,'currentaxes', axes);
a = getframe();
imwrite(a.cdata, path);
end