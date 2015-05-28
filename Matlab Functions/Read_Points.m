function points = Read_Points(file_path, type)

file = fopen(file_path, 'rb');
num = fread(file, 1, 'int');
points = fread(file, 2 * num, type) + 1;
points = [points(1:2:end), points(2:2:end)];
points(:, 1) = points(:,1);
points(:, 2) = points(:,2);
fclose(file);
end