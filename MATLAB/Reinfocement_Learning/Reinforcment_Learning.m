


% Charles Kinzel
% Reinforcement Learning: Gridworld Problem

clear all;
% User input for grid world, start, goal and obstacles
% Gridworld
disp("Enter dimensions of game world.")
m = input("Rows: ");
n = input("Columns: ");
%Start point
disp("Enter start point.")
s_row = input("Row: ");
s_col = input("Col: ");
s = [s_row s_col];
% Goal
disp("Enter goal.")
m_goal = input("Row: ");
n_goal = input("Col: ");
goal = (m_goal - 1)*n + n_goal;
% Reward vector
R = ones(1, m*n)*-1;
R(goal) = 100;

% Set punishments
again = input("Add an obstacle?(1 for yes, 0 for no: ");
while again
  disp("Enter obstacle.")
  m_ob = input("Row: ");
  n_ob = input("Col: ");
  ob = (m_ob - 1)*n + n_ob;
  R(ob) = -100;
  again = input("Add another obstacle?(1 for yes, 0 for no: ");
end

% variable definitions
iteration = 1000;
gamma = 0.8;
alpha = 0.8;
up = 1;
down = 2;
left = 3;
right = 4;
%%%%%%%%%%%%%%%%%%%%% Auto start params %%%%%%%%%%%%%%%
%m = 5;
%n = 5;
%R  = [-1,-1,-1,-1,-10,10];
%R  = [-1,-1,-1,-1,-1,-1,-1,-100,-100,-1,-1,-1,-100,100,-1,-1,-1,-100,-100,-1,-1,-1,-1,-1,-1];
%s_row = 1;
%s_col = 1;
%s = [1, 1];
%goal = 14;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% matrix definitions
Q_matrix = zeros(m*n, 4);
R_table = [0 -1 0 -1; 0 -10 -1 -1; 0 10 -1 0; -1 0 0 -10; -1 0 -1 10; -1 0 -10 0];

% Generate a new state given a random action
function ns = newState(s, a)  
   x = s(1, 1);
   y = s(1, 2);
   
   if (a == 1)
     x = x - 1;
     ns = [x y];
   elseif a == 2
     x = x + 1;
     ns = [x y];
   elseif a == 3
     y = y - 1;
     ns = [x y];
   else
     y = y + 1;
     ns = [x y];
   end
end

% Generate a random action
function a = action(s, m, n)
    
    % action menu
    l_side_action = [1 2 4];
    r_side_action = [1 2 3];
    t_side_action = [2 3 4];
    b_side_action = [1 3 4];
    tlc_action = [2 4];
    trc_action = [2 3];
    brc_action = [1 3];
    blc_action = [1 4];
    center_action = [1 2 3 4];
    x = s(1, 1);
    y = s(1, 2);
    
    if x == 1 && y == 1
        a = tlc_action(1, (randi([1 2])));
    elseif x == 1 && y == n
        a = trc_action(1, (randi([1 2])));
    elseif x == m && y == n
        a = brc_action(1, (randi([1 2])));
    elseif x == m && y == 1
        a = blc_action(1, (randi([1 2])));
        
    elseif x == 1
        a = t_side_action(1, (randi([1 3])));
    elseif x == m
        a = b_side_action(1, (randi([1 3])));
    elseif y == n
        a = r_side_action(1, (randi([1 3])));
    elseif y == 1
        a = l_side_action(1, (randi([1 3])));
        
    else
        a = center_action(1, (randi([1 4])));
    end
end

% Maximum value for available actions of the next state
function mx = maximum(s, elem, m, n, Q_matrix)
 
    x = s(1, 1);
    y = s(1, 2);
    
    if x == 1 && y == 1
        mx = max(Q_matrix(elem, 2), Q_matrix(elem, 4));
    elseif x == 1 && y == n
        mx = max(Q_matrix(elem, 2), Q_matrix(elem, 3));
    elseif x == m && y == n
        mx = max(Q_matrix(elem, 1), Q_matrix(elem, 3));
    elseif x == m && y == 1
        mx = max(Q_matrix(elem, 2), Q_matrix(elem, 4));        
    elseif x == 1
        W = [Q_matrix(elem, 2) Q_matrix(elem, 3) Q_matrix(elem, 4)];
        mx = max(W);
    elseif x == m
        W = [Q_matrix(elem, 1) Q_matrix(elem, 3) Q_matrix(elem, 4)];
        mx = max(W);
    elseif y == n
        W = [Q_matrix(elem, 1) Q_matrix(elem, 2) Q_matrix(elem, 3)];
        mx = max(W);
    elseif y == 1
        W = [Q_matrix(elem, 1) Q_matrix(elem, 2) Q_matrix(elem, 4)];
        mx = max(W);        
    else
        W = [Q_matrix(elem, 1) Q_matrix(elem, 2) Q_matrix(elem, 3) Q_matrix(elem, 4)];
        mx = max(W);
    end
end

for i=1:iteration
    
    x = s(1, 1);
    y = s(1, 2);
    loc = (x - 1)*n + y;
    
    move = action(s, m, n);
    next = newState(s, move);
    h = next(1, 1);
    k = next(1, 2);
    loc_next = (h - 1)*n + k;
    % main algorithm
    V = (1 - alpha)*Q_matrix(loc, move) + alpha*(R(loc_next) + gamma*maximum(next, loc_next, m, n, Q_matrix));
    if (V > Q_matrix(loc, move))
      Q_matrix(loc, move) = V;
    end
    s = next;
    if mod(i, 100) == 0
      Q_matrix
    end
    if loc_next == goal
      s = [s_row s_col];
    end
end

% Display Solution
state = [s_row s_col];
gw = zeros(m,n);
pos = 0;

while (pos != goal)
  x = state(1, 1);
  y = state(1, 2);
  pos = (x - 1)*n + y;
  gw(x, y) = 50;
  [val,loc] = max(Q_matrix(pos,:));
  state = newState(state, loc);
endwhile

image(gw);