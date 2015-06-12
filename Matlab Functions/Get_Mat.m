function mat = Get_Mat(file_dir, type)
if nargin<2
    type='double';
end
file = fopen(file_dir, 'rb');
size = fread(file, 2, 'int');
mat = zeros(size(1), size(2), type);
for i = 1:size(1)
    mat(i,:) = fread(file, size(2), type);
end
fclose(file);
end