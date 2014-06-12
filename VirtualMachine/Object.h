#ifndef _OBJECT_H_
#define _OBJECT_H_
#include <string>
#include <vector>
#include <unordered_map>

#define NILOBJ  0
#define NUMOBJ  1
#define STROBJ  2
#define CLSOBJ  3
#define CLSTYPE 4
#define BOOLOBJ 5
#define USERDATA 6
#define FUNOBJ  1<<3
#define CFUNOBJ 1<<4
#define METHOD  1<<5



class StrObj {
public:
	std::string str;
	bool mark;
	StrObj() :mark(false) {}
	StrObj(const std::string& s):str(s),mark(false) {}
};

static size_t strHasher(StrObj* const & s){
	return std::hash<std::string>()((*s).str);
}

static bool strEq( StrObj* const & s1,StrObj* const & s2){
	return (*s1).str == (*s2).str;
}

struct FunObj {
	std::string functionName;
	std::vector<char> bytes;
	int nArgs;
	FunObj():nArgs(0) {}
};

class ClsType;
class ClsObj;

struct Object;
typedef Object (*cFunc)(void* state);

struct CFunObj{
	int nArgs;
	cFunc fun;
	std::string functionName;
	CFunObj() :nArgs(0), fun(NULL) {}
};

struct Method{
	union {
		FunObj* funObj;
		CFunObj* cFunObj;
	};
	ClsObj* self;
};

struct Object{
	int type;
	union{
		bool boolval;
		float numval;
		StrObj* strObj;
		FunObj* funObj;
		CFunObj* cFunObj;
		ClsObj* clsObj;
		ClsType* clsType;
		Method method;
		void* userData;
	} value;
};

class ClsType {
public:
	std::string clsName;
	std::unordered_map<StrObj*,Object,decltype(strHasher)*,decltype(strEq)*> clsAttrs{0,strHasher,strEq};
	ClsType() {}
};

class ClsObj {
public:
	ClsType* clsType;
	std::unordered_map<StrObj*,Object,decltype(strHasher)*,decltype(strEq)*> attrs{0,strHasher,strEq};
	ClsObj() {}
};

#include <cassert>
namespace std{
	template<>
	struct hash<Object>{
		typedef Object argument_type;
		typedef size_t value_type;
		value_type operator()(const argument_type& obj){
			switch (obj.type){
				case NUMOBJ:return hash<float>()(obj.value.numval); break;
				case STROBJ:return strHasher(obj.value.strObj); break;
				case FUNOBJ:return hash<decltype(obj.value.funObj)>()(obj.value.funObj); break;
				case BOOLOBJ:return hash<bool>()(obj.value.boolval); break;
				case CLSOBJ:return hash<decltype(obj.value.clsObj)>()(obj.value.clsObj); break;
				//case CLSTYPE:return hash<decltype(obj.value.)>()(obj.value.funObj);
				case NILOBJ:
				default:assert(0); break;
			}
		}
	};

	template<>
	struct equal_to<Object>{
		typedef bool result_type;
		typedef Object first_argument_type;
        typedef Object second_argument_type;
		bool operator()(first_argument_type const& a, second_argument_type const& b) const{
			switch (a.type){
				case NUMOBJ:return a.value.numval == b.value.numval;
				case STROBJ:return strEq(a.value.strObj,b.value.strObj); 
				case FUNOBJ:return a.value.funObj == b.value.funObj; 
				case BOOLOBJ:return a.value.boolval == b.value.boolval; 
				case CLSOBJ:return a.value.clsObj == b.value.clsObj;
				//case CLSTYPE:return hash<decltype(obj.value.)>()(obj.value.funObj);
				case NILOBJ:
				default:assert(0); break;
			}
		}
	};
}

struct Symbol{
	Object obj;
	std::string objName;
	//Symbol(Object* o, std::string s) :obj(o), objName(s) {}
};

union CodeWord{
	short word;
	struct{
		char c1;
		char c2;
	}c;
};

union CodeFloat{
	float f;
	struct{
		char c1;
		char c2;
		char c3;
		char c4;
	}c;
};





#endif