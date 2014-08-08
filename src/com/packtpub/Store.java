package com.packtpub;

import com.packtpub.exception.InvalidTypeException;
import com.packtpub.exception.NotExistingKeyException;

public class Store {

	static {
		System.loadLibrary("store");
	}
	
	public native void initializeStore();
	
	public native void finalizeStore();

	public native int getInteger(String pKey) throws NotExistingKeyException,
			InvalidTypeException;

	public native void setInteger(String pKey, int pInt);

	public native String getString(String pKey) throws NotExistingKeyException,
			InvalidTypeException;

	public native void setString(String pKey, String pString);

	public native Color getColor(String pKey) throws NotExistingKeyException,
			InvalidTypeException;

	public native void setColor(String pKey, Color pColor);

	public native int[] getIntegerArray(String pKey)
			throws NotExistingKeyException, InvalidTypeException;

	public native void setIntegerArray(String pKey, int[] pIntArray);

	public native Color[] getColorArray(String pKey)
			throws NotExistingKeyException, InvalidTypeException;

	public native void setColorArray(String pKey, Color[] pColorArray);

	public native String[] getStringArray(String pKey)
			throws NotExistingKeyException, InvalidTypeException;

	public native void setStringArray(String pKey, String[] pIntArray);
}