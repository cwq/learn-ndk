#ifndef _STOREWATCHER_H_
#define _STOREWATCHER_H_

#include "Store.h"
#include <jni.h>
#include <stdint.h>
#include <pthread.h>

#define SLEEP_DURATION 5
#define STATE_OK 0
#define STATE_KO 1

typedef struct {
	Store* mStore;
	JavaVM* mJavaVM;
	jobject mStoreFront;
	jobject mColor;

	jclass ClassStore;
	jclass ClassColor;
	jmethodID MethodOnAlertInt;
	jmethodID MethodOnAlertString;
	jmethodID MethodOnAlertColor;
	jmethodID MethodColorEquals;

	pthread_t mThread;
	int32_t mState;
} StoreWatcher;

void startWatcher(JNIEnv* pEnv, StoreWatcher* pWatcher, Store* pStore,
		jobject pStoreFront);
void stopWatcher(JNIEnv* pEnv, StoreWatcher* pWatcher);

void makeGlobalRef(JNIEnv* pEnv, jobject* pRef);
void deleteGlobalRef(JNIEnv* pEnv, jobject* pRef);
JNIEnv* getJNIEnv(JavaVM* pJavaVM);
void* runWatcher(void* pArgs);

void processEntry(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry);
void processEntryInt(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry);
void processEntryString(JNIEnv* pEnv, StoreWatcher* pWatcher,
		StoreEntry* pEntry);
void processEntryColor(JNIEnv* pEnv, StoreWatcher* pWatcher,
		StoreEntry* pEntry);

#endif
