/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

/*
 * TODO:
 *
 * 1) IT SHOULD BE POSSIBLE TO CREATE A (C++)PROXY WITHOUT ANY (JS)PEER, E.G.
 *    - WHEN PEER-NAME IS ALREADY TAKEN (I.E. FOR SINGLETONS)
 *    - WHEN PEER-NAME IS NOT A JS-IDENTIFIER, ETC.
 *    - OR WHEN THE PROXY INTENDED TO ACT SOLELY AS A "HANDLER"
 *      - QUESTION: HOW TO CONSTRUCT SUCH A PROXY?
 *
 * 2) PEERS COULD HAVE A "NAMESPACE" (IN ADDITION TO THEIR NAME), E.G.
 *    - v1.FileManager
 *
 * 3) PEERS COULD HAVE A STRING-BASED "SIGNATURE" (INSTEAD OF THE PeerProperties STRUCT), E.G.
 *    - Proxy[] (I.E. ARRAY-BASED)
 *    - FileManager (I.E. SINGLETON)
 *    - v1.FileDownloader[] (I.E. ARRAY-BASED, WITH NAMESPACE)
 *
 * 4) SHOULD IT BE POSSIBLE TO CREATE A (C++)PROXY FROM THE JS-SIDE?, E.G.
 *    var peer = new Peer("v1.FileDownloader", "http:://foo.com/bar.txt");
 *    peer.onReady = function(data) { print(data); };
 *    peer.start();
 *
 * 5) THERE COULD BE A STATIC Proxy::peersHandle() METHOD, I.E.
 *    - INSTEAD OF USING get<OBJECT>(globalHandle(), "peers")
 */

#pragma once

#include "jsp/Proto.h"
#include "jsp/WrappedObject.h"

#define TARGET(FN, ...) Proto::FN(__VA_ARGS__)
#define HANDLE(FN, ...) handler->FN(__VA_ARGS__)
#define FORWARD(FN, ...) handler ? HANDLE(FN, __VA_ARGS__) : TARGET(FN, __VA_ARGS__)

namespace jsp
{
    typedef std::function<bool(const CallArgs&)> NativeCallFnType;
    
    struct NativeCall
    {
        std::string name;
        NativeCallFnType fn;
        
        NativeCall(const std::string &name, const NativeCallFnType &fn)
        :
        name(name),
        fn(fn)
        {}
    };
    
    struct PeerProperties
    {
        std::string name;
        bool isSingleton;
        
        PeerProperties(const std::string &name = "", bool isSingleton = false)
        :
        name(name),
        isSingleton(isSingleton)
        {}
    };
    
    class Proxy : public Proto
    {
    public:
        static bool init();
        static void uninit();
        
        // ---
        
        Proxy();
        Proxy(const std::string &peerName, bool isSingleton = false);
        
        ~Proxy();

        virtual Proxy* getHandler() const;
        virtual void setHandler(Proxy *handler);

        // ---

        inline JS::HandleObject peerHandle() const
        {
            return JS::Handle<JSObject*>::fromMarkedLocation(reinterpret_cast<JSObject* const*>(peer.address())); // XXX
        }
        
        std::string getPeerId();

        virtual int32_t registerNativeCall(const std::string &name, const NativeCallFnType &fn);
        virtual bool unregisterNativeCall(const std::string &name);
        virtual bool apply(const NativeCall &nativeCall, const CallArgs &args);
        
        // ---
        
        inline bool exec(const std::string &source, const ReadOnlyCompileOptions &options) override
        {
            return FORWARD(exec, source, options);
        }
        
        inline bool eval(const std::string &source, const ReadOnlyCompileOptions &options, MutableHandleValue result) override
        {
            return FORWARD(eval, source, options, result);
        }
        
        // ---
        
        inline Value call(HandleObject object, const char *functionName, const HandleValueArray& args = HandleValueArray::empty()) override
        {
            return FORWARD(call, object, functionName, args);
        }
        
        inline Value call(HandleObject object, HandleValue functionValue, const HandleValueArray& args = HandleValueArray::empty()) override
        {
            return FORWARD(call, object, functionValue, args);
        }
        
