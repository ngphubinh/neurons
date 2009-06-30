function SET = p_collect_data(DATASETS, set_type)
%ADA_COLLECT_DATA organizes training images for viola-jones
%
%   SET = ada_collect_data(DATASETS, set_type) collects and processes 
%   training images specified by DATASETS and the set_type ('train' or
%   'validation'). Returns the structure SET containing the images
%   comprising the data set.
%
%   example:
%   ------------
%   TRAIN       = ada_collect_data(DATASETS, 'train');
%   VALIDATION  = ada_collect_data(DATASETS, 'validation')
%
%   Copyright 2009 Kevin Smith
%
%   See also P_RECOLLECT_DATA, P_TRAININGFILES
 

%% collect parameters from DATASETS 
% NORM = normalize images?, IMSIZE = training image size, POS_LIM = # positive examples, NEG_LIM = # of negative examples.

[NORM IMSIZE POS_LIM NEG_LIM] = collect_arguments(DATASETS, set_type);
count = 1;

%% collect POSITIVE (c = 1) and NEGATIVE (c = 2) example images into SET

%SET.Images = zeros([IMSIZE POS_LIM+NEG_LIM]);	% initialize with empty images

for c = 1:2  % c = the postive and negative classes
    
    % collect the training image files into d, and initialize the data struct
    if c == 1
        d = p_trainingfiles(DATASETS.filelist, set_type, '+', POS_LIM);
    else
        d = p_trainingfiles(DATASETS.filelist, set_type, '-', NEG_LIM);
    end

    % add each image file to SET, format it, normalize it, and compute features
    for i = 1:length(d)
        % read the file
        filenm = d{i};
        I = imread(filenm);

%         % convert to proper class (pixel intensity represented by [0,1])
%         if ~isa(I, 'double')
%             cls = class(I);
%             I = mat2gray(I, [0 double(intmax(cls))]); 
%         end
% 
%         % convert to grasyscale if necessary
%         if size(I,3) > 1
%             I = rgb2gray(I);
%         end
% 
%         % resize to standard size
%         if ~isequal(size(I), IMSIZE)
%             I = imresize(I, IMSIZE);
%         end
% 
%         % normalize the image intensity if necessary
%         if NORM
%             I = imnormalize('image', I);
%         end

        % store the image into SET
        %SET.Images(:,:,count) = I;
        SET.Images{count} = I;
        SET.IntImages{count} = integral_image(I)';
        SET.filename{count} = filenm;
        if c == 1; SET.class(count) = 1; end
        if c == 2; SET.class(count) = -1; end

        count = count + 1;       
    end
    SET.database = DATASETS.filelist;
end

% in case there were fewer examples then specified, remove blank examples
%SET.Images = SET.Images(:,:,1:count-1);







function [NORM IMSIZE POS_LIM NEG_LIM] = collect_arguments(DATASETS, set_type)

%% define default parameters
NORM = 1; IMSIZE = [24 24];
POS_LIM = Inf;NEG_LIM = Inf;

%% collect predefined arguments
if isfield(DATASETS, 'IMSIZE')
    if ~isempty(DATASETS.IMSIZE)
        IMSIZE = DATASETS.IMSIZE;
    end
end

if isfield(DATASETS, 'NORMALIZE')
    if ~isempty(DATASETS.NORMALIZE)
        NORM = DATASETS.NORMALIZE;
    end
end

if strcmp(set_type,'train')
    if isfield(DATASETS, 'TRAIN_POS')
        if ~isempty(DATASETS.TRAIN_POS)
            POS_LIM = DATASETS.TRAIN_POS;
        end
    end
    if isfield(DATASETS, 'TRAIN_NEG')
        if ~isempty(DATASETS.TRAIN_NEG)
            NEG_LIM = DATASETS.TRAIN_NEG;
        end
    end
end
    
if strcmp(set_type,'validation')
    if isfield(DATASETS, 'VALIDATION_POS')
        if ~isempty(DATASETS.VALIDATION_POS)
            POS_LIM = DATASETS.VALIDATION_POS;
        end
    end
    if isfield(DATASETS, 'VALIDATION_NEG')
        if ~isempty(DATASETS.VALIDATION_NEG)
            NEG_LIM = DATASETS.VALIDATION_NEG;
        end
    end
end



