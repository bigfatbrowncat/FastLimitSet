#include <jni.h>

#include "FastLimitSet.h"

extern "C" {
	JNIEXPORT void JNICALL Java_bfbc_toys_fastlimitset_NativeLimitSetCalculator_doSteps(JNIEnv* env, jclass clz, jint width, jint height, jintArray counter, jdoubleArray x, jdoubleArray y, jint steps) {
		jint *counterBody = env->GetIntArrayElements(counter, 0);
		jdouble *xBody = env->GetDoubleArrayElements(x, 0);
		jdouble *yBody = env->GetDoubleArrayElements(y, 0);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				double x = xBody[j * width + i];
				double y = yBody[j * width + i];
				int counterVal = counterBody[j * width + i];

				double x0 = (4.0 / width) * i - 2;
				double y0 = (4.0 / height) * j - 2;

				for (int step = 0; step < steps; step++) {
					if (x*x + y*y < 1000000.0) {
						double x1 = x*x - y*y + x0;
						double y1 = 2*x*y + y0;
						x = x1; y = y1;
						counterVal ++;
					} else {
						break;
					}
				}

				xBody[j * width + i] = x;
				yBody[j * width + i] = y;
				counterBody[j * width + i] = counterVal;
			}
		}

		env->ReleaseIntArrayElements(counter, counterBody, 0);
		env->ReleaseDoubleArrayElements(x, xBody, 0);
		env->ReleaseDoubleArrayElements(y, yBody, 0);

	}
}
