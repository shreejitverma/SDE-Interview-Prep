/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.design.behavioural.observer;

public class EmailNotifier implements Observer {

    @Override
    public void notifyChange() {
        System.out.println("Send email");        
    }
    
}
