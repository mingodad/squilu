// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
// contributed by Mike Pall

local function BottomUpTree(item, depth){
  if (depth > 0){
    local i = item + item;
    --depth;
    return [ item, BottomUpTree(i-1, depth),  BottomUpTree(i, depth) ];
  }
  return [ item ];
}

local function ItemCheck(tree){
  if (tree.get(1, false))  return tree[0] + ItemCheck(tree[1]) - ItemCheck(tree[2])
  return tree[0]
}

local start = os.clock()
//check_delayed_release_hooks(false);

local N = vargv.get(1, 14).tointeger();
local mindepth = 4
local maxdepth = mindepth + 2
if (maxdepth < N) maxdepth = N

{
  local stretchdepth = maxdepth + 1
  local stretchtree = BottomUpTree(0, stretchdepth)
  print(format("stretch tree of depth %d\t check: %d",
    stretchdepth, ItemCheck(stretchtree)))
}

local longlivedtree = BottomUpTree(0, maxdepth)

for(local depth=mindepth; depth <= maxdepth; depth += 2){
  local iterations = math.pow(2, (maxdepth - depth + mindepth))
  local check = 0
  for(local i=0; i < iterations; ++i){
    check += ItemCheck(BottomUpTree(1, depth)) +
            ItemCheck(BottomUpTree(-1, depth))
  }
  print(format("%d\t trees of depth %d\t check: %d",
    iterations*2, depth, check))
}

print(format("long lived tree of depth %d\t check: %d\n",
  maxdepth, ItemCheck(longlivedtree)))

print("binary-tree", N, os.clock()-start)