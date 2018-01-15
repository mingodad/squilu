//#include "test-ifdef.nut"

# define DEBUG

print("Top !");
#ifndef ANY
	print("Any !");
	#ifdef SOME
		print("Some !");
		#define ANY3
		#ifdef NESTED
			print("Some nested !");
			#ifdef NESTED2
				print("Some nested2 !");
			#else
				print("Else Some nested2 !");
			#endif
		#else
			#ifdef NESTED3
				print("Some nested3 !");
			#else
				print("Else Some nested3 !");
			#endif
			print("Else Some nested !");
		#endif
	#endif
	print("Another any !");
	#ifndef ANY2
		print("Any2 !");
		#ifdef ANY3
			print("Any3 !");
		#else
			print("Esle Any3 !");
		#endif
		#ifndef ANY4
			print("Any4 !");
		#else
			print("Esle Any4 !");
		#endif
		print("Another any2 !");
	#endif
#endif
#ifdef DEBUG
	print("Debug code");
#else
	print("Non debug code");
#endif

#undef DEBUG

#ifdef DEBUG
	print("Debug code2");
#else
	print("Non debug code2");
#endif

print("Botton !");
