package com.packtpub;

import java.util.Arrays;
import java.util.List;

import com.google.common.base.Function;
import com.google.common.base.Joiner;
import com.google.common.collect.Lists;
import com.google.common.primitives.Ints;
import com.packtpub.exception.InvalidTypeException;
import com.packtpub.exception.NotExistingKeyException;
import com.packtpub.exception.StoreFullException;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

public class StoreActivity extends Activity implements StoreListener {
	private EditText mUIKeyEdit, mUIValueEdit;
	private Spinner mUITypeSpinner;
	private Button mUIGetButton, mUISetButton;
	private Store mStore;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_store);
		// Initializes components and binds buttons to handlers.
		mUIKeyEdit = (EditText) findViewById(R.id.uiKeyEdit);
		mUIValueEdit = (EditText) findViewById(R.id.uiValueEdit);
		mUITypeSpinner = (Spinner) findViewById(R.id.uiTypespinner);
		StoreType[] items = {StoreType.Integer, StoreType.String, StoreType.Color,
				StoreType.IntegerArray, StoreType.ColorArray, StoreType.StringArray};
		ArrayAdapter<StoreType> adapter = new ArrayAdapter<StoreType>(this, android.R.layout.simple_spinner_item, items);
		mUITypeSpinner.setAdapter(adapter);
		mUIGetButton = (Button) findViewById(R.id.uiGetButton);
		mUIGetButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				onGetValue();
			}
		});
		mUISetButton = (Button) findViewById(R.id.uiSetButton);
		mUISetButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				onSetValue();
			}
		});
		mStore = new Store(this);
	}
	
	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();
		mStore.initializeStore();
		mStore.setInteger("watcherCounter", 0);
	}
	
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
		mStore.finalizeStore();
	}
	
	private void onGetValue() {
		String lKey = mUIKeyEdit.getText().toString();
		StoreType lType = (StoreType) mUITypeSpinner.getSelectedItem();
		try {
			switch (lType) {
			case Integer:
				mUIValueEdit.setText(Integer.toString(mStore.getInteger(lKey)));
				break;
			case String:
				mUIValueEdit.setText(mStore.getString(lKey));
				break;
			case Color:
				mUIValueEdit.setText(mStore.getColor(lKey).toString());
				break;
			case IntegerArray:
				mUIValueEdit.setText(Ints.join(";",
						mStore.getIntegerArray(lKey)));
				break;
			case ColorArray:
				mUIValueEdit.setText(Joiner.on(";").join(
						mStore.getColorArray(lKey)));
				break;
			case StringArray:
				mUIValueEdit.setText(Joiner.on(";").join(
						mStore.getStringArray(lKey)));
			default:
				break;
			}
		} catch (NotExistingKeyException eNotExistingKeyException) {
			displayError("Key does not exist in store");
		} catch (InvalidTypeException eInvalidTypeException) {
			displayError("Incorrect type.");
		}

	}
	
	private void onSetValue() {
		String lKey = mUIKeyEdit.getText().toString();
		String lValue = mUIValueEdit.getText().toString();
		StoreType lType = (StoreType) mUITypeSpinner.getSelectedItem();
		try {
			switch (lType) {
			case Integer:
				mStore.setInteger(lKey, Integer.parseInt(lValue));
				break;
			case String:
				mStore.setString(lKey, lValue);
				break;
			case Color:
				mStore.setColor(lKey, new Color(lValue));
				break;
			case IntegerArray:
				mStore.setIntegerArray(lKey, Ints.toArray(stringToList(
						new Function<String, Integer>() {
							public Integer apply(String pSubValue) {
								return Integer.parseInt(pSubValue);
							}
						}, lValue)));
				break;
			case ColorArray:
				List<Color> lIdList = stringToList(
						new Function<String, Color>() {
							public Color apply(String pSubValue) {
								return new Color(pSubValue);
							}
						}, lValue);
				Color[] lIdArray = lIdList.toArray(new Color[lIdList.size()]);
				mStore.setColorArray(lKey, lIdArray);
				break;
			case StringArray:
				List<String> list = stringToList(
						new Function<String, String>() {
							public String apply(String pSubValue) {
								return pSubValue;
							}
						}, lValue);
				String[] lStrings = list.toArray(new String[list.size()]);
				mStore.setStringArray(lKey, lStrings);
			default:
				break;
			}
		} catch (NumberFormatException eNumberFormatException) {
			displayError("Incorrect value.");
		} catch (IllegalArgumentException eIllegalArgumentException) {
			displayError("Incorrect value.");
		} catch (StoreFullException eStoreFullException) {
			displayError("Store is full.");
		}
	}
	
	private <TType> List<TType> stringToList(
			Function<String, TType> pConversion, String pValue) {
		String[] lSplitArray = pValue.split(";");
		List<String> lSplitList = Arrays.asList(lSplitArray);
		return Lists.transform(lSplitList, pConversion);
	}

	private void displayError(String pError) {
		Toast.makeText(getApplicationContext(), pError, Toast.LENGTH_LONG)
				.show();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.store, menu);
		return true;
	}

	@Override
	public void onAlert(int pValue) {
		// TODO Auto-generated method stub
		displayError(String.format("%1$d is not an allowed integer", pValue));
	}

	@Override
	public void onAlert(String pValue) {
		// TODO Auto-generated method stub
		displayError(String.format("%1$s is not an allowed string", pValue));
	}

	@Override
	public void onAlert(Color pValue) {
		// TODO Auto-generated method stub
		displayError(String.format("%1$s is not an allowed color", pValue.toString()));
	}

}
