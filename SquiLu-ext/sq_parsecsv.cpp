#include "squirrel.h"
#include <string.h>
SQ_OPT_STRING_STRLEN();

#include <fstream>
#include <istream>
#include <string>
#include <vector>
/// <summary>loads a CSV record from the stream is</summary>
/// <remarks>
/// * leading and trailing white space is removed outside of
//    quoted sections when trimWhiteSpace is true
/// * line breaks are preserved in quoted sections
/// * quote literals consist of two adjacent quote characters
/// * quote literals must be in quoted sections
/// </remarks>
/// <param name=is>input stream for CSV records</param>
/// <param name=trimWhiteSpace>trims white space on unquoted fields</param>
/// <param name=fieldDelim>field delimiter. defaults to ',' for CSV</param>
/// <param name=recordDelim>record delimiter. defaults to '\n' for CSV</param>
/// <param name=quote>delimiter for quoted fields. defaults to '"'</param>
/// <returns>a list of fields in the record</returns>
std::vector<std::string> CsvGetLine(std::istream& is,
                                    bool trimWhiteSpace=true,
                                    const char fieldDelim=',',
                                    const char recordDelim='\n',
                                    const char quote='"')
{
    using namespace std;
    vector<string> record; // result record list. default empty
    string field;          // temporary field construction zone
    int start = -1,        // start of a quoted section for trimming
        end = -1;          // end of a quoted section for trimming
    char ch;
    while (is.get(ch))
    {
        if (ch == fieldDelim || ch == recordDelim)
            // fieldDelim and recordDelim mark the end of a
            // field. save the field, reset for the next field,
            // and break if there are no more fields
        {
            if (trimWhiteSpace)
                // trim all external white space
                // exclude chars between start and end
            {
                const string wsList = " \t\n\f\v\r";
                int ePos, sPos;
                // order dependency: right trim before let trim
                // left trim will invalidate end's index value
                if ((ePos = field.find_last_not_of(wsList)) != string::npos)
                {
                    // ePos+1 because find_last_not_of stops on white space
                    field.erase((end > ePos) ? end : ePos + 1);
                }
                if ((sPos = field.find_first_not_of(wsList)) != string::npos)
                {
                    field.erase(0, (start != -1 && start < sPos) ? start : sPos);
                }
                // reset the quoted section
                start = end = -1;
            }
            // save the new field and reset the temporary
            record.push_back(field);
            field.clear();
            // exit case 1: !is, managed by loop condition
            // exit case 2: recordDelim, managed here
            if (ch == recordDelim) break;
        }
        else if (ch == quote)
        {
            // save the start of the quoted section
            start = field.length();
            while (is.get(ch))
            {
                if (ch == '"')
                {
                    // consecutive quotes are an escaped quote literal
                    // only applies in quoted fields
                    // 'a""b""c' becomes 'abc'
                    // 'a"""b"""c' becomes 'a"b"c'
                    // '"a""b""c"' becomes 'a"b"c'
                    if (is.peek() != '"')
                    {
                        // save the end of the quoted section
                        end = field.length();
                        break;
                    }
                    else field.push_back(is.get());
                }
                else field.push_back(ch);
            }
        }
        else field.push_back(ch);
    }
    return record;
}

static const SQChar sq_parse_csv_TAG[] = _SC("ParseCSV");

static SQRESULT sq_parse_csv_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v) {
    std::ifstream *self = (std::ifstream *)p;
    if(self) delete self;
    return 0;
}

/*
** Creates a new ParseCSV.
*/
static SQRESULT sq_parse_csv_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, fname);
    std::ifstream *self = new std::ifstream(fname);
    if(self->is_open()){

        sq_setinstanceup(v, 1, self);
        sq_setreleasehook(v, 1, sq_parse_csv_release_hook);
        return 1;
    }
    delete self;
    return sq_throwerror(v, _SC("failed to open %s"), fname);
}

static SQRESULT sq_parse_csv_next_row(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INSTANCE_VAR(v, 1, std::ifstream, self, sq_parse_csv_TAG);
    std::vector<std::string> row_fields = CsvGetLine(*self);
    size_t rsize = row_fields.size();
    if (rsize == 0) sq_pushnull(v);
    else {
        sq_newarray(v, rsize);
        for(size_t i=0; i<rsize; ++i){
            sq_pushinteger(v, i);
            std::string &str = row_fields[i];
            sq_pushstring(v, str.c_str(), str.size());
            sq_rawset(v, -3);
        }
    }
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_parse_csv_##name,nparams,tycheck}
static SQRegFunction sq_parse_csv_methods[] =
{
    _DECL_FUNC(constructor,2,_SC("xs")),
    _DECL_FUNC(next_row, 1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sqext_register_csv_parser (HSQUIRRELVM v) {
	sq_pushstring(v,sq_parse_csv_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_parse_csv_TAG);
    sq_insert_reg_funcs(v, sq_parse_csv_methods);
	sq_newslot(v,-3,SQTrue);

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif

