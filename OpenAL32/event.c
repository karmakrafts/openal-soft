
#include "config.h"

#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#include "alMain.h"
#include "alError.h"


AL_API void AL_APIENTRY alEventControlSOFT(ALsizei count, const ALenum *types, ALboolean enable)
{
    ALCcontext *context;
    ALbitfieldSOFT flags = 0;
    ALsizei i;

    context = GetContextRef();
    if(!context) return;

    if(count < 0)
        SET_ERROR_AND_GOTO(context, AL_INVALID_VALUE, done);
    if(count == 0) goto done;
    if(!types)
        SET_ERROR_AND_GOTO(context, AL_INVALID_VALUE, done);

    for(i = 0;i < count;i++)
    {
        if(types[i] == AL_EVENT_TYPE_BUFFER_COMPLETED_SOFT)
            flags |= EventType_BufferCompleted;
        else if(types[i] == AL_EVENT_TYPE_SOURCE_STATE_CHANGED_SOFT)
            flags |= EventType_SourceStateChange;
        else if(types[i] == AL_EVENT_TYPE_ERROR_SOFT)
            flags |= EventType_Error;
        else if(types[i] == AL_EVENT_TYPE_PERFORMANCE_SOFT)
            flags |= EventType_Performance;
        else
            SET_ERROR_AND_GOTO(context, AL_INVALID_ENUM, done);
    }

    almtx_lock(&context->EventLock);

    if(enable)
        context->EnabledEvts |= flags;
    else
        context->EnabledEvts &= ~flags;

    almtx_unlock(&context->EventLock);
done:
    ALCcontext_DecRef(context);
}

AL_API void AL_APIENTRY alEventCallbackSOFT(ALEVENTPROCSOFT callback, void *userParam)
{
    ALCcontext *context;

    context = GetContextRef();
    if(!context) return;

    almtx_lock(&context->EventLock);
    context->EventCb = callback;
    context->EventParam = userParam;
    almtx_unlock(&context->EventLock);

    ALCcontext_DecRef(context);
}
