#include "StoreWatcher.h"
#include <unistd.h>

void startWatcher(JNIEnv* pEnv, StoreWatcher* pWatcher, Store* pStore,
		jobject pStoreFront) {
	memset(pWatcher, 0, sizeof(StoreWatcher));
	pWatcher->mState = STATE_OK;
	pWatcher->mStore = pStore;

	if ((*pEnv)->GetJavaVM(pEnv, &pWatcher->mJavaVM) != JNI_OK) {
		goto ERROR;
	}

	pWatcher->ClassStore = (*pEnv)->FindClass(pEnv, "com/packtpub/Store");
	makeGlobalRef(pEnv, &pWatcher->ClassStore);
	if (pWatcher->ClassStore == NULL) {
		goto ERROR;
	}
	pWatcher->ClassColor = (*pEnv)->FindClass(pEnv, "com/packtpub/Color");
	makeGlobalRef(pEnv, &pWatcher->ClassColor);
	if (pWatcher->ClassColor == NULL) {
		goto ERROR;
	}
	pWatcher->MethodOnAlertInt = (*pEnv)->GetMethodID(pEnv,
			pWatcher->ClassStore, "onAlert", "(I)V");
	if (pWatcher->MethodOnAlertInt == NULL)
		goto ERROR;
	pWatcher->MethodOnAlertString = (*pEnv)->GetMethodID(pEnv,
			pWatcher->ClassStore, "onAlert", "(Ljava/lang/String;)V");
	if (pWatcher->MethodOnAlertString == NULL)
		goto ERROR;
	pWatcher->MethodOnAlertColor = (*pEnv)->GetMethodID(pEnv,
			pWatcher->ClassStore, "onAlert", "(Lcom/packtpub/Color;)V");
	if (pWatcher->MethodOnAlertColor == NULL)
		goto ERROR;
	pWatcher->MethodColorEquals = (*pEnv)->GetMethodID(pEnv,
			pWatcher->ClassColor, "equals", "(Ljava/lang/Object;)Z");
	if (pWatcher->MethodColorEquals == NULL)
		goto ERROR;
	jmethodID ConstructorColor = (*pEnv)->GetMethodID(pEnv,
			pWatcher->ClassColor, "<init>", "(Ljava/lang/String;)V");
	if (ConstructorColor == NULL)
		goto ERROR;

	pWatcher->mStoreFront = (*pEnv)->NewGlobalRef(pEnv, pStoreFront);
	if (pWatcher->mStoreFront == NULL) {
		goto ERROR;
	}

	jstring lColor = (*pEnv)->NewStringUTF(pEnv, "white");
	if (lColor == NULL)
		goto ERROR;
	pWatcher->mColor = (*pEnv)->NewObject(pEnv, pWatcher->ClassColor,
			ConstructorColor, lColor);
	makeGlobalRef(pEnv, &pWatcher->mColor);
	if (pWatcher->mColor == NULL)
		goto ERROR;

	//native thread
	pthread_attr_t lAttributes;
	int lError = pthread_attr_init(&lAttributes);
	if (lError) {
		goto ERROR;
	}
	lError = pthread_create(&pWatcher->mThread, &lAttributes, runWatcher,
			pWatcher);
	if (lError) {
		goto ERROR;
	}
	return;

	ERROR: stopWatcher(pEnv, pWatcher);
	return;
}

JNIEnv* getJNIEnv(JavaVM* pJavaVM) {
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6;
	args.name = "NativeThread";
	args.group = NULL;

	JNIEnv* lEnv;
	if ((*pJavaVM)->AttachCurrentThread(pJavaVM, &lEnv, &args) != JNI_OK) {
		lEnv = NULL;
	}
	return lEnv;
}

