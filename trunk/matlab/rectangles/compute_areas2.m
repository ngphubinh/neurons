function A = compute_areas2(IMSIZE, rects, cols)

IMSIZE1 = IMSIZE+[1 1];
A = cell(size(rects));

for f = 1:length(rects)
    
    rectsf = rects{f};
    colsf = cols{f};
    
    %fA = zeros(1,length(rectsf));
    W = 0;
    B = 0;
    
    for j = 1:length(rectsf)
        
        rect = rectsf{j};
        col = colsf(j);
    
        [r c] = ind2sub(IMSIZE1, rect);
        
        if col == 1
            W = W + (r(4)-r(1))  * (c(4)-c(1));
        else
            B = B + (r(4)-r(1))  * (c(4)-c(1));
        end
        %fA(j) = (r(4)-r(1))  * (c(4)-c(1));
    end
    
    %A{f} = fA;
    if W == B
        A{f} = [0 0];   % if they are equal area, set area to 0 so we skip the normalization!
    else
        A{f} = [W B];
    end
    
end

% white areas is first, black is 2nd