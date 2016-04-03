// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
// contributed by Mike Pall

array_t BottomUpTree(int_t item, int_t depth){
  //array_t ary = [];//array(3);
  //ary.clear();
   //ary.push_back(item);
  if (depth > 0){
    int_t i = item + item;
    --depth;
    //ary.push_back(BottomUpTree(i-1, depth));
    //ary.push_back(BottomUpTree(i, depth));
    return [ item, BottomUpTree(i-1, depth),  BottomUpTree(i, depth) ];
  }
  //return ary;
  return [ item ];
}

int_t ItemCheck(array_t tree){
  //if (tree.get(1, false))  return tree[0] + ItemCheck(tree[1]) - ItemCheck(tree[2])
  if (tree.len() > 1)  return tree[0] + ItemCheck(tree[1]) - ItemCheck(tree[2])
  return tree[0]
}

int_t main()
{
	double_t start = os.clock()
	//check_delayed_release_hooks(false);

	int_t N = vargv.get(1, 14).tointeger();
	int_t mindepth = 4
	int_t maxdepth = mindepth + 2
	if (maxdepth < N) maxdepth = N

	{
	  int_t stretchdepth = maxdepth + 1
	  array_t stretchtree = BottomUpTree(0, stretchdepth)
	  printf("stretch tree of depth %d\t check: %d\n",
	    stretchdepth, ItemCheck(stretchtree));
	}

	array_t longlivedtree = BottomUpTree(0, maxdepth)

	for(int_t depth=mindepth; depth <= maxdepth; depth += 2){
	  int_t iterations = math.pow(2, (maxdepth - depth + mindepth)).tointeger()
	  int_t check = 0
	  for(int_t i=0; i < iterations; ++i){
	    check += (ItemCheck(BottomUpTree(1, depth))) +
		    (ItemCheck(BottomUpTree(-1, depth)));
	  }
	  printf("%d\t trees of depth %d\t check: %d\n",
	    iterations*2, depth, check);
	}

	printf("long lived tree of depth %d\t check: %d\n",
	  maxdepth, ItemCheck(longlivedtree));

	printf("binary-tree \t%d \t%f\n", N, os.clock()-start);

	return 0;
}

main();