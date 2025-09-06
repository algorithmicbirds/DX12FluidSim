#ifdef _DEBUG
#include <iostream>
#include <stdexcept>
#include <string>

#define VALIDATE_PTR(ptr)                                                                                              \
    if (!(ptr))                                                                                                        \
    {                                                                                                                  \
        std::wcerr << L"[" << __FILE__ << L":" << __LINE__ << L" in " << __func__ << L"] " << L#ptr << L" is null\n";  \
        throw std::runtime_error(                                                                                      \
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + " in " + std::string(__func__) + ": " +           \
            std::string(#ptr) + " is null"                                                                             \
        );                                                                                                             \
    }
#else
#define VALIDATE_PTR(ptr) ((void)0)
#endif // _DEBUG

#ifdef _DEBUG
#include <iostream>
#include <stdexcept>
#include <string>

#define DX_VALIDATE(expr, objName)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        HRESULT hr__ = (expr);                                                                                         \
        if (FAILED(hr__))                                                                                              \
        {                                                                                                              \
            std::wcerr << L"[" << __FILE__ << L":" << __LINE__ << L" in " << __FUNCTION__ << L"] " << L#objName        \
                       << L" failed (0x" << std::hex << hr__ << L")\n";                                                \
            throw std::runtime_error(                                                                                  \
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + " in " + std::string(__FUNCTION__) + ": " +   \
                #objName + " failed"                                                                                   \
            );                                                                                                         \
        }                                                                                                              \
    } while (0)
#else
#define DX_VALIDATE(expr, objName) (expr)
#endif // _DEBUG

#ifdef _DEBUG
#define WAIT_FOR_HANDLE(handle, timeout)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        DWORD result__ = WaitForSingleObject(handle, timeout);                                                         \
        if (result__ != WAIT_OBJECT_0)                                                                                 \
        {                                                                                                              \
            std::wcerr << L"[" << __FILE__ << L":" << __LINE__ << L" in " << __FUNCTION__                              \
                       << L"] Wait failed for handle " << #handle << L"\n";                                            \
            throw std::runtime_error(                                                                                  \
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + " in " + std::string(__FUNCTION__) +          \
                ": Wait failed for " #handle                                                                           \
            );                                                                                                         \
        }                                                                                                              \
    } while (0)
#else
#define WAIT_FOR_HANDLE(handle, timeout) WaitForSingleObject(handle, timeout)
#endif // _DEBUG
