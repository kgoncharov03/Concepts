#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

std::unordered_map< std::string, std::unordered_set<std::string>> parents;

#define USE_RTTI_DEF1(className, parentName) { \
	if (parents.find(#className) == parents.end()) { \
		parents[#className] = {}; \
	} \
	parents[#className].insert(#parentName); \
}

#define USE_RTTI_DEF2(className, firstParentName, secondParentName) { \
	if (parents.find(#className) == parents.end()) { \
		parents[#className] = {}; \
	} \
	parents[#className].insert(#firstParentName); \
	parents[#className].insert(#secondParentName); \
}

#define TYPEINFO(objPtr) \
	(objPtr != nullptr) ? objPtr->Holder::info : Typeinfo("NULLPTR"); 

#define DYNAMIC_CAST(resultType, objType, objPtr) \
	isAbleToCast(#resultType, #objType, objPtr) ? reinterpret_cast<resultType*>(objPtr) : nullptr;

class Typeinfo {
public:
	Typeinfo() {}
	Typeinfo(std::string name_) : name(name_) {}
	std::string name;
};

class Holder {
public:
	Holder() {}

	Holder(std::string name) {
		info = Typeinfo(name);
	}
	virtual ~Holder() {};

	Typeinfo info;
};

template<typename T> bool isAbleToCast(std::string first_type, std::string second_type, T ptr) {
	if (parents[second_type].find(first_type) != parents[second_type].end()) {
		ptr->Holder::info.name = first_type;
		return true;
	}
	return false;
}

#define BASE(cls) \
class cls : public Holder { \
public: \
	cls() : Holder(#cls) {} 

#define ENDCLASS \
};

#define DERIVED(cls, parent) \
class cls : public Holder, public parent { \
public: \
	cls() : Holder(#cls) {}

#define DERIVED2(cls, parent1, parent2) \
class cls : public Holder, public parent1, public parent2 { \
public: \
	cls() : Holder(#cls) {}

BASE(A)
ENDCLASS

BASE(B)
void foo() {
	std::cout << "B class" << std::endl;
}
ENDCLASS

DERIVED2(C, A, B)
void foo() {
	std::cout << "C class" << std::endl;
}
ENDCLASS

int main()
{
	USE_RTTI_DEF2(C, A, B);
	C* c = new C;
	Typeinfo cinfo = TYPEINFO(c);
	std::cout << cinfo.name << std::endl;
	c->foo();
	B* b = DYNAMIC_CAST(B, C, c);
	Typeinfo binfo = TYPEINFO(b);
	std::cout << binfo.name << std::endl;
	b->foo();
	A* a = new A;
	b = DYNAMIC_CAST(B, A, a);
	binfo = TYPEINFO(b);
	std::cout << binfo.name << std::endl;
	system("pause");
	return 0;
}
