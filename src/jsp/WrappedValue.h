/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

/*
 * BASED ON SPIDERMONKEY'S BarrieredValue CLASS: https://github.com/mozilla/gecko-dev/blob/esr31/js/src/gc/Barrier.h#L611-697
 *
 * RELEVANT PARTS: HOW TO IMPLEMENT YOUR OWN ValueOperations<T>
 * TO BE IGNORED: THE "PRE WRITE-BARRIER" STUFF, BECAUSE WE'RE FOCUSING ONLY ON "GENERATIONAL GC"
 */

#pragma once

#include "jsp/Context.h"

#include <set>

#define DUMP_WRAPPED_VALUE if (LOG_VERBOSE) { dump(__PRETTY_FUNCTION__); }

namespace jsp
{
    class WrappedValue : public js::MutableValueOperations<WrappedValue>
    {
    public:
        static bool LOG_VERBOSE;
        static std::set<void*> heapTraced;
        
        // ---
        
        WrappedValue();
        ~WrappedValue();
        
        WrappedValue(const Value &v);
        WrappedValue& operator=(const Value &v);
        
        /*
         * THESE 2 ARE NOT MANDATORY (I.E. COMPILER-GENERATED IN ANY-CASE)
         * BUT WE WANT TO CALL dump() WHENEVER IT HAPPENS
         */
        WrappedValue(const WrappedValue &other);
        void operator=(const WrappedValue &other);
        
        template<typename T>
        WrappedValue(const T &newValue)
        {
            assignValue(value, newValue);
            DUMP_WRAPPED_VALUE
        }
    
        template<typename T>
        WrappedValue& operator=(const T &newValue)
        {
            if (TypeTraits<T>::isMarkable)
            {
                assignValue(value, newValue);
                
                if (heapTraced.count(this))
                {
                    beginTracing();
                }
            }
            else if (value.isMarkable())
            {
                if (heapTraced.count(this))
                {
                    endTracing();
                }
                
                assignValue(value, newValue);
            }
            else
            {
                assignValue(value, newValue);
            }
            
            DUMP_WRAPPED_VALUE
            return *this;
        }
        
        operator const Value& () const { return value; }
        
        /*
         * TODO: DECIDE WHAT SHOULD BE THE INTENT OF THIS OPERATOR
         *
         * - CHECKING HOW THE WRAPPED VALUE CONVERTS TO BOOLEAN (AS FOR NOW)?
         * - OR CHECKING IF THE WRAPPED VALUE IS DEFINED (AS IN Heap<WrappedObject>)?
         */
        explicit operator const bool () const;

        bool operator==(const WrappedValue &other) const;
        bool operator!=(const WrappedValue &other) const;

        bool operator==(const Value &other) const;
        bool operator!=(const Value &other) const;

        bool operator==(const std::nullptr_t) const;
        bool operator!=(const std::nullptr_t) const;
        
        bool operator==(const JSObject *other) const;
        bool operator!=(const JSObject *other) const;

        bool operator==(float other) const;
        bool operator!=(float other) const;
        
        bool operator==(double other) const;
        bool operator!=(double other) const;
        
        bool operator==(int32_t other) const;
        bool operator!=(int32_t other) const;
        
        bool operator==(uint32_t other) const;
        bool operator!=(uint32_t other) const;

        bool operator==(bool other) const;
        bool operator!=(bool other) const;
        
        bool operator==(const char *other) const;
        bool operator!=(const char *other) const;

        const Value& get() const { return value; }
        const Value* address() const { return &value; }
        Value* unsafeGet() { return &value; }
        
        void set(const Value &newValue);
        void clear();
        
        /*
         * TODO: USE "AUTOMATIC CONVERSION" FOR THE FOLLOWING 2 (AS IMPLEMENTED IN WrappedObject)
         */
        
        static inline Handle<WrappedValue> toHandle(const Heap<WrappedValue> &heapWrapped)
        {
            return Handle<WrappedValue>::fromMarkedLocation(heapWrapped.address());
        }
        
        static inline MutableHandle<WrappedValue> toMutableHandle(Heap<WrappedValue> &heapWrapped)
        {
            return MutableHandle<WrappedValue>::fromMarkedLocation(heapWrapped.unsafeGet());
        }
        
    protected:
        friend class ValueOperations<WrappedValue>;
        friend class MutableValueOperations<WrappedValue>;
        friend struct js::GCMethods<WrappedValue>;
        
        Value value;
        
        const Value* extract() const { return &value; }
        Value* extractMutable() { return &value; }

        void postBarrier();
        void relocate();
        
        void beginTracing();
        void endTracing();
        void trace(JSTracer *trc);
        
        void dump(const char *prefix);
    };
    
    class AutoWrappedValueVector : public AutoVectorRooter<WrappedValue>
    {
    public:
        explicit AutoWrappedValueVector()
        :
        AutoVectorRooter<WrappedValue>(cx, VALVECTOR)
        {}
        
        operator const HandleValueArray () const
        {
            return HandleValueArray::fromMarkedLocation(length(), reinterpret_cast<const Value*>(begin()));
        }
    };
}

namespace js
{
    template <>
    struct GCMethods<WrappedValue>
    {
        static WrappedValue initial() { return JS::UndefinedValue(); }
        static ThingRootKind kind() { return THING_ROOT_VALUE; }
        static bool poisoned(const WrappedValue &wrapped) { return false; }
        static bool needsPostBarrier(const WrappedValue &wrapped) { return wrapped.value.isMarkable(); }
#ifdef JSGC_GENERATIONAL
        static void postBarrier(WrappedValue *wrapped) { wrapped->postBarrier(); }
        static void relocate(WrappedValue *wrapped) { wrapped->relocate(); }
#endif
    };
}