        inline Value call(HandleObject object, HandleFunction function, const HandleValueArray& args = HandleValueArray::empty()) override
        {
            return FORWARD(call, object, function, args);
        }

        // ---
        
        inline JSObject* newPlainObject() override
        {
            return FORWARD(newPlainObject);
        }
        
        inline JSObject* newObject(const std::string &className, const HandleValueArray& args = HandleValueArray::empty()) override
        {
            return FORWARD(newObject, className, args);
        }

        inline bool hasProperty(HandleObject object, const char *name) override
        {
            return FORWARD(hasProperty, object, name);
        }
        
        inline bool hasOwnProperty(HandleObject object, const char *name) override
        {
            return FORWARD(hasOwnProperty, object, name);
        }
        
        inline bool getOwnPropertyDescriptor(HandleObject object, HandleId id, MutableHandle<JSPropertyDescriptor> desc) override
        {
            return FORWARD(getOwnPropertyDescriptor, object, id, desc);
        }
        
        inline bool getProperty(HandleObject object, const char *name, MutableHandleValue result) override
        {
            return FORWARD(getProperty, object, name, result);
        }
        
        inline bool setProperty(HandleObject object, const char *name, HandleValue value) override
        {
            return FORWARD(setProperty, object, name, value);
        }
        
        inline bool defineProperty(HandleObject object, const char *name, HandleValue value, unsigned attrs = 0)
        {
            return FORWARD(defineProperty, object, name, value, attrs);
        }
        
        inline bool deleteProperty(HandleObject object, const char *name) override
        {
            return FORWARD(deleteProperty, object, name);
        }
        
        // ---
        
        inline JSObject* newArray(size_t length = 0) override
        {
            return FORWARD(newArray, length);
        }
        
        inline JSObject* newArray(const HandleValueArray& contents) override
        {
            return FORWARD(newArray, contents);
        }
        
        inline bool hasElement(HandleObject array, int index) override
        {
            return FORWARD(hasElement, array, index);
        }
        
        inline uint32_t getElementCount(HandleObject array) override
        {
            return FORWARD(getElementCount, array);
        }
        
        inline uint32_t getLength(HandleObject array) override
        {
            return FORWARD(getLength, array);
        }
        
        inline bool setLength(HandleObject array, size_t length) override
        {
            return FORWARD(setLength, array, length);
        }
        
        inline bool getElement(HandleObject array, int index, MutableHandleValue result) override
        {
            return FORWARD(getElement, array, index, result);
        }
        
        inline bool setElement(HandleObject array, int index, HandleValue value) override
        {
            return FORWARD(setElement, array, index, value);
        }
        
        inline bool defineElement(HandleObject array, int index, HandleValue value, unsigned attrs = 0) override
        {
            return FORWARD(defineElement, array, index, value, attrs);
        }
        
        inline bool deleteElement(HandleObject array, int index) override
        {
            return FORWARD(deleteElement, array, index);
        }

    protected:
        Proxy *handler = nullptr;
        
        PeerProperties peerProperties;
        int32_t peerElementIndex = -1;

        virtual const PeerProperties defaultPeerProperties() const;
        virtual JSObject* createPeer();

        static bool forwardNativeCall(JSContext *cx, unsigned argc, Value *vp);
        
    private:
        struct Statics
        {
            int32_t lastInstanceId = -1;
            std::map<int32_t, Proxy*> instances;
            
            Heap<WrappedObject> peers;
        };
        
        static Statics *statics;
        
        static int32_t addInstance(Proxy *instance);
        static bool removeInstance(int32_t instanceId);
        static Proxy* getInstance(int32_t instanceId);
        
        // ---
        
        Heap<WrappedObject> peer;
        
        int32_t instanceId = -1;
        int32_t lastNativeCallId = -1;
        std::map<int32_t, NativeCall> nativeCalls;
        
        const NativeCall* getNativeCall(int32_t nativeCallId) const;
        int32_t getNativeCallId(const std::string &name) const;
    };
}

#undef TARGET
#undef HANDLE
#undef FORWARD
