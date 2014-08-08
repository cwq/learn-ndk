package com.packtpub;

public class Color {

	private int mColor;

	public Color(String pColor) {
		super();
		mColor = android.graphics.Color.parseColor(pColor);
	}

	@Override
	public String toString() {
		return String.format("#%06X", mColor);
	}
	
	@Override
	public int hashCode() {
		// TODO Auto-generated method stub
		return mColor;
	}
	
	@Override
	public boolean equals(Object o) {
		// TODO Auto-generated method stub
		if (this == o) return true;
		if (o == null) return false;
		if (getClass() != o.getClass()) return false;
		Color pColor = (Color) o;
		return (mColor == pColor.mColor);
	}

}
