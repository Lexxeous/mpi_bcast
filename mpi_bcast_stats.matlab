%JONATHAN ALEXANDER GIBSON
%CSC 6740
%PARALLEL DISTRIBUTED ALGORITHMS
%DR. GHAFOOR
%PROGRAM 5 (MPI BCAST COMPARISON)


clear %clear all saved variable values
clc %clear the command window
close all %close all figures
format long %long variable format
%--------------------------------------------------------------------------

procs = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 26, 28]; %x-axis for all graphs

%(i) Custom Double Node Multiplicative Time Increase Per Total Processor Count
yMult1 = [1,35.85,45.23,487.52,80.58,188.7,125.4,231.9,177.93,257.76,362.1,356.47];
figure;
subplot(2, 2, 1);
plot(procs, yMult1, 'o-', 'LineWidth', 2, 'MarkerSize', 8);
grid on;
ylim([1, 500]);
xlim([2, 28]);
ylabel('Multiplicative Time Increase');
xlabel('Total Processor Count');
title({'Custom Double Node MPI', 'Broadcast Implementation'});
%--------------------------------------------------------------------------

%(ii) Default Double Node Multiplicative Time Increase Per Total Processor Count
yMult2 = [1,6.22,2.67,76.88,5.47,4.79,3.99,52.37,3.2,3.37,3.74,3.7];
subplot(2, 2, 2);
plot(procs, yMult2, 'o-', 'LineWidth', 2, 'MarkerSize', 8);
grid on;
ylim([1, 500]);
xlim([2, 28]);
ylabel('Multiplicative Time Increase');
xlabel('Total Processor Count');
title({'Default Double Node MPI', 'Broadcast Implementation'});
%--------------------------------------------------------------------------

%(iii) Custom Single Node Multiplicative Time Increase Per Total Processor Count
yMult3 = [1,2.25,3.15,4.59,6.75,8.57,12.29,20.95,21.22,22.18,27.08,33.41];
subplot(2, 2, 3);
plot(procs, yMult3, 'o-', 'LineWidth', 2, 'MarkerSize', 8);
grid on;
ylim([1, 35]);
xlim([2, 28]);
ylabel('Multiplicative Time Increase');
xlabel('Total Processor Count');
title({'Custom Single Node MPI', 'Broadcast Implementation'});
%--------------------------------------------------------------------------

%(iv) Default Single Node Multiplicative Time Increase Per Total Processor Count
yMult4 = [1,2.69,3,3.29,3.54,4.05,6.35,9.4,9.54,9.06,10.53,11.19];
subplot(2, 2, 4);
plot(procs, yMult4, 'o-', 'LineWidth', 2, 'MarkerSize', 8);
grid on;
ylim([1, 35]);
xlim([2, 28]);
ylabel('Multiplicative Time Increase');
xlabel('Total Processor Count');
title({'Default Single Node MPI', 'Broadcast Implementation'});
%--------------------------------------------------------------------------