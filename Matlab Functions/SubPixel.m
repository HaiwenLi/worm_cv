function SubPixel

close all;

%% 读取图片
raw_image = double(imread('sub_pixel_test_raw_image.tif'));
segment_image = double(imread('sub_pixel_test_segment_image.tif'));
%% 计算整数点处相关系数：

[raw_matrix_1,raw_matrix_2] = size(raw_image);
[seg_matrix_1,seg_matrix_2] = size(segment_image);

segment_image_mean=sum(sum(segment_image))/(seg_matrix_1*seg_matrix_2);
segment_image=segment_image-segment_image_mean;
segment_image2= sqrt(sum(sum(segment_image.^2)));

raw_image_expansion = zeros(raw_matrix_1+2*(seg_matrix_1-1),raw_matrix_2+2*(seg_matrix_2-1));
raw_image_expansion(seg_matrix_1:seg_matrix_1+raw_matrix_1-1,seg_matrix_2:seg_matrix_2+raw_matrix_2-1) = raw_image;

corr_matrix = zeros(raw_matrix_1+seg_matrix_1-1, raw_matrix_2+seg_matrix_2-1);
for i = 1 : raw_matrix_1+seg_matrix_1-1
    for j = 1 : raw_matrix_2+seg_matrix_2-1
        raw_seg = raw_image_expansion(i:i+seg_matrix_1-1,j:j+seg_matrix_2-1);
        raw_seg_mean=sum(sum(raw_seg))/(seg_matrix_1*seg_matrix_2);
        raw_seg=raw_seg-raw_seg_mean;
        raw_seg2 = sqrt(sum(sum(raw_seg.^2)));
        corr_matrix(i,j) = sum(sum(raw_seg.*segment_image))/(raw_seg2*segment_image2);
    end
end

[matchY,matchX] = find(corr_matrix == max(max(corr_matrix)));

fprintf('Matching point is calibrated at [%d %d].\n',matchY,matchX);
figure,
imagesc(corr_matrix);
colorbar;
axis equal

%% 完全二次曲面插值(二选一)
%%%%%%%%%%%%%%%%%%%%直接计算亚像素点处的相关系数%%%%%%%%%%%%%%%%%%%%
tol = 0.001;
Z = corr_matrix(matchY-1:matchY+1,matchX-1:matchX+1);
[X,Y] = meshgrid(matchX-1:matchX+1,matchY-1:matchY+1);

tic
% 
% [X1,Y1] = meshgrid(matchX-1:tol:matchX+1,matchY-1:tol:matchY+1);
% Z1 = interp2(X,Y,Z,X1,Y1,'spline');
% [subX,subY] = find(Z1 == max(max(Z1)));
% % surf(X,Y,Z),hold on,surf(X1,Y1,Z1), alpha(0.5);
% fprintf('Matching point in sub-pixel precision by “完全二次曲面插值” is calibrated at [%.8f %.8f].\n',Y1(subX,subY),X1(subX,subY));
% 
% % figure,imagesc(Z1),colorbar,axis equal;

toc

%%%%%%%%%%%%%%%%%%%%迭代法计算亚像素点处的相关系数%%%%%%%%%%%%%%%%%%%%
tic

x = matchX;
y = matchY;

Z = corr_matrix(matchY-1:matchY+1,matchX-1:matchX+1);
[X,Y] = meshgrid(matchX-1:matchX+1,matchY-1:matchY+1);

step = 1;
while(step > tol)
    step = step/2;
    [X2,Y2] = meshgrid(x-2*step:step:x+2*step,y-2*step:step:y+2*step);     

    Z2 = interp2(X,Y,Z,X2,Y2,'spline');
    [subX,subY] = find(Z2 == max(max(Z2)));
    x = X2(subX,subY);
    y = Y2(subX,subY);
    if(subX == 1)
        X = X2(subX:subX+1,subY-1:subY+1);
        Y = Y2(subX:subX+1,subY-1:subY+1);
        Z = Z2(subX:subX+1,subY-1:subY+1);
    elseif(subX == 5)
        X = X2(subX-1:subX,subY-1:subY+1);
        Y = Y2(subX-1:subX,subY-1:subY+1);
        Z = Z2(subX-1:subX,subY-1:subY+1);
    elseif(subY == 1)
        X = X2(subX-1:subX+1,subY:subY+1);
        Y = Y2(subX-1:subX+1,subY:subY+1);
        Z = Z2(subX-1:subX+1,subY:subY+1);
    elseif(subY == 5)
        X = X2(subX-1:subX+1,subY-1:subY);
        Y = Y2(subX-1:subX+1,subY-1:subY);
        Z = Z2(subX-1:subX+1,subY-1:subY);
    else 
        X = X2(subX-1:subX+1,subY-1:subY+1);
        Y = Y2(subX-1:subX+1,subY-1:subY+1);
        Z = Z2(subX-1:subX+1,subY-1:subY+1);
    end
end

fprintf('Matching point in sub-pixel precision by “完全二次曲面插值” is calibrated at [%.8f %.8f].\n',y,x);

% figure,imagesc(Z2),colorbar,axis equal;

toc

%% 对称二次曲线插值

if corr_matrix(matchY+1,matchX) > corr_matrix(matchY-1,matchX)
    corr_subpixel_y = (corr_matrix(matchY+1,matchX) - corr_matrix(matchY-1,matchX))...
        /(corr_matrix(matchY+1,matchX) - corr_matrix(matchY-1,matchX) + corr_matrix(matchY,matchX) - corr_matrix(matchY+2,matchX));
