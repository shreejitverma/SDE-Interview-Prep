/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.design.creational.factory.database;

public class OracleDriver extends NewDatabaseDriver {

    @Override
    public void connect() {
        System.out.println("Connecting to Oracle Database");        
    }

    @Override
    public NewDatabaseDriver createDriver() {
        return new OracleDriver();
    }
    
}
