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
	parents[#className].insert(#className); \
}

#define USE_RTTI_DEF2(className, firstParentName, secondParentName) { \
	if (parents.find(#className) == parents.end()) { \
		parents[#className] = {}; \
	} \
	parents[#className].insert(#firstParentName); \
	parents[#className].insert(#secondParentName); \
	parents[#className].insert(#className); \
}

#define TYPEINFO(objPtr) \
	(objPtr != nullptr) ? objPtr->Holder::info : Typeinfo("NULLPTR"); 

#define DYNAMIC_CAST(resultType, objPtr) \
	isAbleToCast(#resultType, objPtr) ? reinterpret_cast<resultType*>(objPtr) : nullptr;

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

template<typename T> bool isAbleToCast(std::string first_type, T ptr) {
	auto second_type = ptr->Holder::info.name;
	if (parents[second_type].find(first_type) != parents[second_type].end()) {
		ptr->Holder::info.name = first_type;
		return true;
	}
	return false;
}

#define BASE(cls) \
class cls : public Holder { \
public: \
	cls() : Holder(#cls) {} \
	cls(std::string name) : Holder(name) {} 
	

#define ENDCLASS \
};

#define DERIVED(cls, parent) \
class cls : public Holder, public parent { \
public: \
	cls() { \
		this->Holder::info = Typeinfo(#cls); \
	}

#define DERIVED2(cls, parent1, parent2) \
class cls : public Holder, public parent1, public parent2 { \
public: \
	cls() : parent1(#cls), parent2(#cls) {}


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
	A* ptrC = new C();
	std::cout << ptrC->Holder::info.name << std::endl;
	A* ptrA = new A();
	auto first = DYNAMIC_CAST(C, ptrC);
	if (first != nullptr) {
		std::cout << "Can be casted to C*" << std::endl;
	}
	else {
		std::cout << "Cannot be casted to C*" << std::endl;
	}
	auto second = DYNAMIC_CAST(B, ptrC);
	if (second != nullptr) {
		std::cout << "Can be casted to B*" << std::endl;
	}
	else {
		std::cout << "Cannot be casted to B*" << std::endl;
	}
	auto third = DYNAMIC_CAST(B, ptrA);
	if (third != nullptr) {
		std::cout << "Can be casted to B*" << std::endl;
	}
	else {
		std::cout << "Cannot be casted to B*" << std::endl;
	}
	system("pause");
	return 0;
}
