train_folder1 = 'C:\Users\charles\Documents\MATLAB\train1';
train_folder2 = 'C:\Users\charles\Documents\MATLAB\train2';
train_folder3 = 'C:\Users\charles\Documents\MATLAB\train3';
train_folder4 = 'C:\Users\charles\Documents\MATLAB\train4';
train_folder5 = 'C:\Users\charles\Documents\MATLAB\train5';
train_folder6 = 'C:\Users\charles\Documents\MATLAB\train6';
train_folder7 = 'C:\Users\charles\Documents\MATLAB\train7';
train_folder8 = 'C:\Users\charles\Documents\MATLAB\train8';
train_folder9 = 'C:\Users\charles\Documents\MATLAB\train9';

test1_folder = 'C:\Users\charles\Documents\MATLAB\test1';
test2_folder = 'C:\Users\charles\Documents\MATLAB\test2';

% Key variables pertaining to recognition rate
pps = 1; % Pictures per subject
rank = 2;
testNumber = 10; % 50 tests per setting
itNumber = 100; % looping from iteration = 1 to iteration = 200

for i = 1 : pps * 10
	train{i} = imread( fullfile(train_folder1, sprintf('%d.pgm',i) )); % CHANGE HERE TOO
end

for i = 1 : 10
	test1{i} = imread( fullfile(test1_folder, sprintf('%d.pgm',i) ));
end

for i = 1 : 10
	test2{i} = imread( fullfile(test2_folder, sprintf('%d.pgm',i) ));
end

train = cat(3, train{:});
test1 = cat(3, test1{:});
test2 = cat(3, test2{:});

F = reshape(train, 10304, pps * 10);
P1 = reshape(test1, 10304, 10);
P2 = reshape(test2, 10304, 10);

V = im2double(F);
v1 = im2double(P1);
v2 = im2double(P2);

vars = {'train_folder', 'test1_folder', 'test2_folder', 'train', 'test1', 'test2', 'F', 'P1', 'P2'}; % Delete unused variables
clear(vars{:})

accuracy_test = zeros(testNumber, 2);
accuracy_mean = zeros(itNumber, 2);

for it = 1 : itNumber	 
for testCounter = 1 : testNumber
    	W = 2 * rand(size(V, 1), rank);
    	H = 2 * rand(rank, size(V, 2));
    	for i = 1 : it
        	W = W .* (V*H' ./ (W*H*H' + eps));
        	H = H .* (W'*V ./ (W'*W*H + eps));
    	end

    	h1 = 2 * rand(rank, size(v1, 2));
    	h2 = h1;
    	for i = 1 : it
        	h1 = h1 .* (W'*v1 ./ (W'*W*h1 + eps));
        	h2 = h2 .* (W'*v2 ./ (W'*W*h2 + eps));
    	end

    	% DISTANCE METRIC: Euclidean distance.
    	dist_euclidean1 = zeros(10, 10 * pps); % storing the euclidean distances of all h-H column pairs (row for h and col for H)
    	dist_euclidean2 = zeros(10, 10 * pps);
    	min_euclidean1 = zeros(10, 2);
    	min_euclidean2 = zeros(10, 2);
    	temp1 = zeros(10302, 1);
    	temp2 = zeros(10302, 1);
    	for j = 1 : 10
        	for k = 1 : 10*pps
            	temp1 = h1(:, j) - H(:, k);
            	temp2 = h2(:, j) - H(:, k);
            	dist_euclidean1(j, k) = norm(temp1);
            	dist_euclidean2(j, k) = norm(temp2);
        	end
        	[min_euclidean1(j, 1), min_euclidean1(j, 2)] = min(dist_euclidean1(j, :)); % find the one with the min/max SSD/Euclidean distances
        	[min_euclidean2(j, 1), min_euclidean2(j, 2)] = min(dist_euclidean2(j, :));
    	end
   	 
           	 
    	for itr = 1 : 10
        	if (min_euclidean1(itr, 2) <= (itr * pps) && min_euclidean1(itr, 2) > ((itr - 1) * pps))
            	accuracy_test(testCounter, 1) = accuracy_test(testCounter, 1) + 0.1;
  	 
        	end
        	if (min_euclidean2(itr, 2) <= (itr * pps) && min_euclidean2(itr, 2) > ((itr - 1) * pps))
            	accuracy_test(testCounter, 2) = accuracy_test(testCounter, 2) + 0.1;
        	end
    	end
	end
	accuracy_mean(it, 1) = mean(accuracy_test(1:testNumber, 1));
	accuracy_mean(it, 2) = mean(accuracy_test(1:testNumber, 2));
	accuracy_test = zeros(testNumber, 2); % reset accuracy counters
	it
end