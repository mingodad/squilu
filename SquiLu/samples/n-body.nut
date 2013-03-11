// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
// contributed by Mike Pall
// modified by Geoff Leyland

local sqrt = math.sqrt

local PI = 3.141592653589793
local SOLAR_MASS = 4 * PI * PI
local DAYS_PER_YEAR = 365.24
local bodies = [
  { // Sun
    x = 0,
    y = 0,
    z = 0,
    vx = 0,
    vy = 0,
    vz = 0,
    mass = SOLAR_MASS
  },
  { // Jupiter
    x = 4.84143144246472090e+00,
    y = -1.16032004402742839e+00,
    z = -1.03622044471123109e-01,
    vx = 1.66007664274403694e-03 * DAYS_PER_YEAR,
    vy = 7.69901118419740425e-03 * DAYS_PER_YEAR,
    vz = -6.90460016972063023e-05 * DAYS_PER_YEAR,
    mass = 9.54791938424326609e-04 * SOLAR_MASS
  },
  { // Saturn
    x = 8.34336671824457987e+00,
    y = 4.12479856412430479e+00,
    z = -4.03523417114321381e-01,
    vx = -2.76742510726862411e-03 * DAYS_PER_YEAR,
    vy = 4.99852801234917238e-03 * DAYS_PER_YEAR,
    vz = 2.30417297573763929e-05 * DAYS_PER_YEAR,
    mass = 2.85885980666130812e-04 * SOLAR_MASS
  },
  { // Uranus
    x = 1.28943695621391310e+01,
    y = -1.51111514016986312e+01,
    z = -2.23307578892655734e-01,
    vx = 2.96460137564761618e-03 * DAYS_PER_YEAR,
    vy = 2.37847173959480950e-03 * DAYS_PER_YEAR,
    vz = -2.96589568540237556e-05 * DAYS_PER_YEAR,
    mass = 4.36624404335156298e-05 * SOLAR_MASS
  },
  { // Neptune
    x = 1.53796971148509165e+01,
    y = -2.59193146099879641e+01,
    z = 1.79258772950371181e-01,
    vx = 2.68067772490389322e-03 * DAYS_PER_YEAR,
    vy = 1.62824170038242295e-03 * DAYS_PER_YEAR,
    vz = -9.51592254519715870e-05 * DAYS_PER_YEAR,
    mass = 5.15138902046611451e-05 * SOLAR_MASS
  }
]

local function advance(bodies, nbody, dt){
  for(local i=0; i < nbody; ++i){
    local bi = bodies[i]
    local bix = bi.x, biy = bi.y, biz = bi.z, bimass = bi.mass
    local bivx = bi.vx, bivy = bi.vy, bivz = bi.vz
    for(local j=i+1; j < nbody; ++j){
      local bj = bodies[j]
      local dx = bix-bj.x, dy = biy-bj.y, dz = biz-bj.z
      local mag = sqrt(dx*dx + dy*dy + dz*dz)
      mag = dt / (mag * mag * mag)
      local bm = bj.mass*mag
      bivx -= (dx * bm)
      bivy -= (dy * bm)
      bivz -= (dz * bm)
      bm = bimass*mag
      bj.vx += (dx * bm)
      bj.vy += (dy * bm)
      bj.vz += (dz * bm)
    }
    bi.vx = bivx
    bi.vy = bivy
    bi.vz = bivz
    bi.x = bix + dt * bivx
    bi.y = biy + dt * bivy
    bi.z = biz + dt * bivz
  }
}

local function energy(bodies, nbody){
  local e = 0
  for(local i=0; i < nbody; ++i){
    local bi = bodies[i]
    local vx = bi.vx, vy = bi.vy, vz = bi.vz, bim = bi.mass
    e += (0.5 * bim * (vx*vx + vy*vy + vz*vz))
    for(local j=i+1; j < nbody; ++j){
      local bj = bodies[j]
      local dx = bi.x-bj.x, dy = bi.y-bj.y, dz = bi.z-bj.z
      local distance = sqrt(dx*dx + dy*dy + dz*dz)
      e -= ((bim * bj.mass) / distance)
    }
  }
  return e
}

local function offsetMomentum(b, nbody){
  local px = 0.0, py = 0.0, pz = 0.0
  for(local i=0; i < nbody; ++i){
    local bi = b[i]
    local bim = bi.mass
    px = px + (bi.vx * bim)
    py = py + (bi.vy * bim)
    pz = pz + (bi.vz * bim)
  }
  local b0 = b[0]
  b0.vx = -px / SOLAR_MASS
  b0.vy = -py / SOLAR_MASS
  b0.vz = -pz / SOLAR_MASS
}

local start = os.clock()
local N = vargv.get(1, 100000).tointeger();
local nbody = bodies.len();

offsetMomentum(bodies, nbody)
print( format("%0.9f",energy(bodies, nbody)))
for(local i=0; i < N; ++i) advance(bodies, nbody, 0.01) 
print( format("%0.9f",energy(bodies, nbody)))
print("n-body", N, os.clock()-start)
