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

	pWatcher->mStroeFront = (*pEnv)->NewGlobalRef(pEnv, pStoreFront);
	if (pWatcher->mStroeFront == NULL) {
		goto ERROR;
	}

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
			(*lEnv)->MonitorEnter(lEnv, lWatcher->mStroeFront);

			lRunning = (lWatcher->mState == STATE_OK);
			StoreEntry* lEntryEnd = lWatcher->mStore->mEntries
					+ lWatcher->mStore->mLength;
			lScanning = (lEntry < lEntryEnd);

			if (lRunning && lScanning) {
				processEntry(lEnv, lWatcher, lEntry);
			}

			(*lEnv)->MonitorExit(lEnv, lWatcher->mStroeFront);

			++lEntry;
		}
	}

	ERROR: (*lJavaVM)->DetachCurrentThread(lJavaVM);
	pthread_exit(NULL);
}

void processEntry(JNIEnv* pEnv, StoreWatcher* pWatcher, StoreEntry* pEntry) {
	if ((pEntry->mType == StoreType_Integer)
			&& (strcmp(pEntry->mKey, "watcherCounter") == 0)) {
		++pEntry->mValue.mInteger;
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
		(*pEnv)->MonitorEnter(pEnv, pWatcher->mStroeFront);

		pWatcher->mState = STATE_KO;

		(*pEnv)->MonitorExit(pEnv, pWatcher->mStroeFront);
		pthread_join(pWatcher->mThread, NULL);

		deleteGlobalRef(pEnv, &pWatcher->mStroeFront);
	}
}
