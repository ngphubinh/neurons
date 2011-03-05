
close all;
clear all;

%% Loads the experiments

disp('... loading experiments');
addpath('../');

TRIALS(1) = LoadTrial('/media/data/MICCAI11/', '14-11-2010_%03i.mat',1, 30, 1);

% TRIALS(1) = LoadTrial('/net/cvlabfiler1/home/ksmith/Basel/Results/', '14-11-2010_%03i.mat');
% TRIALS(2) = LoadTrial('/net/cvlabfiler1/home/ksmith/Basel/Results/', '15-11-2010_%03i.mat');
% TRIALS(3) = LoadTrial('/net/cvlabfiler1/home/ksmith/Basel/Results/', '16-11-2010_%03i.mat');
% TRIALS(4) = LoadTrial('/net/cvlabfiler1/home/ksmith/Basel/Results/', '17-11-2010_%03i.mat');

disp('... pfew');



%% Creats an statistical function handle
% statistic = @ffNeuriteLength;
%statistic = @ffMeanGreenIntensitySomata;
% statistic = @ffNumberOfNeuritesPerDetection;
% statistic = @fnMeanNeuriteLength;

% statistic = @ffAreaSomata;
% statistic = @ffAreaNuclei;
% statistic = @ffTravelDistanceNuclei;


% statistic = @ffBranchingPointsPerNeurite;
% statistic = @ffNumberOfNeuritesPerDetection;
statistic = @fnMeanNeuriteLength;


%% Computes the mean value of the function among the experiments of the trial
close all;
LookOnlyAtHappyNeurons = 1;

figure;
CompareFeatureHistogramInExperiments(TRIALS(1), nBins, statistic, LookOnlyAtHappyNeurons);
figure;
[Mean, STD] = GetMeanAndSTDOfTrial(TRIALS(1), statistic, st_arg);
errorbar(1:length(Mean)', Mean, (STD));




%% Compares the statistic in all neurons or only happy ones
close all;
nBins = 100;
LookOnlyAtHappyNeurons = 1;
[fig1, s1] = CompareFeatureHistogramInExperiments(TRIALS(1), nBins, statistic, LookOnlyAtHappyNeurons);
LookOnlyAtHappyNeurons = 0;
[fig2, s2] = CompareFeatureHistogramInExperiments(TRIALS(1), nBins, statistic, LookOnlyAtHappyNeurons);
LookOnlyAtHappyNeurons = 1;

s3 = [min(s1(1), s2(1)), max(s1(2), s2(2)), min(s1(3), s2(3)), max(s1(4), s2(4))];
figure(fig1);
axis(s3);
figure(fig2);
axis(s3);






%% Plots the histogram of the values on the trial
% vals = GetValuesFromExperiment(TRIALS(1).EXPERIMENTS(2), statistic, st_arg);
% figure
% hist(vals, 100);





%% There is still a lot of work to reach this point

% %% Computes the confusion matrix of a trial using the given handle
% ConfMatrix = GetConfusionMatrixOfTrial(TRIALS(1), statistic, st_arg);
% figure;
% imagesc(ConfMatrix);
% title('Confusion Matrix of the Experiments of the Trial');
% 
% %% Computes the confusion matrix of an experiment across trials
% nExperiment = 1;
% ConfMatrix = GetConfusionMatrixOfExperimentAcrossTrials...
%     (TRIALS, nExperiment, statistic, st_arg);
% figure;
% imagesc(ConfMatrix);
% title('Confusion Matrix of the Experiments of the Trial');
% 
% %% Gets the confusion matrix of the runs of an experiment
% ConfMatrix = GetConfusionMatrixOfExperiment...
%     (TRIALS(1).EXPERIMENTS(1), statistic, st_arg);
% figure;
% imagesc(ConfMatrix);
% title('Confusion Matrix of the Runs of an Experiment');
% 









%% Tests the contents of the experiment
% figure;
% for t = 1:1:length(EXP{1}.Dlist)
%     renderData(EXP{1}, t);
%     pause(0.1);
% end
% 
% 
% %% Follows the life of a neuron
% renderNeuronTrack(EXP{1}, 12)
