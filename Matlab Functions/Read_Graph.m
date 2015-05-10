function graph = Read_Graph(file_name)

file = fopen(file_name,'rb');
graph.num = fread(file, 1, 'int');
for i = 1:graph.num
    graph.node(i,1:2) = fread(file,2,'double') + 1;
    graph.node(i,3) = fread(file,1,'int');
    graph.node(i,4:8) = fread(file,5,'int') + 1;
end
fclose(file);
end