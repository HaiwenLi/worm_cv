function draw_skeleton(graph,color)

if nargin<2
    color='b';
end
ecolor = strcat(color,'-');
vcolor = strcat(color,'.');

uniform = graph.node;
for i = 1:graph.num
    for j = 1:uniform(i,3)
        nb = uniform(i,3+j);
        plot([uniform(i,2),uniform(nb,2)],[uniform(i,1),uniform(nb,1)],ecolor),hold on;
    end
    if uniform(i,3)~=2 || mod(i,4) == 1
        plot(uniform(i,2),uniform(i,1), vcolor, 'markersize', 5),hold on;
        text(uniform(i,2), uniform(i,1)-0.15 ,num2str(i));
    end
end
axis equal;
end