/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.design.creational.prototype.game;

import lombok.NoArgsConstructor;

@NoArgsConstructor
public class ForegroundObject implements GraphicalObject {

    @Override
    public ForegroundObject clone() {
        return new ForegroundObject();
    }
    
}
