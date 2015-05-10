function mat = Get_Mat(file_dir, size, type)

file = fopen(file_dir, 'rb');
mat = zeros(size(1), size(2), type);
for i = 1:size(1)
    mat(i,:) = fread(file, size(2), type);
end
fclose(file);
end