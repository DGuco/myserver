//
//  TDispatcher.h
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#ifndef dpsg_TDispatcher_h
#define dpsg_TDispatcher_h


#include "Log.h"
#include "msg.pb.h"
#include "net_Inc.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "snappy.h"


namespace Net
{
    class delegate
    {
    public:
        delegate()
        : object_ptr(0)
        , stub_ptr(0)
        {}
        
        template <class T, void (T::*TMethod)(google::protobuf::Message*, void*)>
        static delegate from_method(T* object_ptr)
        {
            delegate d;
            d.object_ptr = object_ptr;
            d.stub_ptr = &method_stub<T, TMethod>; // #1
            return d;
        }
        
        void operator()(google::protobuf::Message* a1, void* pContext) const
        {
            return (*stub_ptr)(object_ptr, a1, pContext);
        }
        
    private:
        typedef void (*stub_type)(void* object_ptr, google::protobuf::Message*, void*);
        
        void* object_ptr;
        stub_type stub_ptr;
        
        template <class T, void (T::*TMethod)(google::protobuf::Message*, void* )>
        static void method_stub(void* object_ptr, google::protobuf::Message* a1, void* pContext)
        {
            T* p = static_cast<T*>(object_ptr);
            return (p->*TMethod)(a1, pContext); // #2
        }
    };
    
    
	class CMsgDispatcher
	{
	public:
        CMsgDispatcher()
        :m_bCanceled(false)
		{
            m_defaultCallback = delegate::from_method<CMsgDispatcher, &CMsgDispatcher::discardProtobufMessage>(this);
		}
        
        void discardProtobufMessage(google::protobuf::Message* message, void* pContext)
        {
            GH_INFO("Discarding %s", message->GetTypeName().c_str());
        }
        
        
		virtual ~CMsgDispatcher()
		{
            
		}
        
		void CancelDispatch()
		{
			m_bCanceled=true;
		}
        
		void ResumeDispatch()
		{
			m_bCanceled=false;
		}
        
		bool DispatchIsCanceled()const
		{
			return m_bCanceled;
		}
        
		bool LoopDispatch(const void* pData,uint32 uSize,uint32& uProcessedSize, void* pContext)
		{
			uint32 uTotalSize = uSize;
			uint32 uProcessedOnce=0;
			uProcessedSize = 0;
			char* pBuf = static_cast<char*>( const_cast<void*>(pData));
			for(;;)
			{
				bool bRes = OnceDispatch(pBuf, uTotalSize, uProcessedOnce, pContext);
				if(bRes)
				{
					pBuf+=uProcessedOnce;
					uTotalSize -= uProcessedOnce;
					uProcessedSize += uProcessedOnce;
				}
				else
				{
					if( uSize == uProcessedSize)
					{
						return true;
					}else
					{
						return false;
					}
				}
			}
		}
        
        
        inline google::protobuf::Message* createMessage(const std::string& type_name)
        {
            google::protobuf::Message* message = NULL;
            const google::protobuf::Descriptor* descriptor =
            google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
            if (descriptor)
            {
                const google::protobuf::Message* prototype =
                google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
                if (prototype)
                {
                    message = prototype->New();
                }
            }
            return message;
        }
        
        
		bool OnceDispatch(const void* pData,uint32 uSize,uint32& uProcessedSize, void* pContext)
		{
            static rpc::Request req;
            
			if(DispatchIsCanceled())
				return false;
            
			if(uSize<sizeof(int32))
				return false;
            
			const void* pPos=static_cast<char*>( const_cast<void*>(pData));
			const int32* pSize = reinterpret_cast<const int32*>(pPos);
            
            if (uSize - sizeof(int32) < *pSize) {
                return false;
            }
            
            
            uProcessedSize = *pSize + sizeof(int32);
            
            std::string output;
            
            if(!snappy::Uncompress((const char*)(pSize+1), *pSize, &output)){
                GH_ASSERT(false);
            }
            
            if (!req.ParseFromString(output)){
                GH_ASSERT(false);
            }


            
            google::protobuf::Message* message = createMessage(req.method());
            if (message)
            {

                if (message->ParseFromArray(req.serialized_request().c_str(), req.serialized_request().length()))
                {
                    CallbackMap::const_iterator it = m_callbacks.find(message->GetDescriptor());
                    if (it != m_callbacks.end())
                    {
                        it->second(message, pContext);
                    }
                    else
                    {
                        m_defaultCallback(message, pContext);
                    }
                }
                else
                {
                    // parse error
                    GH_ASSERT(false);
                }
                
                delete message;
            }
            else
            {
                GH_ASSERT(false);
            }
            
            
            return true;
              
		}
        
        template<typename T>
        void registerMessageCallback(delegate d)
        {
            m_callbacks[T::descriptor()] = d;
        }
        
    private:
        bool														m_bCanceled;
        
        typedef std::map<const google::protobuf::Descriptor*, delegate> CallbackMap;
        CallbackMap m_callbacks;
        delegate m_defaultCallback;
        
	};
    
}

#endif
