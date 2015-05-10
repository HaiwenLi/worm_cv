function draw_skeleton(graph)

uniform = graph.node;
for i = 1:graph.num
    for j = 1:uniform(i,3)
        nb = uniform(i,3+j);
        plot([uniform(i,2),uniform(nb,2)],[uniform(i,1),uniform(nb,1)],'b-'),hold on;
    end
     if uniform(i,3)~=2 || mod(i,5) == 1
         text(uniform(i,2), uniform(i,1)-0.15 ,num2str(i));
    plot(uniform(i,2),uniform(i,1), 'b.', 'markersize', 5),hold on;
    end
end
end