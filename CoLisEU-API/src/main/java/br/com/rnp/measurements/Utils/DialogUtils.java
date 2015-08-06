package br.com.rnp.measurements.Utils;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;

public class DialogUtils {

	public static AlertDialog makeDialog(String title, String message, Context myContext) {
		AlertDialog.Builder builder = new AlertDialog.Builder(myContext);
		builder.setMessage(message);
		builder.setTitle(title);
		builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub

				dialog.dismiss();
			}
		});
		// Create the AlertDialog
		AlertDialog dialog = builder.create();
		dialog.show();
		return dialog;
	}
	
}
