/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#pragma once

#include <string>

class JSObject;

namespace jsp
{
    typedef JSObject* OBJECT;
    typedef int32_t INT32;
    typedef uint32_t UINT32;
    typedef float FLOAT32;
    typedef double FLOAT64;
    typedef bool BOOLEAN;
    typedef std::string STRING;
    typedef const char* CHARS;
    
    template <class T>
    class TypeTraits
    {
    public:
        static constexpr const bool isNumber = false;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;
        
        typedef T defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return defaultType(); }
    };
    
    // ---
    
    template <>
    class TypeTraits<JSObject*>
    {
    public:
        static constexpr const bool isNumber = false;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = true;
        
        typedef JSObject* defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return nullptr; }
    };
    
    template <>
    class TypeTraits<int32_t>
    {
    public:
        static constexpr const bool isNumber = true;
        static constexpr const bool isInteger = true;
        static constexpr const bool isSigned = true;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;
        
        typedef int32_t defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return 0; }
    };
    
    template <>
    class TypeTraits<uint32_t>
    {
    public:
        static constexpr const bool isNumber = true;
        static constexpr const bool isInteger = true;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;
        
        typedef uint32_t defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return 0; }
    };
    
    template <>
    class TypeTraits<float>
    {
    public:
        static constexpr const bool isNumber = true;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = true;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;
        
        typedef float defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return 0; }
    };
    
    template <>
    class TypeTraits<double>
    {
    public:
        static constexpr const bool isNumber = true;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = true;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;
        
        typedef double defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return 0; }
    };
    
    template <>
    class TypeTraits<bool>
    {
    public:
        static constexpr const bool isNumber = true;
        static constexpr const bool isInteger = true;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = false;
        static constexpr const bool isObject = false;

        typedef bool defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return false; }
    };
    
    template <>
    class TypeTraits<std::string>
    {
    public:
        static constexpr const bool isNumber = false;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = false;
        static constexpr const bool isText = true;
        static constexpr const bool isObject = false;
        
        typedef const char* defaultType; // XXX: std::string CAN'T BE USED IN constexpr
        inline static constexpr const defaultType defaultValue() noexcept { return ""; }
    };
    
    template <>
    class TypeTraits<const char*>
    {
    public:
        static constexpr const bool isNumber = false;
        static constexpr const bool isInteger = false;
        static constexpr const bool isSigned = false;
        static constexpr const bool isStatic = true;
        static constexpr const bool isText = true;
        static constexpr const bool isObject = false;
        
        typedef const char* defaultType;
        inline static constexpr const defaultType defaultValue() noexcept { return ""; }
    };
}