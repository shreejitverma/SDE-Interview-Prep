/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package events;

import models.Record;

public class Load<K, V> extends Event<K, V> {

    public Load(Record<K, V> element, long timestamp) {
        super(element, timestamp);
    }
}
