dir = fileparts(mfilename('fullpath'));
path(strcat(dir, "/../../../../../modules/lib/control/octave/"), path);

pkg load control
pkg load symbolic

# Step 1: start with a continuous time state space model
syms s J b K R L z Ts
A = [-b/J   K/J;
    -K/L   -R/L];
B = [0;
    1/L];
C = [1   0];
D = 0;
sys = ss(A, B, C, D);

# Step 2: transform into a continuous time transfer function
Hz = tf(sys, Ts)

# Step 4: extract the difference equation
[N, D] = numden(Hz)

# Step 5: generate C code
ccode(Hz)

# Step 6: discrete time state space model
syms s J b K R L z Ts
#J = 0.01; b = 0.1; K = 0.01; R = 1; L = 0.5; Ts = 0.01;
A = [-b/J   K/J;
    -K/L   -R/L];
B = [0;
    1/L];
C = [1   0];
D = [0];
#c2d(ss(A, B, C, D), Ts, 'tustin')

#Ad = simplify(sym(inv(Ts * eye(size(A)) - A)*(Ts * eye(size(A)) + A)));
sys = ss(A, B, C, D);
sys = c2d(A, B, C, D, Ts);
display(sys)
ccode(sys)

Ad = [ -500.0/501.0 200.0/201401.0;
			 -4.0/201401.0 -498.0/503.0];
Bd = [ (250.0/7909.0)*pi; (455.0/1438.0)*pi ];
Cd = [1.0/501.0 200.0/201401.0];
Dd = [(250.0/7909.0)*pi];
Ts = 0.01;

#bode(ss(Ad, Bd, Cd, Dd, Ts), c2d(ss(A, B, C, D), Ts))
#input("")
