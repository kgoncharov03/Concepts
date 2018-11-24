#ifndef UNTITLED_OBJECTSTACK_H
#define UNTITLED_OBJECTSTACK_H

#include <iostream>
#include <csetjmp>
#include <exception>
#include <vector>
#include <array>

enum Exception { OK_EXC, CONTINUE_OK, EXC_1, EXC_2, OTHER_EXC};
class ObjectStack;
struct JumpBuffer;

extern int tryNum;
extern int throwNum;
extern int catchNum;
extern std::vector<std::vector<ObjectStack*>> tryStack;
extern std::vector<JumpBuffer> jumpPointers;

struct JumpBuffer {
    jmp_buf buf;
};

class ObjectStack {
public:
    ObjectStack();
    virtual ~ObjectStack();
    const static Exception getException(int code);
    const static int getCode(Exception exc);
private:
    int tryCounter_;
};

#define TRY \
    if (tryNum == 0) { \
        throwNum = 0; \
        catchNum = 0; \
    } \
    tryNum++; \
    JumpBuffer jb; \
    ObjectStack os; \
    jumpPointers.push_back(jb); \
    Exception exception = ObjectStack::getException(setjmp(jumpPointers[tryNum - 1].buf)); \
    if (exception == OK_EXC)

#define CATCH(exc) \
    if (exception == exc && ++catchNum)


#define THROW(exc) \
    if (exc == OK_EXC || tryNum == 0 || throwNum >= tryNum) \
        exit(1); \
    throwNum = tryNum; \
    for (auto obj = tryStack[tryNum - 1].rbegin(); obj != tryStack[tryNum - 1].rend(); ++obj) { \
        (*obj)->~ObjectStack(); \
    } \
    longjmp(jumpPointers[tryNum - 1].buf, ObjectStack::getCode(exc));

#define END_TRY \
    if (exception != OK_EXC) { \
        tryNum--; \
        if (tryNum < 0) { \
            std::cerr << "error with using END_TRY" << exception << std::endl; \
            exit(1); \
        } \
        if (throwNum != tryNum + 1) { \
            for (auto obj = tryStack[tryNum].rbegin(); obj != tryStack[tryNum].rend(); ++obj) { \
                (*obj)->~ObjectStack(); \
            } \
        } \
        if (tryNum == 0 && catchNum == 0){ \
            std::cerr << "no CATCH for error " << exception << std::endl; \
            exit(1); \
        } \
        if (tryNum > 0) { \
            if (catchNum == 0) \
                longjmp(jumpPointers[tryNum - 1].buf, exception); \
            catchNum = 0; \
        } \
    } \
    tryNum--;

#endif //UNTITLED_OBJECTSTACK_H
