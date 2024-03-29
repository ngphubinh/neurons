function [Cells, tracks, trkSeq, timeSeq, edges] = trkCommonKShortestPaths(CellsList, Cells, TEMPORAL_WIN_SIZE, SPATIAL_WINDOWS_SIZE,...
                                                        MIN_TRACK_LENGTH, NB_BEST_TRACKS, IMAGE_SIZE, DISTANCE_TO_BOUNDARY, NUMBER_OF_BINS,...
                                                        CommonEdges, CommonEdgeWeights, CommonEdgeDistances)

TMAX = length(CellsList);

[tracks, edges] = ksp_matlab(Cells, CellsList, TEMPORAL_WIN_SIZE, SPATIAL_WINDOWS_SIZE, IMAGE_SIZE, DISTANCE_TO_BOUNDARY, NUMBER_OF_BINS, CommonEdges, CommonEdgeWeights, CommonEdgeDistances);

%% pruning the tracks and sorting them according to their MeanGreenIntensity summed over lifetime
tracks = tracks(1:end-2);

for k = 1:max(tracks)
    if ( length(find(tracks == k)) < MIN_TRACK_LENGTH)
        tracks(tracks == k) = 0;
    end
end
utracks = unique(tracks);
for i = 2:length(utracks)
    tracks(tracks == utracks(i)) = i-1;
end

scoreTracks = zeros(1, length(utracks)-1);
for i=1:length(scoreTracks)
    
    list_idx = find(tracks == i);
    for j = 1:length(list_idx)
        scoreTracks(i) = scoreTracks(i) + Cells(list_idx(j)).NucleusMeanGreenIntensity * numel(Cells(list_idx(j)).NucleusPixelIdxList);
    end
end

[scoreTracks, II] = sort(scoreTracks, 'descend');
sortedTracks = zeros(size(tracks));
for i=1:min(NB_BEST_TRACKS, length(scoreTracks))
    sortedTracks(tracks == II(i)) = i;
end
tracks = sortedTracks; 
%% assign ID's to each detections
for t = 1:TMAX
    for d = 1:length(CellsList{t}) % loop through detections in this time step
        detect_ind = CellsList{t}(d);
        Cells(detect_ind).ID = tracks(detect_ind); %#ok<*AGROW>
    end
end


%% get a list of detections and associated time steps for each track
[trkSeq, timeSeq] = trkGetTrackSequences(CellsList, tracks, Cells);