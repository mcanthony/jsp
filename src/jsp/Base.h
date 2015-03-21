/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#pragma once

#include "jsp/Proto.h"

#include "chronotext/Exception.h"

namespace jsp
{
    class Base : public Proto
    {
    public:
        static Base* instance();

        /*
         * EVALUATION
         */
        
        bool exec(const std::string &source, const ReadOnlyCompileOptions &options) override;
        bool eval(const std::string &source, const ReadOnlyCompileOptions &options, MutableHandleValue result) override;
        
        /*
         * FUNCTIONS
         */
        
        Value call(HandleObject object, const char *name, const HandleValueArray& args = HandleValueArray::empty()) override;
        Value call(HandleObject object, HandleValue function, const HandleValueArray& args = HandleValueArray::empty()) override;
        Value call(HandleObject object, HandleFunction function, const HandleValueArray& args = HandleValueArray::empty()) override;
        
        /*
         * NATIVE-CALLBACKS
         */
        
        bool applyCallback(std::function<bool(CallArgs args)> &fn, CallArgs args) override;

        /*
         * OBJECTS AND PROPERTIES
         */
        
        JSObject* newObject() override;
        
        bool hasProperty(HandleObject object, const char *name) override;
        bool hasOwnProperty(HandleObject object, const char *name) override;
        
        bool getProperty(HandleObject object, const char *name, MutableHandleValue result) override;
        bool setProperty(HandleObject object, const char *name, HandleValue value) override;
        
        bool deleteProperty(HandleObject object, const char *name) override;

        inline bool getOwnPropertyDescriptor(HandleObject object, HandleId id, MutableHandle<JSPropertyDescriptor> desc) override
        {
            return js::GetOwnPropertyDescriptor(cx, object, id, desc); // XXX: NON-PUBLIC
        }
        
        /*
         * ARRAYS AND ELEMENTS
         */
        
        JSObject* newArray(size_t length = 0) override;
        JSObject* newArray(const HandleValueArray& contents) override;
        
        uint32_t getLength(HandleObject array) override;
        
        bool getElement(HandleObject array, uint32_t index, MutableHandleValue result) override;
        bool setElement(HandleObject array, uint32_t index, HandleValue value) override;
        
        bool deleteElement(HandleObject array, uint32_t index) override;
        
    protected:
        Base() = default;
        Base(const Base &other) = delete;
    };
}