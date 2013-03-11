// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
// contributed by Mike Pall

local function A(i, j){
  local ij = j + i++
  return 1.0/(ij++ * ij/2.0+i);
}

local function Av(x, y, N){
  for(local i=0; i < N; ++i){
    local a = 0
    for(local j=0; j < N; ++j) a += x[j] * A(i, j)
    y[i] = a
  }
}

local function Atv(x, y, N){
  for(local i=0; i < N; ++i){
    local a = 0
    for(local j=0; j < N; ++j) a += x[j] * A(j, i)
    y[i] = a
  }
}

local function AtAv(x, y, t, N){
  Av(x, t, N)
  Atv(t, y, N)
}

local N = vargv.get(1, 500).tointeger();

local start = os.clock()

local u = array(N), v = array(N), t = array(N)
for(local i=0; i < N; ++i) u[i] = 1

for(local i=0; i < 10; ++i){
	AtAv(u, v, t, N)
	AtAv(v, u, t, N)
}

local vBv = 0.0, vv = 0.0
for(local i=0; i < N; ++i){
  local ui = u[i], vi = v[i]
  vBv += ui*vi
  vv += vi*vi
}

local result = math.sqrt(vBv / vv);
print("spectralnorm", N, os.clock()-start)
print(format("%0.9f\n", result))
