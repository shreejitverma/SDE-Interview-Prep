/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.design.creational.factory.game;

import com.scaler.lld.design.creational.prototype.game.BackgroundObject;

public class BackgroundGameObjectFactory implements GameObjectFactory {

    @Override
    public BackgroundObject createGameObject() {
        return new BackgroundObject();
    }
}