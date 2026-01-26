/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

package com.scaler.lld.parkinglot.strategies;

import com.scaler.lld.parkinglot.models.Ticket;

public interface FeesStrategy {
    int calculateFees(Ticket ticket);
}
