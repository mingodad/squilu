// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// contributed by Ian Osgood
// modified by Isaac Gouy

local function A(i,j) {
  local ij = i+j;
  return 1.0/(ij * (ij+1)/2.0+i+1);
}

local function Au(u,v, N) {
  for (var i=0; i<N; ++i) {
    var t = 0.0;
    for (var j=0; j<N; ++j) t += A(i,j) * u[j];
    v[i] = t;
  }
}

local function Atu(u,v, N) {
  for (var i=0; i<N; ++i) {
    var t = 0.0;
    for (var j=0; j<N; ++j) t += A(j,i) * u[j];
    v[i] = t;
  }
}

local function AtAu(u,v,w, N) {
  Au(u,w, N);
  Atu(w,v, N);
}

local function spectralnorm(n) {
  var i, u=array(n), v=array(n), w=array(n), vv=0.0, vBv=0.0;
  for (i=0; i<n; ++i) {
    u[i] = 1.0; v[i] = w[i] = 0.0; 
  }
  for (i=0; i<10; ++i) {
    AtAu(u,v,w, n);
    AtAu(v,u,w, n);
  }
  for (i=0; i<n; ++i) {
    vBv += u[i]*v[i];
    vv  += v[i]*v[i];
  }
  return math.sqrt(vBv/vv);
}

print(check_delayed_release_hooks());
//check_delayed_release_hooks(false);
call_delayed_release_hooks();
print(check_delayed_release_hooks());

local N = vargv.get(1, 500).tointeger();
local start = os.clock();
local result = spectralnorm(N);
print("spectralnorm", N, os.clock()-start);
print(format("%d = %0.9f\n", N, result));