else
    corr_subpixel_y = (corr_matrix(matchY+1,matchX) - corr_matrix(matchY-1,matchX))...
        /(corr_matrix(matchY-1,matchX) - corr_matrix(matchY+1,matchX) + corr_matrix(matchY,matchX) - corr_matrix(matchY-2,matchX));
end

if corr_matrix(matchY,matchX+1) > corr_matrix(matchY,matchX-1)
    corr_subpixel_x = (corr_matrix(matchY,matchX+1) - corr_matrix(matchY,matchX-1))...
        /(corr_matrix(matchY,matchX+1) - corr_matrix(matchY,matchX-1) + corr_matrix(matchY,matchX) - corr_matrix(matchY,matchX+2));
else
    corr_subpixel_x = (corr_matrix(matchY,matchX+1) - corr_matrix(matchY,matchX-1))...
        /(corr_matrix(matchY,matchX-1) - corr_matrix(matchY,matchX+1) + corr_matrix(matchY,matchX) - corr_matrix(matchY,matchX-2));
end

fprintf('Matching point in sub-pixel precision by “对称二次曲线插值” is calibrated at [%.4f %.4f].\n',matchY+corr_subpixel_y,matchX+corr_subpixel_x);

%% 对称二次曲面插值

tol = 2 * 0.005^2;

corr_matrix_center = corr_matrix(matchY-2:matchY+2,matchX-2:matchX+2);
[X_center Y_center] = meshgrid(-2:2,-2:2);

figure,
surf(X_center,Y_center,corr_matrix_center)

coeffient = [0 0 0 0 0 0 0 0]';
% coeffient = unifrnd(-0.1,0.1,8,1);
% coeffient = [1 -0.5 -0.5 0 0 0 0 0]';

for i = 1 : 50
    diff_1 = ones(5,5);
    diff_2 = abs(X_center-coeffient(7));
    diff_3 = abs(Y_center-coeffient(8));
    diff_4 = (X_center-coeffient(7)).^2;
    diff_5 = (Y_center-coeffient(8)).^2;
    diff_6 = abs(X_center-coeffient(7)) .* abs(Y_center-coeffient(8));
    diff_7 = - coeffient(2) * ((X_center - coeffient(7))./abs(X_center - coeffient(7) + eps)) + ...
           2 * coeffient(4) * (X_center - coeffient(7)) - ...
               coeffient(6) * ((X_center - coeffient(7))./abs(X_center - coeffient(7) + eps)) .* abs(Y_center - coeffient(8));
    diff_8 = - coeffient(3) * ((Y_center - coeffient(8))./abs(Y_center - coeffient(8) + eps)) + ...
           2 * coeffient(5) * (Y_center - coeffient(8)) - ...
               coeffient(6) * ((Y_center - coeffient(8))./abs(Y_center - coeffient(8) + eps)) .* abs(X_center - coeffient(7));

    delta_corr =  coeffient(1) * diff_1 + ...
                  coeffient(2) * diff_2 + ...
                  coeffient(3) * diff_3 + ...
                  coeffient(4) * diff_4 + ...
                  coeffient(5) * diff_5 + ...
                  coeffient(6) * diff_6 - ...
                  corr_matrix_center;

    diff_matrix = [diff_1(:) diff_2(:) diff_3(:) diff_4(:) diff_5(:) diff_6(:) diff_7(:) diff_8(:)];
    temp = diff_matrix \ delta_corr(:);
    coeffient = coeffient + temp;
    if i>2 && sum(temp(7:8).^2) < tol
        break;
    end
end

fprintf('Matching point in sub-pixel precision by “对称二次曲面插值” is calibrated at [%.4f %.4f].\n',matchY+coeffient(8),matchX+coeffient(7));

%% 对称二次曲面插值 f(x,y) = a0 + a1*x + a2*y + a3*x^2 + a4*y^2

Z = corr_matrix(matchY-1:matchY+1,matchX-1:matchX+1);
[X,Y] = meshgrid(matchX-1:matchX+1,matchY-1:matchY+1);

coeff_0 = ones(3);
coeff_1 = X;
coeff_2 = Y;
coeff_3 = X.^2;
coeff_4 = Y.^2;
coeff_matrix = [coeff_0(:) coeff_1(:) coeff_2(:) coeff_3(:) coeff_4(:)];
a_coeffient = coeff_matrix \ Z(:);

fprintf('Matching point in sub-pixel precision by “对称二次曲面插值1” is calibrated at [%.4f %.4f].\n',-a_coeffient(3)/(2*a_coeffient(5)),-a_coeffient(2)/(2*a_coeffient(4)));

%% 对称二次曲面插值 log(f(x,y)) = a0 + a1*x + a2*y + a3*x^2 + a4*y^2

Z = corr_matrix(matchY-1:matchY+1,matchX-1:matchX+1);
[X,Y] = meshgrid(matchX-1:matchX+1,matchY-1:matchY+1);

coeff_0 = ones(3);
coeff_1 = X;
coeff_2 = Y;
coeff_3 = X.^2;
coeff_4 = Y.^2;
coeff_matrix = [coeff_0(:) coeff_1(:) coeff_2(:) coeff_3(:) coeff_4(:)];
a_coeffient = coeff_matrix \ log(Z(:));

fprintf('Matching point in sub-pixel precision by “对称二次曲面插值2” is calibrated at [%.4f %.4f].\n',-a_coeffient(3)/(2*a_coeffient(5)),-a_coeffient(2)/(2*a_coeffient(4)));
