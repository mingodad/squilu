#ifndef MARKDOWNCXX_H
#define MARKDOWNCXX_H
#include <string>

void markdown2html(std::string const &in,std::string &out);
void markdown2html(const char *in, int len, std::string &out);
void markdown2html(const char *in, std::string &out);

#endif