void* runWatcher(void* pArgs) {
	StoreWatcher* lWatcher = (StoreWatcher*) pArgs;
	Store* lStore = lWatcher->mStore;
	JavaVM* lJavaVM = lWatcher->mJavaVM;

	JNIEnv* lEnv = getJNIEnv(lJavaVM);
	if (lEnv == NULL) {
		goto ERROR;
	}

	int32_t lRunning = 1;
	while (lRunning) {
		sleep(SLEEP_DURATION);

		StoreEntry* lEntry = lWatcher->mStore->mEntries;
		int32_t lScanning = 1;
		while (lScanning) {
			(*lEnv)->MonitorEnter(lEnv, lWatcher->mStoreFront);

			lRunning = (lWatcher->mState == STATE_OK);
			StoreEntry* lEntryEnd = lWatcher->mStore->mEntries
					+ lWatcher->mStore->mLength;
			lScanning = (lEntry < lEntryEnd);

			if (lRunning && lScanning) {
				processEntry(lEnv, lWatcher, lEntry);
			}

			(*lEnv)->MonitorExit(lEnv, lWatcher->mStoreFront);

			++lEntry;
		}
	}

	ERROR: (*lJavaVM)->DetachCurrentThread(lJavaVM);
	pthread_exit(NULL);
}

void processEntry(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry) {
	switch (pEntry->mType) {
	case StoreType_Integer:
		processEntryInt(pEnv, pWatcher, pEntry);
		break;
	case StoreType_String:
		processEntryString(pEnv, pWatcher, pEntry);
		break;
	case StoreType_Color:
		processEntryColor(pEnv, pWatcher, pEntry);
		break;
	}
}

void processEntryInt(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry) {
	if (strcmp(pEntry->mKey, "watcherCounter") == 0) {
		++pEntry->mValue.mInteger;
	} else if ((pEntry->mValue.mInteger > 1000)
			|| (pEntry->mValue.mInteger < -1000)) {
		(*pEnv)->CallVoidMethod(pEnv, pWatcher->mStoreFront,
				pWatcher->MethodOnAlertInt, (jint) pEntry->mValue.mInteger);
	}
}

void processEntryString(JNIEnv* pEnv, StoreWatcher* pWatcher,
		StoreEntry* pEntry) {
	if (strcmp(pEntry->mValue.mString, "apple")) {
		jstring lValue = (*pEnv)->NewStringUTF(pEnv, pEntry->mValue.mString);
		(*pEnv)->CallVoidMethod(pEnv, pWatcher->mStoreFront,
				pWatcher->MethodOnAlertString, lValue);
		(*pEnv)->DeleteLocalRef(pEnv, lValue);
	}
}

void processEntryColor(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry) {
	jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pWatcher->mColor,
			pWatcher->MethodColorEquals, pEntry->mValue.mColor);
	if (lResult) {
		(*pEnv)->CallVoidMethod(pEnv, pWatcher->mStoreFront,
				pWatcher->MethodOnAlertColor, pEntry->mValue.mColor);
	}
}

void makeGlobalRef(JNIEnv* pEnv, jobject* pRef) {
	if (*pRef != NULL) {
		jobject lGlobalRef = (*pEnv)->NewGlobalRef(pEnv, *pRef);
		(*pEnv)->DeleteLocalRef(pEnv, *pRef);
		*pRef = lGlobalRef;
	}
}

void deleteGlobalRef(JNIEnv* pEnv, jobject* pRef) {
	if (*pRef != NULL) {
		(*pEnv)->DeleteGlobalRef(pEnv, *pRef);
		*pRef = NULL;
	}
}

void stopWatcher(JNIEnv* pEnv, StoreWatcher* pWatcher) {
	if (pWatcher->mState == STATE_OK) {
		(*pEnv)->MonitorEnter(pEnv, pWatcher->mStoreFront);

		pWatcher->mState = STATE_KO;

		(*pEnv)->MonitorExit(pEnv, pWatcher->mStoreFront);
		pthread_join(pWatcher->mThread, NULL);

		deleteGlobalRef(pEnv, &pWatcher->mStoreFront);
		deleteGlobalRef(pEnv, &pWatcher->mColor);
		deleteGlobalRef(pEnv, &pWatcher->ClassStore);
		deleteGlobalRef(pEnv, &pWatcher->ClassColor);
	}
}
