{
  enum option_type { OPT_FLAG=1, OPT_INT};
  option_type opt = option_type.OPT_FLAG;
  print("opt", opt, option_type.OPT_INT);
  print(type(opt));
}
{
	enum bug {ba, bs="bug", bc};
	print(bug.ba, bug.bs, bug.bc);
}
{
	enum bug2 {bs="bug", ba, bc};
	print(bug2.ba, bug2.bs, bug2.bc);
}