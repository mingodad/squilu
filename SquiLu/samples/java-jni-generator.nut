class JavaField {

        clazz = null;
        name = null;
        type = null;
        modifiers = null;
        slot = null;
        signature = null;
        annotations = null;

	constructor(
	/*Class*/ declaringClass,
	  /*String*/ name,
	  /*Class*/ type,
	  /*int*/ modifiers,
	  /*int*/ slot,
	  /*String*/ signature,
	  /*byte[]*/ annotations)
	{
		this.clazz = declaringClass;
		this.name = name;
		this.type = type;
		this.modifiers = modifiers;
		this.slot = slot;
		this.signature = signature;
		this.annotations = annotations;
	}
    
	function copy(){}
	function getDeclaringClass(){}
	function getName(){}
	function getModifiers(){}
	function isEnumConstant(){}
	function isSynthetic(){}
	function getType(){}
	function getGenericType(){}
	function equals(/*Object*/ obj){}
	function hashCode(){}
	function toString(){}
	function toGenericString(){}
	function get(/*Object*/ obj){}
	function getBoolean(/*Object*/ obj){}
	function getByte(/*Object*/ obj){}
	function getChar(/*Object*/ obj){}
	function getShort(/*Object*/ obj){}
	function getInt(/*Object*/ obj){}
	function getLong(/*Object*/ obj){}
	function getFloat(/*Object*/ obj){}
	function getDouble(/*Object*/ obj){}
	function set(){}
	function setBoolean(/*Object*/ obj, /*boolean*/ z){}
	function setByte(/*Object*/ obj, /*byte*/ b){}
	function setChar(/*Object*/ obj, /*char*/ c){}
	function setShort(/*Object*/ obj, /*short*/ s){}
	function setInt(/*Object*/ obj, /*int*/ i){}
	function setLong(/*Object*/ obj, /*long*/ l){}
	function setFloat(/*Object*/ obj, /*float*/ f){}
	function setDouble(/*Object*/ obj, /*double*/ d){}

	function getTypeName(/*Class*/ clztype){}
	function getAnnotation(/*boolean*/ annotationClass){}
	function getDeclaredAnnotations(){}
}

class JavaMethod {
	/*Class*/ declaringClass = null;
	/*String*/ name = null;
	/*Class[]*/ parameterTypes = null;
	/*Class*/ returnType = null;
	/*Class[]*/ checkedExceptions = null;
	/*int*/ modifiers = null;
	/*int*/ slot = null;
	/*String*/ signature = null;
	/*byte[]*/ annotations = null;
	/*byte[]*/ parameterAnnotations = null;
	/*byte[]*/ annotationDefault = null;

	constructor(
		/*Class*/ declaringClass,
		/*String*/ name,
		/*Class[]*/ parameterTypes,
		/*Class*/ returnType,
		/*Class[]*/ checkedExceptions,
		/*int*/ modifiers,
		/*int*/ slot,
		/*String*/ signature,
		/*byte[]*/ annotations,
		/*byte[]*/ parameterAnnotations,
		/*byte[]*/ annotationDefault)
	{
		this.clazz = declaringClass;
		this.name = name;
		this.parameterTypes = parameterTypes;
		this.returnType = returnType;
		this.exceptionTypes = checkedExceptions;
		this.modifiers = modifiers;
		this.slot = slot;
		this.signature = signature;
		this.annotations = annotations;
		this.parameterAnnotations = parameterAnnotations;
		this.annotationDefault = annotationDefault;
	}

	function copy(){}
	function getDeclaringClass(){}
	function getName(){}
	function getModifiers(){}
	function getTypeParameters(){}
	function getReturnType(){}
	function getGenericReturnType(){}
	function getParameterTypes(){}
	function getGenericParameterTypes(){}
	function getExceptionTypes(){}
	function getGenericExceptionTypes(){}
	function equals(/*Object*/ obj){}
	function hashCode(){}
	function toString(){}
	function toGenericString(){}
	function invoke(/*Object*/ obj, /*Object...*/ args){}
	function isBridge(){}
	function isVarArgs(){}
	function isSynthetic(){}
	function getAnnotation(/*Class<A>*/ annotationClass){}
	function getAnnotations(){}
	function getDeclaredAnnotations(){}
	function getDefaultValue(){}
	function getParameterAnnotations(){}
}

class JavaClass {
	function toString(){}
	function forName(/*String*/ className, /*boolean*/ initialize, /*ClassLoader*/ loader){}
	function newInstance(){}
	function isInstance(/*Object*/ obj){}
	function isAssignableFrom(/*Class<?>*/ cls){}
	function isInterface(){}
	function isArray(){}
	function isPrimitive(){}
	function isAnnotation(){}
	function isSynthetic(){}
	function getName(){}
	function getClassLoader(){}
	function getTypeParameters(){}
	function getSuperclass(){}
	function getGenericSuperclass(){}
	function getPackage(){}
	function getInterfaces(){}
	function getGenericInterfaces(){}
	function getComponentType(){}
	function getModifiers(){}
	function getSigners(){}
	function getEnclosingMethod(){}
	function getEnclosingConstructor(){}
	function getDeclaringClass(){}
	function getEnclosingClass(){}
	function getSimpleName(){}
	function getCanonicalName(){}
	function isAnonymousClass(){}
	function isLocalClass(){}
	function isMemberClass(){}
	function getClasses(){}
	function getFields(){}
	function getMethods(){}
	function getConstructors(){}
	function getField(/*String*/ name){}
	function getMethod(/*String*/ name, /*Class<?>...*/ parameterTypes){}
	function getConstructor(/*Class<?>...*/ parameterTypes){}
	function getDeclaredClasses(){}
	function getDeclaredFields(){}
	function getDeclaredMethods(){}
	function getDeclaredConstructors(){}
	function getDeclaredField(/*String*/ name){}
	function getDeclaredMethod(/*String*/ name, /*Class<?>...*/ parameterTypes){}
	function getDeclaredConstructor(/*Class<?>...*/ parameterTypes){}
	function getResourceAsStream(/*String*/ name){}
	function getResource(/*String*/ name){}
	function getProtectionDomain(){}
	function getPrimitiveClass(/*String*/ name){}
	function desiredAssertionStatus(){}
	function isEnum(){}
	function getEnumConstants(){}
	function cast(/*Object*/ obj){}
	function asSubclass(/*Class<U>*/ clazz){}
	function getAnnotation(/*Class<A>*/ annotationClass){}
	function isAnnotationPresent(/*Class<? extends Annotation>*/ annotationClass){}
	function getAnnotations(){}
	function getDeclaredAnnotations(){}
}