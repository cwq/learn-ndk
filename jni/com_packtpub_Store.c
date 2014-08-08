#include "com_packtpub_Store.h"
#include "Store.h"
#include <stdint.h>
#include <string.h>

static Store gStore = { { }, 0 };

JNIEXPORT jint JNICALL Java_com_packtpub_Store_getInteger(JNIEnv* pEnv,
		jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_Integer)) {
		return lEntry->mValue.mInteger;
	} else {
		return 0.0f;
	}
}

JNIEXPORT void JNICALL Java_com_packtpub_Store_setInteger(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jint pInteger) {
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_Integer;
		lEntry->mValue.mInteger = pInteger;
	}
}

JNIEXPORT jstring JNICALL Java_com_packtpub_Store_getString(JNIEnv* pEnv,
		jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_String)) {
		return (*pEnv)->NewStringUTF(pEnv, lEntry->mValue.mString);
	} else {
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_com_packtpub_Store_setString(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jstring pString) {
	const char* lStringTmp = (*pEnv)->GetStringUTFChars(pEnv, pString, NULL);
	if (lStringTmp == NULL) {
		return;
	}
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_String;
		jsize lStringLength = (*pEnv)->GetStringUTFLength(pEnv, pString);
		lEntry->mValue.mString = (char*) malloc(
				sizeof(char) * (lStringLength + 1));
		strcpy(lEntry->mValue.mString, lStringTmp);
	}
	(*pEnv)->ReleaseStringUTFChars(pEnv, pString, lStringTmp);
}

JNIEXPORT jobject JNICALL Java_com_packtpub_Store_getColor(JNIEnv* pEnv,
		jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_Color)) {
		return lEntry->mValue.mColor;
	} else {
		return NULL;
	}
}
JNIEXPORT void JNICALL Java_com_packtpub_Store_setColor(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jobject pColor) {
	jobject lColor = (*pEnv)->NewGlobalRef(pEnv, pColor);
	if (lColor == NULL) {
		return;
	}
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_Color;
		lEntry->mValue.mColor = lColor;
	} else {
		(*pEnv)->DeleteGlobalRef(pEnv, lColor);
	}
}

JNIEXPORT jintArray JNICALL Java_com_packtpub_Store_getIntegerArray(
		JNIEnv* pEnv, jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_IntegerArray)) {
		jintArray lJavaArray = (*pEnv)->NewIntArray(pEnv, lEntry->mLength);
		if (lJavaArray == NULL) {
			return NULL;
		}
		(*pEnv)->SetIntArrayRegion(pEnv, lJavaArray, 0, lEntry->mLength,
				lEntry->mValue.mIntegerArray);
		return lJavaArray;
	} else {
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_com_packtpub_Store_setIntegerArray(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jintArray pIntegerArray) {
	jsize lLength = (*pEnv)->GetArrayLength(pEnv, pIntegerArray);
	int32_t* lArray = (int32_t*) malloc(lLength * sizeof(int32_t));
	(*pEnv)->GetIntArrayRegion(pEnv, pIntegerArray, 0, lLength, lArray);
	if ((*pEnv)->ExceptionCheck(pEnv)) {
		free(lArray);
		return;
	}
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_IntegerArray;
		lEntry->mLength = lLength;
		lEntry->mValue.mIntegerArray = lArray;
	} else {
		free(lArray);
		return;
	}
}

