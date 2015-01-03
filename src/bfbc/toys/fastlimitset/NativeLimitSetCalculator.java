package bfbc.toys.fastlimitset;

public class NativeLimitSetCalculator {
	static {
	    System.loadLibrary("FastLimitSet");
	}
	
	public static native void doStepsExpression(String xExpression, String yExpression, int width, int height, int[] counter, double[] x, double[] y, int steps);
//	public static native double testFormula(String expression);
}
