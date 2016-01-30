class HashTable
{
    /*
    A CursorKind describes the kind of entity that a cursor points to.
    */

    # The required BaseEnumeration declarations.
    _ht = null;
    
    constructor(int_size = 0)
    {
	_ht = {};
    }

    function len(int_size)
    {
        //Test if this is a declaration kind.
        return _ht.len();
    }

    function newSlot(str_key, value)
    {
	_ht.rawset(str_key, value);
    }

    function set(str_key, value)
    {
	_ht.rawset(str_key, value);
    }

    function get(str_key, default_value)
    {
	return _ht.rawget(str_key, default_value);
    }

    function exists(str_key)
    {
	return _ht.rawget(str_key);
    }

    function remove(key)
    {
	_ht.rawdelete(key);
    }

    function clear()
    {
    }

    function next()
    {
    }

    function tostring()
    {
        return format("HashTable.%s", name);
    }
}

dofile("generate-cpp-class-wrapper.nut");
generateCppClassWrapper(HashTable, "HashTable");
