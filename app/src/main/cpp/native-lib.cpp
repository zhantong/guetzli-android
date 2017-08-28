#include <jni.h>
#include "guetzli/guetzli/guetzli.cc"

extern "C"
JNIEXPORT jint JNICALL
Java_com_ploarxiong_guetzli_MainActivity_compressImage(JNIEnv *env, jobject instance,
                                                       jstring inputImagePath_,
                                                       jstring outputImagePath_,
                                                       jint mode_) {
    const char *inputImagePath = env->GetStringUTFChars(inputImagePath_, 0);
    const char *outputImagePath = env->GetStringUTFChars(outputImagePath_, 0);
    const int mode = mode_;

    compressImage(inputImagePath, outputImagePath, mode);

    env->ReleaseStringUTFChars(inputImagePath_, inputImagePath);
    env->ReleaseStringUTFChars(outputImagePath_, outputImagePath);
    return 0;
}
