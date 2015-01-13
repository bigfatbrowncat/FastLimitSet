package bfbc.toys.fastlimitset;

public class NativeLimitSetCalculator {
	static {
	    System.loadLibrary("FastLimitSet");
	    
	}
	
	public static native boolean doStepsExpression(String expression, int width, int height, int[] counter, double[] x, double[] y, int steps);
	public static native void doStepsMandelbrot(int width, int height, int[] counter, double[] x, double[] y, int steps);
	
}
