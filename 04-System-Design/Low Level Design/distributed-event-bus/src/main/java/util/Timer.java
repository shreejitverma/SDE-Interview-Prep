/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package util;

import com.google.inject.Singleton;

@Singleton
public class Timer {
    public long getCurrentTime() {
        return System.nanoTime();
    }
}