JNIEXPORT jobjectArray JNICALL Java_com_packtpub_Store_getColorArray(
		JNIEnv* pEnv, jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_ColorArray)) {
		jclass lColorClass = (*pEnv)->FindClass(pEnv, "com/packtpub/Color");
		if (lColorClass == NULL) {
			return NULL;
		}
		jobjectArray lJavaArray = (*pEnv)->NewObjectArray(pEnv, lEntry->mLength,
				lColorClass, NULL);
		(*pEnv)->DeleteLocalRef(pEnv, lColorClass);
		if (lJavaArray == NULL) {
			return NULL;
		}
		int32_t i;
		for (i = 0; i < lEntry->mLength; ++i) {
			(*pEnv)->SetObjectArrayElement(pEnv, lJavaArray, i,
					lEntry->mValue.mColorArray[i]);
			if ((*pEnv)->ExceptionCheck(pEnv)) {
				return NULL;
			}
		}
		return lJavaArray;
	} else {
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_com_packtpub_Store_setColorArray(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jobjectArray pColorArray) {
	jsize lLength = (*pEnv)->GetArrayLength(pEnv, pColorArray);
	jobject* lArray = (jobject*) malloc(lLength * sizeof(jobject));
	int32_t i, j;
	for (i = 0; i < lLength; ++i) {
		jobject lLocalColor = (*pEnv)->GetObjectArrayElement(pEnv, pColorArray,
				i);
		if (lLocalColor == NULL) {
			for (j = 0; j < i; ++j) {
				(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
			}
			free(lArray);
			return;
		}
		lArray[i] = (*pEnv)->NewGlobalRef(pEnv, lLocalColor);
		if (lArray[i] == NULL) {
			for (j = 0; j < i; ++j) {
				(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
			}
			free(lArray);
			return;
		}
		(*pEnv)->DeleteLocalRef(pEnv, lLocalColor);
	}
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_ColorArray;
		lEntry->mLength = lLength;
		lEntry->mValue.mColorArray = lArray;
	} else {
		for (j = 0; j < i; ++j) {
			(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
		}
		free(lArray);
		return;
	}
}

JNIEXPORT jobjectArray JNICALL Java_com_packtpub_Store_getStringArray(
		JNIEnv* pEnv, jobject pThis, jstring pKey) {
	StoreEntry* lEntry = findEntry(pEnv, &gStore, pKey, NULL);
	if (isEntryValid(pEnv, lEntry, StoreType_StringArray)) {
		jclass lStringClass = (*pEnv)->FindClass(pEnv, "java/lang/String");
		if (lStringClass == NULL) {
			return NULL;
		}
		jobjectArray lJavaArray = (*pEnv)->NewObjectArray(pEnv, lEntry->mLength,
				lStringClass, NULL);
		(*pEnv)->DeleteLocalRef(pEnv, lStringClass);
		if (lJavaArray == NULL) {
			return NULL;
		}
		int32_t i;
		for (i = 0; i < lEntry->mLength; ++i) {
			(*pEnv)->SetObjectArrayElement(pEnv, lJavaArray, i,
					lEntry->mValue.mColorArray[i]);
			if ((*pEnv)->ExceptionCheck(pEnv)) {
				return NULL;
			}
		}
		return lJavaArray;
	} else {
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_com_packtpub_Store_setStringArray(JNIEnv* pEnv,
		jobject pThis, jstring pKey, jobjectArray pStringArray) {
	jsize lLength = (*pEnv)->GetArrayLength(pEnv, pStringArray);
	jobject* lArray = (jobject*) malloc(lLength * sizeof(jobject));
	int32_t i, j;
	for (i = 0; i < lLength; ++i) {
		jobject temp = (*pEnv)->GetObjectArrayElement(pEnv, pStringArray, i);
		if (temp == NULL) {
			for (j = 0; j < i; ++j) {
				(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
			}
			free(lArray);
			return;
		}
		lArray[i] = (*pEnv)->NewGlobalRef(pEnv, temp);
		if (lArray[i] == NULL) {
			for (j = 0; j < i; ++j) {
				(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
			}
			free(lArray);
			return;
		}
		(*pEnv)->DeleteLocalRef(pEnv, temp);
	}
	StoreEntry* lEntry = allocateEntry(pEnv, &gStore, pKey);
	if (lEntry != NULL) {
		lEntry->mType = StoreType_StringArray;
		lEntry->mLength = lLength;
		lEntry->mValue.mStringArray = lArray;
	} else {
		for (j = 0; j < i; ++j) {
			(*pEnv)->DeleteGlobalRef(pEnv, lArray[j]);
		}
		free(lArray);
		return;
	}
}
