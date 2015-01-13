package bfbc.toys.fastlimitset;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Matrix;
import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.ResultReceiver;
import android.view.Display;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;


/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 *
 * @see SystemUiHider
 */
public class PictureActivity extends Activity {

	//private PainterService painterService;
    /**
     * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
     * user interaction before hiding the system UI.
     */
    private static final int AUTO_HIDE_DELAY_MILLIS = 3000;

    private int scale = 32;
    private int newScale = 32;
    private int screenWidth, screenHeight;
    private int width, height;
    private Bitmap picture;
    private Matrix mat = new Matrix();
	
    private ImageView pictureImageView;
    
	private class LimitSetIsBuiltResultReceiver extends ResultReceiver {
		public LimitSetIsBuiltResultReceiver(Handler handler) {
			super(handler);
		}

		@Override
		protected void onReceiveResult(int resultCode, Bundle resultData) {
			int pictureWidth = resultData.getInt(LimitSetCalculatorService.RESULT_BUNDLE_PICTURE_WIDTH);
			int pictureHeight = resultData.getInt(LimitSetCalculatorService.RESULT_BUNDLE_PICTURE_HEIGHT);
			int[] counter = resultData.getIntArray(LimitSetCalculatorService.RESULT_BUNDLE_COUNTER);
			int steps = resultData.getInt(LimitSetCalculatorService.RESULT_BUNDLE_STEPS);
			boolean success = resultData.getBoolean(LimitSetCalculatorService.RESULT_BUNDLE_SUCCESS);
			
			if (success) {
				scale = newScale;
	
	        	picture = Bitmap.createBitmap(pictureWidth, pictureHeight, Config.ARGB_8888);
				
		    	// Filling in pixels
		    	int[] pixels = new int[pictureWidth * pictureHeight];
		    	for (int i = 0; i < pictureWidth; i++) {
					for (int j = 0; j < pictureHeight; j++) {
						byte b = (byte)((double)counter[j * pictureWidth + i] / steps * 0xFF);
						byte g = (byte)((double)counter[j * pictureWidth + i] / steps * 0xFF);
						byte r = (byte)((double)counter[j * pictureWidth + i] / steps * 0xFF);
						pixels[j * pictureWidth + i] = 0xff000000 + b + g * 0x100 + r * 0x10000;
					}
				}
		    	picture.setPixels(pixels, 0, pictureWidth, 0, 0, pictureWidth, pictureHeight);
		    	
		    	pictureImageView.setImageBitmap(picture);
		    	updatePictureMatrix();
		    	
		    	if (scale > 2) {
		    		newScale = scale / 2;
		            LimitSetCalculatorService.start(PictureActivity.this, width / newScale, height / newScale, new LimitSetIsBuiltResultReceiver(new Handler()));
	
		    		//PainterService.proceed(PictureActivity.this);
		    	}
			} else {
				throw new RuntimeException("No success from native code");
			}
		}
	}
	
	@SuppressLint("NewApi")
	void updatePictureMatrix() {
    	Point size = new Point();
    	Display display = getWindowManager().getDefaultDisplay();
		if (Build.VERSION.SDK_INT >= 17) {
    	    display.getRealSize(size);
    	    screenWidth = size.x;
    	    screenHeight = size.y;
		} else {
    	    screenWidth = display.getWidth();
    	    screenHeight = display.getHeight();
		}

		height = Math.max(screenWidth, screenHeight);
    	width = height;

		float deltaY;
		if (screenHeight < screenWidth) {
			deltaY = (screenHeight - screenWidth) / 2;
		} else {
			deltaY = 0;
		}
		
    	mat.setScale(scale, scale);
    	mat.postTranslate(0, deltaY);
    	pictureImageView.setImageMatrix(mat);
	}
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_picture);

        pictureImageView = (ImageView) findViewById(R.id.picture_imageView);

        View decorView = getWindow().getDecorView();

        decorView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
			
			@Override
			public void onSystemUiVisibilityChange(int visibility) {
				if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
		            getActionBar().show();
				} else {
		            getActionBar().hide();
				}

				delayedHide();
			}
		});

        
        // Set up the user interaction to manually show or hide the system UI.
        pictureImageView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
            	delayedHide();
            }
        });
        
        updatePictureMatrix();
        LimitSetCalculatorService.start(this, width / scale, height / scale, new LimitSetIsBuiltResultReceiver(new Handler()));
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.activity_picture, menu);
    	return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	switch (item.getItemId()) {
    	case R.id.test:
    		//getActionBar().setTitle(String.valueOf(NativeLimitSetCalculator.testFormula("return 1 + 2 + 3")));
    	}
    	return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        // Trigger the initial hide() shortly after the activity has been
        // created, to briefly hint to the user that UI controls
        // are available.
        delayedHide();
    }

    Handler mHideHandler = new Handler();
    Runnable mHideRunnable = new Runnable() {
        @Override
        public void run() {
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN);
        }
    };

    /**
     * Schedules a call to hide() in [delay] milliseconds, canceling any
     * previously scheduled calls.
     */
    private void delayedHide() {
        mHideHandler.removeCallbacks(mHideRunnable);
        mHideHandler.postDelayed(mHideRunnable, AUTO_HIDE_DELAY_MILLIS);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
    	updatePictureMatrix();
    	super.onConfigurationChanged(newConfig);
    }
}
