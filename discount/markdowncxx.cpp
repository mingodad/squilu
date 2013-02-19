#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <iostream>
extern "C" {
#include "markdown.h"
}

static void mywrite(char const *s,int len,void *str_ptr)
{
	std::string *cxx_str=static_cast<std::string *>(str_ptr);
	cxx_str->append(s,len);
}

void markdown2html(std::string const &in,std::string &out)
{
	markdown_str(mkd_string(const_cast<char *>(in.c_str()),in.size(),0),&out,mywrite,0);
}

void markdown2html(const char *in, int len, std::string &out)
{
	markdown_str(mkd_string(in, len, 0), &out,mywrite,0);
}

void markdown2html(const char *in, std::string &out)
{
	markdown2html(in, strlen(in), out);
}

#ifdef TEST_MARKDOWN_LIBRARY
int main()
{
	std::string test;
	test.reserve(32000);
	int c;
	while((c=getchar())!=EOF) {
		test+=(char)c;
	}

	std::string out;
	out.reserve(32000);
	markdown2html(test,out);
	std::cout<<out;
	return 0;
}
#endif
