#pragma once

bool DoHook(void *addr,void *handler);
void DoUnHook(void* func);

#define Hook(func, handler) (DoHook((void*)(func), (void*)(handler)))
#define UnHook(func) (DoUnHook((void*)(func)))
