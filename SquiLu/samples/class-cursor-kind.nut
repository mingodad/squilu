class CursorKind
{
    /*
    A CursorKind describes the kind of entity that a cursor points to.
    */

    // The required BaseEnumeration declarations.
    _kinds = [];
    _name_map = null;
    
    constructor(BaseEnumeration)
    {
    }

    static function get_all_kinds()
    {
        //Return all CursorKind enumeration instances.
        return filter(null, CursorKind._kinds)
    }

    function is_declaration()
    {
        //Test if this is a declaration kind.
        return conf.lib.clang_isDeclaration();
    }

    function is_reference()
    {
        //Test if this is a reference kind.
        return conf.lib.clang_isReference();
    }

    function is_expression()
    {
        //Test if this is an expression kind.
        return conf.lib.clang_isExpression();
    }

    function is_statement()
    {
        //Test if this is a statement kind.
        return conf.lib.clang_isStatement();
    }

    function is_attribute()
    {
        //Test if this is an attribute kind.
        return conf.lib.clang_isAttribute();
    }

    function is_invalid()
    {
        //Test if this is an invalid kind.
        return conf.lib.clang_isInvalid();
    }

    function is_translation_unit()
    {
        //Test if this is a translation unit kind.
        return conf.lib.clang_isTranslationUnit();
    }

    function is_preprocessing()
    {
        //Test if this is a preprocessing kind.
        return conf.lib.clang_isPreprocessing();
    }

    function is_unexposed()
    {
        //Test if this is an unexposed kind.
        return conf.lib.clang_isUnexposed();
    }

    function tostring()
    {
        return format("CursorKind.%s", name);
    }
}

dofile("generate-cpp-class-wrapper.nut");
generateCppClassWrapper(CursorKind, "CursorKind");
