/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#pragma once

#include "jsp/Proto.h"

#define TARGET(FN, ...) target->FN(__VA_ARGS__)
#define HANDLE(FN, ...) handler->FN(__VA_ARGS__)
#define FORWARD(FN, ...) handler ? HANDLE(FN, __VA_ARGS__) : TARGET(FN, __VA_ARGS__)

#define REGISTER_CALLBACK(OBJECT, CLASS, METHOD) Proxy::registerCallback(OBJECT, #METHOD, std::bind(&CLASS::METHOD, this, std::placeholders::_1), this)

namespace jsp
{
    class Proxy;
    
    struct Callback
    {
        std::function<bool(CallArgs args)> fn;
        Proxy *proxy;
        
        Callback(const std::function<bool(CallArgs args)> &fn, Proxy *proxy)
        :
        fn(fn),
        proxy(proxy)
        {}
    };
    
    class Proxy : public Proto
    {
    public:
        Proxy(Proto *target);
        Proxy(Proxy *target);

        bool setTarget(Proto *target);
        bool setTarget(Proxy *target);
        
        bool setHandler(Proto *handler);
        bool setHandler(Proxy *handler);

        ~Proxy();

        // ---
        
        inline bool exec(const std::string &source, const ReadOnlyCompileOptions &options) final
        {
            return FORWARD(exec, source, options);
        }
        
        inline bool eval(const std::string &source, const ReadOnlyCompileOptions &options, MutableHandleValue result) final
        {
            return FORWARD(eval, source, options, result);
        }
        
        // ---
        
        inline Value call(HandleObject object, const char *name, const HandleValueArray& args = HandleValueArray::empty()) final
        {
            return FORWARD(call, object, name, args);
        }
        
        inline Value call(HandleObject object, HandleValue function, const HandleValueArray& args = HandleValueArray::empty()) final
        {
            return FORWARD(call, object, function, args);
        }
        
        inline Value call(HandleObject object, HandleFunction function, const HandleValueArray& args = HandleValueArray::empty()) final
        {
            return FORWARD(call, object, function, args);
        }
        
        // ---
        
        inline bool applyCallback(std::function<bool(CallArgs args)> &fn, CallArgs args) final
        {
            return FORWARD(applyCallback, fn, args);
        }
        
        // ---
        
        inline JSObject* newObject() final
        {
            return FORWARD(newObject);
        }
        
        inline bool hasProperty(HandleObject object, const char *name) final
        {
            return FORWARD(hasProperty, object, name);
        }
        
        inline bool hasOwnProperty(HandleObject object, const char *name) final
        {
            return FORWARD(hasOwnProperty, object, name);
        }
        
        inline bool getProperty(HandleObject object, const char *name, MutableHandleValue result) final
        {
            return FORWARD(getProperty, object, name, result);
        }
        
        inline bool setProperty(HandleObject object, const char *name, HandleValue value) final
        {
            return FORWARD(setProperty, object, name, value);
        }
        
        inline bool deleteProperty(HandleObject object, const char *name) final
        {
            return FORWARD(deleteProperty, object, name);
        }
        
        inline bool getOwnPropertyDescriptor(HandleObject object, HandleId id, MutableHandle<JSPropertyDescriptor> desc) final
        {
            return FORWARD(getOwnPropertyDescriptor, object, id, desc);
        }
        
        // ---
        
        inline JSObject* newArray(size_t length = 0) final
        {
            return FORWARD(newArray, length);
        }
        
        inline JSObject* newArray(const HandleValueArray& contents) final
        {
            return FORWARD(newArray, contents);
        }
        
        inline uint32_t getLength(HandleObject array) final
        {
            return FORWARD(getLength, array);
        }
        
        inline bool getElement(HandleObject array, uint32_t index, MutableHandleValue result) final
        {
            return FORWARD(getElement, array, index, result);
        }
        
        inline bool setElement(HandleObject array, uint32_t index, HandleValue value) final
        {
            return FORWARD(setElement, array, index, value);
        }
        
        inline bool deleteElement(HandleObject array, uint32_t index) final
        {
            return FORWARD(deleteElement, array, index);
        }
        
    protected:
        static std::vector<Callback> callbacks;
        
        Proto *target = nullptr;
        Proto *handler = nullptr;
        
        /*
         * TODO:
         *
         * - ADD unregisterCallback():
         *   - SHOULD BE CALLED IF HOST JS-OBJECT IS FINALIZED:
         *     - POSSIBILITY: IN DEFINED-FUNCTION'S FINALIZER?
         *   - SHOULD BE CALLED IF instance IS DESTROYED
         *     - POSSIBILITY: IN Proxy's DESTRUCTOR (ASSUMING instance IS ALWAYS A Proxy)
         *   - SHOULD DELETE THE PREVIOUSLY-DEFINED FUNCTION PROPERTY IN THE HOST JS-OBJECT
         * - registerCallback():
         *   - HOW TO DETECT IF "ALREADY REGISTERED"?
         *     - POSSIBILITY: USING A KEY MADE OF THE ADDRESS OF instance AND name
         *   - IF DETECTED: SHOULD IT FAIL?
         *     - OR THE EXISTING CALLBACK BE REPLACED?
         */
        
        static void registerCallback(JS::HandleObject object, const std::string &name, const std::function<bool(CallArgs args)> &fn, Proxy *proxy);
        static bool dispatchCallback(JSContext *cx, unsigned argc, Value *vp);
    };
}

#undef TARGET
#undef HANDLE
#undef FORWARD