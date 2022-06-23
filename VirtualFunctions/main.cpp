#include <iostream>
#include <string>
#include <map>
#include <functional>

#define VIRTUAL_CLASS( name ) \
class name { \
public: \
	static std::map<std::string, std::function<void(void*)>> methodsMap; \
	static std::map<std::string, std::function<void(void*)>> derivedMethodsMap; \
	static std::string baseClassName; \
	static std::string className; \
	bool isDerived; \
	name(bool isDerived_ = false) : isDerived(isDerived_) {}
	

#define END( name ) \
}; \
std::map<std::string, std::function<void(void*)>> name::methodsMap = {}; \
std::string name::baseClassName = (std::string)#name; \
std::string name::className = (std::string)#name;

#define VIRTUAL_CLASS_DERIVED( nameDerived, nameBase ) \
class nameDerived : public nameBase { \
public: \
	static std::string className; \
	nameDerived() : nameBase(true) {}

#define END_DERIVE( nameDerived, nameBase ) \
}; \
std::map<std::string, std::function<void(void*)>> nameDerived::derivedMethodsMap = {}; \
std::string nameDerived::className = (std::string)#nameDerived;

#define DECLARE_METHOD( nameClass, nameMethod, Body ) { \
if (nameClass::baseClassName == (std::string)#nameClass) { \
	nameClass::methodsMap[(std::string)#nameMethod] = [] (void* ptr) { \
		std::cout << static_cast<nameClass*>(ptr)->className << "::" << #nameMethod << ' '; \
		[](nameClass* self){ Body; }(static_cast<nameClass*>(ptr));  \
	}; \
} \
else { \
	nameClass::derivedMethodsMap[(std::string)#nameMethod] = [] (void* ptr) { \
		std::cout << static_cast<nameClass*>(ptr)->className << "::" << #nameMethod << ' '; \
		[](nameClass* self){ Body; }(static_cast<nameClass*>(ptr)); \
	}; \
} \
}

#define VIRTUAL_CALL( ptr, method ) { \
if (ptr->isDerived == false) { \
	if (ptr->methodsMap.find(#method) != ptr->methodsMap.end()) { \
		ptr->methodsMap[#method](static_cast<void*>(ptr)); \
	} \
} \
else { \
	if (ptr->methodsMap.find(#method) != ptr->methodsMap.end()) { \
		if (ptr->derivedMethodsMap.find(#method) != ptr->derivedMethodsMap.end()) { \
		ptr->derivedMethodsMap[#method](static_cast<void*>(ptr)); \
		} else { \
			ptr->methodsMap[#method](static_cast<void*>(ptr)); \
		} \
	} \
	else { \
		throw "Object doesn't have called method"; \
	} \
}\
}

VIRTUAL_CLASS(Base)
int a;
END(Base)

VIRTUAL_CLASS_DERIVED(Derived, Base)
int b;
END_DERIVE(Derived, Base)

int main()
{
	DECLARE_METHOD(Base, Both, std::cout << self->a << std::endl );
	DECLARE_METHOD(Base, OnlyBase, std::cout << std::endl );
	DECLARE_METHOD(Derived, Both, std::cout << self->b << std::endl );
	DECLARE_METHOD(Derived, OnlyDerived, std::cout << std::endl );
	Base base;
	base.a = 0;
	Derived derived;
	derived.b = 1;
	Base* reallyDerived = reinterpret_cast<Base*>(&derived);
	VIRTUAL_CALL(static_cast<Base*>(&base), Both);
	VIRTUAL_CALL(reallyDerived, Both);
	VIRTUAL_CALL(reallyDerived, OnlyBase);
	VIRTUAL_CALL(reallyDerived, OnlyDerived);
	system("pause");
	return 0;
}