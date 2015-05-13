function points = Read_Points(file_path, type)

file = fopen(file_path, 'rb');
points.num = fread(file, 1, 'int');
points.cood = fread(file, 2 * points.num, type) + 1;
points.cood = [points.cood(1:2:end), points.cood(2:2:end)];
points.cood(:, 1) = points.cood(:,1);
points.cood(:, 2) = points.cood(:,2);
fclose(file);
end