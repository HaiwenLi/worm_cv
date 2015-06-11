function Rect_Plot(rect, color)
hold on;
left = rect(2);
right = rect(2) + rect(4);
top = rect(1);
bottom = rect(1) + rect(3);
X_line = [left, right, right, left, left]+1;
Y_line = [top, top, bottom, bottom, top]+1;
plot(X_line, Y_line, color,'linewidth',1.5);