package bfbc.toys.fastlimitset;

public class NativeLimitSetCalculator {
	static {
	    System.loadLibrary("FastLimitSet");
	}
	
	public static native void doSteps(int width, int height, int[] counter, double[] x, double[] y, int steps);
}
