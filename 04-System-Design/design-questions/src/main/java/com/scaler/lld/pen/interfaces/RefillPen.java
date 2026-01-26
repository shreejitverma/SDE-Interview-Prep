/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.pen.interfaces;

import com.scaler.lld.pen.models.Refill;

public interface RefillPen {

    public void changeRefill(Refill refill);

    public Boolean canRefill();

    public Refill getRefill();
}
