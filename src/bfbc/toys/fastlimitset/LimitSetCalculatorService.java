package bfbc.toys.fastlimitset;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.ResultReceiver;

public class LimitSetCalculatorService extends Service {

	private static final String EXTRA_BUNDLE_CONTINUE = "continue";
	private static final String EXTRA_BUNDLE_PICTURE_WIDTH = "picture_width";
	private static final String EXTRA_BUNDLE_PICTURE_HEIGHT = "picture_height";
	private static final String EXTRA_BUNDLE_RESULT_RECEIVER = "result_receiver";
	
	public static final String RESULT_BUNDLE_COUNTER = "counter";
	public static final String RESULT_BUNDLE_PICTURE_WIDTH = "width";
	public static final String RESULT_BUNDLE_PICTURE_HEIGHT = "height";
	public static final String RESULT_BUNDLE_STEPS = "steps";
	
	public static void start(Context context, int pictureWidth, int pictureHeight, ResultReceiver resultReceiver) {
		Intent intent = new Intent(context, LimitSetCalculatorService.class);
		intent.putExtra(EXTRA_BUNDLE_PICTURE_WIDTH, pictureWidth);
		intent.putExtra(EXTRA_BUNDLE_PICTURE_HEIGHT, pictureHeight);
		intent.putExtra(EXTRA_BUNDLE_RESULT_RECEIVER, resultReceiver);
		
		context.startService(intent);
	}
	
	public static void proceed(Context context) {
		Intent intent = new Intent(context, LimitSetCalculatorService.class);
		intent.putExtra(EXTRA_BUNDLE_CONTINUE, true);
		context.startService(intent);
	}
	
    private int[] counter;
    private double[] x;
    private double[] y;
    private int pictureWidth, pictureHeight;
    //private int scale;
    private int steps;

    private ResultReceiver resultReceiver;
    
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Boolean proceed = intent.getExtras().getBoolean(EXTRA_BUNDLE_CONTINUE);
		if (proceed == null || !proceed) {
			pictureWidth = intent.getExtras().getInt(EXTRA_BUNDLE_PICTURE_WIDTH);
			pictureHeight = intent.getExtras().getInt(EXTRA_BUNDLE_PICTURE_HEIGHT);
			resultReceiver = (ResultReceiver) intent.getExtras().get(EXTRA_BUNDLE_RESULT_RECEIVER);
			
	    	counter = new int[pictureWidth * pictureHeight];
	    	x = new double[pictureWidth * pictureHeight];
			y = new double[pictureWidth * pictureHeight];
			
			steps = 0;
		}
    	(new CountingTask()).execute();
    	
		return super.onStartCommand(intent, flags, startId);
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	
    private class CountingTask extends AsyncTask<Void, Void, Integer> {
		@Override
		protected Integer doInBackground(Void... params) {
	    	// Calculating the limit set
			int delta = 50;
			steps += delta;
			NativeLimitSetCalculator.doSteps(pictureWidth, pictureHeight, counter, x, y, delta);
			
			Bundle resultData = new Bundle();
			resultData.putInt(RESULT_BUNDLE_PICTURE_WIDTH, pictureWidth);
			resultData.putInt(RESULT_BUNDLE_PICTURE_HEIGHT, pictureHeight);
			resultData.putInt(RESULT_BUNDLE_STEPS, steps);
			resultData.putIntArray(RESULT_BUNDLE_COUNTER, counter);
			
			resultReceiver.send(0, resultData);
			return 0;
		}
    }
    

}
