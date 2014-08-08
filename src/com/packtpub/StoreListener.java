package com.packtpub;

public interface StoreListener {

	public void onAlert(int pValue);
	
	public void onAlert(String pValue);
	
	public void onAlert(Color pValue);
}
